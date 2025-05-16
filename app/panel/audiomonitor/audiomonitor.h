#ifndef AUDIOMONITORPANEL_H  // 防止头文件被重复包含的宏
#define AUDIOMONITORPANEL_H  // 定义 AUDIOMONITORPANEL_H 宏

#include "panel/panel.h"                       // 包含 PanelWidget 基类的定义
#include "widget/audiomonitor/audiomonitor.h"  // 包含 AudioMonitor 控件的定义

namespace olive {  // olive 项目的命名空间

/**
 * @brief AudioMonitorPanel 类是一个 PanelWidget 的包装器，用于封装和管理一个 AudioMonitor 控件。
 *
 * PanelWidget 通常是 Olive 编辑器中可停靠、可浮动面板的基类。
 * 这个类将音频监视器 (AudioMonitor，一个用于显示音频电平或波形的UI控件)
 * 集成到面板系统中，使其可以作为编辑器界面的一部分进行布局和管理。
 */
class AudioMonitorPanel : public PanelWidget {  // AudioMonitorPanel 继承自 PanelWidget
 Q_OBJECT                                       // 声明此类使用 Qt 的元对象系统

     public :
     // 构造函数
     AudioMonitorPanel();

  /**
   * @brief 检查内部的音频监视器当前是否正在播放音频。
   * @return 如果正在播放，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool IsPlaying() const { return audio_monitor_->IsPlaying(); }

  /**
   * @brief 设置内部音频监视器的音频参数。
   * @param params 要设置的音频参数 (如采样率、通道数等)。
   */
  void SetParams(const AudioParams& params) { audio_monitor_->SetParams(params); }

 private:
  /**
   * @brief 重写基类的 Retranslate 方法，用于在语言更改时更新此面板内UI元素的文本。
   */
  void Retranslate() override;

  AudioMonitor* audio_monitor_;  // 指向实际的 AudioMonitor 控件的指针
  // 这个控件由 AudioMonitorPanel 创建和管理。
};

}  // namespace olive

#endif  // AUDIOMONITORPANEL_H