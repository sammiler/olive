#ifndef CRASHPAD_INTERFACE_H
#define CRASHPAD_INTERFACE_H

// 仅当启用了 Crashpad 支持时，以下内容才会被编译
#ifdef USE_CRASHPAD
#include <client/crash_report_database.h>  // Crashpad 客户端库：崩溃报告数据库
#include <client/settings.h>               // Crashpad 客户端库：设置
#include <qchar.h>                         // Qt 字符类型
#include <qlist.h>                         // Qt 列表容器 (QStringList)
#include <qobject.h>                       // Qt 对象模型 (虽然这里没有直接使用QObject子类)
#include <qstring.h>                       // Qt 字符串类
#include <qstringlist.h>                   // Qt 字符串列表类

/**
 * @brief 描述 Crashpad 初始化操作结果的结构体。
 */
struct Result {
  bool success;         ///< @brief 指示 Crashpad 初始化是否成功。如果为 true，则表示成功；否则为 false。
  QString processPath;  ///< @brief Crashpad 处理程序（handler）的可执行文件路径。
  QStringList args;     ///< @brief 启动 Crashpad 处理程序所需的命令行参数列表。
};

/**
 * @brief 初始化 Crashpad 崩溃报告系统。
 *
 * 此函数负责设置和启动 Crashpad 客户端，以便在应用程序发生崩溃时
 * 能够捕获崩溃信息并生成报告。
 * @return Result 包含初始化操作结果的 Result 结构体。
 * `success` 成员指示操作是否成功。如果成功，
 * `processPath` 和 `args` 将包含启动 Crashpad handler 进程所需的信息。
 */
Result InitializeCrashpad();

#endif  // USE_CRASHPAD

#endif  // CRASHPAD_INTERFACE_H