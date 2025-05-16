#pragma once

#include <kddockwidgets/DockWidget.h>
#include <kddockwidgets/MainWindow.h>

class MyMainWindow : public KDDockWidgets::MainWindow
{
    Q_OBJECT
public:
    explicit MyMainWindow(const QString &uniqueName, KDDockWidgets::MainWindowOptions options,
                          bool dockWidget0IsNonClosable, bool nonDockableDockWidget9, bool restoreIsRelative,
                          bool maxSizeForDockWidget8, bool dockwidget5DoesntCloseBeforeRestore, bool dock0BlocksCloseEvent,
                          const QString &affinityName = {}, // Usually not needed. Just here to show the feature.
                          QWidget *parent = nullptr);
    ~MyMainWindow() override;

private:
    void createDockWidgets();
    bool eventFilter(QObject *obj, QEvent *ev) override;
    KDDockWidgets::DockWidgetBase *newDockWidget();
    QMenu *m_toggleMenu = nullptr;
    const bool m_dockWidget0IsNonClosable;
    const bool m_dockWidget9IsNonDockable;
    const bool m_restoreIsRelative;
    const bool m_maxSizeForDockWidget8;
    const bool m_dockwidget5DoesntCloseBeforeRestore;
    const bool m_dock0BlocksCloseEvent;
    KDDockWidgets::DockWidget::List m_dockwidgets;
};
