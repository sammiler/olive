#ifdef KDDOCKWIDGETS_QTWIDGETS
#include "FloatingWindowWidget_p.h"
#include "../DockRegistry_p.h"
#include "../DropArea_p.h"
#include "../Logging_p.h"
#include "../TitleBar_p.h"
#include "../Utils_p.h"
#include "KDDockWidgets.h"

#include <QApplication>
#include <QPainter>
#include <QVBoxLayout>
#include <QWindow>
#include <QWindowStateChangeEvent>

using namespace KDDockWidgets;

FloatingWindowWidget::FloatingWindowWidget(QRect suggestedGeometry, MainWindowBase *parent,
                                           FloatingWindowFlags flags)
    : FloatingWindow(suggestedGeometry, parent, flags)
    , m_vlayout(new QVBoxLayout(this))
{
    init();
}

FloatingWindowWidget::FloatingWindowWidget(Frame *frame, QRect suggestedGeometry, MainWindowBase *parent)
    : FloatingWindow(frame, suggestedGeometry, parent)
    , m_vlayout(new QVBoxLayout(this))
{
    init();
}

void FloatingWindowWidget::paintEvent(QPaintEvent *ev)
{
    if (Config::self().disabledPaintEvents() & Config::CustomizableWidget_FloatingWindow) {
        QWidget::paintEvent(ev);
        return;
    }

    QPainter p(this);
    QPen pen(0x666666);
    pen.setWidth(1);
    pen.setJoinStyle(Qt::MiterJoin);
    p.setPen(pen);
    const qreal halfPenWidth = p.pen().widthF() / 2;
    const QRectF rectf = rect();
    p.drawRect(rectf.adjusted(halfPenWidth, halfPenWidth, -halfPenWidth, -halfPenWidth));
}

bool FloatingWindowWidget::event(QEvent *ev)
{
    if (ev->type() == QEvent::NonClientAreaMouseButtonDblClick && (Config::self().flags() & Config::Flag_NativeTitleBar)) {
        if ((windowFlags() & Qt::Tool) == Qt::Tool) {
            if (Config::self().flags() & Config::Flag_DoubleClickMaximizes) {
                // Let's refuse to maximize Qt::Tool. It's not natural.
                // Just avoid this combination: Flag_NativeTitleBar + Qt::Tool + Flag_DoubleClickMaximizes
            } else {
                // Double clicking a Qt::Tool title-bar. Triggers a redocking.
                if (m_titleBar->isFloating()) { // redocking nested floating windows aren't supported
                    m_titleBar->onFloatClicked();
                    return true;
                }
            }
        } else {
            // A normal Qt::Window window. The OS handles the double click.
            // In general this will maximize the window, that's the native behaviour.
        }
    } else if (ev->type() == QEvent::Show && !m_screenChangedConnection) {
        // We connect after QEvent::Show, so we have a QWindow. Qt doesn't offer much API to
        // intercept screen events
        m_screenChangedConnection =
            connect(windowHandle(), &QWindow::screenChanged, DockRegistry::self(),
                    [this] { Q_EMIT DockRegistry::self()->windowChangedScreen(windowHandle()); });

        windowHandle()->installEventFilter(this);
    }

    return FloatingWindow::event(ev);
}

bool FloatingWindowWidget::eventFilter(QObject *o, QEvent *ev)
{
    if (ev->type() == QEvent::WindowStateChange) {

        // TODOv2: Have this fix for QtQuick too

        // QWidget::windowState() is not reliable as it's emitted both for the spontaneous (async) event and non-spontaneous (sync)
        // The sync one being useless, as the window manager can still have the old state.
        // Only emit windowStateChanged once the window manager tells us the state has actually changed
        // See also QTBUG-102430
        if (ev->spontaneous()) {
            m_lastWindowManagerState = windowHandle()->windowState();
            Q_EMIT windowStateChanged();
        }
    }

    return FloatingWindow::eventFilter(o, ev);
}

void FloatingWindowWidget::init()
{
    m_vlayout->setSpacing(0);
    updateMargins();
    m_vlayout->addWidget(m_titleBar);
    m_vlayout->addWidget(m_dropArea);

    connect(DockRegistry::self(), &DockRegistry::windowChangedScreen, this, [this](QWindow *w) {
        if (w == window()->windowHandle())
            updateMargins();
    });
}

void FloatingWindowWidget::updateMargins()
{
    m_vlayout->setContentsMargins(QMargins(4, 4, 4, 4) * logicalDpiFactor(this));
}


#endif
