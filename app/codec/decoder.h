#ifndef DECODER_H
#define DECODER_H

#include <atomic>   // 为了 std::atomic_int64_t
#include <memory>   // 为了 std::shared_ptr
#include <utility>  // 为了 std::move

extern "C" {
#include <libswresample/swresample.h>  // SwrContext 通常与音频重采样相关
}

#include <stdint.h>
#include <QFileInfo>
#include <QMutex>
#include <QObject>
#include <QString>      // 为了 QString
#include <QStringList>  // 如果需要 (当前未使用)
#include <QVector>      // 为了 QVector
#include <QWaitCondition>

#include "common/define.h"  // 包含 olive::rational, olive::PixelFormat, olive::AudioParams, olive::SampleBuffer, olive::TimeRange, olive::TexturePtr 等核心类型
#include "node/block/block.h"                         // 包含 Block
#include "node/project/footage/footagedescription.h"  // 包含 FootageDescription
#include "render/cancelatom.h"                        // 包含 CancelAtom
#include "render/rendermodes.h"                       // 包含 RenderMode::Mode 和 LoopMode

namespace olive {

class Decoder;
/**
 * @brief 指向 Decoder 对象的共享指针类型定义。
 */
using DecoderPtr = std::shared_ptr<Decoder>;

/**
 * @brief 为 Decoder 派生类提供默认析构函数实现的宏。
 * 该析构函数会调用 CloseInternal() 来确保资源被释放。
 * @param x 派生类的名称。
 */
#define DECODER_DEFAULT_DESTRUCTOR(x) \
  virtual ~x() override { CloseInternal(); }

/**
 * @brief 解码器基类，是将外部媒体导入 Olive 的主要接口。
 *
 * 解码器的主要职责是作为编解码器/解码库的抽象层，并提供完整的帧数据。
 * 这些帧可以是视频或音频数据，并以共享指针包装的 Frame 对象形式提供，
 * 以减轻内存管理的负担。
 *
 * 解码器的核心功能是 Retrieve()，它应返回完整的图像/音频数据。解码器应
 * 为应用程序的其他部分屏蔽所有编解码器压缩的复杂性（即解码器不应返回
 * 部分帧，也不应要求系统的其他部分直接与编解码器接口交互）。
 * 这通常需要预先缓存、索引甚至完全转码媒体，这些功能可以通过 Analyze()
 * 函数（此处未直接定义，但暗示了其概念）来实现。
 *
 * 解码器本身不执行任何像素/样本格式转换。帧数据应通过 PixelService
 * (通常指颜色转换或像素格式转换服务) 处理后才能在渲染管线的其余部分使用。
 */
class Decoder : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief 表示解码器检索状态的枚举。
   */
  enum RetrieveState {
    kReady,            ///< @brief 解码器已准备就绪，可以进行检索。
    kFailedToOpen,     ///< @brief 解码器打开媒体文件失败。
    kIndexUnavailable  ///< @brief 媒体索引不可用或正在生成，暂时无法检索。
  };

  /**
   * @brief 构造一个 Decoder 对象。
   */
  Decoder();

  /**
   * @brief 获取解码器的唯一标识符。
   * 派生类必须实现此纯虚函数。
   * @return QString 解码器的唯一ID。
   */
  [[nodiscard]] virtual QString id() const = 0;

  /**
   * @brief 检查此解码器是否支持视频解码。
   * @return bool 如果支持视频则返回 true，默认为 false。
   */
  virtual bool SupportsVideo() { return false; }
  /**
   * @brief 检查此解码器是否支持音频解码。
   * @return bool 如果支持音频则返回 true，默认为 false。
   */
  virtual bool SupportsAudio() { return false; }

  /**
   * @brief 增加解码器的访问时间计数。
   * @param t 要增加的时间量 (通常未使用，实际增加为当前时间)。
   */
  void IncrementAccessTime(qint64 t);

  /**
   * @brief 描述编解码器流的类，包含文件名、流索引和关联的 Block。
   */
  class CodecStream {
   public:
    /**
     * @brief 默认构造函数，创建一个无效的 CodecStream。
     */
    CodecStream() : stream_(-1), block_(nullptr) {}

    /**
     * @brief 使用文件名、流索引和 Block 指针构造一个 CodecStream。
     * @param filename 媒体文件名。
     * @param stream 流在文件中的索引。
     * @param block 指向关联的 Block 对象的指针。
     */
    CodecStream(QString filename, int stream, Block* block)
        : filename_(std::move(filename)), stream_(stream), block_(block) {}

