#ifndef FRAME_H
#define FRAME_H

#include <olive/core/core.h> // 包含 olive::core::rational, olive::core::Color, olive::core::PixelFormat 等
#include <QVector>         // 虽然包含但在此头文件中未直接使用
#include <memory>          // 为了 std::shared_ptr

#include "common/define.h"    // 可能包含 DISABLE_COPY_MOVE 宏和其他通用定义
#include "render/videoparams.h" // 包含 VideoParams (其中可能也定义了 PixelFormat 和 Color)

namespace olive {

class Frame;
/**
 * @brief 指向 Frame 对象的共享指针类型定义。
 */
using FramePtr = std::shared_ptr<Frame>;

/**
 * @brief 表示从解码器获取的视频帧数据或音频样本数据。
 *
 *此类是一个容器，用于存储解码后的原始媒体数据，可以是视频图像
 * 或一段音频样本。它包含与数据相关的参数（通过VideoParams）
 * 以及实际的数据缓冲区。
 */
class Frame {
 public:
  /**
   * @brief 默认构造函数。
   * 初始化一个空的 Frame 对象。
   */
  Frame();

  /**
   * @brief 析构函数。
   * 确保在对象销毁时调用 destroy() 来释放分配的内存。
   */
  ~Frame();

  /**
   * @brief 禁止 Frame 对象的拷贝和移动操作。
   * 这是通过 DISABLE_COPY_MOVE 宏实现的，以防止对内部数据缓冲区的意外处理。
   */
  DISABLE_COPY_MOVE(Frame)

  /**
   * @brief 创建一个新的 Frame 对象的静态工厂方法。
   * @return FramePtr 指向新创建的 Frame 对象的共享指针。
   */
  static FramePtr Create();

  /**
   * @brief 获取与此帧关联的视频参数。
   * @return const VideoParams& 对视频参数对象的常量引用。
   */
  [[nodiscard]] const VideoParams& video_params() const;
  /**
   * @brief 设置此帧的视频参数。
   * 设置参数后通常需要调用 allocate() 来分配或重新分配内存。
   * @param params 要设置的视频参数对象。
   */
  void set_video_params(const VideoParams& params);

  /**
   * @brief 将两个场（顶场和底场）合成为一个隔行扫描帧。
   * @param top 指向顶场数据的 FramePtr。
   * @param bottom 指向底场数据的 FramePtr。
   * @return FramePtr 指向新创建的隔行扫描帧的共享指针；如果输入无效则可能返回 nullptr。
   */
  static FramePtr Interlace(const FramePtr& top, const FramePtr& bottom);

  /**
   * @brief 根据宽度、像素格式和通道数计算图像一行的字节大小 (linesize/stride)。
   * @param width 图像宽度（像素）。
   * @param format 像素格式。
   * @param channel_count 通道数量（对于平面格式可能需要）。
   * @return int 计算出的一行所需的字节数。
   */
  static int generate_linesize_bytes(int width, PixelFormat format, int channel_count);

  /**
   * @brief 获取帧图像一行的像素数量（通常等于宽度）。
   * @return int 一行的像素数。
   */
  [[nodiscard]] int linesize_pixels() const { return linesize_pixels_; }

  /**
   * @brief 获取帧图像数据中一行的字节大小 (stride)。
   * @return int 一行的字节数。
   */
  [[nodiscard]] int linesize_bytes() const { return linesize_; }

  /**
   * @brief 获取帧的有效宽度（像素）。
   * 通常从 video_params() 中获取。
   * @return int 帧宽度。
   */
  [[nodiscard]] int width() const { return params_.effective_width(); }

  /**
   * @brief 获取帧的有效高度（像素）。
   * 通常从 video_params() 中获取。
   * @return int 帧高度。
   */
  [[nodiscard]] int height() const { return params_.effective_height(); }

  /**
   * @brief 获取帧的像素格式。
   * 通常从 video_params() 中获取。
   * @return PixelFormat 帧的像素格式。
   */
  [[nodiscard]] PixelFormat format() const { return params_.format(); }

  /**
   * @brief 获取帧的通道数量。
   * 通常从 video_params() 中获取。
   * @return int 通道数量。
   */
  [[nodiscard]] int channel_count() const { return params_.channel_count(); }

