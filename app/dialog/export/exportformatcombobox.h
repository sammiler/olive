

#ifndef EXPORTFORMATCOMBOBOX_H
#define EXPORTFORMATCOMBOBOX_H

#include <QComboBox>
#include <QWidgetAction>

#include "codec/exportformat.h"
#include "node/output/track/track.h"
#include "widget/menu/menu.h"

namespace olive {

class ExportFormatComboBox : public QComboBox {
  Q_OBJECT
 public:
  enum Mode { kShowAllFormats, kShowAudioOnly, kShowVideoOnly, kShowSubtitlesOnly };

  explicit ExportFormatComboBox(Mode mode, QWidget *parent = nullptr);
  explicit ExportFormatComboBox(QWidget *parent = nullptr) : ExportFormatComboBox(kShowAllFormats, parent) {}

  [[nodiscard]] ExportFormat::Format GetFormat() const { return current_; }

  void showPopup() override;

 signals:
  void FormatChanged(ExportFormat::Format fmt);

 public slots:
  void SetFormat(ExportFormat::Format fmt);

 private slots:
  void HandleIndexChange(QAction *a);

 private:
  void PopulateType(Track::Type type);

  QWidgetAction *CreateHeader(const QIcon &icon, const QString &title);

  Menu *custom_menu_;

  ExportFormat::Format current_;
};

}  // namespace olive

#endif  // EXPORTFORMATCOMBOBOX_H
