/**
 * @file
 * @brief QMainWindow sub-class to enable KDDockWidgets support.
 *
 * @author Sérgio Martins \<sergio.martins@kdab.com\>
 */

#ifndef KD_MAINWINDOW_H
#define KD_MAINWINDOW_H

#ifdef KDDOCKWIDGETS_QTWIDGETS

#include "MainWindowBase.h"

QT_BEGIN_NAMESPACE
class QHBoxLayout;
QT_END_NAMESPACE

namespace KDDockWidgets {

class SideBar;

/**
 * @brief The QMainwindow sub-class that the application should use to be able
 * to dock KDDockWidget::DockWidget instances.
 */
class DOCKS_EXPORT MainWindow : public MainWindowBase
{
    Q_OBJECT
public:
    typedef QVector<MainWindow *> List;

    ///@brief Constructor. Use it as you would use QMainWindow.
    ///@param uniqueName Mandatory name that should be unique between all MainWindow instances.
    ///       This name won't be user visible and just used internally for the save/restore.
    ///@param options optional MainWindowOptions to use
    ///@param parent QObject *parent to pass to QMainWindow constructor.
    ///@param flags Window flags to  pass to QMainWindow constructor.
    explicit MainWindow(const QString &uniqueName, MainWindowOptions options = MainWindowOption_None,
                        QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    ///@brief Destructor
    ~MainWindow() override;

    ///@brief returns the sidebar for the specified location
    [[nodiscard]] SideBar *sideBar(SideBarLocation) const override;

    //@brief returns the margins for the contents widget
    [[nodiscard]] QMargins centerWidgetMargins() const override;

    //@brief sets the margins for the contents widgets
    void setCenterWidgetMargins(QMargins);

    ///@brief returns the internal layout
    /// this is rarely needed unless you want to layout other widgets next to the
    /// main dock widget layout
    [[nodiscard]] QHBoxLayout *internalLayout() const;

protected:
    void closeEvent(QCloseEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    [[nodiscard]] QRect centralAreaGeometry() const override;

private:
    using QMainWindow::setCentralWidget;
    void setCentralWidget(QWidget *); // overridden just to make it private
    class Private;
    Private *const d;
};

}

#endif


#endif
