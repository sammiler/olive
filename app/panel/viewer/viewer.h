#ifndef VIEWER_PANEL_H // 防止头文件被重复包含的宏
#define VIEWER_PANEL_H // 定义 VIEWER_PANEL_H 宏

#include <QOpenGLFunctions> // Qt OpenGL 功能基类 (如果查看器使用OpenGL直接渲染，则可能需要)

#include "viewerbase.h"     // 包含 ViewerPanelBase 基类的定义

namespace olive { // olive 项目的命名空间

/**
 * @brief ViewerPanel 类是一个可停靠的面板，用于包装和管理一个通用的查看器控件 (ViewerWidget)。
 *
 * 它继承自 ViewerPanelBase，后者可能提供了查看器面板所需的一些基础功能和接口
 * (例如连接到数据源、播放控制信号等)。
 * ViewerPanel 本身可以作为更具体的查看器面板 (如 SequenceViewerPanel, FootageViewerPanel)
 * 的直接基类，或者在某些情况下直接使用以显示某种通用类型的视觉内容。
 *
 * 主要职责是提供一个标准的 Olive 面板容器来展示 ViewerWidget 的内容。
 */
class ViewerPanel : public ViewerPanelBase { // ViewerPanel 继承自 ViewerPanelBase
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  /**
   * @brief 构造函数。
   * @param object_name 面板的 Qt 对象名称，可用于样式表或查找。
   */
  explicit ViewerPanel(const QString& object_name);

protected:
  /**
   * @brief 重写基类的 Retranslate 方法，用于在语言更改时更新此面板内UI元素的文本。
   * 例如，可能会更新面板标题或内部查看器控件的标签和提示。
   */
  void Retranslate() override;

  // 注意：实际的 ViewerWidget 实例 (或其派生类) 通常由 ViewerPanelBase
  // 或 PanelWidget 基类的机制来创建和管理，例如通过 setCentralWidget。
  // ViewerPanel 作为其直接子类，会继承这些管理能力。
};

}  // namespace olive

#endif  // VIEWER_PANEL_H