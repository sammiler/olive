#include "panel.h"

#include <QCloseEvent>
#include <QDebug>
#include <QHBoxLayout>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QVariant>

#include "panel/panelmanager.h"

namespace olive {

#define super KDDockWidgets::DockWidget

PanelWidget::PanelWidget(const QString &object_name)
    : super(object_name), border_visible_(false), signal_instead_of_close_(false) {
  setFocusPolicy(Qt::ClickFocus);

  connect(this, &PanelWidget::shown, this, static_cast<void (PanelWidget::*)()>(&PanelWidget::setFocus));

  PanelManager::instance()->RegisterPanel(this);
}

PanelWidget::~PanelWidget() { PanelManager::instance()->UnregisterPanel(this); }

void PanelWidget::SetBorderVisible(bool enabled) {
  border_visible_ = enabled;

  update();
}

void PanelWidget::SetTitle(const QString &t) {
  title_ = t;
  UpdateTitle();
}

void PanelWidget::SetSubtitle(const QString &t) {
  subtitle_ = t;
  UpdateTitle();
}

void PanelWidget::paintEvent(QPaintEvent *event) {
  // Perform default behavior
  super::paintEvent(event);

  // Check if this panel (or a child of it) has focus using PanelFocusManager
  if (border_visible_) {
    // Draw a highlight border if so
    QPainter p(this);

    // We need to adjust the rect by 1 pixel since the bottom and right are "offscreen"
    QRect highlight_border = rect();
    highlight_border.adjust(0, 0, -1, -1);

    // Set the color to the palette's highlight color
    p.setPen(palette().highlight().color());

    // Draw the highlight border
    p.drawRect(highlight_border);
  }
}

void PanelWidget::UpdateTitle() {
  // If there's no subtitle, just use the title. Otherwise, we set a formatted combination of the two that can
  // differ based on translation
  if (subtitle_.isEmpty()) {
    this->setTitle(title_);
  } else {
    this->setTitle(tr("%1: %2").arg(title_, subtitle_));
  }
}

void PanelWidget::SetSignalInsteadOfClose(bool e) { signal_instead_of_close_ = e; }

void PanelWidget::closeEvent(QCloseEvent *event) {
  if (signal_instead_of_close_) {
    event->ignore();
    emit CloseRequested();
  } else {
    super::closeEvent(event);
  }
}

void PanelWidget::changeEvent(QEvent *e) {
  if (e->type() == QEvent::LanguageChange) {
    Retranslate();
  }
  super::changeEvent(e);
}

void PanelWidget::Retranslate() {}

void PanelWidget::SetWidgetWithPadding(QWidget *widget) {
  auto *wrapper = new QWidget();
  auto *layout = new QHBoxLayout(wrapper);
  layout->setContentsMargins(layout->contentsMargins() / 2);
  layout->addWidget(widget);
  setWidget(wrapper);
}

}  // namespace olive
