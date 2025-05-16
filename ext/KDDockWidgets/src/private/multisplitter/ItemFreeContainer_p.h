#pragma once // 确保该头文件在一次编译中仅被包含一次


#include "kddockwidgets/docks_export.h" // 包含导出宏定义
#include "kddockwidgets/KDDockWidgets.h" // 包含 KDDockWidgets 库的主头文件
#include "Item_p.h" // 包含 Item 私有头文件，ItemFreeContainer 继承自 ItemContainer，而 ItemContainer 继承自 Item

namespace Layouting { // 布局相关的命名空间


/**
 * @brief ItemFreeContainer 类是一个支持其子部件占据任意位置的 Item 容器。
 *
 * 这与 ItemBoxContainer 不同，ItemBoxContainer 用于具有嵌套的默认/传统垂直/水平布局。
 * 这种自由布局可用于实现 MDI (多文档界面) 风格的窗口。
 * 子项的位置和大小由它们各自的 geometry() 决定，而不是由容器强制执行线性排列。
 */
class DOCKS_EXPORT_FOR_UNIT_TESTS ItemFreeContainer : public ItemContainer
{
public:
Q_OBJECT // 启用 Qt 元对象系统特性，如信号和槽
    public :
    /**
     * @brief 构造一个 ItemFreeContainer 对象。
     * @param hostWidget 托管此容器的 Widget。
     * @param parent 父 ItemContainer，如果为 nullptr 则表示此容器是顶级容器。
     */
    explicit ItemFreeContainer(Widget *hostWidget, ItemContainer *parent);

    /**
     * @brief 构造一个顶级的 ItemFreeContainer 对象 (没有父项)。
     * @param hostWidget 托管此容器的 Widget。
     */
    explicit ItemFreeContainer(Widget *hostWidget);

    /**
     * @brief 析构函数。
     *
     * 清理所有子项和相关资源。
     */
    ~ItemFreeContainer() override;

    /**
     * @brief 将一个停靠部件 (Item) 添加到容器内的指定局部位置。
     * @param item 要添加的 Item (代表一个停靠部件)。
     * @param localPt Item 在容器局部坐标系中的目标左上角位置。
     */
    void addDockWidget(Item *item, QPoint localPt);

    /**
     * @brief 从容器中移除所有子项。
     *
     * 子项将被销毁或根据实现进行处理。
     */
    void clear() override;

    /**
     * @brief 从容器中移除指定的子项。
     * @param item 要移除的子项。
     * @param hardRemove 如果为 true (默认)，则彻底删除该项；否则，可能只是将其从布局中移除但保留对象。
     */
    void removeItem(Item *, bool hardRemove = true) override;

    /**
     * @brief 恢复一个之前被移除或隐藏的子项到此容器中。
     *
     * 子项将被重新添加到容器的子项列表中，并可能需要重新设置其几何属性。
     * @param child 要恢复的子项。
     */
    void restore(Item *child) override;

    /**
     * @brief 当子项的最小尺寸发生变化时调用的回调函数。
     *
     * ItemFreeContainer 可能需要根据子项的最小尺寸变化来调整自身或其他子项的布局，
     * 但对于自由布局容器，此影响可能与 ItemBoxContainer 不同。
     * @param child 最小尺寸发生改变的子项。
     */
    void onChildMinSizeChanged(Item *child) override;

    /**
     * @brief 当子项的可见性状态发生变化时调用的回调函数。
     *
     * ItemFreeContainer 可能需要根据子项的可见性变化更新其内部状态或重新触发布局计算。
     * @param child 可见性状态发生改变的子项。
     * @param visible 子项新的可见性状态。
     */
    void onChildVisibleChanged(Item *child, bool visible) override;
};

} // namespace Layouting
