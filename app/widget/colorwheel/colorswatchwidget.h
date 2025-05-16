#ifndef COLORSWATCHWIDGET_H
#define COLORSWATCHWIDGET_H

#include <QOpenGLWidget>  // Qt OpenGL 控件基类 (虽然这里继承的是 QWidget，但可能与颜色渲染的上下文有关)
                          // 修正：实际继承的是 QWidget，QOpenGLWidget 不是必需的，除非子类用它。

#include "render/colorprocessor.h"  // 引入色彩处理器类

// 前向声明 Qt 类
class QMouseEvent;
class QPaintEvent;  // QPaintEvent 通常在 QWidget 子类中使用
class QPoint;

namespace olive {

/**
 * @brief ColorSwatchWidget 类是颜色样本控件的抽象基类。
 *
 * 它提供了处理鼠标事件（点击、拖动以选择颜色）的基础功能，
 * 并管理选定的颜色以及用于颜色空间转换的色彩处理器。
 * 派生类需要实现纯虚函数 GetColorFromScreenPos() 来定义
 * 如何从屏幕坐标获取具体的颜色值。
 */
class ColorSwatchWidget : public QWidget {  // 注意：这里继承的是 QWidget，不是 QOpenGLWidget
 Q_OBJECT                                   // Qt 元对象系统宏，用于支持信号和槽机制

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit ColorSwatchWidget(QWidget* parent = nullptr);

  /**
   * @brief 获取当前选定的颜色。
   * @return 返回一个 const 引用，指向当前选定的 Color 对象。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const Color& GetSelectedColor() const;

  /**
   * @brief 设置用于颜色转换的色彩处理器。
   * @param to_linear 指向用于将输入颜色转换为线性色彩空间的色彩处理器的智能指针。
   * @param to_display 指向用于将线性颜色转换为最终显示色彩空间的色彩处理器的智能指针。
   */
  void SetColorProcessor(ColorProcessorPtr to_linear, ColorProcessorPtr to_display);

 public slots:
  /**
   * @brief 设置当前选定的颜色。
   *
   * 此槽函数会更新内部选定的颜色，并触发 SelectedColorChanged 信号。
   * 颜色变化被认为是外部触发的。
   * @param c 要设置的 Color 对象。
   */
  void SetSelectedColor(const Color& c);

 signals:
  /**
   * @brief 当选定的颜色发生改变时发出此信号。
   * @param c 新选定的 Color 对象。
   */
  void SelectedColorChanged(const Color& c);

 protected:
  /**
   * @brief 鼠标按下事件处理函数。
   *
   * 当在控件上按下鼠标按钮时调用。
   * 它会根据点击位置获取颜色并更新选定颜色。
   * @param e 鼠标事件指针。
   */
  void mousePressEvent(QMouseEvent* e) override;

  /**
   * @brief 鼠标移动事件处理函数。
   *
   * 当在控件上按下鼠标按钮并移动时调用。
   * 它会根据当前鼠标位置获取颜色并更新选定颜色，实现拖动选择颜色的功能。
   * @param e 鼠标事件指针。
   */
  void mouseMoveEvent(QMouseEvent* e) override;

  /**
   * @brief 纯虚函数，根据屏幕坐标获取颜色。
   *
   * 派生类必须实现此函数，以定义如何从控件内的特定点获取颜色值。
   * 例如，颜色轮盘会根据角度和半径计算颜色，渐变条会根据位置插值颜色。
   * @param p 控件坐标系中的点。
   * @return 返回在点 p 处的 Color 对象。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] virtual Color GetColorFromScreenPos(const QPoint& p) const = 0;

  /**
   * @brief 当选定颜色发生改变时调用的虚函数。
   *
   * 派生类可以重写此函数以在选定颜色改变时执行特定操作。
   * @param c 新的选定颜色。
   * @param external 布尔值，指示颜色变化是否由外部调用 SetSelectedColor 触发 (true)，
   * 还是由用户在此控件内部通过鼠标操作触发 (false)。
   */
  virtual void SelectedColorChangedEvent(const Color& c, bool external);

  /**
   * @brief 获取用于 UI 选择器（例如，鼠标光标下的颜色拾取指示器）的颜色。
   * @return 返回一个 Qt::GlobalColor 枚举值，通常是对比度较高的颜色（如白色或黑色），
   * 以确保在当前选定颜色背景下的可见性。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Qt::GlobalColor GetUISelectorColor() const;

  /**
   * @brief 获取经过色彩管理转换后的颜色。
   *
   * 使用配置的色彩处理器将输入的颜色（通常来自 GetColorFromScreenPos）
   * 转换为适合内部处理或显示的颜色。
   * @param input 输入的 Color 对象。
   * @return 返回经过色彩管理转换后的 Color 对象。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Color GetManagedColor(const Color& input) const;

 private:
  /**
   * @brief 内部方法，用于设置选定的颜色并通知变化。
   * @param c 要设置的 Color 对象。
   * @param external 布尔值，指示颜色变化是否来自外部。
   */
  void SetSelectedColorInternal(const Color& c, bool external);

  Color selected_color_;  ///< 当前选定的颜色。

  ColorProcessorPtr to_linear_processor_;  ///< 指向将输入颜色转换为线性色彩空间的色彩处理器的智能指针。
  ColorProcessorPtr
      to_display_processor_;  ///< 指向将线性颜色（或参考颜色）转换为最终显示色彩空间的色彩处理器的智能指针。
};

}  // namespace olive

#endif  // COLORSWATCHWIDGET_H
