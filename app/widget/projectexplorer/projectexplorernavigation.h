#ifndef PROJECTEXPLORERLISTVIEWTOOLBAR_H  // 注意：头文件保护宏与文件名 (projectexplorernavigation.h)
                                          // 不完全匹配，可能应为 PROJECTEXPLORERNNAVIGATION_H
#define PROJECTEXPLORERLISTVIEWTOOLBAR_H

#include <QLabel>       // Qt 标签控件
#include <QPushButton>  // Qt 按钮控件
#include <QSlider>      // Qt 滑块控件
#include <QWidget>      // Qt 控件基类

#include "common/define.h"  // 项目通用定义

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QString;
// class QEvent;

namespace olive {

/**
 * @brief ProjectExplorerNavigation 类是 ProjectExplorer 图标视图和列表视图的导航栏控件。
 *
 * 与树视图不同，图标视图和列表视图不遵循信息的层级视图。
 * 这意味着在这些视图类型中没有直接导航进出文件夹的方法。我们通过两种方式解决这个问题：
 *
 * * 在这些视图中双击文件夹将进入该文件夹。
 * * 此导航栏提供一个“向上”按钮用于离开文件夹。
 *
 * 此导航栏还为这些视图提供了一个图标大小滑块（范围在 kProjectIconSizeMinimum 和
 * kProjectIconSizeMaximum 之间），以及一个旨在设置为当前文件夹名称（或根文件夹时为空）的文本。
 *
 * 此控件实际上不与 Project 或 ProjectExplorer 类通信。它仅仅是一些UI控件，
 * 旨在以某种方式连接起来以实现功能。这主要是 ProjectExplorer 的责任。
 *
 * 默认情况下，“向上”按钮是禁用的（假设在根文件夹），文本为空，图标大小滑块设置为 kProjectIconSizeDefault。
 */
class ProjectExplorerNavigation : public QWidget {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针。
      */
     explicit ProjectExplorerNavigation(QWidget* parent);

  /**
   * @brief 设置文本字符串。
   *
   * 此文本旨在设置为当前文件夹的名称。
   *
   * @param s 要设置的文本字符串。
   */
  void set_text(const QString& s);

  /**
   * @brief 设置“向上”按钮是否启用。
   *
   * @param e 如果为 true，则启用按钮；否则禁用。
   */
  void set_dir_up_enabled(bool e);

  /**
   * @brief 设置尺寸滑块的当前值。
   *
   * 注意：不会发出 SizeChanged() 信号。
   *
   * @param s
   *
   * 要设置的新的尺寸值。
   */
  void set_size_value(int s);

 signals:
  /**
   * @brief 当“向上”按钮被点击时发出此信号。
   */
  void DirectoryUpClicked();

  /**
   * @brief 当图标大小滑块的值改变时发出此信号。
   *
   * @param size
   *
   * 滑块中设置的新的尺寸值。
   */
  void SizeChanged(int size);

 protected:
  /**
   * @brief 处理 Qt 的 changeEvent 事件。
   *
   * 主要用于响应 QEvent::LanguageChange 和 QEvent::ThemeChange (如果适用) 事件，
   * 以便在应用程序语言或主题设置更改时更新控件的文本或图标。
   * @param event 指向 QEvent 对象的指针。
   */
  void changeEvent(QEvent*) override;

 private:
  /**
   * @brief 重新翻译控件中的所有文本（例如按钮提示、标签）。
   *
   * 此方法会在构造时以及语言更改时被调用。
   */
  void Retranslate();

  /**
   * @brief 更新控件中所有按钮的图标。
   *
   * 此方法会在构造时以及主题或图标集更改时被调用。
   */
  void UpdateIcons();

  QPushButton* dir_up_btn_;  ///< “向上”（返回上一级目录）按钮。
  QLabel* dir_lbl_;          ///< 用于显示当前目录名称的标签。
  QSlider* size_slider_;     ///< 用于调整图标大小的滑块。
};

}  // namespace olive

#endif  // PROJECTEXPLORERLISTVIEWTOOLBAR_H // 注意：头文件保护宏与文件名不完全匹配
