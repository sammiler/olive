#ifndef OTIOPROPERTIESDIALOG_H
#define OTIOPROPERTIESDIALOG_H

#include <QDialog>      // QDialog 基类
#include <QList>        // 为了 QList<Sequence*>
#include <QTreeWidget>  // 树形控件，用于显示 OTIO 文件中的序列列表
#include <QWidget>      // 为了 QWidget* parent 参数

// Olive 内部头文件
#include "common/define.h"  // 可能包含一些通用定义
// 假设 project.h 声明了 Project 类
#include "node/project.h"
// 假设 sequence.h 声明了 Sequence 类
#include "node/project/sequence/sequence.h"
// OpenTimelineIO 核心头文件
#include "opentimelineio/timeline.h"  // 虽然包含，但在此头文件中未直接使用其类型，可能在 .cpp 中使用

namespace olive {

/**
 * @brief 用于在加载 OpenTimelineIO (OTIO) 文件时，配置导入序列属性的对话框。
 *
 * 当用户导入一个 OTIO 文件（该文件可能包含一个或多个序列）时，
 * 此对话框会显示这些序列，并允许用户为每个序列设置特定的导入选项或属性。
 */
class OTIOPropertiesDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 OTIOPropertiesDialog 对象。
   * @param sequences 从 OTIO 文件中解析出的 Sequence 对象指针列表。对话框将为这些序列提供配置选项。
   * @param active_project 指向当前活动 Project 对象的指针，新的序列将被导入到此项目中。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  OTIOPropertiesDialog(const QList<Sequence*>& sequences, Project* active_project, QWidget* parent = nullptr);

  // ~OTIOPropertiesDialog() override; // 默认析构函数通常足够

 private:
  /**
   * @brief 指向 QTreeWidget 对象的指针，用于显示从 OTIO 文件中解析出的序列列表。
   * 用户可以从中选择序列并可能为每个序列配置不同的设置。
   */
  QTreeWidget* table_;

  /**
   * @brief 存储从构造函数传入的、要进行属性配置的 Sequence 对象指针的常量列表。
   * （注意：成员变量声明为 const QList<Sequence*> sequences_，这意味着列表本身在构造后不能被修改，
   * 但列表中的 Sequence 对象指针所指向的内容是否可修改取决于 Sequence 类的设计。）
   */
  const QList<Sequence*> sequences_;  // 通常，如果是列表的拷贝，不会声明为 const

 private slots:
  /**
   * @brief 当用户选择在 `table_` 中的某个序列并希望配置其详细设置时调用的槽函数。
   * 此函数通常会打开另一个对话框（例如标准的序列设置对话框或特定于OTIO导入的序列设置对话框），
   * 以允许用户为选中的序列配置更详细的参数（如帧率、分辨率等，如果OTIO中未完全指定或允许覆盖）。
   */
  void SetupSequence();
};

}  // namespace olive

#endif  // OTIOPROPERTIESDIALOG_H