#ifndef KD_SIDEBAR_P_H
#define KD_SIDEBAR_P_H

#include "kddockwidgets/docks_export.h"    // 导入导出宏定义

#include "kddockwidgets/KDDockWidgets.h" // KDDockWidgets 公共头文件，包含枚举和基本类型
#include "kddockwidgets/QWidgetAdapter.h"  // QWidget 和 QQuickItem 的适配器类

#include <QVector> // Qt 动态数组容器，用于存储停靠小部件列表

namespace KDDockWidgets {

// 前向声明
class DockWidgetBase; // 停靠小部件基类
class MainWindowBase; // 主窗口基类

/**
 * @brief SideBar 类代表主窗口边缘的侧边栏区域。
 *
 * 侧边栏通常用于放置一些“锚定”的停靠小部件的按钮。当用户点击这些按钮时，
 * 对应的小部件可以作为覆盖层 (overlay) 显示在主窗口内容之上，
 * 或者从覆盖层状态隐藏回侧边栏。
 * SideBar 继承自 QWidgetAdapter，因此它既可以是 QWidget 也可以是 QQuickItem 的包装。
 */
class DOCKS_EXPORT SideBar : public QWidgetAdapter
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数。
     * @param location 此侧边栏在主窗口中的位置 (例如，North, East, West, South)。
     * @param parent 父 MainWindowBase 对象，默认为 nullptr。
     */
    explicit SideBar(SideBarLocation location, MainWindowBase *parent = nullptr);

    /**
     * @brief 向此侧边栏添加一个停靠小部件。
     *
     * 这通常意味着在侧边栏上创建一个代表该停靠小部件的按钮。
     * @param dw 要添加的 DockWidgetBase 对象。
     */
    void addDockWidget(DockWidgetBase *dw);
    /**
     * @brief 从此侧边栏移除一个停靠小部件。
     *
     * 这通常意味着移除侧边栏上代表该停靠小部件的按钮。
     * @param dw 要移除的 DockWidgetBase 对象。
     */
    void removeDockWidget(DockWidgetBase *dw);
    /**
     * @brief 检查此侧边栏是否包含指定的停靠小部件。
     * @param dw 要检查的 DockWidgetBase 对象。
     * @return 如果包含，则返回 true；否则返回 false。
     */
    bool containsDockWidget(DockWidgetBase *dw) const;

    /**
     * @brief 返回此侧边栏的朝向 (垂直或水平)。
     * @return Qt::Orientation 枚举值。
     */
    [[nodiscard]] Qt::Orientation orientation() const;

    /**
     * @brief 返回此侧边栏是否为垂直朝向。
     * @return 如果是垂直方向 (例如 West 或 East 侧边栏)，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isVertical() const
    {
        return m_orientation == Qt::Vertical;
    }

    /**
     * @brief 返回此侧边栏中是否没有任何停靠小部件。
     * @return 如果为空，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isEmpty() const;

    /**
     * @brief 返回此侧边栏在主窗口中的位置。
     * @return SideBarLocation 枚举值。
     */
    [[nodiscard]] SideBarLocation location() const;

    /**
     * @brief 返回此侧边栏所属的主窗口。
     * @return 指向 MainWindowBase 对象的指针。
     */
    [[nodiscard]] MainWindowBase *mainWindow() const;

    /**
     * @brief 切换指定停靠小部件的覆盖层显示状态。
     *
     * 行为上等同于用户点击侧边栏上代表该停靠小部件的按钮。
     * 如果小部件当前是覆盖层，则隐藏它；如果当前隐藏在侧边栏，则以覆盖层形式显示它。
     * @param dw 要切换覆盖层状态的 DockWidgetBase 对象。
     */
    void toggleOverlay(DockWidgetBase *dw);

    /**
     * @brief 返回此侧边栏状态的序列化表示。
     *
     * 当前主要包含其中停靠小部件的 ID (唯一名称) 列表。
     * @return 包含停靠小部件唯一名称的 QStringList。
     */
    [[nodiscard]] QStringList serialize() const;

    /**
     * @brief 清空此侧边栏，移除其中所有的停靠小部件。
     */
    void clear();

protected:
    /**
     * @brief 添加停靠小部件的具体实现 (纯虚函数，由子类提供)。
     *
     * 子类 (例如 SideBarWidget) 需要实现此方法来创建和管理实际的UI元素（如按钮）。
     * @param dock 要添加的 DockWidgetBase 对象。
     */
    virtual void addDockWidget_Impl(DockWidgetBase *dock) = 0;
    /**
     * @brief 移除停靠小部件的具体实现 (纯虚函数，由子类提供)。
     *
     * 子类需要实现此方法来移除和销毁相关的UI元素。
     * @param dock 要移除的 DockWidgetBase 对象。
     */
    virtual void removeDockWidget_Impl(DockWidgetBase *dock) = 0;

    /**
     * @brief 当侧边栏上的某个按钮被点击时的处理函数。
     *
     * 此方法通常会调用 toggleOverlay() 来显示或隐藏关联的停靠小部件。
     * @param dw 与被点击按钮关联的 DockWidgetBase 对象。
     */
    void onButtonClicked(DockWidgetBase *dw);

    /**
     * @brief 获取此侧边栏中当前包含的所有停靠小部件。
     * @return DockWidgetBase 指针的 QVector。
     */
    [[nodiscard]] QVector<DockWidgetBase *> dockWidgets() const;

private:
    /**
     * @brief 当侧边栏中的某个停靠小部件被销毁时的槽函数。
     *
     * 用于从侧边栏内部列表中移除该小部件的引用。
     * @param dw 被销毁的 QObject (实际上是 DockWidgetBase) 的指针。
     */
    void onDockWidgetDestroyed(QObject *dw);
    /**
     * @brief 更新侧边栏的大小。
     *
     * 当添加或移除停靠小部件时，可能需要调整侧边栏自身的大小或布局。
     */
    void updateSize();

    MainWindowBase *const m_mainWindow;     ///< 指向此侧边栏所属的主窗口的常量指针。
    QVector<DockWidgetBase *> m_dockWidgets; ///< 存储此侧边栏中所有停靠小部件的列表。
    const SideBarLocation m_location;       ///< 此侧边栏在主窗口中的位置 (只读)。
    const Qt::Orientation m_orientation;    ///< 此侧边栏的朝向 (垂直或水平，只读)。
};

} // namespace KDDockWidgets

#endif // KD_SIDEBAR_P_H