#ifndef PREFERENCESBEHAVIORTAB_H
#define PREFERENCESBEHAVIORTAB_H

#include <QTreeWidget>  // 引入 QTreeWidget 类，用于创建树形列表控件

#include "dialog/configbase/configdialogbase.h"  // 引入配置对话框基类相关定义

namespace olive {

/**
 * @class PreferencesBehaviorTab
 * @brief “首选项”对话框中的“行为”选项卡类。
 *
 * 继承自 ConfigDialogBaseTab，用于管理应用程序各种行为相关的设置。
 * 这些设置通常以树形结构展示，允许用户开启或关闭特定的行为特性。
 */
class PreferencesBehaviorTab : public ConfigDialogBaseTab {
  Q_OBJECT
 public:
  /**
   * @brief PreferencesBehaviorTab 构造函数。
   *
   * 初始化行为选项卡的用户界面，并加载配置项到树形控件中。
   */
  PreferencesBehaviorTab();

  /**
   * @brief 应用当前选项卡中的更改。
   *
   * @param command 指向 MultiUndoCommand 对象的指针，用于记录撤销/重做操作。
   * 此函数会覆盖基类中的同名虚函数。它会将树形控件中勾选的配置项保存起来。
   */
  void Accept(MultiUndoCommand *command) override;

 private:
  /**
   * @brief 向树形控件中添加一个父项（可折叠的分类）。
   *
   * @param text 父项显示的文本。
   * @param tooltip 父项的提示文本。
   * @param parent 可选参数，此新父项的父项。如果为 nullptr，则添加到顶层。
   * @return 指向新创建的 QTreeWidgetItem 对象的指针。
   */
  QTreeWidgetItem *AddParent(const QString &text, const QString &tooltip, QTreeWidgetItem *parent = nullptr);

  /**
   * @brief 向树形控件中添加一个父项（可折叠的分类），不带提示文本的重载版本。
   *
   * @param text 父项显示的文本。
   * @param parent 可选参数，此新父项的父项。如果为 nullptr，则添加到顶层。
   * @return 指向新创建的 QTreeWidgetItem 对象的指针。
   */
  QTreeWidgetItem *AddParent(const QString &text, QTreeWidgetItem *parent = nullptr);

  /**
   * @brief 向树形控件中添加一个配置子项（通常是一个带有复选框的条目）。
   *
   * @param text 子项显示的文本。
   * @param config_key 此配置项在配置文件中对应的键名。
   * @param tooltip 子项的提示文本。
   * @param parent 此子项所属的父项。
   * @return 指向新创建的 QTreeWidgetItem 对象的指针。
   */
  QTreeWidgetItem *AddItem(const QString &text, const QString &config_key, const QString &tooltip,
                           QTreeWidgetItem *parent);

  /**
   * @brief 向树形控件中添加一个配置子项，不带提示文本的重载版本。
   *
   * @param text 子项显示的文本。
   * @param config_key 此配置项在配置文件中对应的键名。
   * @param parent 此子项所属的父项。
   * @return 指向新创建的 QTreeWidgetItem 对象的指针。
   */
  QTreeWidgetItem *AddItem(const QString &text, const QString &config_key, QTreeWidgetItem *parent);

  /**
   * @brief 存储 QTreeWidgetItem 与其对应配置键（config_key）的映射。
   *
   * 用于在保存配置时快速查找每个树节点对应的配置项。
   */
  QMap<QTreeWidgetItem *, QString> config_map_;

  /**
   * @brief 指向用于显示和编辑行为配置的树形控件的指针。
   */
  QTreeWidget *behavior_tree_;
};

}  // namespace olive

#endif  // PREFERENCESBEHAVIORTAB_H