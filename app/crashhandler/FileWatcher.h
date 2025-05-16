// FileWatcher.h (假设这是文件名)
#ifndef FILEWATCHER_H_OLIVE  // 通常会加上项目名或特定后缀避免冲突
#define FILEWATCHER_H_OLIVE

#include <QDebug>              // Qt 调试输出 (如果需要在槽函数中打印日志)
#include <QFileSystemWatcher>  // Qt 文件系统监控类
#include <QObject>             // QObject 基类
#include <QString>             // Qt 字符串类

// 假设 crashhandler.h 声明了 CrashHandlerDialog 类
#include "crashhandler.h"
// #include <QCoreApplication> // 如果 CrashHandlerDialog 或其父类需要
// #include <cstdlib>          // 如果需要 std::exit 或其他 C 标准库函数

namespace olive {

/**
 * @brief 监控指定文件或目录的变化，并在变化时触发崩溃处理对话框的类。
 *
 * 此类封装了一个 QFileSystemWatcher 来监控一个特定路径。当该路径下的
 * 文件或目录发生变化时，它会打开一个 CrashHandlerDialog。
 * 这可能用于在开发过程中检测外部对特定文件的意外修改，或者在特定
 * 场景下提示用户某些关键文件被更改。
 */
class FileWatcher : public QObject {
  Q_OBJECT

 public:
  /**
   * @brief 构造一个新的 FileWatcher 对象。
   *
   * 初始化 QFileSystemWatcher 来监控指定的 `path`，并创建一个
   * CrashHandlerDialog 实例。同时连接文件/目录变化的信号到相应的槽函数。
   *
   * @param path 要监控的文件或目录的路径。
   * @param parent 父 QObject 对象，默认为 nullptr。FileWatcher 及其内部创建的
   * QFileSystemWatcher 和 CrashHandlerDialog 都会将此作为父对象。
   */
  explicit FileWatcher(const QString &path, QObject *parent = nullptr) : QObject(parent) {
    m_watcher = new QFileSystemWatcher(this);       // 以 this 为父对象，确保随 FileWatcher 销毁
    m_dialog = new CrashHandlerDialog(this, path);  // 以 this 为父对象
    m_watcher->addPath(path);                       // 添加要监控的路径

    // 连接信号和槽
    connect(m_watcher, &QFileSystemWatcher::fileChanged, this, &FileWatcher::onFileChanged);
    connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &FileWatcher::onDirectoryChanged);
  }

  /**
   * @brief 析构函数。
   *
   * 由于 m_watcher 和 m_dialog 的父对象是 this (FileWatcher 实例)，
   * 它们会在 FileWatcher 析构时由 Qt 的父子对象机制自动删除，
   * 通常不需要在此显式 delete。
   */
  ~FileWatcher() override = default;  // 默认析构即可，或根据需要添加清理逻辑

 private slots:
  /**
   * @brief 当监控的文件发生变化时调用的槽函数。
   * 此函数会打开 m_dialog (CrashHandlerDialog)。
   * @param path 发生变化的文件路径。此参数在此槽函数的当前实现中未使用，
   * 但由 QFileSystemWatcher::fileChanged 信号提供。
   */
  void onFileChanged(const QString &path) {
    Q_UNUSED(path);  // 标记 path 参数为未使用，避免编译器警告
    if (m_dialog) {  // 确保对话框指针有效
      m_dialog->open();
    }
  }

  /**
   * @brief 当监控的目录发生变化时调用的槽函数。
   * 此函数简单地调用 onFileChanged，假设目录变化也应触发相同的响应。
   * @param path 发生变化的目录路径。
   */
  void onDirectoryChanged(const QString &path) {
    // 目录变化也视为文件变化来处理，打开对话框
    onFileChanged(path);
  }

 private:
  /**
   * @brief 指向 QFileSystemWatcher 对象的指针，用于监控文件系统事件。
   */
  QFileSystemWatcher *m_watcher;

  /**
   * @brief 指向 CrashHandlerDialog 对象的指针，当文件或目录变化时将显示此对话框。
   */
  CrashHandlerDialog *m_dialog;
};

}  // namespace olive

#endif  // FILEWATCHER_H_OLIVE