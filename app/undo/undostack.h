#ifndef UNDOSTACK_H
#define UNDOSTACK_H

#include <QAbstractItemModel>  // 引入 QAbstractItemModel 基类，用于将撤销栈暴露给 Qt 的模型/视图框架
#include <QAction>             // 引入 QAction 类，用于创建撤销/重做操作的菜单项或工具栏按钮
#include <QString>             // 引入 QString，用于命令名称
#include <list>                // 引入 std::list，用作存储命令的容器
#include <vector>              // 引入 std::vector (虽然在此文件中未直接使用，但 undo/undocommand.h 中可能使用)

#include "common/define.h"     // 引入项目内通用的定义文件
#include "undo/undocommand.h"  // 引入 UndoCommand 基类的定义

namespace olive {

/**
 * @brief 管理撤销和重做操作的栈结构。
 *
 * UndoStack 类不仅存储已执行的命令以供撤销，还存储已撤销的命令以供重做。
 * 它继承自 QAbstractItemModel，使其能够被 Qt 的视图组件（如 QListView）显示，
 * 从而用户可以看到撤销历史记录。
 *此类还管理与撤销/重做操作关联的 QAction 对象。
 */
class UndoStack : public QAbstractItemModel {
  Q_OBJECT
 public:
  /**
   * @brief 构造函数。
   *
   * 初始化撤销栈，并创建关联的撤销和重做 QAction。
   */
  UndoStack();

  /**
   * @brief 析构函数。
   *
   * 清理所有存储的命令对象和 QAction 对象。
   */
  ~UndoStack() override;

  /**
   * @brief 将一个新命令推入撤销栈。
   *
   * 当一个可撤销的操作执行后，对应的命令对象会被添加到此栈中。
   * 添加新命令时，所有之前已撤销的命令（重做栈）会被清空。
   * @param command 指向要添加的 UndoCommand 对象的指针。UndoStack 将拥有此命令的所有权。
   * @param name 命令的可读名称，用于在UI中显示（例如，在历史记录列表中）。
   */
  void push(UndoCommand *command, const QString &name);

  /**
   * @brief 跳转到撤销历史中的特定索引位置。
   *
   * 这允许用户直接选择历史记录中的某个点，并撤销或重做至该状态。
   * @param index 要跳转到的命令在历史记录中的索引。
   */
  void jump(size_t index);

  /**
   * @brief 清空整个撤销和重做历史记录。
   *
   * 所有存储的命令都将被删除。
   */
  void clear();

  /**
   * @brief 检查当前是否可以执行撤销操作。
   * @return 如果撤销栈中有命令（即 `commands_` 列表不为空），则返回 true；否则返回 false。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool CanUndo() const;

  /**
   * @brief 检查当前是否可以执行重做操作。
   * @return 如果重做栈中有命令（即 `undone_commands_` 列表不为空），则返回 true；否则返回 false。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool CanRedo() const { return !undone_commands_.empty(); }

  /**
   * @brief 更新撤销和重做 QAction 的启用状态。
   *
   * 通常在撤销栈状态改变后调用（例如，push, undo, redo 操作之后）。
   */
  void UpdateActions();

  /**
   * @brief 获取撤销操作的 QAction 对象。
   * @return 返回指向撤销 QAction 的指针。
   */
  QAction *GetUndoAction() { return undo_action_; }

  /**
   * @brief 获取重做操作的 QAction 对象。
   * @return 返回指向重做 QAction 的指针。
   */
  QAction *GetRedoAction() { return redo_action_; }

  // QAbstractItemModel 接口的实现，用于将撤销栈作为模型展示给视图
  /**
   * @brief 返回模型中的列数。对于简单的列表模型，通常是1。
   * @param parent 父项的 QModelIndex。对于顶层项，通常是无效的 QModelIndex()。
   * @return 返回列数。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  /**
   * @brief 返回指定索引和角色的数据。
   * @param index 要获取数据的 QModelIndex。
   * @param role 请求的数据角色 (例如 Qt::DisplayRole 用于文本，Qt::DecorationRole 用于图标)。
   * @return 返回一个 QVariant，包含请求的数据。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  /**
   * @brief 返回给定行、列和父项的子项的 QModelIndex。
   * @param row 子项的行号。
   * @param column 子项的列号。
   * @param parent 父项的 QModelIndex。
   * @return 返回子项的 QModelIndex。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

  /**
   * @brief 返回给定索引的父项的 QModelIndex。
   * @param index 子项的 QModelIndex。
   * @return 返回父项的 QModelIndex；对于顶层项，返回无效的 QModelIndex()。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;

  /**
   * @brief 返回给定父项下的行数（子项数量）。
   * @param parent 父项的 QModelIndex。
   * @return 返回行数。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  /**
   * @brief 返回指定区段、方向和角色的表头数据。
   * @param section 区段索引（列号或行号，取决于方向）。
   * @param orientation 表头的方向 (Qt::Horizontal 或 Qt::Vertical)。
   * @param role 请求的数据角色。
   * @return 返回一个 QVariant，包含表头数据。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                    int role = Qt::DisplayRole) const override;

  /**
   * @brief 检查给定的父项是否有子项。
   * @param parent 要检查的父项的 QModelIndex。
   * @return 如果父项有子项，则返回 true；否则返回 false。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

 signals:
  /**
   * @brief 当撤销栈的当前索引（即下一个将要撤销或重做的命令的位置）发生改变时发出的信号。
   * @param i 新的当前索引。
   */
  void indexChanged(int i);

 public slots:
  /**
   * @brief 执行撤销操作。
   *
   * 如果 CanUndo() 返回 true，则从 `commands_` 列表中取出最后一个命令，
   * 执行其 `undo()` 方法，然后将其移动到 `undone_commands_` 列表中。
   */
  void undo();

  /**
   * @brief 执行重做操作。
   *
   * 如果 CanRedo() 返回 true，则从 `undone_commands_` 列表中取出最后一个命令，
   * 执行其 `redo()` 方法，然后将其移回到 `commands_` 列表中。
   */
  void redo();

 private:
  /**
   * @brief 撤销栈中允许存储的最大命令数量。
   *
   * 可能用于限制内存使用。
   */
  static const int kMaxUndoCommands;

  /**
   * @brief 内部结构体，用于存储命令及其用户可读的名称。
   */
  struct CommandEntry {
    UndoCommand *command;  ///< 指向 UndoCommand 对象的指针。
    QString name;          ///< 命令的名称，用于在UI中显示。
  };

  /**
   * @brief 存储已执行且可以被撤销的命令的列表。
   *
   * 最近执行的命令在列表的末尾。
   */
  std::list<CommandEntry> commands_;

  /**
   * @brief 存储已被撤销且可以被重做的命令的列表。
   *
   * 最近撤销的命令在列表的末尾。
   */
  std::list<CommandEntry> undone_commands_;

  /**
   * @brief 指向与撤销操作关联的 QAction 对象的指针。
   */
  QAction *undo_action_;

  /**
   * @brief 指向与重做操作关联的 QAction 对象的指针。
   */
  QAction *redo_action_;
};

}  // namespace olive

#endif  // UNDOSTACK_H
