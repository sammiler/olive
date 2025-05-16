#ifndef TYPESERIALIZER_H  // 防止头文件被多次包含的宏定义开始
#define TYPESERIALIZER_H

#include <olive/core/core.h>  // Olive 核心库的包含，可能定义了 AudioParams 等类型或其依赖的基础类型
#include <QXmlStreamReader>   // Qt XML 流读取器
#include <QXmlStreamWriter>   // Qt XML 流写入器

#include "common/xmlutils.h"  // 通用 XML 工具类，可能包含读写 XML 属性或元素的辅助函数

// 可能需要的前向声明
// class AudioParams; // 假设 AudioParams 在 olive/core/core.h 或其他地方定义

namespace olive {  // Olive 编辑器的命名空间

using namespace core;  // 将 core 命名空间的内容引入到当前的 olive 命名空间中，
// 使得可以直接使用 core 中定义的类型（如 AudioParams），而无需 core:: 前缀。

/**
 * @brief 提供特定数据类型序列化和反序列化功能的工具类。
 * 这个类通常包含一系列静态方法，每对方法负责一种特定数据类型（如 AudioParams, VideoParams 等）
 * 与 XML 格式之间的转换。
 * 它将特定类型的序列化逻辑从通用的项目序列化器 (ProjectSerializer) 中分离出来，
 * 使得代码更模块化，易于管理和扩展。
 */
class TypeSerializer {
 public:
  /**
   * @brief TypeSerializer 默认构造函数。
   *  由于所有方法都是静态的，通常不会创建此类的实例。
   */
  TypeSerializer() = default;

  /**
   * @brief (静态方法) 从 XML 流中加载 (反序列化) AudioParams 对象。
   * @param reader 指向 QXmlStreamReader 的指针，用于读取 XML 数据。
   * @return AudioParams 从 XML 中解析出的音频参数对象。
   */
  static AudioParams LoadAudioParams(QXmlStreamReader *reader);
  /**
   * @brief (静态方法) 将 AudioParams 对象序列化到 XML 流中。
   * @param writer 指向 QXmlStreamWriter 的指针，用于写入 XML 数据。
   * @param a 要序列化的 AudioParams 对象。
   */
  static void SaveAudioParams(QXmlStreamWriter *writer, const AudioParams &a);

  // 此处可以根据需要添加更多其他类型的 Load/Save 方法，例如：
  // static VideoParams LoadVideoParams(QXmlStreamReader *reader);
  // static void SaveVideoParams(QXmlStreamWriter *writer, const VideoParams &v);
  // static SubtitleParams LoadSubtitleParams(QXmlStreamReader *reader);
  // static void SaveSubtitleParams(QXmlStreamWriter *writer, const SubtitleParams &s);
  // static QColor LoadColor(QXmlStreamReader *reader);
  // static void SaveColor(QXmlStreamWriter *writer, const QColor &c);
  // 等等...
};

}  // namespace olive

#endif  // TYPESERIALIZER_H // 头文件宏定义结束