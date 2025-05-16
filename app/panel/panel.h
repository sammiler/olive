#ifndef PANEL_WIDGET_H  // 防止头文件被重复包含的宏
#define PANEL_WIDGET_H  // 定义 PANEL_WIDGET_H 宏

#include <kddockwidgets/DockWidget.h>  // 包含第三方停靠控件库 KDDockWidgets 的 DockWidget 基类
#include <QEvent>                      // Qt 事件基类

#include "common/define.h"  // 可能包含项目通用的定义或宏

namespace olive {  // olive 项目的命名空间

/**
 * @brief PanelWidget 类是 Olive 编辑器中所有可停靠面板的基类。
 *
 * 它继承自 KDDockWidgets::DockWidget，从而获得了停靠、浮动、堆叠等窗口管理能力。
 * PanelWidget 封装了 Olive 面板共有的行为和接口，例如标题设置、边框显示、
 * 以及一系列通用的编辑和播放控制命令的虚函数接口 (如 ZoomIn, PlayPause 等)。
 * 具体的面板 (如 TimelinePanel, NodePanel) 会继承此类并实现或重写这些虚函数
 * 以提供特定于该面板的功能。
 */
class PanelWidget : public KDDockWidgets::DockWidget {  // PanelWidget 继承自 KDDockWidgets::DockWidget
 Q_OBJECT                                               // 声明此类使用 Qt 的元对象系统

     public :
     /**
      * @brief PanelWidget 构造函数。
      * @param object_name 面板的 Qt 对象名称，可用于样式表或查找。
      *        (注意：注释中的 @param parent 与参数列表不符，参数是 object_name)
      */
     explicit PanelWidget(const QString& object_name);

  // 析构函数
  ~PanelWidget() override;

  /**
   * @brief 设置面板高亮边框的可见性，主要用于显示面板焦点。
   * @param enabled 如果为 true，则显示边框；否则隐藏。
   */
  void SetBorderVisible(bool enabled);

  /**
   * @brief 如果启用，则当用户关闭面板时发送 CloseRequested() 信号，而不是直接关闭。
   *
   * 默认为 FALSE。使用此功能可以覆盖面板的默认关闭行为，例如在关闭前提示保存。
   * @param e 如果为 true，则发送信号；否则按默认行为关闭。
   */
  void SetSignalInsteadOfClose(bool e);

  // 类型别名：Info 用于存储和传递面板的配置或状态数据 (通常是字符串键值对)
  using Info = std::map<QString, QString>;

  /**
   * @brief (虚函数) 加载面板的特定数据/状态。
   * 派生类可以重写此方法以从 Info 对象中恢复其特定状态。
   * @param info 包含要加载数据的 Info 对象。
   */
  virtual void LoadData(const Info& info) {}
  /**
   * @brief (虚函数) 保存面板的特定数据/状态。
   * 派生类可以重写此方法以将其特定状态保存到 Info 对象中。
   * @return 返回包含面板数据的 Info 对象。默认实现返回一个空 Info 对象。
   */
  [[nodiscard]] virtual Info SaveData() const { return {}; }

  /**
   * @brief (虚函数) 当此面板获得焦点并且用户使用“放大”操作 (菜单或快捷键) 时调用。
   *
   * 此函数如何解释“放大”取决于具体面板的实现。默认行为是无操作。
   */
  virtual void ZoomIn() {}

  /**
   * @brief (虚函数) 当此面板获得焦点并且用户使用“缩小”操作时调用。
   *
   * 默认行为是无操作。
   */
  virtual void ZoomOut() {}

  // --- 以下为一系列通用的编辑和播放控制命令的虚函数接口 ---
  // 派生类可以根据其功能重写这些方法，以响应全局的命令。
  // 默认实现通常是无操作 (no-op)。

