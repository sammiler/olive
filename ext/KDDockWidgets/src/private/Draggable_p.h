#ifndef KD_DRAGGABLE_P_H
#define KD_DRAGGABLE_P_H

#include "kddockwidgets/QWidgetAdapter.h" // 包含 QWidget 和 QQuickItem 的适配器类

#include <QVector> // Qt 动态数组容器

#include <memory> // C++ 标准库智能指针

namespace KDDockWidgets {

// 前向声明
class DockWidgetBase;       // 停靠小部件基类
class FloatingWindow;       // 浮动窗口类
class WidgetResizeHandler;  // 小部件大小调整处理器类
struct WindowBeingDragged;  // 正在被拖拽的窗口结构体

/**
 * @brief 代表一个可以被拖拽的对象。
 *
 * 例如标签页、已停靠的标题栏，甚至是带有原生操作系统标题栏的浮动窗口。
 *
 * \internal 这是一个内部类，不应在 KDDockWidgets 库外部直接使用。
 */
class DOCKS_EXPORT Draggable
{
public:
    /// @brief Draggable 指针的 QVector 类型定义，方便使用。
    typedef QVector<Draggable *> List;

    /**
     * @brief 构造函数。
     * @param thisWidget 指向此可拖拽对象关联的 QWidgetOrQuick 实例。
     * @param enabled 标记此对象是否默认启用拖拽，默认为 true。
     */
    explicit Draggable(QWidgetOrQuick *thisWidget, bool enabled = true);
    /**
     * @brief 虚析构函数。
     */
    virtual ~Draggable();
    /**
     * @brief 将此可拖拽对象作为 QWidgetOrQuick 返回。
     * @return 指向关联的 QWidgetOrQuick 实例的指针。
     */
    [[nodiscard]] QWidgetOrQuick *asWidget() const;

    /**
     * @brief 如果此可拖拽对象已经是一个窗口，则不执行任何操作。
     * 否则，它应该将其转换成一个窗口，因为拖拽操作通常会这样做。
     * @return 一个包含正在被拖拽的窗口信息的 std::unique_ptr。
     */
    virtual std::unique_ptr<WindowBeingDragged> makeWindow() = 0;

    /**
     * @brief 返回在点 @p p 处是否可拖拽。
     *
     * 因为仅仅继承自 Draggable 并不意味着可以在任何地方点击来拖拽。
     * @param p 鼠标按下的点坐标。
     * @return 如果该位置可拖拽，则返回 true；否则返回 false。默认实现返回 true。
     */
    [[nodiscard]] virtual bool isPositionDraggable(QPoint p) const
    {
        Q_UNUSED(p) // 标记参数 p 未被使用，避免编译器警告
        return true;
    }

    /**
     * @brief 返回鼠标移动是否可以开始一个拖拽操作。
     * 默认实现仅检查移动的增量是否大于 QApplication::startDragDistance()。
     * @param pressPos 鼠标按下的初始位置。
     * @param globalPos 当前的全局鼠标位置。
     * @return 如果可以开始拖拽，则返回 true；否则返回 false。
     */
    [[nodiscard]] virtual bool dragCanStart(QPoint pressPos, QPoint globalPos) const;

    /**
     * @brief 设置一个小部件大小调整处理器。
     * @param w 指向 WidgetResizeHandler 实例的指针。
     */
    void setWidgetResizeHandler(WidgetResizeHandler *w);

    /**
     * @brief 如果此可拖拽对象仅包含一个停靠小部件，则返回它。
     * 否则返回 nullptr。
     *
     * 示例：此可拖拽对象是一个只有一个停靠小部件的浮动窗口。
     * 示例：此可拖拽对象是一个包含两个停靠小部件的标题栏 -> nullptr。
     * @return 指向单个 DockWidgetBase 的指针，如果不满足条件则返回 nullptr。
     */
    [[nodiscard]] virtual DockWidgetBase *singleDockWidget() const = 0;

    /**
     * @brief 返回此可拖拽对象是否为一个 MDI (多文档界面) 窗口，并且正在主窗口内部被拖拽。
     * @return 如果是 MDI 内部拖拽，则返回 true；否则返回 false。
     */
    [[nodiscard]] virtual bool isMDI() const = 0;

    /**
     * @brief 返回此可拖拽对象是否已经是一个窗口。
     *
     * 如果为 true，意味着拖拽将简单地移动现有窗口，不涉及取消停靠或从标签页中分离。
     * @return 如果已经是窗口，则返回 true；否则返回 false。
     */
    [[nodiscard]] virtual bool isWindow() const = 0;

    /**
     * @brief 将可拖拽对象鼠标区域坐标系中的给定点映射到窗口坐标系中的等效点，
     * 并返回映射后的坐标。
     *
     * TODO: (待办事项) 对于 TitleBar 也重写此方法可能是一个好主意。因为标题栏的 (0,0)
     * 大约等于浮动窗口的 (0,0)，所以差异不明显，但如果有更大的窗口边距，则可能会变得明显。
     * @param pos 在可拖拽对象鼠标区域坐标系中的点。
     * @return 在窗口坐标系中映射后的点。默认实现直接返回原点。
     */
    [[nodiscard]] virtual QPoint mapToWindow(QPoint pos) const
    {
        return pos;
    }

private:
    class Private;         ///< PIMPL (Private Implementation) 设计模式的私有实现类前向声明。
    Private *const d;     ///< 指向私有实现类的指针。
    Q_DISABLE_COPY(Draggable) ///< 禁止拷贝构造函数和拷贝赋值操作符。
};
} // namespace KDDockWidgets

#endif // KD_DRAGGABLE_P_H