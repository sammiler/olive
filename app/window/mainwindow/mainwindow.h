#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <kddockwidgets/Config.h>      // 引入 KDDockWidgets 的配置头文件
#include <kddockwidgets/MainWindow.h>  // 引入 KDDockWidgets 的 MainWindow 基类

#include "mainwindowlayoutinfo.h"                  // 引入主窗口布局信息的定义
#include "node/project.h"                          // 引入项目类的定义
#include "panel/audiomonitor/audiomonitor.h"       // 引入音频监视器面板
#include "panel/curve/curve.h"                     // 引入曲线编辑器面板
#include "panel/footageviewer/footageviewer.h"     // 引入素材查看器面板
#include "panel/history/historypanel.h"            // 引入历史记录面板
#include "panel/multicam/multicampanel.h"          // 引入多机位编辑面板
#include "panel/node/node.h"                       // 引入节点编辑器面板
#include "panel/panelmanager.h"                    // 引入面板管理器 (虽然在此文件中未直接使用，但可能是上下文依赖)
#include "panel/param/param.h"                     // 引入参数编辑器面板
#include "panel/pixelsampler/pixelsamplerpanel.h"  // 引入像素采样器面板
#include "panel/project/project.h"                 // 引入项目浏览器面板
#include "panel/scope/scope.h"                     // 引入示波器面板 (如波形图、矢量示波器)
#include "panel/sequenceviewer/sequenceviewer.h"   // 引入序列查看器面板
#include "panel/table/table.h"                     // 引入表格视图面板 (可能用于显示元数据等)
#include "panel/taskmanager/taskmanager.h"         // 引入任务管理器面板
#include "panel/timeline/timeline.h"               // 引入时间轴面板
#include "panel/tool/tool.h"                       // 引入工具栏面板

#ifdef Q_OS_WINDOWS
#include <shobjidl.h>  // 在 Windows 平台上引入 Shell 对象接口定义，用于任务栏进度条等功能
#endif

// 前向声明 MainWindowLayoutInfo, Project, Sequence, Folder, ViewerOutput, Footage, Block, PanelWidget
// 如果它们的完整定义已在上述包含的头文件中，则这些前向声明可能不是严格必需的。
// namespace olive { class MainWindowLayoutInfo; }
// namespace olive { class Project; }
// namespace olive { class Sequence; }
// namespace olive { class Folder; }
// namespace olive { class ViewerOutput; }
// namespace olive { class Footage; }
// namespace olive { class Block; }
// namespace KDDockWidgets { class DockWidget; } // PanelWidget 可能继承自 KDDockWidgets::DockWidget
// class PanelWidget; // PanelWidget 是所有面板的基类

namespace olive {

/**
 * @brief Olive 应用程序的主窗口，负责管理可停靠的部件和主菜单栏。
 *
 * MainWindow 类继承自 KDDockWidgets::MainWindow，提供了灵活的停靠布局功能。
 * 它集成了应用程序的各种核心面板（如时间轴、节点编辑器、查看器等），
 * 并处理项目加载、序列管理、UI布局保存/加载、全屏模式以及与操作系统的交互（如任务栏进度）。
 */
class MainWindow : public KDDockWidgets::MainWindow {
 Q_OBJECT  // 声明此类使用 Qt 的元对象系统（信号和槽）
     public :
     /**
      * @brief 显式构造函数。
      * @param parent 父 QWidget 对象，默认为 nullptr。
      */
     explicit MainWindow(QWidget *parent = nullptr);

  /**
   * @brief 析构函数。
   *
   * 负责清理资源，例如 Windows 平台上的任务栏接口。
   */
  ~MainWindow() override;

  /**
   * @brief 加载主窗口的布局。
   * @param info 包含要加载的布局信息的 MainWindowLayoutInfo 对象。
   */
  void LoadLayout(const MainWindowLayoutInfo &info);

