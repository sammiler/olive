#ifndef TRACK_H
#define TRACK_H

#include "node/block/block.h"

namespace olive {

class Sequence;

/**
 * @brief A time traversal Node for sorting through one channel/track of Blocks
 */
class Track : public Node {
  Q_OBJECT
 public:
  enum Type { kNone = -1, kVideo, kAudio, kSubtitle, kCount };

  Track();

  NODE_DEFAULT_FUNCTIONS(Track)

  [[nodiscard]] const Track::Type& type() const;
  void set_type(const Track::Type& track_type);

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  [[nodiscard]] ActiveElements GetActiveElementsAtTime(const QString& input, const TimeRange& r) const override;
  void Value(const NodeValueRow& value, const NodeGlobals& globals, NodeValueTable* table) const override;

  [[nodiscard]] TimeRange InputTimeAdjustment(const QString& input, int element, const TimeRange& input_time,
                                              bool clamp) const override;

  [[nodiscard]] TimeRange OutputTimeAdjustment(const QString& input, int element,
                                               const TimeRange& input_time) const override;

  static rational TransformTimeForBlock(const Block* block, const rational& time) {
    if (time == RATIONAL_MAX || time == RATIONAL_MIN) {
      return time;
    }

    return time - block->in();
  }

  static TimeRange TransformRangeForBlock(const Block* block, const TimeRange& range) {
    return TimeRange(TransformTimeForBlock(block, range.in()), TransformTimeForBlock(block, range.out()));
  }

  static rational TransformTimeFromBlock(const Block* block, const rational& time) {
    if (time == RATIONAL_MAX || time == RATIONAL_MIN) {
      return time;
    }

    return time + block->in();
  }

  static TimeRange TransformRangeFromBlock(const Block* block, const TimeRange& range) {
    return TimeRange(TransformTimeFromBlock(block, range.in()), TransformTimeFromBlock(block, range.out()));
  }

  [[nodiscard]] const double& GetTrackHeight() const;
  void SetTrackHeight(const double& height);

  [[nodiscard]] int GetTrackHeightInPixels() const { return InternalHeightToPixelHeight(GetTrackHeight()); }

  void SetTrackHeightInPixels(int h) { SetTrackHeight(PixelHeightToInternalHeight(h)); }

  bool LoadCustom(QXmlStreamReader* reader, SerializedData* data) override;
  void SaveCustom(QXmlStreamWriter* writer) const override;
  void PostLoadEvent(SerializedData* data) override;

  static int InternalHeightToPixelHeight(double h) { return qRound(h * QFontMetrics(QFont()).height()); }

  static double PixelHeightToInternalHeight(int h) { return double(h) / double(QFontMetrics(QFont()).height()); }

  static int GetDefaultTrackHeightInPixels() { return InternalHeightToPixelHeight(kTrackHeightDefault); }

  static int GetMinimumTrackHeightInPixels() { return InternalHeightToPixelHeight(kTrackHeightMinimum); }

  void Retranslate() override;

  class Reference {
   public:
    Reference() : type_(kNone), index_(-1) {}

    Reference(const Track::Type& type, const int& index) : type_(type), index_(index) {}

    [[nodiscard]] const Track::Type& type() const { return type_; }

    [[nodiscard]] const int& index() const { return index_; }

    bool operator==(const Reference& ref) const { return type_ == ref.type_ && index_ == ref.index_; }

    bool operator!=(const Reference& ref) const { return !(*this == ref); }

    bool operator<(const Track::Reference& rhs) const {
      if (type_ != rhs.type_) {
        return type_ < rhs.type_;
      }

      return index_ < rhs.index_;
    }

    [[nodiscard]] QString ToString() const {
      QString type_string = TypeToString(type_);
      if (type_string.isEmpty()) {
        return {};
      } else {
        return QStringLiteral("%1:%2").arg(type_string, QString::number(index_));
      }
    }

