#ifndef ACTIONSEARCH_H
#define ACTIONSEARCH_H

#include <QDialog>      // QDialog 基类
#include <QEvent>       // 为了 QEvent* 在 ActionSearchEntry::event
#include <QLineEdit>    // 单行文本输入框基类
#include <QListWidget>  // 列表控件基类
#include <QMenu>        // 菜单类
#include <QMenuBar>     // 菜单栏类
#include <QMouseEvent>  // 为了 QMouseEvent* 在 ActionSearchList::mouseDoubleClickEvent
#include <QStringList>  // Qt 字符串列表 (虽然未直接在参数或成员中使用，但可能在 .cpp 中使用)
#include <QWidget>      // QWidget 基类 (用于 parent 参数)

#include "common/define.h"  // 可能包含一些通用定义

namespace olive {

// 前向声明 ActionSearchList 类，因为 ActionSearch 类中使用了它的指针
class ActionSearchList;

/**
 * @brief “操作搜索”对话框类。
 *
 * 这是一个弹出式窗口 (通常通过“帮助 > 操作搜索”访问)，允许用户通过键入文本
 * 来搜索菜单命令，而不是在菜单栏中浏览。
 * 只要 olive::MainWindow 实例有效，就可以从任何地方创建此对话框。
 */
class ActionSearch : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief ActionSearch 构造函数。
   *
   * 创建“操作搜索”弹出对话框。
   *
   * @param parent 父 QWidget 对象指针。通常情况下，这会是 MainWindow。
   */
  explicit ActionSearch(QWidget* parent);

  // 默认析构函数通常足够，因为 Qt 的父子对象机制会处理 list_widget 的清理
  // ~ActionSearch() override;

  /**
   * @brief 设置此操作搜索对话框要使用的菜单栏。
   * ActionSearch 将从此菜单栏中提取并搜索操作。
   * @param menu_bar 指向要使用的 QMenuBar 对象的指针。
   */
  void SetMenuBar(QMenuBar* menu_bar);

 private slots:
  /**
   * @brief 根据搜索查询更新操作列表。
   *
   * 此函数根据用户输入的搜索查询，在操作列表中添加或移除操作项。
   *
   * 为了遍历菜单栏及其所有菜单和子菜单，此函数会递归调用自身。
   * 因此，它的某些参数不需要从外部设置，因为这些参数会在函数递归调用自身时自动设置。
   *
   * @param s 搜索文本。这是唯一应该从外部设置的参数。
   * @param p 当前的父级菜单层级路径字符串 (例如 "文件 > 打开最近文件")。
   * 在外部调用时，大多数情况下应将其保留为 nullptr 或空字符串。
   * `search_update()` 会在递归调用时根据需要自动填充此参数。
   * @param parent 当前要遍历的 QMenu 对象。在外部调用时，大多数情况下应将其保留为 nullptr。
   * `search_update()` 会在递归调用时根据需要自动填充此参数。
   */
  void search_update(const QString& s, const QString& p = QString(), QMenu* parent = nullptr);

  /**
   * @brief 执行当前选中的操作。
   *
   * 通常在 ActionSearchEntry 字段上按下回车键时触发。
   * 此函数将触发当前高亮显示的操作，然后关闭此弹出对话框。
   * 如果没有条目被高亮显示 (例如列表为空)，则不触发任何操作，但对话框仍会关闭。
   */
  void perform_action();

  /**
   * @brief 向上移动列表中的选中项。
   *
   * 此槽函数响应在 ActionSearchEntry 字段上按下向上箭头键。
   * 将列表中的选中项向上移动一项。如果选中项已在列表顶部，则此操作无效。
   */
  void move_selection_up();

  /**
   * @brief 向下移动列表中的选中项。
   *
   * 此槽函数响应在 ActionSearchEntry 字段上按下向下箭头键。
   * 将列表中的选中项向下移动一项。如果选中项已在列表底部，则此操作无效。
   */
  void move_selection_down();

 private:
  /**
   * @brief 显示命令列表的主要控件。
   * 是 ActionSearchList 类的一个实例。
   */
  ActionSearchList* list_widget;

  /**
   * @brief 附加到此操作搜索对话框的 QMenuBar 对象指针。
   * 用于从中提取和搜索可用的操作。
   */
  QMenuBar* menu_bar_;
};

/**
 * @brief “操作搜索列表”类。
 *
 * 这是 QListWidget 的一个简单封装，主要目的是在列表项被双击时发出一个信号。
 * ActionSearch 类会将此信号连接到一个槽函数，以触发当前选中的操作。
 */
class ActionSearchList : public QListWidget {
  Q_OBJECT
 public:
  /**
   * @brief ActionSearchList 构造函数。
   * @param parent 父 QWidget 对象指针。通常情况下，这会是 ActionSearch 对话框。
   */
  explicit ActionSearchList(QWidget* parent);

 protected:
  /**
   * @brief 重写 QListWidget 的 mouseDoubleClickEvent 事件处理函数。
   * 当列表项被双击时，此方法会发出 dbl_click() 信号。
   * @param event 指向 QMouseEvent 对象的指针，包含鼠标事件信息。
   */
  void mouseDoubleClickEvent(QMouseEvent* event) override;  // event 参数已添加

 signals:
  /**
   * @brief 当 QListWidget 中的一个列表项被双击时发出此信号。
   */
  void dbl_click();
};

/**
 * @brief “操作搜索输入框”类。
 *
 * 这是 QLineEdit 的一个简单封装，主要目的是在按下向上或向下箭头键时发出信号，
 * 以便 ActionSearch 类可以将这些信号连接到相应的槽函数，从而在结果列表中上下移动选中项。
 */
class ActionSearchEntry : public QLineEdit {
  Q_OBJECT
 public:
  /**
   * @brief ActionSearchEntry 构造函数。
   * @param parent 父 QWidget 对象指针。通常情况下，这会是 ActionSearch 对话框。
   */
  explicit ActionSearchEntry(QWidget* parent);

 protected:
  /**
   * @brief 重写 QWidget::event() 事件处理函数，以监听键盘按键事件，特别是向上和向下箭头键。
   * @param e 指向 QEvent 对象的指针，包含事件信息。
   * @return bool 如果事件被处理，则返回 true；否则返回 false，事件将传递给基类处理。
   */
  bool event(QEvent* e) override;

 signals:
  /**
   * @brief 当用户在输入框中按下向上箭头键时发出此信号。
   */
  void moveSelectionUp();

  /**
   * @brief 当用户在输入框中按下向下箭头键时发出此信号。
   */
  void moveSelectionDown();
};

}  // namespace olive

#endif  // ACTIONSEARCH_H