#include "sequencedialogpresettab.h"

#include <QDir>
#include <QGroupBox>
#include <QInputDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QXmlStreamWriter>
#include <utility>

#include "common/filefunctions.h"
#include "config/config.h"
#include "render/videoparams.h"
#include "ui/icons/icons.h"
#include "widget/menu/menu.h"

namespace olive {

const int kDataIsPreset = Qt::UserRole;
const int kDataPresetIsCustomRole = Qt::UserRole + 1;
const int kDataPresetDataRole = Qt::UserRole + 2;

SequenceDialogPresetTab::SequenceDialogPresetTab(QWidget* parent)
    : QWidget(parent), PresetManager<SequencePreset>(this, QStringLiteral("sequencepresets")) {
  auto* outer_layout = new QVBoxLayout(this);
  outer_layout->setContentsMargins(0, 0, 0, 0);

  preset_tree_ = new QTreeWidget();
  preset_tree_->setColumnCount(1);
  preset_tree_->setHeaderLabel(tr("Preset"));
  preset_tree_->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(preset_tree_, &QTreeWidget::customContextMenuRequested, this, &SequenceDialogPresetTab::ShowContextMenu);
  outer_layout->addWidget(preset_tree_);
  connect(preset_tree_, &QTreeWidget::currentItemChanged, this, &SequenceDialogPresetTab::SelectedItemChanged);
  connect(preset_tree_, &QTreeWidget::itemDoubleClicked, this, &SequenceDialogPresetTab::ItemDoubleClicked);

  // Add "my presets" folder
  my_presets_folder_ = CreateFolder(tr("My Presets"));
  preset_tree_->addTopLevelItem(my_presets_folder_);

  // Add presets
  preset_tree_->addTopLevelItem(CreateHDPresetFolder(tr("4K UHD"), 3840, 2160, 2));
  preset_tree_->addTopLevelItem(CreateHDPresetFolder(tr("1080p"), 1920, 1080, 1));
  preset_tree_->addTopLevelItem(CreateHDPresetFolder(tr("720p"), 1280, 720, 1));

  preset_tree_->addTopLevelItem(CreateSDPresetFolder(tr("NTSC"), 720, 480, rational(30000, 1001),
                                                     VideoParams::kPixelAspectNTSCStandard,
                                                     VideoParams::kPixelAspectNTSCWidescreen, 1));
  preset_tree_->addTopLevelItem(CreateSDPresetFolder(tr("PAL"), 720, 576, rational(25, 1),
                                                     VideoParams::kPixelAspectPALStandard,
                                                     VideoParams::kPixelAspectPALWidescreen, 1));

  // Load custom presets
  for (int i = 0; i < GetNumberOfPresets(); i++) {
    AddCustomItem(my_presets_folder_, GetPreset(i), i);
  }
}

void SequenceDialogPresetTab::SaveParametersAsPreset(const SequencePreset& preset) {
  PresetPtr preset_ptr = std::make_shared<SequencePreset>(preset);

  // If replaced, no need to make another item. If not saved, shared ptr will delete itself
  if (SavePreset(preset_ptr) == kAppended) {
    AddCustomItem(my_presets_folder_, preset_ptr, GetNumberOfPresets() - 1);
  }
}

QTreeWidgetItem* SequenceDialogPresetTab::CreateFolder(const QString& name) {
  auto* folder = new QTreeWidgetItem();
  folder->setText(0, name);
  folder->setIcon(0, icon::Folder);
  return folder;
}

QTreeWidgetItem* SequenceDialogPresetTab::CreateHDPresetFolder(const QString& name, int width, int height,
                                                               int divider) {
  const PixelFormat default_format =
      PixelFormat(static_cast<PixelFormat::Format>(OLIVE_CONFIG("OfflinePixelFormat").toInt()));
  const bool default_autocache = false;
  QTreeWidgetItem* parent = CreateFolder(name);
  AddStandardItem(parent,
                  std::make_shared<SequencePreset>(tr("%1 23.976 FPS").arg(name), width, height, rational(24000, 1001),
                                                   VideoParams::kPixelAspectSquare, VideoParams::kInterlaceNone, 48000,
                                                   AV_CH_LAYOUT_STEREO, divider, default_format, default_autocache));
  AddStandardItem(parent,
                  std::make_shared<SequencePreset>(tr("%1 25 FPS").arg(name), width, height, rational(25, 1),
                                                   VideoParams::kPixelAspectSquare, VideoParams::kInterlaceNone, 48000,
                                                   AV_CH_LAYOUT_STEREO, divider, default_format, default_autocache));
  AddStandardItem(parent,
                  std::make_shared<SequencePreset>(tr("%1 29.97 FPS").arg(name), width, height, rational(30000, 1001),
                                                   VideoParams::kPixelAspectSquare, VideoParams::kInterlaceNone, 48000,
                                                   AV_CH_LAYOUT_STEREO, divider, default_format, default_autocache));
  AddStandardItem(parent,
                  std::make_shared<SequencePreset>(tr("%1 50 FPS").arg(name), width, height, rational(50, 1),
                                                   VideoParams::kPixelAspectSquare, VideoParams::kInterlaceNone, 48000,
                                                   AV_CH_LAYOUT_STEREO, divider, default_format, default_autocache));
  AddStandardItem(parent,
                  std::make_shared<SequencePreset>(tr("%1 59.94 FPS").arg(name), width, height, rational(60000, 1001),
                                                   VideoParams::kPixelAspectSquare, VideoParams::kInterlaceNone, 48000,
                                                   AV_CH_LAYOUT_STEREO, divider, default_format, default_autocache));
  return parent;
}

QTreeWidgetItem* SequenceDialogPresetTab::CreateSDPresetFolder(const QString& name, int width, int height,
                                                               const rational& frame_rate, const rational& standard_par,
                                                               const rational& wide_par, int divider) {
  const PixelFormat default_format =
      PixelFormat(static_cast<PixelFormat::Format>(OLIVE_CONFIG("OfflinePixelFormat").toInt()));
  const bool default_autocache = false;
  QTreeWidgetItem* parent = CreateFolder(name);
  preset_tree_->addTopLevelItem(parent);
  AddStandardItem(
      parent, std::make_shared<SequencePreset>(tr("%1 Standard").arg(name), width, height, frame_rate, standard_par,
                                               VideoParams::kInterlacedBottomFirst, 48000, AV_CH_LAYOUT_STEREO, divider,
                                               default_format, default_autocache));
  AddStandardItem(
      parent, std::make_shared<SequencePreset>(tr("%1 Widescreen").arg(name), width, height, frame_rate, wide_par,
                                               VideoParams::kInterlacedBottomFirst, 48000, AV_CH_LAYOUT_STEREO, divider,
                                               default_format, default_autocache));
  return parent;
}

QTreeWidgetItem* SequenceDialogPresetTab::GetSelectedItem() {
  QList<QTreeWidgetItem*> selected_items = preset_tree_->selectedItems();

  if (selected_items.isEmpty()) {
    return nullptr;
  } else {
    return selected_items.first();
  }
}

QTreeWidgetItem* SequenceDialogPresetTab::GetSelectedCustomPreset() {
  QTreeWidgetItem* sel = GetSelectedItem();

  if (sel && sel->data(0, kDataIsPreset).toBool() && sel->data(0, kDataPresetIsCustomRole).toBool()) {
    return sel;
  }

  return nullptr;
}

void SequenceDialogPresetTab::AddStandardItem(QTreeWidgetItem* folder, const PresetPtr& preset,
                                              const QString& description) {
  int index = default_preset_data_.size();
  default_preset_data_.append(preset);
  AddItemInternal(folder, preset, false, index, description);
}

void SequenceDialogPresetTab::AddCustomItem(QTreeWidgetItem* folder, const PresetPtr& preset, int index,
                                            const QString& description) {
  AddItemInternal(folder, preset, true, index, description);
}

void SequenceDialogPresetTab::AddItemInternal(QTreeWidgetItem* folder, const PresetPtr& preset, bool is_custom,
                                              int index, const QString& description) {
  auto* item = new QTreeWidgetItem();

  item->setText(0, preset->GetName());
  item->setIcon(0, icon::Video);
  item->setToolTip(0, description);
  item->setData(0, kDataIsPreset, true);
  item->setData(0, kDataPresetIsCustomRole, is_custom);
  item->setData(0, kDataPresetDataRole, index);

  folder->addChild(item);
}

void SequenceDialogPresetTab::SelectedItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous) {
  Q_UNUSED(previous)

  if (current->data(0, kDataIsPreset).toBool()) {
    int preset_index = current->data(0, kDataPresetDataRole).toInt();

    PresetPtr preset_data = (current->data(0, kDataPresetIsCustomRole).toBool())
                                ? GetPreset(preset_index)
                                : default_preset_data_.at(preset_index);

    emit PresetChanged(*dynamic_cast<SequencePreset*>(preset_data.get()));
  }
}

