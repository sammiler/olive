

#include "preferencesappearancetab.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>

#include "node/node.h"
#include "ui/colorcoding.h"
#include "widget/colorbutton/colorbutton.h"
#include "widget/menu/menushared.h"

namespace olive {

PreferencesAppearanceTab::PreferencesAppearanceTab() {
  auto* layout = new QVBoxLayout(this);

  auto* appearance_layout = new QGridLayout();
  layout->addLayout(appearance_layout);

  int row = 0;

  // Appearance -> Theme
  appearance_layout->addWidget(new QLabel(tr("Theme")), row, 0);

  style_combobox_ = new QComboBox();

  {
    const QMap<QString, QString>& themes = StyleManager::available_themes();
    QMap<QString, QString>::const_iterator i;
    for (i = themes.cbegin(); i != themes.cend(); i++) {
      style_combobox_->addItem(i.value(), i.key());

      if (StyleManager::GetStyle() == i.key()) {
        style_combobox_->setCurrentIndex(style_combobox_->count() - 1);
      }
    }
  }

  appearance_layout->addWidget(style_combobox_, row, 1);

  row++;

  {
    auto* color_group = new QGroupBox();
    color_group->setTitle(tr("Default Node Colors"));

    auto* color_layout = new QGridLayout(color_group);

    for (int i = 0; i < Node::kCategoryCount; i++) {
      QString cat_name = Node::GetCategoryName(static_cast<Node::CategoryID>(i));
      color_layout->addWidget(new QLabel(cat_name), i, 0);

      auto* ccc = new ColorCodingComboBox();
      ccc->SetColor(OLIVE_CONFIG_STR(QStringLiteral("CatColor%1").arg(i)).toInt());
      color_layout->addWidget(ccc, i, 1);
      color_btns_.append(ccc);
    }

    appearance_layout->addWidget(color_group, row, 0, 1, 2);
  }

  row++;
  {
    auto* marker_group = new QGroupBox();
    marker_group->setTitle(tr("Miscellaneous"));

    auto* marker_layout = new QGridLayout(marker_group);

    marker_layout->addWidget(new QLabel("Default Marker Color"), 0, 0);

    marker_btn_ = new ColorCodingComboBox();
    marker_btn_->SetColor(OLIVE_CONFIG("MarkerColor").toInt());
    marker_layout->addWidget(marker_btn_, 0, 1);

    appearance_layout->addWidget(marker_group, row, 0, 1, 2);
  }

  layout->addStretch();
}

void PreferencesAppearanceTab::Accept(MultiUndoCommand* command) {
  Q_UNUSED(command)

  QString style_path = style_combobox_->currentData().toString();

  if (style_path != StyleManager::GetStyle()) {
    StyleManager::SetStyle(style_path);
    OLIVE_CONFIG("Style") = style_path;
  }

  for (int i = 0; i < color_btns_.size(); i++) {
    OLIVE_CONFIG_STR(QStringLiteral("CatColor%1").arg(i)) = color_btns_.at(i)->GetSelectedColor();
  }

  OLIVE_CONFIG("MarkerColor") = marker_btn_->GetSelectedColor();
}

}  // namespace olive
