#ifdef KDDOCKWIDGETS_QTQUICK

#ifndef KD_TITLEBARQUICK_P_H
#define KD_TITLEBARQUICK_P_H

#include "kddockwidgets/docks_export.h"
#include "kddockwidgets/private/TitleBar_p.h"

namespace KDDockWidgets {

class DockWidget;
class Frame;

class DOCKS_EXPORT TitleBarQuick : public TitleBar
{
    Q_OBJECT
    // These properties is just for the unit-tests
    Q_PROPERTY(QQuickItem *titleBarQmlItem READ titleBarQmlItem WRITE setTitleBarQmlItem NOTIFY titleBarQmlItemChanged)
    Q_PROPERTY(QQuickItem *titleBarMouseArea READ titleBarMouseArea CONSTANT)
public:
    explicit TitleBarQuick(Frame *parent);
    explicit TitleBarQuick(FloatingWindow *parent);
    ~TitleBarQuick() override;

protected:
#ifdef DOCKS_DEVELOPER_MODE
    // These 4 just for unit-tests
    bool isCloseButtonEnabled() const override;
    bool isCloseButtonVisible() const override;
    bool isFloatButtonEnabled() const override;
    bool isFloatButtonVisible() const override;
#endif

    /*void paintEvent(QPaintEvent *) override;
    void mouseDoubleClickEvent(QMouseEvent *) override; */

    QQuickItem *titleBarQmlItem() const;
    QQuickItem *titleBarMouseArea() const;
    void setTitleBarQmlItem(QQuickItem *);

Q_SIGNALS:
    void titleBarQmlItemChanged();

private:
    QQuickItem *floatButton() const;
    QQuickItem *closeButton() const;

    QPointer<QQuickItem> m_titleBarQmlItem;
};

}

#endif

#endif