void SequenceDialogPresetTab::ItemDoubleClicked(QTreeWidgetItem* item, int column) {
  Q_UNUSED(column)

  if (item->data(0, kDataIsPreset).toBool()) {
    emit PresetAccepted();
  }
}

void SequenceDialogPresetTab::ShowContextMenu() {
  QTreeWidgetItem* sel = GetSelectedCustomPreset();

  if (sel) {
    Menu m(this);

    QAction* delete_action = m.addAction(tr("Delete Preset"));
    connect(delete_action, &QAction::triggered, this, &SequenceDialogPresetTab::DeleteSelectedPreset);

    m.exec(QCursor::pos());
  }
}

void SequenceDialogPresetTab::DeleteSelectedPreset() {
  QTreeWidgetItem* sel = GetSelectedCustomPreset();

  if (sel) {
    int preset_index = sel->data(0, kDataPresetDataRole).toInt();

    // Shift all items whose index was after this preset forward
    for (int i = 0; i < my_presets_folder_->childCount(); i++) {
      QTreeWidgetItem* custom_item = my_presets_folder_->child(i);
      int this_item_index = custom_item->data(0, kDataPresetDataRole).toInt();

      if (this_item_index > preset_index) {
        custom_item->setData(0, kDataPresetDataRole, this_item_index - 1);
      }
    }

    // Remove the preset
    DeletePreset(preset_index);

    // Delete the item
    delete sel;
  }
}

}  // namespace olive
