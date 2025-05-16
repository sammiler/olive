#ifndef KD_DOCKWIDGET_BASE_P_H
#define KD_DOCKWIDGET_BASE_P_H

#include "DockWidgetBase.h"
#include "SideBar_p.h"
#include "DockRegistry_p.h"
#include "Position_p.h"
#include "FloatingWindow_p.h"

#include <QCoreApplication>
#include <QString>
#include <QSize>

QT_BEGIN_NAMESPACE
class QAction; // 前向声明 QAction 类
QT_END_NAMESPACE

namespace KDDockWidgets {

// DOCKS_EXPORT_FOR_UNIT_TESTS 宏用于单元测试时导出符号
// clazy:exclude=missing-qobject-macro clazy 静态分析器排除缺少 Q_OBJECT 宏的警告
/**
 * @brief DockWidgetBase 的私有实现类。
 *
 * 该类继承自 QObject，封装了 DockWidgetBase 的内部逻辑和数据。
 * 此处使用 PIMPL (Private Implementation) 设计模式。
 */
class DOCKS_EXPORT_FOR_UNIT_TESTS DockWidgetBase::Private : public QObject
{
public:
    /**
     * @brief 构造函数。
     * @param dockName 此停靠小部件的名称，用于序列化和恢复布局。
     * @param options_ 此停靠小部件的选项。
     * @param layoutSaverOptions_ 布局保存器的选项。
     * @param qq 指向公开类 DockWidgetBase 的指针（q-pointer）。
     */
    Private(const QString &dockName, DockWidgetBase::Options options_,
            LayoutSaverOptions layoutSaverOptions_, DockWidgetBase *qq);

    /**
     * @brief 初始化函数。
     *
     * 在构造之后调用，用于执行一些初始化操作，例如更新标题。
     */
    void init() const
    {
        updateTitle(); // 调用更新标题的函数
    }

    /**
     * @brief 返回此停靠小部件所在的 FloatingWindow（浮动窗口）。
     *
     * 如果返回 nullptr，则表示此停靠小部件位于 MainWindow 中。
     * 注意：即使在浮动窗口中，isFloating() 也可能返回 false，
     * 因为停靠小部件可能与浮动窗口中的其他停靠小部件并排显示。
     * @return 如果在浮动窗口中，则返回指向 FloatingWindow 的指针；否则返回 nullptr。
     */
    [[nodiscard]] FloatingWindow *floatingWindow() const
    {
        return qobject_cast<FloatingWindow *>(q->window()); // 获取停靠小部件的顶层窗口并尝试转换为 FloatingWindow
    }

    /**
     * @brief 返回此停靠小部件所在的 MainWindowBase（主窗口基类）。
     * @return 如果在主窗口中，则返回指向 MainWindowBase 的指针；否则返回 nullptr。
     */
    [[nodiscard]] MainWindowBase *mainWindow() const
    {
        if (q->isWindow()) // 如果停靠小部件本身是一个窗口（例如浮动状态），则它不在主窗口内
            return nullptr;

        // 注意：不要简单地使用 window()，因为 MainWindow 可能嵌入到其他东西中
        QWidgetOrQuick *p = q->parentWidget(); // 获取父小部件
        while (p) {
            if (auto window = qobject_cast<MainWindowBase *>(p)) // 尝试将父小部件转换为 MainWindowBase
                return window;

            if (p->isWindow()) // 如果父小部件是一个独立的窗口但不是 MainWindowBase，则停止搜索
                return nullptr;

            p = p->parentWidget(); // 继续向上查找父小部件
        }

        return nullptr; // 未找到主窗口
    }

    /**
     * @brief 返回此停靠小部件关联的 SideBar（侧边栏）。
     * @return 指向 SideBar 的指针，如果没有关联的侧边栏则可能为 nullptr。
     */
    [[nodiscard]] SideBar *sideBar() const
    {
        return DockRegistry::self()->sideBarForDockWidget(q); // 从停靠注册表中获取侧边栏
    }

