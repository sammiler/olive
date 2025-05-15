#ifndef LOOPMODE_H // 防止头文件被重复包含的宏
#define LOOPMODE_H // 定义 LOOPMODE_H 宏

namespace olive { // olive 项目的命名空间

/**
 * @brief LoopMode 枚举类定义了不同的循环播放模式。
 *
 * 这个枚举用于指定当播放到达媒体片段或序列的末尾时应该如何处理：
 * - **kLoopModeOff (关闭循环):** 播放到末尾即停止。
 * - **kLoopModeLoop (循环播放):** 播放到末尾后从头开始重新播放。
 * - **kLoopModeClamp (钳位/停在最后一帧):** 播放到末尾后停留在最后一帧。
 *   (有时也称作 "Hold Last Frame" 或 "Freeze Last Frame")
 *
 * 这个模式通常应用于时间轴上的片段 (Block)、序列 (Sequence) 或素材查看器中的播放行为。
 */
enum class LoopMode { // 使用 enum class 以提供更强的类型安全和作用域
  kLoopModeOff,   // 关闭循环模式，播放到末尾停止
  kLoopModeLoop,  // 循环播放模式，到达末尾后从头开始
  kLoopModeClamp  // 钳位模式，到达末尾后停留在最后一帧
};

} // namespace olive

#endif  // LOOPMODE_H