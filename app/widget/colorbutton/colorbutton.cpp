#include "colorbutton.h"

#include "dialog/color/colordialog.h"

namespace olive {

ColorButton::ColorButton(ColorManager *color_manager, bool show_dialog_on_click, QWidget *parent)
    : QPushButton(parent), color_manager_(color_manager), color_processor_(nullptr), dialog_open_(false) {
  setAutoFillBackground(true);

  if (show_dialog_on_click) {
    connect(this, &ColorButton::clicked, this, &ColorButton::ShowColorDialog);
  }

  SetColor(ManagedColor(Color(1.0f, 1.0f, 1.0f)));
}

const ManagedColor &ColorButton::GetColor() const { return color_; }

void ColorButton::SetColor(const ManagedColor &c) {
  color_ = c;

  color_.set_color_input(color_manager_->GetCompliantColorSpace(color_.color_input()));
  color_.set_color_output(color_manager_->GetCompliantColorSpace(color_.color_output()));

  UpdateColor();
}

void ColorButton::ShowColorDialog() {
  if (!dialog_open_) {
    dialog_open_ = true;
    auto *cd = new ColorDialog(color_manager_, color_, this);

    connect(cd, &ColorDialog::finished, this, &ColorButton::ColorDialogFinished);

    cd->show();
  }
}

void ColorButton::ColorDialogFinished(int e) {
  auto *cd = dynamic_cast<ColorDialog *>(sender());

  if (e == QDialog::Accepted) {
    color_ = cd->GetSelectedColor();

    UpdateColor();

    emit ColorChanged(color_);
  }

  cd->deleteLater();

  dialog_open_ = false;
}

void ColorButton::UpdateColor() {
  color_processor_ = ColorProcessor::Create(color_manager_, color_.color_input(), color_.color_output());

  QColor managed = QtUtils::toQColor(color_processor_->ConvertColor(color_));

  setStyleSheet(QStringLiteral("%1--ColorButton {background: %2;}").arg(MACRO_VAL_AS_STR(olive), managed.name()));
}

}  // namespace olive
