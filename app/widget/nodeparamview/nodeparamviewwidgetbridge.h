

#ifndef NODEPARAMVIEWWIDGETBRIDGE_H
#define NODEPARAMVIEWWIDGETBRIDGE_H

#include <QObject>

#include "node/inputdragger.h"
#include "widget/slider/base/numericsliderbase.h"
#include "widget/timetarget/timetarget.h"

namespace olive {

class NodeParamViewScrollBlocker : public QObject {
  Q_OBJECT
 public:
  bool eventFilter(QObject* watched, QEvent* event) override;
};

class NodeParamViewWidgetBridge : public QObject, public TimeTargetObject {
  Q_OBJECT
 public:
  NodeParamViewWidgetBridge(NodeInput input, QObject* parent);

  [[nodiscard]] const QVector<QWidget*>& widgets() const { return widgets_; }

  // Set the timebase of certain Timebased widgets
  void SetTimebase(const rational& timebase);

 signals:
  void ArrayWidgetDoubleClicked();

  void WidgetsRecreated(const NodeInput& input);

  void RequestEditTextInViewer();

 protected:
  void TimeTargetDisconnectEvent(ViewerOutput* v) override;
  void TimeTargetConnectEvent(ViewerOutput* v) override;

 private:
  void CreateWidgets();

  void SetInputValue(const QVariant& value, int track);

  void SetInputValueInternal(const QVariant& value, int track, MultiUndoCommand* command,
                             bool insert_on_all_tracks_if_no_key);

  void ProcessSlider(NumericSliderBase* slider, int slider_track, const QVariant& value);
  void ProcessSlider(NumericSliderBase* slider, const QVariant& value) {
    ProcessSlider(slider, widgets_.indexOf(slider), value);
  }

  void SetProperty(const QString& key, const QVariant& value);

  template <typename T>
  void CreateSliders(int count, QWidget* parent);

  void UpdateWidgetValues();

  [[nodiscard]] rational GetCurrentTimeAsNodeTime() const;

  [[nodiscard]] const NodeInput& GetOuterInput() const { return input_hierarchy_.first(); }

  [[nodiscard]] const NodeInput& GetInnerInput() const { return input_hierarchy_.last(); }

  [[nodiscard]] QString GetCommandName() const;

  [[nodiscard]] NodeValue::Type GetDataType() const { return GetOuterInput().GetDataType(); }

  void UpdateProperties();

  QVector<NodeInput> input_hierarchy_;

  QVector<QWidget*> widgets_;

  NodeInputDragger dragger_;

  NodeParamViewScrollBlocker scroll_filter_;

 private slots:
  void WidgetCallback();

  void InputValueChanged(const NodeInput& input, const TimeRange& range);

  void InputDataTypeChanged(const QString& input, NodeValue::Type type);

  void PropertyChanged(const QString& input, const QString& key, const QVariant& value);
};

}  // namespace olive

#endif  // NODEPARAMVIEWWIDGETBRIDGE_H
