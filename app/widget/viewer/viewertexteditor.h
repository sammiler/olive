#ifndef VIEWERTEXTEDITOR_H
#define VIEWERTEXTEDITOR_H

#include <QApplication>
#include <QDebug>
#include <QFontComboBox>
#include <QPushButton>
#include <QTextEdit>

#include "widget/slider/floatslider.h"
#include "widget/slider/integerslider.h"

namespace olive {

class ViewerTextEditorToolBar : public QWidget {
  Q_OBJECT
 public:
  explicit ViewerTextEditorToolBar(QWidget *parent = nullptr);

  [[nodiscard]] QString GetFontFamily() const { return font_combo_->currentText(); }

  [[nodiscard]] QString GetFontStyleName() const { return style_combo_->currentText(); }

 public slots:
  void SetFontFamily(const QString &s) {
    font_combo_->blockSignals(true);
    font_combo_->setCurrentFont(s);
    UpdateFontStyleList(s);
    font_combo_->blockSignals(false);
  }

  void SetStyle(const QString &style) {
    style_combo_->blockSignals(true);
    style_combo_->setCurrentText(style);
    style_combo_->blockSignals(false);
  }

  void SetFontSize(double d) { font_sz_slider_->SetValue(d); }
  void SetUnderline(bool e) { underline_btn_->setChecked(e); }
  void SetStrikethrough(bool e) { strikethrough_btn_->setChecked(e); }
  void SetAlignment(Qt::Alignment a);
  void SetVerticalAlignment(Qt::Alignment a);
  void SetColor(const QColor &c);
  void SetSmallCaps(bool e) { small_caps_btn_->setChecked(e); }
  void SetStretch(int i) { stretch_slider_->SetValue(i); }
  void SetKerning(qreal i) { kerning_slider_->SetValue(i); }
  void SetLineHeight(qreal i) { line_height_slider_->SetValue(i); }

 signals:
  void FamilyChanged(const QString &s);
  void SizeChanged(double d);
  void StyleChanged(const QString &s);
  void UnderlineChanged(bool e);
  void StrikethroughChanged(bool e);
  void AlignmentChanged(Qt::Alignment alignment);
  void VerticalAlignmentChanged(Qt::Alignment alignment);
  void ColorChanged(const QColor &c);
  void SmallCapsChanged(bool e);
  void StretchChanged(int i);
  void KerningChanged(qreal i);
  void LineHeightChanged(qreal i);

  void FirstPaint();

 protected:
  void mousePressEvent(QMouseEvent *event) override;

  void mouseMoveEvent(QMouseEvent *event) override;

  void mouseReleaseEvent(QMouseEvent *event) override;

  void closeEvent(QCloseEvent *event) override;

  void paintEvent(QPaintEvent *event) override;

 private:
  void AddSpacer(QLayout *l);

  QPoint drag_anchor_;

  QFontComboBox *font_combo_;

  FloatSlider *font_sz_slider_;

  QComboBox *style_combo_;

  QPushButton *underline_btn_;
  QPushButton *strikethrough_btn_;

  QPushButton *align_left_btn_;
  QPushButton *align_center_btn_;
  QPushButton *align_right_btn_;
  QPushButton *align_justify_btn_;

  QPushButton *align_top_btn_;
  QPushButton *align_middle_btn_;
  QPushButton *align_bottom_btn_;

  IntegerSlider *stretch_slider_;
  FloatSlider *kerning_slider_;
  FloatSlider *line_height_slider_;
  QPushButton *small_caps_btn_;

  QPushButton *color_btn_;

  bool painted_;

  bool drag_enabled_;

 private slots:
  void UpdateFontStyleList(const QString &family);

  void UpdateFontStyleListAndEmitFamilyChanged(const QString &family);
};

class ViewerTextEditor : public QTextEdit {
  Q_OBJECT
 public:
  explicit ViewerTextEditor(double scale, QWidget *parent = nullptr);

  void ConnectToolBar(ViewerTextEditorToolBar *toolbar);

  void Paint(QPainter *p, Qt::Alignment valign);

  void dragEnterEvent(QDragEnterEvent *e) override { return QTextEdit::dragEnterEvent(e); }
  void dragMoveEvent(QDragMoveEvent *e) override { return QTextEdit::dragMoveEvent(e); }
  void dragLeaveEvent(QDragLeaveEvent *e) override { return QTextEdit::dragLeaveEvent(e); }
  void dropEvent(QDropEvent *e) override { return QTextEdit::dropEvent(e); }

 protected:
  void paintEvent(QPaintEvent *event) override;

 private:
  static void UpdateToolBar(ViewerTextEditorToolBar *toolbar, const QTextCharFormat &f, const QTextBlockFormat &b,
                            Qt::Alignment alignment);

  void MergeCharFormat(const QTextCharFormat &fmt);

  static void ApplyStyle(QTextCharFormat *format, const QString &family, const QString &style);

  QVector<ViewerTextEditorToolBar *> toolbars_;

  QImage dpi_force_;

  QTextDocument *transparent_clone_;

  bool block_update_toolbar_signal_;

  bool forced_default_;
  QTextCharFormat default_fmt_;

 private slots:
  void FormatChanged(const QTextCharFormat &f);

  void SetFamily(const QString &s);

  void SetStyle(const QString &s);

  void SetFontStrikethrough(bool e);

  void SetSmallCaps(bool e);

  void SetFontStretch(int i);

  void SetFontKerning(qreal i);

  void SetLineHeight(qreal i);

  void LockScrollBarMaximumToZero();

  void DocumentChanged();
};

}  // namespace olive

#endif  // VIEWERTEXTEDITOR_H
