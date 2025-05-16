#ifndef AUDIOMONITORWIDGET_H
#define AUDIOMONITORWIDGET_H

#include <QFile>          // Qt 文件操作类
#include <QOpenGLWidget>  // Qt OpenGL 控件基类
#include <QTimer>         // Qt 定时器类

#include "audio/audiovisualwaveform.h"  // 音频可视化波形相关
#include "common/define.h"              // 项目通用定义
#include "render/audiowaveformcache.h"  // 音频波形缓存相关

namespace olive {

/**
 * @brief 音频监视器控件类，用于显示音频电平或波形。
 *
 * AudioMonitor 继承自 QOpenGLWidget，使用 OpenGL 进行渲染，以提供实时的音频可视化。
 * 它可以显示来自实时音频流或预先计算的音频波形缓存的音频数据。
 */
class AudioMonitor : public QOpenGLWidget {
 Q_OBJECT  // Qt 元对象系统宏，用于支持信号和槽机制
     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit AudioMonitor(QWidget *parent = nullptr);

  /**
   * @brief 析构函数。
   */
  ~AudioMonitor() override;

  /**
   * @brief 检查当前是否正在播放或显示波形。
   * @return 如果正在处理波形数据则返回 true，否则返回 false。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool IsPlaying() const { return waveform_; }

  /**
   * @brief 在所有 AudioMonitor 实例上开始显示波形。
   * @param waveform 音频波形缓存数据指针。
   * @param start 波形显示的起始时间。
   * @param playback_speed 播放速度。
   */
  static void StartWaveformOnAll(const AudioWaveformCache *waveform, const rational &start, int playback_speed) {
    foreach (AudioMonitor *m, instances_) {  // 遍历所有 AudioMonitor 实例
      m->StartWaveform(waveform, start, playback_speed);
    }
  }

  /**
   * @brief 停止所有 AudioMonitor 实例的波形显示。
   */
  static void StopOnAll() {
    foreach (AudioMonitor *m, instances_) {  // 遍历所有 AudioMonitor 实例
      m->Stop();
    }
  }

  /**
   * @brief 将音频采样缓冲区推送到所有 AudioMonitor 实例。
   * @param d 音频采样缓冲区。
   */
  static void PushSampleBufferOnAll(const SampleBuffer &d) {
    foreach (AudioMonitor *m, instances_) {  // 遍历所有 AudioMonitor 实例
      m->PushSampleBuffer(d);
    }
  }

 public slots:
  /**
   * @brief 设置音频参数。
   * @param params 音频参数结构体。
   */
  void SetParams(const AudioParams &params);

  /**
   * @brief 停止当前实例的音频监控和波形显示。
   */
  void Stop();

  /**
   * @brief 推送新的音频采样缓冲区到当前实例。
   * @param d 音频采样缓冲区。
   */
  void PushSampleBuffer(const SampleBuffer &d);

  /**
   * @brief 在当前实例上开始显示波形。
   * @param waveform 音频波形缓存数据指针。
   * @param start 波形显示的起始时间。
   * @param playback_speed 播放速度。
   */
  void StartWaveform(const AudioWaveformCache *waveform, const rational &start, int playback_speed);

 protected:
  /**
   * @brief OpenGL 绘制事件处理函数。
   *
   * 在此函数中执行所有 OpenGL 相关的绘制操作。
   */
  void paintGL() override;

  /**
   * @brief 鼠标按下事件处理函数。
   * @param event 鼠标事件指针。
   */
  void mousePressEvent(QMouseEvent *event) override;

 private:
  /**
   * @brief 设置更新循环的启用状态。
   * @param e 如果为 true，则启动更新循环；否则停止。
   */
  void SetUpdateLoop(bool e);

  /**
   * @brief 从波形数据更新内部值。
   * @param v 用于存储更新后值的 QVector<double> 引用。
   * @param delta_time 时间增量。
   */
  void UpdateValuesFromWaveform(QVector<double> &v, qint64 delta_time);

  /**
   * @brief 将 AudioVisualWaveform 的采样数据转换为内部表示的值。
   * @param in 输入的 AudioVisualWaveform 采样数据。
   * @param out 输出的内部值 QVector<double> 引用。
   */
  static void AudioVisualWaveformSampleToInternalValues(const AudioVisualWaveform::Sample &in, QVector<double> &out);

  /**
   * @brief 推送一组新的音频值用于显示。
   * @param v 包含各通道音频值的 QVector<double>。
   */
  void PushValue(const QVector<double> &v);

  /**
   * @brief 将原始字节数据转换为采样摘要信息。
   * @param bytes 包含原始音频数据的 QByteArray。
   * @param v 用于存储转换后采样摘要的 QVector<double> 引用。
   */
  void BytesToSampleSummary(const QByteArray &bytes, QVector<double> &v);

  /**
   * @brief 获取当前各通道的平均音频电平。
   * @return 包含各通道平均电平的 QVector<double>。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QVector<double> GetAverages() const;

  AudioParams params_;  ///< 存储当前音频参数 (如采样率, 通道数等)。

  qint64 last_time_{};  ///< 上一次更新或接收到采样数据的时间戳。

  const AudioWaveformCache *waveform_;  ///< 指向音频波形缓存数据的指针，用于显示预渲染的波形。
  rational waveform_time_;              ///< 当前在波形数据中显示的时间位置。
  rational waveform_length_;            ///< 当前加载的波形数据的总长度。

  int playback_speed_{};  ///< 音频播放或波形扫描的速度。

  QVector<QVector<double> >
      values_;            ///< 存储用于显示的音频电平值，外层 QVector 对应时间序列，内层 QVector 对应各通道的值。
  QVector<bool> peaked_;  ///< 标记各个通道是否已经达到峰值。

  QPixmap cached_background_;  ///< 缓存的背景图像，用于提高重绘效率。
  int cached_channels_;        ///< 缓存的音频通道数，用于优化背景重绘。

  static QVector<AudioMonitor *> instances_;  ///< 静态列表，存储所有 AudioMonitor 控件的实例指针。
};

}  // namespace olive

#endif  // AUDIOMONITORWIDGET_H
