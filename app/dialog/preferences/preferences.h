#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QCheckBox>      // 复选框控件 (虽然未直接在此头文件中作为成员，但可能在ConfigDialogBase或其选项卡中使用)
#include <QDialog>        // QDialog 基类
#include <QListWidget>    // 列表控件 (同上)
#include <QMenuBar>       // 菜单栏 (同上)
#include <QStackedWidget> // 堆叠控件 (同上)
#include <QTabWidget>     // 选项卡控件 (同上)
#include <QWidget>        // 为了 QWidget* parent 参数

// Olive 内部头文件
// 假设 configdialogbase.h 声明了 ConfigDialogBase 基类
#include "dialog/configbase/configdialogbase.h"
// #include "common/define.h" // 如果需要 common/define.h 中的内容

// 前向声明 (如果需要)
// namespace olive { class MainWindow; } // MainWindow 在构造函数参数中用到

namespace olive {

// 前向声明 MainWindow 类，因为构造函数参数中用到了它的指针
class MainWindow;

/**
 * @brief “首选项”对话框类，用于全局应用程序设置。
 *
 * 此对话框继承自 ConfigDialogBase，为用户提供一个集中的界面来配置
 * Olive 编辑器的各种全局设置。它在很大程度上是 Config 单例类的一个用户界面。
 * 对话框通常包含多个选项卡，每个选项卡对应不同类别的设置（例如常规、外观、性能等）。
 */
class PreferencesDialog : public ConfigDialogBase {
  Q_OBJECT

 public:
  /**
   * @brief 构造一个新的 PreferencesDialog 对象。
   * @param main_window 指向主窗口 (MainWindow) 对象的指针。
   * 这可能用于访问应用程序级别的上下文或设置，或者将对话框作为主窗口的子窗口。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   * (注意：构造函数的实现通常会将 `main_window` 或 `main_window` 的某个子widget作为实际的 `parent`)
   */
  explicit PreferencesDialog(MainWindow *main_window); // 将 main_window 添加到参数列表并提供parent
  // ~PreferencesDialog() override; // 默认析构函数通常足够

protected:
  /**
   * @brief 重写 ConfigDialogBase::AcceptEvent() 虚函数。
   *
   * 在所有配置选项卡的 `Save()` 方法被调用之后，但在对话框实际关闭之前，
   * 此方法会被调用。派生类可以覆盖此方法以执行在所有首选项
   * 被接受（保存）后需要进行的特定操作，例如通知应用程序的其他部分
   * 配置已更改，或应用某些需要立即生效的设置。
   */
  void AcceptEvent() override;
};

}  // namespace olive

#endif  // PREFERENCESDIALOG_H