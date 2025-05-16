#ifndef VIEWERQUEUE_H
#define VIEWERQUEUE_H

#include "codec/frame.h"  // 包含帧相关的定义

namespace olive {

/**
 * @brief 表示查看器播放队列中的一帧。
 * 包含时间戳和帧数据。
 */
struct ViewerPlaybackFrame {
  /**
   * @brief 帧的时间戳。
   * 使用 rational 类型表示，可以精确表示帧率和时间。
   */
  rational timestamp;
  /**
   * @brief 帧数据。
   * 使用 QVariant 类型，可以存储不同类型的帧数据，例如图像或纹理ID。
   */
  QVariant frame;
};

/**
 * @brief 查看器播放队列类。
 * 继承自 std::list<ViewerPlaybackFrame>，用于存储和管理 ViewerPlaybackFrame 对象。
 * 这个队列用于缓冲待播放的帧，以确保流畅播放。
 */
class ViewerQueue : public std::list<ViewerPlaybackFrame> {
 public:
  /**
   * @brief 默认构造函数。
   * 创建一个空的 ViewerQueue。
   */
  ViewerQueue() = default;

  /**
   * @brief 按时间顺序追加帧到队列中。
   *
   * 此方法会根据播放速度（正向或反向）将帧插入到队列的正确位置，
   * 以保持队列中的帧按时间戳有序。
   * 如果队列为空，或者新帧的时间戳在正向播放时大于队尾帧的时间戳，
   * 或者在反向播放时小于队尾帧的时间戳，则直接将新帧添加到队尾。
   * 否则，遍历队列找到合适的插入位置。
   *
   * @param f 要追加的 ViewerPlaybackFrame。
   * @param playback_speed 播放速度。正数表示正向播放，负数表示反向播放。
   */
  void AppendTimewise(const ViewerPlaybackFrame& f, int playback_speed) {
    // 如果队列为空，或者根据播放方向，新帧的时间戳应该在队尾
    if (this->empty() || (this->back().timestamp < f.timestamp) == (playback_speed > 0)) {
      this->push_back(f);  // 直接添加到队尾
    } else {
      // 遍历队列寻找正确的插入位置
      for (auto i = this->begin(); i != this->end(); i++) {
        // 根据播放方向，如果当前迭代到的帧的时间戳在新帧之后
        if ((i->timestamp > f.timestamp) == (playback_speed > 0)) {
          this->insert(i, f);  // 在当前位置之前插入新帧
          break;               // 插入完成，跳出循环
        }
      }
    }
  }

  /**
   * @brief 清除队列中指定时间之前的帧。
   *
   * 此方法会根据播放速度和指定的时间，移除队列前端的过期帧。
   * 在正向播放时，移除时间戳小于指定时间的帧。
   * 在反向播放时，移除时间戳大于指定时间的帧。
   *
   * @param time 指定的时间点。
   * @param playback_speed 播放速度。正数表示正向播放，负数表示反向播放。
   */
  void PurgeBefore(const rational& time, int playback_speed) {
    // 当队列不为空，并且队首的帧根据播放方向判断为过期时，循环移除
    while (!this->empty() && ((playback_speed > 0 && this->front().timestamp < time) ||
                              (playback_speed < 0 && this->front().timestamp > time))) {
      this->pop_front();  // 移除队首帧
    }
  }
};

}  // namespace olive

#endif  // VIEWERQUEUE_H
