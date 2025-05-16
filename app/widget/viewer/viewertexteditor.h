#ifndef VIEWERTEXTEDITOR_H
#define VIEWERTEXTEDITOR_H

#include <QApplication>    // 包含 QApplication 类的头文件
#include <QDebug>          // 包含 QDebug 类的头文件，用于调试输出
#include <QFontComboBox>   // 包含 QFontComboBox 类的头文件，用于字体选择
#include <QPushButton>     // 包含 QPushButton 类的头文件
#include <QTextEdit>       // 包含 QTextEdit 类的头文件，富文本编辑器

#include "widget/slider/floatslider.h"   // 包含浮点数滑块控件
#include "widget/slider/integerslider.h" // 包含整数滑块控件

namespace olive {

/**
 * @brief 查看器文本编辑器工具栏类。
 * 提供用于格式化文本的各种控件，如字体选择、样式、大小、对齐方式等。
 */
class ViewerTextEditorToolBar : public QWidget {
  Q_OBJECT // Q_OBJECT宏，用于支持 Qt 的元对象系统（信号、槽等）
 public:
  /**
   * @brief ViewerTextEditorToolBar 的显式构造函数。
   * @param parent 父 QWidget 指针，默认为 nullptr。
   */
  explicit ViewerTextEditorToolBar(QWidget *parent = nullptr);

  /**
   * @brief 获取当前选择的字体家族名称。
   * @return 返回字体家族名称的 QString。
   */
  [[nodiscard]] QString GetFontFamily() const { return font_combo_->currentText(); }

  /**
   * @brief 获取当前选择的字体样式名称。
   * @return 返回字体样式名称的 QString。
   */
  [[nodiscard]] QString GetFontStyleName() const { return style_combo_->currentText(); }

 public slots:
  /**
   * @brief 设置字体家族。
   * 同时会更新字体样式列表。
   * @param s 要设置的字体家族名称。
   */
  void SetFontFamily(const QString &s) {
    font_combo_->blockSignals(true); // 暂时阻塞信号，避免不必要的信号发射
    font_combo_->setCurrentFont(s);  // 设置当前字体
    UpdateFontStyleList(s);          // 更新字体样式列表
    font_combo_->blockSignals(false); // 解除信号阻塞
  }

  /**
   * @brief 设置字体样式。
   * @param style 要设置的字体样式名称。
   */
  void SetStyle(const QString &style) {
    style_combo_->blockSignals(true);    // 暂时阻塞信号
    style_combo_->setCurrentText(style); // 设置当前文本（样式）
    style_combo_->blockSignals(false);   // 解除信号阻塞
  }

  /**
   * @brief 设置字体大小。
   * @param d 字体大小值。
   */
  void SetFontSize(double d) { font_sz_slider_->SetValue(d); }
  /**
   * @brief 设置是否启用下划线。
   * @param e 如果为 true，则启用下划线；否则禁用。
   */
  void SetUnderline(bool e) { underline_btn_->setChecked(e); }
  /**
   * @brief 设置是否启用删除线。
   * @param e 如果为 true，则启用删除线；否则禁用。
   */
  void SetStrikethrough(bool e) { strikethrough_btn_->setChecked(e); }
  /**
   * @brief 设置文本对齐方式。
   * @param a Qt 对齐方式枚举值。
   */
  void SetAlignment(Qt::Alignment a);
  /**
   * @brief 设置文本垂直对齐方式。
   * @param a Qt 对齐方式枚举值。
   */
  void SetVerticalAlignment(Qt::Alignment a);
  /**
   * @brief 设置文本颜色。
   * @param c 要设置的颜色。
   */
  void SetColor(const QColor &c);
  /**
   * @brief 设置是否启用小型大写字母。
   * @param e 如果为 true，则启用小型大写字母；否则禁用。
   */
  void SetSmallCaps(bool e) { small_caps_btn_->setChecked(e); }
  /**
   * @brief 设置字体拉伸。
   * @param i 字体拉伸值。
   */
  void SetStretch(int i) { stretch_slider_->SetValue(i); }
  /**
   * @brief 设置字间距调整。
   * @param i 字间距调整值。
   */
  void SetKerning(qreal i) { kerning_slider_->SetValue(i); }
  /**
   * @brief 设置行高。
   * @param i 行高值。
   */
  void SetLineHeight(qreal i) { line_height_slider_->SetValue(i); }

