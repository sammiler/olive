#ifndef RENDERMODE_H  // 防止头文件被重复包含的宏
#define RENDERMODE_H  // 定义 RENDERMODE_H 宏

#include "common/define.h"  // 可能包含项目通用的定义或宏

namespace olive {  // olive 项目的命名空间

/**
 * @brief RenderMode 类 (更像是一个命名空间或仅包含枚举的类) 定义了渲染器可以运行的主要不同“模式”。
 *
 * 这些模式指示了渲染操作的目标和质量要求，允许节点根据当前的渲染模式
 * 调整其行为，例如在预览时使用较低精度的算法以提高性能，
 * 或在最终导出时使用较高精度的算法以获得最佳质量。
 */
class RenderMode {
 public:
  /**
   * @brief Mode 枚举定义了渲染器可以工作的不同模式。
   */
  enum Mode {
    /**
     * @brief kOffline (离线/预览模式)
     *
     * 此渲染仅用于实时预览，不需要“完美”。
     * 节点可以在可能的情况下使用较低精度的函数来节省性能。
     * 例如，在拖动播放头或进行快速预览时，可以使用此模式。
     */
    kOffline,

    /**
     * @brief kOnline (在线/导出模式)
     *
     * 此渲染是某种形式的导出或最终母版拷贝，节点应该花费时间/带宽/系统资源
     * 来生成一个更高精度的版本。
     * 例如，在最终渲染输出影片或高质量代理文件时，应使用此模式。
     */
    kOnline
  };
  // 注意：这里只定义了枚举 Mode，并没有定义类的成员或方法。
  // 在C++中，如果一个类只包含静态成员或类型定义 (如此处的枚举)，
  // 有时会使用 struct 或 namespace 来代替 class，或者像这里一样使用一个空的类。
  // 或者，更常见的做法是将枚举直接放在命名空间 olive 下，而不是嵌套在 RenderMode 类中，
  // 除非 RenderMode 类未来会有其他相关的非静态成员。
  // 例如： namespace olive { enum class RenderModeType { kOffline, kOnline }; }
  // 但遵循不修改代码的原则，此处保留原始结构。
};

}  // namespace olive

#endif  // RENDERMODE_H