

#ifndef FILEFIELD_H
#define FILEFIELD_H

#include <QLineEdit>
#include <QPushButton>

namespace olive {

class FileField : public QWidget {
  Q_OBJECT
 public:
  explicit FileField(QWidget* parent = nullptr);

  [[nodiscard]] QString GetFilename() const { return line_edit_->text(); }

  void SetFilename(const QString& s) { line_edit_->setText(s); }

  void SetPlaceholder(const QString& s) { line_edit_->setPlaceholderText(s); }

  void SetDirectoryMode(bool e) { directory_mode_ = e; }

 signals:
  void FilenameChanged(const QString& filename);

 private:
  QLineEdit* line_edit_;

  QPushButton* browse_btn_;

  bool directory_mode_;

 private slots:
  void BrowseBtnClicked();

  void LineEditChanged(const QString& text);
};

}  // namespace olive

#endif  // FILEFIELD_H
