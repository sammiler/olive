#ifndef CODECSTACK_H
#define CODECSTACK_H

#include <QStackedWidget>  // QStackedWidget 基类，用于管理多个页面并只显示一个
#include <QWidget>         // 为了 QWidget* parent 和 addWidget 的参数

// 假设这个类会与 CodecSection 或类似的类交互，但头文件中没有直接包含
// #include "codecsection.h"

namespace olive {

/**
 * @brief 用于根据选定的编解码器显示不同设置界面的堆叠控件类。
 *
 *此类继承自 QStackedWidget，专门用于在导出对话框等场景中，
 * 动态地展示与特定编解码器相关的配置选项区域 (通常是 CodecSection 的派生类)。
 * 当用户选择不同的编解码器时，此类可以切换到对应的设置页面。
 * 它还可能包含一些逻辑来响应当前显示页面的变化。
 */
class CodecStack : public QStackedWidget {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 CodecStack 对象。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit CodecStack(QWidget *parent = nullptr);

  // 默认析构函数通常足够
  // ~CodecStack() override;

  /**
   * @brief 向堆叠控件中添加一个新的编解码器设置页面 (widget)。
   *
   * 此方法重写或扩展了 QStackedWidget::addWidget() 的行为，
   * 可能在添加 widget 后会进行一些额外的初始化或连接操作。
   * @param widget 指向要添加的 QWidget (通常是 CodecSection 的派生类) 的指针。
   */
  void addWidget(QWidget *widget);  // Qt 5 中 addWidget 不是虚函数，所以这里不是 override

 signals:
  // 此处可以定义当 CodecStack 的状态（例如当前显示的页面）发生有意义的变化时发出的信号。
  // 例如：
  // /**
  //  * @brief 当 CodecStack 中显示的当前编解码器设置页面发生变化时发出此信号。
  //  * @param new_codec_section_widget 指向新显示的编解码器设置页面的指针。
  //  */
  // void currentCodecSectionChanged(QWidget* new_codec_section_widget);

 private slots:
  /**
   * @brief 当 QStackedWidget 的当前显示页面索引发生变化时调用的槽函数。
   *
   * 此槽通常连接到 QStackedWidget::currentChanged(int index) 信号。
   * 它可以用于在页面切换时执行一些操作，例如更新相关的UI元素或数据模型。
   * @param index 新显示的页面在堆叠控件中的索引。
   */
  void OnChange(int index);
};

}  // namespace olive

#endif  // CODECSTACK_H