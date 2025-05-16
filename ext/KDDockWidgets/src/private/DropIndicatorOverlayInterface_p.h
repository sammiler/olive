#ifndef KD_DROPINDICATOROVERLAYINTERFACE_P_H
#define KD_DROPINDICATOROVERLAYINTERFACE_P_H

#include "kddockwidgets/docks_export.h"    // 导入导出宏定义
#include "kddockwidgets/QWidgetAdapter.h"  // QWidget 和 QQuickItem 的适配器类
#include "Frame_p.h"                       // Frame 私有头文件
#include "kddockwidgets/KDDockWidgets.h" // KDDockWidgets 公共头文件，包含枚举和基本类型

namespace KDDockWidgets {

// 前向声明
class DropArea; // 放置区域类

/**
 * @brief 拖放指示器覆盖层的接口类。
 *
 * 这是一个抽象基类，定义了不同类型的拖放指示器（例如经典箭头样式、分段样式等）
 * 需要实现的通用行为和属性。它通常作为一个覆盖层显示在 DropArea 之上，
 * 向用户显示可以将拖拽对象放置在何处。
 */
class DOCKS_EXPORT DropIndicatorOverlayInterface : public QWidgetAdapter
{
    Q_OBJECT
    /// @brief Q_PROPERTY 宏，暴露 hoveredFrameRect 属性给 Qt 元对象系统，表示当前悬停的框架矩形区域。
    Q_PROPERTY(QRect hoveredFrameRect READ hoveredFrameRect NOTIFY hoveredFrameRectChanged)
    /// @brief Q_PROPERTY 宏，暴露 currentDropLocation 属性给 Qt 元对象系统，表示当前计算出的拖放位置。
    Q_PROPERTY(KDDockWidgets::DropLocation currentDropLocation READ currentDropLocation NOTIFY currentDropLocationChanged)
public:
    /**
     * @brief 构造函数。
     * @param dropArea 指向此覆盖层所属的 DropArea。
     */
    explicit DropIndicatorOverlayInterface(DropArea *dropArea);
    /**
     * @brief 设置当前鼠标悬停在其上的框架 (Frame)。
     * @param frame 指向悬停的 Frame 对象。
     */
    void setHoveredFrame(Frame *frame);
    /**
     * @brief 设置是否有窗口正在被拖拽并悬停在此区域。
     * @param hovering 如果为 true，表示有窗口正在悬停；否则为 false。
     */
    void setWindowBeingDragged(bool hovering);
    /**
     * @brief 获取当前悬停的框架的矩形区域（在覆盖层坐标系中）。
     * @return 悬停框架的 QRect。
     */
    [[nodiscard]] QRect hoveredFrameRect() const;
    /**
     * @brief 检查当前是否有框架处于悬停状态。
     * @return 如果有框架被悬停，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isHovered() const;
    /**
     * @brief 获取当前计算出的拖放位置。
     * @return DropLocation 枚举值，表示可能的放置位置。
     */
    [[nodiscard]] DropLocation currentDropLocation() const;
    /**
     * @brief 获取当前鼠标悬停在其上的框架。
     * @return 指向悬停的 Frame 对象的指针。
     */
    [[nodiscard]] Frame *hoveredFrame() const
    {
        return m_hoveredFrame;
    }
    /**
     * @brief 设置当前的拖放位置。
     * @param location 要设置的 DropLocation。
     */
    void setCurrentDropLocation(DropLocation location);

    /**
     * @brief 根据全局鼠标位置计算并更新悬停状态和拖放指示器。
     * @param globalPos 当前的全局鼠标位置。
     * @return 计算得到的 DropLocation。
     */
    KDDockWidgets::DropLocation hover(QPoint globalPos);

    /**
     * @brief 清除并隐藏所有拖放指示器。
     *
     * 通常在拖拽结束或取消时调用。
     */
    void removeHover();

    /**
     * @brief 获取指定拖放位置的指示器的位置。
     *
     * 返回的是全局坐标。这是一个纯虚函数，子类必须实现。
     * @param location 指定的 DropLocation。
     * @return 指示器在全局坐标系中的 QPoint。
     */
    [[nodiscard]] virtual QPoint posForIndicator(DropLocation location) const = 0;

    /**
     * @brief 返回指定的拖放指示器是否应该可见。
     *
     * 默认实现通常基于当前是否有悬停的框架以及拖拽窗口的状态。
     * @param location 要检查的 DropLocation。
     * @return 如果指示器应该可见，则返回 true；否则返回 false。
     */
    [[nodiscard]] virtual bool dropIndicatorVisible(DropLocation location) const;

    /**
     * @brief 将 DropLocation 转换为 MultiSplitter 使用的 KDDockWidgets::Location。
     * @param dropLocation 要转换的 DropLocation。
     * @return 对应的 KDDockWidgets::Location。
     */
    static KDDockWidgets::Location multisplitterLocationFor(DropLocation dropLocation);

Q_SIGNALS:
    /**
     * @brief 当悬停的框架发生变化时发射此信号。
     * @param frame 指向新的悬停框架的指针，可能为 nullptr。
     */
    void hoveredFrameChanged(KDDockWidgets::Frame *frame);
    /**
     * @brief 当悬停框架的矩形区域发生变化时发射此信号。
     */
    void hoveredFrameRectChanged();
    /**
     * @brief 当当前的拖放位置发生变化时发射此信号。
     */
    void currentDropLocationChanged();

private:
    /**
     * @brief 当关联的框架被销毁时的槽函数。
     */
    void onFrameDestroyed();
    /**
     * @brief 设置悬停框架的矩形区域，并触发 hoveredFrameRectChanged 信号。
     * @param rect 新的悬停框架矩形。
     */
    void setHoveredFrameRect(QRect rect);

    QRect m_hoveredFrameRect; ///< 存储当前悬停框架的矩形区域。
    DropLocation m_currentDropLocation = DropLocation_None; ///< 当前计算出的拖放位置。

protected:
    /**
     * @brief 悬停逻辑的具体实现，由子类提供。
     * @param globalPos 当前的全局鼠标位置。
     * @return 计算得到的 DropLocation。
     */
    virtual DropLocation hover_impl(QPoint globalPos) = 0;
    /**
     * @brief 当悬停的框架发生变化时调用的虚函数，子类可以重写以执行特定操作。
     * @param frame 指向新的悬停框架的指针。
     */
    virtual void onHoveredFrameChanged(Frame *frame);
    /**
     * @brief 更新指示器的可见性，子类可以重写以实现自定义的可见性逻辑。
     */
    virtual void updateVisibility()
    {
    }

    Frame *m_hoveredFrame = nullptr; ///< 指向当前悬停的框架。
    DropArea *const m_dropArea;      ///< 指向此覆盖层所属的 DropArea。
    bool m_draggedWindowIsHovering = false; ///< 标记是否有窗口正在被拖拽并悬停在此区域。
};

} // namespace KDDockWidgets

#endif // KD_DROPINDICATOROVERLAYINTERFACE_P_H