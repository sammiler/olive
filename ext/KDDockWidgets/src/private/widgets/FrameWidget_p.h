#ifndef KD_FRAME_WIDGET_P_H
#define KD_FRAME_WIDGET_P_H

#ifdef KDDOCKWIDGETS_QTWIDGETS

#include "../Frame_p.h"

QT_BEGIN_NAMESPACE
class QTabBar;
QT_END_NAMESPACE

class TestDocks;

namespace KDDockWidgets {

class TabWidget;

/**
 * @brief The GUI counterpart of Frame. Inherits Frame and implements paintEvent().
 */
class DOCKS_EXPORT FrameWidget : public Frame
{
    Q_OBJECT
public:
    explicit FrameWidget(QWidget *parent = nullptr, FrameOptions = FrameOption_None,
                         int userType = 0);
    ~FrameWidget() override;
    [[nodiscard]] QTabBar *tabBar() const;

protected:
    void paintEvent(QPaintEvent *) override;
    [[nodiscard]] QSize maxSizeHint() const override;
    int indexOfDockWidget_impl(const DockWidgetBase *) override;
    void setCurrentDockWidget_impl(DockWidgetBase *) override;
    [[nodiscard]] int currentIndex_impl() const override;
    void insertDockWidget_impl(DockWidgetBase *, int index) override;
    void removeWidget_impl(DockWidgetBase *) override;
    void setCurrentTabIndex_impl(int) override;
    [[nodiscard]] DockWidgetBase *currentDockWidget_impl() const override;
    [[nodiscard]] DockWidgetBase *dockWidgetAt_impl(int index) const override;
    [[nodiscard]] QRect dragRect() const override;
    void renameTab(int index, const QString &) override;
    void changeTabIcon(int index, const QIcon &) override;
    [[nodiscard]] int nonContentsHeight() const override;

    [[nodiscard]] QSize minimumSizeHint() const override;

private:
    friend class ::TestDocks;
};


}

#endif


#endif
