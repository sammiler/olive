#ifndef TIMELINEVIEWGHOSTITEM_H // 防止头文件被多次包含的宏定义
#define TIMELINEVIEWGHOSTITEM_H

#include <QVariant> // 引入 QVariant 类，用于存储不同类型的数据

#include "node/block/clip/clip.h"           // 引入 ClipBlock 类的定义
#include "node/block/transition/transition.h" // 引入 TransitionBlock 类的定义
#include "node/output/track/track.h"        // 引入 Track 类及其嵌套类型 (如 Track::Reference) 的定义
#include "node/project/footage/footage.h"   // 引入 Footage 相关类的定义 (如 ViewerOutput，虽然此处未直接使用，但 AttachedFootage 结构体中用到)
#include "timeline/timelinecommon.h"        // 引入时间轴相关的通用定义 (如 rational, Timeline::MovementMode)

namespace olive { // olive 命名空间开始

/**
 * @brief TimelineViewGhostItem 类表示用户在应用更改之前所做更改的图形化预览。
 *
 * "幽灵项" (Ghost Item) 通常在时间轴上用于显示拖动、修剪或移动操作的即时反馈，
 * 用户可以看到操作的结果，然后在释放鼠标或确认操作后，这些更改才会实际应用到数据模型中。
 */
class TimelineViewGhostItem {
 public:
  /**
   * @brief DataType 枚举定义了幽灵项可以附加的额外数据类型。
   *
   * 这允许幽灵项携带关于其来源或特殊状态的上下文信息。
   */
  enum DataType {
    kAttachedBlock,        ///< 幽灵项关联到一个已存在的 Block (例如，正在被拖动的剪辑)
    kReferenceBlock,       ///< 幽灵项引用另一个 Block (可能用于显示相对位置或限制)
    kAttachedFootage,      ///< 幽灵项关联到一个素材 (Footage) (例如，从媒体池拖动新素材时)
    kGhostIsSliding,       ///< 标记此幽灵项代表一个正在进行的滑动 (slide) 编辑操作
    kTrimIsARollEdit,      ///< 标记此幽灵项代表一个滚动 (roll) 编辑修剪操作
    kTrimShouldBeIgnored   ///< 标记此幽灵项的修剪操作应被忽略 (可能用于特定工具逻辑)
  };

  /**
   * @brief AttachedFootage 结构体用于存储附加到幽灵项的素材信息。
   */
  struct AttachedFootage {
    ViewerOutput* footage{}; ///< 指向素材的 ViewerOutput 节点的指针，默认为 nullptr。
    QString output;          ///< 可能表示素材的特定输出流或标识符。
  };

  /**
   * @brief 构造一个默认的 TimelineViewGhostItem 对象。
   *
   * 初始化成员变量，例如轨道调整量、移动模式、是否可见等。
   */
  TimelineViewGhostItem()
      : track_adj_(0), mode_(Timeline::kNone), can_have_zero_length_(true), can_move_tracks_(true), invisible_(false) {}

  /**
   * @brief 从一个现有的 Block 对象创建一个新的 TimelineViewGhostItem。
   *
   * 这是一个静态工厂方法，方便地根据 Block 的属性初始化幽灵项。
   * @param block 指向源 Block 对象的指针。
   * @return 指向新创建的 TimelineViewGhostItem 对象的指针。
   */
  static TimelineViewGhostItem* FromBlock(Block* block) {
    auto* ghost = new TimelineViewGhostItem(); // 创建新的幽灵项实例

    ghost->SetIn(block->in()); // 设置幽灵项的入点为 Block 的入点
    ghost->SetOut(block->out()); // 设置幽灵项的出点为 Block 的出点
    if (dynamic_cast<ClipBlock*>(block)) { // 如果 Block 是一个 ClipBlock
      ghost->SetMediaIn(dynamic_cast<ClipBlock*>(block)->media_in()); // 设置幽灵项的媒体入点
    }
    ghost->SetTrack(block->track()->ToReference()); // 设置幽灵项的轨道引用
    ghost->SetData(kAttachedBlock, QtUtils::PtrToValue(block)); // 将原始 Block 作为附加数据存储

    // 根据 Block 类型设置是否可以有零长度的属性
    if (dynamic_cast<ClipBlock*>(block)) {
      ghost->can_have_zero_length_ = false; // 剪辑块通常不能为零长度
    } else if (dynamic_cast<TransitionBlock*>(block)) {
      ghost->can_have_zero_length_ = false; // 转场块通常也不能为零长度
    }

    return ghost; // 返回创建的幽灵项
  }