    /// For IDs that shouldn't change between localizations
    static QString TypeToString(Type type) {
      switch (type) {
        case kVideo:
          return QStringLiteral("v");
        case kAudio:
          return QStringLiteral("a");
        case kSubtitle:
          return QStringLiteral("s");
        case kCount:
        case kNone:
          break;
      }

      return {};
    }

    /// For human-facing strings
    static QString TypeToTranslatedString(Type type) {
      switch (type) {
        case kVideo:
          return tr("V");
        case kAudio:
          return tr("A");
        case kSubtitle:
          return tr("S");
        case kCount:
        case kNone:
          break;
      }

      return {};
    }

    static Type TypeFromString(const QString& s) {
      if (s.size() >= 3) {
        if (s.at(1) == ':') {
          if (s.at(0) == 'v') {
            // Video stream
            return Track::kVideo;
          } else if (s.at(0) == 'a') {
            // Audio stream
            return Track::kAudio;
          } else if (s.at(0) == 's') {
            // Subtitle stream
            return Track::kSubtitle;
          }
        }
      }

      return Track::kNone;
    }

    static Reference FromString(const QString& s) {
      Reference ref;
      Type parse_type = TypeFromString(s);

      if (parse_type != Track::kNone) {
        bool ok;
        int parse_index = s.mid(2).toInt(&ok);

        if (ok) {
          ref.type_ = parse_type;
          ref.index_ = parse_index;
        }
      }

      return ref;
    }

    [[nodiscard]] bool IsValid() const { return type_ > kNone && type_ < kCount && index_ >= 0; }

   private:
    Track::Type type_;

    int index_;
  };

  [[nodiscard]] Reference ToReference() const { return Reference(type(), Index()); }

  [[nodiscard]] const int& Index() const { return index_; }

  void SetIndex(const int& index);

  /**
   * @brief Returns the block that starts BEFORE (not AT) and ends AFTER (not AT) a time
   *
   * Catches the first block that matches `block.in < time && block.out > time` or nullptr if any
   * block starts/ends precisely at that time or the time exceeds the track length.
   */
  [[nodiscard]] Block* BlockContainingTime(const rational& time) const;

  /**
   * @brief Returns the block that starts BEFORE a given time and ends either AFTER or AT that time
   *
   * @return Catches the first block that matches `block.out >= time` or nullptr if this time
   * exceeds the track length.
   */
  [[nodiscard]] Block* NearestBlockBefore(const rational& time) const;

  /**
   * @brief Returns the block that starts BEFORE or AT a given time.
   *
   * @return Catches the first block that matches `block.out > time` or nullptr if this time
   * exceeds the track length.
   */
  [[nodiscard]] Block* NearestBlockBeforeOrAt(const rational& time) const;

  /**
   * @brief Returns the block that starts either AT a given time or the soonest block AFTER
   *
   * @return Catches the first block that matches `block.in >= time` or nullptr if this time
   * exceeds the track length.
   */
  [[nodiscard]] Block* NearestBlockAfterOrAt(const rational& time) const;

  /**
   * @brief Returns the block that starts AFTER the given time (but never AT the given time)
   *
   * @return Catches the first block that matches `block.in > time` or nullptr if this time
   * exceeds the track length.
   */
  [[nodiscard]] Block* NearestBlockAfter(const rational& time) const;

  /*
   * @brief Returns whether a time range is empty or only has a gap
   */
  [[nodiscard]] bool IsRangeFree(const TimeRange& range) const;

  [[nodiscard]] const QVector<Block*>& Blocks() const { return blocks_; }

  void InvalidateCache(const TimeRange& range, const QString& from, int element,
                       InvalidateCacheOptions options) override;

  [[nodiscard]] Block* VisibleBlockAtTime(const rational& t) const {
    int index = GetBlockIndexAtTime(t);
    return (index == -1) ? nullptr : blocks_.at(index);
  }

  /**
   * @brief Adds Block `block` at the very beginning of the Sequence before all other clips
   */
  void PrependBlock(Block* block);