  virtual void GoToStart() {}  // 跳转到开始
  virtual void PrevFrame() {}  // 上一帧
  /**
   * @brief (虚函数) 当此面板获得焦点并且用户使用“播放/暂停”操作时调用。
   *
   * (注释中对Zoom Out的描述有误，应为Play/Pause)
   * 默认行为是无操作。
   */
  virtual void PlayPause() {}              // 播放/暂停
  virtual void PlayInToOut() {}            // 播放入点到出点
  virtual void NextFrame() {}              // 下一帧
  virtual void GoToEnd() {}                // 跳转到结束
  virtual void SelectAll() {}              // 全选
  virtual void DeselectAll() {}            // 取消全选
  virtual void RippleToIn() {}             // 波纹编辑到入点
  virtual void RippleToOut() {}            // 波纹编辑到出点
  virtual void EditToIn() {}               // 编辑到入点
  virtual void EditToOut() {}              // 编辑到出点
  virtual void ShuttleLeft() {}            // 向左穿梭
  virtual void ShuttleStop() {}            // 停止穿梭
  virtual void ShuttleRight() {}           // 向右穿梭
  virtual void GoToPrevCut() {}            // 跳转到上一个剪辑点
  virtual void GoToNextCut() {}            // 跳转到下一个剪辑点
  virtual void RenameSelected() {}         // 重命名选中的项
  virtual void DeleteSelected() {}         // 删除选中的项
  virtual void RippleDelete() {}           // 波纹删除
  virtual void IncreaseTrackHeight() {}    // 增加轨道高度
  virtual void DecreaseTrackHeight() {}    // 减少轨道高度
  virtual void SetIn() {}                  // 设置入点
  virtual void SetOut() {}                 // 设置出点
  virtual void ResetIn() {}                // 重置入点
  virtual void ResetOut() {}               // 重置出点
  virtual void ClearInOut() {}             // 清除入点和出点
  virtual void SetMarker() {}              // 设置标记
  virtual void ToggleLinks() {}            // 切换链接状态
  virtual void CutSelected() {}            // 剪切选中的项
  virtual void CopySelected() {}           // 复制选中的项
  virtual void Paste() {}                  // 粘贴
  virtual void PasteInsert() {}            // 粘贴并插入
  virtual void ToggleShowAll() {}          // 切换显示全部 (缩放以适应内容)
  virtual void GoToIn() {}                 // 跳转到入点
  virtual void GoToOut() {}                // 跳转到出点
  virtual void DeleteInToOut() {}          // 删除入点到出点之间的内容
  virtual void RippleDeleteInToOut() {}    // 波纹删除入点到出点之间的内容
  virtual void ToggleSelectedEnabled() {}  // 切换选中项的启用/禁用状态
  virtual void Duplicate() {}              // 复制并粘贴 (创建副本)
  virtual void SetColorLabel(int) {}       // 设置颜色标签
  virtual void NudgeLeft() {}              // 向左微移
  virtual void NudgeRight() {}             // 向右微移
  virtual void MoveInToPlayhead() {}       // 将入点移动到播放头
  virtual void MoveOutToPlayhead() {}      // 将出点移动到播放头

 signals:  // Qt 信号声明
  /**
   * @brief 当 SetSignalInsteadOfClose(true) 被设置，并且用户尝试关闭面板时发出此信号。
   */
  void CloseRequested();

 protected:
  /**
   * @brief (重写 QWidget::paintEvent) 绘制事件处理函数。
   * 可能用于绘制自定义边框或其他视觉元素。
   * @param event 绘制事件对象。
   */
  void paintEvent(QPaintEvent* event) override;

  /**
   * @brief (重写 QWidget::changeEvent) 状态改变事件处理函数。
   * 用于响应例如语言改变、样式改变等事件。
   * @param e 事件对象。
   */
  void changeEvent(QEvent* e) override;

  /**
   * @brief (重写 QWidget::closeEvent) 关闭事件处理函数。
   * 如果 SetSignalInsteadOfClose(true) 被设置，此函数会发出 CloseRequested() 信号并忽略关闭事件。
   * @param event 关闭事件对象。
   */
  void closeEvent(QCloseEvent* event) override;

  /**
   * @brief (虚函数) 用于重新翻译面板中的UI文本。
   * 当应用程序语言更改时，会调用此方法。派生类应重写此方法以更新其特定的UI元素。
   */
  virtual void Retranslate();

  /**
   * @brief 为面板设置中心控件，并自动添加边距 (padding)。
   * @param widget 要设置为中心内容的 QWidget 指针。
   */
  void SetWidgetWithPadding(QWidget* widget);

 protected slots:  // Qt 受保护的槽函数
  /**
   * @brief 设置面板的标题。
   *
   * 使用此函数设置面板的标题。
   * PanelWidget 的标题默认格式为 "Title: Subtitle" (根据翻译可能有所不同)。
   * 如果未设置副标题，则标题格式仅为 "Title"。
   *
   * @param t 要设置的标题字符串。
   */
  void SetTitle(const QString& t);

  /**
   * @brief 设置面板的副标题。
   *
   * 使用此函数设置面板的副标题。
   * 格式说明同 SetTitle。
   *
   * @param t 要设置的副标题字符串。
   */
  void SetSubtitle(const QString& t);

 private:
  /**
   * @brief 内部函数，在标题或副标题更改时更新 KDDockWidget 的窗口标题。
   *
   * 每当 title_ 或 subtitle_ 发生更改时都应调用此函数。
   */
  void UpdateTitle();

  QString title_;     // 存储面板的主标题
  QString subtitle_;  // 存储面板的副标题

  bool border_visible_;  // 标记高亮边框是否可见

  bool signal_instead_of_close_;  // 标记关闭时是发送信号还是直接关闭
};

}  // namespace olive

#endif  // PANEL_WIDGET_H