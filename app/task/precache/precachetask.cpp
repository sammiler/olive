#include "precachetask.h"

#include "node/project.h"

namespace olive {

PreCacheTask::PreCacheTask(Footage *footage, int index, Sequence *sequence) {
  // Set video and audio params
  set_video_params(sequence->GetVideoParams());
  set_audio_params(sequence->GetAudioParams());

  // Create new project
  project_ = new Project();

  // Create viewer with same parameters as the sequence
  set_viewer(new ViewerOutput());
  viewer()->setParent(project_);
  viewer()->SetVideoParams(sequence->GetVideoParams());
  viewer()->SetAudioParams(sequence->GetAudioParams());

  // Copy project config nodes
  Project::CopySettings(footage->project(), project_);

  // Copy footage node so it can precache without any modifications from the user screwing it up
  footage_ = dynamic_cast<Footage *>(footage->copy());
  footage_->setParent(project_);
  Node::CopyInputs(footage, footage_, false);

  Node::ConnectEdge(footage_, NodeInput(viewer(), ViewerOutput::kTextureInput));
  viewer()->SetValueHintForInput(
      ViewerOutput::kTextureInput,
      Node::ValueHint({NodeValue::kTexture}, Track::Reference(Track::kVideo, index).ToString()));

  SetTitle(tr("Pre-caching %1:%2").arg(footage_->filename(), QString::number(index)));
}

PreCacheTask::~PreCacheTask() {
  // This should delete the footage we copied and the viewer we created
  delete project_;
}

bool PreCacheTask::Run() {
  // Get list of invalidated ranges
  TimeRange intersection;

  if (footage_->GetWorkArea()->enabled()) {
    // If we're caching only in-out, limit the range to that
    intersection = footage_->GetWorkArea()->range();
  } else {
    // Otherwise use full length
    intersection = TimeRange(rational(0), footage_->GetVideoLength());
  }

  TimeRangeList video_range = viewer()->video_frame_cache()->GetInvalidatedRanges(intersection);

  Render(project_->color_manager(), video_range, TimeRangeList(), TimeRange(), RenderMode::kOnline,
         viewer()->video_frame_cache());

  return true;
}

bool PreCacheTask::FrameDownloaded(FramePtr frame, const rational &time) {
  // Do nothing. Pre-cache essentially just creates more frames in the cache, it doesn't need to do
  // anything else.

  Q_UNUSED(frame)
  Q_UNUSED(time)

  return true;
}

bool PreCacheTask::AudioDownloaded(const TimeRange &range, const SampleBuffer &samples) {
  // Pre-cache doesn't cache any audio

  Q_UNUSED(range)
  Q_UNUSED(samples)

  return true;
}

}  // namespace olive
