#ifndef COLORCODINGCOMBOBOX_H
#define COLORCODINGCOMBOBOX_H

#include <QComboBox>  // Qt 组合框控件基类

#include "widget/colorlabelmenu/colorlabelmenu.h"  // 引入颜色标签菜单类，可能用于自定义下拉列表的显示

namespace olive {

/**
 * @brief ColorCodingComboBox 类是一个用于选择颜色编码或颜色标签的自定义组合框。
 *
 * 它继承自 QComboBox，并可能与 ColorLabelMenu 配合使用，
 * 以提供一个带有颜色预览的下拉选项列表。
 * 用户可以通过这个组合框选择一个预定义的颜色。
 */
class ColorCodingComboBox : public QComboBox {
 Q_OBJECT  // Qt 元对象系统宏，用于支持信号和槽机制

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit ColorCodingComboBox(QWidget* parent = nullptr);

  /**
   * @brief 重写 QComboBox 的 showPopup 方法。
   *
   * 此方法在下拉列表即将显示时被调用。
   * 可能会在此处进行自定义的下拉列表构建或显示逻辑，
   * 例如，使用 ColorLabelMenu 来展示带有颜色样本的选项。
   */
  void showPopup() override;

  /**
   * @brief 设置当前选中的颜色。
   *
   * 根据提供的索引值来设置组合框的当前选中项。
   * @param index 要设置的颜色索引。
   */
  void SetColor(int index);

  /**
   * @brief 获取当前选中的颜色索引。
   * @return 返回当前选中颜色的索引值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] int GetSelectedColor() const { return index_; }

 private:
  int index_{};  ///< 存储当前选中颜色的索引。
};

}  // namespace olive

#endif  // COLORCODINGCOMBOBOX_H
