#ifndef PREVIEWAUDIODEVICE_H // 防止头文件被重复包含的宏
#define PREVIEWAUDIODEVICE_H // 定义 PREVIEWAUDIODEVICE_H 宏

#include "previewautocacher.h" // 包含 PreviewAutoCacher 相关的定义
                               // (虽然在此头文件中 PreviewAutoCacher 未被直接使用，
                               //  但 PreviewAudioDevice 的设计可能与其协同工作)
// 假设 QIODevice, QMutex, QByteArray 等 Qt 类已通过标准方式被包含。

namespace olive { // olive 项目的命名空间

/**
 * @brief PreviewAudioDevice 类是一个自定义的 QIODevice，专门用于音频预览播放。
 *
 * 它作为一个音频数据的提供者，可以被 Qt 的音频输出系统 (如 QAudioSink) 使用。
 * 这个设备内部维护一个缓冲区 (`buffer_`)，音频数据可以写入此缓冲区，然后由音频输出系统读取播放。
 *
 * 主要功能和特点：
 * - 继承自 QIODevice，实现了 `readData` 和 `writeData` 等核心IO接口。
 * - 线程安全：使用 QMutex (`lock_`) 来保护内部缓冲区的并发访问。
 * - 通知机制：可以通过 `Notify` 信号来通知有新数据可读或需要更多数据。
 * - 缓冲管理：管理内部的字节缓冲区 (`buffer_`)，处理数据的写入和读取。
 * - `bytes_per_frame_`: 定义了每个音频帧 (通常是所有通道的一个采样点) 所占的字节数。
 * - `notify_interval_`: 可能用于控制 `Notify` 信号发出的频率或条件。
 *
 * 这个类通常与音频渲染和缓存系统配合，渲染线程将生成的音频PCM数据写入此设备，
 * 而音频播放线程从此设备读取数据并发送到声卡。
 */
class PreviewAudioDevice : public QIODevice { // PreviewAudioDevice 继承自 QIODevice
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  /**
   * @brief 构造函数。
   * @param parent 父对象指针，默认为 nullptr。
   */
  explicit PreviewAudioDevice(QObject *parent = nullptr);

  // 析构函数
  ~PreviewAudioDevice() override;

  /**
   * @brief 开始排队 (或准备接收) 音频数据。
   * 可能会重置内部状态或缓冲区。
   */
  void StartQueuing();

  /**
   * @brief (重写 QIODevice::isSequential) 指示此设备是否为顺序访问设备。
   * 对于音频流，通常是顺序的。
   * @return 通常返回 true。
   */
  [[nodiscard]] bool isSequential() const override;

  /**
   * @brief (重写 QIODevice::readData) 从内部缓冲区读取音频数据。
   * 音频输出系统会调用此方法来获取要播放的数据。
   * @param data 指向用于存储读取数据的字符数组的指针。
   * @param maxSize 可以读取的最大字节数。
   * @return 返回实际读取的字节数。如果无数据可读或发生错误，可能返回0或-1。
   */
  qint64 readData(char *data, qint64 maxSize) override;

  /**
   * @brief (重写 QIODevice::writeData) 将音频数据写入到内部缓冲区。
   * 音频渲染或缓存系统会调用此方法来提供PCM数据。
   * @param data 指向要写入的音频数据的字符数组的指针。
   * @param length 要写入的字节数。
   * @return 返回实际写入的字节数。
   */
  qint64 writeData(const char *data, qint64 length) override;

  /**
   * @brief 获取每个音频帧 (所有通道的一个采样点) 所占的字节数。
   * 例如，对于16位立体声，一个音频帧是 2通道 * 2字节/通道 = 4字节。
   * @return 返回每个音频帧的字节数。
   */
  [[nodiscard]] int bytes_per_frame() const { return bytes_per_frame_; }

  /**
   * @brief 设置每个音频帧的字节数。
   * @param b 新的字节数。
   */
  void set_bytes_per_frame(int b) { bytes_per_frame_ = b; }

  /**
   * @brief 设置通知间隔。
   * 这可能与 `Notify` 信号的发出条件有关，例如，每当读取或写入一定量的字节后发出通知。
   * @param i 通知间隔 (单位可能是字节数或其他)。
   */
  void set_notify_interval(qint64 i) { notify_interval_ = i; }

  /**
   * @brief 清空内部缓冲区和相关状态。
   */
  void clear();

 signals: // Qt 信号声明
  /**
   * @brief 通知信号。
   * 可能在以下情况下发出：
   * - 当有足够的新数据可供读取时。
   * - 当缓冲区空间已满或需要更多数据时 (由 writeData 触发)。
   * - 达到通知间隔 (`notify_interval_`) 时。
   * 音频播放和数据提供逻辑会响应此信号。
   */
  void Notify();

 private:
  QMutex lock_; // 互斥锁，用于保护对内部缓冲区 `buffer_` 和其他共享状态的线程安全访问

  QByteArray buffer_; // 内部字节数组，用作音频数据的环形缓冲区或队列

  int bytes_per_frame_{}; // 每个音频帧的字节数 (例如，采样大小 * 通道数)

  qint64 notify_interval_; // 通知间隔，可能用于控制 Notify() 信号的发出频率

  qint64 bytes_read_; // 已从缓冲区读取的总字节数 (或类似的计数器，用于通知逻辑)
};

}  // namespace olive

#endif  // PREVIEWAUDIODEVICE_H