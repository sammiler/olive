

#ifndef NODEPARAMVIEWITEMTITLEBAR_H
#define NODEPARAMVIEWITEMTITLEBAR_H

#include <QCheckBox>
#include <QLabel>
#include <QWidget>

#include "widget/collapsebutton/collapsebutton.h"

namespace olive {

class NodeParamViewItemTitleBar : public QWidget {
  Q_OBJECT
 public:
  explicit NodeParamViewItemTitleBar(QWidget *parent = nullptr);

  [[nodiscard]] bool IsExpanded() const { return collapse_btn_->isChecked(); }

 public slots:
  void SetExpanded(bool e);

  void SetText(const QString &s) {
    lbl_->setText(s);
    lbl_->setToolTip(s);
    lbl_->setMinimumWidth(1);
  }

  void SetPinButtonVisible(bool e) { pin_btn_->setVisible(e); }

  void SetAddEffectButtonVisible(bool e) { add_fx_btn_->setVisible(e); }

  void SetEnabledCheckBoxVisible(bool e) { enabled_checkbox_->setVisible(e); }

  void SetEnabledCheckBoxChecked(bool e) { enabled_checkbox_->setChecked(e); }

 signals:
  void ExpandedStateChanged(bool e);

  void PinToggled(bool e);

  void AddEffectButtonClicked();

  void EnabledCheckBoxClicked(bool e);

  void Clicked();

 protected:
  void paintEvent(QPaintEvent *event) override;

  void mousePressEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;

 private:
  bool draw_border_;

  QLabel *lbl_;

  CollapseButton *collapse_btn_;

  QPushButton *pin_btn_;

  QPushButton *add_fx_btn_;

  QCheckBox *enabled_checkbox_;
};

}  // namespace olive

#endif  // NODEPARAMVIEWITEMTITLEBAR_H
