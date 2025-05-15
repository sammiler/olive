#ifndef EXPORTTASK_H
#define EXPORTTASK_H

#include "codec/encoder.h"
#include "node/output/viewer/viewer.h"
#include "render/colorprocessor.h"
#include "render/projectcopier.h"
#include "task/render/render.h"
#include "task/task.h"

namespace olive {

class ExportTask : public RenderTask {
  Q_OBJECT
 public:
  ExportTask(ViewerOutput *viewer_node, ColorManager *color_manager, const EncodingParams &params);

 protected:
  bool Run() override;

  bool FrameDownloaded(FramePtr frame, const rational &time) override;

  bool AudioDownloaded(const TimeRange &range, const SampleBuffer &samples) override;

  bool EncodeSubtitle(const SubtitleBlock *sub) override;

  [[nodiscard]] bool TwoStepFrameRendering() const override { return false; }

 private:
  bool WriteAudioLoop(const TimeRange &time, const SampleBuffer &samples);

  ProjectCopier *copier_;

  QHash<rational, FramePtr> time_map_;

  QHash<TimeRange, SampleBuffer> audio_map_;

  ColorManager *color_manager_;

  EncodingParams params_;

  std::shared_ptr<Encoder> encoder_;

  std::shared_ptr<Encoder> subtitle_encoder_;

  ColorProcessorPtr color_processor_;

  int64_t frame_time_{};

  rational audio_time_;

  TimeRange export_range_;
};

}  // namespace olive

#endif  // EXPORTTASK_H
