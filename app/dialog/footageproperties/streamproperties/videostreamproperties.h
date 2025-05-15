#ifndef VIDEOSTREAMPROPERTIES_H
#define VIDEOSTREAMPROPERTIES_H

#include <QCheckBox> // 复选框控件
#include <QComboBox> // 下拉选择框控件
#include <QWidget>   // QWidget 基类, 为了 parent 参数
#include <QString>   // Qt 字符串类
#include <cstdint>   // 为了 int64_t

// Olive 内部头文件
// 假设 footage.h 声明了 Footage 类和 Project 类 (或者 Project 在其他地方声明)
#include "node/project/footage/footage.h"
// 假设 streamproperties.h 声明了 StreamProperties 基类
#include "streamproperties.h"
// 假设 integerslider.h 声明了 IntegerSlider 类
#include "widget/slider/integerslider.h"
// 假设 standardcombos.h 声明了 InterlacedComboBox, FrameRateComboBox, PixelAspectRatioComboBox
#include "widget/standardcombos/standardcombos.h"
// 假设 undocommand.h 声明了 UndoCommand 基类和 MultiUndoCommand
#include "undo/undocommand.h"
// 假设 videoparams.h (或其包含的 common/define.h) 声明了 VideoParams::Interlacing, rational, VideoParams::ColorRange
#include "render/videoparams.h"
// #include "common/define.h" // 如果 rational 等未被 videoparams.h 包含

namespace olive {

// 前向声明 (如果 Project 类定义在别处且未通过 footage.h 包含)
// class Project;

/**
 * @brief 用于表示和配置特定素材中单个视频流属性的UI组件类。
 *
 *此类继承自 StreamProperties，专门用于处理视频流的特定属性。
 * 它通常会关联到一个 Footage 对象和一个视频流索引，并允许用户
 * 查看或修改该视频流的多种属性，如预乘Alpha、颜色空间、颜色范围、
 * 隔行扫描方式、像素宽高比，以及针对图像序列的开始/结束时间和帧率。
 *
 * 更改通过内部的撤销命令类 (VideoStreamChangeCommand, ImageSequenceChangeCommand)
 * 进行管理，以支持撤销/重做功能。
 */
class VideoStreamProperties : public StreamProperties {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 VideoStreamProperties 对象。
   * @param footage 指向包含此视频流的 Footage 对象的指针。
   * @param video_index 此视频流在 `footage` 对象中的索引。
   */
  VideoStreamProperties(Footage* footage, int video_index);

  // ~VideoStreamProperties() override; // 默认析构函数通常足够

  /**
   * @brief 应用或接受对视频流属性所做的更改。
   *
   * 此方法是 StreamProperties 基类中定义的虚函数的覆盖实现。
   * 它负责将用户在属性编辑器中所做的修改持久化，并创建相应的撤销命令。
   *
   * @param parent 指向 MultiUndoCommand 对象的指针，用于聚合此属性更改产生的撤销命令。
   */
  void Accept(MultiUndoCommand* parent) override;

  /**
   * @brief 对当前UI控件中的用户输入或设置进行有效性检查。
   *
   * 此方法是 StreamProperties 基类中定义的虚函数的覆盖实现。
   * 用于确保用户输入的视频流参数是合理和有效的。
   *
   * @return bool 如果所有设置都通过了有效性检查，则返回 true；否则返回 false。
   */
  bool SanityCheck() override;

 private:
  /**
   * @brief 指向包含此视频流的 Footage 对象的指针。
   */
  Footage* footage_;

  /**
   * @brief 此视频流在 `footage_` 对象中的索引。
   */
  int video_index_;

  /**
   * @brief 用于设置关联/预乘 Alpha 的复选框。
   */
  QCheckBox* video_premultiply_alpha_;

  /**
   * @brief 用于设置此媒体颜色空间的下拉框。
   * (例如，选择 OCIO 颜色空间)
   */
  QComboBox* video_color_space_; // 具体类型可能是自定义的 ColorSpaceComboBox

  /**
   * @brief 用于设置此流颜色范围的下拉框。
   * (例如，TV/Limited Range vs PC/Full Range)
   */
  QComboBox* color_range_combo_; // 具体类型可能是自定义的 ColorRangeComboBox

  /**
   * @brief 用于设置视频隔行扫描方式的下拉框 (自定义控件)。
   */
  InterlacedComboBox* video_interlace_combo_;

  /**
   * @brief 用于设置图像序列起始帧号/时间的整数滑块。
   */
  IntegerSlider* imgseq_start_time_;

  /**
   * @brief 用于设置图像序列结束帧号/时间的整数滑块。
   * (注意：变量名是 imgseq_end_time_，但通常图像序列定义是 起始+时长 或 起始+结束。
   * 如果这里确实是“结束时间”，其与“时长”的关系需要在实现中明确。)
   */
  IntegerSlider* imgseq_end_time_; // 或者可能是 imgseq_duration_slider_

