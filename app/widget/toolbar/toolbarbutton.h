#ifndef TOOLBARBUTTON_H  // 防止头文件被多次包含的宏定义
#define TOOLBARBUTTON_H

#include <QPushButton>  // 引入 QPushButton 类，ToolbarButton 是 QPushButton 的派生类

#include "tool/tool.h"  // 引入 Tool 枚举 (Tool::Item) 和相关工具定义

// 根据代码上下文，QWidget 类型应由 <QPushButton> 或其传递包含的头文件提供定义。
// 此处严格按照用户提供的代码，不添加额外的 #include 或前向声明。

namespace olive {  // olive 命名空间开始

/**
 * @brief ToolbarButton 类是 QPushButton 的一个简单派生类，用于包含一个工具ID (Tool ID)。
 *
 * 它作为 Toolbar 控件中的主要按钮控件使用。
 * 每个 ToolbarButton 实例代表一个特定的工具。
 */
class ToolbarButton : public QPushButton {
 public:
  /**
   * @brief ToolbarButton 构造函数。
   *
   * @param parent
   * 父 QWidget 对象。几乎总是 Toolbar 的一个实例。
   *
   * @param tool
   * 工具对象。必须是 Tool::Item 枚举的成员，如果此按钮不代表任何工具，则可以是 kNone。
   */
  ToolbarButton(QWidget* parent, const Tool::Item& tool);

  /**
   * @brief 获取此按钮所代表的工具 ID。
   *
   * 该 ID 在构造函数中设置，并且在其生命周期内不应更改。
   * @return 对此按钮关联的 Tool::Item 枚举值的常量引用。
   */
  const Tool::Item& tool();

 private:  // 私有成员变量
  /**
   * @brief 内部存储的工具值。
   *
   * 代表此按钮关联的特定工具。
   */
  Tool::Item tool_;
};

}  // namespace olive

#endif  // TOOLBARBUTTON_H
