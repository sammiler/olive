#ifndef KD_DOCKREGISTRY_P_H
#define KD_DOCKREGISTRY_P_H

#include "kddockwidgets/DockWidgetBase.h"   // 包含停靠部件基类的公共头文件
#include "kddockwidgets/MainWindowBase.h" // 包含主窗口基类的公共头文件

#include "kddockwidgets/private/Frame_p.h"  // 包含 Frame 私有头文件

#include <QVector>   // 包含 QVector 类
#include <QObject>   // 包含 QObject 基类
#include <QPointer>  // 包含 QPointer 类，用于安全地跟踪 QObject 对象
#include <QStringList> // 包含 QStringList 类
#include <QHash>       // 包含 QHash 类

// 前向声明 Qt 命名空间内的类
QT_BEGIN_NAMESPACE
class QWindow;
class QMouseEvent;
QT_END_NAMESPACE

/**
 * @file DockRegistry_p.h
 * @brief DockRegistry 是一个单例类，它了解所有 DockWidget。
 * 它用于恢复布局。
 * 这是一个私有的实现细节。
 */
namespace KDDockWidgets {

// 前向声明命名空间内的类
class FloatingWindow;
class Frame;
class LayoutWidget;
class MainWindowMDI;
class SideBar;
struct WindowBeingDragged; // 假设这是一个结构体


/**
 * @brief DockRegistry 类是一个单例，负责跟踪 KDDockWidgets 框架中所有主要的组件实例。
 *
 * 这些组件包括停靠部件 (DockWidgetBase)、主窗口 (MainWindowBase)、浮动窗口 (FloatingWindow)、
 * 布局部件 (LayoutWidget) 和框架 (Frame)。
 * DockRegistry 的主要目的是为了能够保存和恢复布局状态，通过提供一种中心化的方式来查找和管理这些组件。
 *
 * 注意：此类是 KDDockWidgets 的内部实现细节，不应直接被库的用户使用。
 */
class DOCKS_EXPORT DockRegistry : public QObject
{
    Q_OBJECT
    /**
     * @qproperty KDDockWidgets::Frame* frameInMDIResize
     * @brief 只读属性，表示当前在 MDI 布局中正在被调整大小的 Frame。
     * 如果没有 Frame 正在被调整大小，则为 nullptr。
     * @see frameInMDIResizeChanged()
     */
    Q_PROPERTY(
        KDDockWidgets::Frame *frameInMDIResize READ frameInMDIResize NOTIFY frameInMDIResizeChanged)
public:
    /**
     * @brief 枚举类，定义通过名称查找停靠部件时的行为标志。
     */
    enum class DockByNameFlag {
        None = 0,             ///< 无特殊标志，如果找不到则返回 nullptr。
        ConsultRemapping = 1, ///< 查找时考虑 ID 重映射（用于布局恢复）。
        CreateIfNotFound = 2  ///< 如果找不到停靠部件，则尝试通过用户提供的部件工厂创建它。
    };
    Q_DECLARE_FLAGS(DockByNameFlags, DockByNameFlag) // 为 DockByNameFlag 声明一个 Qt 标志类型 DockByNameFlags

    /**
     * @brief 获取 DockRegistry 的单例实例。
     * @return 指向 DockRegistry 单例对象的指针。
     */
    static DockRegistry *self();

    /**
     * @brief 析构函数。
     */
    ~DockRegistry() override;

    /**
     * @brief 注册一个停靠部件。
     * @param dw 要注册的 DockWidgetBase 指针。
     */
    void registerDockWidget(DockWidgetBase *dw);

    /**
     * @brief 取消注册一个停靠部件。
     * @param dw 要取消注册的 DockWidgetBase 指针。
     */
    void unregisterDockWidget(DockWidgetBase *dw);

    /**
     * @brief 注册一个主窗口。
     * @param mw 要注册的 MainWindowBase 指针。
     */
    void registerMainWindow(MainWindowBase *mw);

