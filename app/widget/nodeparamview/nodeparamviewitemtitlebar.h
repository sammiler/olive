#ifndef NODEPARAMVIEWITEMTITLEBAR_H
#define NODEPARAMVIEWITEMTITLEBAR_H

#include <QCheckBox>  // Qt 复选框控件
#include <QLabel>     // Qt 标签控件
#include <QWidget>    // Qt 控件基类

#include "widget/collapsebutton/collapsebutton.h"  // 自定义的可折叠/展开按钮

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QString;
// class QPaintEvent;
// class QMouseEvent;
// class QPushButton; // QPushButton 是 CollapseButton 的基类，且在此文件中直接使用

namespace olive {

/**
 * @brief NodeParamViewItemTitleBar 类是节点参数视图中每个可停靠项的自定义标题栏。
 *
 * 它继承自 QWidget，并包含一个文本标签、一个折叠/展开按钮、一个钉住按钮、
 * 一个添加效果按钮以及一个启用/禁用复选框。
 * 此标题栏负责显示项的名称，并提供控制项状态（如展开、钉住、启用）的用户界面。
 */
class NodeParamViewItemTitleBar : public QWidget {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit NodeParamViewItemTitleBar(QWidget *parent = nullptr);

  /**
   * @brief 检查标题栏关联的项当前是否处于展开状态。
   *
   * 状态由内部的折叠按钮 (collapse_btn_) 的选中状态决定。
   * @return 如果已展开，则返回 true；否则返回 false。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool IsExpanded() const { return collapse_btn_->isChecked(); }

 public slots:
  /**
   * @brief 设置标题栏关联项的展开状态。
   *
   * 这会更新内部折叠按钮的选中状态。
   * @param e 如果为 true，则设置为展开状态；否则为折叠状态。
   */
  void SetExpanded(bool e);

  /**
   * @brief 设置标题栏显示的文本。
   *
   * 同时也会设置标签的工具提示 (tooltip) 为相同的文本。
   * @param s 要显示的文本字符串。
   */
  void SetText(const QString &s) {
    lbl_->setText(s);          // 设置标签文本
    lbl_->setToolTip(s);       // 设置工具提示
    lbl_->setMinimumWidth(1);  // 确保标签有最小宽度以避免完全消失
  }

  /**
   * @brief 设置钉住按钮的可见性。
   * @param e 如果为 true，则显示钉住按钮；否则隐藏。
   */
  void SetPinButtonVisible(bool e) { pin_btn_->setVisible(e); }

  /**
   * @brief 设置添加效果按钮的可见性。
   * @param e 如果为 true，则显示添加效果按钮；否则隐藏。
   */
  void SetAddEffectButtonVisible(bool e) { add_fx_btn_->setVisible(e); }

  /**
   * @brief 设置启用/禁用复选框的可见性。
   * @param e 如果为 true，则显示复选框；否则隐藏。
   */
  void SetEnabledCheckBoxVisible(bool e) { enabled_checkbox_->setVisible(e); }

  /**
   * @brief 设置启用/禁用复选框的勾选状态。
   * @param e 如果为 true，则勾选复选框；否则取消勾选。
   */
  void SetEnabledCheckBoxChecked(bool e) { enabled_checkbox_->setChecked(e); }

 signals:
  /**
   * @brief 当展开/折叠状态（通过 collapse_btn_）发生改变时发出此信号。
   * @param e 新的展开状态 (true 表示展开，false 表示折叠)。
   */
  void ExpandedStateChanged(bool e);

  /**
   * @brief 当钉住按钮的勾选状态发生改变时发出此信号。
   * @param e 新的钉住状态 (true 表示已钉住)。
   */
  void PinToggled(bool e);

  /**
   * @brief 当添加效果按钮被点击时发出此信号。
   */
  void AddEffectButtonClicked();

  /**
   * @brief 当启用/禁用复选框的状态（勾选/未勾选）发生改变时发出此信号。
   * @param e 复选框的新状态 (true 表示已勾选/启用)。
   */
  void EnabledCheckBoxClicked(bool e);

  /**
   * @brief 当标题栏本身（非特定按钮）被点击时发出此信号。
   */
  void Clicked();

 protected:
  /**
   * @brief 重写 QWidget 的绘制事件处理函数。
   *
   * 可能用于绘制自定义的背景、边框或分隔线。
   * @param event 绘制事件指针。
   */
  void paintEvent(QPaintEvent *event) override;

  /**
   * @brief 重写 QWidget 的鼠标按下事件处理函数。
   *
   * 当用户在标题栏上按下鼠标时，发出 Clicked() 信号。
   * @param event 鼠标事件指针。
   */
  void mousePressEvent(QMouseEvent *event) override;
  /**
   * @brief 重写 QWidget 的鼠标双击事件处理函数。
   *
   * 双击标题栏通常会切换其关联项的展开/折叠状态。
   * @param event 鼠标事件指针。
   */
  void mouseDoubleClickEvent(QMouseEvent *event) override;

 private:
  bool draw_border_;  ///< 标记是否需要绘制边框。

  QLabel *lbl_;  ///< 用于显示标题文本的标签。

  CollapseButton *collapse_btn_;  ///< 用于控制展开/折叠的自定义按钮。

  QPushButton *pin_btn_;     ///< 用于“钉住”参数项的按钮。
  QPushButton *add_fx_btn_;  ///< 用于添加效果的按钮。

  QCheckBox *enabled_checkbox_;  ///< 用于启用/禁用关联项的复选框。
};

}  // namespace olive

#endif  // NODEPARAMVIEWITEMTITLEBAR_H
