#ifndef PLAYBACKCONTROLS_H
#define PLAYBACKCONTROLS_H

#include <QEvent>          // Qt 事件基类 (用于 changeEvent)
#include <QLabel>          // Qt 标签控件
#include <QPushButton>     // Qt 按钮控件
#include <QStackedWidget>  // Qt 堆叠控件，用于在播放/暂停按钮之间切换
#include <QTimer>          // Qt 定时器类 (用于播放按钮闪烁)
#include <QWidget>         // Qt 控件基类

#include "common/define.h"                 // 项目通用定义 (可能包含 rational 类型)
#include "dragbutton.h"                    // 自定义的拖拽按钮
#include "olive/core/core.h"               // Olive 核心库 (可能包含 rational 类型定义)
#include "widget/slider/rationalslider.h"  // 自定义的有理数滑块/显示控件 (用于时间码)

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QMouseEvent; // DragButton 可能使用，但此处不直接使用
// class QString;     // QLabel, QPushButton 可能使用

// 前向声明项目内部类 (根据用户要求，不添加)
// class rational; // 已通过 common/define.h 或 olive/core/core.h 包含

namespace olive {

/**
 * @brief PlaybackControls 类是一个提供媒体导航按钮的播放控制控件。
 *
 * 此控件可选地具有用于当前时间码和结束时间码的时间码显示功能。
 * 它包含标准的播放、暂停、前进、后退、到开头、到结尾等按钮，
 * 以及可选的音频/视频拖拽按钮。
 */
class PlaybackControls : public QWidget {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit PlaybackControls(QWidget* parent = nullptr);

  /**
   * @brief 设置是否显示时间码。
   * @param enabled 如果为 true，则显示时间码标签；否则隐藏。
   */
  void SetTimecodeEnabled(bool enabled);

  /**
   * @brief 设置当前控件使用的时间基准（例如帧率）。
   * @param r 新的时间基准 (rational 类型)。
   */
  void SetTimebase(const rational& r);

  /**
   * @brief 设置音频和视频拖拽按钮的可见性。
   * @param e 如果为 true，则显示拖拽按钮；否则隐藏。
   */
  void SetAudioVideoDragButtonsVisible(bool e);

 public slots:
  /**
   * @brief 设置并显示当前时间。
   * @param r 当前时间 (rational 类型)。
   */
  void SetTime(const rational& r);

  /**
   * @brief 设置并显示结束时间。
   * @param r 结束时间 (rational 类型)。
   */
  void SetEndTime(const rational& r);

  /**
   * @brief 显示暂停按钮（并隐藏播放按钮）。
   *
   * 通常在媒体正在播放时调用。
   */
  void ShowPauseButton();

  /**
   * @brief 显示播放按钮（并隐藏暂停按钮）。
   *
   * 通常在媒体已暂停或停止时调用。
   */
  void ShowPlayButton();

  /**
   * @brief 开始播放按钮的闪烁效果。
   *
   * 通常在录制或某种特殊播放模式下使用。
   */
  void StartPlayBlink() {
    play_blink_timer_->start();                // 启动闪烁定时器
    SetButtonRecordingState(play_btn_, true);  // 设置播放按钮为录制状态样式
  }

  /**
   * @brief 停止播放按钮的闪烁效果。
   */
  void StopPlayBlink() {
    play_blink_timer_->stop();                  // 停止闪烁定时器
    SetButtonRecordingState(play_btn_, false);  // 清除播放按钮的录制状态样式
  }

  /**
   * @brief 设置暂停按钮的录制状态样式。
   * @param on 如果为 true，则应用录制状态样式；否则清除。
   */
  void SetPauseButtonRecordingState(bool on) { SetButtonRecordingState(pause_btn_, on); }

 signals:
  /**
   * @brief 当“到开头”按钮被点击时发出此信号。
   */
  void BeginClicked();

  /**
   * @brief 当“上一帧”按钮被点击时发出此信号。
   */
  void PrevFrameClicked();

