#pragma once // 确保该头文件在一次编译中仅被包含一次

#include "kddockwidgets/docks_export.h"      // 包含导出宏定义，用于库的导出和导入
#include "kddockwidgets/KDDockWidgets.h" // 包含 KDDockWidgets 库的主头文件

#include <QObject>   // 包含 QObject 类，Qt 对象模型的基础
#include <QVector>   // 包含 QVector 类，动态数组
#include <QRect>     // 包含 QRect 类，用于表示矩形区域
#include <QVariant>  // 包含 QVariant 类，可以存储多种不同类型的数据
#include <QDebug>    // 包含 QDebug 类，用于调试输出

#include <memory> // 包含智能指针相关的头文件

class TestMultiSplitter; // 前向声明 TestMultiSplitter 类，用于测试

namespace Layouting { // 布局相关的命名空间
Q_NAMESPACE         // 声明此命名空间也用于 Qt 元对象系统

// 前向声明
class ItemContainer;
class ItemBoxContainer;
class Item;
class Separator;
class Widget;
struct LengthOnSide;

/**
 * @brief 枚举，表示两个可能的方向或侧边。
 */
enum Side {
    Side1, ///< 第一个侧边或方向
    Side2  ///< 第二个侧边或方向
};
Q_ENUM_NS(Side) // 将 Side 枚举注册到元对象系统

/**
 * @brief 枚举，定义当需要额外空间时，项目的增长策略。
 */
enum class GrowthStrategy {
    BothSidesEqually, ///< 在两侧平均增长
    Side1Only,        ///< 仅在第一个侧边增长
    Side2Only         ///< 仅在第二个侧边增长
};

/**
 * @brief 枚举，定义分隔符的选项。
 */
enum class SeparatorOption {
    None = 0,       ///< 无特殊选项
    LazyResize = 1  ///< 延迟调整大小，可能用于性能优化，仅在必要时调整
};
Q_DECLARE_FLAGS(SeparatorOptions, SeparatorOption) // 为 SeparatorOption 声明一个标志类型 SeparatorOptions

/**
 * @brief 枚举，定义容器在调整大小时其子项的调整策略。
 */
enum class ChildrenResizeStrategy {
    Percentage,         ///< 子项保持其在容器中所占的百分比进行调整
    Side1SeparatorMove, ///< 调整大小时，优先移动第一个侧边的分隔符（即调整第一个侧边的子项大小）
    Side2SeparatorMove  ///< 调整大小时，优先移动第二个侧边的分隔符（即调整第二个侧边的子项大小）
};
Q_ENUM_NS(ChildrenResizeStrategy) // 将 ChildrenResizeStrategy 枚举注册到元对象系统

/**
 * @brief 枚举，定义当一个项目需要收缩空间时，其邻居项目的收缩策略。
 */
enum class NeighbourSqueezeStrategy {
    AllNeighbours,          ///< 收缩压力平均分配给所有邻居，不仅仅是直接邻居
    ImmediateNeighboursFirst ///< 直接邻居首先承担尽可能多的收缩压力，然后才轮到更远的邻居
};
Q_ENUM_NS(NeighbourSqueezeStrategy) // 将 NeighbourSqueezeStrategy 枚举注册到元对象系统

/**
 * @brief 枚举，定义布局中项目可能接触到的边界位置。
 */
enum LayoutBorderLocation {
    LayoutBorderLocation_None = 0,                                                                                ///< 不接触任何边界
    LayoutBorderLocation_North = 1,                                                                               ///< 接触北部（上）边界
    LayoutBorderLocation_East = 2,                                                                                ///< 接触东部（右）边界
    LayoutBorderLocation_West = 4,                                                                                ///< 接触西部（左）边界
    LayoutBorderLocation_South = 8,                                                                               ///< 接触南部（下）边界
    LayoutBorderLocation_All = LayoutBorderLocation_North | LayoutBorderLocation_East | LayoutBorderLocation_West | LayoutBorderLocation_South, ///< 接触所有边界
    LayoutBorderLocation_Verticals = LayoutBorderLocation_West | LayoutBorderLocation_East,                       ///< 接触垂直边界（左和右）
    LayoutBorderLocation_Horizontals = LayoutBorderLocation_North | LayoutBorderLocation_South,                   ///< 接触水平边界（上和下）
};
Q_DECLARE_FLAGS(LayoutBorderLocations, LayoutBorderLocation) // 为 LayoutBorderLocation 声明一个标志类型 LayoutBorderLocations

/**
 * @brief 根据给定的方向获取点的坐标分量。
 * @param p 点。
 * @param o 方向 (Qt::Vertical 或 Qt::Horizontal)。
 * @return 如果是垂直方向，返回 y 坐标；如果是水平方向，返回 x 坐标。
 */
inline int pos(QPoint p, Qt::Orientation o)
{
    return o == Qt::Vertical ? p.y()
                             : p.x();
}

/**
 * @brief 根据给定的方向获取尺寸的长度分量。
 * @param sz 尺寸。
 * @param o 方向 (Qt::Vertical 或 Qt::Horizontal)。
 * @return 如果是垂直方向，返回高度；如果是水平方向，返回宽度。
 */
inline int length(QSize sz, Qt::Orientation o)
{
    return o == Qt::Vertical ? sz.height()
                             : sz.width();
}

/**
 * @brief 将 QSize 对象转换为包含宽度和高度的 QVariantMap。
 * @param sz 要转换的 QSize 对象。
 * @return 包含 "width" 和 "height" 键的 QVariantMap。
 */
inline QVariantMap sizeToMap(QSize sz)
{
    QVariantMap map;
    map.insert(QStringLiteral("width"), sz.width());
    map.insert(QStringLiteral("height"), sz.height());

    return map;
}

/**
 * @brief 将 QRect 对象转换为包含 x, y, 宽度和高度的 QVariantMap。
 * @param rect 要转换的 QRect 对象。
 * @return 包含 "x", "y", "width", "height" 键的 QVariantMap。
 */
inline QVariantMap rectToMap(QRect rect)
{
    QVariantMap map;
    map.insert(QStringLiteral("x"), rect.x());
    map.insert(QStringLiteral("y"), rect.y());
    map.insert(QStringLiteral("width"), rect.width());
    map.insert(QStringLiteral("height"), rect.height());

    return map;
}

/**
 * @brief 将包含宽度和高度的 QVariantMap 转换为 QSize 对象。
 * @param map 包含 "width" 和 "height" 键的 QVariantMap。
 * @return 从 map 中解析出的 QSize 对象。
 */
inline QSize mapToSize(const QVariantMap &map)
{
    return { map.value(QStringLiteral("width")).toInt(),
             map.value(QStringLiteral("height")).toInt() };
}

/**
 * @brief 将包含 x, y, 宽度和高度的 QVariantMap 转换为 QRect 对象。
 * @param map 包含 "x", "y", "width", "height" 键的 QVariantMap。
 * @return 从 map 中解析出的 QRect 对象。
 */
inline QRect mapToRect(const QVariantMap &map)
{
    return QRect(map.value(QStringLiteral("x")).toInt(),
                 map.value(QStringLiteral("y")).toInt(),
                 map.value(QStringLiteral("width")).toInt(),
                 map.value(QStringLiteral("height")).toInt());
}

/**
 * @brief 结构体，存储布局项的尺寸和几何信息。
 *
 * 此结构体用于在布局计算过程中传递和管理项的大小、最小/最大尺寸、几何位置等信息。
 */
struct SizingInfo
{
    /**
     * @brief 默认构造函数。
     */
    SizingInfo();

