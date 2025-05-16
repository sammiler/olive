#ifndef KDDOCKWIDGETS_QT5QT6_COMPAT_P_H
#define KDDOCKWIDGETS_QT5QT6_COMPAT_P_H

#include <QMouseEvent> // 包含 Qt 鼠标事件类
#include <QDropEvent> // 包含 Qt 拖放事件中的放置事件类


namespace KDDockWidgets::Qt5Qt6Compat { // KDDockWidgets 内部用于 Qt5/Qt6 兼容性的命名空间

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0) // 条件编译：如果 Qt 版本大于等于 6.0.0

// Qt 6 及更高版本的兼容性定义

/**
 * @brief 在 Qt 6 中，QQuickItem 的 geometryChanged 信号更名为 geometryChange。
 * 此宏定义了一个别名 QQUICKITEMgeometryChanged 以便在代码中统一使用，
 * 实际会根据 Qt 版本解析为正确的信号名称。
 */
#define QQUICKITEMgeometryChanged geometryChange

// 类型别名，在 Qt 6 中这些类型通常是标准或已明确定义的。
using QEnterEvent = QEnterEvent; ///< QEnterEvent 类型在 Qt 6 中保持不变。
using qintptr = ::qintptr; ///< qintptr 类型在 Qt 6 中通常是标准定义的整数指针类型。
using qhashtype = size_t; ///< qHash 函数返回值的类型，在 Qt 6 中通常是 size_t。

/**
 * @brief (Qt 6) 从 QDropEvent 获取事件发生的位置。
 * Qt 6 中 QDropEvent::position() 返回 QPointF，需要转换为 QPoint。
 * @param ev 指向 QDropEvent 的指针。
 * @return 事件发生的 QPoint。
 */
inline QPoint eventPos(QDropEvent *ev)
{
    return ev->position().toPoint();
}

/**
 * @brief (Qt 6) 从 QMouseEvent 获取事件发生的全局位置。
 * Qt 6 中 QMouseEvent::globalPosition() 返回 QPointF，需要转换为 QPoint。
 * @param ev 指向 QMouseEvent 的指针。
 * @return 事件发生的全局 QPoint。
 */
inline QPoint eventGlobalPos(QMouseEvent *ev)
{
    return ev->globalPosition().toPoint();
}

#else // Qt 5 的兼容性定义 (QT_VERSION < 6.0.0)

/**
 * @brief 在 Qt 5 中，QQuickItem 的 geometryChanged 信号名称就是 geometryChanged。
 * 此宏定义了一个别名 QQUICKITEMgeometryChanged 以便在代码中统一使用。
 */
#define QQUICKITEMgeometryChanged geometryChanged

// 类型别名，用于兼容 Qt 5
using QEnterEvent = QEvent; ///< 在 Qt 5 中，QEnterEvent 可能不直接存在或用法不同，有时用 QEvent::Enter 代替，这里直接用 QEvent 作为一种通用兼容方式 (具体场景需确认)。
using qintptr = long; ///< qintptr 类型在 Qt 5 的某些配置或平台上可能是 long。
using qhashtype = uint; ///< qHash 函数返回值的类型，在 Qt 5 中通常是 uint。

/**
 * @brief (Qt 5) 从 QDropEvent 获取事件发生的位置。
 * Qt 5 中 QDropEvent::pos() 直接返回 QPoint。
 * @param ev 指向 QDropEvent 的指针。
 * @return 事件发生的 QPoint。
 */
inline QPoint eventPos(QDropEvent *ev)
{
    return ev->pos();
}

/**
 * @brief (Qt 5) 从 QMouseEvent 获取事件发生的全局位置。
 * Qt 5 中 QMouseEvent::globalPos() 直接返回 QPoint。
 * @param ev 指向 QMouseEvent 的指针。
 * @return 事件发生的全局 QPoint。
 */
inline QPoint eventGlobalPos(QMouseEvent *ev)
{
    return ev->globalPos();
}

#endif // QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

} // namespace KDDockWidgets::Qt5Qt6Compat


#endif // KDDOCKWIDGETS_QT5QT6_COMPAT_P_H
