#include "tracklist.h"

#include <utility>

#include "node/factory.h"
#include "node/math/math/math.h"
#include "node/math/merge/merge.h"
#include "node/output/viewer/viewer.h"
#include "node/project/sequence/sequence.h"

namespace olive {

TrackList::TrackList(Sequence *parent, const Track::Type &type, QString track_input)
    : QObject(parent), track_input_(std::move(track_input)), total_length_(0), type_(type) {}

Track *TrackList::GetTrackAt(int index) const {
  if (index >= 0 && index < track_cache_.size()) {
    return track_cache_.at(index);
  } else {
    return nullptr;
  }
}

void TrackList::TrackConnected(Node *node, int element) {
  if (element == -1) {
    parent()->InvalidateAll(track_input(), element);
    return;
  }

  auto *track = dynamic_cast<Track *>(node);

  if (!track) {
    return;
  }

  // Determine where in the cache this block will be
  int cache_index = -1;
  for (int i = element + 1; i < ArraySize(); i++) {
    // Find next track because this will be the index we insert at
    cache_index = GetCacheIndexFromArrayIndex(i);

    if (cache_index >= 0) {
      break;
    }
  }

  // If there was no next, this will be inserted at the end
  if (cache_index == -1) {
    cache_index = track_cache_.size();
  }

  track_cache_.insert(cache_index, track);
  track_array_indexes_.insert(cache_index, element);

  // Update track indexes in the list (including this track)
  UpdateTrackIndexesFrom(cache_index);

  connect(track, &Track::TrackLengthChanged, this, &TrackList::UpdateTotalLength);
  connect(track, &Track::TrackHeightChanged, this, [this]() {
    auto *t = dynamic_cast<Track *>(sender());
    emit TrackHeightChanged(t, t->GetTrackHeightInPixels());
  });

  track->set_type(type_);
  track->set_sequence(parent());

  emit TrackListChanged();

  // This function must be called after the track is added to track_cache_, since it uses track_cache_ to determine
  // the track's index
  emit TrackAdded(track);

  UpdateTotalLength();
}

void TrackList::TrackDisconnected(Node *node, int element) {
  if (element == -1) {
    // User has replaced the entire array, we will invalidate everything
    parent()->InvalidateAll(track_input(), element);
    return;
  }

  auto *track = dynamic_cast<Track *>(node);

  if (!track) {
    return;
  }

  // Traverse through Tracks uncaching and disconnecting them
  emit TrackRemoved(track);

  int cache_index = GetCacheIndexFromArrayIndex(element);

  // Remove track here
  track_cache_.removeAt(cache_index);
  track_array_indexes_.removeAt(cache_index);

  // Update indices for all subsequent tracks
  UpdateTrackIndexesFrom(cache_index);

  track->SetIndex(-1);
  track->set_type(Track::kNone);
  track->set_sequence(nullptr);

  disconnect(track, &Track::TrackLengthChanged, this, &TrackList::UpdateTotalLength);

  emit TrackListChanged();

  UpdateTotalLength();
}

void TrackList::UpdateTrackIndexesFrom(int index) {
  for (int i = index; i < track_cache_.size(); i++) {
    track_cache_.at(i)->SetIndex(i);
  }
}

Project *TrackList::GetParentGraph() const { return parent()->parent(); }

const QString &TrackList::track_input() const { return track_input_; }

NodeInput TrackList::track_input(int element) const { return NodeInput(parent(), track_input(), element); }

Sequence *TrackList::parent() const { return dynamic_cast<Sequence *>(QObject::parent()); }

int TrackList::ArraySize() const { return parent()->InputArraySize(track_input()); }

void TrackList::ArrayAppend() const { parent()->InputArrayAppend(track_input()); }

void TrackList::ArrayRemoveLast() const { parent()->InputArrayRemoveLast(track_input()); }

void TrackList::UpdateTotalLength() {
  total_length_ = rational(0);

  foreach (Track *track, track_cache_) {
    if (track) {
      total_length_ = qMax(total_length_, track->track_length());
    }
  }

  emit LengthChanged(total_length_);
}

}  // namespace olive
