#ifndef PREFERENCESKEYBOARDTAB_H
#define PREFERENCESKEYBOARDTAB_H

#include <QMenuBar>     // 引入 QMenuBar 类，用于访问主窗口的菜单栏
#include <QTreeWidget>  // 引入 QTreeWidget 类，用于创建树形列表控件

#include "../keysequenceeditor.h"                // 引入自定义的键盘序列编辑器控件
#include "dialog/configbase/configdialogbase.h"  // 引入配置对话框基类相关定义

namespace olive {

// 向前声明 MainWindow 类，以避免头文件循环依赖
class MainWindow;

/**
 * @class PreferencesKeyboardTab
 * @brief “首选项”对话框中的“键盘快捷键”选项卡类。
 *
 * 继承自 ConfigDialogBaseTab，用于管理和自定义应用程序的键盘快捷键。
 * 它允许用户查看、修改、加载、保存和重置快捷键设置。
 */
class PreferencesKeyboardTab : public ConfigDialogBaseTab {
  Q_OBJECT
 public:
  /**
   * @brief PreferencesKeyboardTab 构造函数。
   *
   * @param main_window 指向主窗口对象的指针，用于访问菜单栏以获取和设置快捷键。
   */
  explicit PreferencesKeyboardTab(MainWindow* main_window);

  /**
   * @brief 应用当前选项卡中的更改。
   *
   * @param command 指向 MultiUndoCommand 对象的指针，用于记录撤销/重做操作。
   * 此函数会覆盖基类中的同名虚函数。它会将用户在界面上修改的快捷键应用到相应的 QAction。
   */
  void Accept(MultiUndoCommand* command) override;

 private slots:
  /**
   * @brief 槽函数：显示文件对话框以从文件加载外部快捷键预设。
   */
  void load_shortcut_file();

  /**
   * @brief 槽函数：显示文件对话框以将当前快捷键预设保存到文件。
   */
  void save_shortcut_file();

  /**
   * @brief 槽函数：将 keyboard_tree_ 中所有选定的快捷键重置为其默认值。
   */
  void reset_default_shortcut();

  /**
   * @brief 槽函数：不加区分地将所有快捷键重置为其默认值。
   *
   * 直接调用此函数是安全的，因为它会在重置前询问用户是否希望这样做。
   */
  void reset_all_shortcuts();

  /**
   * @brief 槽函数：根据快捷键查询显示/隐藏快捷键条目。
   *
   * 为简单起见，此函数可以直接连接到 QLineEdit::textChanged() 信号。
   *
   * @param s 用于与快捷键名称进行比较的搜索查询字符串。
   * @param parent 此参数用于函数递归调用以遍历菜单项层级结构。从外部调用时，应将其保留为 nullptr。
   * @return 返回值用于函数递归调用，以确定菜单父项是否有任何未隐藏的子项。
   * 如果有，则返回 TRUE，这样父项也会显示（即使它不匹配搜索查询）。
   * 如果没有，则返回 FALSE，这样父项将被隐藏。
   */
  bool refine_shortcut_list(const QString& s, QTreeWidgetItem* parent = nullptr);

 private:
  /**
   * @brief 使用菜单栏中的键盘快捷键填充键盘快捷键面板。
   *
   * @param menu 对主应用程序菜单栏的引用。通常是 MainWindow::menuBar()。
   */
  void setup_kbd_shortcuts(QMenuBar* menu);

  /**
   * @brief 由 setup_kbd_shortcuts() 调用的内部函数，用于向下遍历菜单栏的层级结构并填充快捷键面板。
   *
   * 当发现提供的菜单包含子菜单时，此函数将递归调用自身。
   * 它还会在提供的父项下创建 QTreeWidgetItem，这些子项既可以用作子菜单的父项，
   * 也可以附加一个 KeySequenceEditor 用于快捷键编辑。
   *
   * @param menu 要向下遍历的当前菜单。
   * @param parent 要向其添加 QTreeWidgetItem 的父项。
   */
  void setup_kbd_shortcut_worker(QMenu* menu, QTreeWidgetItem* parent);

  /**
   * @brief 用于编辑键盘快捷键的树形列表控件。
   */
  QTreeWidget* keyboard_tree_;

  /**
   * @brief 可触发的键盘快捷键动作列表 (与 key_shortcut_items_ 和 key_shortcut_fields_ 相关联)。
   */
  QVector<QAction*> key_shortcut_actions_;

  /**
   * @brief keyboard_tree_ 中与现有动作对应的键盘快捷键项目列表 (与 key_shortcut_actions_ 和 key_shortcut_fields_
   * 相关联)。
   */
  QVector<QTreeWidgetItem*> key_shortcut_items_;

  /**
   * @brief keyboard_tree_ 中与现有动作对应的键盘快捷键编辑字段列表 (与 key_shortcut_actions_ 和 key_shortcut_items_
   * 相关联)。
   */
  QVector<KeySequenceEditor*> key_shortcut_fields_;

  /**
   * @brief 指向主窗口对象的指针。
   */
  MainWindow* main_window_;
};

}  // namespace olive

#endif  // PREFERENCESKEYBOARDTAB_H
