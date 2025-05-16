#ifndef SLIDERBASE_H
#define SLIDERBASE_H

#include <QStackedWidget> // Qt 堆叠控件基类，用于在标签和编辑器之间切换

#include "sliderlabel.h"                               // 自定义滑块标签控件
#include "sliderladder.h"                              // 滑块梯形调整器控件 (虽然未直接使用，但 NumericSliderBase 可能需要)
#include "widget/focusablelineedit/focusablelineedit.h"  // 可获取焦点的行编辑控件

// Qt 命名空间中常用的类，如 QVariant, QString, QVector, QPair, QColor, QWidget, QObject, QEvent, Qt::Alignment
// 通常在包含 <QStackedWidget> 或其他核心 Qt 头文件时会被间接包含，或者作为函数参数/返回值类型时，
// 编译器能找到其声明。根据用户明确要求，不在此处添加显式的 #include 指令。

namespace olive {

/**
 * @brief SliderBase 类是所有自定义滑块控件的通用基类。
 *
 * 它使用 QStackedWidget 来在标签显示模式和行编辑模式之间切换。
 * 提供了值格式化、默认值、三态支持以及标签替换等核心功能。
 * 派生类需要实现特定于数据类型的值与字符串之间的转换逻辑。
 */
class SliderBase : public QStackedWidget {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit SliderBase(QWidget* parent = nullptr);

  /**
   * @brief 设置标签和编辑器的对齐方式。
   * @param alignment Qt::Alignment 枚举值。
   */
  void SetAlignment(Qt::Alignment alignment);

  /**
   * @brief 检查滑块当前是否处于三态模式。
   *
   * 三态模式可能用于表示未设置值、混合值或标准值之外的特殊状态。
   * @return 如果处于三态模式，则返回 true；否则返回 false。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool IsTristate() const;
  /**
   * @brief 设置滑块的三态模式。
   */
  void SetTristate();

  /**
   * @brief 设置用于格式化显示值的字符串格式。
   *
   * 格式字符串中可能包含占位符（如 "%1"）来代表实际值。
   * @param s 格式字符串。
   * @param plural 布尔值，指示格式是否应用于复数形式（如果适用）。默认为 false。
   */
  void SetFormat(const QString& s, bool plural = false);
  /**
   * @brief 清除自定义的显示格式，恢复到默认值到字符串的转换。
   */
  void ClearFormat();

  /**
   * @brief 检查当前格式是否为复数形式。
   * @return 如果格式是复数形式，则返回 true。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool IsFormatPlural() const;

  /**
   * @brief 设置滑块的默认值。
   *
   * 用户可能可以通过某种操作（例如，右键菜单）将滑块重置为此默认值。
   * @param v 要设置的默认值 (QVariant 类型)。
   */
  void SetDefaultValue(const QVariant& v);

  /**
   * @brief 获取给定值格式化后的字符串表示。
   *
   * 会应用自定义格式（如果已设置）或标签替换。
   * @param v 要格式化的值 (QVariant 类型)。
   * @return 返回格式化后的 QString。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QString GetFormattedValueToString(const QVariant& v) const;

  /**
   * @brief 添加一个标签替换规则。
   *
   * 当滑块的值等于指定的 `value` 时，标签将显示为 `label` 而不是通过 `ValueToString` 转换的文本。
   * @param value 要被替换的 QVariant 值。
   * @param label 替换 `value` 时显示的 QString 标签。
   */
  void InsertLabelSubstitution(const QVariant& value, const QString& label) {
    label_substitutions_.append({value, label}); // 添加替换规则
    UpdateLabel(); // 更新标签显示
  }

  /**
   * @brief 设置滑块标签的文本颜色。
   * @param c 要设置的 QColor。
   */
  void SetColor(const QColor& c) { label_->SetColor(c); }

 public slots:
  /**
   * @brief 显示行编辑器，允许用户通过文本输入来修改滑块的值。
   *
   * 此槽函数会将 QStackedWidget 切换到显示 FocusableLineEdit 的页面。
   */
  void ShowEditor();

 protected slots:
  /**
   * @brief 更新滑块标签 (SliderLabel) 上显示的文本。
   *
   * 此槽函数会获取当前值，将其格式化，并设置到标签上。
   * 通常在值改变或格式改变后调用。
   */
  void UpdateLabel();

