#ifndef KD_MULTISPLITTER_SEPARATOR_P_H
#define KD_MULTISPLITTER_SEPARATOR_P_H

#include "kddockwidgets/docks_export.h" // 包含导出宏定义

#include <QObject> // 包含 QObject 基类
#include <QPoint>  // 包含 QPoint 类，用于表示二维点

// 前向声明 Qt::Orientation，避免不必要的 Qt 核心头文件包含
QT_BEGIN_NAMESPACE
enum class Orientation;
QT_END_NAMESPACE

namespace Layouting { // 布局相关的命名空间

class Config;             // 前向声明 Config 类
class ItemBoxContainer;   // 前向声明 ItemBoxContainer 类
class Separator;          // 前向声明 Separator 类自身 (用于 typedef List)
class Widget;             // 前向声明 Widget 类 (可能是 KDDockWidgets::Widget 的别名或特定于布局的 Widget)

/**
 * @brief Separator 类代表布局中的分隔条。
 *
 * 分隔条是用户可以拖动以调整相邻布局项大小的视觉元素。
 * 此类定义了分隔条的基本接口和行为，具体的视觉表现和事件处理
 * 可能由其子类（例如 Separator_qwidget 或 Separator_quick）实现。
 *
 * Separator 对象通常由 ItemBoxContainer 管理。
 */
class DOCKS_EXPORT Separator
{
public:
    /**
     * @brief Separator 指针的 QVector 类型定义。
     *
     * 用于存储一组分隔条对象。
     */
    typedef QVector<Separator *> List;

    /**
     * @brief 虚析构函数。
     *
     * 确保通过基类指针删除派生类对象时能够正确调用派生类的析构函数。
     */
    virtual ~Separator();

    /**
     * @brief 检查分隔条是否为垂直方向。
     * @return 如果分隔条是垂直的，则返回 true；否则返回 false (表示水平)。
     */
    [[nodiscard]] bool isVertical() const;

    /**
     * @brief 移动分隔条到新的位置。
     *
     * 位置 `p` 通常是相对于其父容器的坐标，并且取决于分隔条的方向
     * (例如，对于垂直分隔条是 x 坐标，对于水平分隔条是 y 坐标)。
     * @param p 新的位置。
     */
    void move(int p);

    /**
     * @brief 获取分隔条的方向。
     * @return Qt::Orientation 枚举值 (Qt::Horizontal 或 Qt::Vertical)。
     */
    [[nodiscard]] Qt::Orientation orientation() const;

    /**
     * @brief 设置分隔条的几何形状。
     *
     * 对于垂直分隔条，`pos` 是 x 坐标，`pos2` 是 y 坐标，`length` 是高度。
     * 对于水平分隔条，`pos` 是 y 坐标，`pos2` 是 x 坐标，`length` 是宽度。
     *
     * @param pos 分隔条在其方向上的位置。
     * @param pos2 分隔条在其垂直方向上的起始位置。
     * @param length 分隔条的长度（垂直分隔条的高度，水平分隔条的宽度）。
     */
    void setGeometry(int pos, int pos2, int length);

    /**
     * @brief 使用 QRect 设置分隔条的几何形状。
     * @param r 包含新几何信息的 QRect 对象。
     */
    void setGeometry(QRect r);

    /**
     * @brief 获取分隔条在其方向上的当前位置。
     * @return 对于垂直分隔条是 x 坐标，对于水平分隔条是 y 坐标。
     */
    [[nodiscard]] int position() const;

    /**
     * @brief 获取托管此分隔条的宿主 QObject。
     *
     * 这通常是包含整个 KDDockWidgets 布局的顶层窗口或部件。
     * @return 指向宿主 QObject 的指针。
     */
    [[nodiscard]] QObject *host() const;

    /**
     * @brief 初始化分隔条。
     *
     * 将分隔条关联到其父 ItemBoxContainer 并设置其方向。
     * @param parentContainer 指向父 ItemBoxContainer 的指针。
     * @param orientation 分隔条的方向 (Qt::Horizontal 或 Qt::Vertical)。
     */
    void init(Layouting::ItemBoxContainer *parentContainer, Qt::Orientation orientation);

