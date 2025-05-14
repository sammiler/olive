/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2019-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

/**
 * @file
 * @brief A DockWidget wrapper that adds a QTabWidget and a TitleBar.
 *
 * @author Sérgio Martins \<sergio.martins@kdab.com\>
 */

#ifndef KD_FRAME_P_H
#define KD_FRAME_P_H

#include "kddockwidgets/docks_export.h"
#include "kddockwidgets/QWidgetAdapter.h"
#include "kddockwidgets/FocusScope.h"
#include "kddockwidgets/DockWidgetBase.h"
#include "kddockwidgets/LayoutSaver.h"
#include "multisplitter/Widget.h"

#include <QVector>
#include <QDebug>
#include <QPointer>

class TestDocks;

namespace KDDockWidgets {

class TitleBar;
class TabWidget;
class DockWidgetBase;
class FloatingWindow;
class MainWindowBase;
class MDILayoutWidget;
class WidgetResizeHandler;

/**
 * @brief A DockWidget wrapper that adds a QTabWidget and a TitleBar
 *
 * Frame is the actual widget that goes into the MultiSplitter. It provides a TitleBar which you
 * can use to detach, and also a QTabWidget so you can tab dock widgets together.
 *
 * This class doesn't actually add window frames and it's never a top-level widget. A Frame is
 * always inside a LayoutWidget. Be it a MultiSplitter belonging to a MainWindow or belonging to a
 * FloatingWindow.
 */
class DOCKS_EXPORT Frame
    : public LayoutGuestWidget,
      public FocusScope
{
    Q_OBJECT
    Q_PROPERTY(KDDockWidgets::TitleBar *titleBar READ titleBar CONSTANT)
    Q_PROPERTY(KDDockWidgets::TitleBar *actualTitleBar READ actualTitleBar NOTIFY actualTitleBarChanged)
    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentDockWidgetChanged)
    Q_PROPERTY(int userType READ userType CONSTANT)
    Q_PROPERTY(bool isMDI READ isMDI NOTIFY isMDIChanged)
public:
    typedef QList<Frame *> List;

    explicit Frame(QWidgetOrQuick *parent = nullptr, FrameOptions = FrameOption_None,
                   int userType = 0);
    ~Frame() override;

    static Frame *deserialize(const LayoutSaver::Frame &);
    [[nodiscard]]  LayoutSaver::Frame serialize() const;

    ///@brief Adds a widget into the Frame's TabWidget
    void addWidget(DockWidgetBase *, InitialOption = {});
    ///@overload
    void addWidget(Frame *, InitialOption = {});
    ///@overload
    void addWidget(FloatingWindow *floatingWindow, InitialOption = {});

    ///@brief Inserts a widget into the Frame's TabWidget at @p index
    void insertWidget(DockWidgetBase *, int index, InitialOption = {});

    ///@brief removes a dockwidget from the frame
    void removeWidget(DockWidgetBase *);

    ///@brief detaches this dock widget
    FloatingWindow *detachTab(DockWidgetBase *);

    ///@brief returns the index of the specified dock widget
    int indexOfDockWidget(const DockWidgetBase *);

    ///@brief returns the index of the current tab
    [[nodiscard]]  int currentIndex() const;

    ///@brief sets the current tab index
    void setCurrentTabIndex(int index);

    ///@brief Sets the specified dock widget to be the current tab
    void setCurrentDockWidget(DockWidgetBase *);

    ///@brief Inserts a dock widget into the specified index
    void insertDockWidget(DockWidgetBase *, int index);

    /// @brief Returns the dock widget at @p index
    [[nodiscard]]  DockWidgetBase *dockWidgetAt(int index) const;

    ///@brief Returns the current dock widget
    [[nodiscard]]  DockWidgetBase *currentDockWidget() const;

    /// @brief returns the number of dock widgets inside the frame
    [[nodiscard]]  int dockWidgetCount() const;

