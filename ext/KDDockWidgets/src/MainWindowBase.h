/**
 * @file
 * @brief MainWindow (主窗口) 的基类，在 QtWidgets 和 QtQuick 技术栈之间共享。
 *
 * @author Sérgio Martins <sergio.martins@kdab.com>
 */

#ifndef KD_MAINWINDOW_BASE_H
#define KD_MAINWINDOW_BASE_H

#include "docks_export.h" // 导入导出宏定义，用于库的符号可见性
#include "KDDockWidgets.h" // KDDockWidgets 公共头文件，包含核心枚举和类型定义
#include "QWidgetAdapter.h" // QWidget 和 QQuickItem 的适配器类
#include "LayoutSaver.h" // 布局保存与恢复相关的类

#include <QVector> // Qt 动态数组容器
#include <QMargins> // Qt 边距类

// 前向声明，用于测试目的
class TestDocks;

namespace KDDockWidgets {

// 前向声明 KDDockWidgets 内部或公共类
class DockWidgetBase; // 停靠小部件基类
class Frame; // 框架类 (容纳 DockWidgetBase)
class DropArea; // 放置区域类
class MDILayoutWidget; // MDI (多文档界面) 布局小部件类
class MultiSplitter; // 多重分割器类
class LayoutWidget; // 布局小部件基类
class DropAreaWithCentralFrame; // 带有中央框架的放置区域类
class SideBar; // 侧边栏类

/**
 * @brief MainWindow 的基类。MainWindow 和 MainWindowBase 被拆分为两个类，
 * 以便与 QtQuick 实现共享一些代码，QtQuick 实现也派生自 MainWindowBase。
 *
 * 用户代码中不应直接实例化此类。请改用 MainWindow (用于 QtWidgets) 或相应的 QtQuick 主窗口类。
 */
#ifndef PYTHON_BINDINGS // 条件编译：处理 Pyside 的一个 BUG (PYSIDE-1327)，如果不是为 Python 绑定编译
// QMainWindowOrQuick 是一个类型别名或宏，根据构建目标 (QtWidgets 或 QtQuick) 解析为 QMainWindow 或 QQuickWindow/QQuickItem
class DOCKS_EXPORT MainWindowBase : public QMainWindowOrQuick
#else // 如果是为 Python 绑定编译 (PYSIDE)
class DOCKS_EXPORT MainWindowBase : public QMainWindow // Pyside bug 解决方法：直接继承自 QMainWindow
#endif
{
Q_OBJECT // Q_OBJECT 宏，用于启用 Qt 元对象系统特性，如信号和槽

// Q_PROPERTY 宏，向 Qt 元对象系统暴露属性
/// @brief Q_PROPERTY，表示此主窗口的亲和性列表 (只读)。
Q_PROPERTY(QStringList affinities READ affinities CONSTANT)
    /// @brief Q_PROPERTY，表示此主窗口的唯一名称 (只读)。
    Q_PROPERTY(QString uniqueName READ uniqueName CONSTANT)
    /// @brief Q_PROPERTY，表示此主窗口的选项 (只读)。
    Q_PROPERTY(KDDockWidgets::MainWindowOptions options READ options CONSTANT)
    /// @brief Q_PROPERTY，表示此主窗口是否处于 MDI (多文档界面) 模式 (只读)。
    Q_PROPERTY(bool isMDI READ isMDI CONSTANT)
    /// @brief Q_PROPERTY，表示侧边栏覆盖层与主窗口边缘的间距。可读写。
    Q_PROPERTY(int overlayMargin READ overlayMargin WRITE setOverlayMargin NOTIFY overlayMarginChanged)

        public :
    /// @brief MainWindowBase 指针的 QVector 类型定义，方便使用。
    typedef QVector<MainWindowBase *> List;

    /**
     * @brief 构造函数。
     * @param uniqueName 强制性的名称，在所有 MainWindow 实例之间应该是唯一的。
     * 此名称不会对用户可见，仅在内部用于保存/恢复布局。
     * @param options 可选的 MainWindowOptions，用于配置主窗口特性，默认为 MainWindowOption_HasCentralFrame。
     * @param parent 父 WidgetType (QWidget* 或 QQuickItem*) 对象，默认为 nullptr。
     * @param flags 窗口标志，默认为 Qt::WindowFlags()。
     */
    explicit MainWindowBase(const QString &uniqueName, MainWindowOptions options = MainWindowOption_HasCentralFrame,
                            WidgetType *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    /**
     * @brief 析构函数。
     */
    ~MainWindowBase() override;

    /**
     * @brief 将一个 DockWidget 作为标签页停靠到中央框架中。
     * @warning 要求主窗口构造时使用了 MainWindowOption_HasCentralFrame 选项。
     * @param dockwidget 要停靠的停靠小部件。
     *
     * @sa DockWidgetBase::addDockWidgetAsTab()
     */
    Q_INVOKABLE void addDockWidgetAsTab(KDDockWidgets::DockWidgetBase *dockwidget);

    /**
     * @brief 将一个 DockWidget 停靠到此主窗口中。
     * @param dw 要添加到此主窗口的停靠小部件。
     * @param location 停靠的位置。
     * @param relativeTo 如果是相对于另一个停靠小部件进行停靠，则指定该参照小部件。如果为 nullptr，则相对于主窗口。
     * @param initialOption 允许指定一些初始选项，例如将停靠小部件添加为隐藏状态，
     * 仅在标签页中记录一个占位符。这样，当它最终显示时，会恢复为标签化状态。
     */
    Q_INVOKABLE void addDockWidget(KDDockWidgets::DockWidgetBase *dw,
                                   KDDockWidgets::Location location,
                                   KDDockWidgets::DockWidgetBase *relativeTo = nullptr,
                                   KDDockWidgets::InitialOption initialOption = {}) const;

    /**
     * @brief 设置一个持久的中央小部件。它不能被分离。
     *
     * 要求在构造函数中传递 MainWindowOption_HasCentralWidget。
     * 这类似于 MainWindowOption_HasCentralFrame 的中央框架概念，
     * 不同之处在于它不会显示标签页。
     *
     * @param widget 您想要设置的 QWidget (或 QQuickItem，如果使用 QtQuick 支持构建)。
     *
     * 示例: kddockwidgets_example --central-widget
     */
    Q_INVOKABLE void setPersistentCentralWidget(KDDockWidgets::QWidgetOrQuick *widget);
    /**
     * @brief 获取持久的中央小部件。
     * @return 指向持久中央小部件的 QWidgetOrQuick 指针，如果未设置则为 nullptr。
     */
    [[nodiscard]] QWidgetOrQuick *persistentCentralWidget() const;

    /**
     * @brief 返回通过构造函数传递的唯一名称。
     * 由保存/恢复机制内部使用。
     * @internal 内部使用。
     * @return 唯一名称字符串。
     */
    [[nodiscard]] QString uniqueName() const;


    /**
     * @brief 返回通过构造函数传递的主窗口选项。
     * @return MainWindowOptions 枚举值的组合。
     */
    [[nodiscard]] MainWindowOptions options() const;

    /**
     * @internal
     * @brief 返回放置区域 (DropArea)。
     * @return 指向 DropAreaWithCentralFrame 对象的指针。
     */
    [[nodiscard]] DropAreaWithCentralFrame *dropArea() const;

    /**
     * @internal
     * @brief 返回多重分割器 (MultiSplitter)。
     * @return 指向 MultiSplitter 对象的指针。
     */
    [[nodiscard]] MultiSplitter *multiSplitter() const;

    /**
     * @internal
     * @brief 返回布局小部件 (LayoutWidget)。
     * @return 指向 LayoutWidget 对象的指针。
     */
    [[nodiscard]] LayoutWidget *layoutWidget() const;

    /**
     * @internal
     * @brief 返回 MDI 布局。如果这不是一个 MDI 主窗口，则返回 nullptr。
     * @return 指向 MDILayoutWidget 的指针，或者 nullptr。
     */
    [[nodiscard]] MDILayoutWidget *mdiLayoutWidget() const;

    /**
     * @brief 设置亲和性名称。停靠小部件只能停靠到具有相同亲和性的主窗口中。
     *
     * 默认情况下，亲和性为空，停靠小部件可以停靠到任何主窗口中。通常您不需要调用此函数，
     * 除非您有某些停靠小部件只能停靠到某些特定主窗口的需求。
     * @sa DockWidgetBase::setAffinities()。
     *
     * 注意：在创建主窗口后，停靠任何停靠小部件之前以及恢复任何布局之前立即调用此函数。
     * 注意：目前只能调用此函数一次，以保持代码简单并避免边缘情况。
     * 只有在出现需要多次更改亲和性的良好用例时，才会更改此行为。
     *
     * @param names 亲和性名称列表。
     */
    void setAffinities(const QStringList &names);

    /**
     * @brief 返回亲和性名称列表。默认为空。
     * @return 亲和性名称的 QStringList。
     */
    [[nodiscard]] QStringList affinities() const;

    /**
     * @brief 平均分配所有小部件的尺寸，使其在其父容器内大小相等。
     *
     * 注意，布局是一个嵌套的水平和垂直容器布局树。
     * 越靠近根节点的节点将获得更多空间。
     *
     * 最小/最大尺寸约束仍将被遵守。
     */
    Q_INVOKABLE void layoutEqually() const;

    /**
     * @brief 类似于 layoutEqually()，但从包含 @p dockWidget 的容器开始。
     * layoutEqually() 从布局树的根开始，而此函数从子树开始。
     * @param dockWidget 作为子树起点的停靠小部件。
     */
    Q_INVOKABLE void layoutParentContainerEqually(KDDockWidgets::DockWidgetBase *dockWidget) const;

    /**
     * @brief 将停靠小部件移动到主窗口的某个侧边栏。
     * 这意味着停靠小部件将从当前布局中移除，侧边栏会显示一个按钮，
     * 如果按下该按钮，将以覆盖层形式切换停靠小部件的可见性。这就是自动隐藏功能。
     *
     * 选择哪个侧边栏将取决于一些启发式方法，主要是邻近度。
     * @param dw 要移动到侧边栏的 DockWidgetBase 对象。
     */
    Q_INVOKABLE void moveToSideBar(KDDockWidgets::DockWidgetBase *dw);

    /**
     * @brief (重载) 允许指定使用哪个侧边栏，而不是使用启发式方法。
     * @param dw 要移动到侧边栏的 DockWidgetBase 对象。
     * @param location 目标侧边栏的位置。
     */
    Q_INVOKABLE void moveToSideBar(KDDockWidgets::DockWidgetBase *dw, KDDockWidgets::SideBarLocation location);

    /**
     * @brief 将停靠小部件从侧边栏移除并再次停靠回主窗口。
     * @param dw 要从侧边栏恢复的 DockWidgetBase 对象。
     */
    Q_INVOKABLE void restoreFromSideBar(KDDockWidgets::DockWidgetBase *dw);

    /**
     * @brief 将停靠小部件作为覆盖层显示在主窗口之上，放置在侧边栏旁边。
     * @param dw 要作为覆盖层显示的 DockWidgetBase 对象。
     */
    Q_INVOKABLE void overlayOnSideBar(KDDockWidgets::DockWidgetBase *dw);

    /**
     * @brief 显示或隐藏一个覆盖层。假定停靠小部件已经在侧边栏中。
     * @param dw 要切换覆盖层状态的 DockWidgetBase 对象。
     */
    Q_INVOKABLE void toggleOverlayOnSideBar(KDDockWidgets::DockWidgetBase *dw);

    /**
     * @brief 关闭任何覆盖显示的停靠小部件。侧边栏仍将其显示为按钮。
     * @param deleteFrame 如果为 true (默认)，则在关闭覆盖层时删除其关联的 Frame。
     */
    Q_INVOKABLE void clearSideBarOverlay(bool deleteFrame = true);

    /**
     * @brief 返回此停靠小部件所在的侧边栏。如果不在任何侧边栏中，则返回 nullptr。
     * @param dw 要查询的 DockWidgetBase 对象。
     * @return 指向 SideBar 对象的指针，或 nullptr。
     */
    Q_INVOKABLE KDDockWidgets::SideBar *
    sideBarForDockWidget(const KDDockWidgets::DockWidgetBase *dw) const;

    /**
     * @brief 返回指定的侧边栏是否可见。
     * @param location 要检查的侧边栏位置。
     * @return 如果侧边栏可见，则返回 true；否则返回 false。
     */
    Q_INVOKABLE [[nodiscard]] bool sideBarIsVisible(KDDockWidgets::SideBarLocation location) const;

    /**
     * @brief 返回当前覆盖显示的停靠小部件。如果没有，则返回 nullptr。
     * 仅在使用自动隐藏和侧边栏功能时才有意义。
     * @return 指向当前覆盖显示的 DockWidgetBase 的指针，或 nullptr。
     */
    [[nodiscard]] DockWidgetBase *overlayedDockWidget() const;

    /**
     * @brief 返回是否有任何侧边栏是可见的。
     * @return 如果至少有一个侧边栏可见，则返回 true。
     */
    [[nodiscard]] bool anySideBarIsVisible() const;

    /**
     * @brief 返回此主窗口是否使用 MDI 布局。
     * 换句话说，如果在构造函数中传递了 MainWindowOption_MDI，则返回 true。
     * @return 如果是 MDI 模式，则返回 true。
     */
    [[nodiscard]] bool isMDI() const;

    /**
     * @brief 关闭所有停靠在此主窗口中的停靠小部件。
     * 这是单独调用 DockWidget::close() 的便利方法。
     * 如果 force 为 true，则各个停靠小部件无法阻止此过程。
     * @param force 如果为 true，则强制关闭。
     * @return 如果至少有一个停靠小部件拒绝关闭，则返回 false。如果所有停靠小部件都已关闭（0个或多个），则返回 true。
     */
    Q_INVOKABLE bool closeDockWidgets(bool force = false);

    /**
     * @brief 返回窗口的几何形状。
     * 通常与 MainWindowBase::geometry() 相同。
     * 但修复了以下特殊情况：
     * - QWidgets: 我们的 MainWindow 嵌入在另一个小部件中。
     * - QtQuick: 我们的 MainWindow 是 QQuickItem。
     * @return 窗口的全局几何 QRect。
     */
    [[nodiscard]] QRect windowGeometry() const;

    /**
     * @brief 返回覆盖停靠小部件使用的边距。默认值：1。
     * @return 边距大小（像素）。
     */
    [[nodiscard]] int overlayMargin() const;

    /**
     * @brief 设置覆盖停靠小部件使用的边距。
     * 不会修改当前已覆盖显示的停靠小部件。
     * @param margin 新的边距大小（像素）。
     */
    void setOverlayMargin(int margin);

protected:
#ifdef KDDOCKWIDGETS_QTWIDGETS
    /**
     * @brief 处理关闭事件 (QWidget 虚函数重写)。
     * @param event 关闭事件对象。
     */
    void onCloseEvent(QCloseEvent *event);
#else
    // QtQuick 使用不同的基类。这将在 wip/2.0 分支中修复。
    /**
     * @brief 处理关闭事件 (重写)。
     * @param event 关闭事件对象。
     */
    void onCloseEvent(QCloseEvent *event) override;
#endif

    /**
     * @brief 设置主窗口的唯一名称。
     * @param uniqueName 新的唯一名称。
     */
    void setUniqueName(const QString &uniqueName);
    /**
     * @brief 当窗口大小调整时的回调函数（因为 QtQuick 没有 resizeEvent()）。
     * @param event 大小调整事件对象 (对于 QtWidgets)。
     */
    void onResized(QResizeEvent *event);
    /**
     * @brief (纯虚函数) 返回中央小部件区域的内容边距。
     * @return QMargins 对象。
     */
    [[nodiscard]] virtual QMargins centerWidgetMargins() const = 0;
    /**
     * @brief (纯虚函数) 返回指定位置的侧边栏。
     * @param location 侧边栏位置。
     * @return 指向 SideBar 对象的指针。
     */
    [[nodiscard]] virtual SideBar *sideBar(SideBarLocation location) const = 0;
    /**
     * @brief (虚函数) 获取中央区域的几何形状。
     * 默认实现返回一个空 QRect。
     * @return 中央区域的 QRect。
     */
    [[nodiscard]] virtual QRect centralAreaGeometry() const
    {
        return {};
    }

Q_SIGNALS: // 信号部分
    /**
     * @brief 当唯一名称改变时发射此信号。
     */
    void uniqueNameChanged();

    /**
     * @brief 当停靠的框架数量改变时发射此信号。
     * 注意，这里使用 "Frame" 而不是 "DockWidget" 的术语，因为 DockWidget
     * 可以组合成标签页，在这种情况下此信号不会发射。
     * @param count 新的框架数量。
     */
    void frameCountChanged(int count);

    /**
     * @brief 当覆盖层边距改变时发射此信号。
     * @param margin 新的边距值。
     */
    void overlayMarginChanged(int margin);

private:
    class Private; ///< PIMPL (Private Implementation) 设计模式的私有实现类前向声明。
    Private *const d; ///< 指向私有实现类的常量指针。

    // 声明友元类，允许它们访问私有成员
    friend class ::TestDocks; // 测试类
    friend class LayoutSaver; // 布局保存器类

    /**
     * @brief 从保存的布局数据中反序列化主窗口的状态。
     * @param data 包含主窗口布局信息的 LayoutSaver::MainWindow 对象。
     * @return 如果反序列化成功则返回 true，否则返回 false。
     */
    bool deserialize(const LayoutSaver::MainWindow &data);
    /**
     * @brief 将当前主窗口的状态序列化为布局数据。
     * @return 包含主窗口布局信息的 LayoutSaver::MainWindow 对象。
     */
    [[nodiscard]] LayoutSaver::MainWindow serialize() const;
};
} // namespace KDDockWidgets

#endif // KD_MAINWINDOW_BASE_H