  /**
   * @brief 获取指定坐标的像素颜色值。
   * @param x 像素的 x 坐标。
   * @param y 像素的 y 坐标。
   * @return Color 该点的颜色对象；如果坐标无效或帧未分配，则行为未定义或返回默认颜色。
   */
  [[nodiscard]] Color get_pixel(int x, int y) const;
  /**
   * @brief 检查指定的像素坐标是否在帧的有效范围内。
   * @param x 像素的 x 坐标。
   * @param y 像素的 y 坐标。
   * @return bool 如果坐标有效则返回 true，否则返回 false。
   */
  [[nodiscard]] bool contains_pixel(int x, int y) const;
  /**
   * @brief 设置指定坐标的像素颜色值。
   * @param x 像素的 x 坐标。
   * @param y 像素的 y 坐标。
   * @param c 要设置的颜色对象。
   */
  void set_pixel(int x, int y, const Color& c);

  /**
   * @brief 获取帧的时间戳。
   *
   * 此时间戳总是一个有理数，表示以秒为单位的时间。
   * @return const rational& 对时间戳的常量引用。
   */
  [[nodiscard]] const rational& timestamp() const { return timestamp_; }

  /**
   * @brief 设置帧的时间戳。
   * @param timestamp 要设置的时间戳（以秒为单位的有理数）。
   */
  void set_timestamp(const rational& timestamp) { timestamp_ = timestamp; }

  /**
   * @brief 获取此帧的数据缓冲区的指针。
   * @return char* 指向数据缓冲区的指针；如果未分配，则为 nullptr。
   */
  char* data() { return data_; }

  /**
   * @brief 获取此帧的数据缓冲区的常量指针。
   * @return const char* 指向数据缓冲区的常量指针；如果未分配，则为 nullptr。
   */
  [[nodiscard]] const char* const_data() const { return data_; }

  /**
   * @brief 根据当前设置的参数分配内存缓冲区以存储数据。
   *
   * 对于视频帧，必须先设置 width(), height() 和 format() 这些参数，此函数才能正常工作。
   * 如果之前已分配过内存缓冲区且未销毁，此函数将先销毁旧缓冲区再分配新缓冲区。
   * @return bool 如果内存分配成功则返回 true，否则返回 false。
   */
  bool allocate();

  /**
   * @brief 返回帧的数据缓冲区是否已分配。
   * @return bool 如果 data_ 指针非空 (即已分配内存) 则返回 true，否则返回 false。
   */
  [[nodiscard]] bool is_allocated() const { return data_; }

  /**
   * @brief 销毁通过 allocate() 分配的内存缓冲区。
   * 将 data_ 指针置为 nullptr 并重置 data_size_。
   */
  void destroy();

  /**
   * @brief 返回 data() 函数返回的数组的大小（以字节为单位）。
   *
   * 如果没有分配内存，则返回 0。
   * @return int 已分配数据缓冲区的大小（字节）。
   */
  [[nodiscard]] int allocated_size() const { return data_size_; }

  /**
   * @brief 将当前帧的数据转换为指定的像素格式。
   * @param format 目标像素格式。
   * @return FramePtr 指向包含转换后数据的新 Frame 对象的共享指针；如果转换失败，则可能返回 nullptr。
   */
  [[nodiscard]] FramePtr convert(PixelFormat format) const;

 private:
  /**
   * @brief 存储帧的视频/图像参数（如尺寸、像素格式等）。
   */
  VideoParams params_;

  /**
   * @brief 指向实际存储帧像素或音频样本数据的内存缓冲区的指针。
   */
  char* data_{nullptr};
  /**
   * @brief 已分配的数据缓冲区的大小（以字节为单位）。
   */
  int data_size_{0};

  /**
   * @brief 帧的时间戳，以秒为单位的有理数。
   */
  rational timestamp_;

  /**
   * @brief 图像数据中一行的字节大小 (stride)。
   */
  int linesize_{0};

  /**
   * @brief 图像数据中一行的像素数量（通常等于宽度）。
   */
  int linesize_pixels_{0};
};

}  // namespace olive

Q_DECLARE_METATYPE(olive::FramePtr)

#endif  // FRAME_H