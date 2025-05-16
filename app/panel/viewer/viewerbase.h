#ifndef VIEWERPANELBASE_H  // 防止头文件被重复包含的宏
#define VIEWERPANELBASE_H  // 定义 VIEWERPANELBASE_H 宏

#include "panel/pixelsampler/pixelsamplerpanel.h"  // 包含像素采样器面板 (PixelSamplerPanel) 的定义 (可能用于交互)
#include "panel/timebased/timebased.h"             // 包含 TimeBasedPanel 基类的定义
#include "widget/viewer/viewer.h"                  // 包含 ViewerWidget (查看器核心控件) 基类的定义

namespace olive {  // olive 项目的命名空间

class MulticamWidget;  // 向前声明 MulticamWidget 类

/**
 * @brief ViewerPanelBase 类是所有特定类型查看器面板 (如序列查看器、素材查看器) 的通用基类。
 *
 * 它继承自 TimeBasedPanel，因此拥有时间导航、播放控制和连接到 ViewerOutput 的能力。
 * ViewerPanelBase 进一步封装了一个 ViewerWidget 控件，该控件负责实际的图像/视频帧的渲染和显示，
 * 以及可能的屏幕交互 (如 Gizmos、文本编辑)。
 *
 * 这个基类提供了一些通用的查看器面板功能，例如全屏切换、颜色管理访问、
 * 以及与其他面板 (如时间轴、像素采样器) 的联动。
 */
class ViewerPanelBase : public TimeBasedPanel {  // ViewerPanelBase 继承自 TimeBasedPanel
 Q_OBJECT                                        // 声明此类使用 Qt 的元对象系统

     public :
     /**
      * @brief 构造函数。
      * @param object_name 面板的 Qt 对象名称。
      */
     explicit ViewerPanelBase(const QString &object_name);

  /**
   * @brief 获取内部封装的 ViewerWidget 控件的指针。
   * 通过动态类型转换从基类 (TimeBasedPanel) 提供的 GetTimeBasedWidget() 方法获取。
   * @return 返回 ViewerWidget 指针，如果转换失败或内部控件不是 ViewerWidget 类型，则返回 nullptr。
   */
  [[nodiscard]] ViewerWidget *GetViewerWidget() const {
    // 将基类提供的 TimeBasedWidget 动态转换为 ViewerWidget
    return dynamic_cast<ViewerWidget *>(GetTimeBasedWidget());
  }

  // --- 重写自 TimeBasedPanel 的播放控制方法 ---
  // ViewerPanelBase 通常会更具体地实现这些播放控制，
  // 可能会直接操作内部的 ViewerWidget 或发出更具体的播放请求。
  void PlayPause() override;
  void PlayInToOut() override;
  void ShuttleLeft() override;
  void ShuttleStop() override;
  void ShuttleRight() override;

  /**
   * @brief 将指定的 TimeBasedPanel (例如时间轴) 连接到此查看器面板。
   * 这通常用于同步时间、播放状态等。
   * @param panel 要连接的 TimeBasedPanel 指针。
   */
  void ConnectTimeBasedPanel(TimeBasedPanel *panel) const;

  /**
   * @brief断开指定的 TimeBasedPanel 与此查看器面板的连接。
   * @param panel 要断开的 TimeBasedPanel 指针。
   */
  void DisconnectTimeBasedPanel(TimeBasedPanel *panel) const;

  /**
   * @brief ViewerWidget::SetFullScreen() 的包装器方法，用于设置查看器全屏显示。
   * @param screen (可选) 指定要在哪个屏幕上全屏显示。如果为 nullptr，则通常在当前屏幕全屏。
   */
  void SetFullScreen(QScreen *screen = nullptr) const;

  /**
   * @brief 获取当前查看器使用的色彩管理器。
   * @return 返回 ColorManager 指针。
   */
  [[nodiscard]] ColorManager *GetColorManager() const { return GetViewerWidget()->color_manager(); }

  /**
   * @brief 请求内部的 ViewerWidget 从其连接的节点更新显示的纹理。
   */
  void UpdateTextureFromNode() const { GetViewerWidget()->UpdateTextureFromNode(); }

