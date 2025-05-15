#ifndef SEQUENCEDIALOGPRESETTAB_H
#define SEQUENCEDIALOGPRESETTAB_H

#include <QLabel>
#include <QTreeWidget>
#include <QWidget>

#include "presetmanager.h"
#include "sequencepreset.h"

namespace olive {

class SequenceDialogPresetTab : public QWidget, public PresetManager<SequencePreset> {
  Q_OBJECT
 public:
  explicit SequenceDialogPresetTab(QWidget* parent = nullptr);

 public slots:
  void SaveParametersAsPreset(const SequencePreset& preset);

 signals:
  void PresetChanged(const SequencePreset& preset);

  void PresetAccepted();

 private:
  static QTreeWidgetItem* CreateFolder(const QString& name);

  QTreeWidgetItem* CreateHDPresetFolder(const QString& name, int width, int height, int divider);

  QTreeWidgetItem* CreateSDPresetFolder(const QString& name, int width, int height, const rational& frame_rate,
                                        const rational& standard_par, const rational& wide_par, int divider);

  QTreeWidgetItem* GetSelectedItem();
  QTreeWidgetItem* GetSelectedCustomPreset();

  void AddStandardItem(QTreeWidgetItem* folder, const PresetPtr& preset, const QString& description = QString());

  static void AddCustomItem(QTreeWidgetItem* folder, const PresetPtr& preset, int index,
                            const QString& description = QString());

  static void AddItemInternal(QTreeWidgetItem* folder, const PresetPtr& preset, bool is_custom, int index,
                              const QString& description = QString());

  QTreeWidget* preset_tree_;

  QTreeWidgetItem* my_presets_folder_;

  QVector<PresetPtr> default_preset_data_;

 private slots:
  void SelectedItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

  void ItemDoubleClicked(QTreeWidgetItem* item, int column);

  void ShowContextMenu();

  void DeleteSelectedPreset();
};

}  // namespace olive

#endif  // SEQUENCEDIALOGPRESETTAB_H
