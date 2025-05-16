#ifndef TASK_H  // 防止头文件被重复包含的预处理器指令
#define TASK_H  // 定义 TASK_H 宏

#include <QDateTime>  // 包含了 Qt 日期时间类，用于获取时间戳
#include <QDebug>     // 包含了 Qt 调试输出类
#include <QObject>    // 包含了 Qt 对象模型基类
#include <memory>     // 包含了 C++ 标准库的智能指针等内存管理工具

#include "common/cancelableobject.h"  // 包含了可取消对象的基类定义

namespace olive {  // olive 项目的命名空间

/**
 * @brief Olive 中后台任务的基类。
 *
 * 任务在设计上是多线程的（即它们总是会生成一个新线程并在其中运行）。
 *
 * 要创建自定义任务，请继承此类并重写纯虚函数 Run()。Run() 方法应返回 true 表示成功，
 * 返回 false 表示失败。请注意，一个任务即使产生了“否定性”的输出，其本身也可能已经成功完成了。
 * 例如，ProbeTask 的作用是确定某个媒体文件是否可以在 Olive 中使用。即使探测 *未能* 为此文件找到解码器，
 * ProbeTask 任务本身也 *成功地* 发现了这一点。ProbeTask 的失败将意味着灾难性的故障，
 * 表明它无法确定有关该文件的任何信息。
 *
 * 任务应与 TaskManager 一起使用，TaskManager 将管理任务的启动和删除。
 * TaskManager 还会根据系统可用的线程数量来限制同时启动的任务数量，以避免系统过载。
 *
 * 任务支持“依赖任务”，即一个任务应该在另一个任务开始之前完成。
 */
class Task : public QObject, public CancelableObject {
 Q_OBJECT  // Qt 对象的宏，用于启用信号和槽机制等 Qt 特性
     public :
     /**
      * @brief Task 构造函数。
      *
      * 初始化任务的标题为 "Task"，错误信息为 "Unknown error"，开始时间为 0。
      */
     Task()
     : title_(tr("Task")), error_(tr("Unknown error")), start_time_(0) {}

  /**
   * @brief 获取此任务的当前标题。
   * @return const QString& 对任务标题字符串的常量引用。
   */
  [[nodiscard]] const QString& GetTitle() const { return title_; }

  /**
   * @brief 如果 Run() 方法返回 false，则此方法返回发生的错误信息。
   * @return const QString& 对错误信息字符串的常量引用。
   */
  [[nodiscard]] const QString& GetError() const { return error_; }

  /**
   * @brief 获取任务开始执行的时间戳。
   * @return const qint64& 对任务开始时间戳（自纪元以来的毫秒数）的常量引用。
   */
  [[nodiscard]] const qint64& GetStartTime() const { return start_time_; }

 public slots:
  /**
   * @brief 启动此任务。
   *
   * 此方法会记录任务的开始时间，发射 Started() 信号，然后调用受保护的虚方法 Run() 来执行实际的任务逻辑。
   * 任务完成后，会打印任务执行耗时（用于调试），并发射 Finished() 信号。
   * @return 如果任务的 Run() 方法成功完成则返回 true，否则返回 false。
   * @see GetError() 如果此方法返回 false，请查看错误信息。
   */
  bool Start() {
    start_time_ = QDateTime::currentMSecsSinceEpoch();  // 记录任务开始时间
    emit Started(start_time_);                          // 发射任务开始信号

    bool ret = Run();  // 执行任务的核心逻辑

    // 出于调试目的，打印此任务花费的时间
    qDebug() << this << "took" << (QDateTime::currentMSecsSinceEpoch() - start_time_);

    emit Finished(this, ret);  // 发射任务完成信号

    return ret;  // 返回任务执行结果
  }

  /**
   * @brief 重置任务状态，以便可以再次调用 Run()。
   *
   * 如果派生类持有任何应在 Run() 再次安全运行之前清除或修改的持久状态，
   * 则应重写此方法。默认实现为空。
   */
  virtual void Reset() {}

  /**
   * @brief 取消任务。
   *
   * 向任务发送一个信号，使其尽快停止其当前操作。
   * **重要提示：** 始终直接调用此方法或通过 Qt::DirectConnection 连接信号槽，
   * 否则取消请求可能会在任务已经完成后才被排队处理，从而导致取消无效。
   * 此方法内部调用基类 CancelableObject::Cancel()。
   */
  void Cancel() { CancelableObject::Cancel(); }

 protected:
  /**
   * @brief (纯虚方法) 派生类必须实现此方法以定义任务的具体执行逻辑。
   *
   * 这是任务的主要工作函数，在调用 Start() 后会在一个新的线程中执行。
   * @return 如果任务成功完成，则返回 true；如果任务执行过程中发生错误，则返回 false。
   */
  virtual bool Run() = 0;

  /**
   * @brief 设置错误消息。
   *
   * 建议在您的 Run() 函数返回 false 时调用此方法，以向用户说明失败的原因。
   * @param s 要设置的错误信息字符串。
   */
  void SetError(const QString& s) { error_ = s; }

  /**
   * @brief 设置任务的标题。
   *
   * 此标题用于在用户界面的任务管理器中区分不同的任务。
   * 通常，标题应在构造函数中设置，并且在任务的生命周期内不需要更改。
   * 要显示错误消息，建议改用 SetError() 方法。
   * @param s 要设置的任务标题字符串。
   */
  void SetTitle(const QString& s) { title_ = s; }

 signals:
  /**
   * @brief 当任务开始执行时发射此信号。
   * @param start_time 任务开始的时间戳 (自纪元以来的毫秒数)。
   */
  void Started(qint64 start_time);

  /**
   * @brief 每当任务取得进展时发射此信号。
   *
   * 在 Run() 方法的执行过程中应适时发射此信号，以更新任何附加的进度条（如 QProgressBar）
   * 来显示此任务的进度。
   * @param d 一个表示进度的浮点值，范围应在 0.0 (未开始) 到 1.0 (已完成) 之间。
   */
  void ProgressChanged(double d);

  /**
   * @brief 当任务完成时发射此信号（无论成功或失败）。
   * @warning **重要：** 在接收到此信号后，不要立即 `delete task`。
   * 如果需要删除任务对象，应调用 `task->deleteLater()` 以确保在安全的时机进行删除，
   * 避免在信号处理过程中对象被意外销毁。
   * @param task 指向已完成任务的指针。
   * @param succeeded 一个布尔值，指示任务是否成功完成 (即 Run() 方法是否返回 true)。
   */
  void Finished(Task* task, bool succeeded);

 private:
  QString title_;  ///< @brief 任务的标题，用于在用户界面中显示和区分任务。

  QString error_;  ///< @brief 存储任务执行失败时的错误信息。

  qint64 start_time_;  ///< @brief 任务开始执行的时间戳，以自纪元以来的毫秒数表示。
};

}  // namespace olive

#endif  // TASK_H // 结束预处理器指令 #ifndef TASK_H