    /**
     * @brief 添加包含此停靠小部件的当前布局项。
     * @param item 指向布局项的指针。
     */
    void addPlaceholderItem(Layouting::Item *);

    /**
     * @brief 返回最后的位置信息，主要用于测试。
     * @return 对最后位置智能指针的引用。
     */
    Position::Ptr &lastPosition();

    /**
     * @brief 强制关闭停靠小部件。
     *
     * 这通常用于内部逻辑，绕过一些正常的关闭检查。
     */
    void forceClose();
    /**
     * @brief 获取浮动状态下的默认中心位置。
     * @return 浮动时的默认中心点。
     */
    [[nodiscard]] QPoint defaultCenterPosForFloating() const;

    /**
     * @brief 事件过滤器。
     *
     * 用于监视和处理其他对象的事件。
     * @param watched 被监视的对象。
     * @param event 发生的事件。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    bool eventFilter(QObject *watched, QEvent *event) override;

    /**
     * @brief 更新停靠小部件的标题。
     *
     * 通常在标题文本或图标更改时调用。
     */
    void updateTitle() const;
    /**
     * @brief 切换停靠小部件的可见性。
     * @param enabled 如果为 true，则显示；否则隐藏。
     */
    void toggle(bool enabled) const;
    /**
     * @brief 更新切换动作（QAction）的状态。
     *
     * 例如，更新其选中状态以反映停靠小部件的可见性。
     */
    void updateToggleAction();
    /**
     * @brief 更新浮动动作（QAction）的状态。
     *
     * 例如，更新其文本或选中状态以反映停靠小部件的浮动状态。
     */
    void updateFloatAction();
    /**
     * @brief 当停靠小部件显示时调用的处理函数。
     */
    void onDockWidgetShown();
    /**
     * @brief 当停靠小部件隐藏时调用的处理函数。
     */
    void onDockWidgetHidden();
    /**
     * @brief 显示停靠小部件。
     */
    void show() const;
    /**
     * @brief 关闭停靠小部件。
     */
    void close();
    /**
     * @brief 尝试将停靠小部件恢复到其先前的位置。
     * @return 如果成功恢复，则返回 true；否则返回 false。
     */
    bool restoreToPreviousPosition();
    /**
     * @brief 可能会将停靠小部件恢复到其先前的位置。
     *
     * 通常在某些条件下触发，例如从浮动状态变回停靠状态。
     */
    void maybeRestoreToPreviousPosition();
    /**
     * @brief 获取当前标签页的索引。
     * @return 如果停靠小部件在标签组中，则返回其索引；否则可能返回-1或无效值。
     */
    [[nodiscard]] int currentTabIndex() const;

    /**
     * @brief 将此停靠小部件序列化为一个中间形式。
     * @return 表示此停靠小部件状态的共享指针。
     */
    [[nodiscard]] std::shared_ptr<LayoutSaver::DockWidget> serialize() const;

    /**
     * @brief 返回包含此停靠小部件的 Frame（框架）。
     *
     * Frame 包装了一个停靠的 DockWidget，为其提供一个 TabWidget，以便它可以接受其他停靠小部件。
     * Frame 也是实际进入 LayoutWidget 的类。
     *
     * 在创建后立即为 nullptr。
     * @return 指向 Frame 的指针，如果不存在则为 nullptr。
     */
    [[nodiscard]] Frame *frame() const;

    /**
     * @brief 如果此停靠小部件正在浮动，则保存其几何形状（位置和大小）。
     */
    void saveLastFloatingGeometry();

    /**
     * @brief 在浮动停靠小部件之前保存其位置。
     *
     * 这样在调用 DockWidget::setFloating(false) 时可以恢复它。
     */
    void saveTabIndex();

    /**
     * @brief 创建一个 FloatingWindow 并将自身添加到其中。
     * @return 创建的 FloatingWindow 的指针。
     */
    KDDockWidgets::FloatingWindow *morphIntoFloatingWindow();