    /**
     * @brief 获取当前项的尺寸。
     * @return QSize 表示项的当前尺寸。
     */
    [[nodiscard]] QSize size() const
    {
        return geometry.size();
    }

    /**
     * @brief 设置项的尺寸。
     * @param sz 新的尺寸。
     */
    void setSize(QSize sz)
    {
        geometry.setSize(sz);
    }

    /**
     * @brief 根据指定方向获取项的长度。
     * @param o 方向 (Qt::Vertical 或 Qt::Horizontal)。
     * @return 项在指定方向上的长度。
     */
    [[nodiscard]] int length(Qt::Orientation o) const
    {
        return Layouting::length(size(), o);
    }

    /**
     * @brief 根据指定方向获取项的最小长度。
     * @param o 方向 (Qt::Vertical 或 Qt::Horizontal)。
     * @return 项在指定方向上的最小长度。
     */
    [[nodiscard]] int minLength(Qt::Orientation o) const
    {
        return Layouting::length(minSize, o);
    }

    /**
     * @brief 根据指定方向获取项的最大长度提示。
     *
     * 确保返回值不小于最小长度。
     * @param o 方向 (Qt::Vertical 或 Qt::Horizontal)。
     * @return 项在指定方向上的最大长度提示。
     */
    [[nodiscard]] int maxLengthHint(Qt::Orientation o) const
    {
        return qMax(minLength(o), Layouting::length(maxSizeHint, o));
    }

    /**
     * @brief 根据指定方向获取项可用的（可收缩的）长度。
     *
     * 即当前长度超出最小长度的部分。
     * @param o 方向 (Qt::Vertical 或 Qt::Horizontal)。
     * @return 可用长度，如果小于0则返回0。
     */
    [[nodiscard]] int availableLength(Qt::Orientation o) const
    {
        return qMax(0, length(o) - minLength(o));
    }

    /**
     * @brief 根据指定方向获取项缺失的（需要增长的）长度。
     *
     * 即当前长度小于最小长度的部分。
     * @param o 方向 (Qt::Vertical 或 Qt::Horizontal)。
     * @return 缺失长度，如果小于0则返回0。
     */
    [[nodiscard]] int missingLength(Qt::Orientation o) const
    {
        return qMax(0, minLength(o) - length(o));
    }

    /**
     * @brief 获取项的左上角位置。
     * @return QPoint 表示项的左上角坐标。
     */
    [[nodiscard]] QPoint pos() const
    {
        return geometry.topLeft();
    }

    /**
     * @brief 根据指定方向获取项的位置坐标。
     * @param o 方向 (Qt::Vertical 或 Qt::Horizontal)。
     * @return 项在指定方向上的位置坐标。
     */
    [[nodiscard]] int position(Qt::Orientation o) const
    {
        return Layouting::pos(pos(), o);
    }

    /**
     * @brief 根据指定方向获取项的边缘位置。
     * @param o 方向 (Qt::Vertical 或 Qt::Horizontal)。
     * @return 如果是垂直方向，返回底部边缘坐标；如果是水平方向，返回右侧边缘坐标。
     */
    [[nodiscard]] int edge(Qt::Orientation o) const
    {
        return o == Qt::Vertical ? geometry.bottom()
                                 : geometry.right();
    }

    /**
     * @brief 设置项在指定方向上的长度。
     * @param l 新的长度。
     * @param o 方向 (Qt::Vertical 或 Qt::Horizontal)。
     */
    void setLength(int l, Qt::Orientation o)
    {
        if (o == Qt::Vertical) {
            geometry.setHeight(l);
        } else {
            geometry.setWidth(l);
        }
    }

    /**
     * @brief 增加项在指定方向上的长度。
     * @param byAmount 增加的量。
     * @param o 方向 (Qt::Vertical 或 Qt::Horizontal)。
     */
    void incrementLength(int byAmount, Qt::Orientation o)
    {
        setLength(length(o) + byAmount, o);
    }

    /**
     * @brief 设置项在与指定方向相反的方向上的长度。
     * @param l 新的长度。
     * @param o 参考方向 (Qt::Vertical 或 Qt::Horizontal)。
     */
    void setOppositeLength(int l, Qt::Orientation o);

    /**
     * @brief 设置项在指定方向上的位置。
     * @param p 新的位置坐标。
     * @param o 方向 (Qt::Vertical 或 Qt::Horizontal)。
     */
    void setPos(int p, Qt::Orientation o)
    {
        if (o == Qt::Vertical)
            geometry.moveTop(p);
        else
            geometry.moveLeft(p);
    }

    /**
     * @brief 检查几何信息是否为空（无效）。
     * @return 如果 geometry.isNull() 为 true，则返回 true。
     */
    [[nodiscard]] bool isNull() const
    {
        return geometry.isNull();
    }

    /**
     * @brief 设置项的几何区域。
     * @param geo 新的几何区域。
     */
    void setGeometry(QRect geo)
    {
        geometry = geo;
    }

    /**
     * @brief 根据指定方向获取项可增长的空间。
     * @param o 方向 (Qt::Vertical 或 Qt::Horizontal)。
     * @return 可增长的长度。
     */
    [[nodiscard]] int availableToGrow(Qt::Orientation o) const
    {
        return maxLengthHint(o) - length(o);
    }

    /**
     * @brief 根据指定方向获取项需要收缩的空间（如果当前长度超过最大长度提示）。
     * @param o 方向 (Qt::Vertical 或 Qt::Horizontal)。
     * @return 需要收缩的长度，如果小于0则返回0。
     */
    [[nodiscard]] int neededToShrink(Qt::Orientation o) const
    {
        return qMax(0, length(o) - maxLengthHint(o));
    }

    /**
     * @brief 检查项在指定方向上是否已超过其最大长度提示。
     * @param o 方向 (Qt::Vertical 或 Qt::Horizontal)。
     * @return 如果可增长空间大于等于0 (即未超过或等于最大长度)，则返回 true。
     */
    [[nodiscard]] bool isPastMax(Qt::Orientation o) const
    {
        return availableToGrow(o) >= 0;
    }

    /**
     * @brief 将 SizingInfo 对象转换为 QVariantMap。
     * @return 包含 SizingInfo 数据的 QVariantMap。
     */
    [[nodiscard]] QVariantMap toVariantMap() const;

    /**
     * @brief 从 QVariantMap 中填充 SizingInfo 对象。
     * @param map 包含 SizingInfo 数据的 QVariantMap。
     */
    void fromVariantMap(const QVariantMap &);

