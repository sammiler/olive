#ifndef SEQUENCEDIALOG_H
#define SEQUENCEDIALOG_H

#include <QComboBox>  // 引入 QComboBox 类，用于创建下拉列表框控件
#include <QDialog>    // 引入 QDialog 类，对话框窗口的基类
#include <QSpinBox>   // 引入 QSpinBox 类，用于创建整数输入框控件

// 前向声明，避免不必要的头文件包含
QT_BEGIN_NAMESPACE
class QLineEdit;   // Qt的单行文本输入框
class QTabWidget;  // Qt的选项卡控件
QT_END_NAMESPACE

#include "node/project/sequence/sequence.h"  // 引入序列类定义
#include "sequencedialogparametertab.h"      // 引入序列对话框参数选项卡
#include "sequencedialogpresettab.h"         // 引入序列对话框预设选项卡
#include "undo/undocommand.h"                // 引入撤销命令基类

namespace olive {

/**
 * @class SequenceDialog
 * @brief 用于编辑序列 (Sequence) 参数的对话框。
 *
 * 此对话框向用户公开序列的所有参数，允许他们根据需要设置序列。
 * 可以通过构造函数将一个序列传递给此对话框。所有字段将使用该序列的参数填充，
 * 允许用户查看和编辑它们。接受对话框会将更改应用回该序列，
 * 可以直接应用或使用 UndoCommand (参见 SetUndoable())。
 *
 * 如果要创建一个新序列，仍必须先构造该序列，然后再将其发送到 SequenceDialog。
 * SequenceDialog 不会创建任何新对象。在大多数创建新序列的情况下，
 * 使用 SequenceDialog 编辑其参数的操作将与将序列添加到项目的操作配对。
 * 在这种情况下，由于后者将是主要的不可撤销操作，因此参数编辑不必是可撤销的，
 * 因为对用户而言，它们将被视为单个操作 (参见 SetUndoable())。
 */
class SequenceDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @enum Type
   * @brief 用于设置对话框操作模式 (参见 SequenceDialog() 构造函数)。
   */
  enum Type {
    kNew,      ///< 用于创建新序列的模式。
    kExisting  ///< 用于编辑现有序列的模式。
  };

  /**
   * @brief SequenceDialog 构造函数。
   *
   * @param s 要编辑的序列 (Sequence) 对象指针。
   * @param t 操作模式 (Type)，会更改某些UI元素，如窗口标题，以最好地表示正在执行的操作。默认为 kExisting。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit SequenceDialog(Sequence* s, Type t = kExisting, QWidget* parent = nullptr);

  /**
   * @brief 设置参数更改是否应创建为撤销命令。
   *
   * 默认为 true。
   * @param u 如果为 true，则更改将是可撤销的；否则为 false。
   */
  void SetUndoable(bool u);

  /**
   * @brief 设置此序列的名称是否可以通过此对话框进行编辑。
   *
   * 默认为 true。
   * @param e 如果为 true，则名称可编辑；否则为 false。
   */
  void SetNameIsEditable(bool e);

 public slots:
  /**
   * @brief 当用户按下“确定”按钮时调用的槽函数。
   *
   * 此函数会验证输入，并将更改应用到序列对象。
   */
  void accept() override;

 private:
  /**
   * @brief 指向正在编辑的序列对象的指针。
   */
  Sequence* sequence_;

  /**
   * @brief 指向序列预设选项卡的指针。
   */
  SequenceDialogPresetTab* preset_tab_;

  /**
   * @brief 指向序列参数选项卡的指针。
   */
  SequenceDialogParameterTab* parameter_tab_;

  /**
   * @brief 标记参数更改是否应创建撤销命令的布尔值。
   */
  bool make_undoable_;

  /**
   * @brief 指向序列名称编辑字段的指针。
   */
  QLineEdit* name_field_;

  /**
   * @class SequenceParamCommand
   * @brief 用于设置序列参数的撤销命令类。
   *
   * 这是一个内部类，封装了更改序列参数的操作，以便可以撤销和重做。
   */
  class SequenceParamCommand : public UndoCommand {
   public:
    /**
     * @brief SequenceParamCommand 构造函数。
     *
     * @param s 指向要修改参数的 Sequence 对象。
     * @param video_params 新的视频参数。
     * @param audio_params 新的音频参数。
     * @param name 新的序列名称。
     * @param autocache 新的自动缓存设置。
     */
    SequenceParamCommand(Sequence* s, VideoParams video_params, AudioParams audio_params, QString name, bool autocache);

    /**
     * @brief 获取与此命令相关的项目。
     * @return [[nodiscard]] 指向相关 Project 对象的指针。
     * [[nodiscard]] 属性提示编译器调用者应该使用此函数的返回值。
     */
    [[nodiscard]] Project* GetRelevantProject() const override;

   protected:
    /**
     * @brief 执行命令（应用新参数）。
     */
    void redo() override;
    /**
     * @brief 撤销命令（恢复旧参数）。
     */
    void undo() override;

   private:
    /**
     * @brief 指向目标序列对象的指针。
     */
    Sequence* sequence_;

    /**
     * @brief 存储新的视频参数。
     */
    VideoParams new_video_params_;
    /**
     * @brief 存储新的音频参数。
     */
    AudioParams new_audio_params_;
    /**
     * @brief 存储新的序列名称。
     */
    QString new_name_;
    /**
     * @brief 存储新的自动缓存设置。
     */
    bool new_autocache_;

    /**
     * @brief 存储旧的视频参数（用于撤销）。
     */
    VideoParams old_video_params_;
    /**
     * @brief 存储旧的音频参数（用于撤销）。
     */
    AudioParams old_audio_params_;
    /**
     * @brief 存储旧的序列名称（用于撤销）。
     */
    QString old_name_;
    /**
     * @brief 存储旧的自动缓存设置（用于撤销）。
     */
    bool old_autocache_;
  };

 private slots:
  /**
   * @brief 私有槽函数：“设为默认值”按钮点击时调用。
   *
   * 用于将当前对话框中的参数设置为新序列的默认参数。
   */
  void SetAsDefaultClicked();
};

}  // namespace olive

#endif  // SEQUENCEDIALOG_H
