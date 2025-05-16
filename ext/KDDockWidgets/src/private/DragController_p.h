#ifndef KD_DRAGCONTROLLER_P_H
#define KD_DRAGCONTROLLER_P_H

#include "kddockwidgets/docks_export.h" // 导入导出宏定义

#include "TitleBar_p.h" // 标题栏私有类头文件
#include "WindowBeingDragged_p.h" // 正在被拖拽窗口的私有类头文件

#include <QPoint> // Qt 点坐标类
#include <QMimeData> // Qt Mime 数据类
#include <QTimer> // Qt 定时器类

#include <memory> // C++ 标准库智能指针

namespace KDDockWidgets {

// 前向声明
class StateBase; // 状态基类
class StateNone; // 无操作状态类
class StateInternalMDIDragging; // MDI 内部拖拽状态类
class DropArea; // 放置区域类
class Draggable; // 可拖拽对象接口类
class FallbackMouseGrabber; // 备用鼠标捕获器类
class MinimalStateMachine; // 最小状态机类

/**
 * @brief 状态机的单个状态基类。
 *
 * 代表状态机中的一个具体状态。
 */
class State : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数。
     * @param parent 指向其所属的最小状态机。
     */
    explicit State(MinimalStateMachine *parent);
    /**
     * @brief 析构函数。
     */
    ~State() override;

    /**
     * @brief 添加一个状态转换。
     * @tparam Obj 信号发送者对象的类型。
     * @tparam Signal 信号的类型。
     * @param obj 信号发送者对象指针。
     * @param signal 信号。
     * @param dest 目标状态指针。
     */
    template<typename Obj, typename Signal>
    void addTransition(Obj *, Signal, State *dest);
    /**
     * @brief 检查当前状态是否为活动状态。
     * @return 如果是当前状态机的活动状态，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isCurrentState() const;

    /**
     * @brief 当进入此状态时调用的纯虚函数。
     *
     * 子类必须实现此方法以定义进入状态时的行为。
     */
    virtual void onEntry() = 0;
    /**
     * @brief 当退出此状态时调用的虚函数。
     *
     * 子类可以重写此方法以定义退出状态时的行为。
     */
    virtual void onExit()
    {
    }

private:
    MinimalStateMachine *const m_machine; ///< 指向所属状态机的指针。
};

/**
 * @brief 一个最小化的状态机实现。
 *
 * 管理一组状态以及它们之间的转换。
 */
class MinimalStateMachine : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数。
     * @param parent 父 QObject 对象，默认为 nullptr。
     */
    explicit MinimalStateMachine(QObject *parent = nullptr);

    /**
     * @brief 获取当前状态。
     * @return 指向当前 State 对象的指针。
     */
    [[nodiscard]] State *currentState() const;
    /**
     * @brief 设置当前状态。
     * @param state 要设置的新状态的指针。
     */
    void setCurrentState(State *);

Q_SIGNALS:
    /**
     * @brief 当当前状态改变时发射此信号。
     */
    void currentStateChanged();

private:
    State *m_currentState = nullptr; ///< 指向当前状态的指针。
};

/**
 * @brief 拖拽控制器类，管理整个拖拽操作的状态和逻辑。
 *
 * 这是一个单例类，通过状态机管理拖拽的不同阶段。
 * 它继承自 MinimalStateMachine，并定义了拖拽相关的特定状态和事件处理。
 */
class DOCKS_EXPORT DragController : public MinimalStateMachine
{
    Q_OBJECT
    /// @brief Q_PROPERTY 宏，用于暴露 isDragging 属性给 Qt 元对象系统。
    Q_PROPERTY(bool isDragging READ isDragging NOTIFY isDraggingChanged)
public:
    /**
     * @brief 定义拖拽控制器的不同状态。
     */
    enum State {
        State_None = 0, ///< 无拖拽操作状态。
        State_PreDrag, ///< 预拖拽状态，鼠标已按下但尚未达到拖拽阈值。
        State_Dragging ///< 正在拖拽状态。
    };
    Q_ENUM(State) ///< 将 State 枚举注册到 Qt 元对象系统。

