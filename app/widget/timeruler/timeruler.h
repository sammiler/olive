#ifndef TIMERULER_H  // 防止头文件被多次包含的宏定义
#define TIMERULER_H

#include <QTimer>   // 引入 QTimer 类，用于定时器功能
#include <QWidget>  // 引入 QWidget 类，是所有用户界面对象的基类

#include "render/playbackcache.h"  // 引入 PlaybackCache 类的定义，用于显示播放缓存状态
#include "seekablewidget.h"        // 引入 SeekableWidget 类的定义，TimeRuler 是 SeekableWidget 的派生类

namespace olive {  // olive 命名空间开始

class PlaybackCache;  // 前向声明 PlaybackCache 类 (虽然已包含头文件，但保持前向声明以符合原始风格，如果原始代码有的话)

/**
 * @brief TimeRuler 类是一个专门用于显示时间刻度标尺的控件。
 *
 * 它继承自 SeekableWidget，因此具备了可定位和时间缩放的基础功能。
 * TimeRuler 主要负责在时间轴的顶部或侧边绘制时间刻度、标记点、播放头，
 * 并且可以选择性地显示文本标签和播放缓存的状态。
 */
class TimeRuler : public SeekableWidget {
 Q_OBJECT  // Q_OBJECT 宏，用于启用 Qt 的元对象特性，如信号和槽

     public :
     /**
      * @brief 构造一个 TimeRuler 对象。
      * @param text_visible 时间刻度上的文本标签是否可见，默认为 true。
      * @param cache_status_visible 播放缓存状态条是否可见，默认为 false。
      * @param parent 父 QWidget 对象，默认为 nullptr。
      */
     explicit TimeRuler(bool text_visible = true, bool cache_status_visible = false, QWidget* parent = nullptr);

  /**
   * @brief 设置时间刻度上的文本标签是否居中显示。
   * @param c 如果为 true，则文本居中显示；否则可能靠左或其他对齐方式。
   */
  void SetCenteredText(bool c);

  /**
   * @brief 设置用于显示缓存状态的 PlaybackCache 对象。
   * @param cache 指向 PlaybackCache 对象的指针。
   */
  void SetPlaybackCache(PlaybackCache* cache);

 protected:  // 受保护的重写方法
  /**
   * @brief 重写 QGraphicsView::drawForeground() (通过 SeekableWidget -> TimeBasedView -> QGraphicsView)，
   * 用于在前景中绘制时间标尺的刻度、文本标签、标记点、播放头和缓存状态。
   * @param painter QPainter 指针，用于绘图。
   * @param rect 需要重绘的区域 (QRectF)。
   */
  void drawForeground(QPainter* painter, const QRectF& rect) override;

  /**
   * @brief 当时间基准发生变化时调用的事件处理函数 (重写自 SeekableWidget -> TimeBasedView -> TimeScaledObject)。
   * @param tb 新的时间基准 (rational)。
   */
  void TimebaseChangedEvent(const rational& tb) override;

 protected slots:  // 受保护槽函数
  /**
   * @brief 显示上下文菜单的虚函数 (重写自 SeekableWidget)。
   *
   * 在时间标尺上右键点击时调用，用于显示与标尺相关的操作选项。
   * @param p 鼠标点击的本地坐标 (QPoint)，用于定位菜单。
   * @return 如果菜单被显示并处理了事件，则返回 true；否则返回 false。
   */
  bool ShowContextMenu(const QPoint& p) override;

 private:  // 私有方法
  /**
   * @brief 更新控件的高度，通常在文本可见性或缓存状态可见性改变时调用。
   */
  void UpdateHeight();

  /**
   * @brief 获取用于显示缓存状态条的高度。
   * @return 如果显示缓存状态，则返回其预设高度；否则返回0。
   */
  [[nodiscard]] int CacheStatusHeight() const;

  int minimum_gap_between_lines_;  ///< 绘制时间刻度线之间的最小间隙（像素）。

  bool text_visible_;  ///< 标记时间刻度上的文本标签是否可见。

  bool centered_text_;  ///< 标记时间刻度上的文本标签是否应居中显示。

  double timebase_flipped_dbl_{};  ///< 时间基准的倒数的 double 类型缓存值，可能用于性能优化。

  bool show_cache_status_;  ///< 标记是否显示播放缓存状态条。

  PlaybackCache* playback_cache_;  ///< 指向 PlaybackCache 对象的指针，用于获取和显示缓存信息。
};

}  // namespace olive

#endif  // TIMERULER_H
