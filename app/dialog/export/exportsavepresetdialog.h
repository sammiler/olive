#ifndef EXPORTSAVEPRESETDIALOG_H
#define EXPORTSAVEPRESETDIALOG_H

#include <QDialog>     // QDialog 基类
#include <QLineEdit>   // 单行文本输入框，用于预设名称
#include <QListWidget> // 列表控件 (虽然包含，但在此类声明中未直接作为成员使用，可能用于显示现有预设以避免重名)
#include <QWidget>     // 为了 QWidget* parent 参数
#include <QString>     // 为了 GetSelectedPresetName() 返回值

// 假设 encoder.h 声明了 EncodingParams 类
#include "codec/encoder.h" // 包含 EncodingParams

namespace olive {

/**
 * @brief 用于保存导出设置预设的对话框类。
 *
 * 此对话框允许用户为当前的导出参数 (EncodingParams) 指定一个名称，
 * 并将其保存为一个预设，以便将来可以快速加载和使用这些设置。
 */
class ExportSavePresetDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 ExportSavePresetDialog 对象。
   * @param p 要保存为预设的编码参数 (EncodingParams) 对象。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit ExportSavePresetDialog(EncodingParams p, QWidget *parent = nullptr);

  // 默认析构函数通常足够，因为 Qt 的父子对象机制会处理 name_edit_ (如果它有父对象)
  // ~ExportSavePresetDialog() override;

  /**
   * @brief 获取用户在对话框中输入的预设名称。
   * @return QString 用户输入的预设名称。
   */
  [[nodiscard]] QString GetSelectedPresetName() const { return name_edit_->text(); }

public slots:
 /**
  * @brief 重写 QDialog::accept() 槽函数。
  * 当用户点击“确定”或等效按钮以确认保存预设时调用。
  * 此函数通常会获取用户输入的预设名称，并使用 `params_` 成员中存储的
  * 编码参数来实际保存预设文件。
  */
 void accept() override;

private:
  /**
   * @brief 指向 QLineEdit 对象的指针，用于让用户输入预设的名称。
   */
  QLineEdit *name_edit_;

  /**
   * @brief 存储从构造函数传入的、要保存为预设的编码参数。
   */
  EncodingParams params_;
};

}  // namespace olive

#endif  // EXPORTSAVEPRESETDIALOG_H