#ifndef KD_LAYOUTSAVER_P_H
#define KD_LAYOUTSAVER_P_H

#include "kddockwidgets/KDDockWidgets.h" // KDDockWidgets 公共头文件，包含枚举和基本类型
#include "kddockwidgets/LayoutSaver.h" // LayoutSaver 公共接口头文件
#include "kddockwidgets/QWidgetAdapter.h" // QWidget 和 QQuickItem 的适配器类

#include <QDebug> // Qt 调试输出类
#include <QGuiApplication> // Qt GUI 应用程序类，用于访问屏幕信息等
#include <QJsonDocument> // Qt JSON 文档处理类
#include <QRect> // Qt 矩形类
#include <QScreen> // Qt 屏幕信息类
#include <QSettings> // Qt 设置存储类 (尽管这里主要用 JSON，但可能用于某些配置)

#include <memory> // C++ 标准库智能指针

/**
 * @brief 每当序列化格式发生变化时，增加此版本号，以便仍然可以加载旧的布局。
 * 版本 1: 初始版本
 * 版本 2: 引入了 MainWindow::screenSize 和 FloatingWindow::screenSize
 * 版本 3: 新的布局引擎
 */
#define KDDOCKWIDGETS_SERIALIZATION_VERSION 3


namespace KDDockWidgets {

// 前向声明
class FloatingWindow; // 浮动窗口类
class DockRegistry; // 停靠小部件注册表类

/**
 * @brief KDDockWidgets::RestoreOption 的一个更细粒度的版本。
 * 有些粒度我们不想暴露给所有用户，但希望允许某些用户使用。
 * 一旦这些选项得到验证，我们可能会将更多选项公开，目前它们是内部的。
 */
enum class InternalRestoreOption {
    None = 0, ///< 无特殊选项。
    SkipMainWindowGeometry = 1, ///< 恢复时不要重新定位主窗口的几何形状。
    RelativeFloatingWindowGeometry = 2 ///< 浮动窗口相对于新的主窗口大小进行重新定位。
};
Q_DECLARE_FLAGS(InternalRestoreOptions, InternalRestoreOption) // 将 InternalRestoreOption 声明为可用于 QFlags 的枚举


/**
 * @brief 模板函数，从 QVariantList 转换为特定类型的列表。
 * @tparam T 目标结构体类型，该类型应包含 fromVariantMap 方法。
 * @param listV 包含 QVariantMap 的 QVariantList。
 * @return 转换后的特定类型列表。
 */
template<typename T>
typename T::List fromVariantList(const QVariantList &listV)
{
    typename T::List result; // 存储结果的列表

    result.reserve(listV.size()); // 预分配空间以提高效率
    for (const QVariant &v : listV) {
        T t; // 创建目标类型的临时对象
        t.fromVariantMap(v.toMap()); // 从 QVariantMap 初始化对象
        result.push_back(t); // 添加到结果列表
    }

    return result; // 返回转换后的列表
}

/**
 * @brief 模板函数，将特定类型的列表转换为 QVariantList。
 * @tparam T 源结构体类型，该类型应包含 toVariantMap 方法。
 * @param list 要转换的特定类型列表。
 * @return 转换后的 QVariantList，其中每个元素都是一个 QVariantMap。
 */
template<typename T>
QVariantList toVariantList(const typename T::List &list)
{
    QVariantList result; // 存储结果的 QVariantList
    result.reserve(list.size()); // 预分配空间以提高效率
    for (const T &v : list)
        result.push_back(v.toVariantMap()); // 将每个对象转换为 QVariantMap 并添加到结果列表
    return result; // 返回转换后的 QVariantList
}

/**
 * @brief 代表布局中占位符的信息。
 *
 * 占位符用于标记一个停靠小部件在布局中的位置，尤其是在多文档界面 (MDI) 或复杂嵌套布局中。
 */
struct LayoutSaver::Placeholder
{
    /// @brief Placeholder 对象的 QVector 类型定义。
    typedef QVector<LayoutSaver::Placeholder> List;

    /**
     * @brief 将 Placeholder 对象序列化为 QVariantMap。
     * @return 包含 Placeholder 数据的 QVariantMap。
     */
    [[nodiscard]] QVariantMap toVariantMap() const;
    /**
     * @brief 从 QVariantMap 反序列化数据到 Placeholder 对象。
     * @param map 包含 Placeholder 数据的 QVariantMap。
     */
    void fromVariantMap(const QVariantMap &map);