  /**
   * @brief 当“播放”按钮被点击时发出此信号。
   */
  void PlayClicked();

  /**
   * @brief 当“暂停”按钮被点击时发出此信号。
   */
  void PauseClicked();

  /**
   * @brief 当“下一帧”按钮被点击时发出此信号。
   */
  void NextFrameClicked();

  /**
   * @brief 当“到结尾”按钮被点击时发出此信号。
   */
  void EndClicked();

  /**
   * @brief 当音频拖拽按钮被点击时发出此信号。
   */
  void AudioClicked();

  /**
   * @brief 当视频拖拽按钮被点击时发出此信号。
   */
  void VideoClicked();

  /**
   * @brief 当音频拖拽按钮开始被拖拽时发出此信号。
   */
  void AudioDragged();

  /**
   * @brief 当视频拖拽按钮开始被拖拽时发出此信号。
   */
  void VideoDragged();

  /**
   * @brief 当当前时间码显示（通过 RationalSlider）被用户编辑后发出此信号。
   * @param t 新的时间值 (rational 类型)。
   */
  void TimeChanged(const rational& t);

 protected:
  /**
   * @brief 处理 Qt 的 changeEvent 事件。
   *
   * 主要用于响应 QEvent::LanguageChange 和 QEvent::ThemeChange (如果适用) 事件，
   * 以便在应用程序语言或主题设置更改时更新控件的文本或图标。
   * @param event 指向 QEvent 对象的指针。
   */
  void changeEvent(QEvent*) override;

 private:
  /**
   * @brief 更新所有按钮的图标。
   *
   * 通常在主题或图标集更改时调用。
   */
  void UpdateIcons();

  /**
   * @brief 静态辅助函数，设置按钮的“录制中”视觉状态。
   *
   * 可能会通过设置特定的 QSS 属性来实现。
   * @param btn 要设置状态的 QPushButton 指针。
   * @param on 如果为 true，则设置为录制状态样式；否则清除。
   */
  static void SetButtonRecordingState(QPushButton* btn, bool on);

  QWidget* lower_left_container_;   ///< 左下方控件的容器 (可能用于时间码显示)。
  QWidget* lower_right_container_;  ///< 右下方控件的容器 (可能用于时间码显示)。

  RationalSlider* cur_tc_lbl_;  ///< 用于显示和编辑当前时间码的 RationalSlider 控件。
  QLabel* end_tc_lbl_;          ///< 用于显示结束时间码的 QLabel。

  rational end_time_;   ///< 存储当前的结束时间。
  rational time_base_;  ///< 存储当前的时间基准（帧率）。

  QPushButton* go_to_start_btn_;  ///< “到开头”按钮。
  QPushButton* prev_frame_btn_;   ///< “上一帧”按钮。
  QPushButton* play_btn_;         ///< “播放”按钮。
  QPushButton* pause_btn_;        ///< “暂停”按钮。
  QPushButton* next_frame_btn_;   ///< “下一帧”按钮。
  QPushButton* go_to_end_btn_;    ///< “到结尾”按钮。
  DragButton* video_drag_btn_;    ///< 视频拖拽按钮。
  DragButton* audio_drag_btn_;    ///< 音频拖拽按钮。

  QStackedWidget* playpause_stack_;  ///< 用于在播放按钮和暂停按钮之间切换显示的堆叠控件。

  QTimer* play_blink_timer_;  ///< 用于实现播放按钮闪烁效果的定时器。

 private slots:
  /**
   * @brief 当 RationalSlider 中的时间码被用户编辑后调用的槽函数。
   *
   * 此函数会发出 TimeChanged 信号。
   */
  void TimecodeChanged();

  /**
   * @brief 播放按钮闪烁定时器的超时槽函数。
   *
   * 用于切换播放按钮的视觉状态以产生闪烁效果。
   */
  void PlayBlink();
};

}  // namespace olive

#endif  // PLAYBACKCONTROLS_H
