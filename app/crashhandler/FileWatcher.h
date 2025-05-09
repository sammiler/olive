#include <qchar.h>
#include <qfilesystemwatcher.h>
#include <QCoreApplication>
#include <QDebug>
#include <QFileSystemWatcher>
#include <cstdlib>
#include "crashhandler.h"
namespace olive {

class FileWatcher : public QObject {
  Q_OBJECT

 public:
  FileWatcher(const QString &path) {
    m_watcher = new QFileSystemWatcher(this);
    m_dialog = new CrashHandlerDialog(this, path);
    m_watcher->addPath(path);

    connect(m_watcher, &QFileSystemWatcher::fileChanged, this, &FileWatcher::onFileChanged);
    connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &FileWatcher::onDirectoryChanged);
  }

 private slots:
  void onFileChanged(const QString &path) { m_dialog->open(); }

  void onDirectoryChanged(const QString &path) { onFileChanged(path); }

 private:
  QFileSystemWatcher *m_watcher;
  CrashHandlerDialog *m_dialog;
};

}  // namespace olive