    bool isFloatingWindow {}; ///< 标记此占位符是否代表一个浮动窗口中的位置。
    int indexOfFloatingWindow {}; ///< 如果 isFloatingWindow 为 true，则表示浮动窗口在其列表中的索引。
    int itemIndex {}; ///< 占位符在其父布局项中的索引。
    QString mainWindowUniqueName; ///< 如果占位符位于主窗口内，则表示主窗口的唯一名称。
};

/**
 * @brief 包含主窗口如何缩放的信息。
 * 用于 RestoreOption_RelativeToMainWindow 选项。
 */
struct LayoutSaver::ScalingInfo
{
    /**
     * @brief 默认构造函数。
     */
    ScalingInfo() = default;
    /**
     * @brief 构造函数，根据主窗口ID、已保存的主窗口几何形状和屏幕索引初始化。
     * @param mainWindowId 主窗口的唯一标识符。
     * @param savedMainWindowGeo 保存时主窗口的几何形状。
     * @param screenIndex 主窗口所在的屏幕索引。
     */
    explicit ScalingInfo(const QString &mainWindowId, QRect savedMainWindowGeo, int screenIndex);

    /**
     * @brief 检查缩放信息是否有效。
     * @return 如果高度因子和宽度因子都大于0，并且至少有一个因子不接近于1，则返回 true。
     */
    [[nodiscard]] bool isValid() const
    {
        // 高度和宽度缩放因子必须大于0
        // 并且至少有一个缩放因子不是（近似）1，表示确实发生了缩放
        return heightFactor > 0 && widthFactor > 0 && !((qFuzzyCompare(widthFactor, 1.0) && qFuzzyCompare(heightFactor, 1.0)));
    }

    /**
     * @brief 根据缩放信息平移点坐标（当前未实现具体逻辑）。
     * @param pos 要平移的点（引用传递）。
     */
    void translatePos(QPoint &pos) const;
    /**
     * @brief 将缩放因子应用于点坐标。
     * @param pos 要应用缩放的点（引用传递）。
     */
    void applyFactorsTo(QPoint &pos) const;
    /**
     * @brief 将缩放因子应用于尺寸。
     * @param size 要应用缩放的尺寸（引用传递）。
     */
    void applyFactorsTo(QSize &size) const;
    /**
     * @brief 将缩放因子和平移应用于矩形。
     * @param rect 要应用缩放和平移的矩形（引用传递）。
     */
    void applyFactorsTo(QRect &rect) const;

    QString mainWindowName; ///< 主窗口的名称。
    QRect savedMainWindowGeometry; ///< 保存布局时主窗口的几何形状。
    QRect realMainWindowGeometry; ///< 恢复布局时主窗口的实际几何形状。
    double heightFactor = -1.0; ///< 高度缩放因子。
    double widthFactor = -1.0; ///< 宽度缩放因子。
    bool mainWindowChangedScreen = false; ///< 标记主窗口是否在不同的屏幕上恢复。
};

/**
 * @brief 代表停靠小部件的最后位置信息。
 */
struct LayoutSaver::Position
{
    QRect lastFloatingGeometry; ///< 最后一次作为浮动窗口时的几何形状。
    int tabIndex {}; ///< 在其父框架的标签页小部件中的索引。
    bool wasFloating {}; ///< 标记此小部件在保存时是否处于浮动状态。
    LayoutSaver::Placeholder::List placeholders; ///< 与此位置关联的占位符列表。
    QHash<SideBarLocation, QRect> lastOverlayedGeometries; ///< 在各个侧边栏最后一次作为覆盖层时的几何形状。

    /**
     * @brief 遍历布局并修补所有绝对尺寸。参见 RestoreOption_RelativeToMainWindow。
     * @param scalingInfo 包含缩放信息的对象。
     */
    void scaleSizes(const ScalingInfo &scalingInfo);

