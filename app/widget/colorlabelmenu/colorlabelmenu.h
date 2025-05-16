#ifndef COLORLABELMENU_H
#define COLORLABELMENU_H

#include "widget/menu/menu.h" // 引入自定义的 Menu 基类

// 前向声明 Qt 类，如果 Menu.h 中没有包含的话
class QEvent;
class QAction;

namespace olive {

/**
 * @brief ColorLabelMenu 类是一个用于选择颜色标签的自定义菜单。
 *
 * 它继承自项目自定义的 Menu 类，并提供了一系列颜色选项供用户选择。
 * 当用户选择一个颜色标签时，会发出 ColorSelected 信号，并附带该颜色的索引。
 * 此菜单还支持在应用程序语言更改时重新翻译其内容。
 */
class ColorLabelMenu : public Menu {
  Q_OBJECT // Qt 元对象系统宏，用于支持信号和槽机制

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit ColorLabelMenu(QWidget* parent = nullptr);

  /**
   * @brief 处理 Qt 的 changeEvent 事件。
   *
   * 主要用于响应 QEvent::LanguageChange 事件，以便在应用程序语言设置更改时
   * 调用 Retranslate() 方法来更新菜单项的文本。
   * @param event 指向 QEvent 对象的指针。
   */
  void changeEvent(QEvent* event) override;

  signals:
   /**
    * @brief 当用户从菜单中选择一个颜色标签时发出此信号。
    * @param i 选定颜色标签的索引。
    */
   void ColorSelected(int i);

private:
  /**
   * @brief 重新翻译菜单项的文本。
   *
   * 此方法通常在构造函数中首次调用以初始化菜单项文本，
   * 并在接收到 QEvent::LanguageChange 事件时再次调用，
   * 以确保菜单项的显示语言与应用程序当前的语言设置一致。
   */
  void Retranslate();

  QVector<QAction*> color_items_; ///< 存储菜单中各个颜色标签对应的 QAction 对象。

private slots:
 /**
  * @brief 当菜单中的某个 QAction被触发（即用户点击了某个颜色选项）时调用的槽函数。
  *
  * 此槽函数会确定是哪个颜色选项被选中，并发出 ColorSelected 信号，
  * 传递所选颜色标签的索引。
  */
 void ActionTriggered();
};

}  // namespace olive

#endif  // COLORLABELMENU_H
