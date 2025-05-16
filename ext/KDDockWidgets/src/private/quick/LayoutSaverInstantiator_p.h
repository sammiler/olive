#ifndef KD_LAYOUT_SAVER_INSTANTIATOR_P_H
#define KD_LAYOUT_SAVER_INSTANTIATOR_P_H


#ifdef KDDOCKWIDGETS_QTQUICK

#include "LayoutSaver.h"

#include <QObject>

class QQuickItem;

namespace KDDockWidgets {


/**
 * @brief A QObject wrapper around LayoutSaver so it can be used in QML.
 * Use it from QML, like: LayoutSaver { id: saver }
 * For C++, just use KDDockWidgets::LayoutSaver directly
 */
class DOCKS_EXPORT LayoutSaverInstantiator
    : public QObject,
      public LayoutSaver
{
    Q_OBJECT
public:
    explicit LayoutSaverInstantiator(QObject *parent = nullptr);
    ~LayoutSaverInstantiator() override;

    Q_INVOKABLE bool saveToFile(const QString &jsonFilename);
    Q_INVOKABLE bool restoreFromFile(const QString &jsonFilename);
};

}


#endif

#endif
