#ifndef DISKMANAGER_H
#define DISKMANAGER_H

#include <QMap>
#include <QMutex>
#include <QObject>
#include <QTimer>

#include "common/define.h"
#include "node/project.h"

namespace olive {

class DiskCacheFolder : public QObject {
  Q_OBJECT
 public:
  explicit DiskCacheFolder(const QString& path, QObject* parent = nullptr);

  ~DiskCacheFolder() override;

  bool ClearCache();

  void Accessed(const QString& filename);

  void CreatedFile(const QString& filename);

  [[nodiscard]] const QString& GetPath() const { return path_; }

  void SetPath(const QString& path);

  [[nodiscard]] qint64 GetLimit() const { return limit_; }

  [[nodiscard]] bool GetClearOnClose() const { return clear_on_close_; }

  void SetLimit(qint64 l) { limit_ = l; }

  void SetClearOnClose(bool e) { clear_on_close_ = e; }

  bool DeleteSpecificFile(const QString& f);

 signals:
  void DeletedFrame(const QString& path, const QString& filename);

 private:
  struct HashTime {
    qint64 file_size;
    qint64 access_time;
  };

  bool DeleteFileInternal(QMap<QString, HashTime>::iterator hash_to_delete);

  bool DeleteLeastRecent();

  void CloseCacheFolder();

  QString path_;

  QString index_path_;

  QMap<QString, HashTime> disk_data_;

  qint64 consumption_{};

  qint64 limit_{};

  bool clear_on_close_{};

  QTimer save_timer_;

 private slots:
  void SaveDiskCacheIndex();
};

class DiskManager : public QObject {
  Q_OBJECT
 public:
  static void CreateInstance();

  static void DestroyInstance();

  static DiskManager* instance();

  bool ClearDiskCache(const QString& cache_folder);

  [[nodiscard]] DiskCacheFolder* GetDefaultCacheFolder() const {
    // The first folder will always be the default
    return open_folders_.first();
  }

  [[nodiscard]] const QString& GetDefaultCachePath() const { return GetDefaultCacheFolder()->GetPath(); }

  DiskCacheFolder* GetOpenFolder(const QString& path);

  [[nodiscard]] const QVector<DiskCacheFolder*>& GetOpenFolders() const { return open_folders_; }

  static bool ShowDiskCacheChangeConfirmationDialog(QWidget* parent);

  static QString GetDefaultDiskCacheConfigFile();

  static QString GetDefaultDiskCachePath();

  static void ShowDiskCacheSettingsDialog(DiskCacheFolder* folder, QWidget* parent);
  void ShowDiskCacheSettingsDialog(const QString& path, QWidget* parent);

 public slots:
  void Accessed(const QString& cache_folder, const QString& filename);

  void CreatedFile(const QString& cache_folder, const QString& filename);

  void DeleteSpecificFile(const QString& filename);

 signals:
  void DeletedFrame(const QString& path, const QString& filename);

  void InvalidateProject(Project* p);

 private:
  DiskManager();

  ~DiskManager() override;

  static DiskManager* instance_;

  QVector<DiskCacheFolder*> open_folders_;
};

}  // namespace olive

#endif  // DISKMANAGER_H
