#ifndef KDDOCKWIDGETS_POSITION_P_H
#define KDDOCKWIDGETS_POSITION_P_H

#include "kddockwidgets/docks_export.h" // 导入导出宏定义

#include "Logging_p.h" // KDDockWidgets 内部日志相关的私有头文件
#include "kddockwidgets/LayoutSaver.h" // 布局保存与恢复相关的公共接口头文件
#include "kddockwidgets/QWidgetAdapter.h" // QWidget 和 QQuickItem 的适配器类

#include <QHash> // Qt 哈希表容器
#include <QPointer> // Qt QObject 指针的弱引用
#include <QScopedValueRollback> // Qt RAII 类，用于在作用域结束时自动恢复某个值

#include <memory> // C++ 标准库智能指针 (例如 std::unique_ptr, std::shared_ptr)
#include <vector> // C++ 标准库动态数组

// 前向声明内部布局引擎相关的类
namespace Layouting {
class Item; // 布局中的一个基本项
}

namespace KDDockWidgets {

// 前向声明
class LayoutWidget; // KDDockWidgets 布局小部件基类

/**
 * @brief 一个 RAII (Resource Acquisition Is Initialization) 类，用于管理对 Layouting::Item 的引用。
 *
 * 确保在 ItemRef 对象生命周期结束时，能够安全地处理与 Layouting::Item 相关的资源或连接。
 * 主要目的是通过 QMetaObject::Connection 管理信号槽连接，并在析构时断开连接，
 * 同时通过 QPointer 监视 Item 的生命周期，防止悬空指针。
 */
struct ItemRef
{
    /**
     * @brief 构造函数。
     * @param connection 与此 ItemRef 关联的 Qt 信号槽连接。
     * @param item 指向被引用的 Layouting::Item 对象。
     */
    explicit ItemRef(QMetaObject::Connection connection, Layouting::Item *item);
    /**
     * @brief 析构函数。
     *
     * 通常会在这里断开通过 connection 建立的信号槽连接。
     */
    ~ItemRef();

    /**
     * @brief 检查引用的布局项是否位于主窗口内。
     * @return 如果布局项有效且其所在的 LayoutWidget 位于主窗口内，则返回 true。
     */
    [[nodiscard]] bool isInMainWindow() const;

    Layouting::Item *const item; ///< 指向被引用的 Layouting::Item 对象的常量指针。
    const QPointer<Layouting::Item> guard; ///< 用于监视 item 是否被销毁的 QPointer。
    const QMetaObject::Connection connection; ///< 与此 Item 关联的 Qt 信号槽连接。

private:
    Q_DISABLE_COPY(ItemRef) ///< 禁止拷贝构造函数和拷贝赋值操作符。
};


// 前向声明
class DockWidgetBase; // 停靠小部件基类
class Frame; // 框架类 (容纳 DockWidgetBase)

/**
 * @internal
 * @brief 代表停靠小部件 (DockWidget) 的最后位置信息。
 *
 * 当停靠小部件关闭时，其位置会被保存；当它再次显示时，其位置会被恢复。
 * 此类持有该位置信息，包括它所在的布局项、标签页索引、是否浮动以及浮动时的几何形状等。
 */
class DOCKS_EXPORT_FOR_UNIT_TESTS Position // DOCKS_EXPORT_FOR_UNIT_TESTS 用于单元测试时导出
{
    Q_DISABLE_COPY(Position) ///< 禁止拷贝构造函数和拷贝赋值操作符。
public:
    /// @brief Position 对象的共享指针类型定义。
    typedef std::shared_ptr<Position> Ptr;

    /**
     * @brief 默认构造函数。
     */
    Position() = default;
    /**
     * @brief 析构函数。
     *
     * 会负责清理 m_placeholders 中的 ItemRef 对象。
     */
    ~Position();

    /**
     * @brief 从 LayoutSaver::Position 对象中反序列化（加载）位置信息。
     * @param savedPosition 包含已保存位置数据的对象。
     */
    void deserialize(const LayoutSaver::Position &savedPosition);
    /**
     * @brief 将当前位置信息序列化为 LayoutSaver::Position 对象。
     * @return 包含当前位置数据的 LayoutSaver::Position 对象。
     */
    [[nodiscard]] LayoutSaver::Position serialize() const;

    /**
     * @brief 返回此 Position 对象是否有效。如果无效，则表示关联的 DockWidget 从未在主窗口中出现过。
     * @return 如果 layoutItem() 返回非 nullptr，则表示有效，返回 true；否则返回 false。
     */
    [[nodiscard]] bool isValid() const
    {
        return layoutItem() != nullptr; // 有效性取决于是否存在关联的布局项
    }

    /**
     * @brief 返回停靠小部件最后是否位于一个标签页小部件中。
     * @return 如果最后是标签化的 (m_tabIndex != -1)，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isTabbed() const
    {
        return m_tabIndex != -1;
    }

    /// @brief 如果停靠小部件位于 TabWidget 中，则为其标签页索引；否则为 -1。
    int m_tabIndex = -1;

    /// @brief 如果停靠小部件在关闭时是浮动的，则为 true。
    bool m_wasFloating = false;

    /**
     * @brief 添加停靠小部件最后所在（或当前所在）的布局项作为占位符。
     * @param placeholder 指向要添加的 Layouting::Item 的指针。
     */
    void addPlaceholderItem(Layouting::Item *placeholder);

