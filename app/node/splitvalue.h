#ifndef SPLITVALUE_H  // 防止头文件被重复包含的宏
#define SPLITVALUE_H  // 定义 SPLITVALUE_H 宏

#include <QVariant>  // Qt 通用数据类型 QVariant
#include <QVector>   // Qt 动态数组容器 QVector

namespace olive {  // olive 项目的命名空间

/**
 * @brief SplitValue 类型定义为一个 QVector<QVariant>。
 *
 * 这个类型通常用于表示一个参数的多个分量或轨道的值。
 * 例如：
 * - 一个颜色参数 (RGBA) 可以被表示为一个包含4个 QVariant 的 SplitValue，每个 QVariant 代表 R, G, B, A 的一个分量。
 * - 一个二维向量参数 (X, Y) 可以被表示为一个包含2个 QVariant 的 SplitValue。
 * - 一个可以有多条关键帧轨道的参数 (例如，一个属性有 X, Y, Z 三个可独立设置关键帧的子属性)
 *   也可以使用 SplitValue 来存储每个轨道的当前值或默认值。
 *
 * 使用 QVector<QVariant> 提供了灵活性，因为每个分量可以是不同的基础数据类型 (尽管通常它们是相同的，如都是 double 或
 * int)。
 */
using SplitValue = QVector<QVariant>;  // 类型别名：SplitValue 代表一个 QVariant 的向量

}  // namespace olive

#endif  // SPLITVALUE_H