#ifndef FOOTAGE_H  // 防止头文件被多次包含的宏定义开始
#define FOOTAGE_H

#include <olive/core/core.h>  // Olive 核心库的包含，可能定义了 rational 等基础类型
#include <QDateTime>          // Qt 日期时间类，用于时间戳
#include <QList>              // Qt 列表容器

#include "codec/decoder.h"              // 解码器相关定义，用于探测和解码媒体文件
#include "footagedescription.h"         // 素材描述相关定义 (可能是一个结构体或类)
#include "node/output/viewer/viewer.h"  // 引入基类 ViewerOutput 的定义
#include "render/cancelatom.h"          // 用于取消操作的原子类
#include "render/videoparams.h"         // 视频参数定义 (AudioParams 和 SubtitleParams 可能也在此或类似文件中)
// (实际上 VideoParams, AudioParams, SubtitleParams 分别在自己的头文件中，但这里遵循原文)

// 可能需要的前向声明
// class QVariant;
// class QXmlStreamReader;
// class QXmlStreamWriter;
// class SerializedData;
// class TimeRange;
// enum class LoopMode;
// struct NodeValueRow;
// struct NodeGlobals;
// class NodeValueTable;
// class Project;

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 代表项目中对外部媒体文件的引用及其元数据。
 * Footage 对象有两个主要目的：存储外部媒体的元数据，并作为项目中的一个可管理项。
 * 它内部存储了一系列流对象 (Stream，通常通过 VideoParams, AudioParams, SubtitleParams 表示)，
 * 这些流对象保存了大部分视频/音频元数据，与媒体文件中的流数据相对应。
 * Footage 继承自 ViewerOutput，意味着它可以被直接连接到查看器或用作其他节点的输入源。
 */
class Footage : public ViewerOutput {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief Footage 构造函数。
      * @param filename (可选) 要引用的媒体文件的路径。如果为空，则创建一个空的 Footage 对象。
      */
     explicit Footage(const QString &filename = QString());

  NODE_DEFAULT_FUNCTIONS(Footage)  // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /** @brief 获取此素材节点的名称 (例如 "媒体")，支持国际化。 */
  [[nodiscard]] QString Name() const override { return tr("Media"); }
  /** @brief 获取此素材节点的唯一标识符。 */
  [[nodiscard]] QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.footage"); }
  /** @brief 获取此节点所属的分类 ID 列表 (通常是 "项目")。 */
  [[nodiscard]] QVector<CategoryID> Category() const override { return {kCategoryProject}; }
  /** @brief 获取此素材节点的描述信息 (例如 "将视频、音频或静态图像文件导入到合成中。")，支持国际化。 */
  [[nodiscard]] QString Description() const override {
    return tr("Import video, audio, or still image files into the composition.");
  }

  /** @brief 当界面语言等需要重新翻译时调用。 */
  void Retranslate() override;

  /**
   * @brief 重置 Footage 对象的状态，准备重新进行探测 (Probe)。
   * 如果 Footage 对象需要被重新探测（例如源文件改变或链接到新文件），其状态需要重置，
   * 以便 Decoder::Probe() 函数能够准确地反映源文件。Clear() 会将 Footage 对象的状态
   * 重置为刚创建时的状态（保留文件名）。
   * 在大多数情况下，重新探测会使用 olive::ProbeMedia()，它已经调用了 Clear()。
   */
  void Clear();

  /** @brief 检查此 Footage 对象是否有效（即是否已成功探测并加载了媒体信息）。 */
  [[nodiscard]] bool IsValid() const { return valid_; }

  /**
   * @brief 将此 Footage 对象标记为有效并准备好使用。
   * 通常在成功探测媒体文件后调用。
   */
  void SetValid();

  /**
   * @brief 返回此 Footage 对象当前引用的文件名（完整路径）。
   */
  [[nodiscard]] QString filename() const;

  /**
   * @brief 设置此 Footage 对象引用的文件名。
   * 注意：这不会自动清除旧的流信息并为新文件重新探测。如果文件链接已更改，需要手动执行这些操作。
   * @param s 新的文件名（完整路径）。
   */
  void set_filename(const QString &s);

  /**
   * @brief 获取文件的最后修改时间戳。
   * 文件的时间戳被存储起来，可能用于在项目浏览器中进行组织。
   * @return const qint64& 对时间戳（通常是自 epoch以来的毫秒数）的常量引用。
   */
  [[nodiscard]] const qint64 &timestamp() const;

  /**
   * @brief 设置文件的最后修改时间戳。
   * 通常只应在导入或替换文件时执行此操作。
   * @param t 新的时间戳。
   */
  void set_timestamp(const qint64 &t);

  /**
   * @brief 设置一个取消原子指针，用于在耗时操作（如探测或解码）中检查是否需要取消。
   * @param c 指向 CancelAtom 对象的指针。
   */
  void SetCancelPointer(CancelAtom *c) { cancelled_ = c; }

