#ifndef NODEVIEWTOOLBAR_H
#define NODEVIEWTOOLBAR_H

#include <QPushButton> // Qt 按钮控件基类
#include <QWidget>     // Qt 控件基类

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QEvent;

namespace olive {

/**
 * @brief NodeViewToolBar 类是为节点视图 (NodeView) 提供的一个工具栏控件。
 *
 * 它继承自 QWidget，并包含一些常用操作的按钮，
 * 例如添加新节点到图中，以及切换小地图的显示/隐藏状态。
 */
class NodeViewToolBar : public QWidget {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit NodeViewToolBar(QWidget *parent = nullptr);

public slots:
 /**
  * @brief 设置小地图按钮的勾选状态。
  *
  * 当外部逻辑（例如 NodeView 本身）改变小地图的可见性时，
  * 可以调用此槽来同步工具栏上按钮的状态。
  * @param e 如果为 true，则小地图按钮应显示为已勾选（表示小地图已启用）；否则为未勾选。
  */
 void SetMiniMapEnabled(bool e) { minimap_btn_->setChecked(e); }

  signals:
   /**
    * @brief 当“添加节点”按钮被点击时发出此信号。
    */
   void AddNodeClicked();

  /**
   * @brief 当小地图启用/禁用按钮的状态被切换时发出此信号。
   * @param e 按钮的新勾选状态 (true 表示小地图已启用)。
   */
  void MiniMapEnabledToggled(bool e);

protected:
  /**
   * @brief 处理 Qt 的 changeEvent 事件。
   *
   * 主要用于响应 QEvent::LanguageChange 和 QEvent::ThemeChange (如果适用) 事件，
   * 以便在应用程序语言或主题设置更改时更新工具栏按钮的文本或图标。
   * @param e 指向 QEvent 对象的指针。
   */
  void changeEvent(QEvent *e) override;

private:
  /**
   * @brief 重新翻译工具栏中所有按钮的文本和工具提示。
   *
   * 此方法会在构造时以及语言更改时被调用。
   */
  void Retranslate();

  /**
   * @brief 更新工具栏中所有按钮的图标。
   *
   * 此方法会在构造时以及主题或图标集更改时被调用。
   */
  void UpdateIcons();

  QPushButton *add_node_btn_; ///< “添加节点”按钮。
  QPushButton *minimap_btn_;  ///< 用于切换小地图显示/隐藏状态的按钮（通常是可勾选的）。
};

}  // namespace olive

#endif  // NODEVIEWTOOLBAR_H
