/**
 * @file
 * @brief FocusScope (焦点范围) 类，用于在 QtWidgets 中实现类似 QtQuick FocusScope 的功能。
 *
 * @author Sérgio Martins <sergio.martins@kdab.com>
 */

#ifndef KD_DOCKWIDGETS_FOCUSSCOPE_H
#define KD_DOCKWIDGETS_FOCUSSCOPE_H

#include "docks_export.h"         // 导入导出宏定义
#include "QWidgetAdapter.h"       // QWidget 和 QQuickItem 的适配器类

namespace KDDockWidgets {

/**
 * @brief FocusScope 类允许在 QtWidgets 中实现类似于 QtQuick 的 FocusScope 项的功能。
 *
 * FocusScope 管理一个“范围”内的焦点。当这个范围获得焦点时，它会将实际的键盘焦点
 * 传递给该范围内最后一个获得焦点的小部件。如果范围内的某个子小部件获得焦点，
 * 那么这个 FocusScope 也被认为是获得了焦点。
 *
 * 这对于像停靠小部件这样的复合组件非常有用，其中整个停靠小部件（作为一个单元）
 * 可以获得或失去焦点，而实际的键盘输入会定向到其内部特定的输入控件。
 */
class DOCKS_EXPORT FocusScope
{
    Q_DISABLE_COPY(FocusScope) // 禁止拷贝构造函数和拷贝赋值操作符，确保 FocusScope 对象不被意外复制。
public:
    /**
     * @brief 构造函数。
     * @param thisWidget 指向与此 FocusScope 关联的 QWidgetAdapter 实例。
     * 这个 QWidgetAdapter 通常包装了实现 FocusScope 行为的 QWidget 或 QQuickItem。
     */
    explicit FocusScope(QWidgetAdapter *thisWidget);
    /**
     * @brief 虚析构函数。
     */
    virtual ~FocusScope();

    /**
     * @brief 返回此 FocusScope 是否拥有焦点。
     *
     * 这类似于 QWidget::hasFocus()，不同之处在于它也考虑了子小部件是否拥有焦点。
     * 即：如果任何子小部件拥有焦点，则此 FocusScope 也拥有焦点。
     * @return 如果此 FocusScope 或其任何子项拥有焦点，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isFocused() const;

    /**
     * @brief 返回此作用域内拥有焦点的小部件。
     *
     * 该小部件本身可能不像 QWidget::hasFocus() 那样拥有实际焦点，
     * 但一旦此作用域获得焦点，它将获得实际焦点。
     * @return 指向当前在此作用域内逻辑上拥有焦点的小部件 (WidgetType*) 的指针。
     * 如果作用域内没有特定小部件拥有焦点记忆，则可能返回 nullptr 或默认小部件。
     */
    [[nodiscard]] WidgetType *focusedWidget() const;

    /**
     * @brief 将焦点设置到此作用域上。
     *
     * 这将在此作用域内最后获得焦点的 QWidget 上调用 QWidget::setFocus()。
     * @param reason 焦点设置的原因，默认为 Qt::OtherFocusReason。
     */
    void focus(Qt::FocusReason reason = Qt::OtherFocusReason);

    /*Q_SIGNALS:*/ // 注释掉的信号部分，实际信号应在派生的 QObject 子类中声明。
protected:
    /**
     * @brief (纯虚函数) 当 isFocused 状态改变时由子类实现的回调函数。
     *
     * 派生自 FocusScope 的第一个 QObject 子类需要重写此方法，
     * 以便在焦点状态实际改变时发出相应的信号 (例如 isFocusedChanged())。
     */
    virtual void isFocusedChangedCallback() = 0;
    /**
     * @brief (纯虚函数) 当 focusedWidget 改变时由子类实现的回调函数。
     *
     * 派生自 FocusScope 的第一个 QObject 子类需要重写此方法，
     * 以便在作用域内逻辑焦点小部件改变时发出相应的信号 (例如 focusedWidgetChanged())。
     */
    virtual void focusedWidgetChangedCallback() = 0;

private:
    class Private;   ///< PIMPL (Private Implementation) 设计模式的私有实现类前向声明。
    Private *const d; ///< 指向私有实现类的常量指针。
};
} // namespace KDDockWidgets

#endif // KD_DOCKWIDGETS_FOCUSSCOPE_H
