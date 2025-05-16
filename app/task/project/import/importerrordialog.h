#ifndef PROJECTIMPORTERRORDIALOG_H  // 防止头文件被重复包含的预处理器指令
#define PROJECTIMPORTERRORDIALOG_H  // 定义 PROJECTIMPORTERRORDIALOG_H 宏

#include <QDialog>  // 包含了 Qt 对话框基类

#include "common/define.h"  // 包含了项目中通用的定义或宏

// olive 命名空间前向声明 (如果 ProjectImportErrorDialog 中使用了其他 olive 命名空间下的类型作为指针或引用，
// 且这些类型的完整定义不需要在此头文件中，则可以在此进行前向声明。
// 但根据当前代码，define.h 可能已经处理了必要的 olive 命名空间内容，或者此类不需要额外的前向声明。)

namespace olive {  // olive 项目的命名空间

/**
 * @brief ProjectImportErrorDialog 类定义，继承自 QDialog 类。
 *
 * 此类用于创建一个对话框，以向用户显示在项目导入过程中发生的错误。
 * 它通常会列出导入失败或无法识别的文件名。
 */
class ProjectImportErrorDialog : public QDialog {
 Q_OBJECT  // Qt 对象的宏，用于启用信号和槽机制、国际化等 Qt 特性
     public :
     /**
      * @brief ProjectImportErrorDialog 的构造函数。
      * @param filenames 一个字符串列表，包含了导入时发生错误或无法识别的文件名。
      * 这些文件名将被显示在对话框中，告知用户哪些文件未能成功导入。
      * @param parent 指向父 QWidget 对象的指针。对话框通常会有一个父窗口。
      * 默认为 nullptr，表示该对话框没有父窗口（即顶级窗口）。
      */
     explicit ProjectImportErrorDialog(const QStringList& filenames, QWidget* parent = nullptr);
};

}  // namespace olive

#endif  // PROJECTIMPORTERRORDIALOG_H // 结束预处理器指令 #ifndef PROJECTIMPORTERRORDIALOG_H