    typedef QVector<SizingInfo> List; ///< SizingInfo 对象的向量类型定义。
    QRect geometry;                     ///< 项的几何区域 (位置和尺寸)。
    QSize minSize;                      ///< 项的最小尺寸。
    QSize maxSizeHint;                  ///< 项的最大尺寸提示。
    double percentageWithinParent = 0.0; ///< 项在其父容器中所占的百分比。
    bool isBeingInserted = false;        ///< 标记项当前是否处于被插入过程。
};

/**
 * @brief Item 类是布局系统中所有可布局元素（如 Widget 或 ItemContainer）的基类。
 *
 * 它提供了通用的属性（如几何信息、可见性、父子关系）和操作（如大小调整、位置设置）。
 * Item 可以包含一个 "guest" Widget，这是实际显示的用户界面元素。
 */
class DOCKS_EXPORT_FOR_UNIT_TESTS Item : public QObject
{
    Q_OBJECT
    /** @qproperty int x
     * @brief 项的 x 坐标。
     * @see xChanged()
     */
    Q_PROPERTY(int x READ x NOTIFY xChanged)
    /** @qproperty int y
     * @brief 项的 y 坐标。
     * @see yChanged()
     */
    Q_PROPERTY(int y READ y NOTIFY yChanged)
    /** @qproperty int width
     * @brief 项的宽度。
     * @see widthChanged()
     */
    Q_PROPERTY(int width READ width NOTIFY widthChanged)
    /** @qproperty int height
     * @brief 项的高度。
     * @see heightChanged()
     */
    Q_PROPERTY(int height READ height NOTIFY heightChanged)
    /** @qproperty QRect geometry
     * @brief 项的几何区域 (位置和尺寸)。
     * @see geometryChanged()
     */
    Q_PROPERTY(QRect geometry READ geometry NOTIFY geometryChanged)
    /** @qproperty bool isContainer
     * @brief 只读属性，指示此项是否为一个容器。
     */
    Q_PROPERTY(bool isContainer READ isContainer CONSTANT)
public:
    typedef QVector<Item *> List; ///< Item 指针的向量类型定义。

    /**
     * @brief 构造一个 Item 对象。
     * @param hostWidget 托管此布局项的 Widget。
     * @param parent 父 ItemContainer，如果为 nullptr 则表示此项是顶级项或无父项。
     */
    explicit Item(Widget *hostWidget, ItemContainer *parent = nullptr);
    /**
     * @brief 析构函数。
     */
    ~Item() override;

    /**
     * @brief 返回此项是否为根容器。
     * @return 如果此项是容器且没有父容器，则返回 true。
     */
    [[nodiscard]] bool isRoot() const;

    /**
     * @brief 返回此项接触到的布局边界。
     * @return 一个 LayoutBorderLocations 标志，指示项接触的边界；如果不接触任何边界，则返回 LayoutBorderLocation_None。
     */
    [[nodiscard]] LayoutBorderLocations adjacentLayoutBorders() const;

    /**
     * @brief 递归地返回此项及其子项中可见项的总数。
     * @return 可见项的数量。
     */
    [[nodiscard]] virtual int visibleCount_recursive() const;

    static QSize hardcodedMinimumSize;    ///< 静态成员，所有 Widget 的硬编码最小尺寸，无论其自身的最小尺寸如何。
    static QSize hardcodedMaximumSize;    ///< 静态成员，所有 Widget 的硬编码最大尺寸。
    static int separatorThickness;      ///< 静态成员，分隔条的厚度。

    /** @brief 获取项的 x 坐标。*/
    [[nodiscard]] int x() const;
    /** @brief 获取项的 y 坐标。*/
    [[nodiscard]] int y() const;
    /** @brief 获取项的宽度。*/
    [[nodiscard]] int width() const;
    /** @brief 获取项的高度。*/
    [[nodiscard]] int height() const;
    /** @brief 获取项的尺寸。*/
    [[nodiscard]] QSize size() const;
    /** @brief 设置项的尺寸。 */
    void setSize(QSize);
    /** @brief 获取项的左上角位置。*/
    [[nodiscard]] QPoint pos() const;
    /** @brief 根据指定方向获取项的位置坐标。*/
    [[nodiscard]] int pos(Qt::Orientation) const;
    /** @brief 获取项的几何区域。*/
    [[nodiscard]] QRect geometry() const;
    /** @brief 获取项的矩形区域 (同 geometry())。*/
    [[nodiscard]] QRect rect() const;
    /** @brief 检查此项是否为容器。*/
    [[nodiscard]] bool isContainer() const;
    /** @brief 获取父容器。*/
    [[nodiscard]] ItemContainer *parentContainer() const;
    /** @brief 获取父 ItemBoxContainer (如果父容器是 ItemBoxContainer 类型)。*/
    [[nodiscard]] ItemBoxContainer *parentBoxContainer() const;
    /** @brief 设置项的最小尺寸。*/
    void setMinSize(QSize);
    /** @brief 设置项的最大尺寸提示。*/
    void setMaxSizeHint(QSize);
    /** @brief 检查此项是否为占位符 (通常表示一个空的或未填充的布局槽)。*/
    [[nodiscard]] bool isPlaceholder() const;
    /** @brief 设置项的几何区域。*/
    void setGeometry(QRect rect);
    /** @brief 获取根 ItemBoxContainer。*/
    [[nodiscard]] ItemBoxContainer *root() const;
    /** @brief 将局部矩形映射到根坐标系。*/
    [[nodiscard]] QRect mapToRoot(QRect) const;
    /** @brief 将局部点映射到根坐标系。*/
    [[nodiscard]] QPoint mapToRoot(QPoint) const;
    /** @brief 将局部坐标值按指定方向映射到根坐标系。*/
    [[nodiscard]] int mapToRoot(int p, Qt::Orientation) const;
    /** @brief 将根坐标系中的点映射回局部坐标系。*/
    [[nodiscard]] QPoint mapFromRoot(QPoint) const;
    /** @brief 将根坐标系中的矩形映射回局部坐标系。*/
    [[nodiscard]] QRect mapFromRoot(QRect) const;
    /** @brief 将父坐标系中的点映射到局部坐标系。 */
    [[nodiscard]] QPoint mapFromParent(QPoint) const;
    /** @brief 将根坐标系中的坐标值按指定方向映射回局部坐标系。*/
    [[nodiscard]] int mapFromRoot(int p, Qt::Orientation) const;

    /** @brief 获取作为 QObject 的 "guest" 部件。*/
    [[nodiscard]] QObject *guestAsQObject() const;
    /** @brief 获取 "guest" Widget 部件。*/
    [[nodiscard]] Widget *guestWidget() const
    {
        return m_guest;
    }
    /** @brief 设置 "guest" Widget 部件。*/
    void setGuestWidget(Widget *);

    /** @brief 增加引用计数。*/
    void ref();
    /** @brief 减少引用计数。*/
    void unref();
    /** @brief 获取当前引用计数。*/
    [[nodiscard]] int refCount() const;
    /** @brief 将此项转换为空的占位符。*/
    void turnIntoPlaceholder();

    /** @brief 根据指定方向获取项的最小长度。*/
    [[nodiscard]] int minLength(Qt::Orientation) const;
    /** @brief 根据指定方向获取项的最大长度提示。*/
    [[nodiscard]] int maxLengthHint(Qt::Orientation) const;

