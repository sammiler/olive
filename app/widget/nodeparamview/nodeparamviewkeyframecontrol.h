#ifndef NODEPARAMVIEWKEYFRAMECONTROL_H
#define NODEPARAMVIEWKEYFRAMECONTROL_H

#include <QPushButton> // Qt 按钮控件基类
#include <QWidget>     // Qt 控件基类

#include "node/param.h"                   // 节点参数相关定义 (包含 NodeInput)
#include "widget/timetarget/timetarget.h" // 时间目标对象接口

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QIcon;
// class rational; // 包含在 timetarget.h 或 node/param.h 中

// 前向声明项目内部类 (根据用户要求，不添加)
// class ViewerOutput; // TimeTargetObject 中可能使用

namespace olive {

/**
 * @brief NodeParamViewKeyframeControl 类是一个用于控制节点参数关键帧的自定义控件。
 *
 * 它通常显示在节点参数旁边，提供一组按钮来操作关键帧，
 * 例如：跳转到上一个/下一个关键帧、在当前时间点添加/删除关键帧、启用/禁用参数的动画。
 * 此控件也作为 TimeTargetObject，因此可以响应并同步到播放时间。
 */
class NodeParamViewKeyframeControl : public QWidget, public TimeTargetObject {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param right_align 布尔值，指示按钮是否应右对齐。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit NodeParamViewKeyframeControl(bool right_align, QWidget* parent = nullptr);
  /**
   * @brief 构造函数重载，默认按钮右对齐。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit NodeParamViewKeyframeControl(QWidget* parent = nullptr) : NodeParamViewKeyframeControl(true, parent) {}

  /**
   * @brief 获取当前此控件关联的节点输入参数。
   * @return 返回一个 const 引用，指向 NodeInput 对象。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const NodeInput& GetConnectedInput() const { return input_; }

  /**
   * @brief 设置此控件关联的节点输入参数。
   *
   * 当设置新的输入参数时，控件会连接到该参数的信号并更新其状态。
   * @param input 要关联的 NodeInput 对象。
   */
  void SetInput(const NodeInput& input);

 protected:
  /**
   * @brief 当此控件与时间目标（通常是查看器）断开连接时调用的事件处理函数。
   * @param v 指向被断开连接的 ViewerOutput 对象的指针。
   */
  void TimeTargetDisconnectEvent(ViewerOutput* v) override;
  /**
   * @brief 当此控件连接到新的时间目标（通常是查看器）时调用的事件处理函数。
   * @param v 指向新连接的 ViewerOutput 对象的指针。
   */
  void TimeTargetConnectEvent(ViewerOutput* v) override;

 private:
  /**
   * @brief 静态辅助函数，创建一个带有指定图标的新工具按钮。
   * @param icon 按钮上显示的 QIcon。
   * @return 返回创建的 QPushButton 指针。
   */
  [[nodiscard]] static QPushButton* CreateNewToolButton(const QIcon& icon);

  /**
   * @brief 设置关键帧控制按钮（上一个、下一个、切换）的启用状态。
   * @param e 如果为 true，则启用这些按钮；否则禁用。
   */
  void SetButtonsEnabled(bool e);

  /**
   * @brief 获取当前时间点（通常来自时间目标）并将其转换为关联节点的本地时间。
   * @return 返回 rational 类型的时间值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] rational GetCurrentTimeAsNodeTime() const;

  /**
   * @brief 将给定的时间值（通常是节点本地时间）转换为查看器时间。
   * @param r 要转换的 rational 时间。
   * @return 返回转换后的 rational 时间。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] rational ConvertToViewerTime(const rational& r) const;

  QPushButton* prev_key_btn_;   ///< “跳转到上一个关键帧”按钮。
  QPushButton* toggle_key_btn_; ///< “在当前时间添加/删除关键帧”按钮。
  QPushButton* next_key_btn_;   ///< “跳转到下一个关键帧”按钮。
  QPushButton* enable_key_btn_; ///< “启用/禁用参数动画（关键帧）”按钮。

  NodeInput input_; ///< 此控件当前关联的节点输入参数。

 private slots:
  /**
   * @brief 根据参数是否启用了关键帧动画来显示或隐藏导航和切换按钮。
   * @param e 如果为 true（启用了关键帧），则显示按钮；否则隐藏。
   */
  void ShowButtonsFromKeyframeEnable(bool e);

  /**
   * @brief 切换（添加或删除）当前时间点的关键帧。
   * @param e 按钮的选中状态（通常不直接使用，而是根据当前是否存在关键帧来决定操作）。
   */
  void ToggleKeyframe(bool e);

  /**
   * @brief 更新所有按钮的状态（例如，是否启用、是否选中）。
   *
   * 此函数会在多种情况下被调用，例如时间改变、关键帧数据改变、输入参数改变等。
   */
  void UpdateState();

  /**
   * @brief 跳转到关联参数的上一个关键帧。
   */
  void GoToPreviousKey();

  /**
   * @brief 跳转到关联参数的下一个关键帧。
   */
  void GoToNextKey();

  /**
   * @brief “启用/禁用关键帧”按钮被点击时的槽函数。
   * @param e 按钮的新勾选状态 (true 表示启用关键帧)。
   */
  void KeyframeEnableBtnClicked(bool e);

  /**
   * @brief 当关联输入参数的关键帧启用状态发生改变时调用的槽函数。
   * @param input 发生改变的 NodeInput。
   * @param e 新的启用状态 (true 表示已启用关键帧)。
   */
  void KeyframeEnableChanged(const NodeInput& input, bool e);
};

}  // namespace olive

#endif  // NODEPARAMVIEWKEYFRAMECONTROL_H
