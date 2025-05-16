/**
 * @file
 * @brief FocusScope
 *
 * @author Sérgio Martins \<sergio.martins@kdab.com\>
 */

#ifndef KD_DOCKWIDGETS_FOCUSSCOPE_H
#define KD_DOCKWIDGETS_FOCUSSCOPE_H

#include "docks_export.h"
#include "QWidgetAdapter.h"

namespace KDDockWidgets {
///@brief Allows to implement a similar functionality to QtQuick's FocusScope item, in QtWidgets
class DOCKS_EXPORT FocusScope
{
    Q_DISABLE_COPY(FocusScope)
public:
    ///@brief constructor
    explicit FocusScope(QWidgetAdapter *thisWidget);
    virtual ~FocusScope();

    ///@brief Returns true if this FocusScope is focused.
    /// This is similar to the QWidget::hasFocus(), except that it counts with the children being focused too.
    /// i.e: If any child is focused then this FocusScope has focus too.
    [[nodiscard]] bool isFocused() const;

    ///@brief Returns the widget that's focused in this scope
    /// The widget itself might not have focus as in QWidget::hasFocus(), but will get actual focus
    /// as soon as this scope is focused.
    [[nodiscard]] WidgetType *focusedWidget() const;

    ///@brief Sets focus on this scope.
    ///
    /// This will call QWidget::focus() on the last QWidget that was focused in this scope.
    void focus(Qt::FocusReason = Qt::OtherFocusReason);

    /*Q_SIGNALS:*/
protected:
    ///@brief reimplement in the 1st QObject derived class
    virtual void isFocusedChangedCallback() = 0;
    virtual void focusedWidgetChangedCallback() = 0;

private:
    class Private;
    Private *const d;
};
}

#endif
