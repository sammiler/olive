#ifndef PATHWIDGET_H
#define PATHWIDGET_H

#include <QLineEdit>     // Qt 单行文本输入框控件
#include <QPushButton>   // Qt 按钮控件
#include <QWidget>       // Qt 控件基类 (作为 PathWidget 的基类)

#include "common/define.h" // 项目通用定义

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QString;

namespace olive {

/**
 * @brief PathWidget 类是一个用于输入和选择文件或目录路径的自定义控件。
 *
 * 它由一个 QLineEdit (用于显示和编辑路径) 和一个 QPushButton (通常是“浏览...”按钮，
 * 用于打开文件或目录选择对话框) 组成。
 * 当路径文本框的内容改变时，会触发相应的处理。
 */
class PathWidget : public QWidget {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param path 控件初始化时显示的默认路径字符串。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit PathWidget(const QString& path, QWidget* parent = nullptr);

  /**
   * @brief 获取当前在路径编辑框中显示的文本。
   * @return 返回包含路径的 QString。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QString text() const { return path_edit_->text(); }

private slots:
 /**
  * @brief “浏览...”按钮点击事件的槽函数。
  *
  * 此函数会打开一个文件或目录选择对话框（具体行为可能在 .cpp 文件中定义，
  * 例如，是否区分文件和目录模式），并将用户选择的路径设置到路径编辑框中。
  */
 void BrowseClicked();

  /**
   * @brief 路径编辑框 (QLineEdit) 文本内容发生改变时的槽函数。
   *
   * 当用户手动编辑路径或通过浏览按钮选择了新路径后，此函数会被调用。
   * 可能会发出一个信号通知外部路径已更改（虽然此头文件中未明确声明该信号）。
   */
  void LineEditChanged();

private:
  QLineEdit* path_edit_;     ///< 用于显示和编辑文件或目录路径的单行文本输入框。
  QPushButton* browse_btn_;  ///< “浏览...”按钮，用于触发文件或目录选择对话框。
};

}  // namespace olive

#endif  // PATHWIDGET_H
