#ifndef TRACKVIEWSPLITTER_H  // 防止头文件被多次包含的宏定义
#define TRACKVIEWSPLITTER_H

#include <QSplitter>  // 引入 QSplitter 类，是 TrackViewSplitter 和 TrackViewSplitterHandle 的基类或相关类

#include "common/define.h"  // 引入项目通用的定义，可能包含宏、枚举或类型别名

namespace olive {  // olive 命名空间开始

class TrackViewSplitter;  // 前向声明 TrackViewSplitter 类，因为 TrackViewSplitterHandle 的构造函数需要它

/**
 * @brief TrackViewSplitterHandle 类是 QSplitterHandle 的一个自定义子类。
 *
 * 它用于 TrackViewSplitter 中，以提供自定义的分割条拖动行为和外观。
 * 这个类重写了鼠标事件和绘制事件，以实现特定的交互和视觉效果。
 */
class TrackViewSplitterHandle : public QSplitterHandle {
 Q_OBJECT  // Q_OBJECT 宏，用于启用 Qt 的元对象特性

     public :
     /**
      * @brief 构造一个 TrackViewSplitterHandle 对象。
      * @param orientation 分割条的方向 (Qt::Orientation)，例如 Qt::Horizontal 或 Qt::Vertical。
      * @param parent 指向其所属的 QSplitter (在此场景下是 TrackViewSplitter) 的指针。
      */
     TrackViewSplitterHandle(Qt::Orientation orientation, QSplitter *parent);

 protected:  // 受保护方法，通常是事件处理器
  /**
   * @brief 重写 QWidget::mousePressEvent()，处理鼠标按下事件。
   *
   * 当用户在此分割条句柄上按下鼠标按钮时调用。
   * @param e 指向 QMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void mousePressEvent(QMouseEvent *e) override;
  /**
   * @brief 重写 QWidget::mouseMoveEvent()，处理鼠标移动事件。
   *
   * 当用户在此分割条句柄上按下鼠标按钮并移动鼠标时调用。
   * @param e 指向 QMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void mouseMoveEvent(QMouseEvent *e) override;
  /**
   * @brief 重写 QWidget::mouseReleaseEvent()，处理鼠标释放事件。
   *
   * 当用户在此分割条句柄上释放鼠标按钮时调用。
   * @param e 指向 QMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void mouseReleaseEvent(QMouseEvent *e) override;

  /**
   * @brief 重写 QWidget::paintEvent()，处理绘制事件。
   *
   * 当分割条句柄需要重绘时调用，用于自定义其外观。
   * @param e 指向 QPaintEvent 对象的指针，包含绘制事件的详细信息。
   */
  void paintEvent(QPaintEvent *e) override;

 private:         // 私有成员变量
  int drag_y_{};  ///< 存储拖动开始时的鼠标 Y 坐标，或拖动过程中的相对 Y 坐标。

  bool dragging_;  ///< 标记当前是否正在拖动此分割条句柄。
};

/**
 * @brief TrackViewSplitter 类是 QSplitter 的一个自定义子类，专门用于 TrackView。
 *
 * 它使用自定义的 TrackViewSplitterHandle 来提供特定的分割条行为和外观。
 * 这个类管理一组垂直排列的 QWidget (通常是 TrackViewItem)，并允许用户调整它们之间的高度。
 */
class TrackViewSplitter : public QSplitter {
 Q_OBJECT  // Q_OBJECT 宏，用于启用 Qt 的元对象特性，如信号和槽

     public :
     /**
      * @brief 构造一个 TrackViewSplitter 对象。
      * @param vertical_alignment 子控件在垂直方向上的对齐方式 (Qt::Alignment)。
      * @param parent 父 QWidget 对象，默认为 nullptr。
      */
     explicit TrackViewSplitter(Qt::Alignment vertical_alignment, QWidget *parent = nullptr);

  /**
   * @brief 处理来自 TrackViewSplitterHandle 的拖动事件。
   *
   * 当用户拖动一个分割条句柄时，句柄会调用此方法来通知 TrackViewSplitter 调整相应子控件的大小。
   * @param h 指向被拖动的 TrackViewSplitterHandle 对象的指针。
   * @param diff 鼠标在 Y 轴方向上移动的差值。
   */
  void HandleReceiver(TrackViewSplitterHandle *h, int diff);

  /**
   * @brief 根据提供的大小列表设置所有子控件的高度。
   * @param sizes 一个包含每个子控件目标高度的 QList<int>。
   */
  void SetHeightWithSizes(QList<int> sizes);

  /**
   * @brief 在指定的索引位置插入一个新的子控件，并设置其初始高度。
   * @param index 要插入子控件的索引位置。
   * @param height 新子控件的初始高度。
   * @param item 指向要插入的 QWidget 子控件的指针。
   */
  void Insert(int index, int height, QWidget *item);
  /**
   * @brief 移除指定索引位置的子控件。
   * @param index 要移除的子控件的索引位置。
   */
  void Remove(int index);

  /**
   * @brief 设置分割条底部空白区域的高度。
   * @param height 空白区域的高度。
   */
  void SetSpacerHeight(int height);

 public slots:  // 公共槽函数
  /**
   * @brief 设置指定索引位置的轨道（子控件）的高度。
   * @param index 目标轨道的索引。
   * @param h 轨道的新高度。
   */
  void SetTrackHeight(int index, int h);

 signals:  // 信号
  /**
   * @brief 当某个轨道的视图项高度发生变化时发出此信号。
   * @param index 发生高度变化的轨道的索引。
   * @param height 轨道视图项的新高度。
   */
  void TrackHeightChanged(int index, int height);

 protected:  // 受保护方法
  /**
   * @brief 重写 QSplitter::createHandle()，用于创建自定义的分割条句柄。
   * @return 指向新创建的 QSplitterHandle (在此情况下是 TrackViewSplitterHandle) 对象的指针。
   */
  QSplitterHandle *createHandle() override;

 private:                    // 私有成员变量
  Qt::Alignment alignment_;  ///< 存储子控件在垂直方向上的对齐方式。

  int spacer_height_;  ///< 分割条底部空白区域的高度。
};

}  // namespace olive

#endif  // TRACKVIEWSPLITTER_H
