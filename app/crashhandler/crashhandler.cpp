#include "crashhandler.h"
#include <qobject.h>

#include <QApplication>
#include <QCloseEvent>
#include <QDir>
#include <QFile>
#include <QFontDatabase>
#include <QHttpMultiPart>
#include <QLabel>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QProcess>
#include <QScrollBar>
#include <QSplitter>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>
#include <cstdlib>
#include "FileWatcher.h"
#include "common/crashpadutils.h"
#include "common/filefunctions.h"
#include "version.h"

namespace olive {

CrashHandlerDialog::CrashHandlerDialog(QObject* parent, const QString& report_path) {
  setWindowTitle(tr("Olive"));
  setWindowFlags(Qt::WindowStaysOnTopHint);

  report_filename_ = report_path;
  waiting_for_upload_ = false;

  auto* layout = new QVBoxLayout(this);

  layout->addWidget(
      new QLabel(tr("We're sorry, Olive has crashed. Please help us fix it by "
                    "sending an error report.")));

  auto* splitter = new QSplitter(Qt::Vertical);
  splitter->setChildrenCollapsible(false);
  layout->addWidget(splitter);

  summary_edit_ = new QTextEdit();
  summary_edit_->setPlaceholderText(
      tr("Describe what you were doing in as much detail as "
         "possible. If you can, provide steps to reproduce this crash."));

  splitter->addWidget(summary_edit_);

  auto* crash_widget = new QWidget();
  auto* crash_widget_layout = new QVBoxLayout(crash_widget);
  crash_widget_layout->setMargin(0);

  crash_widget_layout->addWidget(new QLabel(tr("Crash Report:")));

  crash_report_ = new QTextEdit();
  crash_report_->setReadOnly(true);
  crash_report_->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  crash_widget_layout->addWidget(crash_report_);

  splitter->addWidget(crash_widget);

  auto* btn_layout = new QHBoxLayout();
  btn_layout->setMargin(0);
  btn_layout->addStretch();

  send_report_btn_ = new QPushButton(tr("Send Error Report"));
  connect(send_report_btn_, &QPushButton::clicked, this, &CrashHandlerDialog::SendErrorReport);
  btn_layout->addWidget(send_report_btn_);

  dont_send_btn_ = new QPushButton(tr("Don't Send"));
  connect(dont_send_btn_, &QPushButton::clicked, this, &CrashHandlerDialog::reject);
  btn_layout->addWidget(dont_send_btn_);

  layout->addLayout(btn_layout);

  crash_report_->setEnabled(false);
  send_report_btn_->setEnabled(false);

  crash_report_->setText(tr("Waiting for crash report to be generated..."));

  GenerateReport();
}

void CrashHandlerDialog::SetGUIObjectsEnabled(bool e) {
  summary_edit_->setEnabled(e);
  crash_report_->setEnabled(e);
  send_report_btn_->setEnabled(e);
  dont_send_btn_->setEnabled(e);
}

QString CrashHandlerDialog::GetSymbolPath() {
  QDir app_path(qApp->applicationDirPath());
  QString symbols_path;

#if BUILDFLAG(IS_WIN)
  symbols_path = app_path.filePath(QStringLiteral("symbols"));
#elif BUILDFLAG(IS_LINUX)
  app_path.cdUp();
  symbols_path = app_path.filePath(QStringLiteral("share/olive-editor/symbols"));
#elif BUILDFLAG(IS_APPLE)
  app_path.cdUp();
  symbols_path = app_path.filePath(QStringLiteral("Resources/symbols"));
#endif

  return symbols_path;
}

void CrashHandlerDialog::GenerateReport() {
  auto* p = new QProcess();

  connect(p, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
          &CrashHandlerDialog::ReadProcessFinished);
  connect(p, &QProcess::readyReadStandardOutput, this, &CrashHandlerDialog::ReadProcessHasData);

  QString stackwalk_filename = FileFunctions::GetFormattedExecutableForPlatform(QStringLiteral("minidump_stackwalk"));

  QString stackwalk_bin = QDir(qApp->applicationDirPath()).filePath(stackwalk_filename);
  p->start(stackwalk_bin, {report_filename_, GetSymbolPath()});
  crash_report_->setText(QStringLiteral("Trying to run: %1").arg(stackwalk_bin));
}

void CrashHandlerDialog::ReplyFinished(QNetworkReply* reply) {
  waiting_for_upload_ = false;

  if (reply->error() == QNetworkReply::NoError) {
    // Close dialog
    QDialog::accept();
  } else {
    QMessageBox b(this);
    b.setIcon(QMessageBox::Critical);
    b.setWindowModality(Qt::WindowModal);
    b.setWindowTitle(tr("Upload Failed"));
    b.setText(tr("Failed to send error report (%1). Please try again later.").arg(QString::number(reply->error())));
    b.addButton(QMessageBox::Ok);
    b.exec();

    SetGUIObjectsEnabled(true);
  }
}

void CrashHandlerDialog::HandleSslErrors(QNetworkReply* reply, const QList<QSslError>& se) {
  QStringList errors;
  for (const QSslError& err : se) {
    errors.append(err.errorString());
  }

  QMessageBox b(this);
  b.setIcon(QMessageBox::Critical);
  b.setWindowModality(Qt::WindowModal);
  b.setWindowTitle(tr("SSL Error"));
  b.setText(tr("Encountered the following SSL errors:\n\n%1").arg(errors.join('\n')));
  b.addButton(QMessageBox::Ok);
  b.exec();
}

void CrashHandlerDialog::AttemptToFindReport() {
  // If we found it, use it, otherwise wait a second and try again
  if (report_filename_.isEmpty()) {
    // Couldn't find report, try again in one second
    QTimer::singleShot(500, this, &CrashHandlerDialog::AttemptToFindReport);
  } else {
    GenerateReport();
  }
}

void CrashHandlerDialog::ReadProcessHasData() {
  report_data_.append(dynamic_cast<QProcess*>(sender())->readAllStandardOutput());
}

void CrashHandlerDialog::ReadProcessFinished() {
  SetGUIObjectsEnabled(true);
  crash_report_->setText(report_data_);
  delete sender();
}

void CrashHandlerDialog::SendErrorReport() {
  if (summary_edit_->document()->isEmpty()) {
    QMessageBox b(this);
    b.setIcon(QMessageBox::Question);
    b.setWindowModality(Qt::WindowModal);
    b.setText(tr("You must write a description to submit this crash report."));
    b.addButton(QMessageBox::Ok);
    b.exec();
    return;
  }

  auto* manager = new QNetworkAccessManager();
  connect(manager, &QNetworkAccessManager::finished, this, &CrashHandlerDialog::ReplyFinished);
  connect(manager, &QNetworkAccessManager::sslErrors, this, &CrashHandlerDialog::HandleSslErrors);

  QNetworkRequest request;
  request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
  request.setUrl(QStringLiteral("https://olivevideoeditor.org/crashpad/report.php"));

  // Create HTTP form
  auto* multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

  // Create description section
  QHttpPart desc_part;
  desc_part.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("text/plain; charset=UTF-8"));
  desc_part.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"description\""));
  desc_part.setBody(summary_edit_->toPlainText().toUtf8());
  multipart->append(desc_part);

  // Create report section
  QHttpPart report_part;
  report_part.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("text/plain; charset=UTF-8"));
  report_part.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"report\""));
  report_part.setBody(report_data_);
  multipart->append(report_part);

  // Create commit section
  QHttpPart commit_part;
  commit_part.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("text/plain; charset=UTF-8"));
  commit_part.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"commit\""));
  commit_part.setBody(kAppVersionLong.toUtf8());
  multipart->append(commit_part);

  // Create dump section
  QHttpPart dump_part;
  dump_part.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/octet-stream"));
  dump_part.setHeader(
      QNetworkRequest::ContentDispositionHeader,
      QStringLiteral("form-data; name=\"dump\"; filename=\"%1\"").arg(QFileInfo(report_filename_).fileName()));
  auto* dump_file = new QFile(report_filename_);
  dump_file->open(QFile::ReadOnly);
  dump_part.setBodyDevice(dump_file);
  dump_file->setParent(multipart);  // Delete file with multipart
  multipart->append(dump_part);

  // Find symbol file
  QDir symbol_dir(GetSymbolPath());

  QString symbol_bin_name;
