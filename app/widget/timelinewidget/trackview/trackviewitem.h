#ifndef TRACKVIEWITEM_H  // 防止头文件被多次包含的宏定义
#define TRACKVIEWITEM_H

#include <QPushButton>     // 引入 QPushButton 类，用于创建按钮
#include <QStackedWidget>  // 引入 QStackedWidget 类，用于在同一位置显示多个控件，一次只显示一个
#include <QWidget>         // 引入 QWidget 类，是所有用户界面对象的基类

#include "node/output/track/track.h"                     // 引入 Track 类的定义，表示时间轴上的一个轨道
#include "widget/clickablelabel/clickablelabel.h"        // 引入 ClickableLabel 类，可点击的标签
#include "widget/focusablelineedit/focusablelineedit.h"  // 引入 FocusableLineEdit 类，可获取焦点的行编辑器
#include "widget/timelinewidget/view/timelineviewmouseevent.h"  // 引入 TimelineViewMouseEvent 类，虽然在此头文件中未直接使用，但可能在 cpp 文件或其他关联部分使用

namespace olive {  // olive 命名空间开始

/**
 * @brief TrackViewItem 类是一个控件，用于在 TrackView 中可视化地表示单个轨道 (Track) 的头部信息和控制。
 *
 * 它通常包含轨道的名称（可编辑）、静音(Mute)/独奏(Solo)/锁定(Lock)按钮等。
 * 用户可以通过这个控件与单个轨道进行交互。
 */
class TrackViewItem : public QWidget {
 Q_OBJECT  // Q_OBJECT 宏，用于启用 Qt 的元对象特性，如信号和槽

     public :
     /**
      * @brief 构造一个 TrackViewItem 对象。
      * @param track 指向此视图项所关联的 Track 对象的指针。
      * @param parent 父 QWidget 对象，默认为 nullptr。
      */
     explicit TrackViewItem(Track* track, QWidget* parent = nullptr);

 signals:  // 信号
  /**
   * @brief 当此轨道项对应的轨道即将被删除时发出此信号。
   *
   * 通常在用户通过此轨道项的上下文菜单请求删除轨道时触发。
   * @param track 指向即将被删除的 Track 对象的指针。
   */
  void AboutToDeleteTrack(Track* track);

 private:  // 私有静态方法
  /**
   * @brief 创建一个用于静音(Mute)/独奏(Solo)/锁定(Lock)状态的按钮。
   *
   * 这是一个静态辅助函数，用于创建具有特定样式和行为的 QPushButton。
   * @param checked_color 按钮在选中状态下的颜色 (QColor)。
   * @return 指向新创建的 QPushButton 对象的指针。
   */
  [[nodiscard]] static QPushButton* CreateMSLButton(const QColor& checked_color);

  QStackedWidget* stack_;  ///< QStackedWidget 指针，用于在轨道名称标签和行编辑器之间切换显示。

  ClickableLabel* label_;         ///< 指向 ClickableLabel 对象的指针，用于显示轨道名称，点击后可切换到编辑模式。
  FocusableLineEdit* line_edit_;  ///< 指向 FocusableLineEdit 对象的指针，用于编辑轨道名称。

  QPushButton* mute_button_;    ///< 指向静音按钮的指针。
  QPushButton* solo_button_{};  ///< 指向独奏按钮的指针，使用 C++11 的列表初始化为 nullptr。
  QPushButton* lock_button_;    ///< 指向锁定按钮的指针。

  Track* track_;  ///< 指向此视图项所关联的 Track 对象的指针。

 private slots:  // 私有槽函数
  /**
   * @brief 当轨道名称标签 (label_) 被点击时调用的槽函数。
   *
   * 通常会切换到行编辑器 (line_edit_) 以允许用户修改轨道名称。
   */
  void LabelClicked();

  /**
   * @brief 当行编辑器 (line_edit_) 中的编辑被确认（例如按下回车键）时调用的槽函数。
   *
   * 会将编辑后的名称应用到轨道，并切换回标签显示。
   */
  void LineEditConfirmed();

  /**
   * @brief 当行编辑器 (line_edit_) 中的编辑被取消（例如按下 Esc 键或失去焦点未确认）时调用的槽函数。
   *
   * 会放弃编辑，并切换回标签显示。
   */
  void LineEditCancelled();

  /**
   * @brief 更新轨道名称标签显示的槽函数。
   *
   * 通常在轨道名称发生变化时（例如通过 Track::NameChanged 信号）调用。
   */
  void UpdateLabel();

  /**
   * @brief 显示上下文菜单的槽函数。
   *
   * 通常在用户右键点击此轨道项时调用。
   * @param p 鼠标点击的本地坐标 (QPoint)。
   */
  void ShowContextMenu(const QPoint& p);

  /**
   * @brief 删除当前轨道的槽函数。
   *
   * 通常由上下文菜单中的“删除轨道”动作触发。
   */
  void DeleteTrack();

  /**
   * @brief 删除所有空轨道的槽函数。
   *
   * 通常由上下文菜单中的相关动作触发。
   */
  void DeleteAllEmptyTracks();

  /**
   * @brief 更新静音按钮状态的槽函数。
   *
   * 通常连接到 Track::MuteChanged() 信号。
   * @param e 布尔值，true 表示静音，false 表示取消静音。
   */
  void UpdateMuteButton(bool e);

  /**
   * @brief 更新锁定按钮状态的槽函数。
   *
   * 通常连接到 Track::LockChanged() 信号。
   * @param e 布尔值，true 表示锁定，false 表示解锁。
   */
  void UpdateLockButton(bool e);
};

}  // namespace olive

#endif  // TRACKVIEWITEM_H