  /**
   * @brief 检查此幽灵项是否可以具有零长度。
   * @return 如果可以为零长度，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool CanHaveZeroLength() const { return can_have_zero_length_; }

  /**
   * @brief 检查此幽灵项是否允许跨轨道移动。
   * @return 如果允许跨轨道移动，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool GetCanMoveTracks() const { return can_move_tracks_; }

  /**
   * @brief 设置此幽灵项是否允许跨轨道移动。
   * @param e 如果为 true，则允许跨轨道移动。
   */
  void SetCanMoveTracks(bool e) { can_move_tracks_ = e; }

  /**
   * @brief 获取幽灵项的原始入点时间。
   * @return 对原始入点时间 (rational) 的常量引用。
   */
  [[nodiscard]] const rational& GetIn() const { return in_; }

  /**
   * @brief 获取幽灵项的原始出点时间。
   * @return 对原始出点时间 (rational) 的常量引用。
   */
  [[nodiscard]] const rational& GetOut() const { return out_; }

  /**
   * @brief 获取幽灵项的原始媒体入点时间（如果适用）。
   * @return 对原始媒体入点时间 (rational) 的常量引用。
   */
  [[nodiscard]] const rational& GetMediaIn() const { return media_in_; }

  /**
   * @brief 获取幽灵项的原始长度。
   * @return 幽灵项的长度 (out_ - in_) (rational)。
   */
  [[nodiscard]] rational GetLength() const { return out_ - in_; }

  /**
   * @brief 获取幽灵项调整后的长度。
   * @return 调整后的长度 (GetAdjustedOut() - GetAdjustedIn()) (rational)。
   */
  [[nodiscard]] rational GetAdjustedLength() const { return GetAdjustedOut() - GetAdjustedIn(); }

  /**
   * @brief 设置幽灵项的原始入点时间。
   * @param in 新的入点时间 (rational)。
   */
  void SetIn(const rational& in) { in_ = in; }

  /**
   * @brief 设置幽灵项的原始出点时间。
   * @param out 新的出点时间 (rational)。
   */
  void SetOut(const rational& out) { out_ = out; }

  /**
   * @brief 设置幽灵项的原始媒体入点时间。
   * @param media_in 新的媒体入点时间 (rational)。
   */
  void SetMediaIn(const rational& media_in) { media_in_ = media_in; }

  /**
   * @brief 设置入点时间的调整量。
   * @param in_adj 入点时间的调整值 (rational)。
   */
  void SetInAdjustment(const rational& in_adj) { in_adj_ = in_adj; }

  /**
   * @brief 设置出点时间的调整量。
   * @param out_adj 出点时间的调整值 (rational)。
   */
  void SetOutAdjustment(const rational& out_adj) { out_adj_ = out_adj; }

  /**
   * @brief 设置轨道索引的调整量。
   * @param track_adj 轨道索引的调整值 (int)。
   */
  void SetTrackAdjustment(const int& track_adj) { track_adj_ = track_adj; }

  /**
   * @brief 设置媒体入点时间的调整量。
   * @param media_in_adj 媒体入点时间的调整值 (rational)。
   */
  void SetMediaInAdjustment(const rational& media_in_adj) { media_in_adj_ = media_in_adj; }

  /**
   * @brief 获取入点时间的调整量。
   * @return 对入点时间调整量 (rational) 的常量引用。
   */
  [[nodiscard]] const rational& GetInAdjustment() const { return in_adj_; }

  /**
   * @brief 获取出点时间的调整量。
   * @return 对出点时间调整量 (rational) 的常量引用。
   */
  [[nodiscard]] const rational& GetOutAdjustment() const { return out_adj_; }

  /**
   * @brief 获取媒体入点时间的调整量。
   * @return 对媒体入点时间调整量 (rational) 的常量引用。
   */
  [[nodiscard]] const rational& GetMediaInAdjustment() const { return media_in_adj_; }

  /**
   * @brief 获取轨道索引的调整量。
   * @return 对轨道索引调整量 (int) 的常量引用。
   */
  [[nodiscard]] const int& GetTrackAdjustment() const { return track_adj_; }

  /**
   * @brief 获取调整后的入点时间 (原始入点 + 调整量)。
   * @return 调整后的入点时间 (rational)。
   */
  [[nodiscard]] rational GetAdjustedIn() const { return in_ + in_adj_; }

