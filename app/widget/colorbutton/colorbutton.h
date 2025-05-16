#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QPushButton> // Qt 按钮控件基类

#include "node/color/colormanager/colormanager.h" // 色彩管理器类
#include "render/managedcolor.h"                   // 带色彩空间管理的颜色类

namespace olive {

/**
 * @brief ColorButton 类是一个专门用于选择和显示颜色的按钮。
 *
 * 它继承自 QPushButton，通常用于打开一个颜色选择对话框，
 * 并在按钮本身上显示当前选定的颜色。
 * 它与 ColorManager 和 ManagedColor 类交互，以支持色彩管理。
 */
class ColorButton : public QPushButton {
  Q_OBJECT // Qt 元对象系统宏，用于支持信号和槽机制

 public:
  /**
   * @brief 构造函数。
   * @param color_manager 指向色彩管理器的指针。
   * @param show_dialog_on_click 布尔值，指示单击按钮时是否显示颜色对话框。
   * @param parent 父控件指针，默认为 nullptr。
   */
  ColorButton(ColorManager* color_manager, bool show_dialog_on_click, QWidget* parent = nullptr);

  /**
   * @brief 构造函数的重载版本，默认在单击时显示颜色对话框。
   * @param color_manager 指向色彩管理器的指针。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit ColorButton(ColorManager* color_manager, QWidget* parent = nullptr)
      : ColorButton(color_manager, true, parent) {} // 委托给主构造函数

  /**
   * @brief 获取当前按钮表示的颜色。
   * @return 返回一个 const 引用，指向当前选定的 ManagedColor 对象。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const ManagedColor& GetColor() const;

 public slots:
  /**
   * @brief 设置按钮的颜色。
   *
   * 此槽函数会更新按钮内部存储的颜色，并触发按钮外观的更新
   * 以及 ColorChanged 信号的发射。
   * @param c 要设置的 ManagedColor 对象。
   */
  void SetColor(const ManagedColor& c);

 signals:
  /**
   * @brief 当按钮的颜色发生改变时发出此信号。
   * @param c 新选定的 ManagedColor 对象。
   */
  void ColorChanged(const ManagedColor& c);

 private slots:
  /**
   * @brief 显示颜色选择对话框的槽函数。
   *
   * 通常在按钮被点击时调用此函数（如果构造时允许）。
   */
  void ShowColorDialog();

  /**
   * @brief 颜色选择对话框关闭后的处理槽函数。
   * @param e 对话框的退出代码 (例如，QDialog::Accepted 或 QDialog::Rejected)。
   */
  void ColorDialogFinished(int e);

 private:
  /**
   * @brief 更新按钮的外观以反映当前选定的颜色。
   *
   * 这可能包括改变按钮的背景色或显示一个颜色样本。
   */
  void UpdateColor();

  ColorManager* color_manager_; ///< 指向色彩管理器的指针，用于处理颜色空间转换等。

  ManagedColor color_; ///< 当前按钮所表示的颜色，带有色彩空间信息。

  ColorProcessorPtr color_processor_; ///< 指向色彩处理器的智能指针，可能用于将 ManagedColor 转换为可显示的颜色。

  bool dialog_open_; ///< 标记颜色选择对话框当前是否已打开。
};

}  // namespace olive

#endif  // COLORBUTTON_H
