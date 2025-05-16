#ifndef PREFERENCESTAB_H  // 文件名可能是 PREFERENCESTAB_H 或 CONFIGDIALOGBASETAB_H
#define PREFERENCESTAB_H  // 保持与 #ifndef 一致

#include <QWidget>  // QWidget 基类

// 假设 config.h 声明了 Config 类 (虽然在此头文件中未直接使用)
#include "config/config.h"
// 假设 undocommand.h 声明了 MultiUndoCommand 类
#include "undo/undocommand.h"
// #include "common/define.h" // 如果 ConfigDialogBaseTab 使用了 common/define.h 中的内容

namespace olive {

/**
 * @brief 配置对话框中单个选项卡页面的抽象基类。
 *
 *此类作为 `ConfigDialogBase` 中显示的每个具体配置页面的接口。
 * 派生类需要实现 `Accept()` 方法以保存该选项卡上的设置，
 * 并且可以选择性地覆盖 `Validate()` 方法以在应用更改前进行数据验证。
 */
class ConfigDialogBaseTab : public QWidget {
  // Q_OBJECT // 如果此类需要信号槽机制，则应添加此宏
 public:
  /**
   * @brief 默认构造函数。
   * 创建一个 ConfigDialogBaseTab 实例。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit ConfigDialogBaseTab(QWidget* parent = nullptr) : QWidget(parent) {}  // 添加了显式构造函数定义

  // 如果基类 QWidget 的析构函数不是虚的，或者 ConfigDialogBaseTab 需要管理特定资源，
  // 则应提供一个虚析构函数。对于 QWidget 派生类，通常其析构函数已是虚的。
  /**
   * @brief 虚析构函数。
   * 确保派生类对象能够被正确销毁。
   */
  ~ConfigDialogBaseTab() override = default;  // 使用 C++11 的 override 和 default

  /**
   * @brief 验证当前选项卡上的用户输入或设置是否有效。
   *
   * 派生类可以覆盖此方法以实现特定的验证逻辑。
   * 例如，检查数值范围、字符串格式等。
   * 此方法通常在用户尝试应用或保存设置（例如点击“确定”按钮）之前被调用。
   *
   * @return bool 如果所有设置都有效，则返回 true；否则返回 false。
   * 默认实现返回 true，表示默认情况下所有设置都是有效的。
   */
  virtual bool Validate();

  /**
   * @brief 应用（保存）当前选项卡上的配置更改。
   *
   * 这是一个纯虚函数，必须由所有派生类实现。
   * 实现此方法时，应将当前选项卡UI控件中的设置值保存到
   * 应用程序的配置系统中 (通常是 `Config::Current()`)。
   * 如果更改可以被撤销/重做，则应创建并添加相应的 `UndoCommand`
   *到传入的 `parent` (MultiUndoCommand) 中。
   *
   * @param parent 指向 MultiUndoCommand 对象的指针，用于聚合此选项卡产生的撤销命令。
   * 如果此选项卡的更改不需要撤销/重做，或者由外部统一处理，则此参数可能未使用。
   */
  virtual void Accept(MultiUndoCommand* parent) = 0;
};

}  // namespace olive

#endif  // PREFERENCESTAB_H