    /**
     * @brief 将 Position 对象序列化为 QVariantMap。
     * @return 包含 Position 数据的 QVariantMap。
     */
    [[nodiscard]] QVariantMap toVariantMap() const;
    /**
     * @brief 从 QVariantMap 反序列化数据到 Position 对象。
     * @param map 包含 Position 数据的 QVariantMap。
     */
    void fromVariantMap(const QVariantMap &map);
};

/**
 * @brief 代表一个可序列化的停靠小部件 (DockWidget) 的状态。
 */
struct DOCKS_EXPORT LayoutSaver::DockWidget
{
    /// @brief DockWidget 共享指针的类型定义，因为需要修改共享实例。
    typedef std::shared_ptr<LayoutSaver::DockWidget> Ptr;
    /// @brief DockWidget 共享指针的 QVector 类型定义。
    typedef QVector<Ptr> List;
    /// @brief 用于存储按名称索引的 DockWidget 共享指针的静态哈希表。
    static QHash<QString, Ptr> s_dockWidgets;

    /**
     * @brief 检查此 DockWidget 的保存信息是否有效。
     * @return 如果 uniqueName 不为空，则返回 true。
     */
    [[nodiscard]] bool isValid() const;

    /**
     * @brief 遍历布局并修补所有绝对尺寸。参见 RestoreOption_RelativeToMainWindow。
     * @param scalingInfo 包含缩放信息的对象。
     */
    void scaleSizes(const ScalingInfo &scalingInfo);

    /**
     * @brief 根据名称获取或创建 DockWidget 的保存状态对象。
     * @param name 停靠小部件的唯一名称。
     * @return 指向 LayoutSaver::DockWidget 的共享指针。
     */
    static Ptr dockWidgetForName(const QString &name)
    {
        Ptr dw = s_dockWidgets.value(name); // 尝试从缓存中获取
        if (dw)
            return dw; // 如果找到，直接返回

        // 如果未找到，则创建一个新的并存入缓存
        dw = Ptr(new LayoutSaver::DockWidget);
        s_dockWidgets.insert(name, dw);
        dw->uniqueName = name; // 设置唯一名称

        return dw;
    }

    /**
     * @brief 检查此停靠小部件是否应跳过恢复过程。
     * @return 如果设置了 SkipRestoreLayout 选项，则返回 true。
     */
    [[nodiscard]] bool skipsRestore() const;

    /**
     * @brief 将 DockWidget 对象序列化为 QVariantMap。
     * @return 包含 DockWidget 数据的 QVariantMap。
     */
    [[nodiscard]] QVariantMap toVariantMap() const;
    /**
     * @brief 从 QVariantMap 反序列化数据到 DockWidget 对象。
     * @param map 包含 DockWidget 数据的 QVariantMap。
     */
    void fromVariantMap(const QVariantMap &map);

    QString uniqueName; ///< 停靠小部件的唯一名称。
    QStringList affinities; ///< 停靠小部件的亲和性列表。
    LayoutSaver::Position lastPosition; ///< 停靠小部件的最后位置信息。

private:
    /**
     * @brief 私有默认构造函数，防止外部直接创建实例（应通过 dockWidgetForName 获取）。
     */
    DockWidget() = default;
};


/**
 * @brief 将 LayoutSaver::DockWidget::List 转换为 QVariantList。
 * @param list 要转换的 DockWidget 共享指针列表。
 * @return 转换后的 QVariantList，其中每个元素都是一个 QVariantMap。
 */
inline QVariantList toVariantList(const LayoutSaver::DockWidget::List &list)
{
    QVariantList result;
    result.reserve(list.size());
    for (const auto &dw : list)
        result.push_back(dw->toVariantMap());
    return result;
}

/**
 * @brief 从 LayoutSaver::DockWidget::List 中提取所有停靠小部件的名称。
 * @param list DockWidget 共享指针列表。
 * @return 包含所有唯一名称的 QVariantList。
 */
inline QVariantList dockWidgetNames(const LayoutSaver::DockWidget::List &list)
{
    QVariantList result;
    result.reserve(list.size());
    for (auto &dw : list)
        result.push_back(dw->uniqueName);
    return result;
}

/**
 * @brief 代表一个可序列化的框架 (Frame) 的状态。
 */
struct LayoutSaver::Frame
{
    /**
     * @brief 检查此 Frame 的保存信息是否有效。
     * @return 如果 isNull 为 false，则返回 true。
     */
    [[nodiscard]] bool isValid() const;

    /**
     * @brief 检查此框架是否只包含一个停靠小部件。
     * @return 如果 dockWidgets 列表大小为 1，则返回 true。
     */
    [[nodiscard]] bool hasSingleDockWidget() const;
    /**
     * @brief 检查此框架及其所有子停靠小部件是否应跳过恢复过程。
     * @return 如果所有子 DockWidget 都跳过恢复，则返回 true。
     */
    [[nodiscard]] bool skipsRestore() const;