    /**
     * @brief 取消注册一个主窗口。
     * @param mw 要取消注册的 MainWindowBase 指针。
     */
    void unregisterMainWindow(MainWindowBase *mw);

    /**
     * @brief 注册一个浮动窗口。
     * @param fw 要注册的 FloatingWindow 指针。
     */
    void registerFloatingWindow(FloatingWindow *fw);

    /**
     * @brief 取消注册一个浮动窗口。
     * @param fw 要取消注册的 FloatingWindow 指针。
     */
    void unregisterFloatingWindow(FloatingWindow *fw);

    /**
     * @brief 注册一个布局部件。
     * @param layout 要注册的 LayoutWidget 指针。
     */
    void registerLayout(LayoutWidget *layout);

    /**
     * @brief 取消注册一个布局部件。
     * @param layout 要取消注册的 LayoutWidget 指针。
     */
    void unregisterLayout(LayoutWidget *layout);

    /**
     * @brief 注册一个框架。
     * @param frame 要注册的 Frame 指针。
     */
    void registerFrame(Frame *frame);

    /**
     * @brief 取消注册一个框架。
     * @param frame 要取消注册的 Frame 指针。
     */
    void unregisterFrame(Frame *frame);

    /**
     * @brief Q_INVOKABLE 方法，返回当前拥有焦点的停靠部件。
     * @return 指向当前获得焦点的 DockWidgetBase 的指针；如果没有，则返回 nullptr。
     */
    Q_INVOKABLE KDDockWidgets::DockWidgetBase *focusedDockWidget() const;

    /**
     * @brief Q_INVOKABLE 方法，检查是否已注册具有指定唯一名称的停靠部件。
     * @param uniqueName 要检查的停靠部件的唯一名称。
     * @return 如果存在具有该名称的停靠部件，则返回 true；否则返回 false。
     */
    Q_INVOKABLE bool containsDockWidget(const QString &uniqueName) const;

    /**
     * @brief Q_INVOKABLE 方法，检查是否已注册具有指定唯一名称的主窗口。
     * @param uniqueName 要检查的主窗口的唯一名称。
     * @return 如果存在具有该名称的主窗口，则返回 true；否则返回 false。
     */
    Q_INVOKABLE bool containsMainWindow(const QString &uniqueName) const;

    /**
     * @brief Q_INVOKABLE 方法，根据唯一名称查找停靠部件。
     * @param uniqueName 要查找的停靠部件的唯一名称。
     * @param flags 控制查找行为的标志 (DockByNameFlags)，默认为空。
     * @return 指向找到的 DockWidgetBase 的指针；如果未找到，则根据标志行为返回 nullptr 或新创建的部件。
     */
    Q_INVOKABLE KDDockWidgets::DockWidgetBase *dockByName(const QString &uniqueName,
                                                          KDDockWidgets::DockRegistry::DockByNameFlags flags = {}) const;
    /**
     * @brief Q_INVOKABLE 方法，根据唯一名称查找主窗口。
     * @param uniqueName 要查找的主窗口的唯一名称。
     * @return 指向找到的 MainWindowBase 的指针；如果未找到，则返回 nullptr。
     */
    Q_INVOKABLE KDDockWidgets::MainWindowBase *mainWindowByName(const QString &uniqueName) const;

    /**
     * @brief Q_INVOKABLE 方法，根据唯一名称查找 MDI 主窗口。
     * @param uniqueName 要查找的 MDI 主窗口的唯一名称。
     * @return 指向找到的 MainWindowMDI 的指针；如果未找到，则返回 nullptr。
     */
    Q_INVOKABLE KDDockWidgets::MainWindowMDI *mdiMainWindowByName(const QString &uniqueName) const;

