#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QCheckBox>  // 复选框控件
#include <QDialog>    // QDialog 基类
#include <QWidget>    // 为了 QWidget* parent 参数

#include "common/define.h"  // 可能包含一些通用定义

namespace olive {

/**
 * @brief 显示应用程序“关于”信息的对话框类。
 *
 * “关于”对话框通常通过“帮助 > 关于”菜单项访问，用于显示软件的
 * 版本号、版权信息、许可证详情以及贡献者名单等。
 * 此对话框也可以配置为在程序启动时作为欢迎对话框显示。
 */
class AboutDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief AboutDialog 构造函数。
   *
   * 创建并初始化“关于”对话框。
   *
   * @param welcome_dialog 如果为 true，则对话框将作为欢迎对话框（可能包含“不再显示”选项）进行配置和显示；
   * 如果为 false，则作为标准的“关于”对话框。
   * @param parent 父 QWidget 对象指针。通常，当从主菜单调用时，这会是 MainWindow。默认为 nullptr。
   */
  explicit AboutDialog(bool welcome_dialog, QWidget *parent = nullptr);

  // 默认析构函数通常足够，因为 dont_show_again_checkbox_ 如果有父对象会被自动清理。
  // ~AboutDialog() override;

 public slots:
  /**
   * @brief 重写 QDialog::accept() 槽函数。
   * 当对话框被接受时（例如用户点击“确定”按钮）调用。
   * 如果 `dont_show_again_checkbox_` 被选中，可能会将此偏好设置保存到配置中。
   */
  void accept() override;

 private:
  /**
   * @brief 指向“不再显示此消息”复选框的指针。
   * 仅当对话框作为欢迎对话框 (welcome_dialog 为 true) 时，此复选框才可能被创建和使用。
   */
  QCheckBox *dont_show_again_checkbox_;
};

}  // namespace olive

#endif  // ABOUTDIALOG_H