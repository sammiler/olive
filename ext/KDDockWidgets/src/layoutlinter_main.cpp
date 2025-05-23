#include "Config.h"

#ifdef KDDOCKWIDGETS_QTQUICK
#include "private/quick/DockWidgetQuick.h"
#include "private/quick/MainWindowQuick_p.h"
#else
#include "DockWidget.h"
#include "MainWindow.h"
#endif

#include <QApplication>
#include <QDebug>
#include <QString>

using namespace KDDockWidgets;

static bool lint(const QString &filename)
{
    DockWidgetFactoryFunc dwFunc = [](const QString &dwName) {
        return static_cast<DockWidgetBase *>(new DockWidgetType(dwName));
    };

    MainWindowFactoryFunc mwFunc = [](const QString &dwName) {
        return static_cast<MainWindowBase *>(new MainWindowType(dwName));
    };

    KDDockWidgets::Config::self().setDockWidgetFactoryFunc(dwFunc);
    KDDockWidgets::Config::self().setMainWindowFactoryFunc(mwFunc);

    LayoutSaver restorer;
    return restorer.restoreFromFile(filename);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (QApplication::arguments().size() != 2) {
        qDebug() << "Usage: kddockwidgets_linter <layout json file>";
        return 1;
    }

    return lint(QApplication::arguments().at(1)) ? 0 : 2;
}
