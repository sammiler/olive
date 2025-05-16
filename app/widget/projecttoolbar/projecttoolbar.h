#ifndef PROJECTTOOLBAR_H
#define PROJECTTOOLBAR_H

#include <QLineEdit>     // Qt 单行文本输入框控件 (用于搜索框)
#include <QPushButton>   // Qt 按钮控件
#include <QWidget>       // Qt 控件基类

#include "common/define.h" // 项目通用定义

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QEvent;
// class QString;

namespace olive {

/**
 * @brief ProjectToolbar 类是一个工具栏，包含项目功能（新建/打开/保存）、
 * 编辑功能（撤销/重做 - 注：头文件中未直接显示撤销/重做按钮，但类注释提及）、
 * 一个搜索字段以及一个项目视图选择器（树状/图标/列表）。
 *
 * 此对象的信号可以连接到应用程序中的各种功能，以提供更好的用户体验。
 */
class ProjectToolbar : public QWidget {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针。
   */
  explicit ProjectToolbar(QWidget* parent);

  /**
   * @brief ViewType 枚举定义了项目浏览器支持的视图模式。
   */
  enum ViewType {
    TreeView, ///< 树状视图模式。
    ListView, ///< 列表视图模式。
    IconView  ///< 图标视图模式。
  };

 public slots:
  /**
   * @brief 设置当前的项目视图模式。
   *
   * 这会更新工具栏上视图切换按钮的选中状态。
   * @param type 要设置的 ViewType 枚举值。
   */
  void SetView(ViewType type);

 protected:
  /**
   * @brief 处理 Qt 的 changeEvent 事件。
   *
   * 主要用于响应 QEvent::LanguageChange 和 QEvent::ThemeChange (如果适用) 事件，
   * 以便在应用程序语言或主题设置更改时更新工具栏按钮的文本或图标。
   * @param event 指向 QEvent 对象的指针。
   */
  void changeEvent(QEvent*) override;

 signals:
  /**
   * @brief 当“新建项目”按钮被点击时发出此信号。
   */
  void NewClicked();
  /**
   * @brief 当“打开项目”按钮被点击时发出此信号。
   */
  void OpenClicked();
  /**
   * @brief 当“保存项目”按钮被点击时发出此信号。
   */
  void SaveClicked();

  /**
   * @brief 当搜索字段中的文本发生改变时发出此信号。
   * @param text 搜索字段中新的文本内容。
   */
  void SearchChanged(const QString&);

  /**
   * @brief 当用户点击按钮切换项目视图模式时发出此信号。
   * @param type 新选中的 ViewType。
   */
  void ViewChanged(ViewType type);

 private:
  /**
   * @brief 重新翻译工具栏中所有按钮的文本和工具提示。
   *
   * 此方法会在构造时以及语言更改时被调用。
   */
  void Retranslate();
  /**
   * @brief 更新工具栏中所有按钮的图标。
   *
   * 此方法会在构造时以及主题或图标集更改时被调用。
   */
  void UpdateIcons();

  QPushButton* new_button_;  ///< “新建项目”按钮。
  QPushButton* open_button_; ///< “打开项目”按钮。
  QPushButton* save_button_; ///< “保存项目”按钮。

  QLineEdit* search_field_; ///< 用于输入搜索文本的单行编辑框。

  QPushButton* tree_button_; ///< 用于切换到树状视图模式的按钮。
  QPushButton* list_button_; ///< 用于切换到列表视图模式的按钮。
  QPushButton* icon_button_; ///< 用于切换到图标视图模式的按钮。

 private slots:
  /**
   * @brief 当某个视图切换按钮（树状、列表、图标）被点击时调用的槽函数。
   *
   * 此函数会确定哪个按钮被点击，并发出 ViewChanged 信号。
   */
  void ViewButtonClicked();
};

}  // namespace olive

#endif  // PROJECTTOOLBAR_H
