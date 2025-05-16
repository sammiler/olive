#ifndef TRACK_H  // 防止头文件被多次包含的宏定义开始
#define TRACK_H

#include "node/block/block.h"  // 引入 Block 类的定义，轨道是 Block 的容器

// 定义 RATIONAL_MAX 和 RATIONAL_MIN (如果它们是宏)
// #define RATIONAL_MAX some_value
// #define RATIONAL_MIN some_other_value

namespace olive {  // Olive 编辑器的命名空间

class Sequence;  // 前向声明 Sequence 类，轨道通常属于某个序列

/**
 * @brief 代表时间线上的一个轨道（或通道）的节点。
 * 轨道是组织和排列媒体块 (Block) 的容器，例如视频轨道、音频轨道或字幕轨道。
 * 它负责管理这些块的时间顺序和处理它们的输出。
 */
class Track : public Node {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief 定义轨道的类型。
      */
     enum Type {
       kNone = -1,  ///< 未指定或无效类型
       kVideo,      ///< 视频轨道
       kAudio,      ///< 音频轨道
       kSubtitle,   ///< 字幕轨道
       kCount       ///< 轨道类型的总数（用于迭代或数组大小）
     };

  /**
   * @brief Track 构造函数。
   */
  Track();

  NODE_DEFAULT_FUNCTIONS(Track)  // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此轨道的类型。
   * @return const Track::Type& 对轨道类型的常量引用。
   */
  [[nodiscard]] const Track::Type& type() const;
  /**
   * @brief 设置此轨道的类型。
   * @param track_type 新的轨道类型。
   */
  void set_type(const Track::Type& track_type);

  /**
   * @brief 获取此轨道的名称。
   * @return QString 类型的轨道名称 (例如 "视频轨道 1", "音频轨道 A")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此轨道的唯一标识符。
   * @return QString 类型的轨道 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "轨道" (Track) 或 "输出" (Output) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此轨道的描述信息。
   * @return QString 类型的轨道描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 获取在指定时间范围内，特定输入端口上的活动元素（通常是轨道上的媒体块）。
   * @param input 输入端口的名称。
   * @param r 时间范围。
   * @return ActiveElements 描述活动元素信息的对象。
   */
  [[nodiscard]] ActiveElements GetActiveElementsAtTime(const QString& input, const TimeRange& r) const override;
  /**
   * @brief 计算并输出轨道在特定时间点的值（通常是该时间点上活动块的输出）。
   * @param value 当前输入行数据。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表。
   */
  void Value(const NodeValueRow& value, const NodeGlobals& globals, NodeValueTable* table) const override;

  /**
   * @brief 根据输入调整时间范围 (从轨道时间到块内部时间)。
   * @param input 输入端口的名称。
   * @param element 输入端口的元素索引。
   * @param input_time 输入的时间范围 (轨道时间)。
   * @param clamp 是否将时间限制在有效范围内。
   * @return 调整后的时间范围 (块内部时间)。
   */
  [[nodiscard]] TimeRange InputTimeAdjustment(const QString& input, int element, const TimeRange& input_time,
                                              bool clamp) const override;

  /**
   * @brief 根据输出调整时间范围 (从块内部时间到轨道时间)。
   * @param input 输入端口的名称。
   * @param element 输入端口的元素索引。
   * @param input_time 输入的时间范围 (块内部时间)。
   * @return 调整后的时间范围 (轨道时间)。
   */
  [[nodiscard]] TimeRange OutputTimeAdjustment(const QString& input, int element,
                                               const TimeRange& input_time) const override;

  /**
   * @brief (静态工具函数) 将轨道时间转换为指定媒体块的内部相对时间。
   * @param block 目标媒体块。
   * @param time 轨道上的绝对时间。
   * @return rational 块内部的相对时间 (时间 - 块的入点)。
   */
  static rational TransformTimeForBlock(const Block* block, const rational& time) {
    if (time == RATIONAL_MAX || time == RATIONAL_MIN) {  // 处理特殊时间值
      return time;
    }
    return time - block->in();  // 时间减去块的入点
  }

  /**
   * @brief (静态工具函数) 将轨道时间范围转换为指定媒体块的内部相对时间范围。
   * @param block 目标媒体块。
   * @param range 轨道上的时间范围。
   * @return TimeRange 块内部的相对时间范围。
   */
  static TimeRange TransformRangeForBlock(const Block* block, const TimeRange& range) {
    return TimeRange(TransformTimeForBlock(block, range.in()), TransformTimeForBlock(block, range.out()));
  }

