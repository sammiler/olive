#ifndef PATHWIDGET_H
#define PATHWIDGET_H

#include <QLineEdit>
#include <QPushButton>

#include "common/define.h"

namespace olive {

class PathWidget : public QWidget {
  Q_OBJECT
 public:
  explicit PathWidget(const QString& path, QWidget* parent = nullptr);

  [[nodiscard]] QString text() const { return path_edit_->text(); }

 private slots:
  void BrowseClicked();

  void LineEditChanged();

 private:
  QLineEdit* path_edit_;

  QPushButton* browse_btn_;
};

}  // namespace olive

#endif  // PATHWIDGET_H
