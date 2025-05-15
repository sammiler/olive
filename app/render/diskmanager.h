#ifndef DISKMANAGER_H // 防止头文件被重复包含的宏
#define DISKMANAGER_H // 定义 DISKMANAGER_H 宏

#include <QMap>    // Qt 映射容器 (有序哈希表)
#include <QMutex>  // Qt 互斥锁 (虽然在此头文件中未直接使用，但具体实现中可能需要)
#include <QObject> // Qt 对象模型基类
#include <QTimer>  // Qt 定时器类

#include "common/define.h" // 可能包含项目通用的定义或宏
#include "node/project.h"  // 包含 Project 类的定义 (DiskManager 可能需要与项目交互)

namespace olive { // olive 项目的命名空间

/**
 * @brief DiskCacheFolder 类负责管理一个特定的磁盘缓存文件夹。
 *
 * 它跟踪文件夹中的文件、它们的访问时间、文件大小，并根据设定的限制 (大小限制、关闭时清除等)
 * 来管理缓存的清理。当缓存大小超过限制时，它会删除最近最少使用的文件。
 * 它还会定期将缓存索引（文件列表及其元数据）保存到磁盘。
 */
class DiskCacheFolder : public QObject { // DiskCacheFolder 继承自 QObject
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  /**
   * @brief 构造函数。
   * @param path 此缓存文件夹在磁盘上的路径。
   * @param parent 父对象指针，默认为 nullptr。
   */
  explicit DiskCacheFolder(const QString& path, QObject* parent = nullptr);

  // 析构函数，在对象销毁时可能会执行清理操作 (例如保存索引、根据设置清除缓存)。
  ~DiskCacheFolder() override;

  /**
   * @brief 清除此缓存文件夹中的所有缓存文件。
   * @return 如果清除成功，返回 true。
   */
  bool ClearCache();

  /**
   * @brief 记录指定文件被访问的时间。
   * 用于实现基于最近最少使用 (LRU) 的缓存淘汰策略。
   * @param filename被访问的文件名 (相对于缓存路径)。
   */
  void Accessed(const QString& filename);

  /**
   * @brief 当在此缓存文件夹中创建了一个新文件时调用此方法。
   * 用于更新缓存索引和当前占用空间。
   * @param filename 新创建的文件名。
   */
  void CreatedFile(const QString& filename);

  /**
   * @brief 获取此缓存文件夹的路径。
   * @return 返回路径字符串的常量引用。
   */
  [[nodiscard]] const QString& GetPath() const { return path_; }

  /**
   * @brief 设置此缓存文件夹的路径。
   * (注意：更改已在使用的缓存文件夹路径可能需要复杂的迁移或清理逻辑)。
   * @param path 新的路径字符串。
   */
  void SetPath(const QString& path);

  /**
   * @brief 获取此缓存文件夹的大小限制 (通常以字节为单位)。
   * @return 返回 qint64 类型的限制大小。
   */
  [[nodiscard]] qint64 GetLimit() const { return limit_; }

  /**
   * @brief 获取是否在应用程序关闭时清除此缓存文件夹的设置。
   * @return 如果设置为关闭时清除，则返回 true。
   */
  [[nodiscard]] bool GetClearOnClose() const { return clear_on_close_; }

  /**
   * @brief 设置此缓存文件夹的大小限制。
   * @param l 新的限制大小。
   */
  void SetLimit(qint64 l) { limit_ = l; }

  /**
   * @brief 设置是否在应用程序关闭时清除此缓存文件夹。
   * @param e 如果为 true，则在关闭时清除。
   */
  void SetClearOnClose(bool e) { clear_on_close_ = e; }

  /**
   * @brief 删除缓存文件夹中指定的单个文件。
   * @param f 要删除的文件名。
   * @return 如果删除成功，返回 true。
   */
  bool DeleteSpecificFile(const QString& f);

 signals: // Qt 信号声明
  /**
   * @brief 当一个缓存帧文件被删除时发出的信号。
   * @param path 缓存文件夹的路径。
   * @param filename 被删除的文件名。
   */
  void DeletedFrame(const QString& path, const QString& filename);

 private:
  // 内部结构体，用于存储缓存文件的元数据
  struct HashTime {
    qint64 file_size;   // 文件大小 (字节)
    qint64 access_time; // 文件的最后访问时间戳
  };

  // 内部辅助函数：删除指定的缓存文件 (通过迭代器指向的文件)
  bool DeleteFileInternal(QMap<QString, HashTime>::iterator hash_to_delete);

  // 删除最近最少使用的文件，直到缓存大小低于限制
  bool DeleteLeastRecent();

  // 关闭缓存文件夹时调用的清理函数 (例如保存索引)
  void CloseCacheFolder();

  QString path_;       // 缓存文件夹的完整路径
  QString index_path_; // 缓存索引文件的路径

  // 存储磁盘上缓存文件及其元数据 (文件名 -> HashTime) 的映射
  QMap<QString, HashTime> disk_data_;

  qint64 consumption_{}; // 当前缓存已占用的磁盘空间 (字节)
  qint64 limit_{};       // 缓存大小限制 (字节)

  bool clear_on_close_{}; // 是否在关闭时清除此缓存的标志

  QTimer save_timer_; // 用于定期保存缓存索引的定时器

