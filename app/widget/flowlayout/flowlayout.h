#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include <QLayout>  // Qt 布局基类
#include <QRect>    // Qt 矩形类
#include <QStyle>   // Qt 样式类，用于获取像素度量等

// 注意：这里不需要显式包含 QWidget、QLayoutItem、QSize、QList，
// 因为它们通常会被 QLayout 或 QStyle 间接包含，或者作为函数参数/返回值类型时，
// 只需要前向声明（但根据用户要求，不添加前向声明）。
// 实际项目中，如果编译器报错缺少定义，则应添加相应头文件。

// class QWidget; // Qt 控件基类 - 不添加
// class QLayoutItem; // Qt 布局项基类 - 不添加

/**
 * @brief FlowLayout 类是一个自定义布局，它将子项（控件）从左到右排列，
 * 当一行空间不足时会自动换到下一行。
 *
 * 类似于 HTML 中的浮动布局或文本段落的自动换行效果。
 * 用户可以指定子项之间的水平和垂直间距，以及布局的边距。
 */
class FlowLayout : public QLayout {
  // Q_OBJECT // FlowLayout 通常不需要 Q_OBJECT 宏，除非它定义了自己的信号或槽。
  // 如果确实需要，原始代码中应该已经存在。此处遵循不修改原则。

 public:
  /**
   * @brief 构造函数，指定父控件和可选的边距及间距。
   * @param parent 父控件指针。
   * @param margin 布局四周的边距。如果为 -1，则使用样式默认值。
   * @param hSpacing 子项之间的水平间距。如果为 -1，则使用样式默认值。
   * @param vSpacing 子项之间的垂直间距。如果为 -1，则使用样式默认值。
   */
  explicit FlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
  /**
   * @brief 构造函数，不指定父控件，但可以指定可选的边距及间距。
   * @param margin 布局四周的边距。如果为 -1，则使用样式默认值。
   * @param hSpacing 子项之间的水平间距。如果为 -1，则使用样式默认值。
   * @param vSpacing 子项之间的垂直间距。如果为 -1，则使用样式默认值。
   */
  explicit FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
  /**
   * @brief 析构函数。
   *
   * 会清空布局中的所有子项。
   */
  ~FlowLayout() override;

  /**
   * @brief 向布局中添加一个布局项。
   * @param item 要添加的 QLayoutItem 指针。
   */
  void addItem(QLayoutItem *item) override;
  /**
   * @brief 获取子项之间的水平间距。
   * @return 水平间距的像素值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] int horizontalSpacing() const;
  /**
   * @brief 获取子项之间的垂直间距。
   * @return 垂直间距的像素值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] int verticalSpacing() const;
  /**
   * @brief 返回布局在哪些方向上具有扩展性。
   *
   * FlowLayout 通常只在水平方向上扩展（填满宽度），垂直方向根据内容自适应。
   * @return 返回 Qt::Orientations 枚举值，指示扩展方向。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Qt::Orientations expandingDirections() const override;
  /**
   * @brief 检查布局是否具有“宽度确定高度”的特性。
   *
   * 对于 FlowLayout，其高度确实依赖于给定的宽度（因为宽度决定了如何换行）。
   * @return 如果高度依赖于宽度，则返回 true；否则返回 false。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool hasHeightForWidth() const override;
  /**
   * @brief 根据给定的宽度计算布局所需的高度。
   * @param width 给定的宽度。
   * @return 布局在此宽度下所需的高度。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] int heightForWidth(int width) const override;
  /**
   * @brief 获取布局中子项的数量。
   * @return 子项的数量。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] int count() const override;
  /**
   * @brief 获取指定索引处的布局项。
   * @param index 要获取的布局项的索引。
   * @return 返回对应索引的 QLayoutItem 指针；如果索引无效，则返回 nullptr。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QLayoutItem *itemAt(int index) const override;
  /**
   * @brief 计算并返回布局的最小尺寸。
   * @return 布局所需的最小 QSize。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QSize minimumSize() const override;
  /**
   * @brief 设置布局的几何区域，并在此区域内排列所有子项。
   * @param rect 布局可用的几何矩形区域。
   */
  void setGeometry(const QRect &rect) override;
  /**
   * @brief 计算并返回布局的建议尺寸（理想尺寸）。
   * @return 布局的建议 QSize。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QSize sizeHint() const override;
  /**
   * @brief 从布局中移除并返回指定索引处的布局项。
   *
   * 被移除的项的所有权将转移给调用者。
   * @param index 要移除的布局项的索引。
   * @return 返回被移除的 QLayoutItem 指针；如果索引无效，则返回 nullptr。
   */
  QLayoutItem *takeAt(int index) override;

 private:
  /**
   * @brief 执行实际的布局计算和子项排列。
   * @param rect 布局可用的几何矩形区域。
   * @param testOnly 如果为 true，则仅计算所需高度而不实际设置子项的几何形状。
   * @return 返回布局在此矩形区域内所需的高度。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] int doLayout(const QRect &rect, bool testOnly) const;
  /**
   * @brief 根据当前样式智能地获取水平或垂直间距。
   *
   * 如果构造函数中未指定间距（使用默认值 -1），则此函数会查询当前 QStyle
   * 以获取合适的默认间距值。
   * @param pm 要查询的像素度量类型 (例如 QStyle::PM_LayoutHorizontalSpacing)。
   * @return 计算得到的间距值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] int smartSpacing(QStyle::PixelMetric pm) const;

  QList<QLayoutItem *> itemList;  ///< 存储布局中所有 QLayoutItem 的列表。
  int m_hSpace;                   ///< 子项之间的水平间距。
  int m_vSpace;                   ///< 子项之间的垂直间距。
};

#endif  // FLOWLAYOUT_H
