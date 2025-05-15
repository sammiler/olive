

#ifndef CRASHHANDLERDIALOG_H
#define CRASHHANDLERDIALOG_H

#include <client/crash_report_database.h> // Crashpad 客户端库：崩溃报告数据库 (虽然在头文件中未直接使用其类型)
#include <QDialog>          // QDialog 基类
#include <QDialogButtonBox> // 标准对话框按钮盒 (可能在 .cpp 中用于按钮)
#include <QNetworkReply>    // 用于处理网络回复 (发送报告)
#include <QPushButton>      // 按钮控件
#include <QTextEdit>        // 文本编辑控件，用于显示摘要和报告
#include <QList>            // 为了 QList<QSslError>
#include <QSslError>        // SSL 错误信息类 (如果需要)
#include <QString>          // Qt 字符串类
#include <QByteArray>       // Qt 字节数组类
#include <QCloseEvent>      // Qt 关闭事件类


#include "common/define.h" // 可能包含一些通用定义

namespace olive {

/**
 * @brief 用于显示崩溃报告并允许用户发送报告的对话框类。
 *
 * 当应用程序发生崩溃并由 Crashpad 等系统捕获到报告后，
 *此类会创建一个对话框，向用户展示崩溃的简要信息和详细报告内容。
 * 用户可以选择是否将此报告发送到指定的服务器以供开发者分析。
 */
class CrashHandlerDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 CrashHandlerDialog 对象。
   * @param parent 父 QObject 对象 (通常是 QWidget*)，对话框将显示在其上。
   * @param report_path 指向崩溃报告文件（例如 .dmp 文件）的路径。
   * 如果此路径上的文件被 `FileWatcher` 监控，则可能是由 `FileWatcher` 触发创建此对话框。
   */
  CrashHandlerDialog(QObject* parent, const QString& report_path);
  // 析构函数通常默认为 default，或在 .cpp 中实现以确保 m_dialogButtonBox 等UI指针正确删除 (如果它们没有父对象)
  // ~CrashHandlerDialog() override;

 private:
  /**
   * @brief 设置对话框中GUI对象的启用/禁用状态。
   * 例如，在发送报告期间禁用按钮，发送完成后再启用。
   * @param e 如果为 true，则启用GUI对象；如果为 false，则禁用。
   */
  void SetGUIObjectsEnabled(bool e);

  /**
   * @brief 生成或准备要显示的崩溃报告内容。
   * 这可能涉及到读取 .dmp 文件，或者调用 Crashpad 工具来符号化堆栈跟踪等。
   */
  void GenerateReport();

  /**
   * @brief 获取用于符号化崩溃报告的符号文件的路径。
   * @return QString 符号文件的路径。
   */
  static QString GetSymbolPath(); // 通常需要依赖于构建和部署配置

  /**
   * @brief 用于显示崩溃摘要信息的文本编辑框。
   */
  QTextEdit* summary_edit_;

  /**
   * @brief 用于显示详细崩溃报告内容的文本编辑框。
   */
  QTextEdit* crash_report_;

  /**
   * @brief “发送报告”按钮。
   */
  QPushButton* send_report_btn_;

  /**
   * @brief “不发送”按钮 (或“关闭”按钮)。
   */
  QPushButton* dont_send_btn_;

  /**
   * @brief 存储崩溃报告文件的完整路径名。
   */
  QString report_filename_;

  /**
   * @brief 存储从崩溃报告文件读取的原始数据或处理后的报告数据。
   */
  QByteArray report_data_;

  /**
   * @brief 标记当前是否正在等待报告上传完成。
   * 用于防止重复发送或在发送过程中关闭对话框。
   */
  bool waiting_for_upload_;

 protected:
  /**
   * @brief 重写 QWidget::closeEvent() 以处理对话框关闭事件。
   * 可能需要在此处处理正在上传报告时用户尝试关闭对话框的情况。
   * @param e 指向 QCloseEvent 对象的指针。
   */
  void closeEvent(QCloseEvent* e) override;

 private slots:
  /**
   * @brief 当网络请求（用于上传崩溃报告）完成时调用的槽函数。
   * @param reply 指向 QNetworkReply 对象的指针，包含服务器的响应信息。
   */
  void ReplyFinished(QNetworkReply* reply);

  /**
   * @brief 处理 SSL/TLS 错误（如果在上传报告时发生）。
   * @param reply 指向相关的 QNetworkReply 对象的指针。
   * @param se 包含发生的 SSL 错误的 QList。
   */
  void HandleSslErrors(QNetworkReply* reply, const QList<QSslError>& se);

  /**
   * @brief 尝试查找并加载指定的崩溃报告文件。
   * 通常在对话框初始化时调用。
   */
  void AttemptToFindReport();

  /**
   * @brief 当外部进程（例如符号化工具）有数据输出到标准输出或标准错误时调用的槽函数。
   * 用于捕获和显示符号化过程的输出。
   */
  void ReadProcessHasData(); // 通常连接到 QProcess::readyReadStandardOutput/Error

  /**
   * @brief 当外部进程（例如符号化工具）完成时调用的槽函数。
   */
  void ReadProcessFinished(); // 通常连接到 QProcess::finished

  /**
   * @brief 发送错误报告到指定的服务器。
   * 此函数通常会在用户点击 "发送报告" 按钮时被调用。
   */
  void SendErrorReport();
};

}  // namespace olive

#endif  // CRASHHANDLERDIALOG_H