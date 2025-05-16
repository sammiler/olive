/**
 * @file
 * @brief Tree Widget to show the object tree. Used for debugging only, for apps that don't support GammaRay.
 *
 * @author Sérgio Martins \<sergio.martins@kdab.com\>
 */

#ifndef OBJECTVIEWER_H
#define OBJECTVIEWER_H

#ifdef KDDOCKWIDGETS_QTWIDGETS

#include <QWidget>
#include <QStandardItemModel>
#include <QTreeView>
#include <QPointer>
#include <QObject>
#include <QMenu>

QT_BEGIN_NAMESPACE
class QStandardItem;
QT_END_NAMESPACE


namespace KDDockWidgets::Debug {

class ObjectViewer : public QWidget // clazy:exclude=missing-qobject-macro
{
public:
    explicit ObjectViewer(QWidget *parent = nullptr);

    void refresh();

private:
    void dumpSelectedWidgetToPng();
    void updateSelectedWidget();
    void toggleVisible();
    static void dumpWindows();
    static QString nameForObj(QObject *o);
    void add(QObject *obj, QStandardItem *parent);
    void remove(QObject *obj);
    void onSelectionChanged();
    static void printProperties(QObject *);
    [[nodiscard]] QObject *selectedObject() const;
    [[nodiscard]] QWidget *selectedWidget() const;
    void updateItemAppearence(QStandardItem *);
    static QObject *objectForItem(QStandardItem *);
    static QWidget *widgetForItem(QStandardItem *);

#ifdef Q_OS_WIN
    void sendHitTest();
#endif

    QTreeView m_treeView;
    QStandardItemModel m_model;
    QPointer<QObject> m_selectedObject;
    QMenu m_menu;
    bool m_highlightsWidget = true;
    bool m_ignoreMenus = true;
    bool m_ignoreShortcuts = true;
    bool m_ignoreToolBars = true;
    QHash<QObject *, QStandardItem *> m_itemMap;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
};
}



#endif

#endif
