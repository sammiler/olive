/**
 * @file
 * @brief Implements a QTabWidget derived class with support for docking and undocking
 * KDockWidget::DockWidget as tabs .
 *
 * @author Sérgio Martins \<sergio.martins@kdab.com\>
 */

#ifndef KD_TABBAR_WIDGET_P_H
#define KD_TABBAR_WIDGET_P_H
#ifdef KDDOCKWIDGETS_QTWIDGETS

#include "../TabWidget_p.h"

#include <QTabBar>

QT_BEGIN_NAMESPACE
class QMouseEvent;
QT_END_NAMESPACE

// clazy:excludeall=ctor-missing-parent-argument

namespace KDDockWidgets {

class DockWidget;
class TabWidget;

class DOCKS_EXPORT TabBarWidget
    : public QTabBar,
      public TabBar
{
    Q_OBJECT
public:
    explicit TabBarWidget(TabWidget *parent = nullptr);
    [[nodiscard]] int tabAt(QPoint localPos) const override;

    [[nodiscard]] DockWidgetBase *currentDockWidget() const;

    [[nodiscard]] QString text(int index) const override;
    [[nodiscard]] QRect rectForTab(int index) const override;
    void moveTabTo(int from, int to) override;

Q_SIGNALS:
    void dockWidgetInserted(int index);
    void dockWidgetRemoved(int index);

protected:
    [[nodiscard]] bool dragCanStart(QPoint pressPos, QPoint pos) const override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    bool event(QEvent *) override;
    void tabInserted(int index) override;
    void tabRemoved(int index) override;

private:
    TabWidget *const m_tabWidget;
};
}


#endif

#endif
