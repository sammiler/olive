#ifndef TOOL_H
#define TOOL_H

#include <QCoreApplication> // 引入 QCoreApplication 类，主要用于国际化翻译 (translate)
#include <QString>          // 引入 QString 类，用于字符串操作

#include "common/define.h"   // 引入项目内通用的定义文件

namespace olive {

/**
 * @brief 定义了应用程序中可用的各种工具及其相关功能。
 *
 * 这个类主要包含两个枚举：`Item` 定义了主要的编辑工具，
 * `AddableObject` 定义了可以通过特定工具（如 kAdd）添加到时间轴的对象类型。
 * 它还提供了静态方法来获取这些可添加对象的名称和ID。
 */
class Tool {
 public:
  /**
   * @brief 定义了可在整个应用程序中使用的工具列表。
   *
   * 这些工具代表了用户在时间轴或其他编辑器面板上可以激活的不同编辑模式或操作。
   */
  enum Item {
    /// @brief 无工具。不应将其设置为主应用程序工具，其唯一实际用途是指示某处缺少工具，类似于 nullptr。
    kNone,

    /// @brief 指针工具。通常用于选择、移动和基本操作。
    kPointer,

    /// @brief 编辑工具。可能用于更精细的编辑操作，如调整片段边缘。
    kEdit,

    /// @brief 涟漪编辑工具。调整一个片段时，会影响后续片段的位置。
    kRipple,

    /// @brief 卷动编辑工具。同时调整相邻两个片段的入点和出点，总时长不变。
    kRolling,

    /// @brief 剃刀工具。用于在时间轴上切割片段。
    kRazor,

    /// @brief 滑移工具。在保持片段在时间轴上时长和位置不变的情况下，改变片段内容的入点和出点。
    kSlip,

    /// @brief 滑动工具。移动片段在时间轴上的位置，同时调整相邻片段的长度以填充空隙或腾出空间，而不改变被移动片段的长度。
    kSlide,

    /// @brief 手型工具。用于平移查看区域，例如在时间轴或节点编辑器中。
    kHand,

    /// @brief 缩放工具。用于放大或缩小查看区域。
    kZoom,

    /// @brief 转场工具。用于创建或编辑片段之间的转场效果。
    kTransition,

    /// @brief 录制工具。可能用于实时录制输入（例如音频、视频）。
    kRecord,

    /// @brief 添加工具。用于向时间轴添加各种类型的预设对象。
    kAdd,

    /// @brief 轨道选择工具。用于选择整个轨道或轨道的一部分。
    kTrackSelect,

    /// @brief 工具枚举的总数。用于迭代或边界检查。
    kCount
  };

  /**
   * @brief 定义了可以使用 kAdd 工具添加的对象类型。
   *
   * 这些是用户可以快速添加到时间轴上的预设媒体或效果类型。
   */
  enum AddableObject {
    /// @brief 空片段。一个没有任何内容的占位片段。
    kAddableEmpty,

    /// @brief 彩条片段。一个显示通用视频测试彩条的视频片段。
    kAddableBars,

    /// @brief 形状片段。一个显示基本几何形状的视频片段。
    kAddableShape,

    /// @brief 纯色片段。一个填充了纯色的视频片段。
    kAddableSolid,

    /// @brief 标题片段。一个用于显示文本标题的视频片段。
    kAddableTitle,

    /// @brief 音频音调片段。一个包含正弦波等音频信号的音频片段。
    kAddableTone,

    /// @brief 字幕片段。用于在视频上显示字幕。
    kAddableSubtitle,

    /// @brief 可添加对象枚举的总数。用于迭代或边界检查。
    kAddableCount
  };

  /**
   * @brief 获取可添加对象的本地化显示名称。
   * @param a 要获取名称的 AddableObject 枚举值。
   * @return 返回对应 AddableObject 的可读 QString 名称，如果未知则返回 "Unknown"。
   */
  static QString GetAddableObjectName(const AddableObject& a) {
    switch (a) {
      case kAddableEmpty:
        return QCoreApplication::translate("Tool", "Empty"); // 空对象
      case kAddableBars:
        return QCoreApplication::translate("Tool", "Bars");  // 彩条
      case kAddableShape:
        return QCoreApplication::translate("Tool", "Shape"); // 形状
      case kAddableSolid:
        return QCoreApplication::translate("Tool", "Solid"); // 纯色
      case kAddableTitle:
        return QCoreApplication::translate("Tool", "Title"); // 标题
      case kAddableTone:
        return QCoreApplication::translate("Tool", "Tone");  // 音调
      case kAddableSubtitle:
        return QCoreApplication::translate("Tool", "Subtitle"); // 字幕
      case kAddableCount: // kAddableCount 通常不代表一个实际对象
        break;
    }

    return QCoreApplication::translate("Tool", "Unknown"); // 未知对象
  }

  /**
   * @brief 获取可添加对象的唯一标识符字符串。
   *
   * 这些ID通常用于内部识别或序列化。
   * @param a 要获取ID的 AddableObject 枚举值。
   * @return 返回对应 AddableObject 的 QString ID；如果未知或为 kAddableCount，则返回空 QString。
   */
  static QString GetAddableObjectID(const AddableObject& a) {
    switch (a) {
      case kAddableEmpty:
        return QStringLiteral("empty");
      case kAddableBars:
        return QStringLiteral("bars");
      case kAddableShape:
        return QStringLiteral("shape");
      case kAddableSolid:
        return QStringLiteral("solid");
      case kAddableTitle:
        return QStringLiteral("title");
      case kAddableTone:
        return QStringLiteral("tone");
      case kAddableSubtitle:
        return QStringLiteral("subtitle");
      case kAddableCount: // kAddableCount 通常不代表一个实际对象
        break;
    }

    return {}; // 返回一个空的 QString
  }
};

}  // namespace olive

#endif  // TOOL_H