    /**
     * @brief 获取 DragController 的单例实例。
     * @return 指向 DragController 单例的指针。
     */
    static DragController *instance();

    /**
     * @brief 注册一个可拖拽的对象。
     * @param draggable 指向要注册的 Draggable 对象的指针。
     */
    void registerDraggable(Draggable *);
    /**
     * @brief 注销一个可拖拽的对象。
     * @param draggable 指向要注销的 Draggable 对象的指针。
     */
    void unregisterDraggable(Draggable *);

    /**
     * @brief 检查当前是否正在进行拖拽操作。
     * @return 如果正在拖拽，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isDragging() const;
    /**
     * @brief 检查当前是否处于非客户区拖拽状态。
     *
     * 非客户区拖拽通常指拖拽窗口的标题栏或边框。
     * @return 如果是非客户区拖拽，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isInNonClientDrag() const;
    /**
     * @brief 检查当前是否处于客户区拖拽状态。
     *
     * 客户区拖拽通常指拖拽小部件内部的内容。
     * @return 如果是客户区拖拽，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isInClientDrag() const;
    /**
     * @brief 检查拖拽控制器是否处于空闲状态 (即 State_None)。
     * @return 如果处于空闲状态，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isIdle() const;

    /**
     * @brief 为指定的小部件（QWidget 或 QQuickItem）捕获鼠标。
     * @param widget 指向 QWidgetOrQuick 对象的指针。
     */
    void grabMouseFor(QWidgetOrQuick *);
    /**
     * @brief 释放指定小部件（QWidget 或 QQuickItem）的鼠标捕获。
     * @param widget 指向 QWidgetOrQuick 对象的指针。
     */
    void releaseMouse(QWidgetOrQuick *);

    /**
     * @brief 获取当前正在被拖拽的浮动窗口。
     * @return 如果有浮动窗口正在被拖拽，则返回其指针；否则返回 nullptr。
     */
    [[nodiscard]] FloatingWindow *floatingWindowBeingDragged() const;

    /**
     * @brief 返回当前鼠标光标下的放置区域 (DropArea)。
     * @return 指向当前 DropArea 的指针，如果鼠标下没有放置区域则返回 nullptr。
     */
    [[nodiscard]] DropArea *dropAreaUnderCursor() const;

    /**
     * @brief 返回正在被拖拽的窗口对象。
     * @return 指向 WindowBeingDragged 对象的指针。
     */
    [[nodiscard]] WindowBeingDragged *windowBeingDragged() const;

    /**
     * @brief 启用备用鼠标捕获器（实验性功能，内部使用，不建议通用）。
     */
    void enableFallbackMouseGrabber();

