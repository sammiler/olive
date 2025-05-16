#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>  // Qt 标签控件基类

#include "common/define.h"  // 项目通用定义

namespace olive {

/**
 * @brief ClickableLabel 类是一个可点击的 QLabel。
 *
 * 它继承自 QLabel，并重写了鼠标事件处理函数，
 * 以便在用户单击或双击标签时发出相应的信号。
 */
class ClickableLabel : public QLabel {
 Q_OBJECT  // Qt 元对象系统宏，用于支持信号和槽机制

     public :
     /**
      * @brief 构造函数，使用指定的文本和父控件。
      * @param text 标签上显示的文本。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit ClickableLabel(const QString& text, QWidget* parent = nullptr);

  /**
   * @brief 构造函数，仅使用父控件。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit ClickableLabel(QWidget* parent = nullptr);

 protected:
  /**
   * @brief 鼠标释放事件处理函数。
   *
   * 当鼠标在标签上释放时调用此函数。
   * 它会发出 MouseClicked() 信号。
   * @param event 鼠标事件指针。
   */
  void mouseReleaseEvent(QMouseEvent* event) override;

  /**
   * @brief 鼠标双击事件处理函数。
   *
   * 当鼠标在标签上双击时调用此函数。
   * 它会发出 MouseDoubleClicked() 信号。
   * @param event 鼠标事件指针。
   */
  void mouseDoubleClickEvent(QMouseEvent* event) override;

 signals:
  /**
   * @brief 当标签被单击（鼠标释放）时发出此信号。
   */
  void MouseClicked();

  /**
   * @brief 当标签被双击时发出此信号。
   */
  void MouseDoubleClicked();
};

}  // namespace olive

#endif  // CLICKABLELABEL_H