    /**
     * @brief 获取此分隔条所属的父 ItemBoxContainer。
     * @return 指向父 ItemBoxContainer 的指针。
     */
    [[nodiscard]] ItemBoxContainer *parentContainer() const;

    /**
     * @brief 静态函数，返回当前是否有任何分隔条正在被用户拖动调整大小。
     *
     * 这对于应用程序来说可能很有用，以便在调整大小时暂停其他可能冲突或消耗性能的工作。
     * @return 如果有分隔条正在被拖动，则返回 true；否则返回 false。
     */
    static bool isResizing();

    /**
     * @brief 纯虚函数，返回代表此分隔条的实际 Widget 对象。
     *
     * 派生类必须实现此函数，以提供对底层 UI 元素的访问 (例如 QWidget 或 QQuickItem 的适配器)。
     * @return 指向表示此分隔条的 Widget 对象的指针。
     */
    virtual Widget *asWidget() = 0;

    /**
     * @brief (内部使用) 仅用于单元测试。
     *
     * 返回当前存活的 Separator 实例的总数。
     * @return 当前 Separator 实例的数量。
     */
    static int numSeparators();

protected:
    /**
     * @brief 受保护的构造函数。
     *
     * 通常由派生类调用。
     * @param hostWidget 托管此分隔条的 Widget。
     */
    explicit Separator(Widget *hostWidget);

    /**
     * @brief (可选) 创建一个橡皮筋 (RubberBand) 部件，用于在拖动分隔条时提供视觉反馈。
     *
     * 默认实现返回 nullptr。派生类可以重写此方法以提供自定义的橡皮筋。
     * @param parent 橡皮筋的父 Widget。
     * @return 指向新创建的橡皮筋 Widget 的指针，如果不支持则返回 nullptr。
     */
    virtual Widget *createRubberBand(Widget *parent)
    {
        Q_UNUSED(parent); // 标记 parent 参数为未使用，以避免编译器警告
        return nullptr;
    }

    /**
     * @brief 处理鼠标按下事件。
     *
     * 当用户在分隔条上按下鼠标按钮时，由具体的部件实现调用。
     * 通常会启动拖动会话。
     */
    void onMousePress();

    /**
     * @brief 处理鼠标释放事件。
     *
     * 当用户释放鼠标按钮（通常在拖动分隔条后）时，由具体的部件实现调用。
     * 通常会结束拖动会话并应用新的位置。
     */
    void onMouseReleased();

    /**
     * @brief 处理鼠标双击事件。
     *
     * 当用户在分隔条上双击鼠标时，由具体的部件实现调用。
     * 可能用于触发特定行为，例如将相邻项恢复到默认大小。
     */
    void onMouseDoubleClick();

    /**
     * @brief 处理鼠标移动事件。
     *
     * 当用户在按住鼠标按钮并拖动分隔条时，由具体的部件实现调用。
     * @param pos 当前鼠标相对于父部件的坐标。
     */
    void onMouseMove(QPoint pos);

private:
    friend class Config; // Config 类是友元类，可以访问 Separator 的私有成员

    Q_DISABLE_COPY(Separator) // 禁止拷贝构造函数和赋值操作符

    /**
     * @brief 设置分隔条的延迟位置。
     *
     * 如果启用了延迟调整大小 (LazyResize)，则此函数用于在拖动过程中临时存储位置，
     * 直到鼠标释放时才实际应用。
     * @param pos 新的延迟位置。
     */
    void setLazyPosition(int);

    /**
     * @brief 检查此分隔条当前是否正在被用户拖动。
     * @return 如果正在被拖动，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isBeingDragged() const;

    /**
     * @brief 检查此分隔条是否启用了延迟调整大小 (LazyResize) 选项。
     * @return 如果启用了延迟调整大小，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool usesLazyResize() const;

    static bool s_isResizing;                           ///< 静态成员，标记当前是否有任何分隔条正在调整大小。
    static Separator *s_separatorBeingDragged;          ///< 静态成员，指向当前正在被拖动的 Separator 对象，如果没有则为 nullptr。

    struct Private;     // 前向声明私有数据结构体 (PImpl 模式)
    Private *const d;   ///< 指向私有数据结构体的常量指针。
};

} // namespace Layouting

#endif // KD_MULTISPLITTER_SEPARATOR_P_H