    /**
     * @brief 返回当前的活动状态对象。
     * @return 指向当前 StateBase 对象的指针。
     */
    [[nodiscard]] StateBase *activeState() const;

Q_SIGNALS:
    /**
     * @brief 当鼠标按下时发射此信号。
     */
    void mousePressed();
    /**
     * @brief 当鼠标移动达到曼哈顿距离阈值时发射此信号（用于标准停靠）。
     */
    void manhattanLengthMove();
    /**
     * @brief 当鼠标移动达到曼哈顿距离阈值时发射此信号（用于 MDI 内部拖拽）。
     */
    void manhattanLengthMoveMDI();
    /**
     * @brief 当 MDI 窗口被拖拽出成为浮动窗口时发射此信号。
     */
    void mdiPopOut();
    /**
     * @brief 当拖拽操作被取消时发射此信号。
     */
    void dragCanceled();
    /**
     * @brief 当拖拽物被放置时发射此信号。
     */
    void dropped();
    /**
     * @brief 当 isDragging 属性的状态改变时发射此信号。
     */
    void isDraggingChanged();

protected:
    /**
     * @brief 事件过滤器，用于拦截和处理拖拽相关的事件。
     * @param watched 被监视的对象。
     * @param event 发生的事件。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    bool eventFilter(QObject *, QEvent *) override;

private:
    // 声明友元类，允许它们访问此类的私有成员
    friend class StateBase;
    friend class StateNone;
    friend class StatePreDrag;
    friend class StateDragging;
    friend class StateInternalMDIDragging;
    friend class StateDropped;
    friend class StateDraggingWayland;

    /**
     * @brief 私有构造函数，确保单例模式。
     * @param parent 父 QObject 对象，默认为 nullptr。
     */
    explicit DragController(QObject * = nullptr);
    /**
     * @brief 获取当前鼠标光标下的顶层 Qt 小部件。
     * @return 指向顶层 WidgetType 的指针，如果不存在则为 nullptr。
     */
    [[nodiscard]] WidgetType *qtTopLevelUnderCursor() const;
    /**
     * @brief 根据 QObject 获取对应的 Draggable 对象。
     * @param o 指向 QObject 的指针。
     * @return 指向 Draggable 对象的指针，如果未找到则为 nullptr。
     */
    Draggable *draggableForQObject(QObject *o) const;

    QPoint m_pressPos; ///< 鼠标按下的初始位置。
    QPoint m_offset; ///< 拖拽开始时鼠标指针相对于被拖拽对象左上角的偏移量。

    Draggable::List m_draggables; ///< 已注册的可拖拽对象列表。
    Draggable *m_draggable = nullptr; ///< 当前正在被处理的可拖拽对象。
    QPointer<WidgetType> m_draggableGuard; ///< 用于监视可拖拽对象是否被销毁的 QPointer。
    std::unique_ptr<WindowBeingDragged> m_windowBeingDragged; ///< 指向正在被拖拽窗口对象的智能指针。
    DropArea *m_currentDropArea = nullptr; ///< 当前鼠标光标下的放置区域。
    bool m_nonClientDrag = false; ///< 标记是否为非客户区拖拽。
    FallbackMouseGrabber *m_fallbackMouseGrabber = nullptr; ///< 备用鼠标捕获器实例。
    StateNone *m_stateNone = nullptr; ///< “无操作”状态的实例。
    StateInternalMDIDragging *m_stateDraggingMDI = nullptr; ///< “MDI 内部拖拽”状态的实例。
};

/**
 * @brief 所有拖拽状态类的基类。
 *
 * 定义了各种事件处理的虚函数接口，供具体状态类实现。
 */
class StateBase : public State
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数。
     * @param parent 指向 DragController 的指针。
     */
    explicit StateBase(DragController *parent);
    /**
     * @brief 析构函数。
     */
    ~StateBase() override;

    /**
     * @brief 处理鼠标按钮按下事件的虚函数。
     * @param receiver 事件接收者（可拖拽对象）。
     * @param globalPos 全局鼠标位置。
     * @param pos 相对于接收者的鼠标位置。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    virtual bool handleMouseButtonPress(Draggable * /*receiver*/, QPoint /*globalPos*/, QPoint /*pos*/)
    {
        return false;
    }
    /**
     * @brief 处理鼠标移动事件的虚函数。
     * @param globalPos 全局鼠标位置。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    virtual bool handleMouseMove(QPoint /*globalPos*/)
    {
        return false;
    }
    /**
     * @brief 处理鼠标按钮释放事件的虚函数。
     * @param globalPos 全局鼠标位置。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    virtual bool handleMouseButtonRelease(QPoint /*globalPos*/)
    {
        return false;
    }
    /**
     * @brief 处理鼠标双击事件的虚函数。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    virtual bool handleMouseDoubleClick()
    {
        return false;
    }

    /**
     * @brief 处理拖拽进入事件的虚函数 (主要用于 Wayland)。
     * @param event 拖拽进入事件。
     * @param dropArea 目标放置区域。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    virtual bool handleDragEnter(QDragEnterEvent *, DropArea *)
    {
        return false;
    }
    /**
     * @brief 处理拖拽离开事件的虚函数 (主要用于 Wayland)。
     * @param dropArea 离开的放置区域。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    virtual bool handleDragLeave(DropArea *)
    {
        return false;
    }
    /**
     * @brief 处理拖拽移动事件的虚函数 (主要用于 Wayland)。
     * @param event 拖拽移动事件。
     * @param dropArea 当前的放置区域。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    virtual bool handleDragMove(QDragMoveEvent *, DropArea *)
    {
        return false;
    }
    /**
     * @brief 处理放置事件的虚函数 (主要用于 Wayland)。
     * @param event 放置事件。
     * @param dropArea 目标放置区域。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    virtual bool handleDrop(QDropEvent *, DropArea *)
    {
        return false;
    }

    /**
     * @brief 检查此状态是否为当前活动状态。
     * @return 如果是活动状态，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isActiveState() const;

    DragController *const q; ///< 指向 DragController 的指针 (q-pointer 模式)。
};

/**
 * @brief "无操作"状态实现。
 *
 * 表示当前没有进行任何拖拽操作。
 */
