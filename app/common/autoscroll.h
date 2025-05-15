#ifndef AUTOSCROLL_H
#define AUTOSCROLL_H

#include "common/define.h" // 可能包含一些通用的定义或 Olive 核心类型

namespace olive {

/**
 * @brief 定义自动滚动行为的类。
 *
 * 此类主要用于提供一个枚举，以指定在某些用户界面（如时间轴）中
 * 播放头到达视图边缘时的自动滚动方式。
 */
class AutoScroll {
public:
  /**
   * @brief 自动滚动的不同方法。
   */
  enum Method {
    kNone,  ///< @brief 不进行自动滚动。
    kPage,  ///< @brief 按页滚动。当播放头到达视图边缘时，视图会滚动一整页的距离。
    kSmooth ///< @brief 平滑滚动。当播放头接近视图边缘时，视图会平滑地跟随播放头移动。
  };
  // 此类目前仅包含枚举定义，将来可能会添加相关的方法或属性。
};

}  // namespace olive

#endif  // AUTOSCROLL_H