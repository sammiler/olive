#ifndef FOOTAGERELINKDIALOG_H
#define FOOTAGERELINKDIALOG_H

#include <QDialog>      // QDialog 基类
#include <QString>      // (可能在 .cpp 中使用)
#include <QTreeWidget>  // 树形控件，用于显示需要重新链接的素材列表
#include <QVector>      // 为了 QVector<Footage*>
#include <QWidget>      // 为了 QWidget* parent 参数

// Olive 内部头文件
// 假设 footage.h 声明了 Footage 类
#include "node/project/footage/footage.h"
// #include "common/define.h" // 如果需要 common/define.h 中的内容

namespace olive {

/**
 * @brief “素材重新链接”对话框类。
 *
 *当项目中引用的媒体素材文件在其原始路径下找不到时（例如文件被移动或删除），
 * 此对话框会显示这些“离线”的素材列表，并允许用户为每个素材
 * 浏览并指定新的文件路径，以重新建立链接。
 */
class FootageRelinkDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 FootageRelinkDialog 对象。
   * @param footage 一个包含所有需要或可能需要重新链接的 Footage 对象指针的 QVector。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit FootageRelinkDialog(const QVector<Footage*>& footage, QWidget* parent = nullptr);

  // ~FootageRelinkDialog() override; // 默认析构函数通常足够，因为 Qt 的父子对象机制会处理 table_

 private:
  /**
   * @brief 更新树形控件 (`table_`) 中指定索引处的素材项的显示信息。
   * 这通常在用户为该素材选择了新的文件路径后调用，以反映新的路径和状态。
   * @param index 要更新的素材在 `footage_` QVector 中的索引，也对应于 `table_` 中的某个项。
   */
  void UpdateFootageItem(int index);

  /**
   * @brief 指向 QTreeWidget 对象的指针，用于以表格或树状列表的形式显示
   * 所有需要重新链接的素材及其当前状态（例如原始路径、新路径、是否已找到）。
   */
  QTreeWidget* table_;

  /**
   * @brief 存储传递给对话框的所有 Footage 对象指针的 QVector。
   * 对话框将操作这个列表中的素材。
   */
  QVector<Footage*> footage_;

 private slots:
  /**
   * @brief 当用户点击“浏览”按钮为某个选中的离线素材选择新文件路径时调用的槽函数。
   * 此函数通常会打开一个文件选择对话框，让用户定位到新的媒体文件。
   * 成功选择后，会更新对应素材的路径并可能调用 `UpdateFootageItem` 来刷新UI。
   */
  void BrowseForFootage();
};

}  // namespace olive

#endif  // FOOTAGERELINKDIALOG_H