    /** @brief 获取托管此布局项的 QObject。*/
    [[nodiscard]] QObject *host() const;
    /** @brief 获取托管此布局项的 Widget。*/
    [[nodiscard]] Widget *hostWidget() const;
    /**
     * @brief 恢复此项，并关联指定的 "guest" Widget。
     * @param guestWidget 要关联的新 "guest" Widget。
     */
    void restore(Widget *guestWidget);

    /**
     * @brief 获取从根项到此项的路径，表示为一系列索引。
     * @return 一个包含索引的 QVector<int>。
     */
    [[nodiscard]] QVector<int> pathFromRoot() const;

    /**
     * @brief 检查布局项的内部状态是否一致和有效。
     *
     * 这是一个纯虚函数或应在子类中重写的函数，用于调试和验证。
     * @return 如果状态有效，则返回 true，否则返回 false。
     */
    Q_REQUIRED_RESULT virtual bool checkSanity();

    /**
     * @brief 检查此项是否用于 MDI (多文档界面) 模式。
     * @return 如果是 MDI 项，则返回 true。
     */
    [[nodiscard]] bool isMDI() const;

    /** @brief 获取项的最小尺寸 (虚函数，子类可以重写)。*/
    [[nodiscard]] virtual QSize minSize() const;
    /** @brief 获取项的最大尺寸提示 (虚函数，子类可以重写)。*/
    [[nodiscard]] virtual QSize maxSizeHint() const;
    /**
     * @brief 递归地设置此项及其子项的尺寸。
     * @param newSize 新的尺寸。
     * @param strategy 子项尺寸调整策略，默认为按百分比调整。
     */
    virtual void setSize_recursive(QSize newSize, ChildrenResizeStrategy strategy = ChildrenResizeStrategy::Percentage);
    /**
     * @brief 检查此项是否可见。
     * @param excludeBeingInserted 如果为 true，则在判断可见性时排除正处于插入过程中的项。
     * @return 如果项可见，则返回 true。
     */
    [[nodiscard]] virtual bool isVisible(bool excludeBeingInserted = false) const;
    /**
     * @brief 递归地设置此项及其子项的几何区域。
     * @param rect 新的几何区域。
     */
    virtual void setGeometry_recursive(QRect rect);
    /**
     * @brief 输出布局信息到调试输出，用于调试。
     * @param level 当前递归层级，用于格式化输出。
     */
    virtual void dumpLayout(int level = 0);
    /**
     * @brief 设置托管此布局项的 Widget (虚函数)。
     * @param hostWidget 新的宿主 Widget。
     */
    virtual void setHostWidget(Widget *);
    /**
     * @brief 将项的状态序列化为 QVariantMap。
     * @return 包含项状态的 QVariantMap。
     */
    [[nodiscard]] virtual QVariantMap toVariantMap() const;
    /**
     * @brief 从 QVariantMap 中反序列化并填充项的状态。
     * @param map 包含项状态的 QVariantMap。
     * @param widgets 一个从名称到 Widget 指针的哈希表，用于恢复 "guest" Widget。
     */
    virtual void fillFromVariantMap(const QVariantMap &map, const QHash<QString, Widget *> &widgets);

    /**
     * @brief 从 QVariantMap 创建并返回一个新的 Item 对象。
     * @param hostWidget 托管新项的 Widget。
     * @param parent 新项的父 ItemContainer。
     * @param map 包含项状态的 QVariantMap。
     * @param widgets 一个从名称到 Widget 指针的哈希表。
     * @return 创建的 Item 指针。
     */
    static Item *createFromVariantMap(Widget *hostWidget, ItemContainer *parent,
                                      const QVariantMap &map, const QHash<QString, Widget *> &widgets);

Q_SIGNALS:
    void geometryChanged(); ///< 当项的几何区域改变时发射。
    void xChanged();        ///< 当项的 x 坐标改变时发射。
    void yChanged();        ///< 当项的 y 坐标改变时发射。
    void widthChanged();    ///< 当项的宽度改变时发射。
    void heightChanged();   ///< 当项的高度改变时发射。
    /**
     * @brief 当项的可见性状态改变时发射。
     * @param thisItem 指向状态改变的 Item 的指针。
     * @param visible 新的可见性状态。
     */
    void visibleChanged(Layouting::Item *thisItem, bool visible);
    /**
     * @brief 当项的最小尺寸改变时发射。
     * @param thisItem 指向状态改变的 Item 的指针。
     */
    void minSizeChanged(Layouting::Item *thisItem);
    /**
     * @brief 当项的最大尺寸提示改变时发射。
     * @param thisItem 指向状态改变的 Item 的指针。
     */
    void maxSizeChanged(Layouting::Item *thisItem);

protected:
    friend class ::TestMultiSplitter; ///< 声明 TestMultiSplitter 为友元类，用于测试。
    /**
     * @brief 受保护的构造函数，供子类使用。
     * @param isContainer 指示此项是否为容器。
     * @param hostWidget 托管此布局项的 Widget。
     * @param parent 父 ItemContainer。
     */
    explicit Item(bool isContainer, Widget *hostWidget, ItemContainer *parent);
    /** @brief 设置父容器。*/
    void setParentContainer(ItemContainer *parent);
    /** @brief 连接到父容器的信号。*/
    void connectParent(ItemContainer *parent);
    /** @brief 设置项的位置。*/
    void setPos(QPoint);
    /** @brief 根据指定方向设置项的位置坐标。*/
    void setPos(int pos, Qt::Orientation);
    /** @brief 将此项作为 const ItemContainer* 类型转换 (如果它是容器)。*/
    [[nodiscard]] const ItemContainer *asContainer() const;
    /** @brief 将此项作为 ItemContainer* 类型转换 (如果它是容器)。*/
    ItemContainer *asContainer();
    /** @brief 将此项作为 ItemBoxContainer* 类型转换 (如果它是 ItemBoxContainer)。*/
    ItemBoxContainer *asBoxContainer();
    /** @brief 根据指定方向设置项的长度。*/
    void setLength(int length, Qt::Orientation);
    /** @brief 递归地根据指定方向设置项的长度。*/
    virtual void setLength_recursive(int length, Qt::Orientation);
    /** @brief 根据指定方向获取项的长度。*/
    [[nodiscard]] int length(Qt::Orientation) const;
    /** @brief 根据指定方向获取项的可用的（可收缩的）长度。*/
    [[nodiscard]] int availableLength(Qt::Orientation) const;
    /** @brief 获取项缺失的尺寸 (当前尺寸小于最小尺寸的部分)。*/
    [[nodiscard]] QSize missingSize() const;
    /** @brief 更新 "guest" Widget 的几何区域 (虚函数)。*/
    virtual void updateWidgetGeometries();
    /** @brief 设置项的可见性状态 (虚函数)。*/
    virtual void setIsVisible(bool);
    /** @brief 检查项是否正处于被插入的过程中。*/
    [[nodiscard]] bool isBeingInserted() const;
    /** @brief 设置项是否正处于被插入的过程中。*/
    void setBeingInserted(bool);