  /**
   * @brief 获取调整后的出点时间 (原始出点 + 调整量)。
   * @return 调整后的出点时间 (rational)。
   */
  [[nodiscard]] rational GetAdjustedOut() const { return out_ + out_adj_; }

  /**
   * @brief 获取调整后的媒体入点时间 (原始媒体入点 + 调整量)。
   * @return 调整后的媒体入点时间 (rational)。
   */
  [[nodiscard]] rational GetAdjustedMediaIn() const { return media_in_ + media_in_adj_; }

  /**
   * @brief 获取调整后的轨道引用。
   * @return 调整后的轨道引用 (Track::Reference)。
   */
  [[nodiscard]] Track::Reference GetAdjustedTrack() const {
    return Track::Reference(track_.type(), track_.index() + track_adj_); // 基于原始轨道类型和调整后的索引创建新的轨道引用
  }

  /**
   * @brief 获取幽灵项当前的移动/修剪模式。
   * @return 对当前模式 (Timeline::MovementMode) 的常量引用。
   */
  [[nodiscard]] const Timeline::MovementMode& GetMode() const { return mode_; }

  /**
   * @brief 设置幽灵项的移动/修剪模式。
   * @param mode 新的模式 (Timeline::MovementMode)。
   */
  void SetMode(const Timeline::MovementMode& mode) { mode_ = mode; }

  /**
   * @brief 检查幽灵项的任何时间或轨道属性是否已被调整。
   * @return 如果有任何调整量不为零，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool HasBeenAdjusted() const {
    return GetInAdjustment() != rational(0) || GetOutAdjustment() != rational(0) ||
           GetMediaInAdjustment() != rational(0) || GetTrackAdjustment() != 0;
  }

  /**
   * @brief 获取与指定键关联的附加数据。
   * @param key 数据类型键 (DataType 枚举值)。
   * @return 存储的 QVariant 数据，如果键不存在则可能返回无效的 QVariant。
   */
  [[nodiscard]] QVariant GetData(int key) const { return data_.value(key); }

  /**
   * @brief 设置与指定键关联的附加数据。
   * @param key 数据类型键 (DataType 枚举值)。
   * @param value 要存储的 QVariant 数据。
   */
  void SetData(int key, const QVariant& value) { data_.insert(key, value); }

  /**
   * @brief 获取幽灵项的原始轨道引用。
   * @return 对原始轨道引用 (Track::Reference) 的常量引用。
   */
  [[nodiscard]] const Track::Reference& GetTrack() const { return track_; }

  /**
   * @brief 设置幽灵项的原始轨道引用。
   * @param track 新的轨道引用 (Track::Reference)。
   */
  void SetTrack(const Track::Reference& track) { track_ = track; }

  /**
   * @brief 检查幽灵项当前是否被标记为不可见。
   * @return 如果不可见，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool IsInvisible() const { return invisible_; }

  /**
   * @brief 设置幽灵项的可见性状态。
   * @param e 如果为 true，则标记为不可见；否则标记为可见。
   */
  void SetInvisible(bool e) { invisible_ = e; }

 protected: // 受保护成员 (在此类中为空)
 private: // 私有成员变量
  rational in_;         ///< 幽灵项的原始入点时间。
  rational out_;        ///< 幽灵项的原始出点时间。
  rational media_in_;   ///< 幽灵项的原始媒体入点时间（主要用于 ClipBlock）。

  rational in_adj_;     ///< 入点时间的调整量。
  rational out_adj_;    ///< 出点时间的调整量。
  rational media_in_adj_; ///< 媒体入点时间的调整量。

  int track_adj_;       ///< 轨道索引的调整量。

  Timeline::MovementMode mode_; ///< 当前幽灵项的移动或修剪模式。

  bool can_have_zero_length_; ///< 标记此幽灵项是否允许具有零长度。
  bool can_move_tracks_;      ///< 标记此幽灵项是否允许在轨道间移动。

  Track::Reference track_;    ///< 幽灵项的原始轨道引用。

  QHash<int, QVariant> data_; ///< 用于存储与此幽灵项相关的附加数据 (例如关联的 Block 指针)。

  bool invisible_;            ///< 标记此幽灵项是否应在视图中绘制。
};

}  // namespace olive

// 声明 AttachedFootage 结构体为元类型，以便可以在 QVariant 中使用。
// 这通常在命名空间之外，但在定义了该类型的命名空间之后。
Q_DECLARE_METATYPE(olive::TimelineViewGhostItem::AttachedFootage)

#endif  // TIMELINEVIEWGHOSTITEM_H