    /**
     * @brief 如果停靠小部件可见且是顶层窗口，则调用 morphIntoFloatingWindow()。
     *
     * 当我们显示一个浮动停靠小部件时，会延迟调用此函数，以便获得一个 FloatingWindow。
     */
    void maybeMorphIntoFloatingWindow();

    /**
     * @brief 返回此停靠小部件所在的 MDI (多文档界面) 布局，如果有的话。
     * @return 指向 MDILayoutWidget 的指针，如果不在 MDI 布局中则为 nullptr。
     */
    [[nodiscard]] MDILayoutWidget *mdiLayout() const;

    /**
     * @brief 返回这是否是一个为在 MDI 内部托管放置区域而自动创建的辅助 DockWidget。
     *
     * 这仅由 DockWidget::Option_MDINestable 功能使用。
     * @return 如果是 MDI 包装器，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isMDIWrapper() const;

    /**
     * @brief 如果此停靠小部件是 MDI 包装器 (isMDIWrapper() 返回 true)，则返回包装器的放置区域。
     * @return 指向 DropArea 的指针，如果不是 MDI 包装器或没有放置区域则为 nullptr。
     */
    [[nodiscard]] DropArea *mdiDropAreaWrapper() const;

    /**
     * @brief 如果此停靠小部件位于嵌套在 MDI 中的放置区域内，则返回包装器停靠小部件。
     *
     * 此函数向上遍历层次结构，而 mdiDropAreaWrapper 向下遍历。
     * @return 指向包装器 DockWidgetBase 的指针，如果不存在则为 nullptr。
     */
    [[nodiscard]] DockWidgetBase *mdiDockWidgetWrapper() const;

    /// @brief 停靠小部件的名称，用于识别和序列化。
    const QString name;
    /// @brief 此停靠小部件的亲和性列表，用于控制哪些其他小部件可以与其停靠在一起。
    QStringList affinities;
    /// @brief 停靠小部件的标题文本。
    QString title;
    /// @brief 标题栏图标。
    QIcon titleBarIcon;
    /// @brief 标签栏图标。
    QIcon tabBarIcon;
    /// @brief 停靠小部件实际承载的用户界面内容 (可以是 QWidget 或 QQuickItem)。
    QWidgetOrQuick *widget = nullptr;
    /// @brief 指向公开类 DockWidgetBase 的指针 (q-pointer)。
    DockWidgetBase *const q;
    /// @brief 停靠小部件的选项。
    DockWidgetBase::Options options;
    /// @brief 浮动窗口的标志位，控制浮动窗口的行为。默认为从全局配置获取。
    FloatingWindowFlags m_flags = FloatingWindowFlag::FromGlobalConfig;
    /// @brief 布局保存器的选项。
    const LayoutSaverOptions layoutSaverOptions;
    /// @brief 用于切换停靠小部件可见性的 QAction。
    QAction *const toggleAction;
    /// @brief 用于切换停靠小部件浮动状态的 QAction。
    QAction *const floatAction;
    /// @brief 存储停靠小部件最后的位置信息。
    Position::Ptr m_lastPosition = std::make_shared<Position>();
    /// @brief 标记是否为持久的中央停靠小部件。
    bool m_isPersistentCentralDockWidget = false;
    /// @brief 标记是否正在处理切换动作。
    bool m_processingToggleAction = false;
    /// @brief 标记是否正在更新切换动作的状态。
    bool m_updatingToggleAction = false;
    /// @brief 标记是否正在更新浮动动作的状态。
    bool m_updatingFloatAction = false;
    /// @brief 标记是否正在被强制关闭。
    bool m_isForceClosing = false;
    /// @brief 标记是否正在移动到侧边栏。
    bool m_isMovingToSideBar = false;
    /// @brief 记录最后一次叠加层的大小，用于拖放指示。
    QSize m_lastOverlayedSize = QSize(0, 0);
    /// @brief 用户定义的类型，可用于存储额外信息。
    int m_userType = 0;
};
}

