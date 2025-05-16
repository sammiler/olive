#ifndef PROJECTEXPLORERICONVIEWITEMDELEGATE_H
#define PROJECTEXPLORERICONVIEWITEMDELEGATE_H

#include <QStyledItemDelegate> // Qt 风格化项委托基类，用于自定义项的绘制和编辑
#include <QPainter>            // Qt 绘图类 (用于 paint 方法)
#include <QStyleOptionViewItem> // Qt 视图项的样式选项类 (用于 paint 和 sizeHint 方法)
#include <QModelIndex>         // Qt 模型索引类 (用于 paint 和 sizeHint 方法)
#include <QSize>               // Qt 尺寸类 (用于 sizeHint 返回类型)
#include <QObject>             // Qt 对象模型基类 (QStyledItemDelegate 的基类)


#include "common/define.h" // 项目通用定义

namespace olive {

/**
 * @brief ProjectExplorerIconViewItemDelegate 类是当 ProjectExplorer 处于图标视图时，用于绘制项目项的委托。
 *
 * 它继承自 QStyledItemDelegate，允许自定义每个项目（如文件夹、素材、序列等）
 * 在图标视图中的外观和尺寸。
 */
class ProjectExplorerIconViewItemDelegate : public QStyledItemDelegate {
  // Q_OBJECT // QStyledItemDelegate 继承自 QAbstractItemDelegate，后者继承自 QObject，因此 Q_OBJECT 宏通常是可选的，除非定义了新的信号或槽。遵循不修改原则。
public:
  /**
   * @brief 构造函数。
   * @param parent 父 QObject 指针，默认为 nullptr。
   */
  explicit ProjectExplorerIconViewItemDelegate(QObject *parent = nullptr);

  /**
   * @brief 返回给定模型索引处项目项的建议尺寸。
   *
   * 在图标视图中，这决定了每个图标及其文本标签所占用的空间大小。
   * @param option 视图项的样式选项。
   * @param index 项目的模型索引。
   * @return 返回项目项的建议 QSize。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
  /**
   * @brief 绘制给定模型索引处项目项的外观。
   *
   * 此函数负责实际绘制图标、文本标签以及任何其他视觉元素（如选中状态、缩略图等）。
   * @param painter 用于进行绘制操作的 QPainter 指针。
   * @param option 视图项的样式选项，包含状态（如选中、悬停）和矩形区域等信息。
   * @param index 要绘制项目项的模型索引。
   */
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

}  // namespace olive

#endif  // PROJECTEXPLORERICONVIEWITEMDELEGATE_H