    /**
     * @brief 如果此框架只有一个停靠小部件，则返回该停靠小部件的保存状态。
     * @return 指向 LayoutSaver::DockWidget 的共享指针，如果不满足条件则返回 nullptr。
     */
    [[nodiscard]] LayoutSaver::DockWidget::Ptr singleDockWidget() const;

    /**
     * @brief 将 Frame 对象序列化为 QVariantMap。
     * @return 包含 Frame 数据的 QVariantMap。
     */
    [[nodiscard]] QVariantMap toVariantMap() const;
    /**
     * @brief 从 QVariantMap 反序列化数据到 Frame 对象。
     * @param map 包含 Frame 数据的 QVariantMap。
     */
    void fromVariantMap(const QVariantMap &map);

    bool isNull = true; ///< 标记此框架是否为空（例如，在某些布局中可能存在空占位）。
    QString objectName; ///< 框架的 Qt 对象名称。
    QRect geometry; ///< 框架的几何形状。
    QFlags<FrameOption>::Int options {}; ///< 框架的选项 (FrameOption 的整数表示)。
    int currentTabIndex {}; ///< 当前激活的标签页索引。
    QString id; ///< 用于关联目的的内部 ID。

    /// @brief 可能为空（如果不在主窗口中）。用于在恢复时避免为持久的中央框架创建新框架，
    /// 因为持久的中央框架在恢复时永远不会被删除。
    QString mainWindowUniqueName;

    LayoutSaver::DockWidget::List dockWidgets; ///< 此框架中包含的停靠小部件列表。
};

/**
 * @brief 代表一个可序列化的多重分割器 (MultiSplitter) 的状态。
 */
struct LayoutSaver::MultiSplitter
{
    /**
     * @brief 检查此 MultiSplitter 的保存信息是否有效。
     * @return 如果 layout QVariantMap 不为空，则返回 true。
     */
    [[nodiscard]] bool isValid() const;

    /**
     * @brief 检查此 MultiSplitter 是否只包含一个停靠小部件。
     * @return 如果只有一个框架且该框架只有一个停靠小部件，则返回 true。
     */
    [[nodiscard]] bool hasSingleDockWidget() const;
    /**
     * @brief 如果此 MultiSplitter 只有一个停靠小部件，则返回该停靠小部件的保存状态。
     * @return 指向 LayoutSaver::DockWidget 的共享指针，如果不满足条件则返回 nullptr。
     */
    [[nodiscard]] LayoutSaver::DockWidget::Ptr singleDockWidget() const;
    /**
     * @brief 检查此 MultiSplitter 及其所有内容是否应跳过恢复过程。
     * @return 如果所有框架都跳过恢复，则返回 true。
     */
    [[nodiscard]] bool skipsRestore() const;

    /**
     * @brief 将 MultiSplitter 对象序列化为 QVariantMap。
     * @return 包含 MultiSplitter 数据的 QVariantMap。
     */
    [[nodiscard]] QVariantMap toVariantMap() const;
    /**
     * @brief 从 QVariantMap 反序列化数据到 MultiSplitter 对象。
     * @param map 包含 MultiSplitter 数据的 QVariantMap。
     */
    void fromVariantMap(const QVariantMap &map);

    QVariantMap layout; ///< MultiSplitter 自身的布局信息 (通常是其内部项的结构)。
    QHash<QString, LayoutSaver::Frame> frames; ///< 按 ID 索引的此 MultiSplitter 包含的 Frame 列表。
};

/**
 * @brief 代表一个可序列化的浮动窗口 (FloatingWindow) 的状态。
 */
struct LayoutSaver::FloatingWindow
{
    /// @brief FloatingWindow 对象的 QVector 类型定义。
    typedef QVector<LayoutSaver::FloatingWindow> List;

    /**
     * @brief 检查此 FloatingWindow 的保存信息是否有效。
     * @return 如果其内部的 multiSplitterLayout 有效，则返回 true。
     */
    [[nodiscard]] bool isValid() const;