    /**
     * @brief 返回托管指定 "guest" 部件的停靠部件。
     * @param guest 指向 "guest" 部件 (QWidgetOrQuick) 的指针。
     * @return 如果找到，则返回包含该 guest 的 DockWidgetBase 指针；否则返回 nullptr。
     */
    DockWidgetBase *dockWidgetForGuest(QWidgetOrQuick *guest) const;

    /**
     * @brief 检查注册表内部状态是否一致和有效。
     * @return 如果状态有效，则返回 true；否则返回 false。
     */
    bool isSane() const;

    /**
     * @brief 返回所有已注册的 DockWidgetBase 实例的列表。
     * @return DockWidgetBase 指针的列表。
     */
    DockWidgetBase::List dockwidgets() const;

    /**
     * @brief 重载版本，返回名称在指定列表中的所有 DockWidgetBase 实例。
     * @param names 要查找的停靠部件名称列表。
     * @return 符合名称的 DockWidgetBase 指针的列表。
     */
    DockWidgetBase::List dockWidgets(const QStringList &names);

    /**
     * @brief 返回所有已关闭的 DockWidgetBase 实例的列表。
     * @return 已关闭的 DockWidgetBase 指针的列表。
     */
    DockWidgetBase::List closedDockwidgets() const;

    /**
     * @brief 返回所有已注册的 MainWindowBase 实例的列表。
     * @return MainWindowBase 指针的列表。
     */
    MainWindowBase::List mainwindows() const;

    /**
     * @brief 重载版本，返回名称在指定列表中的所有 MainWindowBase 实例。
     * @param names 要查找的主窗口名称列表。
     * @return 符合名称的 MainWindowBase 指针的列表。
     */
    MainWindowBase::List mainWindows(const QStringList &names);

    /**
     * @brief 返回所有已注册的 LayoutWidget 实例的列表。
     * @return LayoutWidget 指针的向量。
     */
    QVector<LayoutWidget *> layouts() const;

    /**
     * @brief 返回所有已注册的 Frame 实例的列表。
     * @return Frame 指针的列表。
     */
    QList<Frame *> frames() const;

    /**
     * @brief 返回所有已注册的 FloatingWindow 实例。
     *
     * 这不一定包含所有浮动的停靠部件，因为某些 DockWidget 可能尚未“变形”为完整的 FloatingWindow。
     * @param includeBeingDeleted 如果为 true，则结果中也包含正在被删除的浮动窗口。默认为 false。
     * @return FloatingWindow 指针的向量。
     */
    QVector<FloatingWindow *> floatingWindows(bool includeBeingDeleted = false) const;

    /**
     * @brief 重载版本，返回所有浮动窗口的 QWindow 句柄列表。
     *
     * 这种方式对于同时支持 QtWidgets 和 QtQuick 更友好。
     * @return QWindow 指针的向量。
     */
    QVector<QWindow *> floatingQWindows() const;

    /**
     * @brief Q_INVOKABLE 方法，检查当前是否存在至少一个浮动窗口。
     * @return 如果存在浮动窗口，则返回 true；否则返回 false。
     */
    Q_INVOKABLE bool hasFloatingWindows() const;

    /**
     * @brief 静态方法，根据窗口句柄 ID 返回对应的 QWindow。
     * @param id 窗口句柄 ID (WId)。
     * @return 指向 QWindow 的指针；如果未找到，则返回 nullptr。
     */
    static QWindow *windowForHandle(WId id);

    /**
     * @brief 根据 QWindow 句柄返回对应的 FloatingWindow。
     * @param windowHandle 指向 QWindow 的指针。
     * @return 指向 FloatingWindow 的指针；如果未找到，则返回 nullptr。
     */
    FloatingWindow *floatingWindowForHandle(QWindow *windowHandle) const;

    /**
     * @brief 根据窗口句柄 ID (WId) 返回对应的 FloatingWindow。
     * @param hwnd 窗口句柄 ID。
     * @return 指向 FloatingWindow 的指针；如果未找到，则返回 nullptr。
     */
    FloatingWindow *floatingWindowForHandle(WId hwnd) const;

