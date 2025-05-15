#ifndef TRACKLIST_H
#define TRACKLIST_H

#include <QObject>

#include "node/output/track/track.h"
#include "timeline/timelinecommon.h"

namespace olive {

class Sequence;

class TrackList : public QObject {
  Q_OBJECT
 public:
  TrackList(Sequence* parent, const Track::Type& type, QString track_input);

  [[nodiscard]] const Track::Type& type() const { return type_; }

  [[nodiscard]] const QVector<Track*>& GetTracks() const { return track_cache_; }

  [[nodiscard]] Track* GetTrackAt(int index) const;

  [[nodiscard]] const rational& GetTotalLength() const { return total_length_; }

  [[nodiscard]] int GetTrackCount() const { return track_cache_.size(); }

  [[nodiscard]] Project* GetParentGraph() const;

  [[nodiscard]] const QString& track_input() const;
  [[nodiscard]] NodeInput track_input(int element) const;

  [[nodiscard]] Sequence* parent() const;

  [[nodiscard]] int ArraySize() const;

  void ArrayAppend() const;
  void ArrayRemoveLast() const;

  [[nodiscard]] int GetArrayIndexFromCacheIndex(int index) const { return track_array_indexes_.at(index); }

  [[nodiscard]] int GetCacheIndexFromArrayIndex(int index) const { return track_array_indexes_.indexOf(index); }

 public slots:
  /**
   * @brief Slot for when the track connection is added
   */
  void TrackConnected(Node* node, int element);

  /**
   * @brief Slot for when the track connection is removed
   */
  void TrackDisconnected(Node* node, int element);

 signals:
  void TrackListChanged();

  void LengthChanged(const rational& length);

  void TrackAdded(Track* track);

  void TrackRemoved(Track* track);

  void TrackHeightChanged(Track* track, int height);

 private:
  void UpdateTrackIndexesFrom(int index);

  /**
   * @brief A cache of connected Tracks
   */
  QVector<Track*> track_cache_;
  QVector<int> track_array_indexes_;

  QString track_input_;

  rational total_length_;

  enum Track::Type type_;

 private slots:
  /**
   * @brief Slot for when any of the track's length changes so we can update the length of the tracklist
   */
  void UpdateTotalLength();
};

}  // namespace olive

#endif  // TRACKLIST_H