class StateNone : public StateBase
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数。
     * @param parent 指向 DragController 的指针。
     */
    explicit StateNone(DragController *parent);
    /**
     * @brief 析构函数。
     */
    ~StateNone() override;
    /**
     * @brief 进入此状态时的处理逻辑。
     */
    void onEntry() override;
    /**
     * @brief 处理鼠标按钮按下事件。
     * @param draggable 事件接收者（可拖拽对象）。
     * @param globalPos 全局鼠标位置。
     * @param pos 相对于接收者的鼠标位置。
     * @return 如果事件被处理（例如，转换到预拖拽状态），则返回 true；否则返回 false。
     */
    bool handleMouseButtonPress(Draggable *draggable, QPoint globalPos, QPoint pos) override;
};

/**
 * @brief "预拖拽"状态实现。
 *
 * 鼠标已经按下，但尚未移动足够的距离以启动正式的拖拽操作。
 */
class StatePreDrag : public StateBase
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数。
     * @param parent 指向 DragController 的指针。
     */
    explicit StatePreDrag(DragController *parent);
    /**
     * @brief 析构函数。
     */
    ~StatePreDrag() override;
    /**
     * @brief 进入此状态时的处理逻辑。
     */
    void onEntry() override;
    /**
     * @brief 处理鼠标移动事件。
     * @param globalPos 全局鼠标位置。
     * @return 如果事件被处理（例如，转换到拖拽状态），则返回 true；否则返回 false。
     */
    bool handleMouseMove(QPoint globalPos) override;
    /**
     * @brief 处理鼠标按钮释放事件。
     * @param globalPos 全局鼠标位置。
     * @return 如果事件被处理（例如，转换回无操作状态），则返回 true；否则返回 false。
     */
    bool handleMouseButtonRelease(QPoint globalPos) override;
    /**
     * @brief 处理鼠标双击事件。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    bool handleMouseDoubleClick() override;
};

/**
 * @brief "正在拖拽"状态实现。
 *
 * 用于除 Wayland 之外的所有平台。
 * @see StateDraggingWayland
 */
