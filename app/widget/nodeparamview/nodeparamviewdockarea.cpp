

#include "nodeparamviewdockarea.h"

#include <QDockWidget>

namespace olive {

NodeParamViewDockArea::NodeParamViewDockArea(QWidget *parent) : QMainWindow(parent) {
  // Disable dock widgets from tabbing and disable glitchy animations
  setDockOptions(static_cast<QMainWindow::DockOption>(0));

  // HACK: Hide the main window separators (unfortunately the cursors still appear)
  setStyleSheet(QStringLiteral("QMainWindow::separator {background: rgba(0, 0, 0, 0)}"));
}

QMenu *NodeParamViewDockArea::createPopupMenu() { return nullptr; }

void NodeParamViewDockArea::AddItem(QDockWidget *item) {
  item->setAllowedAreas(Qt::LeftDockWidgetArea);
  item->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
  addDockWidget(Qt::LeftDockWidgetArea, item);
}

}  // namespace olive