    /**
     * @brief 检查此 CodecStream 是否有效。
     * 有效条件是文件名非空且流索引大于等于0。
     * @return bool 如果有效则返回 true，否则返回 false。
     */
    [[nodiscard]] bool IsValid() const { return !filename_.isEmpty() && stream_ >= 0; }

    /**
     * @brief 检查关联的文件是否存在于文件系统中。
     * @return bool 如果文件存在则返回 true，否则返回 false。
     */
    [[nodiscard]] bool Exists() const { return QFileInfo::exists(filename_); }

    /**
     * @brief 将当前 CodecStream 重置为无效状态。
     */
    void Reset() { *this = CodecStream(); }

    /**
     * @brief 比较两个 CodecStream 对象是否相等。
     * 基于文件名和流索引进行比较。
     * @param rhs 要比较的另一个 CodecStream 对象。
     * @return bool 如果相等则返回 true，否则返回 false。
     */
    bool operator==(const CodecStream& rhs) const { return filename_ == rhs.filename_ && stream_ == rhs.stream_; }

    /**
     * @brief 获取媒体文件名。
     * @return const QString& 对文件名的常量引用。
     */
    [[nodiscard]] const QString& filename() const { return filename_; }

    /**
     * @brief 获取流索引。
     * @return int 流索引。
     */
    [[nodiscard]] int stream() const { return stream_; }

    /**
     * @brief 获取关联的 Block 对象指针。
     * @return Block* 指向 Block 对象的指针。
     */
    [[nodiscard]] Block* block() const { return block_; }

   private:
    /**
     * @brief 媒体文件名。
     */
    QString filename_;

    /**
     * @brief 流在文件中的索引。
     */
    int stream_;

    /**
     * @brief 指向关联的 Block 对象的指针。
     */
    Block* block_;
  };

  /**
   * @brief 打开指定的编解码器流以进行解码。
   *
   * 此函数是线程安全的。
   *
   * @param stream 要打开的 CodecStream 对象。
   * @return bool 如果流成功打开，则返回 true。如果解码器已打开且流与提供的流相同，也返回 true。
   * 如果无法打开流，或者解码器已打开但流不同，则返回 false。
   */
  bool Open(const CodecStream& stream);

  /**
   * @brief 表示任意时间码的常量，通常用于请求最新或最合适的帧。
   */
  static const rational kAnyTimecode;

  /**
   * @brief 检索视频帧时使用的参数结构体。
   */
  struct RetrieveVideoParams {
    Renderer* renderer = nullptr;  ///< @brief 指向渲染器的指针，可能用于特定渲染上下文。
    rational time;                 ///< @brief 请求的视频帧的时间戳。
    int divider =
        1;  ///< @brief 视频分辨率的除数，用于请求较低分辨率的预览 (例如，1 表示完整分辨率，2 表示一半分辨率)。
    PixelFormat maximum_format =
        PixelFormat(PixelFormat::INVALID);  ///< @brief 请求的最大像素格式，用于限制解码输出的格式。
    CancelAtom* cancelled = nullptr;        ///< @brief 指向 CancelAtom 的指针，用于在操作过程中检查是否已请求取消。
    VideoParams::ColorRange force_range = VideoParams::kColorRangeDefault;   ///< @brief 强制使用的颜色范围。
    VideoParams::Interlacing src_interlacing = VideoParams::kInterlaceNone;  ///< @brief 源视频的隔行扫描模式。
  };

  /**
   * @brief 从素材中检索一个视频帧。
   *
   * 此函数总是返回一个有效的纹理指针 (TexturePtr)，除非发生致命错误（此时返回 nullptr）。
   * 如果请求的时间码在素材开始之前，此函数应返回第一帧。
   * 同样，如果时间码在素材结束之后，则应返回最后一帧。
   *
   * 此函数是线程安全的，并且只能在解码器打开时运行。\see Open()
   * @param p 包含检索视频帧所需参数的 RetrieveVideoParams 结构体。
   * @return TexturePtr 指向包含解码后视频帧数据的纹理的共享指针。
   */
  TexturePtr RetrieveVideo(const RetrieveVideoParams& p);

  /**
   * @brief 表示检索音频数据状态的枚举。
   */
  enum RetrieveAudioStatus {
    kInvalid = -1,       ///< @brief 无效状态或发生错误。
    kOK,                 ///< @brief 音频数据成功检索。
    kWaitingForConform,  ///< @brief 正在等待音频适配完成。
    kUnknownError        ///< @brief 发生未知错误。
  };

