#include "colordialog.h"

#include <QDialogButtonBox>
#include <QSplitter>
#include <QVBoxLayout>

#include "common/qtutils.h"

namespace olive {

ColorDialog::ColorDialog(ColorManager* color_manager, const ManagedColor& start, QWidget* parent)
    : QDialog(parent), color_manager_(color_manager) {
  setWindowTitle(tr("Select Color"));

  auto* layout = new QVBoxLayout(this);

  auto* splitter = new QSplitter(Qt::Horizontal);
  splitter->setChildrenCollapsible(false);
  layout->addWidget(splitter);

  auto* graphics_area = new QWidget();
  splitter->addWidget(graphics_area);

  auto* graphics_layout = new QVBoxLayout(graphics_area);

  auto* wheel_layout = new QHBoxLayout();
  graphics_layout->addLayout(wheel_layout);

  color_wheel_ = new ColorWheelWidget();
  wheel_layout->addWidget(color_wheel_);

  hsv_value_gradient_ = new ColorGradientWidget(Qt::Vertical);
  hsv_value_gradient_->setFixedWidth(QtUtils::QFontMetricsWidth(fontMetrics(), QStringLiteral("HHH")));
  wheel_layout->addWidget(hsv_value_gradient_);

  auto* swatch_layout = new QHBoxLayout();
  graphics_layout->addLayout(swatch_layout);

  swatch_layout->addStretch();

  swatch_ = new ColorSwatchChooser(color_manager_);
  swatch_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  swatch_layout->addWidget(swatch_);

  swatch_layout->addStretch();

  auto* value_area = new QWidget();
  auto* value_layout = new QVBoxLayout(value_area);
  value_layout->setSpacing(0);
  splitter->addWidget(value_area);

  color_values_widget_ = new ColorValuesWidget(color_manager_);
  color_values_widget_->IgnorePickFrom(this);
  value_layout->addWidget(color_values_widget_);

  chooser_ = new ColorSpaceChooser(color_manager_);

  value_layout->addWidget(chooser_);

  // Split window 50/50
  splitter->setSizes({INT_MAX, INT_MAX});

  connect(color_wheel_, &ColorWheelWidget::SelectedColorChanged, color_values_widget_, &ColorValuesWidget::SetColor);
  connect(color_wheel_, &ColorWheelWidget::SelectedColorChanged, hsv_value_gradient_,
          &ColorGradientWidget::SetSelectedColor);
  connect(color_wheel_, &ColorWheelWidget::SelectedColorChanged, swatch_, &ColorSwatchChooser::SetCurrentColor);
  connect(hsv_value_gradient_, &ColorGradientWidget::SelectedColorChanged, color_values_widget_,
          &ColorValuesWidget::SetColor);
  connect(hsv_value_gradient_, &ColorGradientWidget::SelectedColorChanged, color_wheel_,
          &ColorWheelWidget::SetSelectedColor);
  connect(hsv_value_gradient_, &ColorGradientWidget::SelectedColorChanged, swatch_,
          &ColorSwatchChooser::SetCurrentColor);
  connect(color_values_widget_, &ColorValuesWidget::ColorChanged, hsv_value_gradient_,
          &ColorGradientWidget::SetSelectedColor);
  connect(color_values_widget_, &ColorValuesWidget::ColorChanged, color_wheel_, &ColorWheelWidget::SetSelectedColor);
  connect(color_values_widget_, &ColorValuesWidget::ColorChanged, swatch_, &ColorSwatchChooser::SetCurrentColor);
  connect(swatch_, &ColorSwatchChooser::ColorClicked, hsv_value_gradient_, &ColorGradientWidget::SetSelectedColor);
  connect(swatch_, &ColorSwatchChooser::ColorClicked, color_wheel_, &ColorWheelWidget::SetSelectedColor);
  connect(swatch_, &ColorSwatchChooser::ColorClicked, color_values_widget_, &ColorValuesWidget::SetColor);

  connect(color_wheel_, &ColorWheelWidget::DiameterChanged, hsv_value_gradient_, &ColorGradientWidget::setFixedHeight);

  auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
  layout->addWidget(buttons);

  SetColor(start);

  connect(chooser_, &ColorSpaceChooser::ColorSpaceChanged, this, &ColorDialog::ColorSpaceChanged);
  ColorSpaceChanged(chooser_->input(), chooser_->output());

  // Set default size ratio to 2:1
  resize(sizeHint().height() * 2, sizeHint().height());
}

void ColorDialog::SetColor(const ManagedColor& start) {
  chooser_->set_input(start.color_input());
  chooser_->set_output(start.color_output());

  Color managed_start;

  if (start.color_input().isEmpty()) {
    managed_start = start;

  } else {
    // Convert reference color to the input space
    ColorProcessorPtr linear_to_input = ColorProcessor::Create(color_manager_, color_manager_->GetReferenceColorSpace(),
                                                               ColorTransform(start.color_input()));

    managed_start = linear_to_input->ConvertColor(start);
  }

  color_wheel_->SetSelectedColor(managed_start);
  hsv_value_gradient_->SetSelectedColor(managed_start);
  color_values_widget_->SetColor(managed_start);
  swatch_->SetCurrentColor(ManagedColor(managed_start));
}

ManagedColor ColorDialog::GetSelectedColor() const {
  ManagedColor selected = ManagedColor(color_wheel_->GetSelectedColor());

  // Convert to linear and return a linear color
  if (input_to_ref_processor_) {
    selected = ManagedColor(input_to_ref_processor_->ConvertColor(selected));
  }

  selected.set_color_input(GetColorSpaceInput());
  selected.set_color_output(GetColorSpaceOutput());

  return selected;
}

QString ColorDialog::GetColorSpaceInput() const { return chooser_->input(); }

ColorTransform ColorDialog::GetColorSpaceOutput() const { return chooser_->output(); }

void ColorDialog::ColorSpaceChanged(const QString& input, const ColorTransform& output) {
  input_to_ref_processor_ =
      ColorProcessor::Create(color_manager_, input, ColorTransform(color_manager_->GetReferenceColorSpace()));

  ColorProcessorPtr ref_to_display =
      ColorProcessor::Create(color_manager_, color_manager_->GetReferenceColorSpace(), output);

  ColorProcessorPtr ref_to_input =
      ColorProcessor::Create(color_manager_, color_manager_->GetReferenceColorSpace(), ColorTransform(input));

  // FIXME: For some reason, using OCIO::TRANSFORM_DIR_INVERSE (wrapped by ColorProcessor::kInverse) causes OCIO to
  //        crash. We've disabled that functionality for now (also disabling display_tab_ in ColorValuesWidget)

  /*ColorProcessorPtr display_to_ref = ColorProcessor::Create(color_manager_->GetConfig(),
                                                            color_manager_->GetReferenceColorSpace(),
                                                            display,
                                                            view,
                                                            look,
                                                            ColorProcessor::kInverse);*/

  color_wheel_->SetColorProcessor(input_to_ref_processor_, ref_to_display);
  hsv_value_gradient_->SetColorProcessor(input_to_ref_processor_, ref_to_display);
  color_values_widget_->SetColorProcessor(input_to_ref_processor_, ref_to_display, nullptr, ref_to_input);
}

}  // namespace olive