  /**
   * @brief 保存当前主窗口的布局。
   * @return 返回一个 MainWindowLayoutInfo 对象，包含当前布局信息。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] MainWindowLayoutInfo SaveLayout() const;

  /**
   * @brief 打开一个序列，并在新的或已有的时间轴面板中显示它。
   * @param sequence 指向要打开的 Sequence 对象的指针。
   * @param enable_focus 如果为 true (默认)，则新打开的时间轴面板将获得焦点。
   * @return 返回打开该序列的 TimelinePanel 指针；如果失败则可能返回 nullptr。
   */
  TimelinePanel *OpenSequence(Sequence *sequence, bool enable_focus = true);

  /**
   * @brief 关闭一个已打开的序列及其对应的时间轴面板。
   * @param sequence 指向要关闭的 Sequence 对象的指针。
   */
  void CloseSequence(Sequence *sequence);

  /**
   * @brief 检查指定的序列当前是否已在主窗口中打开。
   * @param sequence 指向要检查的 Sequence 对象的指针。
   * @return 如果序列已打开，则返回 true；否则返回 false。
   */
  bool IsSequenceOpen(Sequence *sequence) const;

  /**
   * @brief 在项目面板中打开一个文件夹。
   * @param i 指向要打开的 Folder 对象的指针。
   * @param floating 如果为 true，则可能以浮动窗口的形式打开文件夹视图。
   */
  void OpenFolder(Folder *i, bool floating);

  /**
   * @brief 在查看器面板中打开一个节点（通常是可渲染输出的节点）。
   * @param node 指向要打开的 ViewerOutput (或其派生类) 对象的指针。
   */
  void OpenNodeInViewer(ViewerOutput *node);

  /**
   * @brief 定义应用程序在操作系统级别显示的进度状态。
   */
  enum ProgressStatus {
    kProgressNone,  ///< 无进度显示。
    kProgressShow,  ///< 显示进度。
    kProgressError  ///< 显示错误状态的进度（例如，红色进度条）。
  };

  /**
   * @brief 在操作系统级别显示应用程序的进度（如果适用）。
   *
   * - 对于 Windows，这表现为任务栏图标上的进度条。
   * - 对于 macOS，这表现为 Dock 图标上的进度指示。
   * @param status 要设置的进度状态 (ProgressStatus 枚举)。
   */
  void SetApplicationProgressStatus(ProgressStatus status);

  /**
   * @brief 如果 SetApplicationProgressStatus 设置为 kProgressShow，则使用此函数设置进度值。
   *
   * @param value 期望一个百分比值 (0-100 包含边界)。
   */
  void SetApplicationProgressValue(int value);

  /**
   * @brief 在项目面板中选择指定的素材。
   * @param e 包含要选择的 Footage 指针的 QVector。
   */
  void SelectFootage(const QVector<Footage *> &e);

 public slots:
  /**
   * @brief 设置当前活动的项目。
   *
   * 当加载或切换项目时调用此槽。
   * @param p 指向新的 Project 对象的指针。
   */
  void SetProject(Project *p);

  /**
   * @brief 设置主窗口的全屏模式。
   * @param fullscreen true 进入全屏，false 退出全屏。
   */
  void SetFullscreen(bool fullscreen);

  /**
   * @brief 切换当前拥有焦点的面板的最大化状态。
   */
  void ToggleMaximizedPanel();

  /**
   * @brief 将窗口布局重置为默认布局。
   */
  void SetDefaultLayout();

 protected:
  /**
   * @brief 重写 QWidget::showEvent()。
   *
   * 在窗口首次显示时执行特定操作，例如加载自定义快捷键或显示欢迎对话框。
   * @param e 指向 QShowEvent 事件对象的指针。
   */
  void showEvent(QShowEvent *e) override;

  /**
   * @brief 重写 QWidget::closeEvent()。
   *
   * 在窗口关闭前执行操作，例如保存布局和自定义快捷键。
   * @param e 指向 QCloseEvent 事件对象的指针。
   */
  void closeEvent(QCloseEvent *e) override;

#ifdef Q_OS_WINDOWS
  /**
   * @brief 重写 QWidget::nativeEvent()，用于处理特定于平台的事件 (仅 Windows)。
   *
   * 可能用于处理任务栏按钮消息等。
   * @param eventType 事件类型的字节数组。
   * @param message 指向消息数据的空指针。
   * @param result 指向用于存储事件处理结果的长整型指针。
   * @return 如果事件被处理则返回 true，否则返回 false。
   */
  virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
#endif

