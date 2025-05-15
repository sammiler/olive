

#include "volume.h"

#include "widget/slider/floatslider.h"

namespace olive {

const QString VolumeNode::kSamplesInput = QStringLiteral("samples_in");
const QString VolumeNode::kVolumeInput = QStringLiteral("volume_in");

#define super MathNodeBase

VolumeNode::VolumeNode() {
  AddInput(kSamplesInput, NodeValue::kSamples, InputFlags(kInputFlagNotKeyframable));

  AddInput(kVolumeInput, NodeValue::kFloat, 1.0);
  SetInputProperty(kVolumeInput, QStringLiteral("min"), 0.0);
  SetInputProperty(kVolumeInput, QStringLiteral("view"), FloatSlider::kDecibel);

  SetFlag(kAudioEffect);
  SetEffectInput(kSamplesInput);
}

QString VolumeNode::Name() const { return tr("Volume"); }

QString VolumeNode::id() const { return QStringLiteral("org.olivevideoeditor.Olive.volume"); }

QVector<Node::CategoryID> VolumeNode::Category() const { return {kCategoryFilter}; }

QString VolumeNode::Description() const { return tr("Adjusts the volume of an audio source."); }

void VolumeNode::Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const {
  // Create a sample job
  SampleBuffer buffer = value[kSamplesInput].toSamples();

  if (buffer.is_allocated()) {
    // If the input is static, we can just do it now which will be faster
    if (IsInputStatic(kVolumeInput)) {
      auto volume = value[kVolumeInput].toDouble();

      if (!qFuzzyCompare(volume, 1.0)) {
        buffer.transform_volume(volume);
      }

      table->Push(NodeValue::kSamples, QVariant::fromValue(buffer), this);
    } else {
      // Requires job
      SampleJob job(globals.time(), kSamplesInput, value);
      job.Insert(kVolumeInput, value);
      table->Push(NodeValue::kSamples, QVariant::fromValue(job), this);
    }
  }
}

void VolumeNode::ProcessSamples(const NodeValueRow &values, const SampleBuffer &input, SampleBuffer &output,
                                int index) const {
  return ProcessSamplesInternal(values, kOpMultiply, kSamplesInput, kVolumeInput, input, output, index);
}

void VolumeNode::Retranslate() {
  super::Retranslate();

  SetInputName(kSamplesInput, tr("Samples"));
  SetInputName(kVolumeInput, tr("Volume"));
}

}  // namespace olive
