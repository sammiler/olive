#ifndef KD_WINDOWBEINGDRAGGED_P_H
#define KD_WINDOWBEINGDRAGGED_P_H

#include "kddockwidgets/docks_export.h" // 导入导出宏定义
#include "FloatingWindow_p.h"           // FloatingWindow 私有头文件

#include <QPointer> // Qt QObject 指针的弱引用，用于安全地跟踪对象生命周期

QT_BEGIN_NAMESPACE
// 前向声明 Qt 类
class QPixmap; // Qt 图像类，用于 Wayland 拖拽时的预览图
QT_END_NAMESPACE

namespace KDDockWidgets {

// 前向声明 KDDockWidgets 内部类
class FloatingWindow; // 浮动窗口类
class Draggable;      // 可拖拽对象接口类
class LayoutWidget;   // 布局小部件基类

/**
 * @brief WindowBeingDragged 结构体代表一个正在被用户拖拽的窗口或可拖拽实体。
 *
 * 它封装了与拖拽操作相关的信息，例如关联的浮动窗口（如果存在）、
 * 原始的可拖拽对象、尺寸约束以及拖拽内容（如停靠小部件列表）。
 * 这是拖放控制器 (DragController) 和放置区域 (DropArea) 用来理解
 * 当前拖拽物属性的核心数据结构。
 * DOCKS_EXPORT_FOR_UNIT_TESTS 用于单元测试时导出此结构体。
 */
struct DOCKS_EXPORT_FOR_UNIT_TESTS WindowBeingDragged
{
public:
    /**
     * @brief 构造函数，当一个已存在的浮动窗口被拖拽时使用。
     * @param fw 指向正在被拖拽的 FloatingWindow 的指针。
     * @param draggable 指向发起拖拽的 Draggable 对象的指针。
     */
    explicit WindowBeingDragged(FloatingWindow *fw, Draggable *draggable);

#ifdef DOCKS_DEVELOPER_MODE // 仅在开发者模式下编译
    /**
     * @brief (仅供测试使用) 构造函数。
     * @param fw 指向一个 FloatingWindow 的指针。
     */
    explicit WindowBeingDragged(FloatingWindow *fw);
#endif

    /**
     * @brief 虚析构函数。
     */
    virtual ~WindowBeingDragged();
    /**
     * @brief 初始化函数，在构造后调用以完成设置。
     */
    void init();

    /**
     * @brief 获取与此拖拽操作关联的浮动窗口。
     * @return 指向 FloatingWindow 的指针，如果当前拖拽的不是一个已存在的浮动窗口，则可能为 nullptr（尤其是在拖拽开始，浮动窗口尚未创建时）。
     */
    [[nodiscard]] FloatingWindow *floatingWindow() const
    {
        return m_floatingWindow;
    }

    /**
     * @brief 捕获或释放鼠标。
     * @param grab 如果为 true，则捕获鼠标；如果为 false，则释放鼠标。
     */
    void grabMouse(bool grab);

    /**
     * @brief 返回此正在被拖拽的窗口是否包含指定的放置区域 (LayoutWidget)。
     * 用于避免将窗口拖放到其自身内部的无效操作。
     * @param layoutWidget 指向要检查的 LayoutWidget 的指针。
     * @return 如果包含，则返回 true；否则返回 false。
     */
    bool contains(LayoutWidget *layoutWidget) const;

    /**
     * @brief (虚函数) 返回正在被拖拽的窗口/内容的亲和性 (affinities) 列表。
     * 亲和性用于控制此拖拽物可以与哪些其他停靠小部件或区域进行停靠。
     * @return 亲和性名称的字符串列表。
     */
    [[nodiscard]] virtual QStringList affinities() const;

    /**
     * @brief (虚函数) 返回正在被拖拽的内容的尺寸。
     * @return 内容的 QSize。
     */
    [[nodiscard]] virtual QSize size() const;

    /**
     * @brief (虚函数) 返回正在被拖拽的内容的最小尺寸。
     * @return 内容的最小 QSize。
     */
    [[nodiscard]] virtual QSize minSize() const;

    /**
     * @brief (虚函数) 返回正在被拖拽的内容的最大尺寸。
     * @return 内容的最大 QSize。
     */
    [[nodiscard]] virtual QSize maxSize() const;