    /**
     * @brief 检查此浮动窗口是否只包含一个停靠小部件。
     * @return 如果其内部的 multiSplitterLayout 只包含一个停靠小部件，则返回 true。
     */
    [[nodiscard]] bool hasSingleDockWidget() const;
    /**
     * @brief 如果此浮动窗口只有一个停靠小部件，则返回该停靠小部件的保存状态。
     * @return 指向 LayoutSaver::DockWidget 的共享指针，如果不满足条件则返回 nullptr。
     */
    [[nodiscard]] LayoutSaver::DockWidget::Ptr singleDockWidget() const;
    /**
     * @brief 检查此浮动窗口及其所有内容是否应跳过恢复过程。
     * @return 如果其内部的 multiSplitterLayout 跳过恢复，则返回 true。
     */
    [[nodiscard]] bool skipsRestore() const;

    /**
     * @brief 遍历布局并修补所有绝对尺寸。参见 RestoreOption_RelativeToMainWindow。
     * @param scalingInfo 包含缩放信息的对象。
     */
    void scaleSizes(const ScalingInfo &scalingInfo);

    /**
     * @brief 将 FloatingWindow 对象序列化为 QVariantMap。
     * @return 包含 FloatingWindow 数据的 QVariantMap。
     */
    [[nodiscard]] QVariantMap toVariantMap() const;
    /**
     * @brief 从 QVariantMap 反序列化数据到 FloatingWindow 对象。
     * @param map 包含 FloatingWindow 数据的 QVariantMap。
     */
    void fromVariantMap(const QVariantMap &map);

    LayoutSaver::MultiSplitter multiSplitterLayout; ///< 此浮动窗口内部的 MultiSplitter 布局信息。
    QStringList affinities; ///< 浮动窗口的亲和性列表。
    int parentIndex = -1; ///< 父主窗口在其列表中的索引（如果适用）。
    QRect geometry; ///< 浮动窗口的几何形状。
    QRect normalGeometry; ///< 浮动窗口在最大化/最小化之前的正常几何形状。
    int screenIndex {}; ///< 浮动窗口所在的屏幕索引。
    int flags = -1; ///< 浮动窗口的标志 (FloatingWindowFlags 的整数表示)。
    QSize screenSize; ///< 用于相对尺寸恢复的屏幕尺寸。
    bool isVisible = true; ///< 标记浮动窗口是否可见。

    /// @brief 在恢复期间创建的 KDDockWidgets::FloatingWindow 实例。
    KDDockWidgets::FloatingWindow *floatingWindowInstance = nullptr;
    Qt::WindowState windowState = Qt::WindowNoState; ///< 窗口状态 (例如最小化、最大化)。
};

/**
 * @brief 代表一个可序列化的主窗口 (MainWindow) 的状态。
 */
struct LayoutSaver::MainWindow
{
public:
    /// @brief MainWindow 对象的 QVector 类型定义。
    typedef QVector<LayoutSaver::MainWindow> List;

    /**
     * @brief 检查此 MainWindow 的保存信息是否有效。
     * @return 如果 uniqueName 不为空，则返回 true。
     */
    [[nodiscard]] bool isValid() const;

    /**
     * @brief 遍历布局并修补所有绝对尺寸。参见 RestoreOption_RelativeToMainWindow。
     */
    void scaleSizes();

    /**
     * @brief 将 MainWindow 对象序列化为 QVariantMap。
     * @return 包含 MainWindow 数据的 QVariantMap。
     */
    [[nodiscard]] QVariantMap toVariantMap() const;
    /**
     * @brief 从 QVariantMap 反序列化数据到 MainWindow 对象。
     * @param map 包含 MainWindow 数据的 QVariantMap。
     */
    void fromVariantMap(const QVariantMap &map);

    QHash<SideBarLocation, QStringList> dockWidgetsPerSideBar; ///< 每个侧边栏位置对应的停靠小部件名称列表。
    KDDockWidgets::MainWindowOptions options; ///< 主窗口的选项。
    LayoutSaver::MultiSplitter multiSplitterLayout; ///< 主窗口内部的 MultiSplitter 布局信息。
    QString uniqueName; ///< 主窗口的唯一名称。
    QStringList affinities; ///< 主窗口的亲和性列表。
    QRect geometry; ///< 主窗口的几何形状。
    QRect normalGeometry; ///< 主窗口在最大化/最小化之前的正常几何形状。
    int screenIndex {}; ///< 主窗口所在的屏幕索引。
    QSize screenSize; ///< 用于相对尺寸恢复的屏幕尺寸。
    bool isVisible {}; ///< 标记主窗口是否可见。
    Qt::WindowState windowState = Qt::WindowNoState; ///< 窗口状态。