  /**
   * @brief (静态工具函数) 将指定媒体块的内部相对时间转换为轨道上的绝对时间。
   * @param block 目标媒体块。
   * @param time 块内部的相对时间。
   * @return rational 轨道上的绝对时间 (时间 + 块的入点)。
   */
  static rational TransformTimeFromBlock(const Block* block, const rational& time) {
    if (time == RATIONAL_MAX || time == RATIONAL_MIN) {  // 处理特殊时间值
      return time;
    }
    return time + block->in();  // 时间加上块的入点
  }

  /**
   * @brief (静态工具函数) 将指定媒体块的内部相对时间范围转换为轨道上的绝对时间范围。
   * @param block 目标媒体块。
   * @param range 块内部的相对时间范围。
   * @return TimeRange 轨道上的绝对时间范围。
   */
  static TimeRange TransformRangeFromBlock(const Block* block, const TimeRange& range) {
    return TimeRange(TransformTimeFromBlock(block, range.in()), TransformTimeFromBlock(block, range.out()));
  }

  /**
   * @brief 获取轨道的当前高度（内部表示，可能是一个相对单位）。
   * @return const double& 对轨道高度的常量引用。
   */
  [[nodiscard]] const double& GetTrackHeight() const;
  /**
   * @brief 设置轨道的当前高度（内部表示）。
   * @param height 新的轨道高度。
   */
  void SetTrackHeight(const double& height);

  /**
   * @brief 获取轨道在界面上显示的像素高度。
   * @return int 像素高度。
   */
  [[nodiscard]] int GetTrackHeightInPixels() const { return InternalHeightToPixelHeight(GetTrackHeight()); }

  /**
   * @brief 通过像素值设置轨道的高度。
   * @param h 像素高度。
   */
  void SetTrackHeightInPixels(int h) { SetTrackHeight(PixelHeightToInternalHeight(h)); }

  /**
   * @brief 从 XML 流加载轨道的自定义数据（例如，轨道上的媒体块、高度、类型等）。
   * @param reader 指向 QXmlStreamReader 的指针。
   * @param data 指向 SerializedData 的指针。
   * @return 如果加载成功则返回 true，否则返回 false。
   */
  bool LoadCustom(QXmlStreamReader* reader, SerializedData* data) override;
  /**
   * @brief 将轨道的自定义数据保存到 XML 流。
   * @param writer 指向 QXmlStreamWriter 的指针。
   */
  void SaveCustom(QXmlStreamWriter* writer) const override;
  /**
   * @brief 在所有节点和其基本数据加载完毕后调用的事件。
   *  用于恢复轨道上块之间的链接等。
   * @param data 指向 SerializedData 的指针。
   */
  void PostLoadEvent(SerializedData* data) override;

  /** @brief (静态工具函数) 将内部高度单位转换为像素高度。基于当前字体大小。 */
  static int InternalHeightToPixelHeight(double h) { return qRound(h * QFontMetrics(QFont()).height()); }

  /** @brief (静态工具函数) 将像素高度转换为内部高度单位。基于当前字体大小。 */
  static double PixelHeightToInternalHeight(int h) { return double(h) / double(QFontMetrics(QFont()).height()); }

  /** @brief (静态工具函数) 获取默认的轨道像素高度。 */
  static int GetDefaultTrackHeightInPixels() { return InternalHeightToPixelHeight(kTrackHeightDefault); }

  /** @brief (静态工具函数) 获取最小的轨道像素高度。 */
  static int GetMinimumTrackHeightInPixels() { return InternalHeightToPixelHeight(kTrackHeightMinimum); }

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本。
   */
  void Retranslate() override;

  /**
   * @brief 用于唯一标识一个轨道的辅助类（通过类型和索引）。
   */
  class Reference {
   public:
    /** @brief 默认构造函数，创建一个无效的轨道引用。 */
    Reference() : type_(kNone), index_(-1) {}

    /**
     * @brief 构造函数，通过轨道类型和索引创建引用。
     * @param type 轨道类型。
     * @param index 轨道在该类型中的索引。
     */
    Reference(const Track::Type& type, const int& index) : type_(type), index_(index) {}

