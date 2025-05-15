#include "crossdissolvetransition.h"

namespace olive {

CrossDissolveTransition::CrossDissolveTransition() = default;

QString CrossDissolveTransition::Name() const { return tr("Cross Dissolve"); }

QString CrossDissolveTransition::id() const { return QStringLiteral("org.olivevideoeditor.Olive.crossdissolve"); }

QVector<Node::CategoryID> CrossDissolveTransition::Category() const { return {kCategoryTransition}; }

QString CrossDissolveTransition::Description() const { return tr("Smoothly transition between two clips."); }

ShaderCode CrossDissolveTransition::GetShaderCode(const ShaderRequest &request) const {
  Q_UNUSED(request)

  return ShaderCode(FileFunctions::ReadFileAsString(":/shaders/crossdissolve.frag"), QString());
}

void CrossDissolveTransition::SampleJobEvent(const SampleBuffer &from_samples, const SampleBuffer &to_samples,
                                             SampleBuffer &out_samples, double time_in) const {
  for (size_t i = 0; i < out_samples.sample_count(); i++) {
    double this_sample_time = out_samples.audio_params().samples_to_time(i).toDouble() + time_in;
    double progress = GetTotalProgress(this_sample_time);

    for (int j = 0; j < out_samples.audio_params().channel_count(); j++) {
      out_samples.data(j)[i] = 0;

      if (from_samples.is_allocated()) {
        if (i < from_samples.sample_count()) {
          out_samples.data(j)[i] += from_samples.data(j)[i] * TransformCurve(1.0 - progress);
        }
      }

      if (to_samples.is_allocated()) {
        // Offset input samples from the end
        size_t remain = (out_samples.sample_count() - to_samples.sample_count());
        if (i >= remain) {
          qint64 in_index = i - remain;
          out_samples.data(j)[i] += to_samples.data(j)[in_index] * TransformCurve(progress);
        }
      }
    }
  }
}

}  // namespace olive
