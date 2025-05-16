#ifndef VIEWERPREVENTSLEEP_H
#define VIEWERPREVENTSLEEP_H

namespace olive {

/**
 * @brief 防止系统在查看器活动期间进入睡眠模式。
 *
 * 这个函数用于控制系统是否应该阻止进入睡眠状态。
 * 这在视频播放或渲染等长时间运行的任务中非常有用，
 * 可以防止系统因不活动而中断操作。
 *
 * @param on 如果为 true，则阻止系统睡眠；如果为 false，则允许系统睡眠。
 */
void PreventSleep(bool on);

}  // namespace olive

#endif  // VIEWERPREVENTSLEEP_H