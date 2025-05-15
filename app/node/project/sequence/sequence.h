#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "node/output/track/tracklist.h"
#include "node/output/viewer/viewer.h"

namespace olive {

/**
 * @brief The main timeline object, an graph of edited clips that forms a complete edit
 */
class Sequence : public ViewerOutput {
  Q_OBJECT
 public:
  Sequence();

  NODE_DEFAULT_FUNCTIONS(Sequence)

  [[nodiscard]] QString Name() const override { return tr("Sequence"); }

  [[nodiscard]] QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.sequence"); }

  [[nodiscard]] QVector<CategoryID> Category() const override { return {kCategoryProject}; }

  [[nodiscard]] QString Description() const override {
    return tr("A series of cuts that result in an edited video. Also called a timeline.");
  }

  void add_default_nodes(MultiUndoCommand *command = nullptr) const;

  [[nodiscard]] QVariant data(const DataType &d) const override;

  [[nodiscard]] const QVector<Track *> &GetTracks() const { return track_cache_; }

  [[nodiscard]] Track *GetTrackFromReference(const Track::Reference &track_ref) const {
    return track_lists_.at(track_ref.type())->GetTrackAt(track_ref.index());
  }

  /**
   * @brief Same as GetTracks() but omits tracks that are locked.
   */
  [[nodiscard]] QVector<Track *> GetUnlockedTracks() const;

  [[nodiscard]] TrackList *track_list(Track::Type type) const { return track_lists_.at(type); }

  void Retranslate() override;

  void InvalidateCache(const TimeRange &range, const QString &from, int element,
                       InvalidateCacheOptions options) override;

  static const QString kTrackInputFormat;

 protected:
  void InputConnectedEvent(const QString &input, int element, Node *output) override;

  void InputDisconnectedEvent(const QString &input, int element, Node *output) override;

  [[nodiscard]] rational VerifyLengthInternal(Track::Type type) const override;

 signals:
  void TrackAdded(Track *track);
  void TrackRemoved(Track *track);

  void SubtitlesChanged(const TimeRange &range);

 private:
  QVector<TrackList *> track_lists_;

  QVector<Track *> track_cache_;

 private slots:
  void UpdateTrackCache();
};

}  // namespace olive

#endif  // SEQUENCE_H
