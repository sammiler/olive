#ifndef TOOLBAR_H  // 防止头文件被多次包含的宏定义
#define TOOLBAR_H

#include <QPushButton>  // 引入 QPushButton 类，虽然 ToolbarButton 是其子类，但此处可能用于其他目的或作为 ToolbarButton 依赖的一部分

#include "tool/tool.h"                     // 引入 Tool 枚举 (Tool::Item) 和相关工具定义
#include "widget/flowlayout/flowlayout.h"  // 引入 FlowLayout 类的定义，用于工具栏按钮的流式布局
#include "widget/toolbar/toolbarbutton.h"  // 引入 ToolbarButton 类的定义，自定义的工具栏按钮

namespace olive {  // olive 命名空间开始

class ToolbarButton;  // 前向声明 ToolbarButton 类 (虽然已包含头文件，但如果原始代码中有，则保留)

/**
 * @brief Toolbar 类是一个包含 Olive 所有应用程序级工具按钮的控件。
 *
 * 按钮显示在一个 FlowLayout 中，该布局会根据控件的大小像文本一样调整和换行。
 *
 * 默认情况下，此工具栏不连接到任何东西。建议将槽函数 SetTool() 和信号 ToolChanged()
 * 连接到 Core（分别对应 Core 的信号 ToolChanged() 和槽函数 SetTool()），
 * 这样工具栏就可以在应用程序范围内更新当前工具，并且当工具在其他地方更改时也会自动更新。
 */
class Toolbar : public QWidget {
 Q_OBJECT  // Q_OBJECT 宏，用于启用 Qt 的元对象特性，如信号和槽

     public :
     /**
      * @brief Toolbar 构造函数
      *
      * 创建并连接所有的工具栏按钮。
      *
      * @param parent 父 QWidget 对象。
      */
     explicit Toolbar(QWidget* parent);

 public slots:  // 公共槽函数
  /**
   * @brief 设置要显示为“选中”状态的工具。
   *
   * 此函数不会设置应用程序范围的工具，它仅设置此控件中哪个工具显示为选中状态。
   * 建议仅将此函数用作连接到 Core::ToolChanged() 的槽，以便在应用程序范围的工具更改时自动更新。
   *
   * @param tool 要显示为选中的工具 (Tool::Item)。
   */
  void SetTool(const Tool::Item& tool);

  /**
   * @brief 设置吸附按钮的选中状态值。
   *
   * 与 SetTool() 类似，这不会在应用程序范围内设置任何内容，它仅更改显示的按钮外观。
   * 在这种情况下，是指吸附按钮应显示为启用吸附还是禁用吸附。
   *
   * @param snapping 吸附状态 (bool)。
   */
  void SetSnapping(const bool& snapping);

 protected:  // 受保护的重写方法
  /**
   * @brief Qt changeEvent 事件处理函数。
   *
   * 重写以捕获语言更改事件 (参见 Retranslate())。
   *
   * @param e 指向 QEvent 对象的指针。
   */
  void changeEvent(QEvent* e) override;

  /**
   * @brief Qt resizeEvent 事件处理函数。
   *
   * 重写以在控件大小改变时可能需要调整内部布局（例如 FlowLayout）。
   * @param e 指向 QResizeEvent 对象的指针。
   */
  void resizeEvent(QResizeEvent* e) override;

 signals:  // 信号
  /**
   * @brief 当通过此控件选择一个工具时发出此信号。
   *
   * @param t 被选中的工具 (Tool::Item)。
   */
  void ToolChanged(const Tool::Item& t);

  /**
   * @brief 当吸附设置通过此控件更改时发出此信号。
   *
   * @param b 新的吸附启用设置 (bool)。
   */
  void SnappingChanged(const bool& b);

  /**
   * @brief 当从转场工具菜单中更改所选转场时发出此信号。
   * @param id 被选中的转场的唯一标识符 (QString)。
   */
  void SelectedTransitionChanged(const QString& id);

 private:  // 私有方法
  /**
   * @brief 根据当前选择的语言重置所有字符串。
   */
  void Retranslate();

  /**
   * @brief 在样式更改后更新图标。
   */
  void UpdateIcons();

