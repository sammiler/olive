

#ifndef MULTICAMWIDGET_H
#define MULTICAMWIDGET_H

#include "multicamdisplay.h"
#include "node/input/multicam/multicamnode.h"
#include "widget/viewer/viewer.h"

namespace olive {

class MulticamWidget : public TimeBasedWidget {
  Q_OBJECT
 public:
  explicit MulticamWidget(QWidget *parent = nullptr);

  [[nodiscard]] MulticamDisplay *GetDisplayWidget() const { return display_; }

  void SetMulticamNode(ViewerOutput *viewer, MultiCamNode *n, ClipBlock *clip, const rational &time);

 protected:
  void ConnectNodeEvent(ViewerOutput *n) override;
  void DisconnectNodeEvent(ViewerOutput *n) override;
  void TimeChangedEvent(const rational &t) override;

 signals:
  void Switched();

 private:
  void SetMulticamNodeInternal(ViewerOutput *viewer, MultiCamNode *n, ClipBlock *clip);

  void Switch(int source, bool split_clip);

  ViewerSizer *sizer_;

  MulticamDisplay *display_;

  MultiCamNode *node_;

  ClipBlock *clip_;

  struct MulticamNodeQueue {
    rational time;
    ViewerOutput *viewer;
    MultiCamNode *node;
    ClipBlock *clip;
  };

  std::list<MulticamNodeQueue> play_queue_;

 private slots:
  void DisplayClicked(const QPoint &p);
};

}  // namespace olive

#endif  // MULTICAMWIDGET_H