 private:
  /**
   * @brief 创建并添加一个新的时间轴面板到主窗口。
   * @return 返回指向新创建的 TimelinePanel 对象的指针。
   */
  TimelinePanel *AppendTimelinePanel();

  /**
   * @brief 模板函数，用于创建并添加指定类型的面板到内部列表和停靠系统中。
   * @tparam T 面板的类型，必须是 PanelWidget 的派生类。
   * @param panel_name 面板的内部名称/标识符。
   * @param list 指向存储此类型面板指针的 QList 的引用。
   * @return 返回指向新创建的 T 类型面板对象的指针。
   */
  template <typename T>
  T *AppendPanelInternal(const QString &panel_name, QList<T *> &list);

  /**
   * @brief 模板函数，用于从内部列表和停靠系统中移除指定类型的面板。
   * @tparam T 面板的类型。
   * @param list 指向存储此类型面板指针的 QList 的引用。
   * @param panel 指向要移除的 T 类型面板对象的指针。
   */
  template <typename T>
  void RemovePanelInternal(QList<T *> &list, T *panel);

  /**
   * @brief 从主窗口中移除一个时间轴面板。
   * @param panel 指向要移除的 TimelinePanel 对象的指针。
   */
  void RemoveTimelinePanel(TimelinePanel *panel);

  /**
   * @brief 当某个时间轴面板获得焦点时调用的内部函数。
   *
   * 可能用于更新其他相关面板（如音频监视器、节点编辑器）的上下文。
   * @param viewer 指向与获得焦点的时间轴关联的 ViewerOutput 对象的指针。
   */
  void TimelineFocused(ViewerOutput *viewer);

  /**
   * @brief 获取自定义快捷键配置文件的路径。
   * @return 返回包含自定义快捷键文件路径的 QString。
   */
  static QString GetCustomShortcutsFile();

  /**
   * @brief 加载自定义快捷键配置。
   */
  void LoadCustomShortcuts();

  /**
   * @brief 保存自定义快捷键配置。
   */
  void SaveCustomShortcuts();

  /**
   * @brief 根据当前查看器更新音频监视器面板的参数。
   * @param viewer 指向当前活动的 ViewerOutput 对象的指针。
   */
  void UpdateAudioMonitorParams(ViewerOutput *viewer);

  /**
   * @brief 根据当前聚焦的时间轴面板更新节点编辑器面板的上下文。
   * @param panel 指向当前聚焦的 TimelinePanel 对象的指针。
   */
  void UpdateNodePanelContextFromTimelinePanel(TimelinePanel *panel);

  /**
   * @brief 静态辅助函数，在指定的项目面板中选择素材。
   * @param e 包含要选择的 Footage 指针的 QVector。
   * @param p 目标 ProjectPanel 指针。
   */
  static void SelectFootageForProjectPanel(const QVector<Footage *> &e, ProjectPanel *p);

  QByteArray premaximized_state_;  ///< 存储面板最大化之前的停靠布局状态。