    /** @brief 获取轨道类型。 */
    [[nodiscard]] const Track::Type& type() const { return type_; }
    /** @brief 获取轨道索引。 */
    [[nodiscard]] const int& index() const { return index_; }

    /** @brief 比较两个轨道引用是否相等。 */
    bool operator==(const Reference& ref) const { return type_ == ref.type_ && index_ == ref.index_; }
    /** @brief 比较两个轨道引用是否不等。 */
    bool operator!=(const Reference& ref) const { return !(*this == ref); }
    /** @brief 定义轨道引用的排序，用于 std::map 等容器。 */
    bool operator<(const Track::Reference& rhs) const {
      if (type_ != rhs.type_) {
        return type_ < rhs.type_;
      }
      return index_ < rhs.index_;
    }

    /**
     * @brief 将轨道引用转换为字符串表示 (例如 "v:0", "a:1")。
     * @return QString 字符串表示，如果类型无效则为空字符串。
     */
    [[nodiscard]] QString ToString() const {
      QString type_string = TypeToString(type_);  // 获取类型字符串
      if (type_string.isEmpty()) {
        return {};
      } else {
        return QStringLiteral("%1:%2").arg(type_string, QString::number(index_));  // 格式化为 "类型:索引"
      }
    }

    /**
     * @brief (静态工具函数) 将轨道类型枚举转换为简短的字符串表示（用于ID，不应本地化）。
     * @param type 轨道类型。
     * @return QString 类型字符串 ("v", "a", "s")，无效则为空。
     */
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

