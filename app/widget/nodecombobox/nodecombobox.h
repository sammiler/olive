#ifndef NODECOMBOBOX_H
#define NODECOMBOBOX_H

#include <QComboBox> // Qt 组合框控件基类

#include "node/node.h" // 节点基类定义

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWidget;
// class QString;
// class QEvent;

namespace olive {

/**
 * @brief NodeComboBox 类是一个自定义的组合框，专门用于显示和选择项目中的节点。
 *
 * 它继承自 QComboBox，并提供了在下拉列表中填充可用节点、
 * 获取和设置当前选中节点（通过节点 ID）的功能。
 * 当选中的节点发生改变时，会发出 NodeChanged 信号。
 */
class NodeComboBox : public QComboBox {
  Q_OBJECT // Qt 元对象系统宏，用于支持信号和槽机制

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit NodeComboBox(QWidget* parent = nullptr);

  /**
   * @brief 重写 QComboBox 的 showPopup 方法。
   *
   * 此方法在下拉列表即将显示时被调用。
   * 可能会在此处动态填充或更新组合框中的节点列表。
   */
  void showPopup() override;

  /**
   * @brief 获取当前选中的节点的 ID。
   * @return 返回一个 const 引用，指向当前选中节点的 ID 字符串 (QString)。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const QString& GetSelectedNode() const;

public slots:
 /**
  * @brief 设置当前选中的节点。
  * @param id 要选中的节点的 ID 字符串。
  */
 void SetNode(const QString& id);

protected:
  /**
   * @brief 处理 Qt 的 changeEvent 事件。
   *
   * 可能用于响应 QEvent::LanguageChange 事件，以便在应用程序语言设置更改时
   * 更新组合框中节点名称的显示（如果节点名称是可翻译的）。
   * @param e 指向 QEvent 对象的指针。
   */
  void changeEvent(QEvent* e) override;

  signals:
   /**
    * @brief 当用户通过组合框选择了不同的节点时发出此信号。
    * @param id 新选中的节点的 ID 字符串。
    */
   void NodeChanged(const QString& id);

private:
  /**
   * @brief 更新组合框当前显示的文本以匹配选中的节点。
   *
   * 这通常会在选中的节点 ID 改变后调用。
   */
  void UpdateText();

  /**
   * @brief 内部方法，用于设置当前选中的节点，并可选择是否发出信号。
   * @param id 要设置的节点的 ID 字符串。
   * @param emit_signal 如果为 true，则在节点改变时发出 NodeChanged 信号。
   */
  void SetNodeInternal(const QString& id, bool emit_signal);

  QString selected_id_; ///< 存储当前选中的节点的 ID。
};

}  // namespace olive

#endif  // FOOTAGECOMBOBOX_H // 注意：这里的宏定义与文件名 NodeComboBox.h 不一致