    /// @brief returns the tab widget
    [[nodiscard]]  TabWidget *tabWidget() const;

    void updateTitleAndIcon();
    void onDockWidgetTitleChanged();
    void updateTitleBarVisibility();
    void updateFloatingActions();
    [[nodiscard]]  bool containsMouse(QPoint globalPos) const;
    [[nodiscard]]  TitleBar *titleBar() const;
    [[nodiscard]]  TitleBar *actualTitleBar() const;
    [[nodiscard]]  QString title() const;
    [[nodiscard]]  QIcon icon() const;
    [[nodiscard]]  const QVector<DockWidgetBase *> dockWidgets() const;

    [[nodiscard]]  bool isTheOnlyFrame() const;

    ///@brief Returns whether this frame is overlayed on top of the MainWindow (auto-hide feature);
    [[nodiscard]]  bool isOverlayed() const;

    ///@brief clears the FrameOption_IsOverlayed flag.
    /// For example, if you drag a side-bar overlay, then it becomes a normal floating window
    void unoverlay();

    /**
     * @brief Returns whether this frame is floating. A floating frame isn't attached to any other MainWindow,
     * and if it's attached to a FloatingWindow then it's considered floating if it's the only frame in that Window.
     * A floating frame can have multiple dock widgets (tabbed), in which case each DockWidget::isFloating() returns false,
     * in which case you can use isInFloatingWindow() which would still return true
     */
    [[nodiscard]]  bool isFloating() const;

    /**
     * @brief Returns whether this frame is in a FloatingWindow, as opposed to MainWindow.
     *
     * After setup it's equivalent to !isInMainWindow().
     */
    [[nodiscard]]  bool isInFloatingWindow() const;

    /**
     * @brief Returns whether this frame is docked inside a MainWindow.
     */
    [[nodiscard]]  bool isInMainWindow() const;

    /**
     * @brief returns if this widget is the central frame
     * MainWindow supports a mode where the middle frame is persistent even if no dock widget is there.
     *
     * @return whether this widget is the central frame in a main window
     */
    [[nodiscard]]  bool isCentralFrame() const
    {
        return m_options & FrameOption_IsCentralFrame;
    }

    /// @brief Returns whether you can DND dock widgets over this frame and tab into it
    [[nodiscard]]  bool isDockable() const
    {
        return !(m_options & FrameOption_NonDockable);
    }

    /**
     * @brief whether the tab widget will always show tabs, even if there's only 1 dock widget
     *
     * While technically a non-floating dock widget is always tabbed, the user won't see the tabs
     * as in most cases there's only 1 widget tabbed. But for the main window central frame it's
     * often wanted to see tabs even if there's only 1 widget, where each widget represents a "document".
     *
     * @return whether the tab widget will always show tabs, even if there's only 1 dock widget
     */
    [[nodiscard]]  bool alwaysShowsTabs() const
    {
        return m_options & FrameOption_AlwaysShowsTabs;
    }

    /// @brief returns whether the dockwidget @p w is inside this frame
    bool containsDockWidget(DockWidgetBase *w) const;

    ///@brief returns the FloatingWindow this frame is in, if any
    [[nodiscard]]  FloatingWindow *floatingWindow() const;

    /**
     * @brief Returns the main window this frame is in.
     * nullptr if not inside a main window.
     */
    [[nodiscard]]  MainWindowBase *mainWindow() const;

    /**
     * @brief Puts the Frame back in its previous main window position
     *
     * Usually DockWidget::Private::restoreToPreviousPosition() is used, but
     * when we have a floating frame with tabs we just reuse the frame instead of
     * moving the tabbed dock widgets one by one.
     */
    void restoreToPreviousPosition();

    void onCloseEvent(QCloseEvent *e) override;
    [[nodiscard]]  int currentTabIndex() const;

    [[nodiscard]]  FrameOptions options() const
    {
        return m_options;
    }
    [[nodiscard]]  bool anyNonClosable() const;
    [[nodiscard]]  bool anyNonDockable() const;