  /**
   * @brief 根据流类型和在该类型中的索引，获取其在内部所有流列表中的“真实”索引。
   * @param type 流类型 (视频, 音频, 字幕)。
   * @param index 该类型流的索引 (例如，第二个视频流的索引是1)。
   * @return int 在内部统一流列表中的索引，如果找不到则为 -1。
   */
  [[nodiscard]] int GetStreamIndex(Track::Type type, int index) const;
  /**
   * @brief 根据轨道引用获取其在内部所有流列表中的“真实”索引。
   * @param ref 轨道引用。
   * @return int 真实索引。
   */
  [[nodiscard]] int GetStreamIndex(const Track::Reference &ref) const {
    return GetStreamIndex(ref.type(), ref.index());
  }

  /**
   * @brief 根据内部流列表中的“真实”索引，获取其对应的轨道引用 (类型和类型内索引)。
   * @param real_index 内部统一流列表中的索引。
   * @return Track::Reference 对应的轨道引用，如果索引无效则返回无效引用。
   */
  [[nodiscard]] Track::Reference GetReferenceFromRealIndex(int real_index) const;

  /**
   * @brief 获取在探测此 Footage 时设置的解码器的 ID。
   * @return const QString& 解码器 ID 的常量引用。
   */
  [[nodiscard]] const QString &decoder() const;

  /** @brief (静态工具函数) 生成视频流的描述字符串。 */
  static QString DescribeVideoStream(const VideoParams &ms);
  /** @brief (静态工具函数) 生成音频流的描述字符串。 */
  static QString DescribeAudioStream(const AudioParams &ms);
  /** @brief (静态工具函数) 生成字幕流的描述字符串。 */
  static QString DescribeSubtitleStream(const SubtitleParams &ms);

  /**
   * @brief 计算并输出节点在特定时间点的值（解码后的视频帧或音频样本）。
   * @param value 当前输入行数据 (通常不直接使用，因为 Footage 是源节点)。
   * @param globals 全局节点处理参数 (包含请求的时间、分辨率等)。
   * @param table 用于存储输出值的表 (包含解码后的数据)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  /** @brief (静态工具函数) 根据流类型获取其名称字符串。 */
  static QString GetStreamTypeName(Track::Type type);

  /** @brief 获取连接到纹理输出的上游节点 (对于 Footage，这通常是自身或内部解码器)。 */
  Node *GetConnectedTextureOutput() override;
  /** @brief 获取连接到音频样本输出的上游节点。 */
  Node *GetConnectedSampleOutput() override;

  /**
   * @brief (静态工具函数) 根据循环模式调整时间。
   * @param time 原始时间。
   * @param loop_mode 循环模式。
   * @param length 素材的长度。
   * @param type 流类型 (影响时间基准的处理方式)。
   * @param timebase 时间基准。
   * @return rational 调整后的时间。
   */
  static rational AdjustTimeByLoopMode(
      rational time, LoopMode loop_mode, const rational &length, VideoParams::Type type,
      const rational &timebase);  // VideoParams::Type 可能是笔误，应为 Track::Type 或类似

  /** @brief 获取特定类型的数据。 */
  [[nodiscard]] QVariant data(const DataType &d) const override;

  /** @brief 获取此素材包含的总流数量 (所有类型)。 */
  [[nodiscard]] int GetTotalStreamCount() const override { return total_stream_count_; }

  /** @brief 从 XML 流加载素材节点的自定义数据 (文件名、时间戳、流信息等)。 */
  bool LoadCustom(QXmlStreamReader *reader, SerializedData *data) override;
  /** @brief 将素材节点的自定义数据保存到 XML 流。 */
  void SaveCustom(QXmlStreamWriter *writer) const override;

  static const QString kFilenameInput;  ///< "Filename" - 存储文件路径的输入参数键名。

  /** @brief 当此节点添加到项目图时调用。 */
  void AddedToGraphEvent(Project *p) override;
  /** @brief 当此节点从项目图移除时调用。 */
  void RemovedFromGraphEvent(Project *p) override;

 protected:
  /** @brief 当节点的输入参数值改变时调用。 */
  void InputValueChangedEvent(const QString &input, int element) override;

  /** @brief (虚保护函数) 验证并返回特定类型流的内部计算长度。 */
  [[nodiscard]] rational VerifyLengthInternal(Track::Type type) const override;

 private:
  /** @brief 根据视频参数获取应使用的色彩空间。 */
  [[nodiscard]] QString GetColorspaceToUse(const VideoParams &ms) const;

  /** @brief 重新探测当前文件名对应的媒体文件。 */
  void Reprobe();

  /** @brief (静态工具函数) 合并两个视频流的参数 (例如，基础参数和覆盖参数)。 */
  static VideoParams MergeVideoStream(const VideoParams &base, const VideoParams &over);

  qint64 timestamp_;        ///< 文件的最后修改时间戳。
  QString decoder_;         ///< 用于此素材的解码器的 ID。
  bool valid_;              ///< 标记此素材是否有效。
  CancelAtom *cancelled_;   ///< 指向取消原子的指针，用于中断操作。
  int total_stream_count_;  ///< 此素材包含的总流数量。

 private slots:
  /** @brief 检查素材文件状态的槽函数 (例如，文件是否存在或已更改)。 */
  void CheckFootage();
  /** @brief 当全局默认色彩空间设置更改时调用的槽函数。 */
  void DefaultColorSpaceChanged();
};

}  // namespace olive

#endif  // FOOTAGE_H // 头文件宏定义结束