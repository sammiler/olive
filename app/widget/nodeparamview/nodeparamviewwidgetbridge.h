#ifndef NODEPARAMVIEWWIDGETBRIDGE_H
#define NODEPARAMVIEWWIDGETBRIDGE_H

#include <QObject>  // Qt 对象模型基类
#include <QVector>  // Qt 动态数组容器
#include <QWidget>  // Qt 控件基类 (用于 CreateSliders 和 widgets_ 成员)

#include "node/inputdragger.h"                     // 节点输入参数拖拽处理器
#include "widget/slider/base/numericsliderbase.h"  // 数值滑块控件基类
#include "widget/timetarget/timetarget.h"          // 时间目标对象接口

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QEvent;
// class QVariant;
// class QString;

// 前向声明项目内部类 (根据用户要求，不添加)
// class NodeInput; // 已在 node/inputdragger.h 或 widget/timetarget/timetarget.h 中间接包含
// class ViewerOutput; // TimeTargetObject 中可能使用
// class MultiUndoCommand; // 用于撤销/重做
// class rational; // 用于时间表示
// class TimeRange; // 用于时间范围

namespace olive {

/**
 * @brief NodeParamViewScrollBlocker 是一个事件过滤器，用于阻止特定控件上的滚轮事件。
 *
 * 当参数编辑控件（如滑块）嵌入到可滚动的参数视图中时，
 * 此过滤器可以防止在这些控件上使用滚轮时意外滚动整个视图，
 * 而是允许滚轮事件用于控件本身（如果控件支持）。
 */
class NodeParamViewScrollBlocker : public QObject {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 事件过滤器函数。
      * @param watched 被监视的对象。
      * @param event 发生的事件。
      * @return 如果事件应被过滤（即阻止其进一步传播），则返回 true；否则返回 false。
      */
     bool
     eventFilter(QObject* watched, QEvent* event) override;
};

/**
 * @brief NodeParamViewWidgetBridge 类充当节点输入参数与其在UI中对应的编辑控件之间的桥梁。
 *
 * 它负责根据参数类型创建合适的编辑控件（如滑块、文本框等），
 * 同步参数值与控件显示，处理用户输入，并与撤销/重做系统交互。
 * 同时，它也作为 TimeTargetObject，可以响应时间变化。
 */
class NodeParamViewWidgetBridge : public QObject, public TimeTargetObject {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param input 此桥接器管理的节点输入参数 (NodeInput)。
      * @param parent 父 QObject 指针。
      */
     NodeParamViewWidgetBridge(NodeInput input, QObject* parent);

  /**
   * @brief 获取此桥接器创建和管理的所有UI控件。
   * @return 返回一个 const 引用，指向包含 QWidget 指针的 QVector。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const QVector<QWidget*>& widgets() const { return widgets_; }

  /**
   * @brief 为此桥接器管理的某些基于时间的控件设置时间基准。
   * @param timebase 要设置的时间基准 (rational 类型)。
   */
  void SetTimebase(const rational& timebase);

 signals:
  /**
   * @brief 当数组参数的编辑控件被双击时发出此信号（通常用于打开更详细的编辑器）。
   */
  void ArrayWidgetDoubleClicked();

  /**
   * @brief 当输入参数的数据类型发生变化，导致需要重新创建编辑控件时发出此信号。
   * @param input 发生改变的 NodeInput。
   */
  void WidgetsRecreated(const NodeInput& input);

  /**
   * @brief 当请求在查看器中编辑文本参数时发出此信号。
   */
  void RequestEditTextInViewer();

 protected:
  /**
   * @brief 当此桥接器与时间目标断开连接时调用的事件处理函数。
   * @param v 指向被断开连接的 ViewerOutput 对象的指针。
   */
  void TimeTargetDisconnectEvent(ViewerOutput* v) override;
  /**
   * @brief 当此桥接器连接到新的时间目标时调用的事件处理函数。
   * @param v 指向新连接的 ViewerOutput 对象的指针。
   */
  void TimeTargetConnectEvent(ViewerOutput* v) override;

 private:
  /**
   * @brief 根据当前管理的 NodeInput 的类型和属性创建相应的编辑控件。
   */
  void CreateWidgets();

  /**
   * @brief 设置输入参数的值。
   * @param value 要设置的值 (QVariant)。
   * @param track 如果参数是多维的（例如向量），则指定要设置哪个轨道/分量。
   */
  void SetInputValue(const QVariant& value, int track);

