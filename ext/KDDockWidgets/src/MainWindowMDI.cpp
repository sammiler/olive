#include "MainWindowMDI.h"
#include "private/MDILayoutWidget_p.h"

using namespace KDDockWidgets;

MainWindowMDI::MainWindowMDI(const QString &uniqueName, WidgetType *parent, Qt::WindowFlags flags)
    : MDIMainWindowBase(uniqueName, MainWindowOption_MDI, parent, flags)
{
}

MainWindowMDI::~MainWindowMDI() = default;

void MainWindowMDI::addDockWidget(DockWidgetBase *dockWidget, QPoint localPos, InitialOption addingOption)
{
    auto layout = dynamic_cast<MDILayoutWidget *>(this->layoutWidget());
    layout->addDockWidget(dockWidget, localPos, addingOption);
}

void MainWindowMDI::addDockWidget(DockWidgetBase *dockWidget, QPointF localPos, InitialOption addingOption)
{
    MainWindowMDI::addDockWidget(dockWidget, localPos.toPoint(), addingOption);
}