  /**
   * @brief 从素材中检索音频数据。
   *
   * 此函数总是填充目标样本缓冲区 (dest)，除非发生致命错误（此时返回非kOK状态）。
   * SampleBuffer 应始终具有所提供范围所需的足够音频数据。
   *
   * 此函数是线程安全的，并且只能在解码器打开时运行。\see Open()
   * @param dest 用于存储检索到的音频数据的 SampleBuffer 对象（输出参数）。
   * @param range 请求的音频数据的时间范围。
   * @param params 请求的音频参数（如采样率、格式）。
   * @param cache_path 音频适配文件的缓存路径。
   * @param loop_mode 循环模式，定义如何在到达素材末尾时处理音频。
   * @param mode 渲染模式，可能影响音频检索的行为。
   * @return RetrieveAudioStatus 表示检索操作状态的枚举值。
   */
  RetrieveAudioStatus RetrieveAudio(SampleBuffer& dest, const TimeRange& range, const AudioParams& params,
                                    const QString& cache_path, LoopMode loop_mode, RenderMode::Mode mode);

  /**
   * @brief 获取此解码器实例最后一次被以任何方式使用的时间。
   * @return qint64 最后访问时间的时间戳 (通常是自 epoch 以来的毫秒数)。
   */
  qint64 GetLastAccessedTime();

  /**
   * @brief 从文件中探测并生成一个 FootageDescription 对象。
   *
   * 如果此解码器能够解析该文件，它将返回一个有效的 FootageDescription。
   * 否则，它将返回一个无效的 FootageDescription (通常通过 IsValid() 判断)。
   *
   * 对于子类，此函数应在逻辑上是静态的。虽然 C++ 不支持虚静态函数，
   * 但它在运行时不应持有或访问任何实例状态。
   *
   * 此函数是可重入的。
   * 派生类必须实现此纯虚函数。
   * @param filename 要探测的文件名。
   * @param cancelled 指向 CancelAtom 的指针，用于在操作过程中检查是否已请求取消。
   * @return FootageDescription 包含探测到的素材信息的对象。
   */
  virtual FootageDescription Probe(const QString& filename, CancelAtom* cancelled) const = 0;

  /**
   * @brief 关闭媒体文件并释放已分配的内存。
   *
   * 此函数是线程安全的，并且只能在解码器打开时运行。\see Open()
   */
  void Close();

  /**
   * @brief 对音频流进行适配（转码/重采样）。
   * @param output_filenames 适配后输出文件的目标路径列表。
   * @param params 目标音频参数。
   * @param cancelled 指向 CancelAtom 的指针，用于在操作过程中检查是否已请求取消 (可选)。
   * @return bool 如果适配成功则返回 true，否则返回 false。
   */
  bool ConformAudio(const QVector<QString>& output_filenames, const AudioParams& params,
                    CancelAtom* cancelled = nullptr);

  /**
   * @brief 使用解码器 ID 创建一个 Decoder 实例。
   * @param id 要创建的解码器的唯一标识符。
   * @return DecoderPtr 指向创建的 Decoder 实例的共享指针，如果具有此 ID 的解码器不存在则为 nullptr。
   */
  static DecoderPtr CreateFromID(const QString& id);

  /**
   * @brief 转换图像序列文件名，将占位符替换为指定的编号。
   * 例如，将 "frame_####.png" 和编号 123 转换为 "frame_0123.png"。
   * @param filename 包含占位符（如 '#' 或 '%d'）的文件名模板。
   * @param number 要替换到文件名中的帧编号。
   * @return QString 转换后的完整文件名。
   */
  static QString TransformImageSequenceFileName(const QString& filename, const int64_t& number);

  /**
   * @brief 获取图像序列文件名中数字占位符的位数。
   * @param filename 图像序列的文件名模板。
   * @return int 数字占位符的位数。
   */
  static int GetImageSequenceDigitCount(const QString& filename);

  /**
   * @brief 从图像序列的单个文件名中提取帧索引号。
   * @param filename 图像序列中的一个文件名。
   * @return int64_t 提取到的帧索引号。
   */
  static int64_t GetImageSequenceIndex(const QString& filename);

  /**
   * @brief 获取系统中所有已注册解码器的列表。
   * @return QVector<DecoderPtr> 包含所有可用解码器实例共享指针的 QVector。
   */
  static QVector<DecoderPtr> ReceiveListOfAllDecoders();

 protected:
  /**
   * @brief 内部打开函数，供子类实现。
   *
   * 子类必须覆盖此函数。此函数在调用时已被互斥锁保护，因此无需担心线程安全问题。
   * 在调用此函数之前，许多其他健全性检查已经完成，因此子类只需关注其自身的打开逻辑。
   * 可以保证解码器尚未打开，并且素材流信息来自该子类的 Probe() 函数。
   *
   * @return bool 如果一切打开成功且解码器准备就绪，则返回 true。否则返回 false。
   * 如果此函数返回 false，Decoder 基类将调用 CloseInternal() 来清理在 OpenInternal() 期间分配的任何内存。
   */
  virtual bool OpenInternal() = 0;

