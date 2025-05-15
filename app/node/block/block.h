#ifndef BLOCK_H
#define BLOCK_H

#include "node/node.h"
#include "timeline/timelinecommon.h"

namespace olive {

class TransitionBlock;

/**
 * @brief A Node that represents a block of time, also displayable on a Timeline
 */
class Block : public Node {
  Q_OBJECT
 public:
  Block();

  [[nodiscard]] QVector<CategoryID> Category() const override;

  [[nodiscard]] const rational& in() const { return in_point_; }

  [[nodiscard]] const rational& out() const { return out_point_; }

  void set_in(const rational& in) { in_point_ = in; }

  void set_out(const rational& out) { out_point_ = out; }

  [[nodiscard]] rational length() const;
  virtual void set_length_and_media_out(const rational& length);
  virtual void set_length_and_media_in(const rational& length);

  [[nodiscard]] TimeRange range() const { return TimeRange(in(), out()); }

  [[nodiscard]] Block* previous() const { return previous_; }

  [[nodiscard]] Block* next() const { return next_; }

  void set_previous(Block* previous) { previous_ = previous; }

  void set_next(Block* next) { next_ = next; }

  [[nodiscard]] Track* track() const { return track_; }

  void set_track(Track* track) {
    track_ = track;
    emit TrackChanged(track_);
  }

  [[nodiscard]] bool is_enabled() const;
  void set_enabled(bool e);

  void Retranslate() override;

  void InvalidateCache(const TimeRange& range, const QString& from, int element = -1,
                       InvalidateCacheOptions options = InvalidateCacheOptions()) override;

  static const QString kLengthInput;

  static void set_previous_next(Block* previous, Block* next);

 public slots:

 signals:
  void EnabledChanged();

  void LengthChanged();

  void PreviewChanged();

  void TrackChanged(Track* track);

 protected:
  void InputValueChangedEvent(const QString& input, int element) override;

  Block* previous_;
  Block* next_;

 private:
  void set_length_internal(const rational& length);

  rational in_point_;
  rational out_point_;
  Track* track_;

  rational last_length_;
};

}  // namespace olive

#endif  // BLOCK_H
