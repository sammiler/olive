#ifndef COLORSPACECHOOSER_H
#define COLORSPACECHOOSER_H

#include <QComboBox>  // Qt 组合框控件
#include <QGroupBox>  // Qt 分组框控件

#include "node/color/colormanager/colormanager.h"  // 色彩管理器类

// 前向声明 Qt 类
class QWidget;
class QString;

namespace olive {

/**
 * @brief ColorSpaceChooser 类是一个用于选择色彩空间相关设置的控件。
 *
 * 它继承自 QGroupBox，内部包含多个 QComboBox，允许用户选择输入色彩空间、
 * 显示设备、视图转换 (view transform) 和外观 (look)。
 * 这些选项通常由 ColorManager 提供，并且当选项改变时会发出相应的信号。
 */
class ColorSpaceChooser : public QGroupBox {
 Q_OBJECT  // Qt 元对象系统宏，用于支持信号和槽机制

     public :
     /**
      * @brief 构造函数。
      * @param color_manager 指向色彩管理器的指针，用于获取可用的色彩空间、显示、视图和外观。
      * @param enable_input_field 布尔值，指示是否启用输入色彩空间选择字段。默认为 true。
      * @param enable_display_fields 布尔值，指示是否启用显示设备、视图和外观选择字段。默认为 true。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit ColorSpaceChooser(ColorManager* color_manager, bool enable_input_field = true,
                                bool enable_display_fields = true, QWidget* parent = nullptr);

  /**
   * @brief 获取当前选定的输入色彩空间名称。
   * @return 返回输入色彩空间的 QString 名称。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QString input() const;

  /**
   * @brief 获取当前选定的输出色彩转换（包括显示、视图和外观）。
   * @return 返回一个 ColorTransform 对象，描述了从场景参考空间到显示空间的完整转换。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] ColorTransform output() const;

  /**
   * @brief 设置输入色彩空间。
   * @param s 要设置的输入色彩空间名称。
   */
  void set_input(const QString& s);

  /**
   * @brief 设置输出色彩转换。
   * @param out 要设置的 ColorTransform 对象。
   */
  void set_output(const ColorTransform& out);

 signals:
  /**
   * @brief 当输入色彩空间选择发生改变时发出此信号。
   * @param input 新选定的输入色彩空间名称。
   */
  void InputColorSpaceChanged(const QString& input);

  /**
   * @brief 当输出色彩空间相关的任一设置（显示、视图、外观）发生改变时发出此信号。
   * @param out 描述新输出色彩转换的 ColorTransform 对象。
   */
  void OutputColorSpaceChanged(const ColorTransform& out);

  /**
   * @brief 当输入或输出色彩空间相关的任一设置发生改变时发出此信号。
   * @param input 当前选定的输入色彩空间名称。
   * @param out 描述当前输出色彩转换的 ColorTransform 对象。
   */
  void ColorSpaceChanged(const QString& input, const ColorTransform& out);

 private slots:
  /**
   * @brief 当显示设备 (display) 的选择发生变化时，更新视图转换 (view) 和外观 (look) 组合框中的可用选项。
   * @param display 新选定的显示设备名称。
   */
  void UpdateViews(const QString& display);

 private:
  ColorManager* color_manager_;  ///< 指向色彩管理器的指针，用于查询和应用色彩空间信息。

  QComboBox* input_combobox_;    ///< 用于选择输入色彩空间的组合框。
  QComboBox* display_combobox_;  ///< 用于选择显示设备的组合框。
  QComboBox* view_combobox_;     ///< 用于选择视图转换的组合框。
  QComboBox* look_combobox_;     ///< 用于选择外观 (look) 的组合框。

 private slots:
  /**
   * @brief 当任何一个组合框的当前选项发生改变时调用的槽函数。
   *
   * 此函数负责收集所有组合框的当前状态，并发出相应的
   * InputColorSpaceChanged、OutputColorSpaceChanged 和 ColorSpaceChanged 信号。
   */
  void ComboBoxChanged();
};

}  // namespace olive

#endif  // COLORSPACECHOOSER_H
