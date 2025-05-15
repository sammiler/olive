#ifndef FOOTAGE_VIEWER_PANEL_H // 防止头文件被重复包含的宏
#define FOOTAGE_VIEWER_PANEL_H // 定义 FOOTAGE_VIEWER_PANEL_H 宏

#include <QOpenGLFunctions> // Qt OpenGL 功能基类 (可能用于渲染)

#include "panel/project/footagemanagementpanel.h" // 包含素材管理面板接口或基类的定义
#include "panel/viewer/viewerbase.h"              // 包含查看器面板基类 ViewerPanelBase 的定义
#include "widget/viewer/footageviewer.h"          // 包含 FootageViewerWidget 控件的定义

namespace olive { // olive 项目的命名空间

/**
 * @brief FootageViewerPanel 类是一个可停靠的面板，用于包装和管理一个 FootageViewerWidget。
 *
 * 这个面板专门用于显示和预览单个素材片段 (Footage)。
 * 它继承自 ViewerPanelBase，表明它具备查看器的一些基本功能 (如播放控制、时间导航等)。
 * 同时，它也实现了 FootageManagementPanel 的接口 (或继承自它)，这意味着它可能还包含
 * 或响应一些与素材管理相关的操作。
 *
 * 用户通常通过这个面板来查看导入的视频、音频或图像素材的原始内容。
 */
class FootageViewerPanel : public ViewerPanelBase, public FootageManagementPanel { // FootageViewerPanel 多重继承
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  // 构造函数
  FootageViewerPanel();

  /**
   * @brief (可能用于) 覆盖或设置查看器的工作区域/播放范围。
   * @param r 新的时间范围。
   */
  void OverrideWorkArea(const TimeRange &r) const;

  /**
   * @brief 获取内部封装的 FootageViewerWidget 控件的指针。
   * 通过动态类型转换从基类提供的 GetTimeBasedWidget() 获取。
   * @return 返回 FootageViewerWidget 指针，如果转换失败则返回 nullptr。
   */
  [[nodiscard]] FootageViewerWidget *GetFootageViewerWidget() const {
    // 将基类提供的 TimeBasedWidget 动态转换为 FootageViewerWidget
    return dynamic_cast<FootageViewerWidget *>(GetTimeBasedWidget());
  }

  /**
   * @brief 重写自 FootageManagementPanel 接口 (或基类) 的方法。
   * 用于获取当前在此素材查看器中被选中或正在显示的素材对应的 ViewerOutput 对象列表。
   * ViewerOutput 通常代表一个可以输出视频/音频帧的节点 (例如一个素材节点)。
   * @return 返回一个包含 ViewerOutput 指针的 QVector。
   */
  [[nodiscard]] QVector<ViewerOutput *> GetSelectedFootage() const override;

protected:
  /**
   * @brief 重写基类的 Retranslate 方法，用于在语言更改时更新此面板内UI元素的文本。
   */
  void Retranslate() override;

  // 注意：实际的 FootageViewerWidget 成员变量通常在构造函数中创建并由 ViewerPanelBase
  // (或其父类) 的机制来管理，例如通过 setCentralWidget 或类似的面板内容设置方法。
};

}  // namespace olive

#endif  // FOOTAGE_VIEWER_PANEL_H