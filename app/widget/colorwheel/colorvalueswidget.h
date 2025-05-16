#ifndef COLORVALUESWIDGET_H
#define COLORVALUESWIDGET_H

#include <QCheckBox>    // Qt 复选框控件
#include <QPushButton>  // Qt 按钮控件
#include <QWidget>      // Qt 控件基类

#include "colorpreviewbox.h"                       // 自定义颜色预览框控件
#include "node/color/colormanager/colormanager.h"  // 色彩管理器类
#include "widget/slider/floatslider.h"             // 自定义浮点数滑块控件
#include "widget/slider/stringslider.h"            // 自定义字符串滑块/输入控件

// 前向声明 Qt 类
class QLabel;
class QEvent;
class QObject;

namespace olive {

/**
 * @brief ColorValuesTab 类是一个用于显示和编辑单个颜色表示（如 RGB、Hex）的控件页。
 *
 * 它包含用于红色、绿色、蓝色分量的浮点数滑块，以及一个用于十六进制颜色代码的字符串滑块。
 * 还提供了一个可选的“传统值”模式，可能会对颜色分量应用不同的缩放或解释。
 * 当颜色值发生变化时，会发出 ColorChanged 信号。
 */
class ColorValuesTab : public QWidget {
 Q_OBJECT  // Qt 元对象系统宏，用于支持信号和槽机制

     public :
     /**
      * @brief 构造函数。
      * @param with_legacy_option 布尔值，指示是否显示“传统值”复选框。默认为 false。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit ColorValuesTab(bool with_legacy_option = false, QWidget* parent = nullptr);

  /**
   * @brief 获取当前选项卡表示的颜色。
   * @return 返回一个 Color 对象。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Color GetColor() const;

  /**
   * @brief 设置当前选项卡显示的颜色。
   * @param c 要设置的 Color 对象。
   */
  void SetColor(const Color& c);

  /**
   * @brief 获取红色分量的值。
   * @return 红色分量的 double 值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] double GetRed() const;
  /**
   * @brief 获取绿色分量的值。
   * @return 绿色分量的 double 值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] double GetGreen() const;
  /**
   * @brief 获取蓝色分量的值。
   * @return 蓝色分量的 double 值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] double GetBlue() const;
  /**
   * @brief 设置红色分量的值。
   * @param r 要设置的红色分量值。
   */
  void SetRed(double r);
  /**
   * @brief 设置绿色分量的值。
   * @param g 要设置的绿色分量值。
   */
  void SetGreen(double g);
  /**
   * @brief 设置蓝色分量的值。
   * @param b 要设置的蓝色分量值。
   */
  void SetBlue(double b);

 signals:
  /**
   * @brief 当此选项卡中的颜色值发生改变时发出此信号。
   * @param c 新的 Color 对象。
   */
  void ColorChanged(const Color& c);

 private:
  static const double kLegacyMultiplier;  ///< 用于“传统值”模式的乘数常量。

  /**
   * @brief 内部辅助函数，用于从指定的浮点数滑块获取值。
   * @param slider 指向 FloatSlider 控件的指针。
   * @return 滑块表示的 double 值，可能根据“传统值”模式进行了调整。
   */
  double GetValueInternal(FloatSlider* slider) const;
  /**
   * @brief 内部辅助函数，用于设置指定浮点数滑块的值。
   * @param slider 指向 FloatSlider 控件的指针。
   * @param v 要设置的 double 值，可能根据“传统值”模式进行了调整。
   */
  void SetValueInternal(FloatSlider* slider, double v);

  /**
   * @brief 检查滑块当前是否处于“传统值”显示模式。
   * @return 如果启用了“传统值”并且复选框被选中，则返回 true；否则返回 false。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool AreSlidersLegacyValues() const;

  /**
   * @brief 创建并配置一个新的颜色分量滑块。
   * @return 指向新创建的 FloatSlider 控件的指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] FloatSlider* CreateColorSlider() const;

  FloatSlider* red_slider_;    ///< 控制红色分量的滑块。
  FloatSlider* green_slider_;  ///< 控制绿色分量的滑块。
  FloatSlider* blue_slider_;   ///< 控制蓝色分量的滑块。

  QLabel* hex_lbl_;           ///< 显示 "Hex" 标签的 QLabel。
  StringSlider* hex_slider_;  ///< 用于输入和显示十六进制颜色代码的 StringSlider。

  QVector<FloatSlider*> sliders_;  ///< 存储所有颜色分量滑块的 QVector，方便统一处理。

  QCheckBox* legacy_box_;  ///< 用于切换“传统值”模式的复选框。

 private slots:
  /**
   * @brief 当任何一个颜色分量滑块的值发生改变时调用的槽函数。
   *
   * 此函数会更新十六进制显示，并发出 ColorChanged 信号。
   */
  void SliderChanged();

  /**
   * @brief 当“传统值”复选框的状态改变时调用的槽函数。
   * @param legacy 复选框的新状态 (true 表示选中，false 表示未选中)。
   */
  void LegacyChanged(bool legacy);

  /**
   * @brief 根据当前 RGB 滑块的值更新十六进制颜色代码的显示。
   */
  void UpdateHex();