  /**
   * @brief向内部的 ViewerWidget 添加一个额外的播放设备 (显示控件)。
   * 这可能用于支持外部监视器或第二个显示窗口。
   * @param vw 指向要添加的 ViewerDisplayWidget 的指针。
   */
  void AddPlaybackDevice(ViewerDisplayWidget *vw) const { GetViewerWidget()->AddPlaybackDevice(vw); }

  /**
   * @brief 将时间轴上选中的 Block (片段) 信息传递给内部的 ViewerWidget。
   * ViewerWidget 可能需要这些信息来进行某些上下文相关的操作或显示。
   * @param b 包含选中 Block 指针的 QVector。
   */
  void SetTimelineSelectedBlocks(const QVector<Block *> &b) const { GetViewerWidget()->SetTimelineSelectedBlocks(b); }

  /**
   * @brief 将节点编辑器中选中的节点信息传递给内部的 ViewerWidget。
   * @param n 包含选中 Node 指针的 QVector。
   */
  void SetNodeViewSelections(const QVector<Node *> &n) const { GetViewerWidget()->SetNodeViewSelections(n); }

  /**
   * @brief 将多机位控件 (MulticamWidget) 连接到内部的 ViewerWidget。
   * 这用于在多机位编辑时，查看器能够响应多机位控件的切换。
   * @param p 指向 MulticamWidget 的指针。
   */
  void ConnectMulticamWidget(MulticamWidget *p) const { GetViewerWidget()->ConnectMulticamWidget(p); }

 public slots:  // Qt 公有槽函数
  /**
   * @brief 为指定的节点设置或更新在查看器中显示的 Gizmos (屏幕控制器)。
   * @param node 要为其显示 Gizmos 的节点指针。如果为 nullptr，则可能清除当前 Gizmos。
   */
  void SetGizmos(Node *node) const;

  /**
   * @brief 请求缓存当前连接序列的整个时长。
   */
  void CacheEntireSequence() const;

  /**
   * @brief 请求缓存当前连接序列的入点到出点之间的范围。
   */
  void CacheSequenceInOut() const;

  /**
   * @brief 请求内部的 ViewerWidget 开始文本编辑模式。
   * 当用户想要在查看器上直接编辑文本节点的内容时调用。
   */
  void RequestStartEditingText() const { GetViewerWidget()->RequestStartEditingText(); }

 signals:  // Qt 信号声明
  /**
   * @brief 当查看器加载并显示了新的视频帧 (纹理) 时发出的信号。
   * @param t 指向新帧纹理的 TexturePtr。
   */
  void TextureChanged(TexturePtr t);

  /**
   * @brief ViewerGLWidget::ColorProcessorChanged() 的包装信号。
   * 当查看器的颜色处理器发生改变时发出。
   * @param processor 指向新的 ColorProcessorPtr。
   */
  void ColorProcessorChanged(ColorProcessorPtr processor);

  /**
   * @brief ViewerGLWidget::ColorManagerChanged() 的包装信号。
   * 当查看器使用的色彩管理器发生改变时发出。
   * @param color_manager 指向新的 ColorManager。
   */
  void ColorManagerChanged(ColorManager *color_manager);

 protected:
  /**
   * @brief 设置此查看器面板的核心显示控件 (必须是 ViewerWidget 或其派生类)。
   * 这个方法通常在派生类的构造函数中被调用。
   * @param vw 指向 ViewerWidget 的指针。
   */
  void SetViewerWidget(ViewerWidget *vw);

 private slots:  // Qt 私有槽函数
  /**
   * @brief 当应用程序中当前聚焦的面板发生改变时的处理槽函数。
   * 查看器面板可能需要根据哪个面板是活动的来调整自身行为 (例如，是否响应快捷键)。
   * @param panel 当前聚焦的 PanelWidget 指针。
   */
  void FocusedPanelChanged(PanelWidget *panel);

  // 注意： ViewerPanelBase 自身不直接创建 ViewerWidget，而是通过 SetViewerWidget
  // 由其派生类 (如 ViewerPanel, SequenceViewerPanel) 在构造时传入具体的 ViewerWidget 实例。
  // 之后通过 GetTimeBasedWidget() -> dynamic_cast<ViewerWidget*>() 来访问。
};

}  // namespace olive

#endif  // VIEWERPANELBASE_H