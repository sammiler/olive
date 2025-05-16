#ifndef MEDIAPROPERTIESDIALOG_H  // 宏名通常与文件名一致
#define MEDIAPROPERTIESDIALOG_H

#include <QCheckBox>  // 复选框控件 (虽然未直接在此头文件中作为成员，但可能在 .cpp 中使用或通过 StreamProperties 间接使用)
#include <QComboBox>       // 下拉选择框控件 (同上)
#include <QDialog>         // QDialog 基类
#include <QDoubleSpinBox>  // 双精度浮点数输入框
#include <QLineEdit>       // 单行文本输入框
#include <QListWidget>     // 列表控件
#include <QStackedWidget>  // 堆叠控件
#include <QString>         // Qt 字符串类
#include <QWidget>         // 为了 QWidget* parent 参数

// Olive 内部头文件
// 假设 footage.h 声明了 Footage 类和 Project 类 (或者 Project 在其他地方声明)
#include "node/project/footage/footage.h"
// 假设 undocommand.h 声明了 UndoCommand 基类和 MultiUndoCommand (后者在此处未使用，但 StreamEnableChangeCommand 继承自
// UndoCommand)
#include "undo/undocommand.h"
// 假设 track.h (或其包含文件) 声明了 Track::Type 枚举
#include "node/output/track/track.h"  // 为了 Track::Type
// #include "common/define.h" // 如果需要 common/define.h 中的内容

// 前向声明 (如果 Project 类定义在别处且未通过 footage.h 包含)
// namespace olive { class Project; }

namespace olive {

/**
 * @brief 用于设置媒体素材 (Footage) 属性的对话框类。
 *
 * 此对话框允许用户修改特定 `Footage` 对象的属性，例如其名称、
 * 内部各个流（视频、音频）的启用状态，以及可能的转码帧率等。
 * 更改可以通过撤销/重做命令进行管理。
 * 只要提供了有效的 `Footage` 对象，就可以从应用程序的任何部分加载此对话框。
 */
class FootagePropertiesDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief FootagePropertiesDialog 构造函数。
   *
   * @param parent 父 QWidget 对象指针。通常是 MainWindow 或项目面板。
   * @param footage 指向要设置其属性的 Footage 对象的指针。此指针在对话框的生命周期内必须保持有效。
   */
  FootagePropertiesDialog(QWidget* parent, Footage* footage);

  // ~FootagePropertiesDialog() override; // 默认析构函数通常足够

 private:
  /**
   * @brief 用于更改媒体流启用/禁用状态的撤销命令类。
   *
   * 此命令记录了对 Footage 对象中特定类型（视频/音频）和索引的流
   * 的启用状态的更改，并支持撤销和重做该更改。
   */
  class StreamEnableChangeCommand : public UndoCommand {
   public:
    /**
     * @brief 构造一个新的 StreamEnableChangeCommand 对象。
     * @param footage 指向要修改其流状态的 Footage 对象。
     * @param type 要修改的流的类型 (例如 Track::Type::kVideo, Track::Type::kAudio)。
     * @param index_in_type 流在其类型中的索引 (例如，第一个音频流，第二个视频流)。
     * @param enabled 流的新启用状态 (true 表示启用，false 表示禁用)。
     */
    StreamEnableChangeCommand(Footage* footage, Track::Type type, int index_in_type, bool enabled);

    /**
     * @brief 获取与此撤销命令相关的 Project 对象。
     * 撤销/重做操作可能需要访问项目级别的上下文。
     * @return Project* 指向相关 Project 对象的指针。
     */
    [[nodiscard]] Project* GetRelevantProject() const override;

   protected:
    /**
     * @brief 执行或重做更改流启用状态的操作。
     * 将流的启用状态设置为 `new_enabled_`。
     */
    void redo() override;
    /**
     * @brief 撤销更改流启用状态的操作。
     * 将流的启用状态恢复为 `old_enabled_`。
     */
    void undo() override;

   private:
    /** @brief 指向关联的 Footage 对象。 */
    Footage* footage_;
    /** @brief 要修改的流的类型。 */
    Track::Type type_;
    /** @brief 流在其类型中的索引。 */
    int index_;

    /** @brief 流更改前的原始启用状态 (使用花括号进行值初始化)。 */
    bool old_enabled_{};
    /** @brief 流的新启用状态。 */
    bool new_enabled_;
  };

  /**
   * @brief 指向 QStackedWidget 的指针，用于根据所选流的类型（视频或音频）显示不同的属性编辑界面。
   * 例如，视频流属性和音频流属性的编辑界面可能不同。
   */
  QStackedWidget* stacked_widget_;

  /**
   * @brief 用于编辑媒体素材名称的单行文本输入框。
   */
  QLineEdit* footage_name_field_;

  /**
   * @brief 指向在构造函数中设置的媒体素材 (Footage) 对象的内部指针。
   * 对话框通过此指针访问和修改素材的属性。
   */
  Footage* footage_;

  /**
   * @brief 用于列出媒体素材中包含的轨道/流 (例如视频流0, 音频流0, 音频流1) 的列表控件。
   * 用户可以从此列表中选择一个流来查看和编辑其特定属性。
   */
  QListWidget* track_list;  // 变量名通常用下划线后缀，如 track_list_

  /**
   * @brief 用于设置素材（尤其是图像序列）的适配帧率的 QDoubleSpinBox。
   * 使用花括号进行值初始化，确保在构造时为 nullptr 或默认状态，如果它是指针的话。
   * (根据类型 QDoubleSpinBox* conform_fr{}; 这应该是一个指针成员)
   */
  QDoubleSpinBox* conform_fr{};

 private slots:
  /**
   * @brief 重写 QDialog::accept() 槽函数，用于在用户确认对话框时保存对素材属性的更改。
   *
   * 当用户点击“确定”或等效按钮时，此槽函数会被调用。
   * 它会收集UI控件中的所有更改（例如素材名称、流的启用状态、适配帧率等），
   * 并通过创建和执行相应的撤销命令 (如 StreamEnableChangeCommand) 来应用这些更改。
   */
  void accept() override;
};

}  // namespace olive

#endif  // MEDIAPROPERTIESDIALOG_H