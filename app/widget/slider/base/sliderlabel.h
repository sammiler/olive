#ifndef SLIDERLABEL_H
#define SLIDERLABEL_H

#include <QLabel>

#include "common/define.h"

namespace olive {

/**
 * @brief SliderLabel 类是一个自定义的 QLabel，专门用作滑块控件 (SliderBase) 中显示值的标签。
 *
 * 它不仅显示文本，还处理鼠标交互（按下、释放）和焦点事件，
 * 并能发出相应的信号。这允许用户通过点击标签来触发某些操作，
 * 例如切换到编辑模式、重置值或更改滑块类型。
 * 它还支持设置自定义的文本颜色。
 */
class SliderLabel : public QLabel {
 Q_OBJECT  // Qt 元对象系统宏，用于支持信号和槽机制

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针。
      */
     explicit SliderLabel(QWidget *parent);

  /**
   * @brief 设置标签的文本颜色。
   *
   * 这会覆盖默认的样式表颜色。
   * @param c 要设置的 QColor。
   */
  void SetColor(const QColor &c);

 protected:
  /**
   * @brief 重写 QWidget 的鼠标按下事件处理函数。
   *
   * 当用户在标签上按下鼠标按钮时，发出 LabelPressed() 信号。
   * @param e 鼠标事件指针。
   */
  void mousePressEvent(QMouseEvent *e) override;

  /**
   * @brief 重写 QWidget 的鼠标释放事件处理函数。
   *
   * 当用户在标签上释放鼠标按钮时，发出 LabelReleased() 信号。
   * @param e 鼠标事件指针。
   */
  void mouseReleaseEvent(QMouseEvent *e) override;

  /**
   * @brief 重写 QWidget 的焦点进入事件处理函数。
   *
   * 当标签获得焦点时，发出 focused() 信号。
   * @param event 焦点事件指针。
   */
  void focusInEvent(QFocusEvent *event) override;

  /**
   * @brief 重写 QWidget 的 changeEvent 事件处理函数。
   *
   * 可能用于响应 QEvent::EnabledChange 或 QEvent::PaletteChange 等事件，
   * 以更新标签的外观。
   * @param event 事件指针。
   */
  void changeEvent(QEvent *event) override;

 signals:
  /**
   * @brief 当用户在标签上按下鼠标按钮时发出此信号。
   */
  void LabelPressed();

  /**
   * @brief 当用户在标签上释放鼠标按钮时发出此信号。
   */
  void LabelReleased();

  /**
   * @brief 当标签获得键盘焦点时发出此信号。
   */
  void focused();

  /**
   * @brief 请求将关联的滑块值重置为其默认值。
   *
   * 通常在用户执行特定操作（如右键菜单中的“重置”）后发出。
   */
  void RequestReset();

  /**
   * @brief 请求更改关联滑块的类型或编辑模式。
   *
   * 例如，从数值滑块切换到曲线编辑器，或者在不同的数值输入模式间切换。
   */
  void ChangeSliderType();

 private:
  bool override_color_enabled_;  ///< 标记是否启用了自定义文本颜色覆盖。
  QColor override_color_;        ///< 存储自定义的文本颜色。
};

}  // namespace olive

#endif  // SLIDERLABEL_H