    SizingInfo m_sizingInfo;      ///< 存储项的尺寸和几何信息。
    const bool m_isContainer;     ///< 标记此项是否为容器。
    ItemContainer *m_parent = nullptr; ///< 指向父容器的指针。
    bool m_isSettingGuest = false; ///< 标记是否正在设置 guest 部件，用于避免重入。

private Q_SLOTS:
    /** @brief 当 "guest" Widget 请求重新布局时调用的槽函数。*/
    void onWidgetLayoutRequested();

private:
    friend class ItemContainer;     ///< ItemContainer 是友元类。
    friend class ItemBoxContainer;  ///< ItemBoxContainer 是友元类。
    friend class ItemFreeContainer; ///< ItemFreeContainer 是友元类。
    /** @brief 事件过滤器，用于监听 "guest" Widget 的事件。*/
    bool eventFilter(QObject *widget, QEvent *event) override;
    int m_refCount = 0;             ///< 引用计数。
    /** @brief 更新 QObject 的对象名称，通常用于调试。*/
    void updateObjectName();
    /** @brief 当 "guest" Widget 被销毁时调用的槽函数。*/
    void onWidgetDestroyed();
    bool m_isVisible = false;       ///< 项的当前可见性状态。
    Widget *m_hostWidget = nullptr; ///< 指向宿主 Widget 的指针。
    Widget *m_guest = nullptr;      ///< 指向 "guest" Widget 的指针。
};

/**
 * @brief ItemContainer 是可以包含其他 Item 对象的 Item 的基类。
 *
 * 它定义了管理子项（如添加、移除、查询）的通用接口。
 */
class DOCKS_EXPORT_FOR_UNIT_TESTS ItemContainer : public Item
{
    Q_OBJECT
public:
    /**
     * @brief 构造一个 ItemContainer 对象。
     * @param hostWidget 托管此容器的 Widget。
     * @param parent 父 ItemContainer。
     */
    explicit ItemContainer(Widget *hostWidget, ItemContainer *parent);
    /**
     * @brief 构造一个顶级的 ItemContainer 对象 (没有父项)。
     * @param hostWidget 托管此容器的 Widget。
     */
    explicit ItemContainer(Widget *hostWidget);
    /**
     * @brief 析构函数。
     */
    ~ItemContainer() override;

    /**
     * @brief 从容器中移除一个子项 (纯虚函数，子类必须实现)。
     * @param item 要移除的子项。
     * @param hardRemove 如果为 true，则彻底删除项；否则可能只是将其标记为隐藏或非活动。
     */
    virtual void removeItem(Item *, bool hardRemove = true) = 0;
    /**
     * @brief 恢复一个之前被移除或隐藏的子项 (纯虚函数)。
     * @param child 要恢复的子项。
     */
    virtual void restore(Item *child) = 0;
    /**
     * @brief 当子项的最小尺寸改变时调用的回调函数 (纯虚函数)。
     * @param child 最小尺寸发生改变的子项。
     */
    virtual void onChildMinSizeChanged(Item *child) = 0;
    /**
     * @brief 当子项的可见性状态改变时调用的回调函数 (纯虚函数)。
     * @param child 可见性状态发生改变的子项。
     * @param visible 新的可见性状态。
     */
    virtual void onChildVisibleChanged(Item *child, bool visible) = 0;

    /** @brief 获取可见子项的数量。*/
    [[nodiscard]] int numVisibleChildren() const;
    /** @brief 获取所有子项的数量。*/
    [[nodiscard]] int numChildren() const;
    /** @brief 检查容器是否拥有子项。*/
    [[nodiscard]] bool hasChildren() const;
    /**
     * @brief 检查容器是否拥有可见的子项。
     * @param excludeBeingInserted 如果为 true，则在判断时排除正处于插入过程中的项。
     * @return 如果有可见子项，则返回 true。
     */
    [[nodiscard]] bool hasVisibleChildren(bool excludeBeingInserted = false) const;
    /** @brief 获取所有子项的列表。*/
    [[nodiscard]] List childItems() const;
    /** @brief 检查容器是否为空 (没有子项)。*/
    [[nodiscard]] bool isEmpty() const;
    /**
     * @brief 检查容器是否直接包含指定的项。
     * @param item 要检查的项。
     * @return 如果容器直接包含该项，则返回 true。
     */
    bool contains(const Item *item) const;
    /**
     * @brief 根据 QObject 查找其对应的 Item 子项。
     * @param object 要查找的 QObject (通常是 "guest" Widget)。
     * @return 如果找到，返回对应的 Item 指针；否则返回 nullptr。
     */
    Item *itemForObject(const QObject *) const;
    /**
     * @brief 根据 Widget 查找其对应的 Item 子项。
     * @param w 要查找的 Widget。
     * @return 如果找到，返回对应的 Item 指针；否则返回 nullptr。
     */
    Item *itemForWidget(const Widget *w) const;
    /**
     * @brief 获取所有可见子项的列表。
     * @param includeBeingInserted 如果为 true，则在结果中包含正处于插入过程中的可见项。
     * @return 可见子项的列表。
     */
    [[nodiscard]] Item::List visibleChildren(bool includeBeingInserted = false) const;
    /** @brief 递归地获取此容器及其所有子容器中的所有 Item。*/
    [[nodiscard]] Item::List items_recursive() const;
    /**
     * @brief 递归地检查此容器或其任何子容器是否包含指定的项。
     * @param item 要检查的项。
     * @return 如果找到该项，则返回 true。
     */
    bool contains_recursive(const Item *item) const;
    /** @brief 递归地返回可见子项的总数。*/
    [[nodiscard]] int visibleCount_recursive() const override;
    /** @brief 递归地返回所有子项的总数。*/
    [[nodiscard]] int count_recursive() const;
    /** @brief 清空容器中的所有子项 (纯虚函数)。*/
    virtual void clear() = 0;

protected:
    /** @brief 检查容器是否只有一个可见的子项。*/
    [[nodiscard]] bool hasSingleVisibleItem() const;

    Item::List m_children; ///< 存储子项的列表。

Q_SIGNALS:
    void itemsChanged(); ///< 当子项列表发生改变（添加、移除等）时发射。
    /**
     * @brief 当可见子项的数量改变时发射。
     * @param num 可见子项的新数量。
     */
    void numVisibleItemsChanged(int);
    void numItemsChanged(); ///< 当子项总数改变时发射。

private:
    struct Private; // 前向声明私有实现结构体
    Private *const d; // 指向私有实现对象的指针 (PImpl 模式)
};

/**
 * @brief ItemBoxContainer 是一个用于水平或垂直排列子项的容器。
 *
 * 类似于 QBoxLayout，它可以按一个方向（水平或垂直）来组织其子项，
 * 并且通常会在子项之间放置 Separator (分隔条) 以允许用户调整它们的大小。
 */
class DOCKS_EXPORT_FOR_UNIT_TESTS ItemBoxContainer : public ItemContainer
{
    Q_OBJECT
public:
    /**
     * @brief 构造一个 ItemBoxContainer 对象。
     * @param hostWidget 托管此容器的 Widget。
     * @param parent 父 ItemContainer。
     */
    explicit ItemBoxContainer(Widget *hostWidget, ItemContainer *parent);
    /**
     * @brief 构造一个顶级的 ItemBoxContainer 对象。
     * @param hostWidget 托管此容器的 Widget。
     */
    explicit ItemBoxContainer(Widget *hostWidget);
    /**
     * @brief 析构函数。
     */
    ~ItemBoxContainer() override;

