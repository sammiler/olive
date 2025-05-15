#ifndef TOHEX_H
#define TOHEX_H

#include <QString>
#include <QtGlobal>

#include "common/define.h"

/**
 * @namespace olive
 * @brief Olive命名空间，用于封装Olive项目相关的功能和定义。
 */
namespace olive {

/**
 * @brief 将64位无符号整数转换为十六进制字符串表示。
 *
 * 该函数接受一个64位无符号整数作为输入，并将其转换为十六进制格式的QString对象。
 * 主要用于调试和日志记录，将数字以十六进制字符串的形式输出。
 *
 * @param t 64位无符号整数，需要转换为十六进制字符串的输入值。
 * @return QString 表示输入值的十六进制字符串。
 */
inline QString ToHex(quint64 t) { return QStringLiteral("%1").arg(t, 0, 16); }

}  // namespace olive

#endif  // TOHEX_H