    /**
     * @brief 获取与此位置关联的主要布局项。
     * 通常是 m_placeholders 列表中的第一个有效项。
     * @return 指向 Layouting::Item 的指针，如果没有则返回 nullptr。
     */
    [[nodiscard]] Layouting::Item *layoutItem() const;

    /**
     * @brief 检查此 Position 是否包含指定的布局项作为占位符。
     * @param item 要检查的 Layouting::Item 指针。
     * @return 如果包含，则返回 true；否则返回 false。
     */
    bool containsPlaceholder(Layouting::Item *item) const;
    /**
     * @brief 清除所有占位符。
     */
    void removePlaceholders();

    /**
     * @brief 返回停靠小部件曾经所在或当前所在的所有位置（占位符）的列表。
     * @return 一个包含 ItemRef 唯一指针的 std::vector 引用。
     */
    [[nodiscard]] const std::vector<std::unique_ptr<ItemRef>> &placeholders() const
    {
        return m_placeholders;
    }

    /**
     * @brief 移除属于指定 MultiSplitter (LayoutWidget) 的所有占位符。
     * @param layoutWidget 指向要移除其占位符的 LayoutWidget。
     */
    void removePlaceholders(const LayoutWidget *layoutWidget);

    /**
     * @brief 移除所有引用浮动窗口 (FloatingWindow) 的占位符。
     * 即只保留那些位于主窗口 (MainWindow) 内的占位符。
     */
    void removeNonMainWindowPlaceholders();

    /**
     * @brief 移除指定的布局项 @p placeholder。
     * @param placeholder 指向要移除的 Layouting::Item 的指针。
     */
    void removePlaceholder(Layouting::Item *placeholder);

    /**
     * @brief 保存标签页索引和浮动状态。
     * @param tabIndex 标签页索引。
     * @param isFloating 是否浮动。
     */
    void saveTabIndex(int tabIndex, bool isFloating)
    {
        m_tabIndex = tabIndex;
        m_wasFloating = isFloating;
    }

    /**
     * @brief 设置最后一次作为浮动窗口时的几何形状。
     * @param geo 浮动时的 QRect。
     */
    void setLastFloatingGeometry(QRect geo)
    {
        m_lastFloatingGeometry = geo;
    }

    /**
     * @brief 返回在保存位置时，停靠小部件是否处于浮动状态。
     * @return 如果是浮动状态，则返回 true。
     */
    [[nodiscard]] bool wasFloating() const
    {
        return m_wasFloating;
    }

    /**
     * @brief 获取最后一次作为浮动窗口时的几何形状。
     * @return 浮动时的 QRect。
     */
    [[nodiscard]] QRect lastFloatingGeometry() const
    {
        return m_lastFloatingGeometry;
    }

    /**
     * @brief 获取最后一个关联的布局项 (等同于 layoutItem())。
     * @return 指向 Layouting::Item 的指针。
     */
    [[nodiscard]] Layouting::Item *lastItem() const
    {
        return layoutItem();
    }

    /**
     * @brief 获取最后一个标签页索引。
     * @return 标签页索引。
     */
    [[nodiscard]] int lastTabIndex() const
    {
        return m_tabIndex;
    }

    /**
     * @brief 获取在指定侧边栏位置最后一次作为覆盖层时的几何形状。
     * @param loc 侧边栏位置。
     * @return 覆盖层在该位置的 QRect。
     */
    [[nodiscard]] QRect lastOverlayedGeometry(SideBarLocation loc) const
    {
        return m_lastOverlayedGeometries.value(loc);
    }

    /**
     * @brief 设置在指定侧边栏位置最后一次作为覆盖层时的几何形状。
     * @param loc 侧边栏位置。
     * @param rect 覆盖层的 QRect。
     */
    void setLastOverlayedGeometry(SideBarLocation loc, QRect rect)
    {
        m_lastOverlayedGeometries[loc] = rect;
    }

private:
    // 声明友元函数，允许 QDebug 操作符访问私有成员
    friend inline QDebug operator<<(QDebug, const KDDockWidgets::Position::Ptr &);

    /// @brief 存储停靠小部件曾位于或当前位于的最后位置列表。
    /// 当调用 setFloating(false) 或 show() 时，可以用于恢复。
    /// 每个 ItemRef 包含一个指向布局项的指针及其相关的信号连接。
    std::vector<std::unique_ptr<ItemRef>> m_placeholders;
    /// @brief 最后一次作为浮动窗口时的几何形状。
    QRect m_lastFloatingGeometry;
    /// @brief 在各个侧边栏位置最后一次作为覆盖层时的几何形状。
    QHash<SideBarLocation, QRect> m_lastOverlayedGeometries;
    /// @brief 标记是否正在清除占位符，用于防止重入。
    bool m_clearing = false;
};

/**
 * @brief QDebug 操作符重载，用于方便地输出 Position 对象的智能指针信息。
 * @param d QDebug 对象。
 * @param p 指向 Position 对象的共享指针。
 * @return QDebug 对象，以便链式调用。
 */
inline QDebug operator<<(QDebug d, const KDDockWidgets::Position::Ptr &p)
{
    if (!p) // 如果指针为空
        return d;

    // 输出占位符列表的大小
    d << "; placeholdersSize=" << p->m_placeholders.size();
    return d;
}

} // namespace KDDockWidgets

#endif // KDDOCKWIDGETS_POSITION_P_H