    /**
     * @brief在指定索引处插入一个子项。
     * @param item 要插入的项。
     * @param index 插入位置的索引。
     * @param option 插入时的初始选项，例如大小调整模式。
     */
    void insertItem(Item *item, int index, KDDockWidgets::InitialOption option = KDDockWidgets::InitialOption(KDDockWidgets::DefaultSizeMode::Fair));
    /**
     * @brief 根据相对位置插入一个子项。
     * @param item 要插入的项。
     * @param location 相对于容器的插入位置 (例如，KDDockWidgets::Top)。
     * @param option 插入时的初始选项。
     */
    void insertItem(Item *item, KDDockWidgets::Location, KDDockWidgets::InitialOption = {});

    /**
     * @brief 将一个项插入到另一个项的相对位置。
     *
     * 这个静态方法允许将 `item` 插入到 `relativeTo` 项的 `location` 指定的位置。
     * 如果 `relativeTo` 的父容器与 `item` 的目标方向不匹配，可能会创建新的子容器。
     *
     * @param item 要插入的项。
     * @param relativeTo 作为参照的项。
     * @param location 相对于 `relativeTo` 的插入位置。
     * @param option 插入时的初始选项。
     */
    static void insertItemRelativeTo(Item *item, Item *relativeTo, KDDockWidgets::Location,
                                     KDDockWidgets::InitialOption = KDDockWidgets::InitialOption(KDDockWidgets::DefaultSizeMode::Fair));

    /**
     * @brief 请求移动一个分隔条。
     * @param separator 要移动的分隔条。
     * @param delta 移动的距离（正值或负值）。
     */
    void requestSeparatorMove(Separator *separator, int delta);
    /**
     * @brief 获取分隔条可以移动到的最小位置 (局部坐标)。
     * @param separator 目标分隔条。
     * @param honourMax 是否考虑最大尺寸限制，默认为 true。
     * @return 分隔条的最小允许位置。
     */
    int minPosForSeparator(Separator *, bool honourMax = true) const;
    /**
     * @brief 获取分隔条可以移动到的最大位置 (局部坐标)。
     * @param separator 目标分隔条。
     * @param honourMax 是否考虑最大尺寸限制，默认为 true。
     * @return 分隔条的最大允许位置。
     */
    int maxPosForSeparator(Separator *, bool honourMax = true) const;
    /**
     * @brief 获取分隔条可以移动到的最小位置 (全局坐标)。
     * @param separator 目标分隔条。
     * @param honourMax 是否考虑最大尺寸限制，默认为 true。
     * @return 分隔条的最小允许全局位置。
     */
    int minPosForSeparator_global(Separator *, bool honourMax = true) const;
    /**
     * @brief 获取分隔条可以移动到的最大位置 (全局坐标)。
     * @param separator 目标分隔条。
     * @param honourMax 是否考虑最大尺寸限制，默认为 true。
     * @return 分隔条的最大允许全局位置。
     */
    int maxPosForSeparator_global(Separator *, bool honourMax = true) const;
    /**
     * @brief 请求将指定分隔条两侧的子项设置为相等的大小。
     * @param separator 目标分隔条。
     */
    void requestEqualSize(Separator *separator);
    /** @brief 将所有子项设置为相等的大小。*/
    void layoutEqually();
    /** @brief 递归地将此容器及其所有子 ItemBoxContainer 中的子项设置为相等的大小。*/
    void layoutEqually_recursive();
    /**
     * @brief 从容器中移除一个子项。
     * @param item 要移除的子项。
     * @param hardRemove 如果为 true，则彻底删除项。
     */
    void removeItem(Item *, bool hardRemove = true) override;
    /** @brief 获取容器的最小尺寸。*/
    [[nodiscard]] QSize minSize() const override;
    /** @brief 获取容器的最大尺寸提示。*/
    [[nodiscard]] QSize maxSizeHint() const override;
    /** @brief 获取容器的可用尺寸（当前尺寸减去固定元素如边框占用的空间）。*/
    [[nodiscard]] QSize availableSize() const;
    /** @brief 检查容器的内部状态是否一致和有效。*/
    Q_REQUIRED_RESULT bool checkSanity() override;
    /** @brief 输出布局信息到调试输出。*/
    void dumpLayout(int level = 0) override;
    /**
     * @brief 递归地设置此容器及其子项的尺寸。
     * @param newSize 新的尺寸。
     * @param strategy 子项尺寸调整策略。
     */
    void setSize_recursive(QSize newSize, ChildrenResizeStrategy strategy = ChildrenResizeStrategy::Percentage) override;
    /**
     * @brief 获取建议的拖放矩形区域。
     *
     * 当将 `item` 拖放到 `relativeTo` 项的 `location` 位置时，此函数返回一个建议的矩形区域
     * 以指示停靠预览。
     *
     * @param item 正在拖动的项。
     * @param relativeTo 作为停靠目标的项。
     * @param location 相对于 `relativeTo` 的停靠位置。
     * @return 建议的拖放预览矩形。
     */
    QRect suggestedDropRect(const Item *item, const Item *relativeTo, KDDockWidgets::Location) const;
    /** @brief 将容器的状态序列化为 QVariantMap。*/
    [[nodiscard]] QVariantMap toVariantMap() const override;
    /**
     * @brief 从 QVariantMap 中反序列化并填充容器的状态。
     * @param map 包含容器状态的 QVariantMap。
     * @param widgets 一个从名称到 Widget 指针的哈希表。
     */
    void fillFromVariantMap(const QVariantMap &map, const QHash<QString, Widget *> &widgets) override;
    /** @brief 清空容器中的所有子项。*/
    void clear() override;
    /** @brief 获取容器的布局方向 (水平或垂直)。*/
    [[nodiscard]] Qt::Orientation orientation() const;
    /** @brief 检查容器是否为垂直布局。*/
    [[nodiscard]] bool isVertical() const;
    /** @brief 检查容器是否为水平布局。*/
    [[nodiscard]] bool isHorizontal() const;
    /** @brief 获取容器在其布局方向上的长度。*/
    [[nodiscard]] int length() const;

    /**
     * @brief 递归地返回在指定方向上并排布局的可见项的数量。
     *
     * 这个函数会考虑嵌套的容器。例如，一个垂直容器内包含两个水平子容器，
     * 每个子容器包含两个项，那么对于水平方向，此函数将返回4 (2+2)。
     *
     * @param orientation 要统计的方向。
     * @return 在指定方向上并排的可见项数量。
     */
    [[nodiscard]] int numSideBySide_recursive(Qt::Orientation) const;

private:
    /** @brief 检查容器是否已设置布局方向。*/
    [[nodiscard]] bool hasOrientation() const;
    /**
     * @brief 获取指定可见子项在其父容器的可见子项列表中的索引。
     * @param child 要查找索引的子项。
     * @return 子项的索引，如果未找到则返回-1。
     */
    int indexOfVisibleChild(const Item *) const;
    /** @brief 恢复一个子项。 */
    void restore(Item *) override;
    /**
     * @brief 恢复一个子项，并指定邻居项的挤压策略。
     * @param child 要恢复的子项。
     * @param neighbourSqueezeStrategy 邻居项的挤压策略。
     */
    void restoreChild(Item *, NeighbourSqueezeStrategy neighbourSqueezeStrategy = NeighbourSqueezeStrategy::AllNeighbours);

