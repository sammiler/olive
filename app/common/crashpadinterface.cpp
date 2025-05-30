#include "crashpadinterface.h"
#include <qchar.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qstringliteral.h>

#ifdef USE_CRASHPAD

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QProcess>

#include "crashpadutils.h"
#include "filefunctions.h"

#if BUILDFLAG(IS_WIN)
#include <Windows.h>
#include <winnt.h>
#endif

crashpad::CrashpadClient *client;

Result InitializeCrashpad() {
  Result result = {false};
  QString report_path = QDir(olive::FileFunctions::GetTempFilePath()).filePath(QStringLiteral("reports"));

  QString handler_fn = olive::FileFunctions::GetFormattedExecutableForPlatform(QStringLiteral("crashpad_handler"));
  QString crash_dialog = olive::FileFunctions::GetFormattedExecutableForPlatform(
      QStringLiteral("olive-crashhandler"));  // Generate absolute path
  QString crash_dialog_abs_path = QDir(QCoreApplication::applicationDirPath()).filePath(crash_dialog);
  QString handler_abs_path = QDir(QCoreApplication::applicationDirPath()).filePath(handler_fn);

  if (QFileInfo::exists(handler_abs_path) && QFileInfo::exists(crash_dialog_abs_path)) {
    base::FilePath handler(QSTRING_TO_BASE_STRING(handler_abs_path));

    base::FilePath reports_dir(QSTRING_TO_BASE_STRING(report_path));

    base::FilePath metrics_dir(
        QSTRING_TO_BASE_STRING(QDir(olive::FileFunctions::GetTempFilePath()).filePath(QStringLiteral("metrics"))));

    // Metadata that will be posted to the server with the crash report map
    std::map<std::string, std::string> annotations;

    // Disable crashpad rate limiting so that all crashes have dmp files
    std::vector<std::string> arguments;
    arguments.emplace_back("--no-rate-limit");
    arguments.emplace_back("--no-upload-gzip");

    // Initialize Crashpad database
    std::unique_ptr<crashpad::CrashReportDatabase> database = crashpad::CrashReportDatabase::Initialize(reports_dir);
    if (database == nullptr) return result;

    // Disable automated crash uploads
    crashpad::Settings *settings = database->GetSettings();
    if (settings == nullptr) return result;
    settings->SetUploadsEnabled(false);

    // Start crash handler
    client = new crashpad::CrashpadClient();
    auto ret =
        client->StartHandler(handler, reports_dir, metrics_dir, "https://olivevideoeditor.org/crashpad/report.php",
                             annotations, arguments, true, true);
    if (ret) {
      result.success = true;
      result.processPath = crash_dialog_abs_path;
      auto finalPath = report_path.append(QDir().separator() + QStringLiteral("reports"));
      result.args << finalPath;

      return result;
    }
  }
  return result;
}

#endif  // USE_CRASHPAD
