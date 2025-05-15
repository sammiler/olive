

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <kddockwidgets/Config.h>
#include <kddockwidgets/MainWindow.h>

#include "mainwindowlayoutinfo.h"
#include "node/project.h"
#include "panel/audiomonitor/audiomonitor.h"
#include "panel/curve/curve.h"
#include "panel/footageviewer/footageviewer.h"
#include "panel/history/historypanel.h"
#include "panel/multicam/multicampanel.h"
#include "panel/node/node.h"
#include "panel/panelmanager.h"
#include "panel/param/param.h"
#include "panel/pixelsampler/pixelsamplerpanel.h"
#include "panel/project/project.h"
#include "panel/scope/scope.h"
#include "panel/sequenceviewer/sequenceviewer.h"
#include "panel/table/table.h"
#include "panel/taskmanager/taskmanager.h"
#include "panel/timeline/timeline.h"
#include "panel/tool/tool.h"

#ifdef Q_OS_WINDOWS
#include <shobjidl.h>
#endif

namespace olive {

/**
 * @brief Olive's main window responsible for docking widgets and the main menu bar.
 */
class MainWindow : public KDDockWidgets::MainWindow {
  Q_OBJECT
 public:
  explicit MainWindow(QWidget *parent = nullptr);

  ~MainWindow() override;

  void LoadLayout(const MainWindowLayoutInfo &info);

  [[nodiscard]] MainWindowLayoutInfo SaveLayout() const;

  TimelinePanel *OpenSequence(Sequence *sequence, bool enable_focus = true);

  void CloseSequence(Sequence *sequence);

  bool IsSequenceOpen(Sequence *sequence) const;

  void OpenFolder(Folder *i, bool floating);

  void OpenNodeInViewer(ViewerOutput *node);

  enum ProgressStatus { kProgressNone, kProgressShow, kProgressError };

  /**
   * @brief Where applicable, show progress on an operating system level
   *
   * * For Windows, this is shown as progress in the taskbar.
   * * For macOS, this is shown as progress in the dock.
   */
  void SetApplicationProgressStatus(ProgressStatus status);

  /**
   * @brief If SetApplicationProgressStatus is set to kShowProgress, set the value with this
   *
   * Expects a percentage (0-100 inclusive).
   */
  void SetApplicationProgressValue(int value);

  void SelectFootage(const QVector<Footage *> &e);

 public slots:
  void SetProject(Project *p);

  void SetFullscreen(bool fullscreen);

  void ToggleMaximizedPanel();

  void SetDefaultLayout();

 protected:
  void showEvent(QShowEvent *e) override;

  void closeEvent(QCloseEvent *e) override;

#ifdef Q_OS_WINDOWS
  virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
#endif

 private:
  TimelinePanel *AppendTimelinePanel();

  template <typename T>
  T *AppendPanelInternal(const QString &panel_name, QList<T *> &list);

  template <typename T>
  void RemovePanelInternal(QList<T *> &list, T *panel);

  void RemoveTimelinePanel(TimelinePanel *panel);

  void TimelineFocused(ViewerOutput *viewer);

  static QString GetCustomShortcutsFile();

  void LoadCustomShortcuts();

  void SaveCustomShortcuts();

  void UpdateAudioMonitorParams(ViewerOutput *viewer);

  void UpdateNodePanelContextFromTimelinePanel(TimelinePanel *panel);

  static void SelectFootageForProjectPanel(const QVector<Footage *> &e, ProjectPanel *p);

  QByteArray premaximized_state_;

  // Standard panels
  ProjectPanel *project_panel_;
  NodePanel *node_panel_;
  ParamPanel *param_panel_;
  CurvePanel *curve_panel_;
  SequenceViewerPanel *sequence_viewer_panel_;
  FootageViewerPanel *footage_viewer_panel_;
  QList<ProjectPanel *> folder_panels_;
  ToolPanel *tool_panel_;
  QList<TimelinePanel *> timeline_panels_;
  AudioMonitorPanel *audio_monitor_panel_;
  TaskManagerPanel *task_man_panel_;
  PixelSamplerPanel *pixel_sampler_panel_;
  ScopePanel *scope_panel_;
  QList<ViewerPanel *> viewer_panels_;
  MulticamPanel *multicam_panel_;
  HistoryPanel *history_panel_;

#ifdef Q_OS_WINDOWS
  unsigned int taskbar_btn_id_;

  ITaskbarList3 *taskbar_interface_;
#endif

  bool first_show_;

  Project *project_;

 private slots:
  void FocusedPanelChanged(PanelWidget *panel);

  void UpdateTitle();

  void TimelineCloseRequested();

  void ViewerCloseRequested();

  void ViewerWithPanelRemovedFromGraph();

  void FolderPanelCloseRequested();

  void StatusBarDoubleClicked();

  void NodePanelGroupOpenedOrClosed();

#ifdef Q_OS_LINUX
  void ShowNouveauWarning();
#endif

  void TimelinePanelSelectionChanged(const QVector<Block *> &blocks);

  void ShowWelcomeDialog();

  static void RevealViewerInProject(ViewerOutput *r);
  void RevealViewerInFootageViewer(ViewerOutput *r, const TimeRange &range);
};

}  // namespace olive

#endif