    /** @brief 递归地设置几何区域。 */
    void setGeometry_recursive(QRect rect) override;

    /**
     * @brief 将一个叶子节点子项转换为一个新的 ItemBoxContainer。
     *
     * 这通常在需要向叶子节点添加另一个项，从而需要创建一个新的嵌套容器时发生。
     *
     * @param leaf 要转换的叶子节点项。
     * @return 指向新创建的 ItemBoxContainer 的指针，该容器现在包含原叶子节点。
     */
    ItemBoxContainer *convertChildToContainer(Item *leaf);
    /**
     * @brief 检查容器是否具有与给定停靠位置兼容的布局方向。
     * @param location 停靠位置。
     * @return 如果方向兼容则返回 true。
     */
    [[nodiscard]] bool hasOrientationFor(KDDockWidgets::Location) const;
    /** @brief 获取容器在其布局方向上的可用长度 (总长度减去分隔条占用的空间)。*/
    [[nodiscard]] int usableLength() const;
    /**
     * @brief 设置容器的子项列表和布局方向。
     * @param children 新的子项列表。
     * @param o 新的布局方向。
     */
    void setChildren(const Item::List &children, Qt::Orientation o);
    /** @brief 设置容器的布局方向。*/
    void setOrientation(Qt::Orientation);
    /** @brief 更新所有直接子项在其父容器中所占的百分比。*/
    void updateChildPercentages();
    /** @brief 递归地更新此容器及其所有子 ItemBoxContainer 中子项的百分比。*/
    void updateChildPercentages_recursive();
    /** @brief 更新所有子 "guest" Widget 的几何区域。*/
    void updateWidgetGeometries() override;
    /** @brief 获取容器在其布局方向的相对方向上的长度。*/
    [[nodiscard]] int oppositeLength() const;

    /**
     * @brief 均等布局一组尺寸信息。
     * @param sizes 包含各项尺寸信息的列表，此列表将被修改以反映均等布局。
     */
    void layoutEqually(SizingInfo::List &sizes);

    /**
     * @brief 增长指定项的邻居以填充它们之间的空白区域。
     *
     * 当一个项被移除时，此函数很有用。它的邻居将扩展以占据它留下的空间。
     * `side1Neighbour` 或 `side2Neighbour` 允许为 null，此时非 null 的那个邻居将占据全部空间。
     *
     * @param side1Neighbour 第一个侧边（通常是左边或上边）的邻居项。
     * @param side2Neighbour 第二个侧边（通常是右边或下边）的邻居项。
     */
    void growNeighbours(Item *side1Neighbour, Item *side2Neighbour);

    /**
     * @brief 按指定量增长一个项。
     *
     * 此函数计算需要在第一个侧边和第二个侧边分别增长多少，
     * 然后调用另一个 `growItem` 重载版本来实际执行增长，并收缩提供空间的邻居项。
     *
     * @param item 要增长的项。
     * @param amount 总共需要增长的量。
     * @param strategy 增长策略 (BothSidesEqually, Side1Only, Side2Only)。
     * @param neighbourSqueezeStrategy 邻居项的收缩策略。
     * @param accountForNewSeparator 是否考虑由于增长可能引入的新分隔条的空间，默认为 false。
     * @param resizeStrategy 子项尺寸调整策略，默认为 Percentage。
     */
    void growItem(Item *, int amount, GrowthStrategy,
                  NeighbourSqueezeStrategy neighbourSqueezeStrategy,
                  bool accountForNewSeparator = false,
                  ChildrenResizeStrategy = ChildrenResizeStrategy::Percentage);
    /**
     * @brief (内部辅助函数) 按指定量增长给定索引处的项。
     * @param index 要增长的项在 `sizes` 列表中的索引。
     * @param sizes 包含各项尺寸信息的列表，此列表将被修改。
     * @param missing 需要增长的总量。
     * @param strategy 增长策略。
     * @param neighbourSqueezeStrategy 邻居项的收缩策略。
     * @param accountForNewSeparator 是否考虑新分隔条的空间。
     */
    void growItem(int index, SizingInfo::List &sizes, int missing, GrowthStrategy,
                  NeighbourSqueezeStrategy neighbourSqueezeStrategy,
                  bool accountForNewSeparator = false);

    /**
     * @brief 收缩指定索引处项的邻居。
     *
     * 第一个侧边（左侧/顶部）的邻居将收缩 `side1Amount`，
     * 第二个侧边（右侧/底部）的邻居将收缩 `side2Amount`。
     * 此函数会影响所有相关邻居，而不仅仅是直接邻居。
     *
     * @param index 目标项在 `sizes` 列表中的索引。
     * @param sizes 包含各项尺寸信息的列表，此列表将被修改。
     * @param side1Amount 第一个侧边需要收缩的量。
     * @param side2Amount 第二个侧边需要收缩的量。
     * @param neighbourSqueezeStrategy 邻居项的收缩策略，默认为 AllNeighbours。
     */
    void shrinkNeighbours(int index, SizingInfo::List &sizes, int side1Amount, int side2Amount,
                          NeighbourSqueezeStrategy = NeighbourSqueezeStrategy::AllNeighbours);

