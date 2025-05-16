#ifndef MAINMENU_H
#define MAINMENU_H

#include <QActionGroup>  // 引入 QActionGroup，用于管理一组互斥的 QAction (例如工具选择)
#include <QEvent>        // 引入 QEvent，用于 changeEvent
#include <QMainWindow>   // 引入 QMainWindow 类，MainMenu 通常附加到主窗口
#include <QMenuBar>      // 引入 QMenuBar 基类，MainMenu 继承自它

#include "dialog/actionsearch/actionsearch.h"  // 引入动作搜索对话框的定义
#include "widget/menu/menu.h"                  // 引入自定义 Menu 类的定义

namespace olive {

// 前向声明 MainWindow 类，MainMenu 是 MainWindow 的一部分
class MainWindow;

/**
 * @brief Olive 应用程序的主菜单栏，附加到其主窗口。
 *
 * MainMenu 类负责创建菜单、连接信号/槽，并在应用程序语言更改时重新翻译菜单项。
 * 它包含了文件、编辑、视图、播放、序列、窗口、工具和帮助等标准菜单。
 */
class MainMenu : public QMenuBar {
 Q_OBJECT  // 声明此类使用 Qt 的元对象系统（信号和槽）
     public :
     /**
      * @brief 显式构造函数。
      * @param parent 指向父 MainWindow 对象的指针。MainMenu 将作为此主窗口的菜单栏。
      */
     explicit MainMenu(MainWindow* parent);

 protected:
  /**
   * @brief 重写 changeEvent 以捕获 QEvent::LanguageChange 事件。
   *
   * 当应用程序语言发生变化时，此事件会被触发，MainMenu 会调用 Retranslate() 来更新所有菜单项的文本。
   * @param e 指向发生的 QEvent 对象的指针。
   */
  void changeEvent(QEvent* e) override;

 private slots:
  /**
   * @brief 处理工具选择菜单项触发的槽函数。
   *
   * 假设 sender() 是一个 QAction*，并且其 data() 存储了 Tool::Item 枚举的成员。
   * 此槽函数使用该 data() 来发出一个信号，通知应用程序其他部分工具已更改。
   */
  void ToolItemTriggered();

  /**
   * @brief “文件”菜单即将显示时触发的槽函数。
   *
   * 可用于在菜单显示前动态更新其内容（例如，启用/禁用某些项）。
   */
  void FileMenuAboutToShow();

  /**
   * @brief “编辑”菜单即将显示时触发的槽函数。
   */
  void EditMenuAboutToShow();
  /**
   * @brief “编辑”菜单即将隐藏时触发的槽函数。
   */
  void EditMenuAboutToHide();

  /**
   * @brief “视图”菜单即将显示时触发的槽函数。
   */
  void ViewMenuAboutToShow();

  /**
   * @brief “工具”菜单即将显示时触发的槽函数。
   */
  void ToolsMenuAboutToShow();

  /**
   * @brief “播放”菜单即将显示时触发的槽函数。
   */
  void PlaybackMenuAboutToShow();

  /**
   * @brief “序列”菜单即将显示时触发的槽函数。
   */
  void SequenceMenuAboutToShow();

  /**
   * @brief “窗口”菜单即将显示时触发的槽函数。
   */
  void WindowMenuAboutToShow();

  /**
   * @brief 填充“打开最近文件”子菜单的槽函数。
   *
   * 动态加载最近打开过的项目列表到菜单中。
   */
  void PopulateOpenRecent();

  /**
   * @brief 重新填充“打开最近文件”子菜单的槽函数。
   *
   * 可能在最近文件列表更新后调用。
   */
  void RepopulateOpenRecent();

  /**
   * @brief 当“打开最近文件”菜单关闭时，清除其动态添加的项的槽函数。
   */
  void CloseOpenRecentMenu();

  /**
   * @brief “放大”操作触发的槽函数。
   *
   * 查找当前拥有焦点的面板，并向其发送“放大”信号或调用其放大方法。
   */
  static void ZoomInTriggered();

  /**
   * @brief “缩小”操作触发的槽函数。
   *
   * 查找当前拥有焦点的面板，并向其发送“缩小”信号或调用其缩小方法。
   */
  static void ZoomOutTriggered();

