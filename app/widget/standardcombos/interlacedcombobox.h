#ifndef INTERLACEDCOMBOBOX_H
#define INTERLACEDCOMBOBOX_H

#include <QComboBox> // Qt 组合框控件基类
#include <QWidget>   // Qt 控件基类 (QComboBox 的基类)

#include "render/videoparams.h" // 视频参数定义 (包含 VideoParams::Interlacing 枚举)

namespace olive {

/**
 * @brief InterlacedComboBox 类是一个自定义的 QComboBox，用于选择视频的隔行扫描模式。
 *
 * 它在构造时会填充预定义的隔行扫描选项（逐行、顶场优先、底场优先），
 * 这些选项的顺序必须与 VideoParams::Interlacing 枚举中的定义相匹配。
 */
class InterlacedComboBox : public QComboBox {
  Q_OBJECT // Qt 元对象系统宏 (虽然此类中没有显式定义信号或槽，但基类 QComboBox 有)

 public:
  /**
   * @brief 构造函数。
   *
   * 初始化组合框，并添加代表不同隔行扫描模式的选项。
   * 选项的文本是可翻译的，其顺序对应 VideoParams::Interlacing 枚举。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit InterlacedComboBox(QWidget* parent = nullptr) : QComboBox(parent) {
    // 添加的这些项必须与 VideoParams 中的 Interlacing 枚举顺序匹配
    this->addItem(tr("None (Progressive)")); // 添加“无（逐行扫描）”选项
    this->addItem(tr("Top-Field First"));    // 添加“顶场优先”选项
    this->addItem(tr("Bottom-Field First")); // 添加“底场优先”选项
  }

  /**
   * @brief 获取当前选中的隔行扫描模式。
   *
   * 通过将组合框的当前索引静态转换为 VideoParams::Interlacing 枚举值来实现。
   * @return 返回 VideoParams::Interlacing 枚举值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] VideoParams::Interlacing GetInterlaceMode() const {
    // 将当前选中项的索引直接转换为 VideoParams::Interlacing 枚举类型
    // 这依赖于 addItem 的顺序与枚举值的顺序严格一致
    return static_cast<VideoParams::Interlacing>(this->currentIndex());
  }

  /**
   * @brief 根据给定的隔行扫描模式设置组合框的当前选中项。
   * @param mode 要设置的 VideoParams::Interlacing 枚举值。
   */
  void SetInterlaceMode(VideoParams::Interlacing mode) {
    // 直接使用枚举值（它应该对应于添加项时的索引）来设置当前选中项
    this->setCurrentIndex(mode);
  }
};

}  // namespace olive

#endif  // INTERLACEDCOMBOBOX_H
