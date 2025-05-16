#ifndef COLUMNEDGRIDLAYOUT_H
#define COLUMNEDGRIDLAYOUT_H

#include <QGridLayout> // Qt 网格布局基类
#include <QWidget>     // Qt 控件基类 (用于构造函数和 Add 方法的参数类型)

#include "common/define.h" // 项目通用定义

namespace olive {

/**
 * @brief ColumnedGridLayout 类是一个自定义的网格布局管理器。
 *
 * 它继承自 QGridLayout，并提供了一种基于指定的最大列数自动排列子控件的功能。
 * 当添加控件时，它会自动计算控件应放置的行和列，
 * 当达到最大列数时会自动换行。
 */
class ColumnedGridLayout : public QGridLayout {
  Q_OBJECT // Qt 元对象系统宏，用于支持信号和槽机制

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   * @param maximum_columns 布局中允许的最大列数。如果为 0 或负数，则可能表示没有限制或由其他逻辑处理。
   */
  explicit ColumnedGridLayout(QWidget* parent = nullptr, int maximum_columns = 0);

  /**
   * @brief 向布局中添加一个控件。
   *
   * 控件会被自动放置到下一个可用的单元格中，
   * 根据 maximum_columns_ 的设置，在达到最大列数时会自动换行。
   * @param widget 要添加到布局中的控件指针。
   */
  void Add(QWidget* widget);

  /**
   * @brief 获取当前设置的最大列数。
   * @return 返回最大列数。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] int MaximumColumns() const;

  /**
   * @brief 设置布局的最大列数。
   *
   * 修改此值后，后续添加的控件将按照新的列数限制进行排列。
   * 已存在的控件布局可能不会立即更新，除非重新布局。
   * @param maximum_columns 新的最大列数值。
   */
  void SetMaximumColumns(int maximum_columns);

private:
  int maximum_columns_; ///< 存储布局允许的最大列数。
};

}  // namespace olive

#endif  // COLUMNEDGRIDLAYOUT_H