  /**
   * @brief “增加轨道高度”操作触发的槽函数。
   */
  static void IncreaseTrackHeightTriggered();
  /**
   * @brief “减小轨道高度”操作触发的槽函数。
   */
  static void DecreaseTrackHeightTriggered();

  /**
   * @brief “跳转到开头”播放操作触发的槽函数。
   */
  static void GoToStartTriggered();
  /**
   * @brief “上一帧”播放操作触发的槽函数。
   */
  static void PrevFrameTriggered();

  /**
   * @brief “播放/暂停”操作触发的槽函数。
   *
   * 查找当前拥有焦点的面板（通常是播放器或时间轴），并发送“播放/暂停”信号。
   */
  static void PlayPauseTriggered();

  /**
   * @brief “播放入点到出点范围”操作触发的槽函数。
   */
  static void PlayInToOutTriggered();

  /**
   * @brief “循环播放”开关状态改变时触发的槽函数。
   * @param enabled true 表示启用循环播放，false 表示禁用。
   */
  static void LoopTriggered(bool enabled);

  /**
   * @brief “下一帧”播放操作触发的槽函数。
   */
  static void NextFrameTriggered();
  /**
   * @brief “跳转到末尾”播放操作触发的槽函数。
   */
  static void GoToEndTriggered();

  /**
   * @brief “全选”编辑操作触发的槽函数。
   */
  static void SelectAllTriggered();
  /**
   * @brief “取消全选”编辑操作触发的槽函数。
   */
  static void DeselectAllTriggered();

  /**
   * @brief “插入模式”编辑操作触发的槽函数。
   */
  static void InsertTriggered();
  /**
   * @brief “覆盖模式”编辑操作触发的槽函数。
   */
  static void OverwriteTriggered();

  /**
   * @brief “涟漪裁切至入点”编辑操作触发的槽函数。
   */
  static void RippleToInTriggered();
  /**
   * @brief “涟漪裁切至出点”编辑操作触发的槽函数。
   */
  static void RippleToOutTriggered();
  /**
   * @brief “编辑至入点”（非涟漪）操作触发的槽函数。
   */
  static void EditToInTriggered();
  /**
   * @brief “编辑至出点”（非涟漪）操作触发的槽函数。
   */
  static void EditToOutTriggered();

  /**
   * @brief “向左微移”编辑操作触发的槽函数。
   */
  static void NudgeLeftTriggered();
  /**
   * @brief “向右微移”编辑操作触发的槽函数。
   */
  static void NudgeRightTriggered();
  /**
   * @brief “移动入点到播放头”编辑操作触发的槽函数。
   */
  static void MoveInToPlayheadTriggered();
  /**
   * @brief “移动出点到播放头”编辑操作触发的槽函数。
   */
  static void MoveOutToPlayheadTriggered();

  /**
   * @brief 触发“动作搜索”对话框的槽函数。
   */
  void ActionSearchTriggered();

  /**
   * @brief “向左快速穿梭”播放操作触发的槽函数。
   */
  static void ShuttleLeftTriggered();
  /**
   * @brief “停止快速穿梭”播放操作触发的槽函数。
   */
  static void ShuttleStopTriggered();
  /**
   * @brief “向右快速穿梭”播放操作触发的槽函数。
   */
  static void ShuttleRightTriggered();

  /**
   * @brief “跳转到上一个剪辑点”操作触发的槽函数。
   */
  static void GoToPrevCutTriggered();
  /**
   * @brief “跳转到下一个剪辑点”操作触发的槽函数。
   */
  static void GoToNextCutTriggered();

  /**
   * @brief “设置标记”操作触发的槽函数。
   */
  static void SetMarkerTriggered();

  /**
   * @brief “全屏查看器”操作触发的槽函数。
   */
  static void FullScreenViewerTriggered();

  /**
   * @brief “切换显示所有轨道/内容”操作触发的槽函数。
   */
  static void ToggleShowAllTriggered();

  /**
   * @brief “删除入点到出点范围内容”（非涟漪）操作触发的槽函数。
   */
  static void DeleteInOutTriggered();
  /**
   * @brief “涟漪删除入点到出点范围内容”操作触发的槽函数。
   */
  static void RippleDeleteInOutTriggered();

