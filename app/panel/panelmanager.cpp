#include "panelmanager.h"

#include "config/config.h"

namespace olive {

PanelManager *PanelManager::instance_ = nullptr;

PanelManager::PanelManager(QObject *parent) : QObject(parent), suppress_changed_signal_(false) {}

void PanelManager::DeleteAllPanels() {
  // Prevent any confusion regarding focus history by clearing it first
  QList<PanelWidget *> copy = focus_history_;
  focus_history_.clear();
  qDeleteAll(copy);
}

const QList<PanelWidget *> &PanelManager::panels() { return focus_history_; }

PanelWidget *PanelManager::CurrentlyFocused(bool enable_hover) const {
  // If hover focus is enabled, find the currently hovered panel and return it (if no panel is hovered, resort to
  // default behavior)
  if (enable_hover && OLIVE_CONFIG("HoverFocus").toBool()) {
    PanelWidget *hovered = CurrentlyHovered();

    if (hovered != nullptr) {
      return hovered;
    }
  }

  if (focus_history_.isEmpty()) {
    return nullptr;
  }

  return focus_history_.first();
}

PanelWidget *PanelManager::CurrentlyHovered() const {
  QPoint global_mouse = QCursor::pos();

  foreach (PanelWidget *panel, focus_history_) {
    if (panel->rect().contains(panel->mapFromGlobal(global_mouse))) {
      return panel;
    }
  }

  return nullptr;
}

PanelWidget *PanelManager::GetPanelWithName(const QString &name) const {
  foreach (PanelWidget *panel, focus_history_) {
    if (panel->objectName() == name) {
      return panel;
    }
  }

  return nullptr;
}

void PanelManager::CreateInstance() { instance_ = new PanelManager(); }

void PanelManager::DestroyInstance() { delete instance_; }

PanelManager *PanelManager::instance() { return instance_; }

void PanelManager::RegisterPanel(PanelWidget *panel) {
  // Add panel to the bottom of the focus history
  focus_history_.append(panel);

  // We're about to center the panel relative to the parent (usually the main window), but for some
  // reason this requires the panel to be shown first.
  panel->show();

  if (focus_history_.size() == 1) {
    // This is the first panel, focus it
    panel->SetBorderVisible(true);
    emit FocusedPanelChanged(panel);
  }
}

void PanelManager::UnregisterPanel(PanelWidget *panel) { focus_history_.removeOne(panel); }

void PanelManager::FocusChanged(QWidget *old, QWidget *now) {
  Q_UNUSED(old)

  QObject *parent = now;
  PanelWidget *panel_cast_test;

  // Loop through widget's parent hierarchy
  if (!focus_history_.empty()) {
    while (parent != nullptr) {
      // Use dynamic_cast to test if this object is a PanelWidget
      panel_cast_test = dynamic_cast<PanelWidget *>(parent);

      if (panel_cast_test) {
        if (focus_history_.first() != panel_cast_test) {
          // If so, bump this to the top of the focus history
          int panel_index = focus_history_.indexOf(panel_cast_test);

          // Disable highlight border on old panel
          if (!focus_history_.isEmpty()) {
            focus_history_.first()->SetBorderVisible(false);
          }

          // Enable new border's highlight
          panel_cast_test->SetBorderVisible(true);

          // If it's not in the focus history, prepend it, otherwise move it
          if (panel_index == -1) {
            focus_history_.prepend(panel_cast_test);
          } else {
            focus_history_.move(panel_index, 0);
          }

          if (!suppress_changed_signal_) {
            emit FocusedPanelChanged(panel_cast_test);
          }
        }

        break;
      }

      parent = parent->parent();
    }
  }
}

}  // namespace olive
