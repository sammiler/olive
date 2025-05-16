#ifndef PATREON_H
#define PATREON_H

#include <QStringList>  // Qt 字符串列表类

// namespace olive { // 如果此变量应属于特定命名空间

/**
 * @brief 存储 Patreon 支持者名单的全局字符串列表。
 *
 * 此列表通常在应用程序的“关于”对话框或其他鸣谢部分使用，
 * 用于展示对项目提供财务支持的 Patreon 用户。
 * 该变量通常在其他地方（例如一个 .cpp 文件）被初始化或填充数据。
 *
 * @note 作为全局变量，应谨慎处理其初始化和多线程访问（如果适用）。
 * 通常，此类数据会通过特定脚本或在构建过程中生成并填充。
 */
inline QStringList patrons;  // 使用 extern 声明，定义应在某个 .cpp 文件中

// } // namespace olive (如果适用)

#endif  // PATREON_H