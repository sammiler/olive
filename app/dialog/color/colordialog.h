#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include <QDialog> // QDialog 基类
#include <QString> // Qt 字符串类
#include <QWidget> // 为了 QWidget* parent 参数

// Olive 内部头文件
#include "node/color/colormanager/colormanager.h" // 包含 ColorManager, ColorProcessorPtr
#include "render/managedcolor.h"                 // 包含 ManagedColor, Color (假设Color在此或其包含文件)
#include "render/colortransform.h"               // 包含 ColorTransform (如果它是一个独立的类/结构体)
#include "widget/colorwheel/colorgradientwidget.h" // 颜色渐变控件
#include "widget/colorwheel/colorspacechooser.h"   // 颜色空间选择器控件
#include "widget/colorwheel/colorswatchchooser.h"  // 颜色样本选择器控件
#include "widget/colorwheel/colorvalueswidget.h"   // 颜色值输入/显示控件
#include "widget/colorwheel/colorwheelwidget.h"    // 色轮控件
#include "common/define.h"                       // 可能包含 Color, ColorProcessorPtr (如果未在其他地方定义)


namespace olive {

/**
 * @brief 提供颜色选择功能的对话框类，集成了颜色管理。
 *
 * 此对话框允许用户通过多种方式选择颜色（例如色轮、RGB/HSV值输入、颜色样本），
 * 并在选定颜色空间和显示颜色空间之间进行正确的颜色转换，
 * 所有操作均通过提供的 `ColorManager` 进行管理。
 */
class ColorDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief ColorDialog 构造函数。
   *
   * @param color_manager 指向 ColorManager 实例的指针，用于所有颜色管理操作。此指针必须有效且在对话框生命周期内保持有效。
   * @param start 初始选中的颜色。此颜色应位于 `color_manager` 的参考颜色空间中 (通常是场景线性空间)。
   * 默认为白色 (1.0f, 1.0f, 1.0f)。
   * @param parent 父 QWidget 对象指针。默认为 nullptr。
   */
  explicit ColorDialog(ColorManager* color_manager, const ManagedColor& start = ManagedColor(Color(1.0f, 1.0f, 1.0f)),
                       QWidget* parent = nullptr);
  // 默认析构函数通常足够，因为 Qt 的父子对象机制会处理内部的UI控件指针。
  // ~ColorDialog() override;

  /**
   * @brief 获取用户在对话框中最终选择的颜色。
   *
   * 返回的颜色始终位于与构造函数中传入的 `ColorManager` 关联的参考颜色空间中
   * (通常是场景线性空间)。
   * @return ManagedColor 用户选择的颜色。
   */
  [[nodiscard]] ManagedColor GetSelectedColor() const;

  /**
   * @brief 获取用户在对话框中选择的输入颜色空间的名称。
   * 这是用户查看和编辑颜色时所使用的颜色空间。
   * @return QString 输入颜色空间的名称。
   */
  [[nodiscard]] QString GetColorSpaceInput() const;

  /**
   * @brief 获取从用户选择的输入颜色空间到参考颜色空间的颜色转换。
   * (此函数名 `GetColorSpaceOutput` 可能指代UI上选择的“输出/显示”颜色空间，
   * 或者是指从“输入颜色空间(UI所用)”到“参考颜色空间(内部存储)”的转换。
   * 根据上下文，更像是后者或与UI上的某个特定“输出/显示”选择相关联的转换。)
   * 假设是获取一个用于显示或最终输出的颜色变换。
   * @return ColorTransform 描述颜色空间转换的对象。
   */
  [[nodiscard]] ColorTransform GetColorSpaceOutput() const;

 public slots:
  /**
   * @brief 以编程方式设置对话框当前显示的颜色。
   * @param start 要设置的颜色，此颜色应位于 `color_manager_` 的参考颜色空间中。
   * 对话框内部会将其转换为当前UI选择的输入颜色空间进行显示。
   */
  void SetColor(const ManagedColor& start);

 private:
  /**
   * @brief 指向应用程序的颜色管理器实例的指针。
   * 用于执行所有颜色空间转换和管理。
   */
  ColorManager* color_manager_;

  /**
   * @brief 指向色轮控件的指针。
   */
  ColorWheelWidget* color_wheel_;

  /**
   * @brief 指向显示和编辑颜色数值（如RGB, HSV）的控件的指针。
   */
  ColorValuesWidget* color_values_widget_;

  /**
   * @brief 指向HSV颜色模型中亮度/饱和度渐变条控件的指针。
   */
  ColorGradientWidget* hsv_value_gradient_;

  /**
   * @brief 指向颜色处理器对象的智能指针，用于将UI输入颜色空间转换为参考颜色空间。
   */
  ColorProcessorPtr input_to_ref_processor_;

  /**
   * @brief 指向颜色空间选择器控件（例如下拉菜单）的指针，允许用户更改UI的输入颜色空间。
   */
  ColorSpaceChooser* chooser_;

  /**
   * @brief 指向颜色样本选择器控件的指针，允许用户从预定义的颜色样本中选择颜色。
   */
  ColorSwatchChooser* swatch_;

 private slots:
  /**
   * @brief 当用户通过 `chooser_` 控件更改了UI的输入颜色空间时调用的槽函数。
   * @param input 新选择的输入颜色空间的名称。
   * @param output (此参数的含义需要参照实现，可能指从新输入空间到某个固定显示空间的转换，
   * 或从新输入空间到参考空间的转换。此处假设为与新输入空间相关的某个输出/显示转换。)
   * 假设为与新选择的输入颜色空间相关的输出或显示变换。
   */
  void ColorSpaceChanged(const QString& input, const ColorTransform& output);
};

}  // namespace olive

#endif  // COLORDIALOG_H