  /**
   * @brief 当十六进制颜色代码输入框的值发生改变时调用的槽函数。
   *
   * 此函数会尝试解析输入的十六进制字符串，并相应地更新 RGB 滑块的值。
   * @param s 新的十六进制颜色字符串。
   */
  void HexChanged(const QString& s);
};

/**
 * @brief ColorValuesWidget 类是一个更全面的颜色编辑控件。
 *
 * 它通常包含多个 ColorValuesTab 实例，用于在不同的色彩空间
 * （如输入、参考、显示）中表示和编辑颜色。
 * 还包含一个颜色预览框 (ColorPreviewBox) 和一个颜色拾取器按钮。
 * 通过 ColorManager 和 ColorProcessor 实现颜色空间之间的转换。
 */
class ColorValuesWidget : public QWidget {
 Q_OBJECT  // Qt 元对象系统宏，用于支持信号和槽机制

     public :
     /**
      * @brief 构造函数。
      * @param manager 指向色彩管理器的指针。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit ColorValuesWidget(ColorManager* manager, QWidget* parent = nullptr);

  /**
   * @brief 获取当前控件表示的颜色（通常是在参考色彩空间中）。
   * @return 返回一个 Color 对象。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Color GetColor() const;

  /**
   * @brief 设置用于在不同色彩空间（输入、参考、显示）之间转换的色彩处理器。
   * @param input_to_ref 从输入空间到参考空间的转换器。
   * @param ref_to_display 从参考空间到显示空间的转换器。
   * @param display_to_ref 从显示空间到参考空间的转换器。
   * @param ref_to_input 从参考空间到输入空间的转换器。
   */
  void SetColorProcessor(ColorProcessorPtr input_to_ref, ColorProcessorPtr ref_to_display,
                         ColorProcessorPtr display_to_ref, ColorProcessorPtr ref_to_input);

  /**
   * @brief 事件过滤器，主要用于实现颜色拾取器的全局鼠标事件捕获。
   * @param watcher 被观察的对象。
   * @param event 发生的事件。
   * @return 如果事件被处理则返回 true，否则返回 false。
   */
  bool eventFilter(QObject* watcher, QEvent* event) override;

  /**
   * @brief 添加一个在颜色拾取模式下应被忽略的控件。
   *
   * 当颜色拾取器激活时，点击这些被忽略的控件不会触发颜色拾取。
   * @param w 要忽略的控件的指针。
   */
  void IgnorePickFrom(QWidget* w) { ignore_pick_from_.append(w); }

 public slots:
  /**
   * @brief 设置控件的颜色。
   *
   * 通常是设置参考色彩空间中的颜色，并相应更新输入和显示选项卡。
   * @param c 要设置的 Color 对象。
   */
  void SetColor(const Color& c);

  /**
   * @brief 设置参考颜色，并更新其他相关的颜色表示。
   * @param c 要设置的参考 Color 对象。
   */
  void SetReferenceColor(const Color& c);

 signals:
  /**
   * @brief 当颜色（通常是参考颜色）发生改变时发出此信号。
   * @param c 新的 Color 对象。
   */
  void ColorChanged(const Color& c);

 private:
  /**
   * @brief 根据参考颜色更新输入颜色选项卡。
   */
  void UpdateInputFromRef();

  /**
   * @brief 根据参考颜色更新显示颜色选项卡。
   */
  void UpdateDisplayFromRef();

  /**
   * @brief 根据输入颜色更新参考颜色选项卡。
   */
  void UpdateRefFromInput();

  /**
   * @brief 根据显示颜色更新参考颜色选项卡。
   */
  void UpdateRefFromDisplay();

  ColorManager* manager_;  ///< 指向色彩管理器的指针。

  ColorPreviewBox* preview_;  ///< 用于显示当前颜色的预览框。

  ColorValuesTab* input_tab_;      ///< 用于显示和编辑输入色彩空间中颜色的选项卡。
  ColorValuesTab* reference_tab_;  ///< 用于显示和编辑参考色彩空间中颜色的选项卡。
  ColorValuesTab* display_tab_;    ///< 用于显示和编辑显示色彩空间中颜色的选项卡。

  ColorProcessorPtr input_to_ref_;    ///< 从输入空间到参考空间的色彩转换器。
  ColorProcessorPtr ref_to_display_;  ///< 从参考空间到显示空间的色彩转换器。
  ColorProcessorPtr display_to_ref_;  ///< 从显示空间到参考空间的色彩转换器。
  ColorProcessorPtr ref_to_input_;    ///< 从参考空间到输入空间的色彩转换器。

  QPushButton* color_picker_btn_;  ///< 颜色拾取器按钮。

  Color picker_end_color_;  ///< 存储颜色拾取操作结束时的颜色。

  QVector<QWidget*> ignore_pick_from_;  ///< 在颜色拾取模式下应忽略的控件列表。

 private slots:
  /**
   * @brief 当输入选项卡中的颜色值改变时调用的槽函数。
   */
  void UpdateValuesFromInput();

  /**
   * @brief 当参考选项卡中的颜色值改变时调用的槽函数。
   */
  void UpdateValuesFromRef();

  /**
   * @brief 当显示选项卡中的颜色值改变时调用的槽函数。
   */
  void UpdateValuesFromDisplay();

  /**
   * @brief 当颜色拾取器按钮的勾选状态改变时调用的槽函数。
   * @param e 按钮的勾选状态 (true 表示激活拾取，false 表示未激活)。
   */
  void ColorPickedBtnToggled(bool e);
};

}  // namespace olive

#endif  // COLORVALUESWIDGET_H