  /**
   * @brief 用于设置图像序列帧率的下拉框 (自定义控件)。
   */
  FrameRateComboBox* imgseq_frame_rate_;

  /**
   * @brief 用于设置流的像素宽高比的下拉框 (自定义控件)。
   */
  PixelAspectRatioComboBox* pixel_aspect_combo_;

  /**
   * @brief 用于处理视频流主要属性（预乘Alpha、颜色空间、隔行扫描、像素宽高比、颜色范围）更改的撤销命令类。
   */
  class VideoStreamChangeCommand : public UndoCommand {
   public:
    /**
     * @brief 构造一个新的 VideoStreamChangeCommand 对象。
     * @param footage 指向关联的 Footage 对象。
     * @param video_index 视频流的索引。
     * @param premultiplied 新的预乘 Alpha 状态。
     * @param colorspace 新的颜色空间名称。
     * @param interlacing 新的隔行扫描方式。
     * @param pixel_ar 新的像素宽高比。
     * @param range 新的颜色范围。
     */
    VideoStreamChangeCommand(Footage* footage, int video_index, bool premultiplied, QString colorspace,
                             VideoParams::Interlacing interlacing, const rational& pixel_ar,
                             VideoParams::ColorRange range);

    /**
     * @brief 获取与此命令相关的项目对象。
     * @return Project* 指向相关 Project 对象的指针。
     */
    [[nodiscard]] Project* GetRelevantProject() const override;

   protected:
    /**
     * @brief 执行（或重做）更改视频流属性的操作。
     */
    void redo() override;
    /**
     * @brief 撤销更改视频流属性的操作，恢复到旧值。
     */
    void undo() override;

   private:
    /** @brief 指向关联的 Footage 对象。 */
    Footage* footage_;
    /** @brief 视频流的索引。 */
    int video_index_;

    /** @brief 新的预乘 Alpha 状态。 */
    bool new_premultiplied_;
    /** @brief 新的颜色空间名称。 */
    QString new_colorspace_;
    /** @brief 新的隔行扫描方式。 */
    VideoParams::Interlacing new_interlacing_;
    /** @brief 新的像素宽高比。 */
    rational new_pixel_ar_;
    /** @brief 新的颜色范围。 */
    VideoParams::ColorRange new_range_;

    /** @brief 旧的预乘 Alpha 状态 (值初始化)。 */
    bool old_premultiplied_{};
    /** @brief 旧的颜色空间名称。 */
    QString old_colorspace_;
    /** @brief 旧的隔行扫描方式。 */
    VideoParams::Interlacing old_interlacing_{VideoParams::kInterlaceNone}; // 假设默认值
    /** @brief 旧的像素宽高比。 */
    rational old_pixel_ar_;
    /** @brief 旧的颜色范围。 */
    VideoParams::ColorRange old_range_; // 假设默认值
  };

  /**
   * @brief 用于处理图像序列特定属性（起始索引、时长、帧率）更改的撤销命令类。
   */
  class ImageSequenceChangeCommand : public UndoCommand {
   public:
    /**
     * @brief 构造一个新的 ImageSequenceChangeCommand 对象。
     * @param footage 指向关联的 Footage 对象。
     * @param video_index 视频流的索引（通常图像序列只有一个视频流）。
     * @param start_index 新的图像序列起始索引/帧号。
     * @param duration 新的图像序列时长（以帧数或时间单位表示，取决于实现）。
     * @param frame_rate 新的图像序列帧率。
     */
    ImageSequenceChangeCommand(Footage* footage, int video_index, int64_t start_index, int64_t duration,
                               const rational& frame_rate);

    /**
     * @brief 获取与此命令相关的项目对象。
     * @return Project* 指向相关 Project 对象的指针。
     */
    [[nodiscard]] Project* GetRelevantProject() const override;

   protected:
    /**
     * @brief 执行（或重做）更改图像序列属性的操作。
     */
    void redo() override;
    /**
     * @brief 撤销更改图像序列属性的操作，恢复到旧值。
     */
    void undo() override;

   private:
    /** @brief 指向关联的 Footage 对象。 */
    Footage* footage_;
    /** @brief 视频流的索引。 */
    int video_index_;

    /** @brief 新的图像序列起始索引。 */
    int64_t new_start_index_;
    /** @brief 旧的图像序列起始索引 (值初始化)。 */
    int64_t old_start_index_{};

    /** @brief 新的图像序列时长。 */
    int64_t new_duration_;
    /** @brief 旧的图像序列时长 (值初始化)。 */
    int64_t old_duration_{};

    /** @brief 新的图像序列帧率。 */
    rational new_frame_rate_;
    /** @brief 旧的图像序列帧率。 */
    rational old_frame_rate_;
  };
};

}  // namespace olive

#endif  // VIDEOSTREAMPROPERTIES_H