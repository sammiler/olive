#ifndef STREAMPROPERTIES_H
#define STREAMPROPERTIES_H

#include <QWidget> // QWidget 基类

// Olive 内部头文件
#include "common/define.h"    // 可能包含一些通用定义
// 假设 undocommand.h 声明了 MultiUndoCommand 类
#include "undo/undocommand.h" // 包含 MultiUndoCommand，用于 Accept 方法

// 前向声明 (如果需要)
// namespace olive {
// class MultiUndoCommand;
// }

namespace olive {

/**
 * @brief 用于显示和编辑媒体流（例如视频或音频流）属性的UI组件的抽象基类。
 *
 *此类继承自 QWidget，为特定类型的流属性编辑器 (例如 AudioStreamProperties,
 * VideoStreamProperties) 提供了一个通用接口。派生类通常会包含用于显示
 * 和修改特定流属性（如分辨率、帧率、采样率、编解码器信息等）的UI控件。
 *
 * 它定义了 `Accept()` 方法来应用更改，以及 `SanityCheck()` 方法来验证用户输入。
 */
class StreamProperties : public QWidget {
  // Q_OBJECT // 如果此类或其派生类需要信号槽机制，则应添加此宏
public:
  /**
   * @brief 构造一个新的 StreamProperties 对象。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit StreamProperties(QWidget* parent = nullptr);

  /**
   * @brief 虚析构函数。
   * 确保派生类对象能够被正确销毁。
   */
  ~StreamProperties() override = default;

  /**
   * @brief 应用或接受对流属性所做的更改。
   *
   * 这是一个虚函数，旨在由派生类覆盖。
   * 派生类应在此方法中实现将其UI控件中的当前设置值保存到
   * 相应的数据模型（例如 Footage 对象中的流信息）。
   * 如果更改支持撤销/重做，则应创建并添加相应的 `UndoCommand`
   * 到传入的 `parent_cmd` (MultiUndoCommand) 中。
   *
   * @param parent_cmd 指向 MultiUndoCommand 对象的指针，用于聚合此属性更改产生的撤销命令。
   * 如果更改不需要撤销/重做，或者由外部统一处理，则此参数可能未使用。
   * 基类中的默认实现为空。
   */
  virtual void Accept(MultiUndoCommand* parent_cmd) { Q_UNUSED(parent_cmd); }

  /**
   * @brief 对当前UI控件中的用户输入或设置进行有效性检查。
   *
   * 这是一个虚函数，旨在由派生类覆盖以实现特定的验证逻辑。
   * 例如，检查数值是否在允许范围内、字符串格式是否正确等。
   * 此方法通常在用户尝试应用或保存设置（例如点击“确定”按钮）之前，
   * 在 `Accept()` 方法内部或之前被调用。
   *
   * @return bool 如果所有设置都通过了有效性检查，则返回 true；否则返回 false。
   * 基类中的默认实现返回 true，表示默认情况下所有设置都是有效的。
   */
  virtual bool SanityCheck() { return true; }
};

}  // namespace olive

#endif  // STREAMPROPERTIES_H