    /**
     * @brief 根据 QWindow 句柄返回对应的 MainWindowBase。
     * @param windowHandle 指向 QWindow 的指针。
     * @return 指向 MainWindowBase 的指针；如果未找到，则返回 nullptr。
     */
    MainWindowBase *mainWindowForHandle(QWindow *windowHandle) const;

    /**
     * @brief 返回与指定 QWindow 关联的顶层部件。
     *
     * 对于 QtWidgets，它返回一个 QWidget，该 QWidget 要么是 KDDockWidgets::MainWindow，要么是 FloatingWindow。
     * 对于 QtQuick，它返回相同的逻辑部件，但类型是 QWidgetAdapter (一个 QQuickItem 的包装器)，而不是 QWidget。
     * @param windowHandle 指向 QWindow 的指针。
     * @return 指向顶层 QWidgetOrQuick 的指针；如果未找到，则返回 nullptr。
     */
    QWidgetOrQuick *topLevelForHandle(QWindow *windowHandle) const;

    /**
     * @brief 返回所有可见的 FloatingWindow 和 MainWindow 实例的顶层父窗口列表。
     *
     * 通常这些就是 FloatingWindow 和 MainWindow 自身。但是，由于 MainWindow 可以嵌入到另一个部件中
     * (例如，像 QWinWidget 那样)，这意味着顶层窗口可能是其他东西。
     * 返回的每个窗口要么是 FloatingWindow，要么是 MainWindow，要么是包含 MainWindow 的某个东西。
     *
     * @param excludeFloatingDocks 如果为 true，则结果中不包含 FloatingWindow。默认为 false。
     * @return QWindow 指针的向量。
     */
    QVector<QWindow *> topLevels(bool excludeFloatingDocks = false) const;

    /**
     * @brief 关闭所有停靠部件，并销毁所有 FloatingWindow。
     *
     * 此方法在恢复布局之前调用。
     * @param affinities 如果指定，则仅关闭具有指定亲和性的停靠部件和主窗口。默认为空列表 (影响所有)。
     */
    Q_INVOKABLE void clear(const QStringList &affinities = {});

    /**
     * @brief clear 的重载版本，仅清除指定的停靠部件和主窗口。
     * @param dockWidgets 要清除的停靠部件列表。
     * @param mainWindows 要清除的主窗口列表。
     * @param affinities 如果指定，则仅当部件的亲和性与列表中的任何一个匹配时才清除。
     */
    static void clear(const DockWidgetBase::List &dockWidgets,
                      const MainWindowBase::List &mainWindows,
                      const QStringList &affinities);

    /**
     * @brief 确保所有浮动的 DockWidget 都有一个 FloatingWindow 作为其窗口。
     *
     * 这是为了在保存布局之前简化事情，这样就不必处理窗口是 DockWidget 本身的情况。
     */
    void ensureAllFloatingWidgetsAreMorphed();

    /**
     * @brief 返回注册表中是否没有任何停靠部件和主窗口。
     *
     * @param excludeBeingDeleted 如果为 true，则当前正在被删除的任何窗口将不计入总数。
     * @return 如果没有已注册的停靠部件和主窗口 (考虑到 excludeBeingDeleted 参数)，则返回 true。
     */
    bool isEmpty(bool excludeBeingDeleted = false) const;

    /**
     * @brief 在所有布局上调用 LayoutWidget::checkSanity()。
     *
     * @param dumpLayout 如果为 true，则同时转储每个布局的调试信息。
     *
     * 此方法由单元测试或模糊测试器调用。如果在此期间框架发出 qWarning()，应用程序将 qFatal()。
     */
    void checkSanityAll(bool dumpLayout = false);

    /**
     * @brief 返回当前是否正在处理 QEvent::Quit 事件。
     *
     * 内部使用，以了解在应用程序关闭时是否应允许 Qt 关闭 NonClosable 的停靠部件。
     * @return 如果正在处理应用程序退出事件，则返回 true。
     */
    bool isProcessingAppQuitEvent() const;

