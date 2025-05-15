#ifndef SEQUENCEPARAM_H // 文件保护宏，应与文件名一致，但这里是 SEQUENCEPARAM_H
#define SEQUENCEPARAM_H

#include <olive/core/core.h> // 引入 Olive 核心库定义，可能包含 rational 等类型
#include <QXmlStreamWriter>  // 引入 Qt XML 流写入器

#include "common/xmlutils.h"                // 引入 XML 处理工具函数
#include "dialog/sequence/presetmanager.h"  // 引入预设管理器基类定义 (Preset)
#include "render/videoparams.h"             // 引入视频参数定义 (VideoParams, PixelFormat)

namespace olive {

/**
 * @class SequencePreset
 * @brief 代表一个序列的预设配置。
 *
 * 此类继承自 Preset，并存储了创建或配置序列所需的所有参数，
 * 例如视频尺寸、帧率、音频设置以及预览参数等。
 * 它还负责从 XML 文件加载这些参数以及将这些参数保存到 XML 文件。
 */
class SequencePreset : public Preset {
 public:
  /**
   * @brief 默认构造函数。
   */
  SequencePreset() = default;

  /**
   * @brief 带参数的构造函数，用于直接创建并初始化一个序列预设对象。
   *
   * @param name 预设的名称。
   * @param width 视频宽度。
   * @param height 视频高度。
   * @param frame_rate 视频帧率 (有理数)。
   * @param pixel_aspect 像素宽高比 (有理数)。
   * @param interlacing 视频隔行扫描模式。
   * @param sample_rate 音频采样率。
   * @param channel_layout 音频声道布局。
   * @param preview_divider 预览分辨率的除数。
   * @param preview_format 预览的像素格式。
   * @param preview_autocache 是否启用预览自动缓存。
   */
  SequencePreset(const QString& name, int width, int height, const rational& frame_rate, const rational& pixel_aspect,
                 VideoParams::Interlacing interlacing, int sample_rate, uint64_t channel_layout, int preview_divider,
                 PixelFormat preview_format, bool preview_autocache)
      : width_(width),
        height_(height),
        frame_rate_(frame_rate),
        pixel_aspect_(pixel_aspect),
        interlacing_(interlacing),
        sample_rate_(sample_rate),
        channel_layout_(channel_layout),
        preview_divider_(preview_divider),
        preview_format_(preview_format),
        preview_autocache_(preview_autocache) {
    SetName(name); // 调用基类的 SetName 方法设置预设名称
  }

  /**
   * @brief 从 XML 流加载预设数据。
   *
   * 重写基类 Preset 的纯虚函数。
   * @param reader 指向 QXmlStreamReader 对象的指针，用于读取 XML 数据。
   */
  void Load(QXmlStreamReader* reader) override {
    // 循环读取当前元素下的所有子元素
    while (XMLReadNextStartElement(reader)) { // XMLReadNextStartElement 是一个辅助函数
      if (reader->name() == QStringLiteral("name")) {
        SetName(reader->readElementText());
      } else if (reader->name() == QStringLiteral("width")) {
        width_ = reader->readElementText().toInt();
      } else if (reader->name() == QStringLiteral("height")) {
        height_ = reader->readElementText().toInt();
      } else if (reader->name() == QStringLiteral("framerate")) {
        // 从字符串转换为 rational 类型
        frame_rate_ = rational::fromString(reader->readElementText().toUtf8().constData());
      } else if (reader->name() == QStringLiteral("pixelaspect")) {
        pixel_aspect_ = rational::fromString(reader->readElementText().toUtf8().constData());
      } else if (reader->name() == QStringLiteral("interlacing")) {
        // 从整数转换为 VideoParams::Interlacing 枚举类型
        interlacing_ = static_cast<VideoParams::Interlacing>(reader->readElementText().toInt());
      } else if (reader->name() == QStringLiteral("samplerate")) {
        sample_rate_ = reader->readElementText().toInt();
      } else if (reader->name() == QStringLiteral("chlayout")) {
        channel_layout_ = reader->readElementText().toULongLong();
      } else if (reader->name() == QStringLiteral("divider")) {
        preview_divider_ = reader->readElementText().toInt();
      } else if (reader->name() == QStringLiteral("format")) {
        // 从整数转换为 PixelFormat 枚举类型
        preview_format_ = static_cast<PixelFormat>(static_cast<PixelFormat::Format>(reader->readElementText().toInt()));
      } else if (reader->name() == QStringLiteral("autocache")) {
        preview_autocache_ = reader->readElementText().toInt();
      } else {
        reader->skipCurrentElement(); // 跳过不认识的元素
      }
    }
  }

