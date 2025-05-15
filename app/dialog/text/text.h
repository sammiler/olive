#ifndef RICHTEXTDIALOG_H // 文件保护宏，通常与文件名一致，但这里是 RICHTEXTDIALOG_H
#define RICHTEXTDIALOG_H

#include <QDialog>        // 引入 QDialog 类，对话框窗口的基类
#include <QFontComboBox>  // 引入 QFontComboBox 类，用于选择字体 (虽然在此文件中未直接使用，但可能在 .cpp 文件或相关UI中使用)
#include <QPlainTextEdit> // 引入 QPlainTextEdit 类，用于多行纯文本编辑

#include "common/define.h"             // 引入项目通用定义
#include "widget/slider/floatslider.h" // 引入浮点数滑块控件 (虽然在此文件中未直接使用)

namespace olive {

/**
 * @class TextDialog
 * @brief 简单的文本输入对话框类。
 *
 * 此对话框提供一个纯文本编辑区域，允许用户输入或编辑文本。
 * 它通常用于需要用户输入较长文本内容的场景。
 * 注意：文件名宏为 RICHTEXTDIALOG_H，但类名为 TextDialog。
 */
class TextDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief TextDialog 构造函数。
   *
   * @param start 对话框中文本编辑区域的初始文本内容。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit TextDialog(const QString& start, QWidget* parent = nullptr);

  /**
   * @brief 获取文本编辑区域中的当前文本。
   * @return [[nodiscard]] QString 用户输入的纯文本。
   * [[nodiscard]] 属性提示编译器调用者应该使用此函数的返回值。
   */
  [[nodiscard]] QString text() const { return text_edit_->toPlainText(); }

private:
  /**
   * @brief 指向纯文本编辑控件的指针。
   */
  QPlainTextEdit* text_edit_;
};

}  // namespace olive

#endif  // RICHTEXTDIALOG_H
