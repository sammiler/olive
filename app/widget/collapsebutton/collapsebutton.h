#ifndef COLLAPSEBUTTON_H
#define COLLAPSEBUTTON_H

#include <QPushButton>  // Qt 按钮控件基类

#include "common/define.h"  // 项目通用定义

namespace olive {

/**
 * @brief CollapseButton 类是一个专门用于折叠/展开功能的按钮。
 *
 * 它继承自 QPushButton，通常用于控制界面元素的显示与隐藏。
 * 按钮的图标会根据其关联的折叠状态（展开或收起）进行更新。
 * 这个按钮通常是可勾选的 (checkable)，其勾选状态 (checked state)
 * 用来表示目标区域是展开还是收起。
 */
class CollapseButton : public QPushButton {
 Q_OBJECT  // Qt 元对象系统宏，用于支持信号和槽机制

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit CollapseButton(QWidget* parent = nullptr);

 private slots:
  /**
   * @brief 更新按钮图标的槽函数。
   *
   * 当按钮的勾选状态改变时（例如，通过 clicked(bool) 信号连接到此槽），
   * 此函数会被调用，以根据新的状态更新按钮上显示的图标。
   * 通常，展开状态和收起状态会对应不同的图标（例如，箭头向上/向下）。
   * @param e 布尔值，通常代表按钮的勾选状态 (checked state)。
   * true 可能表示展开状态，false 表示收起状态，具体取决于实现。
   */
  void UpdateIcon(bool e);
};

}  // namespace olive

#endif  // COLLAPSEBUTTON_H
