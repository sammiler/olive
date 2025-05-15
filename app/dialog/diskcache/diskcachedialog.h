#ifndef DISKCACHEDIALOG_H
#define DISKCACHEDIALOG_H

#include <QCheckBox>
#include <QDialog>
#include <QPushButton>

#include "render/diskmanager.h"
#include "widget/slider/floatslider.h"

namespace olive {

class DiskCacheDialog : public QDialog {
  Q_OBJECT
 public:
  explicit DiskCacheDialog(DiskCacheFolder* folder, QWidget* parent = nullptr);

  static void ClearDiskCache(const QString& path, QWidget* parent, QPushButton* clear_btn = nullptr);

 public slots:
  void accept() override;

 private:
  DiskCacheFolder* folder_;

  FloatSlider* maximum_cache_slider_;

  QCheckBox* clear_disk_cache_;

  QPushButton* clear_cache_btn_;

 private slots:
  void ClearDiskCache();
};

}  // namespace olive

#endif  // DISKCACHEDIALOG_H