    ScalingInfo scalingInfo; ///< 主窗口的缩放信息。
};

/**
 * @brief 序列化一些关于屏幕的信息，以便将来在切换屏幕时可以更智能地恢复布局。
 * 当前未使用，但预先包含在 json 中会很有用。
 */
struct LayoutSaver::ScreenInfo
{
    /// @brief ScreenInfo 对象的 QVector 类型定义。
    typedef QVector<LayoutSaver::ScreenInfo> List;

    /**
     * @brief 将 ScreenInfo 对象序列化为 QVariantMap。
     * @return 包含 ScreenInfo 数据的 QVariantMap。
     */
    [[nodiscard]] QVariantMap toVariantMap() const;
    /**
     * @brief 从 QVariantMap 反序列化数据到 ScreenInfo 对象。
     * @param map 包含 ScreenInfo 数据的 QVariantMap。
     */
    void fromVariantMap(const QVariantMap &map);

    int index {}; ///< 屏幕的索引。
    QRect geometry; ///< 屏幕的几何形状。
    QString name; ///< 屏幕的名称。
    double devicePixelRatio {}; ///< 屏幕的设备像素比。
};

/**
 * @brief 代表整个应用程序布局的顶层结构。
 */
struct LayoutSaver::Layout
{
public:
    /**
     * @brief 构造函数。
     * 初始化时会记录当前所有屏幕的信息。
     */
    Layout()
    {
        s_currentLayoutBeingRestored = this; // 标记当前正在恢复的布局为此实例

        const QList<QScreen *> screens = QGuiApplication::screens(); // 获取所有屏幕
        const int numScreens = screens.size();
        screenInfo.reserve(numScreens); // 预分配屏幕信息列表的空间
        for (int i = 0; i < numScreens; ++i) {
            ScreenInfo info;
            info.index = i;
            info.geometry = screens[i]->geometry();
            info.name = screens[i]->name();
            info.devicePixelRatio = screens[i]->devicePixelRatio();
            screenInfo.push_back(info); // 添加屏幕信息到列表
        }
    }

    /**
     * @brief 析构函数。
     */
    ~Layout()
    {
        s_currentLayoutBeingRestored = nullptr; // 清除当前正在恢复的布局标记
    }

    /**
     * @brief 检查此布局是否有效。
     * @return 如果序列化版本号大于0，则返回 true。
     */
    [[nodiscard]] bool isValid() const;

    /**
     * @brief 将布局数据序列化为 JSON 字节数组。
     * @return 包含布局数据的 QByteArray (JSON格式)。
     */
    [[nodiscard]] QByteArray toJson() const;
    /**
     * @brief 从 JSON 字节数组反序列化布局数据。
     * @param jsonData 包含布局数据的 QByteArray (JSON格式)。
     * @return 如果反序列化成功，则返回 true；否则返回 false。
     */
    bool fromJson(const QByteArray &jsonData);
    /**
     * @brief 将布局对象序列化为 QVariantMap。
     * @return 包含布局数据的 QVariantMap。
     */
    [[nodiscard]] QVariantMap toVariantMap() const;
    /**
     * @brief 从 QVariantMap 反序列化数据到布局对象。
     * @param map 包含布局数据的 QVariantMap。
     */
    void fromVariantMap(const QVariantMap &map);

    /**
     * @brief 遍历布局并修补所有绝对尺寸。参见 RestoreOption_RelativeToMainWindow。
     * @param options 内部恢复选项。
     */
    void scaleSizes(KDDockWidgets::InternalRestoreOptions options);

    /// @brief 指向当前正在被恢复的 Layout 对象的静态指针。
    static LayoutSaver::Layout *s_currentLayoutBeingRestored;

    /**
     * @brief 根据索引获取主窗口的保存状态。
     * @param index 主窗口的索引。
     * @return 对应的 LayoutSaver::MainWindow 对象。
     */
    [[nodiscard]] LayoutSaver::MainWindow mainWindowForIndex(int index) const;
    /**
     * @brief 根据索引获取浮动窗口的保存状态。
     * @param index 浮动窗口的索引。
     * @return 对应的 LayoutSaver::FloatingWindow 对象。
     */
    [[nodiscard]] LayoutSaver::FloatingWindow floatingWindowForIndex(int index) const;

