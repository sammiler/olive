#ifndef SLIDERLINEEDIT_H // 文件名可能是 SLIDERLINEEDIT_H，但类名是 FocusableLineEdit
#define SLIDERLINEEDIT_H

#include <QLineEdit> // Qt 单行文本输入框控件基类

#include "common/define.h" // 项目通用定义

// 前向声明 Qt 类
class QKeyEvent;     // Qt 键盘事件类
class QFocusEvent;   // Qt 焦点事件类
class QWidget;       // Qt 控件基类 (已通过 QLineEdit 间接包含)

namespace olive {

/**
 * @brief FocusableLineEdit 类是一个可获取焦点并处理确认/取消操作的 QLineEdit。
 *
 * 它继承自 QLineEdit，并重写了键盘事件和焦点丢失事件的处理逻辑。
 * 当用户按下回车键或焦点丢失时，通常视为确认编辑，发出 Confirmed() 信号。
 * 当用户按下 Esc 键时，视为取消编辑，发出 Cancelled() 信号。
 * 这个控件常用于需要用户输入文本并明确确认或取消的场景，例如在滑块控件旁边的数值输入框。
 */
class FocusableLineEdit : public QLineEdit {
  Q_OBJECT // Qt 元对象系统宏，用于支持信号和槽机制

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit FocusableLineEdit(QWidget *parent = nullptr);

  signals:
   /**
    * @brief 当用户确认输入时（例如按下回车键或焦点移出）发出此信号。
    */
   void Confirmed();

  /**
   * @brief 当用户取消输入时（例如按下 Esc 键）发出此信号。
   */
  void Cancelled();

protected:
  /**
   * @brief 键盘按下事件处理函数。
   *
   * 捕获特定按键（如回车、Esc）以触发 Confirmed() 或 Cancelled() 信号。
   * @param event 键盘事件指针。
   */
  void keyPressEvent(QKeyEvent *event) override;

  /**
   * @brief 焦点丢失事件处理函数。
   *
   * 当控件失去焦点时，通常视为确认编辑，并发出 Confirmed() 信号。
   * @param event 焦点事件指针。
   */
  void focusOutEvent(QFocusEvent *event) override;
};

}  // namespace olive

#endif  // SLIDERLINEEDIT_H
