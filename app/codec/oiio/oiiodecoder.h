#ifndef OIIODECODER_H
#define OIIODECODER_H

#include <OpenImageIO/imagebuf.h>  // OIIO 图像缓冲区相关
#include <OpenImageIO/imageio.h>   // OIIO 图像输入输出核心

#include "codec/decoder.h"  // 基类 Decoder
#include "common/define.h"  // 包含 olive::PixelFormat, olive::Frame 等

// #include <memory> // 为了 std::unique_ptr (如果未通过其他头文件包含)
// #include <QStringList> // 为了 QStringList (如果未通过其他头文件包含)

namespace olive {

/**
 * @brief 基于 OpenImageIO (OIIO) 库的图像解码器实现。
 *
 * 此类继承自 Decoder，专门用于解码各种静态图像格式 (如 JPEG, PNG, TIFF, EXR 等)
 * 以及支持多帧的图像格式 (如 GIF, WebP动画)。它主要用于视频轨道，
 * 将图像序列或单帧图像作为视频素材处理。
 */
class OIIODecoder : public Decoder {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 OIIODecoder 对象。
   */
  OIIODecoder();

  /**
   * @brief OIIODecoder 的默认析构函数。
   * 使用 DECODER_DEFAULT_DESTRUCTOR 宏定义。
   */
  DECODER_DEFAULT_DESTRUCTOR(OIIODecoder)

  /**
   * @brief 获取解码器的唯一标识符。
   * @return QString 解码器的ID，对于 OIIODecoder 通常是 "OIIODecoder"。
   */
  [[nodiscard]] QString id() const override;

  /**
   * @brief 检查此解码器是否支持视频解码 (在此上下文中，图像被视为单帧视频)。
   * @return bool 总是返回 true，因为 OIIODecoder 用于处理图像作为视频帧。
   */
  bool SupportsVideo() override { return true; }

  /**
   * @brief 探测指定图像文件的素材信息。
   * @param filename 要探测的图像文件名。
   * @param cancelled 指向 CancelAtom 的指针，用于在操作过程中检查是否已请求取消。
   * @return FootageDescription 包含探测到的图像素材信息的对象。
   */
  FootageDescription Probe(const QString& filename, CancelAtom* cancelled) const override;

 protected:
  /**
   * @brief 内部打开图像文件进行解码的实现。
   * @return bool 如果成功打开图像文件则返回 true，否则返回 false。
   */
  bool OpenInternal() override;

  /**
   * @brief 内部检索视频帧（图像）的实现。
   * @param p 检索视频帧所需的参数，主要包含请求的时间点（帧号）。
   * @return TexturePtr 指向解码后图像帧纹理的指针。
   */
  TexturePtr RetrieveVideoInternal(const RetrieveVideoParams& p) override;

  /**
   * @brief 内部关闭图像文件解码器的实现。
   */
  void CloseInternal() override;

 private:
  /**
   * @brief 指向 OIIO ImageInput 对象的智能指针，用于处理图像文件读取。
   */
  std::unique_ptr<OIIO::ImageInput> image_;

  /**
   * @brief 检查给定的文件类型（通过扩展名）是否被此解码器支持。
   * @param fn 文件名。
   * @return bool 如果文件类型受支持则返回 true，否则返回 false。
   */
  static bool FileTypeIsSupported(const QString& fn);

  /**
   * @brief 打开指定的图像文件和子图像（如果适用，例如多页TIFF或EXR的多部分）。
   * @param fn 图像文件名。
   * @param subimage 要打开的子图像索引 (通常为0)。
   * @return bool 如果成功打开图像句柄则返回 true，否则返回 false。
   */
  bool OpenImageHandler(const QString& fn, int subimage);

  /**
   * @brief 关闭当前打开的 OIIO 图像句柄并释放资源。
   */
  void CloseImageHandle();

  /**
   * @brief 从 OIIO::ImageSpec 中提取视频参数信息。
   * @param spec OIIO 图像规范对象。
   * @return VideoParams 从图像规范转换得到的视频参数。
   */
  static VideoParams GetVideoParamsFromImageSpec(const OIIO::ImageSpec& spec);

  /**
   * @brief 解码器内部使用的 Olive 像素格式。
   */
  PixelFormat pix_fmt_;
  /**
   * @brief 解码器内部使用的 OIIO 像素格式的基础类型。
   */
  OIIO::TypeDesc::BASETYPE oiio_pix_fmt_;

  /**
   * @brief 用于存储从 OIIO 读取的原始像素数据的内部帧缓冲区。
   */
  Frame buffer_;
  /**
   * @brief 上一次检索视频帧时使用的参数，用于优化或缓存。
   */
  RetrieveVideoParams last_params_;

  /**
   * @brief 存储 OIIODecoder 支持的图像格式扩展名列表（静态成员）。
   */
  static QStringList supported_formats_;
};

}  // namespace olive

#endif  // OIIODECODER_H