class StateDragging : public StateBase
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数。
     * @param parent 指向 DragController 的指针。
     */
    explicit StateDragging(DragController *parent);
    /**
     * @brief 析构函数。
     */
    ~StateDragging() override;
    /**
     * @brief 进入此状态时的处理逻辑。
     */
    void onEntry() override;
    /**
     * @brief 退出此状态时的处理逻辑。
     */
    void onExit() override;
    /**
     * @brief 处理鼠标按钮释放事件（表示拖拽结束并放置）。
     * @param globalPos 全局鼠标位置。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    bool handleMouseButtonRelease(QPoint globalPos) override;
    /**
     * @brief 处理鼠标移动事件（更新拖拽窗口位置和目标区域）。
     * @param globalPos 全局鼠标位置。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    bool handleMouseMove(QPoint globalPos) override;
    /**
     * @brief 处理鼠标双击事件。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    bool handleMouseDoubleClick() override;

private:
    QTimer m_maybeCancelDrag; ///< 用于在特定条件下（例如鼠标移出窗口）可能取消拖拽的定时器。
};


/**
 * @brief "MDI 内部拖拽"状态实现。
 *
 * 当在主窗口内移动 MDI 停靠小部件而不使其浮动时使用此状态。
 */
class StateInternalMDIDragging : public StateBase
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数。
     * @param parent 指向 DragController 的指针。
     */
    explicit StateInternalMDIDragging(DragController *parent);
    /**
     * @brief 析构函数。
     */
    ~StateInternalMDIDragging() override;
    /**
     * @brief 进入此状态时的处理逻辑。
     */
    void onEntry() override;
    /**
     * @brief 处理鼠标按钮释放事件。
     * @param globalPos 全局鼠标位置。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    bool handleMouseButtonRelease(QPoint globalPos) override;
    /**
     * @brief 处理鼠标移动事件。
     * @param globalPos 全局鼠标位置。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    bool handleMouseMove(QPoint globalPos) override;
    /**
     * @brief 处理鼠标双击事件。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    bool handleMouseDoubleClick() override;
};

/**
 * @brief "Wayland 拖拽"状态实现。
 *
 * 仅在 Wayland 平台下使用，利用 QDrag 而不是在鼠标移动时直接设置几何形状。
 * 继承自 StateDragging。
 */
class StateDraggingWayland : public StateDragging
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数。
     * @param parent 指向 DragController 的指针。
     */
    explicit StateDraggingWayland(DragController *parent);
    /**
     * @brief 析构函数。
     */
    ~StateDraggingWayland() override;
    /**
     * @brief 进入此状态时的处理逻辑，会启动 QDrag。
     */
    void onEntry() override;
    /**
     * @brief 处理鼠标按钮释放事件。
     * @param globalPos 全局鼠标位置。
     * @return 始终返回 false，因为 Wayland 拖拽由 QDrag 管理。
     */
    bool handleMouseButtonRelease(QPoint globalPos) override;
    /**
     * @brief 处理鼠标移动事件。
     * @param globalPos 全局鼠标位置。
     * @return 始终返回 false，因为 Wayland 拖拽由 QDrag 管理。
     */
    bool handleMouseMove(QPoint globalPos) override;
    /**
     * @brief 处理拖拽进入事件。
     * @param event 拖拽进入事件。
     * @param dropArea 目标放置区域。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    bool handleDragEnter(QDragEnterEvent *, DropArea *) override;
    /**
     * @brief 处理拖拽移动事件。
     * @param event 拖拽移动事件。
     * @param dropArea 当前的放置区域。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    bool handleDragMove(QDragMoveEvent *, DropArea *) override;
    /**
     * @brief 处理拖拽离开事件。
     * @param dropArea 离开的放置区域。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    bool handleDragLeave(DropArea *) override;
    /**
     * @brief 处理放置事件。
     * @param event 放置事件。
     * @param dropArea 目标放置区域。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    bool handleDrop(QDropEvent *, DropArea *) override;

    bool m_inQDrag = false; ///< 标记当前是否正在进行 QDrag 操作。
};

/**
 * @brief Wayland 平台下用于拖拽的自定义 MIME 数据类型。
 *
 * 这是一个 QMimeData 的子类，仅用于类型安全检查，确保只接受由此类发起的拖放操作。
 */
class WaylandMimeData : public QMimeData
{
    Q_OBJECT
public:
};

} // namespace KDDockWidgets

#endif // KD_DRAGCONTROLLER_P_H