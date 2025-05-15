

#ifndef FILEFUNCTIONS_H
#define FILEFUNCTIONS_H

#include <QDir>
#include <QString>
#include <QFileInfo> // 为了 QFileInfo::exists()，虽然未直接在头文件中使用但相关
#include <QIODevice> // 虽然未直接在头文件中使用，但ReadFileAsString等函数会涉及

#include "common/define.h" // 可能包含一些通用定义

namespace olive {

/**
 * @brief 提供一系列静态文件和目录操作辅助函数的集合类。
 *
 * 此类封装了常用的文件系统操作，如判断便携模式、获取配置路径、
 * 复制目录、确保文件扩展名、读写文件以及安全地重命名文件等。
 */
class FileFunctions {
 public:
  /**
   * @brief 判断应用程序当前是否以“便携模式”运行。
   *
   * 在便携模式下，所有持久化配置文件应存储在相对于应用程序可执行文件的路径下，
   * 而不是用户的标准主目录中。
   * @return bool 如果应用程序处于便携模式，则返回 true；否则返回 false。
   */
  static bool IsPortable();

  /**
   * @brief 为给定的文件名生成一个唯一的文件标识符。
   * 这个标识符可以用于例如缓存键或内部追踪，确保唯一性。
   * @param filename 原始文件名。
   * @return QString 生成的唯一文件标识符。
   */
  static QString GetUniqueFileIdentifier(const QString& filename);

  /**
   * @brief 获取应用程序配置文件的标准存储位置。
   * 会根据是否为便携模式返回不同的路径。
   * @return QString 配置文件的存储目录路径。
   */
  static QString GetConfigurationLocation();

  /**
   * @brief 获取应用程序可执行文件所在的目录路径。
   * @return QString 应用程序路径。
   */
  static QString GetApplicationPath();

  /**
   * @brief 获取一个临时文件的完整路径。
   * 此路径通常指向系统定义的临时文件目录下的一个唯一文件名。
   * @return QString 临时文件的路径。
   */
  static QString GetTempFilePath();

  /**
   * @brief 检查是否可以在不覆盖目标目录下现有文件的情况下复制源目录。
   * @param source 源目录的路径。
   * @param dest 目标目录的路径。
   * @return bool 如果可以安全复制（即目标目录中不存在与源目录中同名的文件或文件夹），则返回 true；否则返回 false。
   */
  static bool CanCopyDirectoryWithoutOverwriting(const QString& source, const QString& dest);

  /**
   * @brief 复制整个目录（包括其内容）从源路径到目标路径。
   * @param source 源目录的路径。
   * @param dest 目标目录的路径。
   * @param overwrite 如果为 true，并且目标位置已存在同名文件或目录，则覆盖它。默认为 false。
   */
  static void CopyDirectory(const QString& source, const QString& dest, bool overwrite = false);

  /**
   * @brief 检查指定的目录是否有效（例如，是否存在且可读写）。
   * @param dir 要检查的 QDir 对象。
   * @param try_to_create_if_not_exists 如果目录不存在，是否尝试创建它。默认为 true。
   * @return bool 如果目录有效（或成功创建），则返回 true；否则返回 false。
   */
  static bool DirectoryIsValid(const QDir& dir, bool try_to_create_if_not_exists = true);

  /**
   * @brief 确保给定的文件名具有指定的扩展名。
   *
   * 此函数会检查文件名是否已包含提供的扩展名（不区分大小写）。
   * 如果没有，则会将扩展名追加到文件名末尾。
   * 扩展名应不带点号提供 (例如 "ove" 而不是 ".ove")。
   * @param fn 原始文件名。
   * @param extension 要确保的扩展名 (不带点号)。
   * @return QString 处理后的文件名，可能已附加了扩展名。
   */
  static QString EnsureFilenameExtension(QString fn, const QString& extension);

  /**
   * @brief 将整个文件的内容读取为一个字符串。
   * @param filename 要读取的文件名。
   * @return QString 文件内容字符串；如果读取失败，则可能返回空字符串或抛出异常（取决于实现）。
   */
  static QString ReadFileAsString(const QString& filename);

  /**
   * @brief 获取一个临时的、安全的文件名，用于在写入操作期间替代原始文件名。
   *
   * 当需要覆盖一个现有文件时，最安全的方法是先写入到一个新的临时文件，
   * 只有在写入成功完成后才用临时文件替换原始文件。这样可以防止在程序崩溃
   * 或用户中途取消保存时破坏原始文件。
   *
   * 此函数返回一个与原始文件名略有不同且保证当前不存在的文件名，
   * 因此不会意外覆盖任何重要文件。
   * @param original 原始文件名，将基于此名称生成临时文件名。
   * @return QString 一个安全的、唯一的临时文件名。
   */
  static QString GetSafeTemporaryFilename(const QString& original);

  /**
   * @brief 重命名文件，允许覆盖已存在的目标文件。
   * 首先尝试删除已存在的目标文件 `to` (如果存在)，然后将文件 `from` 重命名为 `to`。
   * @param from 源文件名。
   * @param to 目标文件名。
   * @return bool 如果重命名（包括可能的删除和重命名操作）成功，则返回 true；否则返回 false。
   */
  static bool RenameFileAllowOverwrite(const QString& from, const QString& to);

  /**
   * @brief 根据当前操作系统平台格式化可执行文件名。
   * 例如，在 Windows 上为未格式化的名称追加 ".exe" 扩展名。
   * @param unformatted 未格式化的可执行文件基本名称。
   * @return QString 格式化后的可执行文件名。
   */
  inline static QString GetFormattedExecutableForPlatform(QString unformatted) {
#ifdef Q_OS_WINDOWS
    unformatted.append(QStringLiteral(".exe"));
#endif
    // 其他平台的处理可以添加到这里
    return unformatted;
  }

  /**
   * @brief 获取自动恢复文件的根目录路径。
   * @return QString 自动恢复文件的根目录路径。
   */
  static QString GetAutoRecoveryRoot();
};

}  // namespace olive

#endif  // FILEFUNCTIONS_H