    /**
     * @brief (虚函数) 返回代表此窗口的像素图 (QPixmap)。主要用于 QDrag，仅限 Wayland 平台。
     * @return QPixmap 对象，如果不支持或无可用图像则为空。
     */
    [[nodiscard]] virtual QPixmap pixmap() const
    {
        return {}; // 默认返回空 QPixmap
    }

    /**
     * @brief (虚函数) 返回正在被拖拽的停靠小部件 (DockWidgetBase) 列表。
     * @return DockWidgetBase 指针的 QVector。
     */
    [[nodiscard]] virtual QVector<DockWidgetBase *> dockWidgets() const;

    /**
     * @brief 返回发起此次拖拽的 Draggable 对象。
     * @return 指向 Draggable 对象的指针。
     */
    [[nodiscard]] Draggable *draggable() const;

protected:
    /**
     * @brief 受保护的构造函数，当从一个非浮动窗口的 Draggable 对象开始拖拽时使用。
     * @param draggable 指向发起拖拽的 Draggable 对象的指针。
     */
    explicit WindowBeingDragged(Draggable *draggable);
    Q_DISABLE_COPY(WindowBeingDragged) ///< 禁止拷贝构造函数和拷贝赋值操作符。

    QPointer<FloatingWindow> m_floatingWindow; ///< 指向关联的浮动窗口的弱指针。
    Draggable *const m_draggable;              ///< 指向原始可拖拽对象的常量指针。
    QPointer<QWidgetOrQuick> m_draggableWidget; ///< 指向可拖拽对象的实际 QWidgetOrQuick 实例的弱指针，用于安全访问。
};

/**
 * @brief WindowBeingDragged 的 Wayland 特定版本。
 *
 * 在 Wayland 环境下，由于窗口定位的限制，浮动窗口可能无法立即创建。
 * 因此，在拖拽过程中，可能实际拖拽的是一个 Frame（包含多个标签页）或单个 DockWidget（单个标签页）。
 * 此结构体用于存储这些特定于 Wayland 拖拽场景的信息。
 */
struct WindowBeingDraggedWayland : public WindowBeingDragged
{
public:
    /**
     * @brief 构造函数。
     * @param draggable 指向发起拖拽的 Draggable 对象的指针。
     */
    explicit WindowBeingDraggedWayland(Draggable *draggable);
    /**
     * @brief 析构函数。
     */
    ~WindowBeingDraggedWayland() override;

    /**
     * @brief (重写) 返回正在被拖拽的内容的尺寸。
     * @return 内容的 QSize。
     */
    [[nodiscard]] QSize size() const override;
    /**
     * @brief (重写) 返回正在被拖拽的内容的最小尺寸。
     * @return 内容的最小 QSize。
     */
    [[nodiscard]] QSize minSize() const override;
    /**
     * @brief (重写) 返回正在被拖拽的内容的最大尺寸。
     * @return 内容的最大 QSize。
     */
    [[nodiscard]] QSize maxSize() const override;
    /**
     * @brief (重写) 返回代表此窗口的像素图 (QPixmap)。
     * @return QPixmap 对象。
     */
    [[nodiscard]] QPixmap pixmap() const override;
    /**
     * @brief (重写) 返回正在被拖拽的窗口/内容的亲和性列表。
     * @return 亲和性名称的字符串列表。
     */
    [[nodiscard]] QStringList affinities() const override;
    /**
     * @brief (重写) 返回正在被拖拽的停靠小部件列表。
     * @return DockWidgetBase 指针的 QVector。
     */
    [[nodiscard]] QVector<DockWidgetBase *> dockWidgets() const override;

    // 这两个成员仅为 Wayland 设置，在 Wayland 中我们无法立即创建浮动窗口（无法定位它）。
    // 因此，我们拖拽的要么是包含多个停靠小部件的框架，要么是单个标签页，将它们保存在这里。
    // 了解我们正在拖拽什么很重要，这样拖放橡皮筋才能尊重最小/最大尺寸。
    QPointer<Frame> m_frame;               ///< 如果正在拖拽一个框架（可能包含多个标签页），则指向该 Frame 的弱指针。
    QPointer<DockWidgetBase> m_dockWidget; ///< 如果正在拖拽单个停靠小部件（单个标签页），则指向该 DockWidgetBase 的弱指针。
};

} // namespace KDDockWidgets

#endif // KD_WINDOWBEINGDRAGGED_P_H
