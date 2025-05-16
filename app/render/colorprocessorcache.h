#ifndef COLORPROCESSORCACHE_H  // 防止头文件被重复包含的宏
#define COLORPROCESSORCACHE_H  // 定义 COLORPROCESSORCACHE_H 宏

#include "render/colorprocessor.h"  // 包含 ColorProcessor 和 ColorProcessorPtr (std::shared_ptr<ColorProcessor>) 的定义

// 假设 QHash 已通过 <QHash> 包含，这通常是 Qt 项目的常见做法，
// 或者通过 "render/colorprocessor.h" 间接包含。

namespace olive {  // olive 项目的命名空间

/**
 * @brief ColorProcessorCache 类型定义为一个从 QString 到 ColorProcessorPtr 的 QHash。
 *
 * 这个类型用于缓存已创建的 ColorProcessor 对象，以避免重复创建相同的颜色转换处理器。
 * 缓存的键 (QString) 通常是根据颜色转换的参数 (例如输入色彩空间、输出色彩空间、视图变换、外观等)
 * 生成的一个唯一标识符字符串。通过这个ID，可以快速查找并重用已存在的 ColorProcessor。
 *
 * 使用共享指针 (ColorProcessorPtr / std::shared_ptr<ColorProcessor>) 作为值，
 * 有助于管理 ColorProcessor 对象的生命周期，当没有任何地方引用某个处理器时，它会被自动销毁。
 *
 * 这个缓存机制对于提高色彩管理系统的性能非常重要，因为创建 OCIO 处理器可能是一个相对耗时的操作。
 */
using ColorProcessorCache = QHash<QString, ColorProcessorPtr>;
// 类型别名：ColorProcessorCache 代表一个哈希表，
// 其中键是字符串 (通常是颜色转换的唯一ID)，
// 值是指向 ColorProcessor 对象的共享指针。

}  // namespace olive

#endif  // COLORPROCESSORCACHE_H