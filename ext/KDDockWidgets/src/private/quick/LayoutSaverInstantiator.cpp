#ifdef KDDOCKWIDGETS_QTQUICK

/**
 * @file
 * @brief The GUI counterpart of Frame.
 *
 * @author Sérgio Martins \<sergio.martins@kdab.com\>
 */

#include "LayoutSaverInstantiator_p.h"


using namespace KDDockWidgets;

LayoutSaverInstantiator::LayoutSaverInstantiator(QObject *parent)
    : QObject(parent)
    , LayoutSaver()
{
}

LayoutSaverInstantiator::~LayoutSaverInstantiator()
{
}

bool LayoutSaverInstantiator::saveToFile(const QString &jsonFilename)
{
    return LayoutSaver::saveToFile(jsonFilename);
}

bool LayoutSaverInstantiator::restoreFromFile(const QString &jsonFilename)
{
    return LayoutSaver::restoreFromFile(jsonFilename);
}

#endif
