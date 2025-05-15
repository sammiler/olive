#ifndef AUTORECOVERYDIALOG_H
#define AUTORECOVERYDIALOG_H

#include <QDialog>        // QDialog 基类
#include <QTreeWidget>    // 树形控件，用于显示可恢复的文件
#include <QTreeWidgetItem> // 树形控件项 (如果 PopulateTree 中使用了)
#include <QStringList>    // Qt 字符串列表
#include <QWidget>        // 为了 QWidget* parent 参数

#include "common/define.h" // 可能包含一些通用定义

namespace olive {

/**
 * @brief “自动恢复”对话框类。
 *
 * 当应用程序在非正常关闭（例如崩溃）后重新启动时，
 * 此对话框会显示可供恢复的自动保存文件列表，允许用户选择
 * 要恢复的项目或文件。
 */
class AutoRecoveryDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 AutoRecoveryDialog 对象。
   * @param message 对话框顶部显示的介绍性消息或说明文本。
   * @param recoveries 包含可恢复文件路径的字符串列表。
   * @param autocheck_latest 如果为 true，则自动勾选列表中最新的恢复文件。
   * @param parent 父 QWidget 对象指针。
   */
  AutoRecoveryDialog(const QString& message, const QStringList& recoveries, bool autocheck_latest, QWidget* parent);

  // 默认析构函数通常足够，因为 Qt 的父子对象机制会处理 tree_widget_ (如果它有父对象)
  // ~AutoRecoveryDialog() override;

 public slots:
  /**
   * @brief 重写 QDialog::accept() 槽函数。
   * 当用户点击“确定”或等效按钮以确认恢复选择时调用。
   * 此函数通常会处理用户在 `tree_widget_` 中勾选的项目，并尝试恢复它们。
   */
  void accept() override;

 private:
  /**
   * @brief 初始化对话框的UI元素，例如设置窗口标题和布局。
   * @param header_text 对话框中树形控件上方的说明性标题文本。
   */
  void Init(const QString& header_text);

  /**
   * @brief 使用可恢复的文件列表填充树形控件 (tree_widget_)。
   * @param recoveries 包含可恢复文件路径的字符串列表。
   * @param autocheck 如果为 true，则自动勾选最新的恢复文件项。
   */
  void PopulateTree(const QStringList& recoveries, bool autocheck);

  /**
   * @brief 指向 QTreeWidget 对象的指针，用于显示可恢复文件列表。
   * 使用 `{}` 进行值初始化，确保在构造时为 nullptr 或默认状态。
   */
  QTreeWidget* tree_widget_{};

  /**
   * @brief 存储树形控件中所有可勾选的恢复项 (QTreeWidgetItem) 的 QVector。
   * 用于在 `accept()` 时快速访问用户选择的项。
   */
  QVector<QTreeWidgetItem*> checkable_items_;

  /**
   * @brief 用于在 QTreeWidgetItem 中存储自定义数据的角色枚举。
   * 此处定义了 kFilenameRole，可能用于在树项中存储对应的完整文件名。
   */
  enum DataRole {
    kFilenameRole = Qt::UserRole + 1 ///< @brief 用于存储恢复文件名的自定义数据角色。Qt::UserRole 之后的值确保不与内置角色冲突。
  };
};

}  // namespace olive

#endif  // AUTORECOVERYDIALOG_H