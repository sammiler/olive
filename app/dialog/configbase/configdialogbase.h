#ifndef CONFIGBASE_H
#define CONFIGBASE_H

#include <QDialog>         // QDialog 基类
#include <QList>           // 为了存储 ConfigDialogBaseTab 指针的 QList
#include <QListWidget>     // 列表控件，用作选项卡选择器
#include <QStackedWidget>  // 堆叠控件，用于显示不同的配置页面
#include <QString>         // 为了选项卡标题
#include <QWidget>         // 为了 QWidget* parent 参数

// 假设 configdialogbasetab.h 声明了 ConfigDialogBaseTab 类
#include "configdialogbasetab.h"  // 配置对话框中每个选项卡的基类

namespace olive {

/**
 * @brief 配置/首选项对话框的基类。
 *
 *此类提供了一个通用的多选项卡对话框结构，常用于应用程序的“首选项”或“设置”窗口。
 * 它使用一个 QListWidget 来显示可选的配置类别（选项卡），
 * 并使用一个 QStackedWidget 来显示与当前选中类别对应的具体配置页面 (ConfigDialogBaseTab)。
 * 派生类可以通过 AddTab() 方法添加具体的配置页面。
 */
class ConfigDialogBase : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 ConfigDialogBase 对象。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit ConfigDialogBase(QWidget* parent = nullptr);

  // 默认析构函数通常足够，因为 Qt 的父子对象机制会处理内部的UI控件指针和 tabs_ 中的对象（如果它们有父对象）
  // ~ConfigDialogBase() override;

 private slots:
  /**
   * @brief 重写 QDialog::accept() 槽函数，以在用户确认对话框时保存首选项到 Config。
   *
   * 当用户点击“确定”或等效按钮时，此槽函数会被调用。
   * 它会遍历所有已添加的配置选项卡 (ConfigDialogBaseTab)，并调用它们的 `Save()` 方法，
   * 然后调用 `AcceptEvent()` 虚函数，最后才调用基类的 `QDialog::accept()`。
   */
  void accept() override;

 protected:
  /**
   * @brief 向配置对话框中添加一个新的选项卡页面。
   *
   * @param tab 指向要添加的 ConfigDialogBaseTab 派生类对象的指针。
   * 此 `tab` 控件将被添加到 `preference_pane_stack_` 中，
   * 并且其标题将作为一项添加到 `list_widget_` 中。
   * ConfigDialogBase 会管理 `tab` 的所有权 (如果它没有其他父对象)。
   * @param title 选项卡在 `list_widget_` 中显示的标题。
   */
  void AddTab(ConfigDialogBaseTab* tab, const QString& title);

  /**
   * @brief 接受事件的虚处理函数。
   *
   * 在 `accept()` 槽函数中，当所有配置选项卡的 `Save()` 方法被调用之后，
   * 并且在调用基类 `QDialog::accept()` 之前，此虚函数会被调用。
   * 派生类可以覆盖此方法以执行在配置被接受（保存）后需要进行的特定操作。
   * 默认实现为空。
   */
  virtual void AcceptEvent() {}

 private:
  /**
   * @brief 指向 QListWidget 对象的指针，用作左侧的选项卡选择列表。
   * 用户通过点击此列表中的项来切换右侧显示的配置页面。
   */
  QListWidget* list_widget_;

  /**
   * @brief 指向 QStackedWidget 对象的指针，用于容纳和显示各个配置选项卡页面。
   * list_widget_ 中的当前选中项决定了此堆叠控件中哪个页面可见。
   */
  QStackedWidget* preference_pane_stack_;

  /**
   * @brief 存储所有已添加到对话框中的 ConfigDialogBaseTab 对象的指针列表。
   * 用于在 `accept()` 时遍历并保存所有选项卡的设置。
   */
  QList<ConfigDialogBaseTab*> tabs_;
};

}  // namespace olive

#endif  // CONFIGBASE_H