  /**
   * @brief 将预设数据保存到 XML 流。
   *
   * 重写基类 Preset 的纯虚函数。
   * @param writer 指向 QXmlStreamWriter 对象的指针，用于写入 XML 数据。
   */
  void Save(QXmlStreamWriter* writer) const override {
    writer->writeTextElement(QStringLiteral("name"), GetName());
    writer->writeTextElement(QStringLiteral("width"), QString::number(width_));
    writer->writeTextElement(QStringLiteral("height"), QString::number(height_));
    writer->writeTextElement(QStringLiteral("framerate"), QString::fromStdString(frame_rate_.toString()));
    writer->writeTextElement(QStringLiteral("pixelaspect"), QString::fromStdString(pixel_aspect_.toString()));
    // 注意：XML 元素名 "interlacing_" 与成员变量名 interlacing_ 可能存在笔误，通常 XML 元素名不带下划线。
    // 如果 XML 格式固定，则保持 "interlacing_"；否则应为 "interlacing"。此处按代码原样注释。
    writer->writeTextElement(QStringLiteral("interlacing_"), QString::number(static_cast<int>(interlacing_)));
    writer->writeTextElement(QStringLiteral("samplerate"), QString::number(sample_rate_));
    writer->writeTextElement(QStringLiteral("chlayout"), QString::number(channel_layout_));
    writer->writeTextElement(QStringLiteral("divider"), QString::number(preview_divider_));
    writer->writeTextElement(QStringLiteral("format"),
                             QString::number(static_cast<int>(static_cast<PixelFormat::Format>(preview_format_))));
    writer->writeTextElement(QStringLiteral("autocache"), QString::number(preview_autocache_));
  }

  /** @brief 获取视频宽度。 [[nodiscard]] */
  [[nodiscard]] int width() const { return width_; }

  /** @brief 获取视频高度。 [[nodiscard]] */
  [[nodiscard]] int height() const { return height_; }

  /** @brief 获取视频帧率。 [[nodiscard]] */
  [[nodiscard]] const rational& frame_rate() const { return frame_rate_; }

  /** @brief 获取像素宽高比。 [[nodiscard]] */
  [[nodiscard]] const rational& pixel_aspect() const { return pixel_aspect_; }

  /** @brief 获取隔行扫描模式。 [[nodiscard]] */
  [[nodiscard]] VideoParams::Interlacing interlacing() const { return interlacing_; }

  /** @brief 获取音频采样率。 [[nodiscard]] */
  [[nodiscard]] int sample_rate() const { return sample_rate_; }

  /** @brief 获取音频声道布局。 [[nodiscard]] */
  [[nodiscard]] uint64_t channel_layout() const { return channel_layout_; }

  /** @brief 获取预览分辨率除数。 [[nodiscard]] */
  [[nodiscard]] int preview_divider() const { return preview_divider_; }

  /** @brief 获取预览像素格式。 [[nodiscard]] */
  [[nodiscard]] PixelFormat preview_format() const { return preview_format_; }

  /** @brief 获取预览是否自动缓存的设置。 [[nodiscard]] */
  [[nodiscard]] bool preview_autocache() const { return preview_autocache_; }

 private:
  // 私有成员变量，存储序列的各项参数

  int width_{};                                  ///< 视频宽度
  int height_{};                                 ///< 视频高度
  rational frame_rate_;                          ///< 视频帧率
  rational pixel_aspect_;                        ///< 像素宽高比
  VideoParams::Interlacing interlacing_;         ///< 隔行扫描模式
  int sample_rate_{};                            ///< 音频采样率
  uint64_t channel_layout_{};                    ///< 音频声道布局
  int preview_divider_{};                        ///< 预览分辨率除数
  PixelFormat preview_format_;                   ///< 预览像素格式
  bool preview_autocache_{};                     ///< 是否启用预览自动缓存
};

}  // namespace olive

#endif  // SEQUENCEPARAM_H