 private slots: // Qt 私有槽函数
  // 定时器触发时调用的槽函数，用于保存磁盘缓存索引文件
  void SaveDiskCacheIndex();
};

/**
 * @brief DiskManager 类是一个单例对象，负责管理应用程序中所有的磁盘缓存文件夹 (DiskCacheFolder)。
 *
 * 它提供了创建、获取和操作这些缓存文件夹的接口，并可以全局地处理缓存相关的设置和事件。
 * 用户通常通过这个单例来访问默认的缓存文件夹或特定的缓存位置。
 */
class DiskManager : public QObject { // DiskManager 继承自 QObject
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  // (静态) 创建 DiskManager 的单例实例
  static void CreateInstance();

  // (静态) 销毁 DiskManager 的单例实例
  static void DestroyInstance();

  // (静态) 获取 DiskManager 的单例实例指针
  static DiskManager* instance();

  /**
   * @brief 清除指定路径的磁盘缓存文件夹中的所有内容。
   * @param cache_folder 要清除的缓存文件夹路径。
   * @return 如果清除成功，返回 true。
   */
  bool ClearDiskCache(const QString& cache_folder);

  /**
   * @brief 获取默认的磁盘缓存文件夹对象。
   * 应用程序通常会有一个主要的默认缓存位置。
   * (注意：如果 open_folders_ 为空，调用 first() 会导致未定义行为或崩溃。
   *  一个更安全的实现会先检查 isEmpty()。但遵循不修改代码原则，此处保留原样。)
   * @return 返回指向默认 DiskCacheFolder 对象的指针。
   */
  [[nodiscard]] DiskCacheFolder* GetDefaultCacheFolder() const {
    // 第一个打开的文件夹总是默认的
    return open_folders_.first();
  }

  /**
   * @brief 获取默认磁盘缓存文件夹的路径。
   * @return 返回默认缓存路径字符串的常量引用。
   */
  [[nodiscard]] const QString& GetDefaultCachePath() const { return GetDefaultCacheFolder()->GetPath(); }

  /**
   * @brief 根据路径获取一个已打开的 DiskCacheFolder 对象。
   * 如果该路径的缓存文件夹尚未被管理，此方法可能会创建并注册一个新的 DiskCacheFolder。
   * @param path 缓存文件夹的路径。
   * @return 返回对应的 DiskCacheFolder 指针。
   */
  DiskCacheFolder* GetOpenFolder(const QString& path);

  /**
   * @brief 获取所有当前已打开 (被管理) 的磁盘缓存文件夹的列表。
   * @return 返回一个包含 DiskCacheFolder 指针的 QVector 的常量引用。
   */
  [[nodiscard]] const QVector<DiskCacheFolder*>& GetOpenFolders() const { return open_folders_; }

  // (静态) 显示一个确认对话框，询问用户是否确定更改磁盘缓存设置 (通常在更改路径或限制时)
  static bool ShowDiskCacheChangeConfirmationDialog(QWidget* parent);

  // (静态) 获取默认磁盘缓存配置文件的路径
  static QString GetDefaultDiskCacheConfigFile();

  // (静态) 获取默认的磁盘缓存路径 (应用程序级别的默认设置)
  static QString GetDefaultDiskCachePath();

  // (静态) 显示指定 DiskCacheFolder 的磁盘缓存设置对话框
  static void ShowDiskCacheSettingsDialog(DiskCacheFolder* folder, QWidget* parent);
  // 显示指定路径的磁盘缓存设置对话框 (内部会先获取或创建对应的 DiskCacheFolder)
  void ShowDiskCacheSettingsDialog(const QString& path, QWidget* parent);

 public slots: // Qt 公有槽函数
  /**
   * @brief 当指定缓存文件夹中的某个文件被访问时调用的槽函数。
   * 通常由 DiskCacheFolder 对象发出信号，DiskManager 在此接收并可能执行全局逻辑。
   * @param cache_folder 缓存文件夹的路径。
   * @param filename被访问的文件名。
   */
  void Accessed(const QString& cache_folder, const QString& filename);

  /**
   * @brief 当在指定缓存文件夹中创建了一个新文件时调用的槽函数。
   * @param cache_folder 缓存文件夹的路径。
   * @param filename 新创建的文件名。
   */
  void CreatedFile(const QString& cache_folder, const QString& filename);

  /**
   * @brief 删除指定路径下的特定缓存文件 (文件名通常包含完整路径或相对于某个已知根)。
   * @param filename 要删除的文件名。
   */
  void DeleteSpecificFile(const QString& filename);

 signals: // Qt 信号声明
  /**
   * @brief 当一个缓存帧文件被删除时发出的信号 (可能是从任何一个 DiskCacheFolder 发出的)。
   * @param path 缓存文件夹的路径。
   * @param filename 被删除的文件名。
   */
  void DeletedFrame(const QString& path, const QString& filename);

  /**
   * @brief 当磁盘缓存操作可能导致某个项目的数据失效时发出的信号。
   * 例如，清除了项目使用的缓存。
   * @param p 相关的 Project 指针。
   */
  void InvalidateProject(Project* p);

 private:
  // 私有构造函数 (用于单例模式)
  DiskManager();

  // 私有析构函数 (用于单例模式)
  ~DiskManager() override;

  static DiskManager* instance_; // DiskManager 的静态单例实例指针

  // 存储所有当前打开并被管理的 DiskCacheFolder 对象的列表
  QVector<DiskCacheFolder*> open_folders_;
};

}  // namespace olive

#endif  // DISKMANAGER_H