    ///@brief returns whether there's 0 dock widgets. If not persistent then the Frame will delete itself.
    [[nodiscard]]  bool isEmpty() const
    {
        return dockWidgetCount() == 0;
    }

    ///@brief returns whether there's only 1 dock widget.
    [[nodiscard]]  bool hasSingleDockWidget() const
    {
        return dockWidgetCount() == 1;
    }

    ///@brief Called when a dock widget child @p w is shown
    void onDockWidgetShown(DockWidgetBase *w);

    ///@brief Called when a dock widget child @p w is hidden
    void onDockWidgetHidden(DockWidgetBase *w);

    ///@brief returns the layout item that either contains this Frame in the layout or is a placeholder
    [[nodiscard]]  Layouting::Item *layoutItem() const;

    ///@brief For tests-only. Returns the number of Frame instances in the whole application.
    static int dbg_numFrames();

    /**
     * @brief Returns whether a deleteLater has already been issued
     */
    [[nodiscard]]  bool beingDeletedLater() const;

    /**
     * @brief returns true if tabs are visible
     *
     * @sa hasTabsVisibleChanged()
     **/
    [[nodiscard]]  bool hasTabsVisible() const;

    [[nodiscard]]  QStringList affinities() const;

    ///@brief sets the layout item that either contains this Frame in the layout or is a placeholder
    void setLayoutItem(Layouting::Item *item) override;

    /**
     * Returns the drag rect in global coordinates. This is usually the title bar rect.
     * However, when using Config::Flag_HideTitleBarWhenTabsVisible it will be the tab bar background.
     * Returns global coordinates.
     */
    [[nodiscard]]  virtual QRect dragRect() const;

    ///@brief Returns whether all dock widgets have the specified option set
    [[nodiscard]]  bool allDockWidgetsHave(DockWidgetBase::Option) const;

    ///@brief Returns whether at least one dock widget has the specified option set
    [[nodiscard]]  bool anyDockWidgetsHas(DockWidgetBase::Option) const;

    ///@brief Returns whether all dock widgets have the specified  layout saver option set
    [[nodiscard]]  bool allDockWidgetsHave(DockWidgetBase::LayoutSaverOption) const;

    ///@brief Returns whether at least one dock widget has the specified layout saver option set
    [[nodiscard]]  bool anyDockWidgetsHas(DockWidgetBase::LayoutSaverOption) const;

    /// @brief Usually we do resize via the native window manager, but if a widget is docked like
    /// in MDI mode, or in overlayed mode then we allow the user to resize with mouse
    void setAllowedResizeSides(CursorPositions sides);

    /// @brief Returns whether this frame is in a MDI layout
    /// Usually no, unless you're using an MDI main window
    [[nodiscard]]  bool isMDI() const;

    /// @brief Returns whether this frame was created automatically just for the purpose of supporting DockWidget::Option_MDINestable
    [[nodiscard]]  bool isMDIWrapper() const;

    /// @brief If this is an MDI wrapper frame, return the DockWidget MDI wrapper
    /// @sa isMDIWrapper
    [[nodiscard]]  DockWidgetBase *mdiDockWidgetWrapper() const;

    /// @brief If this is an MDI wrapper frame, return the DropArea MDI wrapper
    /// @sa isMDIWrapper
    [[nodiscard]]  DropArea *mdiDropAreaWrapper() const;

    /// @brief If this frame is an MDI wrapper, returns the MDI frame. That is the frame you actually drag inside the MDI area
    [[nodiscard]]  Frame *mdiFrame() const;

    /// @brief Returns the MDI layout. Or nullptr if this frame isn't in a MDI layout
    [[nodiscard]]  MDILayoutWidget *mdiLayoutWidget() const;

    /// @brief If this frame is a MDI frame (isMDI() == true), returns whether it contains nested dock widgets (DockWidget::Option_MDINestable)
    /// @sa isMDI()
    [[nodiscard]]  bool hasNestedMDIDockWidgets() const;

