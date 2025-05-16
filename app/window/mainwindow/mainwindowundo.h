#ifndef MAINWINDOWUNDO_H
#define MAINWINDOWUNDO_H

#include "node/project/sequence/sequence.h"  // 引入 Sequence 类的定义，这些命令与序列的打开/关闭相关
#include "undo/undocommand.h"                // 引入 UndoCommand 基类的定义

namespace olive {

// 前向声明 Sequence 和 Project 类，如果它们的完整定义已在上述头文件中，则可能非必需
// class Sequence;
// class Project;

/**
 * @brief 打开一个序列（Sequence）的撤销命令。
 *
 * 此命令用于封装在主窗口中打开一个序列的操作，使其可以被撤销和重做。
 */
class OpenSequenceCommand : public UndoCommand {
 public:
  /**
   * @brief 显式构造函数。
   * @param sequence 指向要打开的 Sequence 对象的指针。
   */
  explicit OpenSequenceCommand(Sequence* sequence) : sequence_(sequence) {}

  /**
   * @brief 获取与此命令相关的项目对象。
   *
   * 在此实现中返回 nullptr，表明此命令可能不直接修改项目数据，
   * 而是影响UI状态（例如哪个序列在时间轴面板中打开）。
   * 或者，相关的项目上下文可能由调用者或主窗口在更高层次管理。
   * @return 总是返回 nullptr。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return nullptr; }

 protected:
  /**
   * @brief 执行打开序列的操作。
   *
   * 此方法通常会调用主窗口的相应函数来在UI中打开并显示指定的序列。
   */
  void redo() override;

  /**
   * @brief 撤销打开序列的操作。
   *
   * 此方法通常会调用主窗口的相应函数来在UI中关闭之前打开的序列。
   */
  void undo() override;

 private:
  Sequence* sequence_;  ///< 指向要打开或已打开的序列对象的指针。
};

/**
 * @brief 关闭一个序列（Sequence）的撤销命令。
 *
 * 此命令用于封装在主窗口中关闭一个序列的操作，使其可以被撤销和重做。
 */
class CloseSequenceCommand : public UndoCommand {
 public:
  /**
   * @brief 显式构造函数。
   * @param sequence 指向要关闭的 Sequence 对象的指针。
   */
  explicit CloseSequenceCommand(Sequence* sequence) : sequence_(sequence) {}

  /**
   * @brief 获取与此命令相关的项目对象。
   *
   * 与 OpenSequenceCommand 类似，此实现返回 nullptr。
   * 关闭序列主要影响UI状态，而不是直接修改项目核心数据（尽管它可能触发保存等）。
   * @return 总是返回 nullptr。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return nullptr; }

 protected:
  /**
   * @brief 执行关闭序列的操作。
   *
   * 此方法通常会调用主窗口的相应函数来在UI中关闭指定的序列。
   */
  void redo() override;

  /**
   * @brief 撤销关闭序列的操作。
   *
   * 此方法通常会调用主窗口的相应函数来在UI中重新打开之前关闭的序列。
   */
  void undo() override;

 private:
  Sequence* sequence_;  ///< 指向要关闭或已关闭的序列对象的指针。
};

}  // namespace olive

#endif  // MAINWINDOWUNDO_H
