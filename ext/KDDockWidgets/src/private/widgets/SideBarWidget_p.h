#ifndef KD_SIDEBARWIDGET_P_H
#define KD_SIDEBARWIDGET_P_H
#ifdef KDDOCKWIDGETS_QTWIDGETS
#include "kddockwidgets/docks_export.h"
#include "../SideBar_p.h"

#include <QToolButton>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QBoxLayout;
class QAbstractButton;
QT_END_NAMESPACE

namespace KDDockWidgets {

class DockWidget;
class Frame;
class SideBarWidget;

class DOCKS_EXPORT SideBarButton : public QToolButton
{
    Q_OBJECT
public:
    explicit SideBarButton(DockWidgetBase *dw, SideBarWidget *parent);
    [[nodiscard]] bool isVertical() const;
    void paintEvent(QPaintEvent *) override;
    [[nodiscard]] QSize sizeHint() const override;

private:
    SideBarWidget *const m_sideBar;
    const QPointer<DockWidgetBase> m_dockWidget;
};

class DOCKS_EXPORT SideBarWidget : public SideBar
{
    Q_OBJECT
public:
    explicit SideBarWidget(SideBarLocation, KDDockWidgets::MainWindowBase *parent);

protected:
    void addDockWidget_Impl(DockWidgetBase *dw) override;
    void removeDockWidget_Impl(DockWidgetBase *dock) override;

    // virtual so users can provide their own buttons
    virtual SideBarButton *createButton(DockWidgetBase *dw, SideBarWidget *parent) const;

private:
    QBoxLayout *const m_layout;
};

}

#endif


#endif
