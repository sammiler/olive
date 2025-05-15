#ifndef OIIOENCODER_H
#define OIIOENCODER_H

#include "codec/encoder.h" // 基类 Encoder
#include "common/define.h" // 包含 olive::FramePtr, olive::SampleBuffer, EncodingParams 等

// OpenImageIO 的头文件通常在 .cpp 文件中包含，这里仅作示意
// #include <OpenImageIO/imageio.h>
// #include <OpenImageIO/imagebuf.h>

// Qt 相关头文件（如果需要）
// #include <QObject>

// namespace olive {
// class Frame;
// class SampleBuffer;
// struct EncodingParams;
// struct SubtitleBlock;
// namespace core { class rational; }
// }

namespace olive {

/**
 * @brief 基于 OpenImageIO (OIIO) 库的图像编码器实现。
 *
 * 此类继承自 Encoder，用于将视频帧数据编码输出为多种静态图像文件格式
 * (如 PNG, JPEG, TIFF, EXR 等) 或图像序列。它主要负责处理图像数据的
 * 格式化和写入文件。
 */
class OIIOEncoder : public Encoder {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 OIIOEncoder 对象。
   * @param params 编码所需的参数，定义了输出文件的路径、格式、质量等。
   */
  explicit OIIOEncoder(const EncodingParams &params);

  // 析构函数通常由基类 Encoder 通过宏提供默认实现或自定义

 public slots:
  /**
   * @brief 打开编码器并准备写入图像文件。
   * 根据 params_ 中的设置（尤其是输出文件名和格式）初始化 OIIO ImageOutput。
   * @return bool 如果编码器成功打开并准备好写入数据，则返回 true，否则返回 false。
   */
  bool Open() override;

  /**
   * @brief 将一个视频帧（图像）写入到输出文件。
   * @param frame 指向包含视频帧数据的 olive::FramePtr 智能指针。
   * @param time 当前帧的时间戳 (对于图像序列可能用于生成文件名)。
   * @return bool 如果帧成功写入则返回 true，否则返回 false。
   */
  bool WriteFrame(olive::FramePtr frame, olive::core::rational time) override;

  /**
   * @brief 写入音频样本缓冲区 (OIIOEncoder 通常不支持音频)。
   * @param audio 包含音频样本数据的 olive::SampleBuffer 对象。
   * @return bool 通常返回 false，因为图像编码器不处理音频。
   */
  bool WriteAudio(const SampleBuffer &audio) override;

  /**
   * @brief 写入字幕块 (OIIOEncoder 通常不支持字幕)。
   * @param sub_block 指向包含字幕数据的 SubtitleBlock 对象的指针。
   * @return bool 通常返回 false，因为图像编码器不处理字幕。
   */
  bool WriteSubtitle(const SubtitleBlock *sub_block) override;

  /**
   * @brief 关闭编码器，完成所有挂起的图像写入操作并释放 OIIO 资源。
   */
  void Close() override;

  // 如果有私有成员变量和方法，会在这里列出并注释
  // private:
  //  std::unique_ptr<OIIO::ImageOutput> image_out_; ///< OIIO 图像输出对象
  //  bool open_{false}; ///< 标记编码器是否已打开
  //  int frame_count_{0}; ///< 已写入帧的计数器，用于图像序列命名
};

}  // namespace olive

#endif  // OIIOENCODER_H