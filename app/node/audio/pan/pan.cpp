#include "pan.h"

#include "widget/slider/floatslider.h"

namespace olive {

const QString PanNode::kSamplesInput = QStringLiteral("samples_in");
const QString PanNode::kPanningInput = QStringLiteral("panning_in");

#define super Node

PanNode::PanNode() {
  AddInput(kSamplesInput, NodeValue::kSamples, InputFlags(kInputFlagNotKeyframable));

  AddInput(kPanningInput, NodeValue::kFloat, 0.0);
  SetInputProperty(kPanningInput, QStringLiteral("min"), -1.0);
  SetInputProperty(kPanningInput, QStringLiteral("max"), 1.0);
  SetInputProperty(kPanningInput, QStringLiteral("view"), FloatSlider::kPercentage);

  SetFlag(kAudioEffect);
  SetEffectInput(kSamplesInput);
}

QString PanNode::Name() const { return tr("Pan"); }

QString PanNode::id() const { return QStringLiteral("org.olivevideoeditor.Olive.pan"); }

QVector<Node::CategoryID> PanNode::Category() const { return {kCategoryFilter}; }

QString PanNode::Description() const { return tr("Adjust the stereo panning of an audio source."); }

void PanNode::Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const {
  // Create a sample job
  SampleBuffer samples = value[kSamplesInput].toSamples();
  if (samples.is_allocated()) {
    // This node is only compatible with stereo audio
    if (samples.audio_params().channel_count() == 2) {
      // If the input is static, we can just do it now which will be faster
      if (IsInputStatic(kPanningInput)) {
        float pan_volume = value[kPanningInput].toDouble();
        if (!qIsNull(pan_volume)) {
          if (pan_volume > 0) {
            samples.transform_volume_for_channel(0, 1.0f - pan_volume);
          } else {
            samples.transform_volume_for_channel(1, 1.0f + pan_volume);
          }
        }

        table->Push(NodeValue(NodeValue::kSamples, samples, this));
      } else {
        // Requires job
        table->Push(NodeValue::kSamples, SampleJob(globals.time(), kSamplesInput, value), this);
      }
    } else {
      // Pass right through
      table->Push(value[kSamplesInput]);
    }
  }
}

void PanNode::ProcessSamples(const NodeValueRow &values, const SampleBuffer &input, SampleBuffer &output,
                             int index) const {
  float pan_val = values[kPanningInput].toDouble();

  for (int i = 0; i < input.audio_params().channel_count(); i++) {
    output.data(i)[index] = input.data(i)[index];
  }

  if (pan_val > 0) {
    output.data(0)[index] *= (1.0F - pan_val);
  } else if (pan_val < 0) {
    output.data(1)[index] *= (1.0F - qAbs(pan_val));
  }
}

void PanNode::Retranslate() {
  super::Retranslate();

  SetInputName(kSamplesInput, tr("Samples"));
  SetInputName(kPanningInput, tr("Pan"));
}

}  // namespace olive
