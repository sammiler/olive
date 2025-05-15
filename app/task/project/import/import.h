

#ifndef PROJECTIMPORTMANAGER_H
#define PROJECTIMPORTMANAGER_H

#include <QFileInfoList>
#include <QUndoCommand>

#include "codec/decoder.h"
#include "task/task.h"
#include "widget/projectexplorer/projectviewmodel.h"

namespace olive {

class ProjectImportTask : public Task {
  Q_OBJECT
 public:
  ProjectImportTask(Folder* folder, const QStringList& filenames);

  [[nodiscard]] const int& GetFileCount() const;

  [[nodiscard]] MultiUndoCommand* GetCommand() const { return command_; }

  [[nodiscard]] const QStringList& GetInvalidFiles() const { return invalid_files_; }

  [[nodiscard]] bool HasInvalidFiles() const { return !invalid_files_.isEmpty(); }

  [[nodiscard]] const QVector<Footage*>& GetImportedFootage() const { return imported_footage_; }

 protected:
  bool Run() override;

 private:
  void Import(Folder* folder, QFileInfoList import, int& counter, MultiUndoCommand* parent_command);

  void ValidateImageSequence(Footage* footage, QFileInfoList& info_list, int index);

  void AddItemToFolder(Folder* folder, Node* item, MultiUndoCommand* command);

  static bool ItemIsStillImageFootageOnly(Footage* footage);

  static bool CompareStillImageSize(Footage* footage, const QSize& sz);

  static int64_t GetImageSequenceLimit(const QString& start_fn, int64_t start, bool up);

  MultiUndoCommand* command_;

  Folder* folder_;

  QFileInfoList filenames_;

  int file_count_;

  QStringList invalid_files_;

  QList<QString> image_sequence_ignore_files_;

  QVector<Footage*> imported_footage_;
};

}  // namespace olive

#endif  // PROJECTIMPORTMANAGER_H
