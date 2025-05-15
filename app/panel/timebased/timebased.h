#ifndef TIMEBASEDPANEL_H // 防止头文件被重复包含的宏
#define TIMEBASEDPANEL_H // 定义 TIMEBASEDPANEL_H 宏

#include "panel/panel.h"                      // 包含 PanelWidget 基类的定义
#include "widget/timebased/timebasedwidget.h" // 包含 TimeBasedWidget 控件基类的定义

namespace olive { // olive 项目的命名空间

class ViewerOutput; // 向前声明 ViewerOutput 类 (通常代表一个可以输出视频/音频帧的节点)

/**
 * @brief TimeBasedPanel 类是一个 PanelWidget 的派生类，作为所有与时间相关的面板的基类。
 *
 * 这种类型的面板通常会包含一个时间轴、播放控制按钮、以及显示与时间相关内容的核心控件
 * (封装在 TimeBasedWidget 中)。例如，时间轴面板 (TimelinePanel)、曲线编辑器面板 (CurvePanel)、
 * 查看器面板 (ViewerPanel) 等都可能继承自 TimeBasedPanel。
 *
 * 它提供了连接到 ViewerOutput (通常是一个序列或素材节点) 的通用机制，
 * 以及处理时间导航和播放控制的接口。
 */
class TimeBasedPanel : public PanelWidget { // TimeBasedPanel 继承自 PanelWidget
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  /**
   * @brief 构造函数。
   * @param object_name 面板的 Qt 对象名称，可用于样式表或查找。
   */
  explicit TimeBasedPanel(const QString& object_name);

  // 析构函数
  ~TimeBasedPanel() override;

  /**
   * @brief 将此面板连接到一个 ViewerOutput 节点。
   * 连接后，面板通常会显示或响应此节点输出的内容和时间信息。
   * @param node 要连接的 ViewerOutput 节点指针。如果为 nullptr，则断开当前连接。
   */
  void ConnectViewerNode(ViewerOutput* node);

  /**
   * @brief断开当前连接的 ViewerOutput 节点。
   * 相当于调用 ConnectViewerNode(nullptr)。
   */
  void DisconnectViewerNode() { ConnectViewerNode(nullptr); }

  /**
   * @brief 获取此面板内部控件 (TimeBasedWidget) 的时间基准 (timebase)。
   * 时间基准通常定义了时间单位，例如帧率。
   * @return 返回 rational 类型的时间基准。
   */
  const rational& timebase();

  /**
   * @brief 获取当前连接到此面板的 ViewerOutput 节点。
   * @return 返回连接的 ViewerOutput 节点指针，如果未连接则返回 nullptr。
   */
  [[nodiscard]] ViewerOutput* GetConnectedViewer() const { return widget_->GetConnectedNode(); }

  /**
   * @brief 获取此面板内部控件的时间标尺 (TimeRuler) 对象。
   * 时间标尺用于显示时间刻度。
   * @return 返回 TimeRuler 指针。
   */
  [[nodiscard]] TimeRuler* ruler() const { return widget_->ruler(); }

  // --- 重写 PanelWidget 或通用编辑/播放控制接口的方法 ---
  // 以下方法通常是标准编辑或播放控制动作的接口，
  // TimeBasedPanel 会将这些请求转发给其内部的 TimeBasedWidget 或发出相应的信号。

  void ZoomIn() override;          // 放大时间轴或内容视图
  void ZoomOut() override;         // 缩小时间轴或内容视图
  void GoToStart() override;       // 跳转到内容的开始位置
  void PrevFrame() override;       // 跳转到上一帧
  void NextFrame() override;       // 跳转到下一帧
  void GoToEnd() override;         // 跳转到内容的结束位置
  void GoToPrevCut() override;     // 跳转到上一个剪辑点/标记
  void GoToNextCut() override;     // 跳转到下一个剪辑点/标记
  void PlayPause() override;       // 播放/暂停
  void PlayInToOut() override;     // 播放入点到出点之间的范围
  void ShuttleLeft() override;     //向左快速穿梭 (快退)
  void ShuttleStop() override;     // 停止穿梭
  void ShuttleRight() override;    //向右快速穿梭 (快进)
  void SetIn() override;           // 设置当前时间为入点
  void SetOut() override;          // 设置当前时间为出点
  void ResetIn() override;         // 重置/清除入点
  void ResetOut() override;        // 重置/清除出点
  void ClearInOut() override;      // 清除入点和出点
  void SetMarker() override;       // 在当前时间设置标记
  void ToggleShowAll() override;   // 切换是否显示全部内容 (例如，缩放到适合所有内容)
  void GoToIn() override;          // 跳转到入点
  void GoToOut() override;         // 跳转到出点
  void DeleteSelected() override;  // 删除选中的内容
  void CutSelected() override;     // 剪切选中的内容
  void CopySelected() override;    // 复制选中的内容
  void Paste() override;           // 粘贴内容

  /**
   * @brief 获取内部封装的 TimeBasedWidget 控件的指针。
   * @return 返回 TimeBasedWidget 指针。
   */
  [[nodiscard]] TimeBasedWidget* GetTimeBasedWidget() const { return widget_; }

 public slots: // Qt 公有槽函数
  /**
   * @brief 设置此面板内部控件的时间基准。
   * @param timebase 新的时间基准。
   */
  void SetTimebase(const rational& timebase);

 signals: // Qt 信号声明
  // 以下信号通常在相应的播放控制方法被调用时发出，
  // 用于通知其他模块 (如中央播放控制器) 执行实际的播放操作。
  void PlayPauseRequested();       // 请求播放/暂停
  void PlayInToOutRequested();     // 请求播放入点到出点
  void ShuttleLeftRequested();     // 请求向左穿梭
  void ShuttleStopRequested();     // 请求停止穿梭
  void ShuttleRightRequested();    // 请求向右穿梭

 protected:
  /**
   * @brief 设置此面板的中心内容控件 (必须是 TimeBasedWidget 或其派生类)。
   * 这个方法通常在派生类的构造函数中被调用，用于将特定的时间相关控件
   * (如 TimelineWidget, CurveWidget) 设置为面板的核心UI。
   * @param widget 指向 TimeBasedWidget 的指针。
   */
  void SetTimeBasedWidget(TimeBasedWidget* widget);

  /**
   * @brief 重写基类的 Retranslate 方法，用于在语言更改时更新此面板内UI元素的文本。
   */
  void Retranslate() override;

  /**
   * @brief 设置一个标志，指示在连接到 ViewerOutput 时是否自动显示并提升此面板到顶层。
   */
  void SetShowAndRaiseOnConnect() { show_and_raise_on_connect_ = true; }

 private:
  TimeBasedWidget* widget_; // 指向内部核心时间相关控件的指针 (例如 TimelineWidget, CurveWidget)

  bool show_and_raise_on_connect_; // 标志：在连接节点时是否自动显示并提升面板

 private slots: // Qt 私有槽函数
  /**
   * @brief 当连接的 ViewerOutput 节点发生改变时的处理槽函数。
   * @param old 旧的连接节点指针。
   * @param now 新的连接节点指针。
   */
  void ConnectedNodeChanged(ViewerOutput* old, ViewerOutput* now);
};

}  // namespace olive

#endif  // TIMEBASEDPANEL_H