#ifndef KD_DROP_AREA_P_H
#define KD_DROP_AREA_P_H

#include "kddockwidgets/docks_export.h"    // 导入导出宏定义
#include "kddockwidgets/KDDockWidgets.h" // KDDockWidgets 公共头文件，包含枚举和基本类型

#include "MultiSplitter_p.h"                  // MultiSplitter 私有头文件
#include "DropIndicatorOverlayInterface_p.h" // 拖放指示器覆盖层接口私有头文件

// 前向声明，用于测试目的
class TestDocks;

namespace KDDockWidgets {

// 前向声明
class Frame;                 // Frame 类，通常包装一个或多个 DockWidgetBase
class Draggable;             // 可拖拽对象接口类
struct WindowBeingDragged;   // 正在被拖拽的窗口结构体

/**
 * @brief 一个带有悬停时拖放指示器支持的 MultiSplitter。
 *
 * DropArea 继承自 MultiSplitter，并增加了处理拖放操作的功能，
 * 例如显示指示器、接收放置的小部件等。
 */
class DOCKS_EXPORT DropArea : public MultiSplitter
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数。
     * @param parent 父 QWidgetOrQuick 对象。
     * @param isMDIWrapper 标记此 DropArea 是否是为 MDI (多文档界面) 嵌套停靠创建的包装器，默认为 false。
     */
    explicit DropArea(QWidgetOrQuick *parent, bool isMDIWrapper = false);
    /**
     * @brief 析构函数。
     */
    ~DropArea() override;

    /**
     * @brief 移除当前的悬停指示效果。
     *
     * 当拖拽物离开此 DropArea 或者拖拽取消时调用。
     */
    void removeHover();
    /**
     * @brief 处理拖拽窗口在指定全局位置悬停时的逻辑。
     *
     * 计算并显示适当的拖放指示器。
     * @param draggedWindow 指向正在被拖拽的窗口的指针。
     * @param globalPos 当前的全局鼠标位置。
     * @return 计算得到的放置位置 (DropLocation)。
     */
    DropLocation hover(WindowBeingDragged *draggedWindow, QPoint globalPos);
    /**
     * @brief 当用户通过拖放操作放置一个小部件时调用。
     * @param droppedWindow 指向被放置的窗口的指针。
     * @param globalPos 放置时的全局鼠标位置。
     * @return 如果放置成功，则返回 true；否则返回 false。
     */
    bool drop(WindowBeingDragged *droppedWindow, QPoint globalPos);
    /**
     * @brief 获取此 DropArea 中包含的所有 Frame 列表。
     * @return Frame 指针的列表。
     */
    [[nodiscard]] Frame::List frames() const;

    /**
     * @brief 获取中央框架 (如果存在)。
     * @return 指向中央框架的布局项的指针，如果没有中央框架则返回 nullptr。
     */
    [[nodiscard]] Layouting::Item *centralFrame() const;
    /**
     * @brief 获取拖放指示器覆盖层接口。
     * @return 指向 DropIndicatorOverlayInterface 实例的指针。
     */
    [[nodiscard]] DropIndicatorOverlayInterface *dropIndicatorOverlay() const
    {
        return m_dropIndicatorOverlay;
    }
    /**
     * @brief 将一个停靠小部件添加到此 DropArea。
     * @param dockWidget 要添加的停靠小部件。
     * @param location 停靠的位置。
     * @param relativeTo 相对于哪个停靠小部件进行停靠（如果适用）。
     * @param option 初始化选项，默认为空。
     */
    void addDockWidget(DockWidgetBase *dockWidget, KDDockWidgets::Location location,
                       DockWidgetBase *relativeTo, InitialOption option = {});

    /**
     * @brief 检查此 DropArea 是否包含指定的停靠小部件。
     * @param dockWidget 要检查的停靠小部件。
     * @return 如果包含，则返回 true；否则返回 false。
     */
    bool containsDockWidget(DockWidgetBase *dockWidget) const;

