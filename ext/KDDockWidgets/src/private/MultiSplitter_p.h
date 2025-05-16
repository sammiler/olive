#ifndef KDDOCKWIDGETS_MULTISPLITTER_P_H
#define KDDOCKWIDGETS_MULTISPLITTER_P_H

#include "LayoutWidget_p.h"             // 包含父类 LayoutWidget 的私有头文件
#include "kddockwidgets/KDDockWidgets.h" // KDDockWidgets 公共头文件，包含枚举和基本类型
#include "kddockwidgets/QWidgetAdapter.h"  // QWidget 和 QQuickItem 的适配器类
#include "kddockwidgets/docks_export.h"    // 导入导出宏定义

// 前向声明，用于测试目的
class TestDocks;

namespace KDDockWidgets {

// 前向声明
struct WindowBeingDragged; // 代表正在被拖拽的窗口的信息结构体

/**
 * @brief MultiSplitter 只是 Layouting::Item 的一个包装器，其中托管的小部件是 KDDockWidgets::Frame 类的实例。
 * Layouting:: 中的内容是与平台无关且通用的，并非 KDDW 特有。
 *
 * MultiSplitter 类似于 QSplitter，但支持以任意组合方式混合垂直和水平分割器。
 * 它支持将小部件添加到整个 MultiSplitter 的左/上/下/右侧，或者相对于单个小部件添加。
 * 它继承自 LayoutWidget，提供了更具体的基于分割的布局逻辑。
 */
class DOCKS_EXPORT MultiSplitter : public LayoutWidget
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数。
     * @param parent 父 QWidgetOrQuick 对象，默认为 nullptr。
     */
    explicit MultiSplitter(QWidgetOrQuick *parent = nullptr);
    /**
     * @brief 析构函数。
     */
    ~MultiSplitter() override;

    /**
     * @brief 将一个小部件添加到此 MultiSplitter 中。
     * @param widget 要添加的 QWidgetOrQuick 对象 (通常是一个 Frame)。
     * @param location 停靠的位置 (例如，相对于整个 MultiSplitter 或某个现有框架)。
     * @param relativeTo 可选参数，如果停靠是相对的，则指定参照的 Frame 对象。默认为 nullptr。
     * @param option 初始化选项，例如默认的尺寸模式，默认为 DefaultSizeMode::Fair。
     */
    void addWidget(QWidgetOrQuick *widget, KDDockWidgets::Location location,
                   Frame *relativeTo = nullptr,
                   InitialOption option = InitialOption(DefaultSizeMode::Fair));

    /**
     * @brief 将整个 MultiSplitter 添加到此布局中。源 MultiSplitter 在其所有 Frame 被“窃取”后将被删除。
     * 所有添加的 Frame 将保留其原始布局，因此，如果 widgetFoo 在源分割器中位于 widgetBar 的左侧，
     * 那么当整个分割器被拖放到此分割器中时，它仍将位于 widgetBar 的左侧。
     * @param splitter 要添加的源 MultiSplitter 对象。
     * @param location 停靠的位置。
     * @param relativeTo 可选参数，参照的 Frame 对象。默认为 nullptr。
     * @param option 初始化选项，默认为 DefaultSizeMode::Fair。
     */
    void addMultiSplitter(MultiSplitter *splitter, KDDockWidgets::Location location,
                          Frame *relativeTo = nullptr,
                          InitialOption option = InitialOption(DefaultSizeMode::Fair));

    /**
     * @brief 由指示器调用，以便它们在正确的位置绘制拖放橡皮筋。
     * 返回在指定位置放置小部件时橡皮筋的矩形区域。
     * 不包括分隔符的厚度，结果实际上比需要的要小。换句话说，
     * 结果将与小部件将获得的几何形状完全相同。
     * @param wbd 指向正在被拖拽的窗口信息的指针。
     * @param location 目标放置位置。
     * @param relativeTo 相对于哪个布局项进行放置。
     * @return 计算得到的放置矩形区域。
     */
    QRect rectForDrop(const WindowBeingDragged *wbd, KDDockWidgets::Location location,
                      const Layouting::Item *relativeTo) const;