#if defined(QT_WIDGETS_LIB) // 如果定义了 QT_WIDGETS_LIB (即使用 Qt Widgets 模块)
#include <QAction> // 包含标准的 QAction 头文件
#else // 否则 (可能用于 Qt Quick 或其他环境)
// 为 QtQuick 提供的 QAction 替代品。目前仅为编译通过。
/**
 * @brief QtQuick 环境下 QAction 的一个简化版本。
 *
 * 这个类提供了 QAction 的一些基本功能，以便在没有完整 Qt Widgets 模块的环境中编译。
 * 注意：这可能不是一个功能完整的 QAction 实现。
 */
class QAction : public QObject
{
    Q_OBJECT // Q_OBJECT 宏，用于元对象系统支持 (信号和槽等)
public:
    /**
     * @brief 构造函数。
     * @param parent 父对象。
     */
    using QObject::QObject; // 继承 QObject 的构造函数

    /**
     * @brief 检查动作是否被选中。
     * @return 如果被选中，则返回 true；否则返回 false。
     */
    bool isChecked() const
    {
        return m_isChecked;
    }

    /**
     * @brief 设置动作是否可选中。
     * @param is 如果为 true，则可选中；否则不可选中。
     */
    void setCheckable(bool is)
    {
        m_isCheckable = is;
    }

    /**
     * @brief 设置动作的文本。
     * @param text 要设置的文本。
     */
    void setText(const QString &text)
    {
        m_text = text;
    }

    /**
     * @brief 设置动作的工具提示。
     * @param text 工具提示文本。
     */
    void setToolTip(const QString &text)
    {
        m_toolTip = text;
    }

    /**
     * @brief 获取动作的工具提示。
     * @return 工具提示文本。
     */
    QString toolTip() const
    {
        return m_toolTip;
    }

    /**
     * @brief 检查动作是否启用 (已废弃，请使用 isEnabled)。
     * @return 如果启用，则返回 true；否则返回 false。
     */
    bool enabled() const
    {
        return m_enabled;
    }

    /**
     * @brief 设置动作是否启用。
     * @param enabled 如果为 true，则启用；否则禁用。
     */
    void setEnabled(bool enabled)
    {
        m_enabled = enabled;
    }

    /**
     * @brief 检查动作是否被选中 (已废弃，请使用 isChecked)。
     * @return 如果被选中，则返回 true；否则返回 false。
     */
    bool checked() const
    {
        return m_checked;
    }

    /**
     * @brief 设置动作的选中状态。
     * @param checked 如果为 true，则设为选中；否则取消选中。
     */
    void setChecked(bool checked)
    {
        m_checked = checked;
    }

    /**
     * @brief 检查动作是否启用。
     * @return 如果启用，则返回 true；否则返回 false。
     */
    bool isEnabled() const
    {
        return m_enabled;
    }

    /**
     * @brief 切换动作的启用状态。
     *
     * 如果当前是启用状态，则变为禁用；反之亦然。
     * 同时会发出 toggled 信号。
     */
    void toggle()
    {
        m_enabled = !m_enabled; // 切换启用状态
        Q_EMIT toggled(m_enabled); // 发出 toggled 信号
    }

Q_SIGNALS: // 信号部分
    /**
     * @brief 当动作的切换状态改变时发出此信号。
     * @param checked 当前的选中状态 (对于这个简化版，实际是启用状态)。
     */
    bool toggled(bool);

private: // 私有成员变量
    /// @brief 动作的文本。
    QString m_text;
    /// @brief 动作的工具提示文本。
    QString m_toolTip;

    /// @brief 标记动作是否被选中。
    bool m_isChecked = false;
    /// @brief 标记动作是否可选中。
    bool m_isCheckable = false;
    /// @brief 标记动作是否启用。
    bool m_enabled = false;
    /// @brief 标记动作的选中状态（可能与 m_isChecked 重复，取决于具体用法）。
    bool m_checked = false;
};

#endif // QT_WIDGETS_LIB

#endif // KD_DOCKWIDGET_BASE_P_H