 protected:
  /**
   * @brief 获取滑块内部存储的当前值。
   * @return 返回一个 const 引用，指向 QVariant 类型的值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const QVariant& GetValueInternal() const;

  /**
   * @brief 设置滑块内部存储的值。
   *
   * 此函数会首先调用 AdjustValue 对值进行调整（例如，确保在范围内），
   * 然后更新内部值，更新标签显示，并调用 ValueSignalEvent 发出值改变信号。
   * @param v 要设置的新值 (QVariant 类型)。
   */
  void SetValueInternal(const QVariant& v);

  /**
   * @brief 获取当前设置的自定义格式字符串。
   * @return 返回格式字符串。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QString GetFormat() const;

  /**
   * @brief 获取当前值格式化后的字符串表示。
   * @return 返回格式化后的 QString。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QString GetFormattedValueToString() const;

  /**
   * @brief 获取内部的 SliderLabel 控件。
   * @return 返回指向 SliderLabel 的指针。
   */
  SliderLabel* label() { return label_; }

  /**
   * @brief 纯虚函数，将 QVariant 类型的值转换为 QString 以供显示。
   *
   * 派生类必须实现此函数，以提供特定数据类型到字符串的转换逻辑。
   * @param v 要转换的 QVariant 值。
   * @return 返回值的字符串表示。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] virtual QString ValueToString(const QVariant& v) const = 0;

  /**
   * @brief 纯虚函数，将 QString 转换为 QVariant 类型的值。
   *
   * 派生类必须实现此函数，以提供从用户输入的字符串到特定数据类型的转换逻辑。
   * @param s 要转换的 QString。
   * @param ok 指向布尔值的指针，用于指示转换是否成功。
   * @return 返回转换后的 QVariant 值。如果转换失败，*ok 应被设为 false。
   */
  virtual QVariant StringToValue(const QString& s, bool* ok) const = 0;

  /**
   * @brief 虚函数，在设置值之前对其进行调整。
   *
   * 派生类可以重写此函数以实现值的约束（例如，确保在最小/最大范围内）或其他预处理。
   * @param value 要调整的原始值。
   * @return 返回调整后的值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] virtual QVariant AdjustValue(const QVariant& value) const;

  /**
   * @brief 虚函数，检查当前是否可以设置值。
   *
   * 派生类可以重写此函数以添加额外的条件（例如，控件是否已启用）。
   * @return 如果可以设置值，则返回 true。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] virtual bool CanSetValue() const;

  /**
   * @brief 纯虚函数，当滑块的值发生改变时，由派生类实现以发出特定类型的信号。
   * @param value 改变后的新值。
   */
  virtual void ValueSignalEvent(const QVariant& value) = 0;

  /**
   * @brief 重写 QWidget 的 changeEvent 方法。
   *
   * 可能用于响应 QEvent::LanguageChange 事件以更新标签文本或格式。
   * @param e 事件指针。
   */
  void changeEvent(QEvent* e) override;

 private:
  /**
   * @brief 检查给定的值是否有对应的标签替换规则。
   * @param v 要检查的 QVariant 值。
   * @param out 如果找到替换规则，则此 QString 指针会被设置为替换后的标签文本。
   * @return 如果找到了替换规则，则返回 true；否则返回 false。
   */
  bool GetLabelSubstitution(const QVariant& v, QString* out) const;

  SliderLabel* label_; ///< 用于显示滑块当前值的自定义标签控件。
  FocusableLineEdit* editor_; ///< 用于通过文本输入编辑滑块值的可获取焦点的行编辑控件。

  QVariant value_;         ///< 滑块当前的值。
  QVariant default_value_; ///< 滑块的默认值。

  bool tristate_; ///< 标记滑块是否处于三态模式。

  QString custom_format_; ///< 自定义的格式化字符串。
  bool format_plural_;    ///< 标记自定义格式是否用于复数形式。

  QVector<QPair<QVariant, QString> > label_substitutions_; ///< 存储值到标签的替换规则列表。

 private slots:
  /**
   * @brief 当行编辑器 (FocusableLineEdit) 中的输入被确认时（例如按下回车）调用的槽函数。
   *
   * 此函数会尝试将编辑框中的文本转换为值，并更新滑块。
   */
  void LineEditConfirmed();

  /**
   * @brief 当行编辑器 (FocusableLineEdit) 中的输入被取消时（例如按下 Esc）调用的槽函数。
   *
   * 此函数会将 QStackedWidget 切换回显示 SliderLabel 的页面。
   */
  void LineEditCancelled();

  /**
   * @brief 将滑块的值重置为其默认值。
   */
  void ResetValue();
};

}  // namespace olive

#endif  // SLIDERBASE_H
