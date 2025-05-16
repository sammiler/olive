#ifndef OBJECTVIEWER_H
#define OBJECTVIEWER_H

#ifdef KDDOCKWIDGETS_QTWIDGETS // 条件编译：仅当定义了 KDDOCKWIDGETS_QTWIDGETS 时才编译以下内容 (表示使用 Qt Widgets)

#include <QWidget>             // Qt Widget 基类
#include <QStandardItemModel>  // Qt 标准项模型，用于树视图或列表视图
#include <QTreeView>           // Qt 树视图控件
#include <QPointer>            // Qt QObject 指针的弱引用，用于安全地跟踪对象生命周期
#include <QObject>             // Qt 对象模型基类
#include <QMenu>               // Qt 菜单类

QT_BEGIN_NAMESPACE
// 前向声明 Qt 类
class QStandardItem; // Qt 标准项，用于模型中的单个项
QT_END_NAMESPACE


namespace KDDockWidgets::Debug { // KDDockWidgets 库的 Debug 命名空间

/**
 * @brief 一个用于显示对象树的树形视图小部件。
 *
 * 主要用于调试目的，尤其是在没有 GammaRay 这类高级调试工具的场景下。
 * 它允许开发者查看应用程序中 QObject 的层级结构。
 * clazy:exclude=missing-qobject-macro - clazy 静态分析器排除缺少 Q_OBJECT 宏的警告 (对于非 Q_OBJECT 派生但作为 QWidget 使用的情况)
 */
class ObjectViewer : public QWidget
{
    // 注意：此类没有 Q_OBJECT 宏，因此不直接支持信号和槽的完整元对象系统特性，
    // 但作为 QWidget，它仍然可以处理事件和显示。
public:
    /**
     * @brief 构造函数。
     * @param parent 父 QWidget 对象，默认为 nullptr。
     */
    explicit ObjectViewer(QWidget *parent = nullptr);

    /**
     * @brief 刷新对象树视图，重新加载并显示当前的对象层级。
     */
    void refresh();

private:
    /**
     * @brief 将当前选中的小部件（如果是 QWidget）的内容转储为 PNG 图片。
     */
    void dumpSelectedWidgetToPng();
    /**
     * @brief 更新与当前选中项相关的小部件的状态或高亮显示。
     */
    void updateSelectedWidget();
    /**
     * @brief 切换选中对象的可见性（如果它是 QWidget）。
     */
    void toggleVisible();
    /**
     * @brief (静态方法) 转储所有顶层窗口的信息。
     */
    static void dumpWindows();
    /**
     * @brief (静态方法) 获取指定 QObject 的名称，用于在树视图中显示。
     * @param o 指向 QObject 的指针。
     * @return 对象的名称字符串。
     */
    static QString nameForObj(QObject *o);
    /**
     * @brief 将指定的 QObject 及其子对象递归地添加到树视图模型中。
     * @param obj 要添加的 QObject 对象。
     * @param parentItem 此对象在树视图中的父 QStandardItem。
     */
    void add(QObject *obj, QStandardItem *parentItem);
    /**
     * @brief 从树视图模型中移除指定的 QObject 对应的项。
     * @param obj 要移除的 QObject 对象。
     */
    void remove(QObject *obj);
    /**
     * @brief 当树视图中的选中项发生变化时调用的槽函数或处理函数。
     */
    void onSelectionChanged();
    /**
     * @brief (静态方法) 打印指定 QObject 的属性信息。
     * @param obj 指向要打印属性的 QObject 的指针。
     */
    static void printProperties(QObject *obj);
    /**
     * @brief 获取当前在树视图中选中的 QObject 对象。
     * @return 指向选中 QObject 的指针，如果未选中或选中项无效则返回 nullptr。
     */
    [[nodiscard]] QObject *selectedObject() const;
    /**
     * @brief 获取当前在树视图中选中的 QWidget 对象（如果选中项是 QWidget）。
     * @return 指向选中 QWidget 的指针，如果未选中或选中项不是 QWidget 则返回 nullptr。
     */
    [[nodiscard]] QWidget *selectedWidget() const;
    /**
     * @brief 更新树视图中指定项的外观（例如，根据对象状态）。
     * @param item 指向要更新外观的 QStandardItem。
     */
    void updateItemAppearence(QStandardItem *item);
    /**
     * @brief (静态方法) 根据给定的 QStandardItem 获取其关联的 QObject。
     * @param item 指向 QStandardItem 的指针。
     * @return 指向关联的 QObject 的指针，如果无关联则返回 nullptr。
     */
    static QObject *objectForItem(QStandardItem *item);
    /**
     * @brief (静态方法) 根据给定的 QStandardItem 获取其关联的 QWidget。
     * @param item 指向 QStandardItem 的指针。
     * @return 指向关联的 QWidget 的指针，如果无关联或不是 QWidget 则返回 nullptr。
     */
    static QWidget *widgetForItem(QStandardItem *item);

#ifdef Q_OS_WIN
    /**
     * @brief (仅 Windows) 发送命中测试 (Hit Test) 消息。
     * 可能用于模拟或触发 Windows 特定的窗口消息处理，以进行调试。
     */
    void sendHitTest();
#endif

    QTreeView m_treeView;             ///< 用于显示对象层级结构的树视图控件。
    QStandardItemModel m_model;       ///< 树视图使用的数据模型。
    QPointer<QObject> m_selectedObject; ///< 指向当前选中的 QObject 的弱指针，用于安全访问。
    QMenu m_menu;                     ///< 右键上下文菜单。
    bool m_highlightsWidget = true;   ///< 标记是否高亮显示选中的小部件。
    bool m_ignoreMenus = true;        ///< 标记在构建对象树时是否忽略 QMenu 对象。
    bool m_ignoreShortcuts = true;    ///< 标记在构建对象树时是否忽略 QShortcut 对象。
    bool m_ignoreToolBars = true;     ///< 标记在构建对象树时是否忽略 QToolBar 对象。
    QHash<QObject *, QStandardItem *> m_itemMap; ///< QObject 指针到其在模型中对应 QStandardItem 的映射，用于快速查找。

protected:
    /**
     * @brief 处理上下文菜单事件（通常是右键点击）。
     * @param event 上下文菜单事件对象。
     */
    void contextMenuEvent(QContextMenuEvent *event) override;
    /**
     * @brief 事件过滤器，用于拦截和处理其他对象的事件。
     * @param watched 被监视的对象。
     * @param event 发生的事件。
     * @return 如果事件被处理，则返回 true；否则返回 false 以继续传递事件。
     */
    bool eventFilter(QObject *watched, QEvent *event) override;
};
} // namespace KDDockWidgets::Debug

#endif // KDDOCKWIDGETS_QTWIDGETS

#endif // OBJECTVIEWER_H