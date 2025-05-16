#ifndef TIMELINECOORDINATE_H
#define TIMELINECOORDINATE_H

#include "node/output/track/track.h" // 引入 Track 类的定义，TimelineCoordinate 与轨道相关

namespace olive {

/**
 * @brief 表示时间轴上的一个具体坐标。
 *
 * TimelineCoordinate 类用于精确定义时间轴上的一个点，
 * 它由一个帧号（通常是 rational 类型以保证精度）和一个轨道引用组成。
 * 这个类帮助定位在特定轨道的特定时间点。
 */
class TimelineCoordinate {
public:
  /**
   * @brief 默认构造函数。
   *
   * 创建一个空的或无效的时间轴坐标对象。
   * 帧号和轨道引用可能处于未初始化或默认状态。
   */
  TimelineCoordinate();

  /**
   * @brief 构造函数，使用帧号和轨道引用初始化。
   *
   * @param frame 时间轴上的帧号，使用 rational 类型以支持精确的非整数帧位置。
   * @param track 对时间轴上特定轨道的引用。
   */
  TimelineCoordinate(const rational& frame, const Track::Reference& track);

  /**
   * @brief 构造函数，使用帧号、轨道类型和轨道索引初始化。
   *
   * 这是指定轨道的另一种方式，通过轨道类型（如视频、音频）和在该类型中的索引来定位。
   * @param frame 时间轴上的帧号。
   * @param track_type 轨道的类型 (例如，视频轨道，音频轨道)。
   * @param track_index 轨道在其类型中的索引。
   */
  TimelineCoordinate(const rational& frame, const Track::Type& track_type, const int& track_index);

  /**
   * @brief 获取坐标的帧号。
   *
   * @return 返回一个 rational 类型的常量引用，表示当前坐标的帧号。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const rational& GetFrame() const;

  /**
   * @brief 获取坐标所在的轨道引用。
   *
   * @return 返回一个 Track::Reference 类型的常量引用，表示当前坐标关联的轨道。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const Track::Reference& GetTrack() const;

  /**
   * @brief 设置坐标的帧号。
   *
   * @param frame 要设置的新帧号 (rational 类型)。
   */
  void SetFrame(const rational& frame);

  /**
   * @brief 设置坐标所在的轨道引用。
   *
   * @param track 要设置的新轨道引用 (Track::Reference 类型)。
   */
  void SetTrack(const Track::Reference& track);

private:
  /**
   * @brief 存储坐标的帧号。
   *
   * 使用 rational 类型以支持精确的时间表示，例如子帧精度。
   */
  rational frame_;

  /**
   * @brief 存储坐标所在的轨道引用。
   *
   * Track::Reference 用于唯一标识时间轴上的一个轨道。
   */
  Track::Reference track_;
};

}  // namespace olive

#endif  // TIMELINECOORDINATE_H