 signals:
  /**
   * @brief 当字体家族更改时发出的信号。
   * @param s 新的字体家族名称。
   */
  void FamilyChanged(const QString &s);
  /**
   * @brief 当字体大小更改时发出的信号。
   * @param d 新的字体大小。
   */
  void SizeChanged(double d);
  /**
   * @brief 当字体样式更改时发出的信号。
   * @param s 新的字体样式名称。
   */
  void StyleChanged(const QString &s);
  /**
   * @brief 当下划线状态更改时发出的信号。
   * @param e 如果启用下划线，则为 true；否则为 false。
   */
  void UnderlineChanged(bool e);
  /**
   * @brief 当删除线状态更改时发出的信号。
   * @param e 如果启用删除线，则为 true；否则为 false。
   */
  void StrikethroughChanged(bool e);
  /**
   * @brief 当对齐方式更改时发出的信号。
   * @param alignment 新的对齐方式。
   */
  void AlignmentChanged(Qt::Alignment alignment);
  /**
   * @brief 当垂直对齐方式更改时发出的信号。
   * @param alignment 新的垂直对齐方式。
   */
  void VerticalAlignmentChanged(Qt::Alignment alignment);
  /**
   * @brief 当颜色更改时发出的信号。
   * @param c 新的颜色。
   */
  void ColorChanged(const QColor &c);
  /**
   * @brief 当小型大写字母状态更改时发出的信号。
   * @param e 如果启用小型大写字母，则为 true；否则为 false。
   */
  void SmallCapsChanged(bool e);
  /**
   * @brief 当字体拉伸更改时发出的信号。
   * @param i 新的字体拉伸值。
   */
  void StretchChanged(int i);
  /**
   * @brief 当字间距调整更改时发出的信号。
   * @param i 新的字间距调整值。
   */
  void KerningChanged(qreal i);
  /**
   * @brief 当行高更改时发出的信号。
   * @param i 新的行高值。
   */
  void LineHeightChanged(qreal i);

  /**
   * @brief 首次绘制完成时发出的信号。
   */
  void FirstPaint();

 protected:
  /**
   * @brief 处理鼠标按下事件。
   * @param event 鼠标事件。
   */
  void mousePressEvent(QMouseEvent *event) override;

  /**
   * @brief 处理鼠标移动事件。
   * @param event 鼠标事件。
   */
  void mouseMoveEvent(QMouseEvent *event) override;

  /**
   * @brief 处理鼠标释放事件。
   * @param event 鼠标事件。
   */
  void mouseReleaseEvent(QMouseEvent *event) override;

  /**
   * @brief 处理关闭事件。
   * @param event 关闭事件。
   */
  void closeEvent(QCloseEvent *event) override;

  /**
   * @brief 处理绘制事件。
   * @param event 绘制事件。
   */
  void paintEvent(QPaintEvent *event) override;

 private:
  /**
   * @brief 向布局中添加一个间隔项。
   * @param l 要添加间隔项的布局。
   */
  void AddSpacer(QLayout *l);

  /**
   * @brief 拖动锚点，用于窗口拖动。
   */
  QPoint drag_anchor_;

  /**
   * @brief 字体选择组合框。
   */
  QFontComboBox *font_combo_;

  /**
   * @brief 字体大小滑块。
   */
  FloatSlider *font_sz_slider_;

  /**
   * @brief 字体样式选择组合框。
   */
  QComboBox *style_combo_;

  /**
   * @brief 下划线按钮。
   */
  QPushButton *underline_btn_;
  /**
   * @brief 删除线按钮。
   */
  QPushButton *strikethrough_btn_;

  /**
   * @brief 左对齐按钮。
   */
  QPushButton *align_left_btn_;
  /**
   * @brief 居中对齐按钮。
   */
  QPushButton *align_center_btn_;
  /**
   * @brief 右对齐按钮。
   */
  QPushButton *align_right_btn_;
  /**
   * @brief 两端对齐按钮。
   */
  QPushButton *align_justify_btn_;

  /**
   * @brief 顶部对齐按钮。
   */
  QPushButton *align_top_btn_;
  /**
   * @brief 垂直居中对齐按钮。
   */
  QPushButton *align_middle_btn_;
  /**
   * @brief 底部对齐按钮。
   */
  QPushButton *align_bottom_btn_;

  /**
   * @brief 字体拉伸滑块。
   */
  IntegerSlider *stretch_slider_;
  /**
   * @brief 字间距调整滑块。
   */
  FloatSlider *kerning_slider_;
  /**
   * @brief 行高滑块。
   */
  FloatSlider *line_height_slider_;
  /**
   * @brief 小型大写字母按钮。
   */
  QPushButton *small_caps_btn_;

  /**
   * @brief 颜色选择按钮。
   */
  QPushButton *color_btn_;

  /**
   * @brief 标记是否已进行首次绘制。
   */
  bool painted_;

  /**
   * @brief 标记是否启用了拖动。
   */
  bool drag_enabled_;

 private slots:
  /**
   * @brief 根据字体家族更新字体样式列表。
   * @param family 字体家族名称。
   */
  void UpdateFontStyleList(const QString &family);

  /**
   * @brief 更新字体样式列表并发出 FamilyChanged 信号。
   * @param family 字体家族名称。
   */
  void UpdateFontStyleListAndEmitFamilyChanged(const QString &family);
};

/**
 * @brief 查看器文本编辑器类。
 * 继承自 QTextEdit，提供富文本编辑功能，并与 ViewerTextEditorToolBar 联动。
 */
class ViewerTextEditor : public QTextEdit {
  Q_OBJECT // Q_OBJECT宏
 public:
  /**
   * @brief ViewerTextEditor 的显式构造函数。
   * @param scale 缩放比例。
   * @param parent 父 QWidget 指针，默认为 nullptr。
   */
  explicit ViewerTextEditor(double scale, QWidget *parent = nullptr);