  /**
   * @brief “跳转到入点”操作触发的槽函数。
   */
  static void GoToInTriggered();
  /**
   * @brief “跳转到出点”操作触发的槽函数。
   */
  static void GoToOutTriggered();

  /**
   * @brief “打开最近文件”子菜单中的某一项被触发时的槽函数。
   */
  void OpenRecentItemTriggered();

  /**
   * @brief “缓存序列”操作触发的槽函数。
   */
  static void SequenceCacheTriggered();
  /**
   * @brief “缓存序列入点到出点范围”操作触发的槽函数。
   */
  static void SequenceCacheInOutTriggered();
  /**
   * @brief “清除序列磁盘缓存”操作触发的槽函数。
   */
  static void SequenceCacheClearTriggered();

  /**
   * @brief “帮助/反馈”操作触发的槽函数。
   */
  static void HelpFeedbackTriggered();

 private:
  /**
   * @brief 根据当前应用程序语言设置菜单项的文本。
   *
   * 此方法在构造函数和语言更改事件中被调用。
   */
  void Retranslate();

  // 菜单对象指针
  Menu* file_menu_;                        ///< 文件菜单
  Menu* file_new_menu_;                    ///< 文件 -> 新建 子菜单
  QAction* file_open_item_;                ///< 文件 -> 打开 项目
  Menu* file_open_recent_menu_;            ///< 文件 -> 打开最近文件 子菜单
  QAction* file_open_recent_separator_;    ///< "打开最近文件"菜单中的分隔符
  QAction* file_open_recent_clear_item_;   ///< 文件 -> 打开最近文件 -> 清除列表
  QAction* file_save_item_;                ///< 文件 -> 保存
  QAction* file_save_as_item_;             ///< 文件 -> 另存为
  QAction* file_revert_item_;              ///< 文件 -> 恢复到上次保存
  QAction* file_import_item_;              ///< 文件 -> 导入媒体
  Menu* file_export_menu_;                 ///< 文件 -> 导出 子菜单
  QAction* file_export_media_item_;        ///< 文件 -> 导出 -> 媒体
  QAction* file_project_properties_item_;  ///< 文件 -> 项目属性
  QAction* file_exit_item_;                ///< 文件 -> 退出

  Menu* edit_menu_;                          ///< 编辑菜单
  QAction* edit_undo_item_;                  ///< 编辑 -> 撤销
  QAction* edit_redo_item_;                  ///< 编辑 -> 重做
  QAction* edit_delete2_item_;               ///< 编辑 -> 删除 (可能与标准删除键行为不同或有特定上下文)
  QAction* edit_select_all_item_;            ///< 编辑 -> 全选
  QAction* edit_deselect_all_item_;          ///< 编辑 -> 取消全选
  QAction* edit_insert_item_;                ///< 编辑 -> 插入模式
  QAction* edit_overwrite_item_;             ///< 编辑 -> 覆盖模式
  QAction* edit_ripple_to_in_item_;          ///< 编辑 -> 涟漪裁切至入点
  QAction* edit_ripple_to_out_item_;         ///< 编辑 -> 涟漪裁切至出点
  QAction* edit_edit_to_in_item_;            ///< 编辑 -> 编辑至入点
  QAction* edit_edit_to_out_item_;           ///< 编辑 -> 编辑至出点
  QAction* edit_nudge_left_item_;            ///< 编辑 -> 向左微移
  QAction* edit_nudge_right_item_;           ///< 编辑 -> 向右微移
  QAction* edit_move_in_to_playhead_item_;   ///< 编辑 -> 移动入点到播放头
  QAction* edit_move_out_to_playhead_item_;  ///< 编辑 -> 移动出点到播放头
  QAction* edit_delete_inout_item_;          ///< 编辑 -> 删除入点到出点范围
  QAction* edit_ripple_delete_inout_item_;   ///< 编辑 -> 涟漪删除入点到出点范围
  QAction* edit_set_marker_item_;            ///< 编辑 -> 设置标记

  Menu* view_menu_;                           ///< 视图菜单
  QAction* view_zoom_in_item_;                ///< 视图 -> 放大
  QAction* view_zoom_out_item_;               ///< 视图 -> 缩小
  QAction* view_increase_track_height_item_;  ///< 视图 -> 增加轨道高度
  QAction* view_decrease_track_height_item_;  ///< 视图 -> 减小轨道高度
  QAction* view_show_all_item_;               ///< 视图 -> 显示全部 (例如，缩放到适合所有内容)
  QAction* view_full_screen_item_;            ///< 视图 -> 全屏显示应用程序
  QAction* view_full_screen_viewer_item_;     ///< 视图 -> 全屏查看器

