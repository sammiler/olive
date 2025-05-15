#ifndef NODEPARAMVIEWDOCKAREA_H
#define NODEPARAMVIEWDOCKAREA_H

#include <QMainWindow>

namespace olive {

// This may look weird, but QMainWindow is just a QWidget with a fancy layout that allows
// for docking QDockWidgets
class NodeParamViewDockArea : public QMainWindow {
  Q_OBJECT
 public:
  explicit NodeParamViewDockArea(QWidget *parent = nullptr);

  QMenu *createPopupMenu() override;

  void AddItem(QDockWidget *item);
};

}  // namespace olive

#endif  // NODEPARAMVIEWDOCKAREA_H
