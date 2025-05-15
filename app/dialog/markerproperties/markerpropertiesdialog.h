#ifndef MARKERPROPERTIESDIALOG_H
#define MARKERPROPERTIESDIALOG_H

#include <QDialog>     // QDialog 基类
#include <QLineEdit>   // 单行文本输入框基类
#include <QFocusEvent> // 为了 LineEditWithFocusSignal::focusInEvent 参数
#include <QWidget>     // 为了 QWidget* parent 参数
#include <QString>     // Qt 字符串类
#include <vector>      // 为了 std::vector<TimelineMarker*>

// Olive 内部头文件
// 假设 timelinemarker.h 声明了 TimelineMarker 类和 rational 类型 (如果未被其他头文件包含)
#include "timeline/timelinemarker.h"
// 假设 colorcodingcombobox.h 声明了 ColorCodingComboBox 类
#include "widget/colorlabelmenu/colorcodingcombobox.h"
// 假设 rationalslider.h 声明了 RationalSlider 类
#include "widget/slider/rationalslider.h"
// #include "common/define.h" // 如果 rational 等类型未被其他头文件包含

namespace olive {

/**
 * @brief 一个自定义的 QLineEdit，当其获得焦点时会发出一个信号。
 *
 * 此类通过重写 focusInEvent 来在标准 QLineEdit 的基础上增加一个
 * `Focused()` 信号，方便其他组件响应其获得焦点的事件。
 */
class LineEditWithFocusSignal : public QLineEdit {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 LineEditWithFocusSignal 对象。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit LineEditWithFocusSignal(QWidget *parent = nullptr) : QLineEdit(parent) {}

 protected:
  /**
   * @brief 重写 QWidget::focusInEvent() 事件处理函数。
   * 当此输入框获得焦点时，此方法在调用基类实现后会发出 `Focused()` 信号。
   * @param e 指向 QFocusEvent 对象的指针，包含焦点事件信息。
   */
  void focusInEvent(QFocusEvent *e) override {
    QLineEdit::focusInEvent(e); // 调用基类的事件处理
    emit Focused();             // 发出获得焦点信号
  }

 signals:
  /**
   * @brief 当此 LineEdit 获得键盘输入焦点时发出此信号。
   */
  void Focused();
};

/**
 * @brief “标记属性”对话框类。
 *
 * 此对话框用于显示和编辑一个或多个选定的时间轴标记 (TimelineMarker) 的属性。
 * 用户可以修改标记的标签文本、颜色，以及（如果是区间标记）其入点和出点。
 */
class MarkerPropertiesDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 MarkerPropertiesDialog 对象。
   * @param markers 指向一个包含要编辑的 TimelineMarker 对象指针的 std::vector 的常量引用。
   * 对话框将显示这些标记的共同属性，并允许批量修改。
   * @param timebase 标记所在时间轴的时间基准 (例如，序列的帧率)，用于 RationalSlider 显示时间。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  MarkerPropertiesDialog(const std::vector<TimelineMarker *> &markers, const rational &timebase,
                         QWidget *parent = nullptr);

  // ~MarkerPropertiesDialog() override; // 默认析构函数通常足够

 public slots:
  /**
   * @brief 重写 QDialog::accept() 槽函数。
   * 当用户点击“确定”或等效按钮以应用更改时调用。
   * 此函数会将对话框中UI控件设置的属性值（如标签、颜色、入点/出点）
   * 应用到构造时传入的所有标记 (`markers_`)。
   * 这些更改通常应该通过撤销/重做框架来处理。
   */
  void accept() override;

 private:
  /**
   * @brief 存储指向要编辑的时间轴标记对象的向量的拷贝。
   * （注意：构造函数接收的是 const std::vector<TimelineMarker *>& keys，这里是 markers_，
   * 通常如果只是引用，会声明为 const& 或指针。如果是拷贝，其生命周期需明确。）
   * 此处假设 markers_ 存储了要操作的标记列表。
   */
  std::vector<TimelineMarker *> markers_;

  /**
   * @brief 指向 LineEditWithFocusSignal 控件的指针，用于编辑标记的标签文本。
   */
  LineEditWithFocusSignal *label_edit_;

  /**
   * @brief 指向 ColorCodingComboBox (自定义颜色选择下拉框) 的指针，用于选择标记的颜色。
   */
  ColorCodingComboBox *color_menu_;

  /**
   * @brief 指向 RationalSlider 控件的指针，用于编辑区间标记的入点时间。
   * 对于点标记，此控件可能被禁用或隐藏。
   */
  RationalSlider *in_slider_;

  /**
   * @brief 指向 RationalSlider 控件的指针，用于编辑区间标记的出点时间。
   * 对于点标记，此控件可能被禁用或隐藏。
   */
  RationalSlider *out_slider_;
};

}  // namespace olive

#endif  // MARKERPROPERTIESDIALOG_H