  Menu* playback_menu_;                  ///< 播放菜单
  QAction* playback_gotostart_item_;     ///< 播放 -> 跳转到开头
  QAction* playback_prevframe_item_;     ///< 播放 -> 上一帧
  QAction* playback_playpause_item_;     ///< 播放 -> 播放/暂停
  QAction* playback_playinout_item_;     ///< 播放 -> 播放入点到出点范围
  QAction* playback_nextframe_item_;     ///< 播放 -> 下一帧
  QAction* playback_gotoend_item_;       ///< 播放 -> 跳转到末尾
  QAction* playback_prevcut_item_;       ///< 播放 -> 跳转到上一个剪辑点
  QAction* playback_nextcut_item_;       ///< 播放 -> 跳转到下一个剪辑点
  QAction* playback_gotoin_item_;        ///< 播放 -> 跳转到入点
  QAction* playback_gotoout_item_;       ///< 播放 -> 跳转到出点
  QAction* playback_shuttleleft_item_;   ///< 播放 -> 向左快速穿梭
  QAction* playback_shuttlestop_item_;   ///< 播放 -> 停止快速穿梭
  QAction* playback_shuttleright_item_;  ///< 播放 -> 向右快速穿梭
  QAction* playback_loop_item_;          ///< 播放 -> 循环播放 (可勾选)

  Menu* sequence_menu_;                      ///< 序列菜单
  QAction* sequence_cache_item_;             ///< 序列 -> 缓存序列
  QAction* sequence_cache_in_to_out_item_;   ///< 序列 -> 缓存入点到出点范围
  QAction* sequence_disk_cache_clear_item_;  ///< 序列 -> 清除磁盘缓存

  Menu* window_menu_;                    ///< 窗口菜单
  QAction* window_menu_separator_;       ///< 窗口菜单中的分隔符
  QAction* window_maximize_panel_item_;  ///< 窗口 -> 最大化当前面板
  QAction* window_reset_layout_item_;    ///< 窗口 -> 重置布局

  Menu* tools_menu_;                 ///< 工具菜单
  QActionGroup* tools_group_;        ///< 工具菜单项的动作组，确保工具选择的互斥性
  QAction* tools_pointer_item_;      ///< 工具 -> 指针
  QAction* tools_trackselect_item_;  ///< 工具 -> 轨道选择
  QAction* tools_edit_item_;         ///< 工具 -> 编辑
  QAction* tools_ripple_item_;       ///< 工具 -> 涟漪
  QAction* tools_rolling_item_;      ///< 工具 -> 卷动
  QAction* tools_razor_item_;        ///< 工具 -> 剃刀
  QAction* tools_slip_item_;         ///< 工具 -> 滑移
  QAction* tools_slide_item_;        ///< 工具 -> 滑动
  QAction* tools_hand_item_;         ///< 工具 -> 手型
  QAction* tools_zoom_item_;         ///< 工具 -> 缩放
  QAction* tools_transition_item_;   ///< 工具 -> 转场
  QAction* tools_add_item_;          ///< 工具 -> 添加 (通常是一个子菜单)
  QAction* tools_record_item_;       ///< 工具 -> 录制
  QAction* tools_snapping_item_;     ///< 工具 -> 吸附 (可勾选)
  QAction* tools_preferences_item_;  ///< 工具 -> 首选项
  Menu* tools_add_item_menu_;        ///< 工具 -> 添加 子菜单

#ifndef NDEBUG                 // 仅在非调试（发布）模式下编译以下内容
  QAction* tools_magic_item_;  ///< 工具 -> 魔法棒 (可能是一个调试或特殊功能项)
#endif

  Menu* help_menu_;                   ///< 帮助菜单
  QAction* help_action_search_item_;  ///< 帮助 -> 动作搜索
  QAction* help_feedback_item_;       ///< 帮助 -> 发送反馈
  QAction* help_about_item_;          ///< 帮助 -> 关于
};

}  // namespace olive

#endif  // MAINMENU_H