    /**
     * @brief 获取指定项在某个方向上的可见邻居。
     * @param item 参考项。
     * @param side 要查找邻居的方向 (Side1 或 Side2)。
     * @return 可见邻居项的指针，如果不存在则返回 nullptr。
     */
    Item *visibleNeighbourFor(const Item *item, Side side) const;
    /** @brief 获取容器在其布局方向上的可用长度 (已考虑边距和分隔条)。*/
    [[nodiscard]] int availableLength() const;
    /**
     * @brief (静态辅助函数) 计算从指定索引开始，在某个方向上所有项的总长度。
     * @param sizes 包含各项尺寸信息的列表。
     * @param fromIndex 开始计算的索引。
     * @param side 计算的方向 (Side1 或 Side2)。
     * @param o 布局方向。
     * @return 计算得到的总长度结构体。
     */
    [[nodiscard]] static LengthOnSide lengthOnSide(const SizingInfo::List &sizes, int fromIndex, Side, Qt::Orientation);
    /**
     * @brief 获取指定项在某个方向上所有邻居的总长度。
     * @param item 参考项。
     * @param side 要计算邻居的方向。
     * @param o 布局方向。
     * @return 邻居的总长度。
     */
    int neighboursLengthFor(const Item *item, Side, Qt::Orientation) const;
    /**
     * @brief 递归地获取指定项在某个方向上所有邻居的总长度 (考虑嵌套容器)。
     * @param item 参考项。
     * @param side 要计算邻居的方向。
     * @param o 布局方向。
     * @return 邻居的总长度。
     */
    int neighboursLengthFor_recursive(const Item *item, Side, Qt::Orientation) const;
    /**
     * @brief 获取指定项在某个方向上所有邻居的最小总长度。
     * @param item 参考项。
     * @param side 要计算邻居的方向。
     * @param o 布局方向。
     * @return 邻居的最小总长度。
     */
    int neighboursMinLengthFor(const Item *item, Side, Qt::Orientation) const;
    /**
     * @brief 获取指定项在某个方向上所有邻居的最大总长度提示。
     * @param item 参考项。
     * @param side 要计算邻居的方向。
     * @param o 布局方向。
     * @return 邻居的最大总长度提示。
     */
    int neighboursMaxLengthFor(const Item *item, Side, Qt::Orientation) const;
    /**
     * @brief 获取指定子项在某个方向上可供收缩的总空间。
     * @param child 参考子项。
     * @param side 要检查的方向。
     * @return 可收缩的总空间。
     */
    int availableToSqueezeOnSide(const Item *child, Side) const;
    /**
     * @brief 获取指定子项在某个方向上可供增长的总空间。
     * @param child 参考子项。
     * @param side 要检查的方向。
     * @return 可增长的总空间。
     */
    int availableToGrowOnSide(const Item *child, Side) const;
    /**
     * @brief 递归地获取指定子项在某个方向上可供收缩的总空间 (考虑嵌套容器)。
     * @param child 参考子项。
     * @param side 要检查的方向。
     * @param o 布局方向。
     * @return 可收缩的总空间。
     */
    int availableToSqueezeOnSide_recursive(const Item *child, Side, Qt::Orientation) const;
    /**
     * @brief 递归地获取指定子项在某个方向上可供增长的总空间 (考虑嵌套容器)。
     * @param child 参考子项。
     * @param side 要检查的方向。
     * @param o 布局方向。
     * @return 可增长的总空间。
     */
    int availableToGrowOnSide_recursive(const Item *child, Side, Qt::Orientation) const;
    /** @brief 当子项最小尺寸改变时的回调。*/
    void onChildMinSizeChanged(Item *child) override;
    /** @brief 当子项可见性改变时的回调。*/
    void onChildVisibleChanged(Item *child, bool visible) override;
    /** @brief 更新容器自身的大小约束 (最小/最大尺寸)，通常在其子项约束改变后调用。*/
    void updateSizeConstraints();
    /**
     * @brief 获取所有可见子项的 SizingInfo 列表。
     * @param ignoreBeingInserted 如果为 true，则忽略正处于插入过程中的项。
     * @return 包含 SizingInfo 的列表。
     */
    [[nodiscard]] SizingInfo::List sizes(bool ignoreBeingInserted = false) const;
    /**
     * @brief 计算在一系列项中分配指定收缩量时，每个项应收缩多少。
     * @param begin 指向 SizingInfo 列表起始的迭代器。
     * @param end 指向 SizingInfo 列表结束的迭代器。
     * @param needed 需要收缩的总量。
     * @param strategy 收缩策略。
     * @param reversed 是否反向处理列表，默认为 false。
     * @return 一个包含每个项应收缩量的 QVector<int>。
     */
    [[nodiscard]] QVector<int> calculateSqueezes(const SizingInfo::List::ConstIterator &begin,
                                                 const SizingInfo::List::ConstIterator &end, int needed,
                                                 NeighbourSqueezeStrategy, bool reversed = false) const;
    /**
     * @brief 当 `suggestedDropRect` 无法在直接父容器中找到合适位置时的回退逻辑。
     *
     * 这通常涉及到查找更上层的父容器或根容器来确定一个合适的拖放预览位置。
     *
     * @param item 正在拖动的项。
     * @param relativeTo 作为停靠目标的项。
     * @param location 相对于 `relativeTo` 的停靠位置。
     * @return 建议的拖放预览矩形。
     */
    QRect suggestedDropRectFallback(const Item *item, const Item *relativeTo, KDDockWidgets::Location) const;
    /** @brief 根据当前尺寸和子项约束，重新定位所有子项。*/
    void positionItems();
    /** @brief 递归地定位所有子项。*/
    void positionItems_recursive();
    /**
     * @brief 根据提供的尺寸信息列表定位所有子项。
     * @param sizes 包含每个子项目标尺寸和位置信息的列表。
     */
    void positionItems(SizingInfo::List &sizes);
    /**
     * @brief 返回指定点下方的直接子项。
     * @param p 要检查的点 (局部坐标)。
     * @return 如果找到，返回对应的 Item 指针；否则返回 nullptr。
     */
    [[nodiscard]] Item *itemAt(QPoint p) const;
    /**
     * @brief 递归地返回指定点下方最深层的 Item。
     * @param p 要检查的点 (局部坐标)。
     * @return 如果找到，返回对应的 Item 指针；否则返回 nullptr。
     */
    [[nodiscard]] Item *itemAt_recursive(QPoint p) const;
    /** @brief 设置宿主 Widget。*/
    void setHostWidget(Widget *) override;
    /** @brief 设置可见性。*/
    void setIsVisible(bool) override;
    /** @brief 获取可见性。*/
    [[nodiscard]] bool isVisible(bool excludeBeingInserted = false) const override;
    /** @brief 递归地设置长度。*/
    void setLength_recursive(int length, Qt::Orientation) override;
    /**
     * @brief 应用计算好的几何信息到子项。
     * @param sizes 包含每个子项目标几何信息的列表。
     * @param strategy 子项尺寸调整策略。
     */
    void applyGeometries(const SizingInfo::List &sizes, ChildrenResizeStrategy = ChildrenResizeStrategy::Percentage);
    /**
     * @brief 应用计算好的位置信息到子项。
     * @param sizes 包含每个子项目标位置信息的列表。
     */
    void applyPositions(const SizingInfo::List &sizes);

    /**
     * @brief 获取指定分隔条在子项列表中的索引。
     * @param separator 要查找的分隔条。
     * @return 分隔条的索引，如果未找到则返回 -1。
     */
    int indexOf(Separator *) const;
    /**
     * @brief 检查当前是否正在执行 simplify() 操作，用于避免重入。
     * @return 如果正在简化布局，则返回 true。
     */
    [[nodiscard]] bool isInSimplify() const;

#ifdef DOCKS_DEVELOPER_MODE // 仅在开发者模式下编译
    /** @brief 测试 suggestedRect 函数的内部一致性。 */
    bool test_suggestedRect();
#endif

public:
    /** @brief 递归地获取此容器及其所有子 ItemBoxContainer 中的所有 Separator。*/
    [[nodiscard]] QVector<Layouting::Separator *> separators_recursive() const;
    /** @brief 获取此容器直接包含的所有 Separator。*/
    [[nodiscard]] QVector<Layouting::Separator *> separators() const;

private:
    /**
     * @brief 简化布局。
     *
     * 例如，如果一个容器在移除子项后只剩下一个子项，
     * 那么这个容器可能会被其唯一的子项替换掉，以减少布局层级。
     */
    void simplify();
    static bool s_inhibitSimplify; ///< 静态标志，用于临时禁止 simplify() 操作。
    friend class Layouting::Item; ///< Item 是友元类。
    friend class ::TestMultiSplitter; ///< TestMultiSplitter 是友元类。
    struct Private; // 前向声明私有实现结构体
    Private *const d; // 指向私有实现对象的指针 (PImpl 模式)
};

} // namespace Layouting