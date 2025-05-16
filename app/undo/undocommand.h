#ifndef UNDOCOMMAND_H
#define UNDOCOMMAND_H

#include <QString> // 引入 QString 类，虽然在此文件中未直接使用，但派生类可能会用到
#include <list>    // 引入 std::list，虽然在此文件中未直接使用，但可能是项目中其他部分undo/redo栈的选择之一
#include <vector>  // 引入 std::vector，用于 MultiUndoCommand 存储子命令

#include "common/define.h" // 引入项目内通用的定义文件，例如 DISABLE_COPY_MOVE 宏

namespace olive {

// 前向声明 Project 类，UndoCommand 通常与某个项目关联
class Project;

/**
 * @brief 撤销/重做操作的基类。
 *
 * UndoCommand 类是命令模式的一种实现，用于封装所有可撤销的操作。
 * 每个具体的操作（如添加片段、删除轨道等）都应该继承此类，并实现
 * `redo()` 和 `undo()` 方法。
 * 它还管理命令的准备状态和是否已修改项目的状态。
 */
class UndoCommand {
 public:
  /**
   * @brief 默认构造函数。
   *
   * 初始化命令的状态，例如 prepared_ 和 done_ 为 false。
   */
  UndoCommand();

  /**
   * @brief 虚析构函数。
   *
   * 允许派生类的析构函数被正确调用。使用 = default 表示使用编译器生成的默认析构函数。
   */
  virtual ~UndoCommand() = default;

  // 禁止拷贝和移动构造函数及赋值操作符，确保命令对象的唯一性。
  // 这是通过 "common/define.h" 中定义的 DISABLE_COPY_MOVE 宏来实现的。
  DISABLE_COPY_MOVE(UndoCommand)

  /**
   * @brief 检查命令是否已经准备就绪。
   *
   * “准备就绪”通常意味着命令执行前所需的任何状态或数据已经被捕获。
   * @return 如果命令已准备好，则返回 true；否则返回 false。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool has_prepared() const { return prepared_; }

  /**
   * @brief 设置命令的准备状态。
   *
   * 通常在 prepare() 方法成功执行后调用。
   * @param e 新的准备状态，通常为 true。
   */
  void set_prepared(bool e) { prepared_ = true; } // 注意：参数 e 在这里没有使用，直接设置为 true。这可能是一个小笔误，或者设计如此。

  /**
   * @brief 立即执行（或重做）此命令。
   *
   * 此方法会首先调用 prepare() (如果尚未准备)，然后调用 redo()。
   */
  void redo_now();

  /**
   * @brief 立即撤销此命令。
   *
   * 此方法会首先调用 prepare() (如果尚未准备，尽管对于 undo 来说通常已准备好)，然后调用 undo()。
   */
  void undo_now();

  /**
   * @brief 执行（或重做）此命令，并设置项目的“已修改”状态。
   */
  void redo_and_set_modified();

  /**
   * @brief 撤销此命令，并设置项目的“已修改”状态。
   */
  void undo_and_set_modified();

  /**
   * @brief 纯虚函数，获取与此命令相关的项目对象。
   *
   * 派生类必须实现此方法，以返回该命令作用于哪个 Project 实例。
   * @return 返回指向相关 Project 对象的指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] virtual Project* GetRelevantProject() const = 0;

 protected:
  /**
   * @brief 虚函数，准备命令执行所需的状态。
   *
   * 在命令首次执行 redo() 或 undo() 之前调用。
   * 派生类可以重写此方法以保存撤销操作所需的信息，或执行任何必要的预处理。
   * 默认实现为空。
   */
  virtual void prepare() {}

  /**
   * @brief 纯虚函数，执行命令的实际操作。
   *
   * 派生类必须实现此方法来定义命令的具体行为。
   */
  virtual void redo() = 0;

  /**
   * @brief 纯虚函数，撤销命令的实际操作。
   *
   * 派生类必须实现此方法来定义如何回滚 redo() 所做的更改。
   */
  virtual void undo() = 0;

 private:
  bool modified_{}; ///< 标记此命令执行后是否修改了项目内容。

  Project* project_{}; ///< 指向与此命令相关的项目对象，通常在 GetRelevantProject() 中设置或使用。

  bool prepared_; ///< 标记命令是否已准备好执行（即 prepare() 是否已调用）。

  bool done_; ///< 标记 redo() 操作是否已经被执行过。
};

/**
 * @brief 复合撤销命令，用于将多个 UndoCommand 对象组合成一个单一的命令。
 *
 * MultiUndoCommand 允许将一系列子命令作为一个整体来执行 redo 和 undo 操作。
 * 当 MultiUndoCommand 的 redo() 被调用时，它会按顺序调用所有子命令的 redo()。
 * 当其 undo() 被调用时，它会按相反的顺序调用所有子命令的 undo()。
 */
class MultiUndoCommand : public UndoCommand {
 public:
  /**
   * @brief 默认构造函数。
   */
  MultiUndoCommand() = default;

  /**
   * @brief 获取与此命令相关的项目对象。
   *
   * 对于 MultiUndoCommand，它本身可能不直接关联到特定项目，
   * 而是依赖其子命令。默认实现返回 nullptr，派生类或使用者可能需要
   * 根据子命令来确定实际的项目。
   * @return 默认返回 nullptr。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return nullptr; }

  /**
   * @brief 向此复合命令中添加一个子命令。
   * @param command 指向要添加的 UndoCommand 对象的指针。
   * MultiUndoCommand 会存储这个指针，但通常不拥有其生命周期（除非特殊设计）。
   */
  void add_child(UndoCommand* command) { children_.push_back(command); }

  /**
   * @brief 获取子命令的数量。
   * @return 返回存储的子命令个数。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int child_count() const { return children_.size(); }

  /**
   * @brief 获取指定索引处的子命令。
   * @param i 子命令的索引。
   * @return 返回指向指定索引处 UndoCommand 对象的指针。
   * 如果索引无效，行为未定义（取决于 std::vector::operator[]）。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] UndoCommand* child(int i) const { return children_[i]; }

 protected:
  /**
   * @brief 执行所有子命令的 redo() 操作。
   *
   * 子命令按其被添加的顺序执行。
   */
  void redo() override;

  /**
   * @brief 撤销所有子命令的 undo() 操作。
   *
   * 子命令按其被添加顺序的逆序执行。
   */
  void undo() override;

 private:
  /**
   * @brief 存储子 UndoCommand 指针的向量。
   */
  std::vector<UndoCommand*> children_;
};

}  // namespace olive

#endif  // UNDOCOMMAND_H
