#pragma once

#include <kddockwidgets/FrameworkWidgetFactory.h>

#include <QPainter>

// clazy:excludeall=ctor-missing-parent-argument

class CustomWidgetFactory : public KDDockWidgets::DefaultWidgetFactory
{
    Q_OBJECT
public:
    KDDockWidgets::TitleBar *createTitleBar(KDDockWidgets::Frame *frame) const override;
    KDDockWidgets::TitleBar *createTitleBar(KDDockWidgets::FloatingWindow *fw) const override;
    Layouting::Separator *createSeparator(Layouting::Widget *parent = nullptr) const override;
};
