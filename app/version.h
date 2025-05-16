#ifndef GITHASH_H
#define GITHASH_H

#include <QString> // 引入 QString 类，用于存储版本字符串

namespace olive {

/**
 * @brief 存储应用程序的简短版本字符串。
 *
 * 这个字符串通常在构建过程中通过 Git 的提交哈希生成，
 * 可能是一个简短的哈希值或者一个 "vX.Y.Z-commit" 形式的字符串。
 * `extern` 关键字表示这个变量的定义在其他编译单元中（通常是一个 .cpp 文件，其内容可能由构建系统自动生成）。
 */
extern QString kAppVersion;

/**
 * @brief 存储应用程序的详细（长）版本字符串。
 *
 * 这个字符串可能包含更详细的构建信息，例如完整的 Git 提交哈希、构建日期、分支名称等。
 * `extern` 关键字表示这个变量的定义在其他编译单元中。
 */
extern QString kAppVersionLong;

}  // namespace olive

#endif  // GITHASH_H
