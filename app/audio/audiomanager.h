#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <portaudio.h>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <memory>

#include "audio/audioprocessor.h"
#include "audiovisualwaveform.h"
#include "codec/ffmpeg/ffmpegencoder.h"
#include "common/define.h"
#include "render/audioplaybackcache.h"
#include "render/previewaudiodevice.h"

namespace olive {

/**
 * @brief 管理音频输入和输出的核心类。
 *
 * AudioManager 作为单例存在，负责处理音频设备的配置、
 * 音频流的播放和录制，以及与系统中其他音频相关组件的交互。
 * 它封装了 PortAudio 的功能，并提供了 Qt 友好的接口。
 */
class AudioManager : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief 创建 AudioManager 的单例实例。
   * 如果实例已存在，则此函数不执行任何操作。
   */
  static void CreateInstance();

  /**
   * @brief 销毁 AudioManager 的单例实例。
   * 释放相关资源。
   */
  static void DestroyInstance();

  /**
   * @brief 获取 AudioManager 的单例实例。
   * @return AudioManager* 指向 AudioManager 实例的指针。如果实例尚未创建，则可能返回 nullptr 或触发断言。
   */
  static AudioManager *instance();

  /**
   * @brief 设置输出通知的时间间隔。
   * @param n 通知间隔（单位未指定，通常为毫秒）。
   */
  void SetOutputNotifyInterval(int n);

  /**
   * @brief 将音频样本推送到输出设备进行播放。
   * @param params 要播放音频的参数。
   * @param samples 包含音频数据的 QByteArray。
   * @param error 指向 QString 的指针，用于在发生错误时存储错误信息（可选）。
   * @return bool 如果成功推送到输出则返回 true，否则返回 false。
   */
  bool PushToOutput(const AudioParams &params, const QByteArray &samples, QString *error = nullptr);

  /**
   * @brief 清除输出缓冲区中缓存的音频数据。
   */
  void ClearBufferedOutput();

  /**
   * @brief 停止当前的音频输出。
   */
  void StopOutput();

  /**
   * @brief 获取当前使用的 PortAudio 输出设备索引。
   * @return PaDeviceIndex 当前输出设备的索引。
   */
  [[nodiscard]] PaDeviceIndex GetOutputDevice() const { return output_device_; }

  /**
   * @brief 获取当前使用的 PortAudio 输入设备索引。
   * @return PaDeviceIndex 当前输入设备的索引。
   */
  [[nodiscard]] PaDeviceIndex GetInputDevice() const { return input_device_; }

  /**
   * @brief 设置要使用的 PortAudio 输出设备。
   * @param device 要设置的 PortAudio 输出设备的索引。
   */
  void SetOutputDevice(PaDeviceIndex device);

  /**
   * @brief 设置要使用的 PortAudio 输入设备。
   * @param device 要设置的 PortAudio 输入设备的索引。
   */
  void SetInputDevice(PaDeviceIndex device);

  /**
   * @brief 执行硬重置，重新初始化音频设备和流。
   * 通常在音频配置发生重大更改或出现严重错误时调用。
   */
  void HardReset();

  /**
   * @brief 开始从当前输入设备录制音频。
   * @param params 音频录制的编码参数。
   * @param error_str 指向 QString 的指针，用于在发生错误时存储错误信息（可选）。
   * @return bool 如果成功开始录制则返回 true，否则返回 false。
   */
  bool StartRecording(const EncodingParams &params, QString *error_str = nullptr);

  /**
   * @brief 停止当前的音频录制。
   */
  void StopRecording();

  /**
   * @brief 根据名称查找已配置的 PortAudio 设备（通常来自应用程序设置）。
   * @param is_output_device 如果为 true，则查找输出设备；否则查找输入设备。
   * @return PaDeviceIndex 找到的设备的 PortAudio 索引；如果未找到，则返回无效索引 (paNoDevice)。
   */
  static PaDeviceIndex FindConfigDeviceByName(bool is_output_device);

  /**
   * @brief 根据名称查找 PortAudio 设备。
   * @param s 要查找的设备名称。
   * @param is_output_device 如果为 true，则查找输出设备；否则查找输入设备。
   * @return PaDeviceIndex 找到的设备的 PortAudio 索引；如果未找到，则返回无效索引 (paNoDevice)。
   */
  static PaDeviceIndex FindDeviceByName(const QString &s, bool is_output_device);

  /**
   * @brief 将 olive::AudioParams 转换为 PortAudio 的 PaStreamParameters。
   * @param p 要转换的 olive::AudioParams 对象。
   * @param device 目标 PortAudio 设备的索引。
   * @return PaStreamParameters 转换后的 PortAudio 流参数。
   */
  static PaStreamParameters GetPortAudioParams(const AudioParams &p, PaDeviceIndex device);

 signals:
  /**
   * @brief 当音频输出有通知时发出此信号。
   * 通常基于 SetOutputNotifyInterval 设置的间隔定期发出。
   */
  void OutputNotify();

  /**
   * @brief 当音频输出参数发生更改时发出此信号。
   */
  void OutputParamsChanged();

 private:
  /**
   * @brief AudioManager 的私有构造函数，用于实现单例模式。
   */
  AudioManager();

  /**
   * @brief AudioManager 的私有析构函数。
   */
  ~AudioManager() override;

  /**
   * @brief 将 olive::SampleFormat 枚举值转换为 PortAudio 的 PaSampleFormat。
   * @param fmt 要转换的 olive::SampleFormat 值。
   * @return PaSampleFormat 对应的 PortAudio 样本格式。
   */
  static PaSampleFormat GetPortAudioSampleFormat(SampleFormat fmt);

  /**
   * @brief 关闭当前的 PortAudio 输出流。
   */
  void CloseOutputStream();

  /**
   * @brief AudioManager 的静态单例实例指针。
   */
  static AudioManager *instance_;

  /**
   * @brief 当前选择的 PortAudio 输出设备索引。
   */
  PaDeviceIndex output_device_{};
  /**
   * @brief 指向当前活动的 PortAudio 输出流的指针。
   */
  PaStream *output_stream_;
  /**
   * @brief 当前输出流所使用的音频参数。
   */
  AudioParams output_params_;
  /**
   * @brief 指向用于音频输出的预览音频设备缓冲区的指针。
   */
  PreviewAudioDevice *output_buffer_;

  /**
   * @brief 当前选择的 PortAudio 输入设备索引。
   */
  PaDeviceIndex input_device_{};
  /**
   * @brief 指向当前活动的 PortAudio 输入流的指针。
   */
  PaStream *input_stream_;

  /**
   * @brief 指向用于音频录制的 FFmpeg 编码器的指针。
   */
  FFmpegEncoder *input_encoder_;
};

}  // namespace olive

#endif  // AUDIOMANAGER_H