    /**
     * @brief 返回所有与给定亲和性列表中的至少一个匹配的主窗口。
     * @param affinities 要匹配的亲和性字符串列表。
     * @return MainWindowBase 指针的列表。
     */
    MainWindowBase::List mainWindowsWithAffinity(const QStringList &affinities) const;

    /**
     * @brief 静态方法，返回包含指定布局项 (Item) 的 LayoutWidget。
     * @param item 指向 Layouting::Item 的指针。
     * @return 指向包含该项的 LayoutWidget 的指针；如果未找到，则返回 nullptr。
     */
    static LayoutWidget *layoutForItem(const Layouting::Item *);

    /**
     * @brief 静态方法，检查指定的布局项是否位于主窗口内。
     *
     * 此检查会考虑嵌套关系 (例如，MDIArea 在 DropArea 内，DropArea 在 MainWindow 内)。
     * @param item 指向 Layouting::Item 的指针。
     * @return 如果项位于主窗口内，则返回 true。
     */
    static bool itemIsInMainWindow(const Layouting::Item *);

    /**
     * @brief 静态方法，检查两个亲和性列表是否有任何共同的亲和性。
     * @param affinities1 第一个亲和性列表。
     * @param affinities2 第二个亲和性列表。
     * @return 如果两个列表共享至少一个相同的亲和性字符串，则返回 true。
     */
    static bool affinitiesMatch(const QStringList &affinities1, const QStringList &affinities2);

    /**
     * @brief 返回所有已知主窗口的唯一名称列表。
     * @return QStringList 包含所有主窗口的唯一名称。
     */
    QStringList mainWindowsNames() const;

    /**
     * @brief 返回所有已知停靠部件的唯一名称列表。
     * @return QStringList 包含所有停靠部件的唯一名称。
     */
    QStringList dockWidgetNames() const;

    /**
     * @brief 返回指定的窗口是否可能被其他窗口遮挡 (具有更高的 Z 顺序)。
     *
     * 这是一个近似判断，因为没有直接的方法比较 Z 顺序，所以主要通过检查几何区域的相交情况。
     * @param target 要检查是否被遮挡的目标窗口。
     * @param exclude 此窗口不应被视为遮挡者 (例如，因为它正在被拖动)。
     * @return 如果目标窗口可能被遮挡，则返回 true。
     */
    bool isProbablyObscured(QWindow *target, FloatingWindow *exclude) const;

    /**
     * @brief isProbablyObscured 的重载版本。
     * @param target 要检查是否被遮挡的目标窗口。
     * @param exclude 指向正在被拖动的窗口的信息结构体指针，该窗口不应被视为遮挡者。
     * @return 如果目标窗口可能被遮挡，则返回 true。
     */
    bool isProbablyObscured(QWindow *target, WindowBeingDragged *exclude) const;

    /**
     * @brief 返回指定的停靠部件是否位于侧边栏中，以及位于哪个侧边栏。
     *
     * 如果它不在侧边栏中，则返回 SideBarLocation::None。
     * 这仅在使用了自动隐藏和侧边栏功能时才有意义。
     * @param dw 要检查的 DockWidgetBase 指针。
     * @return 停靠部件所在的 SideBarLocation。
     */
    SideBarLocation sideBarLocationForDockWidget(const DockWidgetBase *dw) const;

    /**
     * @brief 重载版本，返回停靠部件所在的 SideBar 对象本身。
     * @param dw 要检查的 DockWidgetBase 指针。
     * @return 指向 SideBar 的指针；如果部件不在侧边栏中，则返回 nullptr。
     */
    SideBar *sideBarForDockWidget(const DockWidgetBase *dw) const;

