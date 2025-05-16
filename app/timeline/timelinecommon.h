#ifndef TIMELINECOMMON_H
#define TIMELINECOMMON_H

#include <olive/core/core.h> // 引入 Olive 核心库的头文件，提供基础数据结构和功能

#include "common/define.h" // 引入项目内通用的定义文件

using namespace olive::core; // 使用 olive::core 命名空间，方便直接访问其成员

namespace olive {

// 前向声明 Block 类，表示时间轴上的一个区块（例如视频片段、音频片段）
class Block;
// 前向声明 Track 类，表示时间轴上的一个轨道
class Track;

/**
 * @brief Timeline 类主要作为命名空间，包含与时间轴相关的通用枚举和结构体。
 *
 * 这个类本身通常不被实例化，而是提供一组静态定义，用于描述时间轴的各种状态和操作模式。
 */
class Timeline {
public:
  /**
   * @brief 定义时间轴上元素的操作模式。
   */
  enum MovementMode {
    kNone,     // 无操作模式
    kMove,     // 移动模式，用于移动整个区块
    kTrimIn,   // 裁切入点模式，用于调整区块的开始时间
    kTrimOut   // 裁切出点模式，用于调整区块的结束时间
  };

  /**
   * @brief 定义时间轴上缩略图的显示模式。
   */
  enum ThumbnailMode {
    kThumbnailOff,    // 关闭缩略图显示
    kThumbnailInOut,  // 仅在区块的入点和出点显示缩略图
    kThumbnailOn      // 完整显示缩略图
  };

  /**
   * @brief 定义时间轴上音频波形的显示模式。
   */
  enum WaveformMode {
    kWaveformsDisabled, // 禁用波形显示
    kWaveformsEnabled   // 启用波形显示
  };

  /**
   * @brief 检查给定的移动模式是否为裁切模式。
   *
   * @param mode 要检查的移动模式。
   * @return 如果模式是 kTrimIn 或 kTrimOut，则返回 true；否则返回 false。
   */
  static bool IsATrimMode(MovementMode mode) { return mode == kTrimIn || mode == kTrimOut; }

  /**
   * @brief 存储编辑操作目标信息的结构体。
   *
   * 当在时间轴上进行编辑（如拖放、插入等）时，此结构体用于保存与编辑目标相关的信息。
   */
  struct EditToInfo {
    Track* track{};          // 指向目标轨道的指针，可能为空
    rational nearest_time;   // 编辑操作发生位置附近的最接近的精确时间点
    Block* nearest_block{};  // 编辑操作发生位置附近的最接近的区块指针，可能为空
  };
};

/**
 * @brief 定义播放头在时间轴上显示的颜色。
 *
 * 通常通过访问当前 UI 调色板的高亮颜色来获取。
 */
#define PLAYHEAD_COLOR palette().highlight().color()

}  // namespace olive

#endif  // TIMELINECOMMON_H