  /**
   * @brief 连接工具栏。
   * 将此文本编辑器与一个 ViewerTextEditorToolBar 实例关联起来。
   * @param toolbar 要连接的 ViewerTextEditorToolBar 指针。
   */
  void ConnectToolBar(ViewerTextEditorToolBar *toolbar);

  /**
   * @brief 执行绘制操作。
   * @param p QPainter 指针，用于绘制。
   * @param valign 垂直对齐方式。
   */
  void Paint(QPainter *p, Qt::Alignment valign);

  /**
   * @brief 处理拖动进入事件。
   * @param e 拖动进入事件。
   */
  void dragEnterEvent(QDragEnterEvent *e) override { return QTextEdit::dragEnterEvent(e); }
  /**
   * @brief 处理拖动移动事件。
   * @param e 拖动移动事件。
   */
  void dragMoveEvent(QDragMoveEvent *e) override { return QTextEdit::dragMoveEvent(e); }
  /**
   * @brief 处理拖动离开事件。
   * @param e 拖动离开事件。
   */
  void dragLeaveEvent(QDragLeaveEvent *e) override { return QTextEdit::dragLeaveEvent(e); }
  /**
   * @brief 处理拖放事件。
   * @param e 拖放事件。
   */
  void dropEvent(QDropEvent *e) override { return QTextEdit::dropEvent(e); }

 protected:
  /**
   * @brief 处理绘制事件。
   * @param event 绘制事件。
   */
  void paintEvent(QPaintEvent *event) override;

 private:
  /**
   * @brief 更新工具栏状态以匹配当前文本格式。
   * @param toolbar 要更新的 ViewerTextEditorToolBar 指针。
   * @param f 当前字符格式。
   * @param b 当前文本块格式。
   * @param alignment 当前对齐方式。
   */
  static void UpdateToolBar(ViewerTextEditorToolBar *toolbar, const QTextCharFormat &f, const QTextBlockFormat &b,
                            Qt::Alignment alignment);

  /**
   * @brief 合并字符格式到当前选区。
   * @param fmt 要合并的 QTextCharFormat。
   */
  void MergeCharFormat(const QTextCharFormat &fmt);

  /**
   * @brief 应用字体家族和样式到字符格式。
   * @param format 要修改的 QTextCharFormat 指针。
   * @param family 字体家族名称。
   * @param style 字体样式名称。
   */
  static void ApplyStyle(QTextCharFormat *format, const QString &family, const QString &style);

  /**
   * @brief 连接的工具栏列表。
   * 一个文本编辑器可以连接多个工具栏。
   */
  QVector<ViewerTextEditorToolBar *> toolbars_;

  /**
   * @brief 用于强制 DPI 的图像。
   * 可能用于解决高 DPI 环境下的绘制问题。
   */
  QImage dpi_force_;

  /**
   * @brief 文档的透明克隆。
   * 可能用于某些特殊的绘制或格式化操作。
   */
  QTextDocument *transparent_clone_;

  /**
   * @brief 标志，用于阻止更新工具栏的信号。
   * 在程序化更改格式时，避免触发不必要的工具栏更新。
   */
  bool block_update_toolbar_signal_;

  /**
   * @brief 标志，指示是否强制使用默认格式。
   */
  bool forced_default_;
  /**
   * @brief 默认的字符格式。
   */
  QTextCharFormat default_fmt_;

 private slots:
  /**
   * @brief 当光标处的文本格式更改时调用的槽函数。
   * @param f 新的字符格式。
   */
  void FormatChanged(const QTextCharFormat &f);

  /**
   * @brief 设置字体家族的槽函数。
   * @param s 字体家族名称。
   */
  void SetFamily(const QString &s);

  /**
   * @brief 设置字体样式的槽函数。
   * @param s 字体样式名称。
   */
  void SetStyle(const QString &s);

  /**
   * @brief 设置字体删除线的槽函数。
   * @param e 如果为 true，则启用删除线；否则禁用。
   */
  void SetFontStrikethrough(bool e);

  /**
   * @brief 设置小型大写字母的槽函数。
   * @param e 如果为 true，则启用小型大写字母；否则禁用。
   */
  void SetSmallCaps(bool e);

  /**
   * @brief 设置字体拉伸的槽函数。
   * @param i 字体拉伸值。
   */
  void SetFontStretch(int i);

  /**
   * @brief 设置字间距调整的槽函数。
   * @param i 字间距调整值。
   */
  void SetFontKerning(qreal i);

  /**
   * @brief 设置行高的槽函数。
   * @param i 行高值。
   */
  void SetLineHeight(qreal i);

  /**
   * @brief 锁定滚动条最大值为零的槽函数。
   * 可能用于特定情况下阻止滚动。
   */
  void LockScrollBarMaximumToZero();

  /**
   * @brief 当文档内容更改时调用的槽函数。
   */
  void DocumentChanged();
};

}  // namespace olive

#endif  // VIEWERTEXTEDITOR_H