  /**
   * @brief Inserts Block `block` at a specific index (0 is the start of the timeline)
   *
   * If the index == 0, this function does the same as PrependBlock(). If the index >= the current number of blocks,
   * this function is the same as AppendBlock().
   */
  void InsertBlockAtIndex(Block* block, int index);

  /**
   * @brief Inserts Block after another Block
   *
   * Equivalent to calling InsertBlockBetweenBlocks(block, before, before->next())
   */
  void InsertBlockAfter(Block* block, Block* before);

  /**
   * @brief Inserts Block before another Block
   */
  void InsertBlockBefore(Block* block, Block* after);

  /**
   * @brief Adds Block `block` at the very end of the Sequence after all other clips
   */
  void AppendBlock(Block* block);

  /**
   * @brief Removes a Block pushing all subsequent Blocks earlier to take up the space
   */
  void RippleRemoveBlock(Block* block);

  /**
   * @brief Replaces Block `old` with Block `replace`
   *
   * Both blocks must have equal lengths.
   */
  void ReplaceBlock(Block* old, Block* replace);

  [[nodiscard]] rational track_length() const;

  [[nodiscard]] bool IsMuted() const;

  [[nodiscard]] bool IsLocked() const;

  int GetArrayIndexFromBlock(Block* block) const;

  [[nodiscard]] Sequence* sequence() const { return sequence_; }

  void set_sequence(Sequence* sequence) { sequence_ = sequence; }

  static const double kTrackHeightDefault;
  static const double kTrackHeightMinimum;
  static const double kTrackHeightInterval;

  static const QString kBlockInput;
  static const QString kMutedInput;
  static const QString kArrayMapInput;

 public slots:
  void SetMuted(bool e);

  void SetLocked(bool e);

 signals:
  /**
   * @brief Signal emitted when a Block is added to this Track
   */
  void BlockAdded(Block* block);

  /**
   * @brief Signal emitted when a Block is removed from this Track
   */
  void BlockRemoved(Block* block);

  /**
   * @brief Signal emitted when the length of the track has changed
   */
  void TrackLengthChanged();

  /**
   * @brief Signal emitted when the height of the track has changed
   */
  void TrackHeightChanged(qreal virtual_height);

  /**
   * @brief Signal emitted when the muted setting changes
   */
  void MutedChanged(bool e);

  /**
   * @brief Signal emitted when the index has changed
   */
  void IndexChanged(int old, int now);

  /**
   * @brief Emitted when a block changes length and all the subsequent blocks had to update
   */
  void BlocksRefreshed();

 protected:
  void InputConnectedEvent(const QString& input, int element, Node* node) override;
  void InputValueChangedEvent(const QString& input, int element) override;

 private:
  void UpdateInOutFrom(int index);

  [[nodiscard]] int GetArrayIndexFromCacheIndex(int index) const;

  [[nodiscard]] int GetCacheIndexFromArrayIndex(int index) const;

  [[nodiscard]] int GetBlockIndexAtTime(const rational& time) const;

  void ProcessAudioTrack(const NodeValueRow& value, const NodeGlobals& globals, NodeValueTable* table) const;

  int ConnectBlock(Block* b);

  void UpdateArrayMap();

  TimeRangeList block_length_pending_invalidations_;

  QVector<Block*> blocks_;
  QVector<uint32_t> block_array_indexes_;

  std::list<int> empty_inputs_;

  Track::Type track_type_;

  double track_height_;

  int index_;

  bool locked_;

  Sequence* sequence_;

  int ignore_arraymap_;
  bool arraymap_invalid_;
  bool ignore_arraymap_set_;

 private slots:
  void BlockLengthChanged();

  void RefreshBlockCacheFromArrayMap();
};

uint qHash(const Track::Reference& r, uint seed = 0);

QDataStream& operator<<(QDataStream& out, const Track::Reference& ref);

QDataStream& operator>>(QDataStream& in, Track::Reference& ref);

}  // namespace olive

#endif  // TRACK_H
