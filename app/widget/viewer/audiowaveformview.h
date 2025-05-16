#ifndef AUDIOWAVEFORMVIEW_H // 防止头文件被多次包含的宏定义
#define AUDIOWAVEFORMVIEW_H

#include <QWidget> // 引入 QWidget 类，是所有用户界面对象的基类
#include <QtConcurrent/QtConcurrent> // 引入 QtConcurrent 模块，用于并发编程（例如，线程池 QThreadPool）

#include "render/audioplaybackcache.h" // 引入 AudioPlaybackCache 类的定义，可能用于获取音频数据以绘制波形
#include "widget/timeruler/seekablewidget.h" // 引入 SeekableWidget 类的定义，AudioWaveformView 是 SeekableWidget 的派生类

namespace olive { // olive 命名空间开始

class ViewerOutput; // 前向声明 ViewerOutput 类

/**
 * @brief AudioWaveformView 类是一个用于显示音频波形的可视化控件。
 *
 * 它继承自 SeekableWidget，这意味着它具有在时间轴上定位和缩放的基础功能。
 * 这个控件主要负责从一个 ViewerOutput (通常代表一个音频源) 获取音频数据，
 * 并将其渲染成用户可见的波形图。
 */
class AudioWaveformView : public SeekableWidget {
  Q_OBJECT // Q_OBJECT 宏，用于启用 Qt 的元对象特性，如信号和槽

 public:
  /**
   * @brief 构造一个 AudioWaveformView 对象。
   * @param parent 父 QWidget 对象，默认为 nullptr。
   */
  explicit AudioWaveformView(QWidget *parent = nullptr);

  /**
   * @brief 设置用于获取音频数据的 ViewerOutput 节点。
   * @param playback 指向 ViewerOutput 对象的指针，该对象提供音频播放/数据源。
   */
  void SetViewer(ViewerOutput *playback);

protected: // 受保护的重写方法
  /**
   * @brief 重写 QGraphicsView::drawForeground() (通过 SeekableWidget -> TimeBasedView -> QGraphicsView)，
   * 用于在前景中绘制音频波形。
   * @param painter QPainter 指针，用于绘图。
   * @param rect 需要重绘的区域 (QRectF)。
   */
  void drawForeground(QPainter *painter, const QRectF &rect) override;

private: // 私有成员变量
  QThreadPool pool_; ///< QThreadPool 对象，可能用于在后台线程处理音频数据或波形生成，以避免阻塞UI线程。

  ViewerOutput *playback_; ///< 指向 ViewerOutput 对象的指针，作为音频数据的来源。
};

}  // namespace olive

#endif  // AUDIOWAVEFORMVIEW_H
