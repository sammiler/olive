#ifndef NODEPARAMVIEWARRAYWIDGET_H
#define NODEPARAMVIEWARRAYWIDGET_H

#include <QLabel>        // Qt 标签控件
#include <QPushButton>   // Qt 按钮控件
#include <QWidget>       // Qt 控件基类

#include "node/param.h"  // 节点参数相关定义 (可能包含 NodeInput, NodeKeyframeTrackReference 等)

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QEvent;
// class QMouseEvent;
// class QString;

// 前向声明项目内部类 (根据用户要求，不添加)
// class Node;

namespace olive {

/**
 * @brief NodeParamViewArrayButton 类是一个专门用于数组参数视图中添加或移除元素的按钮。
 *
 * 它继承自 QPushButton，并根据其类型（添加或移除）显示不同的文本或图标。
 * 该按钮支持在应用程序语言更改时更新其显示文本。
 */
class NodeParamViewArrayButton : public QPushButton {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 定义按钮的类型，指示其功能是添加元素还是移除元素。
   */
  enum Type {
    kAdd,    ///< 按钮用于向数组添加新元素。
    kRemove  ///< 按钮用于从数组移除元素。
  };

  /**
   * @brief 构造函数。
   * @param type 按钮的类型 (kAdd 或 kRemove)。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit NodeParamViewArrayButton(Type type, QWidget* parent = nullptr);

 protected:
  /**
   * @brief 处理 Qt 的 changeEvent 事件。
   *
   * 主要用于响应 QEvent::LanguageChange 事件，以便在应用程序语言设置更改时
   * 调用 Retranslate() 方法来更新按钮的文本或图标。
   * @param event 指向 QEvent 对象的指针。
   */
  void changeEvent(QEvent* event) override;

 private:
  /**
   * @brief 重新翻译按钮的文本（或更新图标）。
   *
   * 此方法会在构造时以及语言更改时被调用，以确保按钮显示正确的本地化文本。
   */
  void Retranslate();

  Type type_; ///< 存储按钮的类型 (kAdd 或 kRemove)。
};

/**
 * @brief NodeParamViewArrayWidget 类是一个用于显示和管理节点数组参数的控件。
 *
 * 它通常显示数组中元素的数量，并可能包含用于添加或移除元素的按钮
 * (NodeParamViewArrayButton)。当用户双击此控件时，可能会触发更详细的数组编辑界面。
 * 此控件与特定的节点 (Node) 及其一个输入参数 (input) 相关联。
 */
class NodeParamViewArrayWidget : public QWidget {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param node 指向与此数组参数关联的 Node 对象的指针。
   * @param input 数组参数的名称 (QString)。
   * @param parent 父控件指针，默认为 nullptr。
   */
  NodeParamViewArrayWidget(Node* node, QString input, QWidget* parent = nullptr);

 signals:
  /**
   * @brief 当用户双击此控件时发出此信号。
   *
   * 通常用于打开一个更详细的数组编辑器。
   */
  void DoubleClicked();

 protected:
  /**
   * @brief 鼠标双击事件处理函数。
   *
   * 当用户双击此控件时，会发出 DoubleClicked() 信号。
   * @param event 鼠标事件指针。
   */
  void mouseDoubleClickEvent(QMouseEvent* event) override;

 private:
  Node* node_; ///< 指向与此数组参数关联的节点对象的指针。

  QString input_; ///< 此控件管理的数组参数的名称。

  QLabel* count_lbl_; ///< 用于显示数组当前元素数量的标签。

 private slots:
  /**
   * @brief 当关联的数组参数大小发生改变时调用的槽函数。
   *
   * 此函数会更新 count_lbl_ 以显示新的元素数量。
   * @param input 发生改变的输入参数的名称（用于确认是正确的参数）。
   * @param old_size 数组的旧大小。
   * @param new_size 数组的新大小。
   */
  void UpdateCounter(const QString& input, int old_size, int new_size);
};

}  // namespace olive

#endif  // NODEPARAMVIEWARRAYWIDGET_H
