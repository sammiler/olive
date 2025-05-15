

#include "tool.h"

#include "core.h"
#include "widget/toolbar/toolbar.h"

namespace olive {

ToolPanel::ToolPanel() : PanelWidget(QStringLiteral("ToolPanel")) {
  auto* t = new Toolbar(this);

  t->SetTool(Core::instance()->tool());
  t->SetSnapping(Core::instance()->snapping());

  SetWidgetWithPadding(t);

  connect(t, &Toolbar::ToolChanged, Core::instance(), &Core::SetTool);
  connect(Core::instance(), &Core::ToolChanged, t, &Toolbar::SetTool);

  connect(t, &Toolbar::SnappingChanged, Core::instance(), &Core::SetSnapping);
  connect(Core::instance(), &Core::SnappingChanged, t, &Toolbar::SetSnapping);

  connect(t, &Toolbar::SelectedTransitionChanged, Core::instance(), &Core::SetSelectedTransitionObject);

  Retranslate();
}

void ToolPanel::Retranslate() { SetTitle(tr("Tools")); }

}  // namespace olive