    /**
     * @brief 获取布局中所有主窗口的名称列表。
     * @return 主窗口名称的 QStringList。
     */
    [[nodiscard]] QStringList mainWindowNames() const;
    /**
     * @brief 获取布局中所有停靠小部件的名称列表。
     * @return 停靠小部件名称的 QStringList。
     */
    [[nodiscard]] QStringList dockWidgetNames() const;
    /**
     * @brief 获取在布局保存时已关闭的停靠小部件的名称列表。
     * @return 已关闭停靠小部件名称的 QStringList。
     */
    [[nodiscard]] QStringList dockWidgetsToClose() const;
    /**
     * @brief 检查布局中是否包含指定名称的停靠小部件。
     * @param uniqueName 停靠小部件的唯一名称。
     * @return 如果包含，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool containsDockWidget(const QString &uniqueName) const;

    int serializationVersion = KDDOCKWIDGETS_SERIALIZATION_VERSION; ///< 布局序列化版本号。
    LayoutSaver::MainWindow::List mainWindows; ///< 主窗口状态列表。
    LayoutSaver::FloatingWindow::List floatingWindows; ///< 浮动窗口状态列表。
    LayoutSaver::DockWidget::List closedDockWidgets; ///< 已关闭的停靠小部件状态列表。
    LayoutSaver::DockWidget::List allDockWidgets; ///< 所有已知的停靠小部件状态列表。
    ScreenInfo::List screenInfo; ///< 屏幕信息列表。

private:
    Q_DISABLE_COPY(Layout) ///< 禁止拷贝构造函数和拷贝赋值操作符。
};

/**
 * @brief LayoutSaver 的私有实现类 (PIMPL 模式)。
 */
class LayoutSaver::Private
{
public:
    /**
     * @brief RAII (Resource Acquisition Is Initialization) 辅助结构体，用于标记恢复过程的开始和结束。
     */
    struct RAIIIsRestoring
    {
        RAIIIsRestoring(); ///< 构造时标记恢复正在进行。
        ~RAIIIsRestoring(); ///< 析构时标记恢复已结束。
        Q_DISABLE_COPY(RAIIIsRestoring) ///< 禁止拷贝。
    };

    /**
     * @brief 构造函数。
     * @param options 恢复选项。
     */
    explicit Private(RestoreOptions options);

    /**
     * @brief 检查给定的亲和性列表是否与当前恢复选项中指定的亲和性匹配。
     * @param affinities 要检查的亲和性列表。
     * @return 如果匹配或没有指定亲和性过滤器，则返回 true。
     */
    [[nodiscard]] bool matchesAffinity(const QStringList &affinities) const;
    /**
     * @brief 将那些在主窗口列表中但标记为跳过恢复的停靠小部件浮动起来。
     * @param mainWindowNames 主窗口名称列表。
     */
    static void floatWidgetsWhichSkipRestore(const QStringList &mainWindowNames);
    /**
     * @brief 将布局中存在但当前应用程序中未知的停靠小部件浮动起来。
     * @param layout 要检查的布局。
     */
    static void floatUnknownWidgets(const LayoutSaver::Layout &layout);

    /**
     * @brief 模板函数，用于反序列化窗口（主窗口或浮动窗口）的几何形状。
     * @tparam T 窗口保存状态的类型 (LayoutSaver::MainWindow 或 LayoutSaver::FloatingWindow)。
     * @param saved 保存的窗口状态。
     * @param topLevel 指向实际的顶层 QWidgetOrQuick 对象。
     */
    template<typename T>
    void deserializeWindowGeometry(const T &saved, QWidgetOrQuick *topLevel);
    /**
     * @brief 删除布局中所有空的框架。
     */
    void deleteEmptyFrames() const;
    /**
     * @brief 清除所有停靠小部件上用于标记是否已恢复的属性。
     */
    static void clearRestoredProperty();

    /**
     * @brief 获取一个指向 QSettings 对象的智能指针，用于可能的配置存储。
     * @return std::unique_ptr<QSettings>。
     */
    [[nodiscard]] static std::unique_ptr<QSettings> settings();

    DockRegistry *const m_dockRegistry; ///< 指向停靠小部件注册表的指针。
    InternalRestoreOptions m_restoreOptions = {}; ///< 内部恢复选项。
    QStringList m_affinityNames; ///< 当前恢复操作的亲和性名称过滤器。

    /// @brief 静态布尔值，标记恢复过程是否正在进行。
    static bool s_restoreInProgress;
};
} // namespace KDDockWidgets

#endif // KD_LAYOUTSAVER_P_H