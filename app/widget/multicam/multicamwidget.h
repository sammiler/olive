#ifndef MULTICAMWIDGET_H
#define MULTICAMWIDGET_H

#include "multicamdisplay.h"                  // 多机位显示控件
#include "node/input/multicam/multicamnode.h" // 多机位数据节点
#include "widget/viewer/viewer.h"             // 查看器相关类 (可能包含 ViewerOutput, ViewerSizer)
#include "widget/timebased/timebasedwidget.h" // 基于时间的控件基类 (MulticamWidget 的父类)

// Qt 类的前向声明 (根据用户要求，不添加)
// class QWidget;
// class QPoint;

// 项目内部类的前向声明 (根据用户要求，不添加)
// class ClipBlock; // 时间线上的剪辑块类
// class rational;  // 有理数类，用于时间表示

namespace olive {

/**
 * @brief MulticamWidget 类是一个用于多机位编辑的用户界面控件。
 *
 * 它继承自 TimeBasedWidget，管理一个 MulticamDisplay 来展示来自 MultiCamNode 的多个视频源。
 * 用户可以通过点击显示区域来切换机位，这个操作可能会影响到时间线上的 ClipBlock。
 * 该控件还处理与播放时间同步的机位切换逻辑。
 */
class MulticamWidget : public TimeBasedWidget {
  Q_OBJECT // Qt 元对象系统宏，用于支持信号和槽机制

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit MulticamWidget(QWidget *parent = nullptr);

  /**
   * @brief 获取内部的 MulticamDisplay 显示控件。
   * @return 指向 MulticamDisplay 控件的指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] MulticamDisplay *GetDisplayWidget() const { return display_; }

  /**
   * @brief 设置当前的多机位节点、关联的查看器输出、目标剪辑以及当前时间。
   *
   * 这个函数初始化或更新多机位编辑会话所需的所有上下文信息。
   * @param viewer 指向 ViewerOutput 的指针，表示多机位画面将显示在哪个查看器上。
   * @param n 指向 MultiCamNode 的指针，提供多路视频源。
   * @param clip 指向 ClipBlock 的指针，表示在时间线上与此多机位编辑相关联的剪辑。
   * @param time 当前的时间点 (rational 类型)。
   */
  void SetMulticamNode(ViewerOutput *viewer, MultiCamNode *n, ClipBlock *clip, const rational &time);

 protected:
  /**
   * @brief 当此控件连接到一个新的节点（通常是 ViewerOutput 所代表的序列或剪辑）时调用的事件处理函数。
   * @param n 指向新连接的 ViewerOutput 对象的指针。
   */
  void ConnectNodeEvent(ViewerOutput *n) override;
  /**
   * @brief 当此控件与当前节点断开连接时调用的事件处理函数。
   * @param n 指向被断开连接的 ViewerOutput 对象的指针。
   */
  void DisconnectNodeEvent(ViewerOutput *n) override;
  /**
   * @brief 当关联的时间目标（例如播放头）的时间发生改变时调用的事件处理函数。
   * @param t 新的当前时间 (rational 类型)。
   */
  void TimeChangedEvent(const rational &t) override;

 signals:
  /**
   * @brief 当用户通过此控件切换了机位源时发出此信号。
   */
  void Switched();

 private:
  /**
   * @brief 内部函数，用于设置多机位节点、查看器和剪辑。
   * @param viewer 查看器输出指针。
   * @param n 多机位节点指针。
   * @param clip 剪辑块指针。
   */
  void SetMulticamNodeInternal(ViewerOutput *viewer, MultiCamNode *n, ClipBlock *clip);

  /**
   * @brief 执行机位切换的逻辑。
   * @param source 要切换到的源的索引。
   * @param split_clip 布尔值，指示切换时是否需要在当前剪辑上创建一个分割点。
   */
  void Switch(int source, bool split_clip);

  ViewerSizer *sizer_; ///< 用于管理 MulticamDisplay 尺寸和布局的辅助对象。

  MulticamDisplay *display_; ///< 内部的 MulticamDisplay 控件实例，用于实际显示多机位画面。

  MultiCamNode *node_; ///< 指向当前活动的多机位数据节点 (MultiCamNode) 的指针。

  ClipBlock *clip_; ///< 指向当前正在进行多机位编辑的目标剪辑块 (ClipBlock) 的指针。

  /**
   * @brief MulticamNodeQueue 结构体用于在播放队列中存储待处理的多机位节点设置请求。
   *
   * 这允许在播放过程中，当播放头到达特定时间点时，自动切换到相应的多机位设置。
   */
  struct MulticamNodeQueue {
    rational time;          ///< 此设置生效的时间点。
    ViewerOutput *viewer;   ///< 关联的查看器输出。
    MultiCamNode *node;     ///< 要切换到的多机位节点。
    ClipBlock *clip;        ///< 关联的剪辑块。
  };

  std::list<MulticamNodeQueue> play_queue_; ///< 存储待处理的多机位节点设置请求的队列。

 private slots:
  /**
   * @brief 当用户点击 MulticamDisplay 显示区域时调用的槽函数。
   *
   * 此函数会根据点击位置计算用户意图切换到的机位源，并调用 Switch() 方法执行切换。
   * @param p 用户在 MulticamDisplay 控件坐标系中的点击位置。
   */
  void DisplayClicked(const QPoint &p);
};

}  // namespace olive

#endif  // MULTICAMWIDGET_H
