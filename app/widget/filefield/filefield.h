#ifndef FILEFIELD_H
#define FILEFIELD_H

#include <QLineEdit>    // Qt 单行文本输入框控件
#include <QPushButton>  // Qt 按钮控件
#include <QWidget>      // Qt 控件基类 (作为 FileField 的基类)

// 前向声明 Qt 类
class QString;  // Qt 字符串类

namespace olive {

/**
 * @brief FileField 类是一个自定义控件，用于选择文件或目录路径。
 *
 * 它由一个 QLineEdit (用于显示和编辑路径) 和一个 QPushButton (用于打开文件/目录浏览对话框) 组成。
 * 可以配置为文件选择模式或目录选择模式。
 * 当路径改变时，会发出 FilenameChanged 信号。
 */
class FileField : public QWidget {
 Q_OBJECT  // Qt 元对象系统宏，用于支持信号和槽机制

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit FileField(QWidget* parent = nullptr);

  /**
   * @brief 获取当前在行编辑框中显示的文件名（或目录路径）。
   * @return 返回包含文件名/路径的 QString。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QString GetFilename() const { return line_edit_->text(); }

  /**
   * @brief 设置行编辑框中显示的文件名（或目录路径）。
   * @param s 要设置的文件名/路径字符串。
   */
  void SetFilename(const QString& s) { line_edit_->setText(s); }

  /**
   * @brief 设置行编辑框的占位提示文本。
   * @param s 要设置的占位文本字符串。
   */
  void SetPlaceholder(const QString& s) { line_edit_->setPlaceholderText(s); }

  /**
   * @brief 设置控件的工作模式（文件选择或目录选择）。
   * @param e 如果为 true，则设置为目录选择模式；否则为文件选择模式。
   */
  void SetDirectoryMode(bool e) { directory_mode_ = e; }

 signals:
  /**
   * @brief 当行编辑框中的文件名（或目录路径）发生改变时发出此信号。
   * @param filename 新的文件名/路径字符串。
   */
  void FilenameChanged(const QString& filename);

 private:
  QLineEdit* line_edit_;     ///< 用于显示和编辑文件/目录路径的单行文本输入框。
  QPushButton* browse_btn_;  ///< “浏览...”按钮，用于打开文件或目录选择对话框。
  bool directory_mode_;      ///< 标记控件当前是否处于目录选择模式 (true) 或文件选择模式 (false)。

 private slots:
  /**
   * @brief “浏览...”按钮点击事件的槽函数。
   *
   * 此函数会根据 directory_mode_ 的状态打开相应的文件或目录选择对话框，
   * 并将用户选择的路径设置到行编辑框中。
   */
  void BrowseBtnClicked();

  /**
   * @brief 行编辑框文本改变事件的槽函数。
   *
   * 当行编辑框中的文本发生改变时，此函数会被调用，
   * 并发出 FilenameChanged 信号。
   * @param text 行编辑框中新的文本内容。
   */
  void LineEditChanged(const QString& text);
};

}  // namespace olive

#endif  // FILEFIELD_H
