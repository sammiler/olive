#ifndef NODEPARAMVIEWITEMBASE_H
#define NODEPARAMVIEWITEMBASE_H

#include <QDockWidget> // Qt 可停靠控件基类

#include "node/node.h"                         // 节点基类 (用于 GetTitleBarTextFromNode)
#include "nodeparamviewitemtitlebar.h"         // 节点参数视图项的自定义标题栏

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWidget;
// class QString;
// class QPaintEvent;
// class QEvent;
// class QMoveEvent;
// class QMouseEvent;

namespace olive {

/**
 * @brief NodeParamViewItemBase 类是节点参数视图中所有可显示项（通常代表一个节点或一个上下文）的基类。
 *
 * 它继承自 QDockWidget，使得每个参数项都可以作为可停靠的窗口。
 * 此类管理一个自定义的标题栏 (NodeParamViewItemTitleBar) 和一个主体区域 (body_) 来显示具体内容。
 * 它还处理高亮、展开/折叠状态以及相关的用户交互和信号。
 */
class NodeParamViewItemBase : public QDockWidget {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit NodeParamViewItemBase(QWidget *parent = nullptr);

  /**
   * @brief 设置此项的高亮状态。
   * @param e 如果为 true，则高亮显示此项；否则取消高亮。
   */
  void SetHighlighted(bool e) {
    highlighted_ = e; // 更新高亮状态标志

    update(); // 请求重绘以反映高亮变化
  }

  /**
   * @brief 检查此项当前是否处于高亮状态。
   * @return 如果已高亮，则返回 true；否则返回 false。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool IsHighlighted() const { return highlighted_; }

  /**
   * @brief 检查此项当前是否处于展开状态（即其主体内容是否可见）。
   * @return 如果已展开，则返回 true；否则返回 false。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool IsExpanded() const;

  /**
   * @brief 静态方法，根据节点信息生成用于标题栏的文本。
   * @param n 指向 Node 对象的指针。
   * @return 返回生成的标题栏文本 QString。
   */
  static QString GetTitleBarTextFromNode(Node *n);

 public slots:
  /**
   * @brief 设置此项的展开状态。
   * @param e 如果为 true，则展开此项以显示其主体内容；否则折叠。
   */
  void SetExpanded(bool e);

  /**
   * @brief 切换此项的展开/折叠状态。
   */
  void ToggleExpanded() { SetExpanded(!IsExpanded()); }

 signals:
  /**
   * @brief 当此项的“钉住”状态（通常由标题栏上的钉住按钮控制）发生改变时发出此信号。
   * @param e 如果为 true，表示已钉住；否则为未钉住。
   */
  void PinToggled(bool e);

  /**
   * @brief 当此项的展开/折叠状态发生改变时发出此信号。
   * @param e 如果为 true，表示已展开；否则为折叠。
   */
  void ExpandedChanged(bool e);

  /**
   * @brief 当此项（作为 QDockWidget）被移动时发出此信号。
   */
  void Moved();

  /**
   * @brief 当此项被点击时（通常是点击标题栏或主体区域）发出此信号。
   */
  void Clicked();

 protected:
  /**
   * @brief 设置此参数项的主体控件。
   *
   * 主体控件是实际显示参数编辑UI的部分，它会被放置在 QDockWidget 的内容区域。
   * @param body 指向要设置为主体的 QWidget 的指针。
   */
  void SetBody(QWidget *body);

  /**
   * @brief 重写 QWidget 的绘制事件处理函数。
   *
   * 可能用于绘制自定义的边框或高亮效果。
   * @param event 绘制事件指针。
   */
  void paintEvent(QPaintEvent *event) override;

  /**
   * @brief 获取此项的自定义标题栏控件。
   * @return 返回指向 NodeParamViewItemTitleBar 对象的 const 指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] NodeParamViewItemTitleBar *title_bar() const { return title_bar_; }

  /**
   * @brief 重写 QWidget 的 changeEvent 事件处理函数。
   *
   * 可能用于响应 QEvent::LanguageChange 事件以更新标题栏文本等。
   * @param e 事件指针。
   */
  void changeEvent(QEvent *e) override;

  /**
   * @brief 重写 QWidget 的 moveEvent 事件处理函数。
   *
   * 当此停靠控件被移动时，发出 Moved() 信号。
   * @param event 移动事件指针。
   */
  void moveEvent(QMoveEvent *event) override;

  /**
   * @brief 重写 QWidget 的 mousePressEvent 事件处理函数。
   *
   * 当用户在此控件上按下鼠标时，发出 Clicked() 信号。
   * @param e 鼠标事件指针。
   */
  void mousePressEvent(QMouseEvent *e) override;

 protected slots:
  /**
   * @brief 虚槽函数，用于重新翻译此项相关的UI文本。
   *
   * 派生类可以重写此方法以实现具体的翻译逻辑。
   * 默认实现为空。
   */
  virtual void Retranslate() {}

 private:
  NodeParamViewItemTitleBar *title_bar_; ///< 指向自定义标题栏控件的指针。

  QWidget *body_{}; ///< 指向作为此项主体内容的 QWidget 的指针。初始化为 nullptr。

  QWidget *hidden_body_; ///< 可能用于在折叠时临时存放 body_ 控件，或者是一个占位符。

  bool highlighted_; ///< 标记此项当前是否处于高亮状态。
};

}  // namespace olive

#endif  // NODEPARAMVIEWITEMBASE_H
