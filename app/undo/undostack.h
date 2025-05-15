

#ifndef UNDOSTACK_H
#define UNDOSTACK_H

#include <QAbstractItemModel>
#include <QAction>

#include "common/define.h"
#include "undo/undocommand.h"

namespace olive {

class UndoStack : public QAbstractItemModel {
  Q_OBJECT
 public:
  UndoStack();

  ~UndoStack() override;

  void push(UndoCommand *command, const QString &name);

  void jump(size_t index);

  void clear();

  [[nodiscard]] bool CanUndo() const;

  [[nodiscard]] bool CanRedo() const { return !undone_commands_.empty(); }

  void UpdateActions();

  QAction *GetUndoAction() { return undo_action_; }

  QAction *GetRedoAction() { return redo_action_; }

  [[nodiscard]] int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;
  [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                    int role = Qt::DisplayRole) const override;
  [[nodiscard]] bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

 signals:
  void indexChanged(int i);

 public slots:
  void undo();

  void redo();

 private:
  static const int kMaxUndoCommands;

  struct CommandEntry {
    UndoCommand *command;
    QString name;
  };

  std::list<CommandEntry> commands_;

  std::list<CommandEntry> undone_commands_;

  QAction *undo_action_;

  QAction *redo_action_;
};

}  // namespace olive

#endif  // UNDOSTACK_H
