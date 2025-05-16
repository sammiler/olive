#ifndef KD_MULTISPLITTER_RUBBERBAND_QUICK_H
#define KD_MULTISPLITTER_RUBBERBAND_QUICK_H

#ifdef KDDOCKWIDGETS_QTQUICK // 仅当定义了 KDDOCKWIDGETS_QTQUICK 宏时才编译以下内容 (表示在 Qt Quick 环境下)

#include "kddockwidgets/docks_export.h" // 包含导出宏定义，用于库的导出和导入
#include "Widget_quick.h" // 包含 Widget_quick.h，可能是 RubberBand 的基类或相关类

#include <QQuickItem> // 包含 QQuickItem 类，这是 Qt Quick 中所有可视化项的基类

namespace Layouting { // 布局相关的命名空间

// TODO: 完成橡皮筋 (RubberBand) 功能的实现
// 这是一个开发者注释，表明该类的功能可能尚未完全实现。
/**
 * @brief RubberBand 类代表一个用于 Qt Quick 环境的橡皮筋选择框。
 *
 * 此类继承自 QQuickItem，使其能够作为 Qt Quick 场景中的一个可视化元素。
 * 它同时也继承自 Layouting::Widget_quick，这可能意味着它与 KDDockWidgets 库的
 * 布局系统中的 Widget 概念相集成，或者作为其在 Qt Quick 中的适配器。
 *
 * 橡皮筋通常用于在拖放操作或选择操作中提供视觉反馈，显示一个可伸缩的矩形区域。
 * 这个特定实现是为 KDDockWidgets 库的 Qt Quick 版本设计的。
 */
class DOCKS_EXPORT_FOR_UNIT_TESTS RubberBand
    : public QQuickItem, // 继承自 QQuickItem，使其成为一个 Qt Quick 可视化项
      public Layouting::Widget_quick // 继承自 Layouting::Widget_quick，可能用于集成到布局系统中
{
Q_OBJECT // 启用 Qt 元对象系统特性，如信号和槽
    public :
    /**
     * @brief 构造一个 RubberBand 对象。
     * @param parent 指向父 Layouting::Widget 对象的指针。
     * 注意：虽然 RubberBand 是一个 QQuickItem，但其构造函数接收一个 Layouting::Widget 指针作为父对象，
     * 这表明它在逻辑上可能属于某个布局 Widget，即使它在 Qt Quick 场景图中有其自身的父 QQuickItem。
     */
    explicit RubberBand(Layouting::Widget *parent);

    /**
     * @brief 析构函数。
     *
     * 清理 RubberBand 对象创建的资源。
     */
    ~RubberBand() override;
};

} // namespace Layouting


#endif // KDDOCKWIDGETS_QTQUICK

#endif // KD_MULTISPLITTER_RUBBERBAND_QUICK_H