    /**
     * @brief 返回在 MDI 布局中当前正在被调整大小的 Frame。
     * @return 指向正在调整大小的 Frame 的指针；如果没有，则返回 nullptr。
     */
    Frame *frameInMDIResize() const;

Q_SIGNALS:
    /**
     * @brief 当主窗口或浮动窗口改变其所在的屏幕时发射此信号。
     * @param window 改变了屏幕的 QWindow 指针。
     */
    void windowChangedScreen(QWindow *window);

    /**
     * @brief 当正在 MDI 布局中调整大小的 Frame 发生改变时发射此信号。
     */
    void frameInMDIResizeChanged();

    /**
     * @brief 每当 Config::dropIndicatorsInhibited 的状态改变时发射此信号。
     * @param inhibited 如果禁止显示拖放指示器，则为 true。
     */
    void dropIndicatorsInhibitedChanged(bool inhibited);

protected:
    /**
     * @brief 重写 QObject::eventFilter()，用于拦截和处理其他对象的事件。
     *
     * 主要用于监听应用程序级别的事件，例如焦点变化或应用程序退出。
     * @param watched 被观察的对象。
     * @param event 发生的事件。
     * @return 如果事件被过滤，则返回 true；否则返回 false。
     */
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    friend class FocusScope; // FocusScope 是友元类，可以访问私有成员
    /**
     * @brief 私有构造函数，确保 DockRegistry 只能通过 self() 单例方法创建。
     * @param parent 父 QObject，默认为 nullptr。
     */
    explicit DockRegistry(QObject *parent = nullptr);

    /**
     * @brief 静态回调函数，当停靠部件被按下时调用。
     * @param dw 被按下的 DockWidgetBase。
     * @param event 鼠标事件。
     * @return 如果事件被处理，则返回 true。
     */
    static bool onDockWidgetPressed(DockWidgetBase *dw, QMouseEvent *event);

    /**
     * @brief 当应用程序的焦点对象发生改变时调用的槽函数。
     * @param obj 新的焦点对象。
     */
    void onFocusObjectChanged(QObject *obj);

    /**
     * @brief 尝试延迟删除自身（如果不再需要）。
     *
     * 单例对象通常在应用程序退出时销毁，此方法可能与该过程相关。
     */
    void maybeDelete();

    /**
     * @brief 设置当前拥有焦点的停靠部件。
     * @param dw 指向获得焦点的 DockWidgetBase 的指针。
     */
    void setFocusedDockWidget(DockWidgetBase *dw);

    bool m_isProcessingAppQuitEvent = false;      ///< 标记当前是否正在处理应用程序退出事件。
    DockWidgetBase::List m_dockWidgets;           ///< 存储所有已注册的停靠部件的列表。
    MainWindowBase::List m_mainWindows;           ///< 存储所有已注册的主窗口的列表。
    QList<Frame *> m_frames;                      ///< 存储所有已注册的框架的列表。
    QVector<FloatingWindow *> m_floatingWindows;  ///< 存储所有已注册的浮动窗口的向量。
    QVector<LayoutWidget *> m_layouts;            ///< 存储所有已注册的布局部件的向量。
    QPointer<DockWidgetBase> m_focusedDockWidget; ///< 使用 QPointer 安全地跟踪当前拥有焦点的停靠部件。

    /**
     * @brief 停靠部件 ID 重映射表，由 LayoutSaver 使用。
     *
     * 当 LayoutSaver 尝试恢复一个名为 "foo" 的停靠部件但它不存在时，
     * 它会尝试调用用户提供的工厂函数。该函数可能返回一个具有不同 ID（例如 "bar"）的停靠部件。
     * 发生这种情况时，此 QHash 会添加一个 "foo" : "bar" 的条目。
     * 声明为 mutable 是因为它可能在 const 方法中被修改（例如在查找过程中进行延迟填充）。
     */
    mutable QHash<QString, QString> m_dockWidgetIdRemapping;
};

} // namespace KDDockWidgets

#endif // KD_DOCKREGISTRY_P_H
