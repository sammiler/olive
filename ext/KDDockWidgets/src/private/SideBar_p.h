#ifndef KD_SIDEBAR_P_H
#define KD_SIDEBAR_P_H

#include "kddockwidgets/docks_export.h"

#include "kddockwidgets/KDDockWidgets.h"
#include "kddockwidgets/QWidgetAdapter.h"

namespace KDDockWidgets {

class DockWidgetBase;
class MainWindowBase;

class DOCKS_EXPORT SideBar : public QWidgetAdapter
{
    Q_OBJECT
public:
    explicit SideBar(SideBarLocation, MainWindowBase *parent = nullptr);

    void addDockWidget(DockWidgetBase *dw);
    void removeDockWidget(DockWidgetBase *dw);
    bool containsDockWidget(DockWidgetBase *) const;

    /// @brief Returns this side bar's orientation
    [[nodiscard]] Qt::Orientation orientation() const;

    /// @brief returns if this side bar has vertical orientation
    [[nodiscard]] bool isVertical() const
    {
        return m_orientation == Qt::Vertical;
    }

    /// @brief returns whether there's no dock widgets
    [[nodiscard]] bool isEmpty() const;

    /// @brief returns the sidebar's location in the main window
    [[nodiscard]] SideBarLocation location() const;

    /// @brief Returns the main window this side bar belongs to
    [[nodiscard]] MainWindowBase *mainWindow() const;

    /// @brief Toggles the dock widget overlay. Equivalent to the user clicking on the button.
    void toggleOverlay(DockWidgetBase *);

    /// @brief returns a serialization of this sidebar's state
    /// Currently it's just a list of dock widget ids
    [[nodiscard]] QStringList serialize() const;

    /// @brief clears the sidebar (removes all dock widgets from it)
    void clear();

protected:
    virtual void addDockWidget_Impl(DockWidgetBase *dock) = 0;
    virtual void removeDockWidget_Impl(DockWidgetBase *dock) = 0;

    void onButtonClicked(DockWidgetBase *dw);

    [[nodiscard]] QVector<DockWidgetBase *> dockWidgets() const;

private:
    void onDockWidgetDestroyed(QObject *dw);
    void updateSize();

    MainWindowBase *const m_mainWindow;
    QVector<DockWidgetBase *> m_dockWidgets;
    const SideBarLocation m_location;
    const Qt::Orientation m_orientation;
};

}

#endif