    /**
     * @brief (静态工具函数) 将轨道类型枚举转换为用户可见的、可本地化的字符串表示。
     * @param type 轨道类型。
     * @return QString 本地化的类型字符串 (例如 "V", "A", "S")。
     */
    static QString TypeToTranslatedString(Type type) {
      switch (type) {
        case kVideo:
          return tr("V");  // 支持翻译
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

    /**
     * @brief (静态工具函数) 从字符串表示中解析出轨道类型。
     *  期望格式如 "v:0", "a:1"。
     * @param s 输入字符串。
     * @return Type 解析出的轨道类型，如果无法解析则为 Track::kNone。
     */
    static Type TypeFromString(const QString& s) {
      if (s.size() >= 3) {  // 至少需要 "x:y" 格式
        if (s.at(1) == ':') {
          if (s.at(0) == 'v')
            return Track::kVideo;
          else if (s.at(0) == 'a')
            return Track::kAudio;
          else if (s.at(0) == 's')
            return Track::kSubtitle;
        }
      }
      return Track::kNone;
    }

    /**
     * @brief (静态工具函数) 从字符串表示中完整解析出轨道引用 (类型和索引)。
     * @param s 输入字符串。
     * @return Reference 解析出的轨道引用，如果无法解析则为无效引用。
     */
    static Reference FromString(const QString& s) {
      Reference ref;
      Type parse_type = TypeFromString(s);  // 先解析类型
      if (parse_type != Track::kNone) {
        bool ok;
        int parse_index = s.mid(2).toInt(&ok);  // 解析索引部分
        if (ok) {
          ref.type_ = parse_type;
          ref.index_ = parse_index;
        }
      }
      return ref;
    }

    /** @brief 检查此轨道引用是否有效 (类型有效且索引非负)。 */
    [[nodiscard]] bool IsValid() const { return type_ > kNone && type_ < kCount && index_ >= 0; }

   private:
    Track::Type type_;  ///< 轨道类型。
    int index_;         ///< 轨道在该类型中的索引。
  };

  /**
   * @brief 将当前轨道对象转换为一个轨道引用。
   * @return Reference 对应的轨道引用。
   */
  [[nodiscard]] Reference ToReference() const { return Reference(type(), Index()); }

  /**
   * @brief 获取此轨道在其所属序列中同类型轨道里的索引。
   * @return const int& 轨道索引的常量引用。
   */
  [[nodiscard]] const int& Index() const { return index_; }

  /**
   * @brief 设置此轨道的索引。
   * @param index 新的轨道索引。会发射 IndexChanged 信号。
   */
  void SetIndex(const int& index);

  /**
   * @brief 返回在给定时间点之前开始并且之后结束的媒体块。
   *  即 `block.in < time && block.out > time`。
   * @param time 给定的时间点。
   * @return Block* 包含该时间点的块，如果没有则为 nullptr。
   */
  [[nodiscard]] Block* BlockContainingTime(const rational& time) const;

  /**
   * @brief 返回在给定时间点之前开始，并且在该时间点或之后结束的最近的媒体块。
   *  即 `block.out >= time`。
   * @param time 给定的时间点。
   * @return Block* 找到的块，如果没有则为 nullptr。
   */
  [[nodiscard]] Block* NearestBlockBefore(const rational& time) const;

  /**
   * @brief 返回在给定时间点或之前开始的最近的媒体块，且其结束点在该时间点之后。
   *  即 `block.out > time`。
   * @param time 给定的时间点。
   * @return Block* 找到的块，如果没有则为 nullptr。
   */
  [[nodiscard]] Block* NearestBlockBeforeOrAt(const rational& time) const;

  /**
   * @brief 返回在给定时间点或之后开始的最近的媒体块。
   *  即 `block.in >= time`。
   * @param time 给定的时间点。
   * @return Block* 找到的块，如果没有则为 nullptr。
   */
  [[nodiscard]] Block* NearestBlockAfterOrAt(const rational& time) const;

  /**
   * @brief 返回严格在给定时间点之后开始的最近的媒体块。
   *  即 `block.in > time`。
   * @param time 给定的时间点。
   * @return Block* 找到的块，如果没有则为 nullptr。
   */
  [[nodiscard]] Block* NearestBlockAfter(const rational& time) const;

  /*
   * @brief 检查给定的时间范围在轨道上是否为空，或者仅包含间隙块 (GapBlock)。
   * @param range 要检查的时间范围。
   * @return 如果范围为空或只有间隙则返回 true，否则返回 false。
   */
  [[nodiscard]] bool IsRangeFree(const TimeRange& range) const;

  /**
   * @brief 获取轨道上所有媒体块的列表。
   * @return const QVector<Block*>& 对媒体块指针向量的常量引用。
   */
  [[nodiscard]] const QVector<Block*>& Blocks() const { return blocks_; }

  /**
   * @brief 使指定时间范围内的缓存无效。
   * @param range 需要作废的时间范围。
   * @param from 触发作废的源头标识。
   * @param element 相关的元素索引。
   * @param options 作废缓存的选项。
   */
  void InvalidateCache(const TimeRange& range, const QString& from, int element,
                       InvalidateCacheOptions options) override;

  /**
   * @brief 获取在指定时间点可见（活动）的媒体块。
   * @param t 时间点。
   * @return Block* 活动的块，如果没有则为 nullptr。
   */
  [[nodiscard]] Block* VisibleBlockAtTime(const rational& t) const {
    int index = GetBlockIndexAtTime(t);                  // 获取该时间点的块索引
    return (index == -1) ? nullptr : blocks_.at(index);  // 返回对应块或nullptr
  }

  /**
   * @brief 将媒体块添加到轨道的开头，在所有其他剪辑之前。
   * @param block 要添加的媒体块。
   */
  void PrependBlock(Block* block);

  /**
   * @brief 在指定的索引位置插入媒体块 (0 表示轨道开始)。
   *  如果索引为0，等同于 PrependBlock()。如果索引大于等于当前块数量，等同于 AppendBlock()。
   * @param block 要插入的媒体块。
   * @param index 插入位置的索引。
   */
  void InsertBlockAtIndex(Block* block, int index);

  /**
   * @brief 在另一个媒体块之后插入新的媒体块。
   *  等同于调用 InsertBlockBetweenBlocks(block, before, before->next())。
   * @param block 要插入的媒体块。
   * @param before 在此块之后插入新块。
   */
  void InsertBlockAfter(Block* block, Block* before);

  /**
   * @brief 在另一个媒体块之前插入新的媒体块。
   * @param block 要插入的媒体块。
   * @param after 在此块之前插入新块。
   */
  void InsertBlockBefore(Block* block, Block* after);

  /**
   * @brief 将媒体块添加到轨道的末尾，在所有其他剪辑之后。
   * @param block 要添加的媒体块。
   */
  void AppendBlock(Block* block);

  /**
   * @brief “波纹删除”一个媒体块。删除该块并使其后的所有块向前移动以填充空间。
   * @param block 要删除的媒体块。
   */
  void RippleRemoveBlock(Block* block);

  /**
   * @brief 用一个新的媒体块替换轨道上一个旧的媒体块。
   *  要求两个块的长度必须相等。
   * @param old 要被替换的旧块。
   * @param replace 用于替换的新块。
   */
  void ReplaceBlock(Block* old, Block* replace);

  /**
   * @brief 获取轨道的总长度（通常由最后一个块的出点决定）。
   * @return rational 轨道的总长度。
   */
  [[nodiscard]] rational track_length() const;

  /**
   * @brief 检查轨道是否被静音。
   * @return 如果静音则返回 true，否则返回 false。
   */
  [[nodiscard]] bool IsMuted() const;

  /**
   * @brief 检查轨道是否被锁定（防止编辑）。
   * @return 如果锁定则返回 true，否则返回 false。
   */
  [[nodiscard]] bool IsLocked() const;

  /**
   * @brief 根据媒体块指针获取其在内部 `blocks_` 数组中的索引。
   * @param block 媒体块指针。
   * @return int 数组索引，如果未找到则为 -1 或其他错误值。
   */
  int GetArrayIndexFromBlock(Block* block) const;

  /**
   * @brief 获取此轨道所属的序列对象。
   * @return Sequence* 指向序列对象的指针。
   */
  [[nodiscard]] Sequence* sequence() const { return sequence_; }

  /**
   * @brief 设置此轨道所属的序列对象。
   * @param sequence 指向序列对象的指针。
   */
  void set_sequence(Sequence* sequence) { sequence_ = sequence; }

  // --- 静态常量，定义轨道高度相关的默认值 ---
  static const double kTrackHeightDefault;   ///< 默认轨道高度（内部单位）。
  static const double kTrackHeightMinimum;   ///< 最小轨道高度（内部单位）。
  static const double kTrackHeightInterval;  ///< 轨道高度调整的步进间隔（内部单位）。

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kBlockInput;     ///< "Blocks" - 连接媒体块的输入端口（可能是一个数组输入）的键名。
  static const QString kMutedInput;     ///< "Muted" - 轨道静音状态的参数键名。
  static const QString kArrayMapInput;  ///< "ArrayMap" - 块索引映射（用于优化或特定处理）的参数键名。

 public slots:
  /**
   * @brief 设置轨道的静音状态。会发射 MutedChanged 信号。
   * @param e true 表示静音，false 表示取消静音。
   */
  void SetMuted(bool e);

  /**
   * @brief 设置轨道的锁定状态。
   * @param e true 表示锁定，false 表示解锁。
   */
  void SetLocked(bool e);

 signals:  // Qt 信号声明区域
  /**
   * @brief 当有媒体块被添加到此轨道时发射此信号。
   * @param block 被添加的媒体块。
   */
  void BlockAdded(Block* block);

  /**
   * @brief 当有媒体块从此轨道移除时发射此信号。
   * @param block 被移除的媒体块。
   */
  void BlockRemoved(Block* block);

  /**
   * @brief 当轨道的总长度发生变化时发射此信号。
   */
  void TrackLengthChanged();

  /**
   * @brief 当轨道的高度（在UI上的显示高度）发生变化时发射此信号。
   * @param virtual_height 新的轨道高度（内部单位）。
   */
  void TrackHeightChanged(qreal virtual_height);

  /**
   * @brief 当轨道的静音设置发生变化时发射此信号。
   * @param e 新的静音状态。
   */
  void MutedChanged(bool e);

  /**
   * @brief 当轨道的索引发生变化时发射此信号。
   * @param old 旧的索引。
   * @param now 新的索引。
   */
  void IndexChanged(int old, int now);

  /**
   * @brief 当轨道上的一个块长度改变，导致其后所有块的位置都需要更新时发射此信号。
   */
  void BlocksRefreshed();

 protected:
  /**
   * @brief 当节点的某个输入端口连接上一个输出节点时调用的事件处理函数。
   * @param input 连接的输入端口名称。
   * @param element 输入端口的元素索引。
   * @param node 连接到的输出节点。
   */
  void InputConnectedEvent(const QString& input, int element, Node* node) override;
  /**
   * @brief 当节点的某个输入参数值发生变化时调用的事件处理函数。
   * @param input 值发生变化的输入端口/参数名称。
   * @param element 相关元素的索引。
   */
  void InputValueChangedEvent(const QString& input, int element) override;

 private:
  /**
   * @brief 从指定的索引开始，更新后续所有块的入点和出点。
   *  用于在插入、删除或某个块长度改变后调整时间线。
   * @param index 开始更新的块在 `blocks_` 数组中的索引。
   */
  void UpdateInOutFrom(int index);

  /** @brief 根据缓存索引获取 `blocks_` 数组中的实际索引。 */
  [[nodiscard]] int GetArrayIndexFromCacheIndex(int index) const;
  /** @brief 根据 `blocks_` 数组中的实际索引获取缓存索引。 */
  [[nodiscard]] int GetCacheIndexFromArrayIndex(int index) const;

  /**
   * @brief 获取在指定时间点活动的媒体块在 `blocks_` 数组中的索引。
   * @param time 时间点。
   * @return int 块的索引，如果该时间点没有活动块则返回 -1。
   */
  [[nodiscard]] int GetBlockIndexAtTime(const rational& time) const;

  /**
   * @brief 处理音频轨道的输出值。
   *  与通用的 Value() 方法不同，此方法特定于音频数据的混合和处理。
   * @param value 当前输入行数据。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表。
   */
  void ProcessAudioTrack(const NodeValueRow& value, const NodeGlobals& globals, NodeValueTable* table) const;

  /**
   * @brief 将一个媒体块连接到此轨道（例如，设置其 track_ 指针，并可能分配输入端口）。
   * @param b 要连接的媒体块。
   * @return int 连接成功后块所使用的输入端口索引，或错误代码。
   */
  int ConnectBlock(Block* b);

  /**
   * @brief 更新内部的块索引映射 (ArrayMap)。
   *  ArrayMap 可能用于优化对特定时间点活动块的查找。
   */
  void UpdateArrayMap();

  TimeRangeList block_length_pending_invalidations_;  ///< 存储因块长度改变而待处理的缓存失效请求。

  QVector<Block*> blocks_;                 ///< 存储轨道上所有媒体块指针的向量。
  QVector<uint32_t> block_array_indexes_;  ///< 存储每个块在某种全局数组或缓存中的索引。

  std::list<int> empty_inputs_;  ///< 存储当前轨道上可用的（未被块占用的）输入端口索引列表。

  Track::Type track_type_;  ///< 轨道的类型 (视频, 音频, 字幕)。
  double track_height_;     ///< 轨道的显示高度 (内部单位)。
  int index_;               ///< 此轨道在其所属序列中同类型轨道里的索引。
  bool locked_;             ///< 轨道是否被锁定。
  Sequence* sequence_;      ///< 指向此轨道所属的序列对象的指针。

  // --- ArrayMap 相关状态变量 ---
  int ignore_arraymap_;       ///< 一个计数器或标志，用于临时忽略 ArrayMap 的更新。
  bool arraymap_invalid_;     ///< 标记 ArrayMap 当前是否失效，需要重建。
  bool ignore_arraymap_set_;  ///< 标记 ignore_arraymap_ 是否被主动设置过。

 private slots:
  /**
   * @brief 当轨道上某个媒体块的长度发生变化时调用的槽函数。
   *  需要触后续块的刷新和轨道总长度的更新。
   */
  void BlockLengthChanged();

  /**
   * @brief 从 ArrayMap 输入参数的值刷新块缓存的槽函数。
   *  当 ArrayMap 更新时，可能需要重新评估哪些块是活动的。
   */
  void RefreshBlockCacheFromArrayMap();
};

/**
 * @brief 为 Track::Reference 类型提供 qHash 函数，使其可以用于 QHash 等 Qt 容器。
 * @param r Track::Reference 对象。
 * @param seed 哈希种子。
 * @return uint 计算得到的哈希值。
 */
uint qHash(const Track::Reference& r, uint seed = 0);

/**
 * @brief 重载 QDataStream 的输出操作符，用于序列化 Track::Reference 对象。
 * @param out QDataStream 输出流。
 * @param ref 要序列化的 Track::Reference 对象。
 * @return QDataStream& 输出流的引用。
 */
QDataStream& operator<<(QDataStream& out, const Track::Reference& ref);

/**
 * @brief 重载 QDataStream 的输入操作符，用于反序列化 Track::Reference 对象。
 * @param in QDataStream 输入流。
 * @param ref 用于存储反序列化结果的 Track::Reference 对象。
 * @return QDataStream& 输入流的引用。
 */
QDataStream& operator>>(QDataStream& in, Track::Reference& ref);

}  // namespace olive

#endif  // TRACK_H // 头文件宏定义结束