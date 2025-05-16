#ifndef DRAGBUTTON_H
#define DRAGBUTTON_H

#include <QMouseEvent>  // Qt 鼠标事件类 (用于 mousePressEvent, mouseMoveEvent, mouseReleaseEvent)
#include <QPushButton>  // Qt 按钮控件基类

#include "common/define.h"  // 项目通用定义

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWidget; // QPushButton 的基类 QAbstractButton 的基类

namespace olive {

/**
 * @brief DragButton 类是一个自定义的 QPushButton，它增加了拖拽起始检测功能。
 *
 * 当用户在此按钮上按下鼠标并开始移动（满足拖拽条件）时，
 * 它会发出一个 DragStarted() 信号。
 * 这可以用于实现一些拖放操作的起点，或者需要按钮在被拖动时触发特定行为的场景。
 */
class DragButton : public QPushButton {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit DragButton(QWidget* parent = nullptr);

 signals:
  /**
   * @brief 当用户在此按钮上开始拖拽操作时发出此信号。
   *
   * 拖拽开始通常定义为：鼠标左键按下后，光标移动了一小段距离。
   */
  void DragStarted();

 protected:
  /**
   * @brief 重写 QWidget 的鼠标按下事件处理函数。
   *
   * 用于记录鼠标按下的初始状态，为可能的拖拽操作做准备。
   * @param event 鼠标事件指针。
   */
  void mousePressEvent(QMouseEvent* event) override;

  /**
   * @brief 重写 QWidget 的鼠标移动事件处理函数。
   *
   * 如果鼠标左键被按下并且光标移动了（且尚未开始拖拽），
   * 则将 dragging_ 标志设置为 true，并发出 DragStarted() 信号。
   * @param event 鼠标事件指针。
   */
  void mouseMoveEvent(QMouseEvent* event) override;

  /**
   * @brief 重写 QWidget 的鼠标释放事件处理函数。
   *
   * 当鼠标按钮被释放时，重置 dragging_ 标志。
   * @param event 鼠标事件指针。
   */
  void mouseReleaseEvent(QMouseEvent* event) override;

 private:
  bool dragging_;  ///< 标记当前是否正在进行拖拽操作。
};

}  // namespace olive

#endif  // DRAGBUTTON_H