  /**
   * @brief 内部关闭函数，供子类实现。
   *
   * 子类必须覆盖此函数。函数应能安全地清除所有已分配的内存。
   * 即使 Open() 未完成或 RetrieveVideo() 从未被调用，此函数也可能被调用。
   */
  virtual void CloseInternal() = 0;

  /**
   * @brief 内部帧检索函数，供支持视频的子类实现。
   *
   * 如果子类支持视频解码，则必须覆盖此函数。
   * 此函数在调用时已被互斥锁保护，因此子类无需担心线程安全问题。
   * @param p 包含检索视频帧所需参数的 RetrieveVideoParams 结构体。
   * @return TexturePtr 指向解码后视频帧数据的纹理的共享指针。
   */
  virtual TexturePtr RetrieveVideoInternal(const RetrieveVideoParams& p);

  /**
   * @brief 内部音频适配函数，供子类实现。
   * @param filenames 适配后输出文件的目标路径列表。
   * @param params 目标音频参数。
   * @param cancelled 指向 CancelAtom 的指针，用于检查是否已请求取消。
   * @return bool 如果适配成功则返回 true，否则返回 false。
   */
  virtual bool ConformAudioInternal(const QVector<QString>& filenames, const AudioParams& params,
                                    CancelAtom* cancelled);

  /**
   * @brief 发送处理进度信号。
   * @param ts 当前处理到的时间戳。
   * @param duration 总时长或当前处理块的时长。
   */
  void SignalProcessingProgress(int64_t ts, int64_t duration);

  /**
   * @brief 返回当前打开的流。
   *
   * 此函数不是线程安全的，因此只能由已确保线程安全的函数调用。
   * @return const CodecStream& 对当前打开的 CodecStream 的常量引用。
   */
  [[nodiscard]] const CodecStream& stream() const { return stream_; }

  /**
   * @brief 获取音频流的起始偏移量。
   * 派生类可以覆盖此方法以提供特定于格式的音频起始偏移。
   * @return rational 音频流的起始偏移，默认为0。
   */
  [[nodiscard]] virtual rational GetAudioStartOffset() const { return rational(0); }

 signals:
  /**
   * @brief 在进行索引操作期间，如果可用，此信号将以百分比 (0-100, 含边界) 的形式提供进度。
   * @param progress 索引进度百分比。
   */
  void IndexProgress(double progress);

 private:
  /**
   * @brief 更新最后访问时间戳。
   */
  void UpdateLastAccessed();

  /**
   * @brief 从已适配的音频文件中检索音频数据。
   * @param sample_buffer 用于存储检索到的音频数据的 SampleBuffer 对象（输出参数）。
   * @param conform_filenames 已适配的音频文件名列表。
   * @param range 请求的音频数据的时间范围。
   * @param loop_mode 循环模式。
   * @param params 请求的音频参数。
   * @return bool 如果成功从适配文件检索到音频则返回 true，否则返回 false。
   */
  bool RetrieveAudioFromConform(SampleBuffer& sample_buffer, const QVector<QString>& conform_filenames, TimeRange range,
                                LoopMode loop_mode, const AudioParams& params);

  /**
   * @brief 当前打开的编解码器流。
   */
  CodecStream stream_;

  /**
   * @brief 用于保护解码器内部状态的互斥锁，确保线程安全。
   */
  QMutex mutex_;

  /**
   * @brief 原子类型，记录解码器最后一次被访问的时间戳。
   */
  std::atomic_int64_t last_accessed_;

  /**
   * @brief 缓存的最后一帧视频纹理。
   */
  TexturePtr cached_texture_;
  /**
   * @brief 缓存的最后一帧视频的时间戳。
   */
  rational cached_time_;
  /**
   * @brief 缓存的最后一帧视频的缩放除数。
   */
  int cached_divider_{};
};

/**
 * @brief 为 Decoder::CodecStream 类型提供 qHash 函数，使其能用于 QHash 等容器。
 * @param stream 要计算哈希值的 CodecStream 对象。
 * @param seed 哈希种子。
 * @return uint 计算得到的哈希值。
 */
uint qHash(const Decoder::CodecStream& stream, uint seed = 0);

}  // namespace olive

Q_DECLARE_METATYPE(olive::Decoder::RetrieveState)

#endif  // DECODER_H