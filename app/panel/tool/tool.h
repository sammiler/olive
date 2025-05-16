#ifndef TOOL_PANEL_H  // 防止头文件被重复包含的宏
#define TOOL_PANEL_H  // 定义 TOOL_PANEL_H 宏

#include "panel/panel.h"  // 包含 PanelWidget 基类的定义

namespace olive {  // olive 项目的命名空间

/**
 * @brief ToolPanel 类是一个 PanelWidget 的包装器，用于封装和管理一个工具栏 (Toolbar)。
 *
 * 这个面板通常会包含一系列工具按钮，用户可以通过点击这些按钮来选择不同的编辑工具
 * (例如，选择工具、剪切工具、文本工具、绘制工具等)。
 * ToolPanel 本身可能不直接实现工具栏的UI，而是作为标准工具栏 (如 QToolBar)
 * 或自定义工具栏控件的一个可停靠面板容器。
 *
 * 它的主要职责是提供一个区域来放置这些工具按钮，并可能处理与工具选择相关的逻辑。
 */
class ToolPanel : public PanelWidget {  // ToolPanel 继承自 PanelWidget
 Q_OBJECT                               // 声明此类使用 Qt 的元对象系统

     public :
     // 构造函数
     ToolPanel();

 private:
  /**
   * @brief 重写基类的 Retranslate 方法，用于在语言更改时更新此面板内UI元素的文本。
   * 例如，可能会更新面板标题或工具栏中按钮的提示文本 (tooltip)。
   */
  void Retranslate() override;

  // 注意：实际的工具栏控件 (例如 QToolBar 或自定义的工具栏 widget)
  // 通常会在 .cpp 文件的构造函数中创建并添加到此面板中。
  // 此头文件主要声明面板的类结构。
};

}  // namespace olive

#endif  // TOOL_PANEL_H