    /**
     * @brief 从保存的布局数据中反序列化 MultiSplitter 的状态。
     * @param data 包含 MultiSplitter 布局信息的 LayoutSaver::MultiSplitter 对象。
     * @return 如果反序列化成功则返回 true，否则返回 false。
     */
    bool deserialize(const LayoutSaver::MultiSplitter &data) override;

    /**
     * @brief 返回此 MultiSplitter 中的分隔符 (Separator) 列表。
     * @return Layouting::Separator 指针的 QVector。
     */
    [[nodiscard]] QVector<Layouting::Separator *> separators() const;

    /**
     * @brief 平均分配此 MultiSplitter 中所有顶级项的空间。
     * 参见 MainWindowBase::layoutEqually() 的文档。
     */
    void layoutEqually();

    /**
     * @brief (重载) 仅在子树内调整小部件大小，使其平均分配空间。
     * @param container 指向要进行平均布局的 Layouting::ItemBoxContainer 的指针。
     */
    static void layoutEqually(Layouting::ItemBoxContainer *container);

    /**
     * @brief 返回水平或垂直排列的项的数量，并考虑嵌套情况。
     * @param orientation 要统计的方向 (Qt::Horizontal 或 Qt::Vertical)。
     * @return 递归计算得到的并排项数量。
     */
    [[nodiscard]] int numSideBySide_recursive(Qt::Orientation orientation) const;

private:
    // 声明友元类，允许测试类访问私有成员
    friend class ::TestDocks;

    /**
     * @brief 获取此 MultiSplitter 的根布局项容器。
     * @return 指向根 Layouting::ItemBoxContainer 的指针。
     */
    [[nodiscard]] Layouting::ItemBoxContainer *rootItem() const;

    /**
     * @brief (用于调试/健壮性) 验证输入参数的有效性。
     * @param widget 要添加的小部件。
     * @param location 停靠位置。
     * @param relativeToFrame 相对停靠的参照框架。
     * @param option 初始化选项。
     * @return 如果输入有效则返回 true，否则返回 false。
     */
    bool validateInputs(QWidgetOrQuick *widget, KDDockWidgets::Location location,
                        const Frame *relativeToFrame, InitialOption option) const;


    /**
     * @brief 设置此 MultiSplitter 的根布局项容器。
     * 此方法在父类 LayoutWidget 中可能也有，这里可能是特定于 MultiSplitter 的重写或调用。
     * @param root 指向作为根的 Layouting::ItemBoxContainer 对象。
     */
    void setRootItem(Layouting::ItemBoxContainer *root);

    /**
     * @brief 类似于 availableLengthForDrop，但仅返回总可用宽度或高度（取决于 @p orientation）。
     * 因此不需要接收任何位置。
     * @param orientation 如果是 Qt::Vertical，则返回可用高度；否则返回可用宽度。
     * @return 指定方向上的可用长度。
     */
    [[nodiscard]] int availableLengthForOrientation(Qt::Orientation orientation) const;

    /**
     * @brief 等同于 availableLengthForOrientation，但返回两个方向的可用尺寸。
     * 宽度对应 Qt::Vertical (即垂直方向上是宽度，水平方向上是高度，这里似乎有些反直觉，需要结合具体实现理解)。
     * 通常 QSize 的 width() 表示水平长度，height() 表示垂直长度。
     * @return 可用的 QSize。
     */
    [[nodiscard]] QSize availableSize() const;

    /// @brief 指向此 MultiSplitter 的根布局项容器。与父类 LayoutWidget 中的 m_rootItem 不同，
    /// 这里特指类型为 ItemBoxContainer，用于传统的分割器布局。
    Layouting::ItemBoxContainer *m_rootItem = nullptr;
};

} // namespace KDDockWidgets

#endif // KDDOCKWIDGETS_MULTISPLITTER_P_H