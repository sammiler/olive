#ifndef COLORSWATCHCHOOSER_H
#define COLORSWATCHCHOOSER_H

#include "node/color/colormanager/colormanager.h" // 色彩管理器类
#include "widget/colorbutton/colorbutton.h"       // 自定义颜色按钮控件

// 前向声明 Qt 类
class QWidget;
class QString;

namespace olive {

/**
 * @brief ColorSwatchChooser 类是一个用于选择和管理颜色样本（色板）的自定义 Qt 控件。
 *
 * 该控件通常显示一个颜色按钮网格，用户可以从中选择颜色。
 * 它还可能提供加载/保存颜色样本集、通过上下文菜单管理单个样本等功能。
 */
class ColorSwatchChooser : public QWidget {
  Q_OBJECT // Qt 元对象系统宏，用于支持信号和槽机制

 public:
  /**
   * @brief 构造函数。
   * @param manager 指向色彩管理器的指针，用于处理颜色相关的操作。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit ColorSwatchChooser(ColorManager *manager, QWidget *parent = nullptr);

 public slots:
  /**
   * @brief 设置当前活动或选中的颜色。
   *
   * 此槽函数接收一个原始 Color 对象，并将其转换为 ManagedColor 后存储。
   * @param c 要设置的 Color 对象。
   */
  void SetCurrentColor(const Color &c) { current_ = ManagedColor(c); }

 signals:
  /**
   * @brief 当用户点击某个颜色样本按钮时发出此信号。
   * @param c 被点击的颜色样本的 ManagedColor 对象。
   */
  void ColorClicked(const ManagedColor &c);

 private:
  /**
   * @brief 设置指定索引的颜色样本按钮为默认颜色。
   * @param index 颜色样本按钮的索引。
   */
  void SetDefaultColor(int index);

  /**
   * @brief 获取存储颜色样本文件的标准文件名或路径。
   * @return 返回包含文件名（可能带有路径）的 QString。
   */
  static QString GetSwatchFilename();

  /**
   * @brief 从文件加载颜色样本集。
   *
   * 此函数会读取预定义的颜色样本文件，并用加载的颜色更新控件中的按钮。
   */
  void LoadSwatches();

  /**
   * @brief 将当前的颜色样本集保存到文件。
   *
   * 此函数会将控件中所有颜色按钮的当前颜色保存到预定义的文件中。
   */
  void SaveSwatches();

  static const int kRowCount = 4; ///< 定义颜色样本按钮网格的行数。
  static const int kColCount = 8; ///< 定义颜色样本按钮网格的列数。
  static const int kBtnCount = kRowCount * kColCount; ///< 颜色样本按钮的总数。
  ColorButton *buttons_[kBtnCount]{}; ///< 存储指向网格中所有 ColorButton 实例的指针数组。初始化为 nullptr。

  ManagedColor current_;  ///< 当前选中的或活动的颜色。
  ColorButton *menu_btn_{}; ///< 可能用于触发上下文菜单的按钮，或者在上下文菜单中被操作的按钮。初始化为 nullptr。

 private slots:
  /**
   * @brief 处理颜色样本按钮点击事件的槽函数。
   *
   * 当网格中的某个 ColorButton 被点击时，此槽函数会被调用。
   * 它会将被点击按钮的颜色设置为当前颜色，并发出 ColorClicked 信号。
   */
  void HandleButtonClick();

  /**
   * @brief 处理颜色样本按钮上下文菜单请求的槽函数。
   *
   * 当用户在某个颜色样本按钮上请求上下文菜单（例如右键点击）时，
   * 此槽函数会被调用。它会显示一个包含操作选项（如保存当前颜色到此样本）的菜单。
   */
  void HandleContextMenu();

  /**
   * @brief 将当前选中的颜色 (current_) 保存到通过上下文菜单选定的样本按钮上。
   */
  void SaveCurrentColor();

  /**
   * @brief 重置（或清除）通过上下文菜单选定的样本按钮的颜色。
   */
  void ResetMenuButton();
};

}  // namespace olive

#endif  // COLORSWATCHCHOOSER_H
