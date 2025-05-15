

#include "save.h"

#include <QDir>
#include <QFile>
#include <QXmlStreamWriter>

#include "common/filefunctions.h"
#include "core.h"
#include "node/project/serializer/serializer.h"

namespace olive {

ProjectSaveTask::ProjectSaveTask(Project *project, bool use_compression)
    : project_(project), use_compression_(use_compression) {
  SetTitle(tr("Saving '%1'").arg(project->filename()));
}

bool ProjectSaveTask::Run() {
  QString using_filename = override_filename_.isEmpty() ? project_->filename() : override_filename_;

  ProjectSerializer::SaveData data(ProjectSerializer::kProject);

  data.SetFilename(using_filename);
  data.SetProject(project_);
  data.SetLayout(layout_);

  ProjectSerializer::Result result = ProjectSerializer::Save(data, use_compression_);

  bool success = false;

  switch (result.code()) {
    case ProjectSerializer::kSuccess:
      success = true;
      break;
    case ProjectSerializer::kXmlError:
      SetError(tr("Failed to write XML data."));
      break;
    case ProjectSerializer::kFileError:
      SetError(tr("Failed to open file \"%1\" for writing.").arg(result.GetDetails()));
      break;
    case ProjectSerializer::kOverwriteError:
      SetError(tr(R"(Failed to overwrite "%1". Project has been saved as "%2" instead.)")
                   .arg(using_filename, result.GetDetails()));
      success = true;
      break;

      // Errors that should never be thrown by a save
    case ProjectSerializer::kProjectTooNew:
    case ProjectSerializer::kProjectTooOld:
    case ProjectSerializer::kUnknownVersion:
    case ProjectSerializer::kNoData:
      SetError(tr("Unknown error."));
      break;
  }

  return success;
}

}  // namespace olive
