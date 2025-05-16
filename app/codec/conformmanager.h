#ifndef CONFORMMANAGER_H
#define CONFORMMANAGER_H

#include <QMutex>
#include <QObject>
#include <QString>         // 包含 QString
#include <QStringList>     // 如果 GetConformedFilename 返回 QStringList (当前是 QVector<QString>)
#include <QVector>         // 包含 QVector
#include <QWaitCondition>  // 包含 QWaitCondition

#include "codec/decoder.h"         // 包含 Decoder::CodecStream 和 AudioParams (假设 AudioParams 在此或其包含的文件中)
#include "task/conform/conform.h"  // 包含 ConformTask
#include "task/task.h"             // 包含 Task (用于 ConformTaskFinished)

// 前向声明 (如果 Decoder::CodecStream 或 AudioParams 未完全定义)
// namespace olive {
// namespace Decoder { struct CodecStream; }
// struct AudioParams;
// class ConformTask;
// class Task;
// }

namespace olive {

/**
 * @brief 管理音频适配 (conforming) 任务的单例类。
 *
 * ConformManager 负责确保音频流以特定参数（如采样率、格式）存在于缓存中。
 * 如果所需的适配版本不存在，它会启动一个 ConformTask 来生成它。
 * 此类设计为线程安全的。
 */
class ConformManager : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief 创建 ConformManager 的单例实例。
   * 如果实例已存在，则此函数不执行任何操作。
   */
  static void CreateInstance() {
    if (!instance_) {
      instance_ = new ConformManager();
    }
  }

  /**
   * @brief 销毁 ConformManager 的单例实例。
   * 释放相关资源，并将实例指针置为 nullptr。
   */
  static void DestroyInstance() {
    delete instance_;
    instance_ = nullptr;
  }

  /**
   * @brief 获取 ConformManager 的单例实例。
   * @return ConformManager* 指向 ConformManager 实例的指针。
   */
  static ConformManager *instance() { return instance_; }

  /**
   * @brief 表示音频适配状态的枚举。
   */
  enum ConformState {
    kConformExists,     ///< @brief 适配文件已存在于缓存中。
    kConformGenerating  ///< @brief 适配文件正在生成过程中。
  };

  /**
   * @brief 描述一个适配任务及其状态的结构体。
   */
  struct Conform {
    ConformState state;          ///< @brief 当前的适配状态。
    QVector<QString> filenames;  ///< @brief 适配后的文件名列表 (可能包含多个文件，例如多通道音频的每个通道一个文件)。
    ConformTask *task;           ///< @brief 如果状态是 kConformGenerating，则指向关联的 ConformTask；否则为 nullptr。
  };

  /**
   * @brief 获取指定音频流的适配状态；如果适配版本不存在，则启动适配任务。
   *
   * 此方法是线程安全的。
   * @param decoder_id 解码器的唯一标识符，用于区分不同解码器产生的缓存。
   * @param cache_path 存储适配文件的缓存路径。
   * @param stream 要适配的原始音频流信息。
   * @param params 目标音频参数（采样率、格式等）。
   * @param wait 如果为 true，并且适配文件正在生成，则此方法将阻塞直到适配完成。
   * 如果为 false，并且适配文件正在生成，则立即返回 kConformGenerating 状态。
   * @return Conform 包含适配状态和相关信息的 Conform 结构体。
   */
  Conform GetConformState(const QString &decoder_id, const QString &cache_path, const Decoder::CodecStream &stream,
                          const AudioParams &params, bool wait);

 signals:
  /**
   * @brief 当一个或多个适配任务完成，并且相应的适配文件准备就绪时发出此信号。
   */
  void ConformReady();

 private:
  /**
   * @brief 私有构造函数，用于实现单例模式。默认为 default。
   */
  ConformManager() = default;

  /**
   * @brief ConformManager 的静态单例实例指针。
   */
  static ConformManager *instance_;

  /**
   * @brief 用于保护对共享数据（如 conforming_ 列表）的访问的互斥锁。
   */
  QMutex mutex_;

  /**
   * @brief 用于在等待适配任务完成时阻塞和唤醒线程的条件变量。
   */
  QWaitCondition conform_done_condition_;

  /**
   * @brief 存储正在进行的或已完成的适配任务数据的内部结构体。
   */
  struct ConformData {
    Decoder::CodecStream stream;  ///< @brief 原始音频流信息。
    AudioParams params;           ///< @brief 目标音频参数。
    ConformTask *task{};          ///< @brief 指向关联的 ConformTask 的指针，如果任务已完成或失败则可能为 nullptr。
    QVector<QString> working_filename;   ///< @brief 适配过程中使用的临时工作文件名。
    QVector<QString> finished_filename;  ///< @brief 适配完成后最终生成的文件名。
  };

  /**
   * @brief 存储当前所有正在进行的适配任务数据的 QVector。
   */
  QVector<ConformData> conforming_;

  /**
   * @brief 根据缓存路径、原始流信息和目标参数，生成适配后文件的目标文件名。
   * @param cache_path 缓存目录的路径。
   * @param stream 原始音频流信息。
   * @param params 目标音频参数。
   * @return QVector<QString> 包含一个或多个适配后文件名的列表。
   */
  static QVector<QString> GetConformedFilename(const QString &cache_path, const Decoder::CodecStream &stream,
                                               const AudioParams &params);

  /**
   * @brief 检查所有指定的适配文件名是否都实际存在于文件系统中。
   * @param filenames 要检查的文件名列表。
   * @return bool 如果所有文件都存在，则返回 true；否则返回 false。
   */
  static bool AllConformsExist(const QVector<QString> &filenames);

 private slots:
  /**
   * @brief 当一个 ConformTask 完成时调用的槽函数。
   * @param task 指向已完成的 Task (实际上是 ConformTask) 的指针。
   * @param succeeded 标记任务是否成功完成。
   */
  void ConformTaskFinished(Task *task, bool succeeded);
};

}  // namespace olive

#endif  // CONFORMMANAGER_H