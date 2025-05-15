

#ifndef CURVEPANEL_H
#define CURVEPANEL_H

#include "panel/timebased/timebased.h"
#include "widget/curvewidget/curvewidget.h"

namespace olive {

class CurvePanel : public TimeBasedPanel {
  Q_OBJECT
 public:
  CurvePanel();

  void DeleteSelected() override;

  void SelectAll() override;

  void DeselectAll() override;

 public slots:
  void SetNode(Node *node) {
    // Convert single pointer to either an empty vector or a vector of one
    QVector<Node *> nodes;

    if (node) {
      nodes.append(node);
    }

    SetNodes(nodes);
  }

  void SetNodes(const QVector<Node *> &nodes);

  void IncreaseTrackHeight() override;

  void DecreaseTrackHeight() override;

 protected:
  void Retranslate() override;
};

}  // namespace olive

#endif  // CURVEPANEL_H
