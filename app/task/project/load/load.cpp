#include "load.h"

#include <QApplication>

#include "node/project/serializer/serializer.h"

namespace olive {

ProjectLoadTask::ProjectLoadTask(const QString &filename) : ProjectLoadBaseTask(filename) {}

bool ProjectLoadTask::Run() {
  project_ = new Project();

  project_->set_filename(GetFilename());

  ProjectSerializer::Result result = ProjectSerializer::Load(project_, GetFilename(), ProjectSerializer::kProject);

  layout_ = result.GetLoadData().layout;

  switch (result.code()) {
    case ProjectSerializer::kSuccess:
      break;
    case ProjectSerializer::kProjectTooOld:
      SetError(tr("This project is from a version of Olive that is no longer supported in this version."));
      break;
    case ProjectSerializer::kProjectTooNew:
      SetError(tr("This project is from a newer version of Olive and cannot be opened in this version."));
      break;
    case ProjectSerializer::kUnknownVersion:
      SetError(tr("Failed to determine project version."));
      break;
    case ProjectSerializer::kFileError:
      SetError(tr("Failed to read file \"%1\" for reading.").arg(GetFilename()));
      break;
    case ProjectSerializer::kXmlError:
      SetError(tr("Failed to read XML document. File may be corrupt. Error was: %1").arg(result.GetDetails()));
      break;
    case ProjectSerializer::kNoData:
      SetError(tr("Failed to find any data to parse."));
      break;

      // Errors that should never be thrown by a load
    case ProjectSerializer::kOverwriteError:
      SetError(tr("Unknown error."));
      break;
  }

  if (result == ProjectSerializer::kSuccess) {
    project_->moveToThread(qApp->thread());
    return true;
  } else {
    delete project_;
    return false;
  }
}

}  // namespace olive
