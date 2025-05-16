#ifndef KD_DOCK_LOGGING_P_H
#define KD_DOCK_LOGGING_P_H

#include <QLoggingCategory> // 包含 Qt 日志类别相关的头文件

// 声明 KDDockWidgets 内部使用的各种日志类别
// 这些类别允许开发者在调试或追踪特定模块行为时，选择性地启用或禁用相关日志输出。

Q_DECLARE_LOGGING_CATEGORY(hovering) ///< 用于记录与鼠标悬停事件相关的日志信息。
Q_DECLARE_LOGGING_CATEGORY(creation) ///< 用于记录对象创建和销毁过程中的日志信息。
Q_DECLARE_LOGGING_CATEGORY(mouseevents) ///< 用于记录鼠标事件（如按下、释放、移动）相关的日志信息。
Q_DECLARE_LOGGING_CATEGORY(state) ///< 用于记录状态机或对象状态变化相关的日志信息。
Q_DECLARE_LOGGING_CATEGORY(docking) ///< 用于记录停靠操作（如停靠、浮动、分离标签页）相关的日志信息。
Q_DECLARE_LOGGING_CATEGORY(globalevents) ///< 用于记录全局事件过滤器或系统级事件相关的日志信息。
Q_DECLARE_LOGGING_CATEGORY(hiding) ///< 用于记录小部件显示和隐藏操作相关的日志信息。
Q_DECLARE_LOGGING_CATEGORY(closing) ///< 用于记录小部件关闭操作（如用户点击关闭按钮）相关的日志信息。
Q_DECLARE_LOGGING_CATEGORY(overlay) ///< 用于记录与覆盖层（如拖放指示器、自动隐藏侧边栏）相关的日志信息。
Q_DECLARE_LOGGING_CATEGORY(dropping) ///< 用于记录拖放操作中“放置” (drop) 阶段相关的日志信息。
Q_DECLARE_LOGGING_CATEGORY(restoring) ///< 用于记录布局恢复过程相关的日志信息。
Q_DECLARE_LOGGING_CATEGORY(title) ///< 用于记录与标题栏或窗口标题更新相关的日志信息。
Q_DECLARE_LOGGING_CATEGORY(closebutton) ///< 用于记录与关闭按钮交互相关的日志信息。
Q_DECLARE_LOGGING_CATEGORY(sizing) ///< 用于记录小部件大小调整、尺寸约束计算等相关的日志信息。
Q_DECLARE_LOGGING_CATEGORY(multisplittercreation) ///< 用于记录 MultiSplitter 及其内部项创建过程的日志信息。
Q_DECLARE_LOGGING_CATEGORY(addwidget) ///< 用于记录向布局或容器中添加小部件操作相关的日志信息。
Q_DECLARE_LOGGING_CATEGORY(anchors) ///< 用于记录与锚点布局或侧边栏锚定功能相关的日志信息。
Q_DECLARE_LOGGING_CATEGORY(item) ///< 用于记录布局引擎中 Item (项) 操作相关的日志信息。
Q_DECLARE_LOGGING_CATEGORY(placeholder) ///< 用于记录布局中占位符 (Placeholder) 操作相关的日志信息。
Q_DECLARE_LOGGING_CATEGORY(toplevels) ///< 用于记录顶层窗口（如 MainWindow, FloatingWindow）管理相关的日志信息。

#endif // KD_DOCK_LOGGING_P_H