  /**
   * @brief 用于快速创建工具按钮的内部便捷函数。
   *
   * 此函数将创建一个 ToolbarButton 对象，将图标设置为 `icon` (图标的获取逻辑在函数内部)，
   * 将其工具值设置为 `tool`，将其添加到控件布局中，将其添加到 `toolbar_btns_` 以便迭代按钮
   * (在各种函数中完成)，并将按钮连接到 ToolButtonClicked()。
   *
   * 如果您需要一个非工具但样式相似的按钮，请使用 CreateNonToolButton()。
   *
   * @param tool 要创建按钮对应的工具 (Tool::Item)。
   * @return 创建的 ToolbarButton 指针。按钮的父对象自动设置为 `this`。
   */
  ToolbarButton* CreateToolButton(const Tool::Item& tool);

  /**
   * @brief 用于快速创建按钮的内部便捷函数。
   *
   * 与 CreateToolButton() 类似，但不将按钮添加到 `toolbar_btns_`，也不将按钮连接到
   * ToolButtonClicked()。这用于创建一个样式相似但不代表实际工具的按钮。
   *
   * @return 创建的 ToolbarButton 指针。按钮的父对象自动设置为 `this`。
   */
  ToolbarButton* CreateNonToolButton();

  /**
   * @brief 用于按钮的内部布局。
   */
  FlowLayout* layout_;

  /**
   * @brief 工具栏按钮的数组/列表。
   *
   * 此列表由 CreateToolButton() 自动追加。它用于通过 for 循环等方式快速迭代工具栏按钮。
   * 例如，参见 SetTool() 的用法。
   */
  QList<ToolbarButton*> toolbar_btns_;

  ToolbarButton* btn_pointer_tool_;      ///< 指针工具按钮。
  ToolbarButton* btn_trackselect_tool_;  ///< 轨道选择工具按钮。
  ToolbarButton* btn_edit_tool_;         ///< 编辑工具按钮。
  ToolbarButton* btn_ripple_tool_;       ///< 涟漪编辑工具按钮。
  ToolbarButton* btn_rolling_tool_;      ///< 滚动编辑工具按钮。
  ToolbarButton* btn_razor_tool_;        ///< 剃刀工具按钮。
  ToolbarButton* btn_slip_tool_;         ///< 滑移工具按钮。
  ToolbarButton* btn_slide_tool_;        ///< 滑动工具按钮。
  ToolbarButton* btn_hand_tool_;         ///< 手型工具（抓手工具）按钮。
  ToolbarButton* btn_transition_tool_;   ///< 转场工具按钮。
  ToolbarButton* btn_zoom_tool_;         ///< 缩放工具按钮。
  ToolbarButton* btn_record_;            ///< 录制工具按钮。
  ToolbarButton* btn_add_;               ///< 添加（可能指添加剪辑或效果）工具按钮。

  ToolbarButton* btn_snapping_toggle_;  ///< 吸附功能切换按钮。

 private slots:  // 私有槽函数
  /**
   * @brief ToolbarButton 被点击时的槽函数。
   *
   * 通过 CreateToolButton() 创建的 ToolbarButton 会自动连接到此槽。
   * 此槽将接收 ToolbarButton 的工具值，并发出一个信号，指示工具已更改为新选择的工具。
   * 此函数将发送者 (sender())静态转换为 ToolbarButton，因此不应将任何其他类类型连接到此槽。
   */
  void ToolButtonClicked();

  /**
   * @brief 吸附切换按钮的接收器。
   *
   * 此函数的主要目的是发出 SnappingChanged() 信号，并应连接到 ToolbarButton 的
   * SIGNAL(clicked(bool))。
   *
   * @param b 从发送者的 clicked 信号接收到的新吸附值。
   */
  void SnappingButtonClicked(bool b);

  /**
   * @brief “添加”按钮的接收器。
   *
   * “添加”按钮会弹出一个列表，供用户选择要创建的对象。
   */
  void AddButtonClicked();

  /**
   * @brief “转场”按钮的接收器。
   *
   * “转场”按钮会弹出一个列表，供用户选择要创建的转场类型。
   */
  void TransitionButtonClicked();

  /**
   * @brief 由 TransitionButtonClicked() 创建的菜单的菜单项被触发时的接收器。
   * @param a 指向被触发的 QAction 的指针。
   */
  void TransitionMenuItemTriggered(QAction* a);
};

}  // namespace olive

#endif  // TOOLBAR_H
