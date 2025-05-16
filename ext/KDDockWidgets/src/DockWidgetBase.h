/**
 * @file
 * @brief DockWidget (停靠小部件) 的基类，在 QtWidgets 和 QtQuick 技术栈之间共享。
 *
 * @author Sérgio Martins <sergio.martins@kdab.com>
 */

#ifndef KD_DOCKWIDGET_BASE_H
#define KD_DOCKWIDGET_BASE_H

#include "docks_export.h"          // 导入导出宏定义
#include "KDDockWidgets.h"         // KDDockWidgets 公共头文件，包含核心枚举和类型定义
#include "QWidgetAdapter.h"        // QWidget 和 QQuickItem 的适配器类
#include "LayoutSaver.h"           // 布局保存与恢复相关的类

#include <QVector>  // Qt 动态数组容器
#include <memory>   // C++ 标准库智能指针 (例如 std::shared_ptr)

// clazy:excludeall=ctor-missing-parent-argument // clazy 静态分析器指令：排除所有关于构造函数缺少父参数的警告

// 前向声明，用于测试目的
class TestDocks;

namespace KDDockWidgets {

// 前向声明 KDDockWidgets 内部类
class Frame;              // 框架类 (容纳 DockWidgetBase)
class FloatingWindow;     // 浮动窗口类
class DragController;     // 拖拽控制器类
class DockRegistry;       // 停靠小部件注册表类
class LayoutSaver;        // 布局保存器类 (已包含头文件，此处为冗余但无害的前向声明)
class TabWidget;          // 标签页小部件类
class TitleBar;           // 标题栏类
class MainWindowBase;     // 主窗口基类
class StateDragging;      // 拖拽状态类 (来自 DragController)
class FrameQuick;         // QtQuick 版本的 Frame (如果适用)
class DockWidgetQuick;    // QtQuick 版本的 DockWidget
class LayoutWidget;       // 布局小部件基类

/**
 * @brief DockWidget 的基类。DockWidget 和 DockWidgetBase 被拆分为两个类，
 * 以便与 QtQuick 实现共享一些代码，QtQuick 实现也派生自 DockWidgetBase。
 *
 * 用户代码中不应直接实例化此类。请改用 DockWidget (用于 QtWidgets) 或 DockWidgetQuick (用于 QtQuick)。
 */
#ifndef PYTHON_BINDINGS // 条件编译：处理 Pyside 的一个 BUG (PYSIDE-1327)，如果不是为 Python 绑定编译
class DOCKS_EXPORT DockWidgetBase : public QWidgetAdapter // 继承自 QWidgetAdapter，使其可以包装 QWidget 或 QQuickItem
#else // 如果是为 Python 绑定编译 (PYSIDE)
class DOCKS_EXPORT DockWidgetBase : public QWidget // Pyside bug 解决方法：直接继承自 QWidget
#endif
{
    Q_OBJECT // Q_OBJECT 宏，用于启用 Qt 元对象系统特性，如信号和槽

    // Q_PROPERTY 宏，向 Qt 元对象系统暴露属性，使其可以在 QML 中访问或通过属性系统操作
    /// @brief Q_PROPERTY，表示此停靠小部件或其内容是否拥有焦点。
    Q_PROPERTY(bool isFocused READ isFocused NOTIFY isFocusedChanged)
    /// @brief Q_PROPERTY，表示此停靠小部件是否处于浮动状态。可读写。
    Q_PROPERTY(bool isFloating READ isFloating WRITE setFloating NOTIFY isFloatingChanged)
    /// @brief Q_PROPERTY，表示此停靠小部件的唯一名称 (只读)。
    Q_PROPERTY(QString uniqueName READ uniqueName CONSTANT)
    /// @brief Q_PROPERTY，表示此停靠小部件的标题文本。可读写。
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    /// @brief Q_PROPERTY，表示此停靠小部件承载的实际用户界面小部件 (QObject*)。
    Q_PROPERTY(QObject *widget READ widget NOTIFY widgetChanged)
    /// @brief Q_PROPERTY，表示此停靠小部件的选项。可读写。
    Q_PROPERTY(KDDockWidgets::DockWidgetBase::Options options READ options WRITE setOptions NOTIFY optionsChanged)

public:
    /// @brief DockWidgetBase 指针的 QVector 类型定义，方便使用。
    typedef QVector<DockWidgetBase *> List;

    /**
     * @brief DockWidget 的构造时选项，用于控制其行为。
     */
    enum Option {
        Option_None = 0, ///< 无选项，默认行为。
        Option_NotClosable = 1, ///< DockWidget 不能通过界面上的[x]按钮关闭，只能通过编程方式关闭。
        Option_NotDockable = 2, ///< DockWidget 不能被停靠，它总是处于浮动状态。
        Option_DeleteOnClose = 4, ///< 关闭时删除 DockWidget 对象。
        Option_MDINestable = 8  ///< 实验性功能。当此停靠小部件显示在 MDI 区域时，它也允许其他停靠小部件停靠到其侧边或组合成标签页。
                                ///< 通常每个 MDI "窗口"对应一个 DockWidget，使用此选项后，每个 "窗口" 将拥有一个包含一个或多个停靠小部件的布局。
                                ///< 运行 "kddockwidgets_mdi_with_docking_example -n" 查看实际效果。
    };
    Q_DECLARE_FLAGS(Options, Option) // 将 Option 枚举声明为可用于 QFlags 的类型。
    Q_ENUM(Options) // 将 Options 枚举注册到 Qt 元对象系统。

    /**
     * @brief 影响布局保存器 (LayoutSaver) 保存/恢复行为的选项。
     */
    enum class LayoutSaverOption {
        None = 0, ///< 使用默认行为。
        Skip = 1, ///< 此停靠小部件将不参与保存/恢复过程。目前仅适用于浮动窗口。
    };
    Q_DECLARE_FLAGS(LayoutSaverOptions, LayoutSaverOption) // 将 LayoutSaverOption 枚举声明为可用于 QFlags 的类型。

    /**
     * @brief 定义图标可以显示的位置。
     */
    enum class IconPlace {
        TitleBar = 1,     ///< 图标显示在标题栏。
        TabBar = 2,       ///< 图标显示在标签栏。
        ToggleAction = 4, ///< 图标用于切换可见性的 QAction。
        All = ToggleAction | TitleBar | TabBar ///< 图标显示在所有这些位置。
    };
    Q_ENUM(IconPlace) // 将 IconPlace 枚举注册到 Qt 元对象系统。
    Q_DECLARE_FLAGS(IconPlaces, IconPlace) // 将 IconPlace 枚举声明为可用于 QFlags 的类型。

    /**
     * @brief 构造一个新的 DockWidgetBase。
     * @param uniqueName 停靠小部件的名称，应该是唯一的。使用 title 设置用户可见的文本。
     * @param options 控制某些行为的选项。
     * @param layoutSaverOptions 控制保存/恢复的选项。
     *
     * 此构造函数没有 parent 参数。DockWidget 在可见时会作为 FloatingWindow 或 MainWindow 的子控件，
     * 在隐藏时则没有父控件。
     */
    explicit DockWidgetBase(const QString &uniqueName,
                            Options options = KDDockWidgets::DockWidgetBase::Options(),
                            LayoutSaverOptions layoutSaverOptions = KDDockWidgets::DockWidgetBase::LayoutSaverOptions());

    /**
     * @brief 析构函数。
     */
    ~DockWidgetBase() override;

    /**
     * @brief 将 @p other 小部件停靠到此小部件中。如果尚未显示标签页，则会显示。
     * @param other 要停靠到此小部件中的另一个停靠小部件。
     * @param initialOption 允许指定一个 InitialOption。这对于将停靠小部件添加为隐藏状态非常有用，
     * 仅在标签页中记录一个占位符。这样，当它最终显示时，会恢复为标签化状态。
     * @sa MainWindow::addDockWidget(), DockWidget::addDockWidgetToContainingWindow()
     */
    Q_INVOKABLE void addDockWidgetAsTab(KDDockWidgets::DockWidgetBase *other,
                                        KDDockWidgets::InitialOption initialOption = {});

    /**
     * @brief 将 @p other 小部件停靠到包含此小部件的窗口中。
     * 等同于 MainWindow::addDockWidget()，不同之处在于它也支持顶层窗口是
     * FloatingWindow 而不是 MainWindow 的情况。
     *
     * @param other 要停靠到窗口中的另一个停靠小部件。
     * @param location 停靠的位置。
     * @param relativeTo @p location 相对于的停靠小部件。如果为 null，则认为相对于窗口。
     * @param initialOption 允许指定一些在停靠时使用的额外选项。
     * @sa MainWindow::addDockWidget(), DockWidget::addDockWidgetAsTab()
     */
    Q_INVOKABLE void
    addDockWidgetToContainingWindow(KDDockWidgets::DockWidgetBase *other,
                                    KDDockWidgets::Location location,
                                    KDDockWidgets::DockWidgetBase *relativeTo = nullptr,
                                    KDDockWidgets::InitialOption initialOption = {});

    /**
     * @brief 设置此停靠小部件承载的小部件。
     * @param widget 要在此停靠小部件内部显示的小部件。不能为 null。
     *
     * @p widget 的所有权将转移给 DockWidgetBase。
     * 任何先前存在的小部件的所有权将转回给用户。这意味着如果您调用
     * setWidget(A) 后再调用 setWidget(B)，那么 A 将需要由您删除，而 B 则由停靠小部件拥有。
     */
    virtual void setWidget(QWidgetOrQuick *widget);

    /**
     * @brief 返回此停靠小部件承载的小部件。
     * @return 指向 QWidgetOrQuick 对象的指针。
     */
    [[nodiscard]] QWidgetOrQuick *widget() const;

    /**
     * @brief 返回停靠小部件是否处于浮动状态。
     * 浮动意味着它没有停靠并且拥有自己的窗口。
     * 注意，如果您将一个浮动的停靠小部件停靠到另一个浮动的停靠小部件中，
     * 那么它们就不再算作浮动状态，因为它们是并排（或标签化）的。
     * @return 如果是浮动状态，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isFloating() const;

    /**
     * @brief 设置器，用于使停靠小部件浮动或停靠。
     * @param floats 如果为 true，则使停靠小部件浮动；否则将其停靠。
     * @return 如果请求成功完成，则返回 true。
     */
    bool setFloating(bool floats);

    /**
     * @brief 返回允许隐藏/显示停靠小部件的 QAction。
     * 对于放入菜单非常有用。
     * @return 指向 QAction 的指针。
     */
    Q_INVOKABLE [[nodiscard]] QAction *toggleAction() const;

    /**
     * @brief 返回允许停靠/取消停靠停靠小部件的 QAction。
     * 对于放入菜单非常有用。
     * @return 指向 QAction 的指针。
     */
    Q_INVOKABLE [[nodiscard]] QAction *floatAction() const;

    /**
     * @brief 停靠小部件的唯一名称。
     * @internal 内部使用。
     * @return 唯一名称字符串。
     */
    [[nodiscard]] QString uniqueName() const;

    /**
     * @brief 返回停靠小部件的标题。
     * 此标题在标题栏和标签栏中可见。
     * @sa setTitle
     * @return 标题字符串。
     */
    [[nodiscard]] QString title() const;

    /**
     * @brief 设置停靠小部件的标题。
     * @param title 停靠小部件的新标题。
     * @sa title()
     */
    void setTitle(const QString &title);

    /**
     * @brief 返回停靠小部件父框架的几何形状。
     *
     * 这总是比 DockWidget 的尺寸大，因为有边距和标题栏。
     * 此外，一个框架可以包含多个（标签化的）停靠小部件，这意味着几何形状将考虑标签栏和标题栏。
     *
     * 矩形的位置是布局坐标。 (0,0) 是持有这些小部件的布局的左上角。
     * @return 框架的 QRect。
     */
    [[nodiscard]] QRect frameGeometry() const;

    /**
     * @brief 返回控制停靠小部件行为的选项。
     * @sa setOptions(), optionsChanged()
     * @return Options 枚举值的组合。
     */
    [[nodiscard]] Options options() const;

    /**
     * @brief 返回影响 LayoutSaver 的每个停靠小部件的选项。
     * 这些是传递给构造函数的选项。
     * @return KDDockWidgets::DockWidgetBase::LayoutSaverOptions 的组合。
     */
    [[nodiscard]] KDDockWidgets::DockWidgetBase::LayoutSaverOptions layoutSaverOptions() const;

    /**
     * @brief 选项的设置器。
     * 只有 Option_NotClosable 允许在构造后更改。对于其他选项，请仅在构造函数中设置。
     * @sa options(), optionsChanged()
     * @param newOptions 新的选项组合。
     */
    void setOptions(Options newOptions);

    /**
     * @brief 返回此停靠小部件是否被标签化到另一个小部件中。
     *
     * 从技术上讲，一个已停靠的 DockWidget 总是存在于一个标签页小部件中，但从用户的角度来看，
     * 当只有一个停靠小部件时，它不被视为标签化的，因为没有显示标签。除非
     * 该框架使用了 Option_AlwaysShowsTabs，在这种情况下，无论是否是单个小部件，此方法都将返回 true。
     * @return 如果是标签化的，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isTabbed() const;

    /**
     * @brief 如果此停靠小部件是其所在标签页小部件中的当前标签，则返回 true。
     * 如果停靠小部件是单独的，则返回 true，因为在这种情况下，即使标签页小部件不可见，它也仍然存在。
     * @return 如果是当前标签，则返回 true。
     */
    [[nodiscard]] bool isCurrentTab() const;

    /**
     * @brief 使此停靠小部件成为其标签组中的当前标签。
     */
    Q_INVOKABLE void setAsCurrentTab();

    /**
     * @brief 返回此停靠小部件占用的标签页索引。
     * 注意，停靠小部件几乎总是标签化的，即使您看不到标签栏。
     * 单个浮动的停靠小部件仍然是在一个只有一个标签的标签页小部件中标签化的。
     * @return 标签页索引。
     */
    [[nodiscard]] int tabIndex() const;

    /**
     * @brief 返回此停靠小部件所在标签组的当前标签的索引。
     * @return 当前标签页的索引。
     */
    [[nodiscard]] int currentTabIndex() const;

    /**
     * @brief 设置要在标题栏和标签栏上显示的图标。
     * @param icon 要设置的 QIcon 对象。
     * @param places 指定图标将显示的位置 (TitleBar, TabBar, ToggleAction, 或 All)，默认为 All。
     *
     * 默认情况下未设置图标。
     * @sa icon()
     */
    void setIcon(const QIcon &icon, IconPlaces places = IconPlace::All);

    /**
     * @brief 返回停靠小部件的标题栏、标签栏或切换动作的图标（取决于传递的 @p place）。
     *
     * 默认情况下为 null。
     * @param place 要获取图标的位置，默认为 IconPlace::TitleBar。
     * @sa setIcon()
     * @return QIcon 对象。
     */
    [[nodiscard]] QIcon icon(IconPlace place = IconPlace::TitleBar) const;

    /**
     * @brief 类似于 QWidget::close()，但不会询问被宿主的小部件是否应该关闭。
     * 即强制关闭。
     */
    Q_INVOKABLE void forceClose();

    /**
     * @brief 返回此停靠小部件的标题栏。
     *
     * 注意，如果多个停靠小部件组合成标签页，它们可以共享同一个标题栏。
     * 隐藏的停靠小部件没有关联的标题栏。
     * @return 指向 TitleBar 对象的指针，如果当前不可见或无关联标题栏则为 nullptr。
     */
    [[nodiscard]] TitleBar *titleBar() const;

    /**
     * @brief 返回此停靠小部件是否打开（可见）。
     * 等同于调用 toggleAction()->isChecked() 或 isVisible()。
     * @return 如果已打开，则返回 true。
     */
    Q_INVOKABLE [[nodiscard]] bool isOpen() const;

    /**
     * @brief 设置亲和性名称。停靠小部件只能停靠到具有相同亲和性的停靠小部件中。
     *
     * 默认情况下，亲和性为空，停靠小部件可以停靠到任何主窗口和任何浮动窗口中。
     * 通常您不需要调用此函数，除非您有某些停靠小部件只能停靠到某些其他停靠小部件和主窗口的特定需求。
     * @sa MainWindowBase::setAffinities()。
     *
     * 注意：在创建停靠小部件后，添加到主窗口之前以及恢复任何布局之前立即调用此函数。
     * 注意：目前只能调用此函数一次，以保持代码简单并避免边缘情况。
     * 只有在出现需要多次更改亲和性的良好用例时，才会更改此行为。
     *
     * @param names 亲和性名称列表。
     */
    void setAffinities(const QStringList &names);

    /**
     * @deprecated @overload 请使用 setAffinities(const QStringList &) 代替。
     * @param affinity 亲和性名称。
     */
    void setAffinityName(const QString &affinity);

    /**
     * @brief 返回亲和性名称。默认为空。
     * @return 亲和性名称的 QStringList。
     */
    [[nodiscard]] QStringList affinities() const;

    /**
     * @brief 等同于 QWidget::show()，但经过优化以减少某些平台上的闪烁。
     */
    Q_INVOKABLE void show();

    /**
     * @brief 将停靠小部件置于顶层。
     *
     * 这意味着：
     * - 如果停靠小部件与其他停靠小部件标签化在一起，但其标签不是当前标签，则将其设为当前标签。
     * - 如果停靠小部件是浮动的，则调用 QWindow::raise()。
     *
     * 仅当停靠小部件已打开时才适用。如果已关闭，则不执行任何操作。
     */
    Q_INVOKABLE void raise();

    /**
     * @brief 返回 widget() 是否为 KDDockWidget::MainWindow。
     *
     * 此函数与此停靠小部件是否在主窗口内无关，而是相反的概念。
     * 这不是一种非常流行的用法，但某些应用程序希望将一个主窗口停靠到另一个主窗口中。
     * 这是通过首先将其放入停靠小部件中来完成的。
     *
     * 另请参见 kddockwidgets_example -j
     * @return 如果承载的小部件是 MainWindowBase 的实例，则返回 true。
     */
    [[nodiscard]] bool isMainWindow() const;

    /**
     * @brief 返回此停靠小部件是否停靠在主窗口中（而不是浮动的）。
     *
     * 注意，isFloating() 返回 false 可能意味着停靠小部件停靠在主窗口中，
     * 或者停靠在浮动窗口中（与其他停靠小部件组合/嵌套）。
     * 如果需要区分这两种情况，请使用此函数。
     * @return 如果停靠在主窗口内，则返回 true。
     */
    [[nodiscard]] bool isInMainWindow() const;

    /**
     * @brief 返回此停靠小部件所在的主窗口。如果它不在主窗口内，则返回 nullptr。
     * 如果它被最小化到侧边栏，也返回 nullptr。
     * @return 指向 MainWindowBase 的指针，或者 nullptr。
     */
    [[nodiscard]] MainWindowBase *mainWindow() const;

    /**
     * @brief 返回此停靠小部件或其任何子小部件是否拥有焦点。
     * 不要与 QWidget::hasFocus() 混淆，后者仅指单个小部件。此变体包括子小部件。
     * @sa isFocusedChanged()
     * @return 如果拥有焦点，则返回 true。
     */
    [[nodiscard]] bool isFocused() const;

    /**
     * @brief 将此停靠小部件最小化到主窗口的侧边栏。
     *
     * 它将从当前布局中取消停靠。其先前的停靠位置将被记住。
     *
     * 此操作仅在停靠小部件停靠在主窗口中时可用。
     * 停靠小部件最初将可见并覆盖在当前布局之上（这是自动隐藏功能）。
     */
    Q_INVOKABLE void moveToSideBar();

    /**
     * @brief 返回此停靠小部件是否从侧边栏覆盖显示。
     *
     * 仅在使用自动隐藏和侧边栏功能时才有意义。
     * 不要与“浮动”（表示顶层窗口）混淆。
     * @return 如果是覆盖显示的，则返回 true。
     */
    [[nodiscard]] bool isOverlayed() const;

    /**
     * @brief 返回此停靠小部件所在的侧边栏及其位置。
     * 如果不在侧边栏中，则返回 SideBarLocation::None。
     * 仅在使用自动隐藏和侧边栏功能时才有意义。
     * @sa isInSideBar
     * @return SideBarLocation 枚举值。
     */
    [[nodiscard]] SideBarLocation sideBarLocation() const;

    /**
     * @brief 返回此停靠小部件是否在侧边栏中。
     * 类似于 sideBarLocation()，但返回一个布尔值。
     * @return 如果在侧边栏中，则返回 true。
     */
    [[nodiscard]] bool isInSideBar() const;

    /**
     * @brief 返回此浮动停靠小部件是否知道其先前的停靠位置。
     * 结果仅在它是浮动时才有意义。
     *
     * 当您调用 dockWidget->setFloating(false) 时，只有当它知道停靠到哪里时才会停靠。
     * @return 如果有先前的停靠位置记录，则返回 true。
     */
    [[nodiscard]] bool hasPreviousDockedLocation() const;

    /**
     * @brief 返回小部件在覆盖显示时的最后尺寸。
     * 否则为空。
     * @return 最后一次覆盖显示时的 QSize。
     */
    [[nodiscard]] QSize lastOverlayedSize() const;


    /**
     * @brief 根据名称返回一个停靠小部件。
     * 这是您传递给 DockWidget 构造函数的相同名称。
     * 如果未找到停靠小部件，则返回 nullptr。
     * @param uniqueName 停靠小部件的唯一名称。
     * @return 指向 DockWidgetBase 的指针，或 nullptr。
     */
    static DockWidgetBase *byName(const QString &uniqueName);

    /**
     * @brief 返回此小部件是否具有 LayoutSaverOption::Skip 标志。
     * @return 如果设置了 Skip 标志，则返回 true。
     */
    [[nodiscard]] bool skipsRestore() const;

    /**
     * @brief 如果此停靠小部件是浮动的，则将其几何形状设置为 @p geo。
     *
     * 如果此停靠小部件是隐藏的，则它会存储该几何形状，以便在下次变为浮动时使用。
     *
     * 这只是一个便利函数，等同于调用 window()->setGeometry(rect)，
     * 额外的好处是如果它仍然隐藏，会记住请求的几何形状。
     * @param geo 新的浮动几何形状。
     */
    void setFloatingGeometry(QRect geo);

    /**
     * @brief 允许用户为此停靠小部件设置一个类型。
     * 该类型是不透明的，KDDockWidgets 不会对其进行解释。
     * 此类型会传递给 FrameWorkWidgetFactory::createTitleBar()，用户可以重写该函数，
     * 并根据类型返回不同的 TitleBar 子类。
     * @param userType 用户定义的整数类型。
     */
    void setUserType(int userType);
    /**
     * @brief 获取用户为此停靠小部件设置的类型。
     * @return 用户定义的整数类型。
     */
    [[nodiscard]] int userType() const;

    /**
     * @brief 在 MDI 布局中将此停靠小部件的位置设置为 pos。
     * 仅当主窗口处于 MDI 模式时才适用，默认情况下不是。
     * @param pos 在 MDI 布局中的新位置。
     */
    void setMDIPosition(QPoint pos);
    /**
     * @brief 类似于 setMDIPosition()，但用于设置尺寸。
     * @param size 在 MDI 布局中的新尺寸。
     */
    void setMDISize(QSize size);
    /**
     * @brief 类似于 setMDIPosition()，但用于设置 Z 顺序。
     * 仅为 QtQuick 实现。
     * @param z 新的 Z 顺序值。
     */
    static void setMDIZ(int z);

    /**
     * @brief 返回此停靠小部件是否是主窗口的持久中央小部件。
     * 仅当使用 MainWindowOption_HasCentralWidget 时才适用。
     * @return 如果是持久中央小部件，则返回 true。
     */
    [[nodiscard]] bool isPersistentCentralDockWidget() const;

    /**
     * @brief 设置期望的浮动窗口标志，以防默认值不符合期望。
     * 默认情况下，KDDW 将使用 KDDockWidgets::Config 中指定的标志来影响窗口的许多方面
     * （例如最小化、最大化支持）。
     * 您可以使用此函数覆盖那些“全局标志”，并为每个停靠小部件指定标志。
     * 请确保不要在浮动窗口已经创建之后调用此函数。建议在显示停靠小部件之前调用它。
     * @param flags 要设置的 FloatingWindowFlags。
     */
    void setFloatingWindowFlags(FloatingWindowFlags flags);
    /**
     * @brief 获取此停靠小部件的浮动窗口标志。
     * @return KDDockWidgets::FloatingWindowFlags。
     */
    [[nodiscard]] KDDockWidgets::FloatingWindowFlags floatingWindowFlags() const;

Q_SIGNALS: // 信号部分
#ifdef KDDOCKWIDGETS_QTWIDGETS
    /**
     * @brief 当父对象改变时发射此信号。
     * QtQuick 已经有 QQuickItem::parentChanged()，所以这里仅为 QtWidgets 添加。
     */
    void parentChanged();
#endif

    /**
     * @brief 当 DockWidget 显示时发射此信号（如 QEvent::Show）。
     */
    void shown();

    /**
     * @brief 当 DockWidget 隐藏时发射此信号（如 QEvent::Hide）。
     */
    void hidden();

    /**
     * @brief 当图标改变时发射此信号。
     */
    void iconChanged();

    /**
     * @brief 当标题改变时发射此信号。
     * @param title 新的标题。
     */
    void titleChanged(const QString &title);

    /**
     * @brief 当承载的小部件改变时发射此信号。
     * @param widget 指向新的 QWidgetOrQuick 对象的指针。
     */
    void widgetChanged(KDDockWidgets::QWidgetOrQuick *widget);

    /**
     * @brief 当选项改变时发射此信号。
     * @sa setOptions(), options()
     * @param newOptions 新的选项。
     */
    void optionsChanged(KDDockWidgets::DockWidgetBase::Options newOptions);

    /**
     * @brief 当 isFocused 属性改变时发射此信号。
     * @sa isFocused
     * @param focused 新的焦点状态。
     */
    void isFocusedChanged(bool focused);

    /**
     * @brief 当 isOverlayed 属性改变时发射此信号。
     * @sa isOverlayed
     * @param overlayed 新的覆盖显示状态。
     */
    void isOverlayedChanged(bool overlayed);

    /**
     * @brief 当 isFloating 属性改变时发射此信号。
     * @param floating 新的浮动状态。
     */
    void isFloatingChanged(bool floating);

    /**
     * @brief 当此停靠小部件从侧边栏移除时发射此信号。
     * 仅与自动隐藏/侧边栏功能相关。
     */
    void removedFromSideBar();

    /**
     * @brief 当此停靠小部件所在的顶层窗口被激活或停用时发射。
     * 这是为了替代跟踪 dockWidget->window() 的便利信号，因为在停靠和取消停靠时窗口会改变。
     *
     * 之所以称为 'aboutTo'，是因为它是在事件过滤器中完成的，此时目标窗口的 'activeWindow' 属性尚未更新。
     * @param activated 如果窗口即将被激活，则为 true；如果即将被停用，则为 false。
     */
    void windowActiveAboutToChange(bool activated);

    /**
     * @brief 当服务于此停靠小部件的标题栏发生变化时发射。
     */
    void actualTitleBarChanged();

    /**
     * @brief 当此停靠小部件由于 Option_DeleteOnClose 即将被删除时发射。
     */
    void aboutToDeleteOnClose();

protected:
    /**
     * @brief 当父对象改变时的回调函数。
     */
    void onParentChanged();
    /**
     * @brief 当小部件显示时的回调函数。
     * @param spontaneous 如果显示是自发的（例如，不是由显式 show() 调用引起的），则为 true。
     */
    void onShown(bool spontaneous);
    /**
     * @brief 当小部件隐藏时的回调函数。
     * @param spontaneous 如果隐藏是自发的，则为 true。
     */
    void onHidden(bool spontaneous);

#ifndef PYTHON_BINDINGS // 条件编译：处理 Pyside 的 BUG
    /**
     * @brief 处理关闭事件 (QWidget 虚函数重写)。
     * @param e 关闭事件对象。
     */
    void onCloseEvent(QCloseEvent *e) override;
    /**
     * @brief 处理大小调整事件 (QWidgetAdapter 虚函数重写)。
     * @param newSize 新的尺寸。
     * @return 如果接受新尺寸则返回 true。
     */
    bool onResize(QSize newSize) override;
#endif

#if defined(DOCKS_DEVELOPER_MODE) // 条件编译：仅在开发者模式下，以下成员为 public
public:
#else // 否则为 private
private:
#endif
    Q_DISABLE_COPY(DockWidgetBase) ///< 禁止拷贝构造函数和拷贝赋值操作符。
    // 声明友元类，允许它们访问此类的私有成员
    friend class MultiSplitter;
    friend class LayoutWidget;
    friend class MDILayoutWidget;
    friend class FloatingWindow;
    friend class Frame;
    friend class DropArea;
    friend class ::TestDocks; // 测试类
    friend class StateDragging;
    friend class KDDockWidgets::TabWidget;
    friend class KDDockWidgets::TitleBar;
    friend class KDDockWidgets::DragController;
    friend class KDDockWidgets::DockRegistry;
    friend class KDDockWidgets::LayoutSaver;
    friend class KDDockWidgets::MainWindowBase;
    friend class KDDockWidgets::FrameQuick;
    friend class KDDockWidgets::DockWidgetQuick;

    /**
     * @brief 从其序列化形式构造一个停靠小部件。
     * @internal 内部使用。
     * @param data 指向包含序列化数据的 LayoutSaver::DockWidget 共享指针。
     * @return 指向新创建并恢复状态的 DockWidgetBase 的指针。
     */
    static DockWidgetBase *deserialize(const std::shared_ptr<LayoutSaver::DockWidget> &data);


    class Private;   ///< PIMPL (Private Implementation) 设计模式的私有实现类前向声明。
    Private *const d; ///< 指向私有实现类的常量指针。

    /**
     * @brief 获取指向私有实现类 (PIMPL) 的指针。
     * @return 指向 Private 实例的指针。
     */
    [[nodiscard]] Private *dptr() const;
};

} // namespace KDDockWidgets

// 将 KDDockWidgets::Location 类型注册到 Qt 元对象系统，以便可以在信号/槽机制和属性系统中使用。
Q_DECLARE_METATYPE(KDDockWidgets::Location)

#endif // KD_DOCKWIDGET_BASE_H