    /// @brief See DockWidgetBase::userType()
    [[nodiscard]]  int userType() const;

    /// @brief Returns the resize handler. Used mostly in MDI mode.
    [[nodiscard]]  WidgetResizeHandler *resizeHandler() const;

    /// @brief User requested floating window flags for when this group floats
    [[nodiscard]]  FloatingWindowFlags requestedFloatingWindowFlags() const;

Q_SIGNALS:
    void currentDockWidgetChanged(KDDockWidgets::DockWidgetBase *);
    void numDockWidgetsChanged();
    void hasTabsVisibleChanged();
    void layoutInvalidated();
    void isInMainWindowChanged();
    void isFocusedChanged();
    void focusedWidgetChanged();
    void isMDIChanged();
    void actualTitleBarChanged();

protected:
    void isFocusedChangedCallback() final;
    void focusedWidgetChangedCallback() final;

protected Q_SLOTS:
    void onDockWidgetCountChanged();
    void onCurrentTabChanged(int index);

protected:
    virtual void renameTab(int index, const QString &) = 0;
    virtual void changeTabIcon(int index, const QIcon &) = 0;

    /**
     * @brief Returns the minimum size of the dock widgets.
     * This might be slightly smaller than Frame::minSize() due to the QTabWidget having some margins
     * and tab bar.
     */
    [[nodiscard]]  QSize dockWidgetsMinSize() const;

    /**
     * @brief Returns the biggest combined maxSize of all dock widgets.
     *
     * Example:
     *   dock 1, max=2000x1000
     *   dock 2, max=3000x400
     *   dock3, max=
     *   result=3000,1000
     *
     * Any widget having 16777215x16777215 is ignored (represents not having a max-size, QWIDGETSIZE_MAX)
     */
    [[nodiscard]]  QSize biggestDockWidgetMaxSize() const;

    virtual void removeWidget_impl(DockWidgetBase *) = 0;
    virtual int indexOfDockWidget_impl(const DockWidgetBase *) = 0;
    [[nodiscard]]  virtual int currentIndex_impl() const = 0;
    virtual void setCurrentTabIndex_impl(int index) = 0;
    virtual void setCurrentDockWidget_impl(DockWidgetBase *) = 0;
    virtual void insertDockWidget_impl(DockWidgetBase *, int index) = 0;
    [[nodiscard]]  virtual DockWidgetBase *dockWidgetAt_impl(int index) const = 0;
    [[nodiscard]]  virtual DockWidgetBase *currentDockWidget_impl() const = 0;
    [[nodiscard]]  virtual int nonContentsHeight() const = 0;

private:
    bool m_inCtor = true; // Needs to be initialized early, as pointed out by UBSAN
protected:
    bool m_inDtor = false;

    TabWidget *const m_tabWidget;
    TitleBar *const m_titleBar;

private:
    Q_DISABLE_COPY(Frame)
    friend class ::TestDocks;
    friend class TabWidget;

    void scheduleDeleteLater();
    bool event(QEvent *) override;

    /// @brief Sets the LayoutWidget which this frame is in
    void setLayoutWidget(LayoutWidget *);

    LayoutWidget *m_layoutWidget = nullptr;
    WidgetResizeHandler *m_resizeHandler = nullptr;
    FrameOptions m_options = FrameOption_None;
    QPointer<Layouting::Item> m_layoutItem;
    bool m_updatingTitleBar = false;
    bool m_beingDeleted = false;
    int m_userType = 0;
    QMetaObject::Connection m_visibleWidgetCountChangedConnection;
};

}

inline QDebug operator<<(QDebug d, KDDockWidgets::Frame *frame)
{
    if (frame) {
        d << static_cast<QObject *>(frame);
        d << "; window=" << frame->window();
        d << "; options=" << frame->options();
        d << "; dockwidgets=" << frame->dockWidgets();
    } else {
        d << "nullptr";
    }

    return d;
}

#endif
