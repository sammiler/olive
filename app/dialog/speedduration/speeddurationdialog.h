#ifndef SPEEDDURATIONDIALOG_H
#define SPEEDDURATIONDIALOG_H

#include <QCheckBox>  // 引入 QCheckBox 类，用于创建复选框控件
#include <QComboBox>  // 引入 QComboBox 类，用于创建下拉列表框控件
#include <QDialog>    // 引入 QDialog 类，对话框窗口的基类

#include "node/block/clip/clip.h"          // 引入片段块 (ClipBlock) 定义
#include "node/block/gap/gap.h"            // 引入间隙块 (GapBlock) 定义 (虽然未直接使用，但可能与片段操作相关)
#include "undo/undocommand.h"              // 引入撤销命令基类 (虽然未直接使用，但修改操作通常应可撤销)
#include "widget/slider/floatslider.h"     // 引入浮点数滑块控件
#include "widget/slider/rationalslider.h"  // 引入有理数滑块控件

namespace olive {

/**
 * @class SpeedDurationDialog
 * @brief “速度/时长”对话框类。
 *
 * 此对话框允许用户修改一个或多个片段 (ClipBlock) 的播放速度和/或时长。
 * 它提供了调整速度、时长、是否反向播放、是否保持音频音调、
 * 是否进行涟漪编辑以及循环模式等选项。
 */
class SpeedDurationDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief SpeedDurationDialog 构造函数。
   *
   * @param clips 要修改速度/时长的片段 (ClipBlock) 对象的 QVector 引用。
   * @param timebase 时间基准，用于精确计算时长和速度。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit SpeedDurationDialog(const QVector<ClipBlock *> &clips, const rational &timebase, QWidget *parent = nullptr);

 public slots:
  /**
   * @brief 重写 QDialog 的 accept() 槽函数。
   *
   * 当用户点击“确定”按钮时调用。此函数将对话框中设置的更改应用到选定的片段。
   */
  void accept() override;

 signals:
  // 此类目前没有定义任何信号。

 private:
  /**
   * @brief 私有静态方法：根据新的速度计算长度的调整量。
   *
   * @param original_length 原始长度。
   * @param original_speed 原始速度 (例如 1.0 代表正常速度)。
   * @param new_speed 新的速度。
   * @param timebase 时间基准，用于确保帧对齐。
   * @return rational 计算出的长度调整量。
   */
  static rational GetLengthAdjustment(const rational &original_length, double original_speed, double new_speed,
                                      const rational &timebase);

  /**
   * @brief 私有静态方法：根据新的长度计算速度的调整量。
   *
   * @param original_speed 原始速度。
   * @param original_length 原始长度。
   * @param new_length 新的长度。
   * @return double 计算出的新速度。
   */
  static double GetSpeedAdjustment(double original_speed, const rational &original_length, const rational &new_length);

  /**
   * @brief 存储指向要修改的 ClipBlock 对象的 QVector。
   */
  QVector<ClipBlock *> clips_;

  /**
   * @brief 指向速度调整滑块控件的指针。
   */
  FloatSlider *speed_slider_;

  /**
   * @brief 指向时长调整滑块控件的指针 (使用有理数)。
   */
  RationalSlider *dur_slider_;

  /**
   * @brief 指向“链接速度和时长”复选框控件的指针。
   * 如果勾选，则调整速度会自动调整时长，反之亦然。
   */
  QCheckBox *link_box_;

  /**
   * @brief 指向“反向播放”复选框控件的指针。
   */
  QCheckBox *reverse_box_;

  /**
   * @brief 指向“保持音频音调”复选框控件的指针。
   * 如果勾选，在调整速度时会尝试保持音频的原始音调。
   */
  QCheckBox *maintain_audio_pitch_box_;

  /**
   * @brief 指向“涟漪编辑”复选框控件的指针。
   * 如果勾选，更改片段时长时会影响后续片段的位置。
   */
  QCheckBox *ripple_box_;

  /**
   * @brief 指向循环模式选择下拉框控件的指针。
   */
  QComboBox *loop_combo_;

  /**
   * @brief 存储初始的反向播放状态。
   */
  int start_reverse_;

  /**
   * @brief 存储初始的保持音频音调状态。
   */
  int start_maintain_audio_pitch_;

  /**
   * @brief 存储初始的速度值。
   */
  double start_speed_;

  /**
   * @brief 存储初始的时长值。
   */
  rational start_duration_;

  /**
   * @brief 存储初始的循环模式。
   */
  int start_loop_;

  /**
   * @brief 时间基准，用于确保帧精度的计算。
   */
  rational timebase_;

 private slots:
  /**
   * @brief 私有槽函数：当速度滑块的值改变时调用。
   *
   * 如果速度和时长是链接的，则会相应地更新时长。
   * @param s 新的速度值。
   */
  void SpeedChanged(double s);

  /**
   * @brief 私有槽函数：当时长滑块的值改变时调用。
   *
   * 如果速度和时长是链接的，则会相应地更新速度。
   * @param r 新的时长值 (有理数)。
   */
  void DurationChanged(const rational &r);
};

}  // namespace olive

#endif  // SPEEDDURATIONDIALOG_H
