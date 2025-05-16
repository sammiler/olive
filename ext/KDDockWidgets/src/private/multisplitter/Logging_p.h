#ifndef KD_DOCKWIDGETS_MULTISPLITTER_LOGGING_P_H
#define KD_DOCKWIDGETS_MULTISPLITTER_LOGGING_P_H

#include <QLoggingCategory> // 包含 Qt 日志分类相关的头文件

// 声明一个名为 "separators" 的日志分类。
// 这允许在代码中通过该分类名称来控制特定模块（此处指分隔条相关功能）的日志输出级别和行为。
// 例如，可以使用 qDebug(separators) << "My debug message"; 来输出属于此分类的调试信息。
Q_DECLARE_LOGGING_CATEGORY(separators)


#endif // KD_DOCKWIDGETS_MULTISPLITTER_LOGGING_P_H