  /**
   * @brief 内部实现设置输入参数值的逻辑，并可选择性地创建撤销命令。
   * @param value 要设置的值。
   * @param track 目标轨道/分量索引。
   * @param command 指向 MultiUndoCommand 的指针，用于聚合多个值的更改。
   * @param insert_on_all_tracks_if_no_key 如果没有关键帧，是否在所有轨道上插入新值。
   */
  void SetInputValueInternal(const QVariant& value, int track, MultiUndoCommand* command,
                             bool insert_on_all_tracks_if_no_key);

  /**
   * @brief 处理数值滑块的值变化。
   * @param slider 发生变化的 NumericSliderBase 指针。
   * @param slider_track 滑块对应的参数轨道/分量索引。
   * @param value 滑块的新值。
   */
  void ProcessSlider(NumericSliderBase* slider, int slider_track, const QVariant& value);
  /**
   * @brief 处理数值滑块的值变化（重载版本，自动推断轨道索引）。
   * @param slider 发生变化的 NumericSliderBase 指针。
   * @param value 滑块的新值。
   */
  void ProcessSlider(NumericSliderBase* slider, const QVariant& value) {
    ProcessSlider(slider, widgets_.indexOf(slider), value);
  }

  /**
   * @brief 设置节点输入参数的附加属性。
   * @param key 属性的键名。
   * @param value 属性的值。
   */
  void SetProperty(const QString& key, const QVariant& value);

  /**
   * @brief 模板函数，用于创建指定数量的某种类型的滑块控件。
   * @tparam T 滑块控件的类型 (必须派生自 NumericSliderBase)。
   * @param count 要创建的滑块数量。
   * @param parent 滑块的父 QWidget。
   */
  template <typename T>
  void CreateSliders(int count, QWidget* parent);

  /**
   * @brief 根据节点参数的当前值更新UI控件的显示。
   */
  void UpdateWidgetValues();

  /**
   * @brief 获取当前时间点（来自时间目标）并将其转换为关联节点的本地时间。
   * @return 返回 rational 类型的时间值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] rational GetCurrentTimeAsNodeTime() const;

  /**
   * @brief 获取输入参数层级结构中的最外层（根）输入。
   * @return 返回一个 const 引用，指向最外层的 NodeInput。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const NodeInput& GetOuterInput() const { return input_hierarchy_.first(); }

  /**
   * @brief 获取输入参数层级结构中的最内层（叶）输入。
   * @return 返回一个 const 引用，指向最内层的 NodeInput。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const NodeInput& GetInnerInput() const { return input_hierarchy_.last(); }

  /**
   * @brief 获取用于撤销/重做命令的描述性名称。
   * @return 返回命令名称的 QString。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QString GetCommandName() const;

  /**
   * @brief 获取最外层输入参数的数据类型。
   * @return 返回 NodeValue::Type 枚举值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] NodeValue::Type GetDataType() const { return GetOuterInput().GetDataType(); }

  /**
   * @brief 更新与输入参数相关的属性（例如，范围、步长等）。
   */
  void UpdateProperties();

  QVector<NodeInput>
      input_hierarchy_;  ///< 存储输入参数的层级结构。例如，一个 Vector3D 参数可能包含 X, Y, Z 三个子输入。

  QVector<QWidget*> widgets_;  ///< 存储为此参数创建的所有UI编辑控件的列表。

  NodeInputDragger dragger_;  ///< 用于处理通过拖拽方式修改参数值的功能。

  NodeParamViewScrollBlocker scroll_filter_;  ///< 事件过滤器，用于阻止在参数控件上意外滚动父视图。

 private slots:
  /**
   * @brief 当其管理的某个UI控件发出值改变信号时调用的通用回调槽函数。
   */
  void WidgetCallback();

  /**
   * @brief 当关联的节点输入参数的值发生改变时（通常由数据模型通知）调用的槽函数。
   * @param input 发生值改变的 NodeInput。
   * @param range 值改变影响的时间范围（对于动画参数）。
   */
  void InputValueChanged(const NodeInput& input, const TimeRange& range);

  /**
   * @brief 当关联的节点输入参数的数据类型发生改变时调用的槽函数。
   *
   * 这通常会导致重新创建UI控件。
   * @param input 发生数据类型改变的输入参数的名称。
   * @param type 新的数据类型。
   */
  void InputDataTypeChanged(const QString& input, NodeValue::Type type);

  /**
   * @brief 当关联的节点输入参数的某个附加属性发生改变时调用的槽函数。
   * @param input 输入参数的名称。
   * @param key 发生改变的属性的键名。
   * @param value 属性的新值。
   */
  void PropertyChanged(const QString& input, const QString& key, const QVariant& value);
};

}  // namespace olive

#endif  // NODEPARAMVIEWWIDGETBRIDGE_H
