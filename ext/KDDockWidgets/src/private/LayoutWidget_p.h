#ifndef KDDOCKWIDGETS_LAYOUTWIDGET_P_H
#define KDDOCKWIDGETS_LAYOUTWIDGET_P_H

#pragma once // 保证此头文件在一次编译中仅被包含一次

#include "kddockwidgets/docks_export.h" // 导入导出宏定义
#include "kddockwidgets/KDDockWidgets.h" // KDDockWidgets 公共头文件，包含枚举和基本类型
#include "kddockwidgets/LayoutSaver.h" // 布局保存与恢复相关的类
#include "kddockwidgets/QWidgetAdapter.h" // QWidget 和 QQuickItem 的适配器类

#include <QList> // Qt 列表容器

// 前向声明内部布局引擎相关的类
namespace Layouting {
class Item; // 布局中的一个基本项
class ItemContainer; // 布局中可以包含其他项的容器
class Separator; // 分隔符类 (用于调整相邻项的大小)
class Widget_qwidget; // QWidget 在布局引擎中的包装类 (如果使用 QWidget)
}

namespace KDDockWidgets {

// 前向声明 KDDockWidgets 内部类
class MainWindowBase; // 主窗口基类
class FloatingWindow; // 浮动窗口类
class Frame; // 框架类 (容纳 DockWidgetBase)
class DockWidgetBase; // 停靠小部件基类

/**
 * @brief 持有停靠小部件布局的界面小部件 (QWidget 或 QQuickItem)。
 *
 * 通常这只是 MultiSplitter，但我们引入了这个基类以支持不同的布局，
 * 例如 MDI (多文档界面) 布局，它们与传统的停靠小部件布局有很大不同。
 *
 * 这个类在 GUI 世界 (QWidget) 和 Layouting::Item 世界之间架起了桥梁。
 * 例如，它适合作为主窗口的中央小部件。实际的布局则由根 Item (m_rootItem) 完成。
 * 它继承自 LayoutGuestWidget (来自 multisplitter 模块)，表明它可以被其父 MultiSplitter 管理。
 */
class DOCKS_EXPORT LayoutWidget : public LayoutGuestWidget
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数。
     * @param parent 父 QWidgetOrQuick 对象，默认为 nullptr。
     */
    explicit LayoutWidget(QWidgetOrQuick *parent = nullptr);
    /**
     * @brief 析构函数。
     */
    ~LayoutWidget() override;

    /**
     * @brief 返回此布局是否位于主窗口 (MainWindow) 中。
     * @param honourNesting 如果为 true，则将嵌套在 DropArea/MDIArea 中的 DropArea/MDIArea 计为主窗口内部。
     * 否则，仅考虑直接父子关系。
     * @return 如果在主窗口中，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isInMainWindow(bool honourNesting = false) const;

    /**
     * @brief 获取此布局所在的主窗口。
     * @param honourNesting 参数含义同 isInMainWindow。
     * @return 指向 MainWindowBase 的指针，如果不在主窗口中则返回 nullptr。
     */
    [[nodiscard]] MainWindowBase *mainWindow(bool honourNesting = false) const;

    /**
     * @brief 获取此布局所在的浮动窗口。
     * @return 指向 FloatingWindow 的指针，如果不在浮动窗口中则返回 nullptr。
     */
    [[nodiscard]] FloatingWindow *floatingWindow() const;

    /**
     * @brief 返回布局的最小尺寸。
     * @sa setLayoutMinimumSize
     * @return 布局的最小 QSize。
     */
    [[nodiscard]] QSize layoutMinimumSize() const;

    /**
     * @brief 返回布局的最大尺寸提示。
     * @return 布局的最大 QSize 提示。
     */
    [[nodiscard]] QSize layoutMaximumSizeHint() const;

    /**
     * @brief 返回内容的宽度。
     * 通常与各自的父 MultiSplitter 的宽度相同。
     * @return 内容宽度。
     */
    [[nodiscard]] int width() const
    {
        return size().width();
    }

    /**
     * @brief 返回内容的高度。
     * 通常与各自的父 MultiSplitter 的高度相同。
     * @return 内容高度。
     */
    [[nodiscard]] int height() const
    {
        return size().height();
    }

    /**
     * @brief 获取尺寸的 getter 方法。
     * @return 当前 LayoutWidget 的 QSize。
     */
    [[nodiscard]] QSize size() const;

    /**
     * @brief 运行一些健全性检查。如果一切正常则返回 true。
     * @return 如果布局健全，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool checkSanity() const;

    /**
     * @brief 清除布局，移除所有内容。
     */
    void clearLayout();

    /**
     * @brief 将布局结构转储到 stderr (标准错误输出)。
     * 用于调试目的。
     */
    void dumpLayout() const;

    /**
     * @brief 设置内容的尺寸。
     * “内容尺寸”就是此布局的 size()。然而，由于调整 QWidget 的大小是异步的，
     * 而我们需要它是同步的。因为有时添加小部件会增加 MultiSplitter 的尺寸
     * （由于小部件的最小尺寸约束）。
     * @param newSize 新的布局尺寸。
     */
    void setLayoutSize(QSize newSize);


    /**
     * @brief 将停靠小部件 @p dw 恢复到其先前在布局项 @p item 中的占位符位置。
     * @param dw 要恢复的 DockWidgetBase。
     * @param item 包含占位符的目标布局项。
     * @param tabIndex 在目标布局项的标签页中的索引。
     */
    void restorePlaceholder(DockWidgetBase *dw, Layouting::Item *item, int tabIndex);

    /**
     * @brief 获取此布局中的项 (Item) 列表。
     * @return Layouting::Item 指针的 QVector。
     */
    [[nodiscard]] QVector<Layouting::Item *> items() const;

