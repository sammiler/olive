#ifndef BLOCK_H
#define BLOCK_H

#include "node/node.h"             // 引入基类 Node 的定义
#include "timeline/timelinecommon.h" // 引入时间线相关的通用定义，可能包含 rational, TimeRange 等

namespace olive {

class TransitionBlock; // 前向声明 TransitionBlock，可能用于类型检查或特定逻辑

/**
 * @brief 代表一个时间块的节点，可以显示在时间线上。
 * 这是时间线上各种元素（如媒体片段、间隙、转场等）的基类。
 */
class Block : public Node {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief Block 构造函数。
   */
  Block();

  /**
   * @brief 获取此 Block 所属的分类 ID 列表。
   *  对于 Block 类型的节点，通常可能属于 "时间线元素" 或类似的分类。
   * @return CategoryID 的 QVector。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;

  /**
   * @brief 获取此 Block 在时间线上的入点。
   * @return const rational& 类型的入点时间。
   */
  [[nodiscard]] const rational& in() const { return in_point_; }

  /**
   * @brief 获取此 Block 在时间线上的出点。
   * @return const rational& 类型的出点时间。
   */
  [[nodiscard]] const rational& out() const { return out_point_; }

  /**
   * @brief 设置此 Block 在时间线上的入点。
   * @param in rational 类型的入点时间。
   */
  void set_in(const rational& in) { in_point_ = in; }

  /**
   * @brief 设置此 Block 在时间线上的出点。
   * @param out rational 类型的出点时间。
   */
  void set_out(const rational& out) { out_point_ = out; }

  /**
   * @brief 获取此 Block 的长度 (出点 - 入点)。
   * @return rational 类型的长度。
   */
  [[nodiscard]] rational length() const;
  /**
   * @brief 设置 Block 的长度，并相应调整媒体的出点（如果适用）。
   *  此方法通常由派生类（如 ClipBlock）重写以实现特定行为。
   * @param length 新的长度。
   */
  virtual void set_length_and_media_out(const rational& length);
  /**
   * @brief 设置 Block 的长度，并相应调整媒体的入点（如果适用）。
   *  此方法通常由派生类（如 ClipBlock）重写以实现特定行为。
   * @param length 新的长度。
   */
  virtual void set_length_and_media_in(const rational& length);

  /**
   * @brief 获取此 Block 所占据的时间范围 (从入点到出点)。
   * @return TimeRange 对象。
   */
  [[nodiscard]] TimeRange range() const { return TimeRange(in(), out()); }

  /**
   * @brief 获取轨道上此 Block 的前一个 Block。
   * @return 指向 Block 对象的指针，如果没有前一个 Block 则为 nullptr。
   */
  [[nodiscard]] Block* previous() const { return previous_; }

  /**
   * @brief 获取轨道上此 Block 的后一个 Block。
   * @return 指向 Block 对象的指针，如果没有后一个 Block 则为 nullptr。
   */
  [[nodiscard]] Block* next() const { return next_; }

  /**
   * @brief 设置轨道上此 Block 的前一个 Block。
   * @param previous 指向 Block 对象的指针。
   */
  void set_previous(Block* previous) { previous_ = previous; }

  /**
   * @brief 设置轨道上此 Block 的后一个 Block。
   * @param next 指向 Block 对象的指针。
   */
  void set_next(Block* next) { next_ = next; }

  /**
   * @brief 获取此 Block 所属的轨道。
   * @return 指向 Track 对象的指针，如果未分配到轨道则为 nullptr。
   */
  [[nodiscard]] Track* track() const { return track_; }

  /**
   * @brief 设置此 Block 所属的轨道。
   * @param track 指向 Track 对象的指针。会发射 TrackChanged 信号。
   */
  void set_track(Track* track) {
    track_ = track;             // 设置轨道指针
    emit TrackChanged(track_); // 发射轨道变更信号
  }

  /**
   * @brief 检查此 Block 是否启用。
   *  禁用的 Block 通常在播放或渲染时被忽略。
   * @return 如果启用则返回 true，否则返回 false。
   */
  [[nodiscard]] bool is_enabled() const;
  /**
   * @brief 设置此 Block 的启用状态。
   * @param e true 表示启用，false 表示禁用。会发射 EnabledChanged 信号。
   */
  void set_enabled(bool e);

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本。
   */
  void Retranslate() override;

  /**
   * @brief 使指定时间范围内的缓存无效。
   *  对于 Block 基类，这可能是一个通用实现或需要派生类进一步具体化。
   * @param range 需要作废的时间范围。
   * @param from 触发作废的源头标识。
   * @param element 相关的元素索引，默认为 -1。
   * @param options 作废缓存的选项，默认为 InvalidateCacheOptions()。
   */
  void InvalidateCache(const TimeRange& range, const QString& from, int element = -1,
                       InvalidateCacheOptions options = InvalidateCacheOptions()) override;

  static const QString kLengthInput; ///< "Length" - 长度参数的键名。

  /**
   * @brief 工具函数，方便地设置两个 Block 之间的前后链接关系。
   * @param previous 前一个 Block。
   * @param next 后一个 Block。
   */
  static void set_previous_next(Block* previous, Block* next);

 public slots: // Qt 槽函数声明区域

 signals: // Qt 信号声明区域
  /**
   * @brief 当 Block 的启用状态发生改变时发射此信号。
   */
  void EnabledChanged();

  /**
   * @brief 当 Block 的长度发生改变时发射此信号。
   */
  void LengthChanged();

  /**
   * @brief 当 Block 的预览状态（例如，是否被预览窗口关注）发生改变时发射此信号。
   */
  void PreviewChanged();

  /**
   * @brief 当 Block 所属的轨道发生改变时发射此信号。
   * @param track 新的轨道指针。
   */
  void TrackChanged(Track* track);

 protected:
  /**
   * @brief 当节点的某个输入参数值发生变化时调用的事件处理函数。
   *  重写自 Node 类，用于处理特定于 Block 的参数变化，例如长度。
   * @param input 值发生变化的输入端口/参数名称。
   * @param element 相关元素的索引。
   */
  void InputValueChangedEvent(const QString& input, int element) override;

  // --- 受保护的成员变量 ---
  Block* previous_; ///< 指向轨道上此 Block 的前一个 Block。
  Block* next_;     ///< 指向轨道上此 Block 的后一个 Block。

 private:
  /**
   * @brief 内部函数，用于设置 Block 的长度，并更新出点。
   * @param length 新的长度。
   */
  void set_length_internal(const rational& length);

  // --- 私有成员变量 ---
  rational in_point_;  ///< Block 在时间线上的入点。
  rational out_point_; ///< Block 在时间线上的出点。
  Track* track_;       ///< 指向此 Block 所属的 Track 对象。

  rational last_length_; ///< 上一次记录的长度，可能用于检测长度变化。
};

}  // namespace olive

#endif  // BLOCK_H