#pragma once

// Make "signals:", "slots:" visible as access specifiers
#define QT_ANNOTATE_ACCESS_SPECIFIER(a) __attribute__((annotate(#a)))

// Define PYTHON_BINDINGS this will be used in some part of c++ to skip problematic parts
#define PYTHON_BINDINGS

#ifndef QT_WIDGETS_LIB
#define QT_WIDGETS_LIB
#endif

#include <kddockwidgets/MainWindowBase.h>
#include <kddockwidgets/MainWindow.h>
#include <kddockwidgets/DockWidgetBase.h>
#include <kddockwidgets/DockWidget.h>
#include <kddockwidgets/LayoutSaver.h>