    /**
     * @brief 返回此布局是否包含指定的项 (Item)。
     * @param item 要检查的布局项。
     * @return 如果包含，则返回 true；否则返回 false。
     */
    bool containsItem(const Layouting::Item *item) const;

    /**
     * @brief  返回此布局是否包含指定的框架 (Frame)。
     * @param frame 要检查的框架。
     * @return 如果包含，则返回 true；否则返回 false。
     */
    bool containsFrame(const Frame *frame) const;

    /**
     * @brief 返回此布局中 Item 对象的数量。
     * 这也包括不可见（占位符）的 Item。
     * @sa visibleCount
     * @return Item 总数。
     */
    [[nodiscard]] int count() const;

    /**
     * @brief 返回此布局中可见 Item 的数量。
     * 即 @ref count 减去 @ref placeholderCount。
     * @sa count
     * @return 可见 Item 数量。
     */
    [[nodiscard]] int visibleCount() const;

    /**
     * @brief 返回此布局中占位符项 (placeholder Item) 的数量。
     * 等同于 @ref count 减去 @ref visibleCount。
     * @sa count, visibleCount
     * @return 占位符 Item 数量。
     */
    [[nodiscard]] int placeholderCount() const;

    /**
     * @brief 返回此布局中持有 @p frame 的 Item。
     * @param frame 要查找其对应 Item 的框架。
     * @return 指向 Layouting::Item 的指针，如果未找到则返回 nullptr。
     */
    Layouting::Item *itemForFrame(const Frame *frame) const;

    /**
     * @brief 返回此布局中包含的 Frame 对象列表。
     * @return Frame 指针的 QList。
     */
    [[nodiscard]] QList<Frame *> frames() const;

    /**
     * @brief 返回此布局中包含的停靠小部件 (DockWidgetBase) 列表。
     * @return DockWidgetBase 指针的 QVector。
     */
    [[nodiscard]] QVector<DockWidgetBase *> dockWidgets() const;

    /**
     * @brief 从其父 MultiSplitter 中移除一个项 (Item)。
     * @param item 要移除的布局项。
     */
    static void removeItem(Layouting::Item *item);

    /**
     * @brief 更新此布局的最小尺寸约束。
     */
    void updateSizeConstraints();

    /**
     * @brief 从保存的布局数据中反序列化 MultiSplitter 的状态到此 LayoutWidget。
     * @param data 包含 MultiSplitter 布局信息的 LayoutSaver::MultiSplitter 对象。
     * @return 如果反序列化成功则返回 true，否则返回 false。
     */
    virtual bool deserialize(const LayoutSaver::MultiSplitter &data);
    /**
     * @brief 将当前 LayoutWidget (通常代表一个 MultiSplitter) 的状态序列化为布局数据。
     * @return 包含 MultiSplitter 布局信息的 LayoutSaver::MultiSplitter 对象。
     */
    [[nodiscard]] LayoutSaver::MultiSplitter serialize() const;

    /**
     * @brief 处理关闭事件。
     * @param event 关闭事件对象。
     */
    void onCloseEvent(QCloseEvent *event) override;

protected:
    /**
     * @brief 设置此 LayoutWidget 的根布局项。
     * @param root 指向作为根的 Layouting::ItemContainer 对象。
     */
    void setRootItem(Layouting::ItemContainer *root);
    /**
     * @brief 设置布局的最小尺寸。
     * @sa layoutMinimumSize
     * @param minSize 新的最小尺寸。
     */
    void setLayoutMinimumSize(QSize minSize);

    /**
     * @brief 当布局需要重新计算时由布局引擎调用 (继承自 LayoutGuestWidget)。
     */
    void onLayoutRequest() override;
    /**
     * @brief 当小部件大小改变时由布局引擎调用 (继承自 LayoutGuestWidget)。
     * @param newSize 新的尺寸。
     * @return 如果接受新尺寸则返回 true。
     */
    bool onResize(QSize newSize) override;

    /**
     * @brief 在添加新框架时移除不需要的占位符项。
     *
     * 一个浮动框架 A 可能在主窗口中有一个占位符（例如，记住它在左侧的位置），
     * 但用户随后可能将其附加到右侧，因此左侧的占位符不再需要。
     * 在将框架添加到右侧之前，我们会移除左侧的占位符，否则它会在我们添加时被取消引用，
     * 从而导致段错误。所以这个函数的作用是让取消引用的操作提前一点发生。
     * @param framesBeingAdded 正在被添加到此布局的框架列表。
     */
    void unrefOldPlaceholders(const QList<Frame *> &framesBeingAdded) const;

    /**
     * @brief 返回包含在 @p frameOrMultiSplitter 中的框架列表。
     * 如果 frameOrMultiSplitter 是一个 Frame，则返回一个包含该框架的单元素列表。
     * 如果 frameOrMultiSplitter 是一个 MultiSplitter，则返回它包含的所有框架的列表。
     * @param frameOrMultiSplitter 指向 Frame 或 MultiSplitter (作为 QWidgetOrQuick) 的指针。
     * @return Frame 指针的 QList。
     */
    static QList<Frame *> framesFrom(QWidgetOrQuick *frameOrMultiSplitter);

Q_SIGNALS:
    /**
     * @brief 当可见小部件的数量发生变化时发射此信号。
     * @param count 新的可见小部件数量。
     */
    void visibleWidgetCountChanged(int count);

private:
    bool m_inResizeEvent = false; ///< 标记当前是否正在处理 resizeEvent，防止重入。
    Layouting::ItemContainer *m_rootItem = nullptr; ///< 指向此 LayoutWidget 的根布局项容器。
};

} // namespace KDDockWidgets

#endif // KDDOCKWIDGETS_LAYOUTWIDGET_P_H