#if BUILDFLAG(IS_WIN)
  symbol_bin_name = QStringLiteral("olive-editor.pdb");
#elif BUILDFLAG(IS_APPLE)
  symbol_bin_name = QStringLiteral("Olive");
#else
  symbol_bin_name = QStringLiteral("olive-editor");
#endif
  symbol_dir = QDir(symbol_dir.filePath(symbol_bin_name));

  QStringList folders_in_symbol_path = symbol_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

  if (!folders_in_symbol_path.empty()) {
    symbol_dir = QDir(symbol_dir.filePath(folders_in_symbol_path.first()));
  } else {
    QMessageBox b(this);
    b.setIcon(QMessageBox::Critical);
    b.setWindowModality(Qt::WindowModal);
    b.setWindowTitle(tr("Failed to send report"));
    b.setText(
        tr("Failed to find symbols necessary to send report. "
           "This is a packaging issue. Please notify "
           "the maintainers of this package."));
    b.addButton(QMessageBox::Ok);
    b.exec();
    return;
  }

  // Create sym section
  QString symbol_filename;
#if BUILDFLAG(IS_APPLE)
  symbol_filename = QStringLiteral("Olive.sym");
#else
  symbol_filename = QStringLiteral("olive-editor.sym");
#endif
  QString symbol_full_path = symbol_dir.filePath(symbol_filename);
  QHttpPart sym_part;
  sym_part.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/octet-stream"));
  sym_part.setHeader(QNetworkRequest::ContentDispositionHeader,
                     QStringLiteral("form-data; name=\"sym\"; filename=\"%1\"").arg(symbol_filename));
  QFile sym_file(symbol_full_path);

  if (!sym_file.open(QFile::ReadOnly)) {
    QMessageBox b(this);
    b.setIcon(QMessageBox::Critical);
    b.setWindowModality(Qt::WindowModal);
    b.setWindowTitle(tr("Failed to send report"));
    b.setText(
        tr("Failed to open symbol file. You may not have "
           "permission to access it."));
    b.addButton(QMessageBox::Ok);
    b.exec();
    return;
  }

  QByteArray symbol_data = qCompress(sym_file.readAll(), 9);
  sym_file.close();

  sym_part.setBody(symbol_data);
  multipart->append(sym_part);

  manager->post(request, multipart);

  SetGUIObjectsEnabled(false);
  waiting_for_upload_ = true;
}

void CrashHandlerDialog::closeEvent(QCloseEvent* e) {
  QMessageBox b(this);
  b.setIcon(QMessageBox::Warning);
  b.setWindowModality(Qt::WindowModal);
  b.setWindowTitle(tr("Confirm Close"));
  b.setText(
      tr("Crash report is still uploading. Closing now may result in no "
         "report being sent. Are you sure you wish to close?"));
  b.addButton(QMessageBox::Ok);
  b.addButton(QMessageBox::Cancel);

  if (waiting_for_upload_ && b.exec() == QMessageBox::Cancel) {
    e->ignore();
  } else {
    e->accept();
  }
  exit(0);
}

}  // namespace olive

int main(int argc, char* argv[]) {
  QString report;

#ifdef Q_OS_WINDOWS
  int num_args;
  LPWSTR* args = CommandLineToArgvW(GetCommandLineW(), &num_args);

  report = QString::fromWCharArray(args[1]);
  LocalFree(args);
#endif

  QApplication a(argc, argv);
  olive::FileWatcher watcher(report);

  return QApplication::exec();
}
