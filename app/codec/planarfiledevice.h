#ifndef PLANARFILEDEVICE_H
#define PLANARFILEDEVICE_H

#include <olive/core/core.h> // 包含了 olive::core 命名空间的基础定义
#include <QFile>             // 用于文件操作
#include <QObject>           // QObject 基类
#include <QIODevice>         // 包含 QIODevice::OpenMode
#include <QVector>           // 用于存储 QFile 指针的 QVector
#include <QString>           // 用于文件名

namespace olive {

using namespace core; // 使用 olive::core 命名空间

/**
 * @brief 管理一组平面文件作为单个逻辑设备的类。
 *
 * 此类允许将多个单独的文件（通常代表多通道音频的各个通道）
 * 视为一个统一的设备进行读写操作。它提供了类似于 QIODevice 的接口，
 * 但针对平面数据进行了优化。
 */
class PlanarFileDevice : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 PlanarFileDevice 对象。
   * @param parent 父 QObject 对象，默认为 nullptr。
   */
  explicit PlanarFileDevice(QObject *parent = nullptr);

  /**
   * @brief 析构函数。
   * 确保所有打开的文件都被关闭并释放相关资源。
   */
  ~PlanarFileDevice() override;

  /**
   * @brief 检查设备（即其管理的文件组）是否已打开。
   * @return bool 如果至少有一个文件被成功打开，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool isOpen() const { return !files_.isEmpty(); }

  /**
   * @brief 打开指定的一组文件名作为平面设备。
   * @param filenames 包含所有平面文件（每个通道一个文件）路径的 QVector<QString>。
   * @param mode 打开文件的模式 (例如 QIODevice::ReadOnly, QIODevice::WriteOnly)。
   * @return bool 如果所有文件都成功以指定模式打开，则返回 true；否则返回 false，并关闭已成功打开的文件。
   */
  bool open(const QVector<QString> &filenames, QIODevice::OpenMode mode);

  /**
   * @brief 从所有打开的平面文件中读取数据。
   * 数据从每个文件的当前位置或指定的偏移量开始读取。
   * @param data 指向字符指针数组的指针，用于存储从每个文件读取的数据。
   * 调用者需要确保为每个通道分配了足够的内存。
   * @param bytes_per_channel 要从每个通道（文件）读取的字节数。
   * @param offset 可选参数，读取操作开始前要定位到的偏移量（相对于文件开头）。默认为0，表示从当前位置读取。
   * @return qint64 成功从每个通道读取的字节数。如果发生错误或到达文件末尾，则可能小于 bytes_per_channel。
   * 如果任何一个文件读取失败，则返回 -1。
   */
  qint64 read(char **data, qint64 bytes_per_channel, qint64 offset = 0);

  /**
   * @brief 将数据写入到所有打开的平面文件中。
   * 数据从每个文件的当前位置或指定的偏移量开始写入。
   * @param data 指向常量字符指针数组的指针，包含要写入到每个文件的数据。
   * @param bytes_per_channel 要写入到每个通道（文件）的字节数。
   * @param offset 可选参数，写入操作开始前要定位到的偏移量（相对于文件开头）。默认为0，表示从当前位置写入。
   * @return qint64 成功写入到每个通道的字节数。如果发生错误，则可能小于 bytes_per_channel。
   * 如果任何一个文件写入失败，则返回 -1。
   */
  qint64 write(const char **data, qint64 bytes_per_channel, qint64 offset = 0);

  /**
   * @brief 获取设备的总大小（以字节为单位）。
   * 对于平面文件设备，这通常是第一个文件的大小。
   * 假设所有平面文件大小相同。
   * @return qint64 设备的大小。如果设备未打开或文件为空，则返回0或-1。
   */
  [[nodiscard]] qint64 size() const;

  /**
   * @brief 在所有打开的平面文件中定位到指定的位置。
   * @param pos 要定位到的字节偏移量（相对于文件开头）。
   * @return bool 如果所有文件都成功定位到指定位置，则返回 true；否则返回 false。
   */
  bool seek(qint64 pos);

  /**
   * @brief 关闭所有打开的平面文件并释放相关资源。
   */
  void close();

 private:
  /**
   * @brief 存储指向已打开的 QFile 对象的指针的 QVector。
   * 每个 QFile 对象代表平面数据的一个通道/文件。
   */
  QVector<QFile *> files_;
};

}  // namespace olive

#endif  // PLANARFILEDEVICE_H