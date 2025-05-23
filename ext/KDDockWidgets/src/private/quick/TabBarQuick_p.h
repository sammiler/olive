/**
 * @file
 * @brief Implements a QTabWidget derived class with support for docking and undocking
 * KDockWidget::DockWidget as tabs .
 *
 * @author Sérgio Martins \<sergio.martins@kdab.com\>
 */

#ifndef KD_TABBAR_QUICK_P_H
#define KD_TABBAR_QUICK_P_H

#ifdef KDDOCKWIDGETS_QTQUICK


#include "../TabWidget_p.h"


namespace KDDockWidgets {

class DockWidget;
class TabWidget;

class DOCKS_EXPORT TabBarQuick
    : public QWidgetAdapter,
      public TabBar
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *tabBarQmlItem READ tabBarQmlItem WRITE setTabBarQmlItem NOTIFY tabBarQmlItemChanged)
public:
    explicit TabBarQuick(TabWidget *parent = nullptr);
    int tabAt(QPoint localPos) const override;

    QQuickItem *tabBarQmlItem() const;
    void setTabBarQmlItem(QQuickItem *);

    QString text(int index) const override;
    QRect rectForTab(int index) const override;

    void moveTabTo(int from, int to) override;

Q_SIGNALS:
    void tabBarQmlItemChanged();

protected:
    bool event(QEvent *ev) override;

private:
    QQuickItem *tabAt(int index) const;
    QQuickItem *listView() const;
    QPointer<QQuickItem> m_tabBarQmlItem;
};
}

#endif


#endif