  // Standard panels / 标准面板实例指针
  ProjectPanel *project_panel_;                 ///< 主项目浏览器面板。
  NodePanel *node_panel_;                       ///< 节点编辑器面板。
  ParamPanel *param_panel_;                     ///< 参数/属性编辑器面板。
  CurvePanel *curve_panel_;                     ///< 曲线编辑器面板 (用于动画等)。
  SequenceViewerPanel *sequence_viewer_panel_;  ///< 序列查看器/监视器面板。
  FootageViewerPanel *footage_viewer_panel_;    ///< 素材查看器面板。
  QList<ProjectPanel *> folder_panels_;         ///< 打开的文件夹视图面板列表 (可能多个)。
  ToolPanel *tool_panel_;                       ///< 工具选择面板/工具栏。
  QList<TimelinePanel *> timeline_panels_;      ///< 打开的时间轴面板列表 (可能多个)。
  AudioMonitorPanel *audio_monitor_panel_;      ///< 音频监视器/电平表面板。
  TaskManagerPanel *task_man_panel_;            ///< 任务管理器面板。
  PixelSamplerPanel *pixel_sampler_panel_;      ///< 像素采样器/颜色拾取器面板。
  ScopePanel *scope_panel_;                     ///< 视频示波器面板 (如波形图、矢量图)。
  QList<ViewerPanel *> viewer_panels_;          ///< 所有查看器类型面板的通用列表。
  MulticamPanel *multicam_panel_;               ///< 多机位编辑面板。
  HistoryPanel *history_panel_;                 ///< 撤销/重做历史记录面板。

#ifdef Q_OS_WINDOWS
  // Windows 平台特定的任务栏集成成员
  unsigned int taskbar_btn_id_;       ///< 任务栏按钮标识符 (未使用或已废弃)。
  ITaskbarList3 *taskbar_interface_;  ///< 指向 Windows ITaskbarList3 COM 接口的指针，用于控制任务栏进度条。
#endif

  bool first_show_;   ///< 标记主窗口是否是第一次显示。
  Project *project_;  ///< 指向当前活动的项目对象的指针。

 private slots:
  /**
   * @brief 当停靠小部件的焦点发生变化时调用的槽函数。
   * @param panel 指向新获得焦点的 PanelWidget (或其派生类) 对象的指针。
   */
  void FocusedPanelChanged(PanelWidget *panel);

  /**
   * @brief 更新主窗口标题栏文本的槽函数。
   *
   * 通常在项目名称更改或保存状态更改时调用。
   */
  void UpdateTitle();

  /**
   * @brief 当时间轴面板请求关闭时调用的槽函数。
   */
  void TimelineCloseRequested();

  /**
   * @brief 当查看器面板请求关闭时调用的槽函数。
   */
  void ViewerCloseRequested();

  /**
   * @brief 当与面板关联的查看器节点从节点图中移除时调用的槽函数。
   */
  void ViewerWithPanelRemovedFromGraph();

  /**
   * @brief 当文件夹面板请求关闭时调用的槽函数。
   */
  void FolderPanelCloseRequested();

  /**
   * @brief 当状态栏被双击时调用的槽函数。
   *
   * 通常用于显示任务管理器面板。
   */
  void StatusBarDoubleClicked();

  /**
   * @brief 当节点编辑器面板中的某个组被打开或关闭时调用的槽函数。
   */
  void NodePanelGroupOpenedOrClosed();

#ifdef Q_OS_LINUX
  /**
   * @brief 在 Linux 平台上，如果检测到 Nouveau (开源NVIDIA驱动)，显示警告信息的槽函数。
   *
   * Nouveau 驱动可能与某些 OpenGL 功能的兼容性或性能不佳。
   */
  void ShowNouveauWarning();
#endif

  /**
   * @brief 当时间轴面板中的选区发生变化时调用的槽函数。
   * @param blocks 当前选中的 Block 对象列表。
   */
  void TimelinePanelSelectionChanged(const QVector<Block *> &blocks);

  /**
   * @brief 显示欢迎对话框的槽函数（可能在首次启动时）。
   */
  void ShowWelcomeDialog();

  /**
   * @brief 静态槽函数，在项目面板中显示（定位到）指定的查看器输出节点。
   * @param r 指向 ViewerOutput 节点的指针。
   */
  static void RevealViewerInProject(ViewerOutput *r);

  /**
   * @brief 在素材查看器中显示指定的查看器输出节点，并可能定位到特定时间范围。
   * @param r 指向 ViewerOutput 节点的指针。
   * @param range 要在素材查看器中聚焦的时间范围。
   */
  void RevealViewerInFootageViewer(ViewerOutput *r, const TimeRange &range);
};

}  // namespace olive

#endif
