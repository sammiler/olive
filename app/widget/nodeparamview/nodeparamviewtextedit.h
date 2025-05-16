#ifndef NODEPARAMVIEWTEXTEDIT_H
#define NODEPARAMVIEWTEXTEDIT_H

#include <QPlainTextEdit>  // Qt 纯文本编辑控件
#include <QPushButton>     // Qt 按钮控件
#include <QWidget>         // Qt 控件基类

#include "common/define.h"  // 项目通用定义

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QString;
// class QTextCursor; // 在 setTextPreservingCursor 中使用

namespace olive {

/**
 * @brief NodeParamViewTextEdit 类是一个用于节点参数视图中的文本编辑控件。
 *
 * 它包含一个 QPlainTextEdit 用于文本输入，以及可选的按钮来
 * 触发外部文本编辑器（例如，一个更大的对话框或直接在查看器中编辑）。
 * 控件可以配置为“仅在查看器中编辑”模式，此时内部的文本框变为只读。
 */
class NodeParamViewTextEdit : public QWidget {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit NodeParamViewTextEdit(QWidget *parent = nullptr);

  /**
   * @brief 获取当前编辑框中的文本。
   * @return 返回文本内容的 QString。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QString text() const { return line_edit_->toPlainText(); }

  /**
   * @brief 设置是否仅在查看器中编辑模式。
   *
   * 如果设置为 true，内部的 QPlainTextEdit 将变为只读，
   * 并且“在查看器中编辑”按钮可能会变得更突出或成为唯一的编辑方式。
   * @param on 如果为 true，则启用“仅在查看器中编辑”模式；否则禁用。
   */
  void SetEditInViewerOnlyMode(bool on);

 public slots:
  /**
   * @brief 设置编辑框的文本内容。
   *
   * 在设置文本时会临时阻塞信号，以防止不必要的 textEdited 信号发射。
   * @param s 要设置的文本字符串。
   */
  void setText(const QString &s) {
    line_edit_->blockSignals(true);   // 阻塞信号，防止 setText 触发 textChanged
    line_edit_->setPlainText(s);      // 设置纯文本内容
    line_edit_->blockSignals(false);  // 解除信号阻塞
  }

  /**
   * @brief 设置文本内容，同时尽量保留光标的原始位置。
   * @param s 要设置的文本字符串。
   */
  void setTextPreservingCursor(const QString &s) {
    // 保存当前光标位置
    int cursor_pos = line_edit_->textCursor().position();

    // 设置文本 (调用上面的 setText 方法)
    this->setText(s);

    // 获取新的文本光标对象
    QTextCursor c = line_edit_->textCursor();
    c.setPosition(cursor_pos);     // 将光标位置设置回保存的位置 (如果新文本长度允许)
    line_edit_->setTextCursor(c);  // 应用新的光标
  }

 signals:
  /**
   * @brief 当编辑框中的文本被用户编辑后发出此信号。
   * @param text 编辑后的文本内容。
   */
  void textEdited(const QString &text);

  /**
   * @brief 当用户点击“在查看器中编辑”按钮时发出此信号。
   *
   * 请求外部（通常是查看器控件）处理此文本参数的编辑。
   */
  void RequestEditInViewer();

 private:
  QPlainTextEdit *line_edit_;  ///< 用于实际文本输入的纯文本编辑控件。

  QPushButton *edit_btn_;  ///< “编辑”按钮，通常用于打开一个更大的文本编辑对话框。

  QPushButton *edit_in_viewer_btn_;  ///< “在查看器中编辑”按钮。

 private slots:
  /**
   * @brief “编辑”按钮被点击时的槽函数。
   *
   * 通常会弹出一个文本编辑对话框。
   */
  void ShowTextDialog();

  /**
   * @brief 内部 QPlainTextEdit 控件的文本内容发生改变时调用的槽函数。
   *
   * 此函数会发出 textEdited 信号。
   */
  void InnerWidgetTextChanged();
};

}  // namespace olive

#endif  // NODEPARAMVIEWTEXTEDIT_H