    /**
     * @brief 返回此布局是否仅包含一个浮动的停靠小部件。
     *
     * 这意味着它位于一个 FloatingWindow 中，并且该 FloatingWindow 只有一个停靠小部件。
     * @return 如果满足条件，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool hasSingleFloatingFrame() const;

    /**
     * @brief 返回此放置区域是否只有一个框架 (Frame)。
     *
     * 更多解释请参见 FloatingWindow::hasSingleFrame()。
     * @return 如果只有一个框架，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool hasSingleFrame() const;

    /**
     * @brief 获取此 DropArea 的亲和性 (affinities) 列表。
     *
     * 亲和性用于控制哪些 DockWidget 可以停靠在此区域。
     * @return 亲和性名称的字符串列表。
     */
    [[nodiscard]] QStringList affinities() const;
    /**
     * @brief 使父容器中的指定停靠小部件及其同级元素等分布局。
     * @param dockWidget 触发此操作的停靠小部件。
     */
    void layoutParentContainerEqually(DockWidgetBase *dockWidget);

    /**
     * @brief 当使用 DockWidget::Option_MDINestable 选项时，已停靠的 MDI 停靠小部件将被包装在一个 DropArea 内，以便它们可以接受拖放。
     * 这个 DropArea 是在停靠时隐式创建的，此函数将返回 true 以表明这种情况。
     * @return 如果是 MDI 包装器，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isMDIWrapper() const;

    /**
     * @brief 返回用于实现 DockWidget::Option_MDINestable 功能的辅助停靠小部件。
     * @return 指向辅助 DockWidgetBase 的指针，如果不是 MDI 包装器则可能为 nullptr。
     */
    [[nodiscard]] DockWidgetBase *mdiDockWidgetWrapper() const;

private:
    Q_DISABLE_COPY(DropArea) ///< 禁止拷贝构造函数和拷贝赋值操作符。
    // 声明友元类，允许它们访问此类的私有成员
    friend class Frame;
    friend class ::TestDocks; // 测试类
    friend class DropIndicatorOverlayInterface;
    friend class AnimatedIndicators;
    friend class FloatingWindow;

    /**
     * @brief 验证拖拽物与目标框架的亲和性。
     * @tparam T 拖拽物的类型 (例如 WindowBeingDragged 或 DockWidgetBase)。
     * @param draggedItem 指向被拖拽的项的指针。
     * @param acceptingFrame 指向接收框架的指针，默认为 nullptr。
     * @return 如果亲和性匹配或无需验证，则返回 true；否则返回 false。
     */
    template<typename T>
    bool validateAffinity(T *draggedItem, Frame *acceptingFrame = nullptr) const;
    /**
     * @brief 处理拖拽窗口在特定框架和位置的放置操作。
     * @param draggedWindow 指向被拖拽窗口的指针。
     * @param acceptingFrame 指向接收拖放的框架的指针。
     * @param location 放置的位置。
     * @return 如果放置成功，则返回 true；否则返回 false。
     */
    bool drop(WindowBeingDragged *draggedWindow, Frame *acceptingFrame, DropLocation location);
    /**
     * @brief 处理 QWidgetOrQuick 在特定框架和位置的放置操作。
     * @param droppedWindow 指向被放置的 QWidgetOrQuick 的指针。
     * @param location 放置的位置。
     * @param relativeTo 指向作为参照物的框架的指针。
     * @return 如果放置成功，则返回 true；否则返回 false。
     */
    bool drop(QWidgetOrQuick *droppedWindow, KDDockWidgets::Location location, Frame *relativeTo);
    /**
     * @brief 获取包含指定全局位置的框架。
     * @param globalPos 全局鼠标位置。
     * @return 指向包含该位置的 Frame 的指针，如果找不到则返回 nullptr。
     */
    [[nodiscard]] Frame *frameContainingPos(QPoint globalPos) const;
    /**
     * @brief 更新所有子框架中停靠小部件的浮动动作 (QAction) 状态。
     */
    void updateFloatingActions() const;

    bool m_inDestructor = false; ///< 标记是否正在析构过程中，用于避免重复操作。
    const bool m_isMDIWrapper;   ///< 标记此 DropArea 是否是为 MDI 嵌套停靠创建的包装器。
    QString m_affinityName;      ///< 此 DropArea 的亲和性名称。
    DropIndicatorOverlayInterface *m_dropIndicatorOverlay = nullptr; ///< 指向拖放指示器覆盖层接口的指针。
};
} // namespace KDDockWidgets

#endif // KD_DROP_AREA_P_H