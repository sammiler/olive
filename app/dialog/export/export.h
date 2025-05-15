

#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QProgressBar>

#include "codec/exportcodec.h"
#include "codec/exportformat.h"
#include "dialog/export/exportformatcombobox.h"
#include "exportaudiotab.h"
#include "exportsubtitlestab.h"
#include "exportvideotab.h"
#include "task/export/export.h"
#include "widget/nodeparamview/nodeparamviewwidgetbridge.h"
#include "widget/viewer/viewer.h"

namespace olive {

class ExportDialog : public QDialog {
  Q_OBJECT
 public:
  ExportDialog(ViewerOutput* viewer_node, bool stills_only_mode, QWidget* parent = nullptr);
  explicit ExportDialog(ViewerOutput* viewer_node, QWidget* parent = nullptr)
      : ExportDialog(viewer_node, false, parent) {}

  [[nodiscard]] rational GetSelectedTimebase() const;
  void SetSelectedTimebase(const rational& r);

  [[nodiscard]] EncodingParams GenerateParams() const;
  void SetParams(const EncodingParams& e);

  bool eventFilter(QObject* o, QEvent* e) override;

 public slots:
  void done(int r) override;

 signals:
  void RequestImportFile(const QString& s);

 private:
  void AddPreferencesTab(QWidget* inner_widget, const QString& title);

  void LoadPresets();
  void SetDefaultFilename();

  [[nodiscard]] bool SequenceHasSubtitles() const;

  void SetDefaults();

  ViewerOutput* viewer_node_;

  ExportFormat::Format previously_selected_format_;

  [[nodiscard]] rational GetExportLength() const;
  [[nodiscard]] int64_t GetExportLengthInTimebaseUnits() const;

  enum RangeSelection { kRangeEntireSequence, kRangeInToOut };

  enum AutoPreset {
    kPresetDefault = -1,
    kPresetLastUsed = -2,
  };

  QTabWidget* preferences_tabs_;

  QComboBox* preset_combobox_;
  QComboBox* range_combobox_;
  std::vector<EncodingParams> presets_;

  QCheckBox* video_enabled_;
  QCheckBox* audio_enabled_;
  QCheckBox* subtitles_enabled_;

  ViewerWidget* preview_viewer_;
  QLineEdit* filename_edit_;
  ExportFormatComboBox* format_combobox_;

  ExportVideoTab* video_tab_;
  ExportAudioTab* audio_tab_;
  ExportSubtitlesTab* subtitle_tab_;

  double video_aspect_ratio_;

  ColorManager* color_manager_;

  QWidget* preferences_area_;
  QCheckBox* export_bkg_box_;
  QCheckBox* import_file_after_export_;

  bool stills_only_mode_;

  bool loading_presets_;

 private slots:
  void BrowseFilename();

  void FormatChanged(ExportFormat::Format current_format);

  void ResolutionChanged();

  void UpdateViewerDimensions();

  void StartExport();

  void ExportFinished();

  void ImageSequenceCheckBoxChanged(bool e);

  void SavePreset();

  void PresetComboBoxChanged();
};

}  // namespace olive

#endif  // EXPORTDIALOG_H
