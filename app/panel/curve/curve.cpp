#include "curve.h"

namespace olive {

CurvePanel::CurvePanel() : TimeBasedPanel(QStringLiteral("CurvePanel")) {
  // Create main widget and set it
  SetTimeBasedWidget(new CurveWidget(this));

  // Set strings
  Retranslate();
}

void CurvePanel::DeleteSelected() { dynamic_cast<CurveWidget*>(GetTimeBasedWidget())->DeleteSelected(); }

void CurvePanel::SelectAll() { dynamic_cast<CurveWidget*>(GetTimeBasedWidget())->SelectAll(); }

void CurvePanel::DeselectAll() { dynamic_cast<CurveWidget*>(GetTimeBasedWidget())->DeselectAll(); }

void CurvePanel::SetNodes(const QVector<Node*>& nodes) {
  dynamic_cast<CurveWidget*>(GetTimeBasedWidget())->SetNodes(nodes);
}

void CurvePanel::IncreaseTrackHeight() {
  auto* c = dynamic_cast<CurveWidget*>(GetTimeBasedWidget());
  c->SetVerticalScale(c->GetVerticalScale() * 2);
}

void CurvePanel::DecreaseTrackHeight() {
  auto* c = dynamic_cast<CurveWidget*>(GetTimeBasedWidget());
  c->SetVerticalScale(c->GetVerticalScale() * 0.5);
}

void CurvePanel::Retranslate() {
  TimeBasedPanel::Retranslate();

  SetTitle(tr("Curve Editor"));
}

}  // namespace olive
