#include "nodeparamviewwidgetbridge.h"

#include <QCheckBox>
#include <QFontComboBox>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <ranges>

#include "common/qtutils.h"
#include "core.h"
#include "node/group/group.h"
#include "node/node.h"
#include "node/nodeundo.h"
#include "node/project/sequence/sequence.h"
#include "nodeparamviewarraywidget.h"
#include "nodeparamviewtextedit.h"
#include "undo/undostack.h"
#include "widget/bezier/bezierwidget.h"
#include "widget/colorbutton/colorbutton.h"
#include "widget/filefield/filefield.h"
#include "widget/slider/floatslider.h"
#include "widget/slider/integerslider.h"
#include "widget/slider/rationalslider.h"

namespace olive {

NodeParamViewWidgetBridge::NodeParamViewWidgetBridge(NodeInput input, QObject* parent) : QObject(parent) {
  do {
    input_hierarchy_.append(input);

    connect(input.node(), &Node::ValueChanged, this, &NodeParamViewWidgetBridge::InputValueChanged);
    connect(input.node(), &Node::InputPropertyChanged, this, &NodeParamViewWidgetBridge::PropertyChanged);
    connect(input.node(), &Node::InputDataTypeChanged, this, &NodeParamViewWidgetBridge::InputDataTypeChanged);
  } while (NodeGroup::GetInner(&input));

  CreateWidgets();
}

int GetSliderCount(NodeValue::Type type) { return NodeValue::get_number_of_keyframe_tracks(type); }

void NodeParamViewWidgetBridge::CreateWidgets() {
  auto* parent = dynamic_cast<QWidget*>(this->parent());

  if (GetInnerInput().IsArray() && GetInnerInput().element() == -1) {
    auto* w = new NodeParamViewArrayWidget(GetInnerInput().node(), GetInnerInput().input(), parent);
    connect(w, &NodeParamViewArrayWidget::DoubleClicked, this, &NodeParamViewWidgetBridge::ArrayWidgetDoubleClicked);
    widgets_.append(w);

  } else {
    // We assume the first data type is the "primary" type
    NodeValue::Type t = GetDataType();
    switch (t) {
      // None of these inputs have applicable UI widgets
      case NodeValue::kNone:
      case NodeValue::kTexture:
      case NodeValue::kMatrix:
      case NodeValue::kSamples:
      case NodeValue::kVideoParams:
      case NodeValue::kAudioParams:
      case NodeValue::kSubtitleParams:
      case NodeValue::kBinary:
      case NodeValue::kDataTypeCount:
        break;
      case NodeValue::kInt: {
        CreateSliders<IntegerSlider>(1, parent);
        break;
      }
      case NodeValue::kRational: {
        CreateSliders<RationalSlider>(1, parent);
        break;
      }
      case NodeValue::kFloat:
      case NodeValue::kVec2:
      case NodeValue::kVec3:
      case NodeValue::kVec4: {
        CreateSliders<FloatSlider>(GetSliderCount(t), parent);
        break;
      }
      case NodeValue::kCombo: {
        auto* combobox = new QComboBox(parent);

        QStringList items = GetInnerInput().GetComboBoxStrings();
        foreach (const QString& s, items) {
          combobox->addItem(s);
        }

        widgets_.append(combobox);
        connect(combobox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
                &NodeParamViewWidgetBridge::WidgetCallback);
        break;
      }
      case NodeValue::kFile: {
        auto* file_field = new FileField(parent);
        widgets_.append(file_field);
        connect(file_field, &FileField::FilenameChanged, this, &NodeParamViewWidgetBridge::WidgetCallback);
        break;
      }
      case NodeValue::kColor: {
        auto* color_button = new ColorButton(GetInnerInput().node()->project()->color_manager(), parent);
        widgets_.append(color_button);
        connect(color_button, &ColorButton::ColorChanged, this, &NodeParamViewWidgetBridge::WidgetCallback);
        break;
      }
      case NodeValue::kText: {
        auto* line_edit = new NodeParamViewTextEdit(parent);
        widgets_.append(line_edit);
        connect(line_edit, &NodeParamViewTextEdit::textEdited, this, &NodeParamViewWidgetBridge::WidgetCallback);
        connect(line_edit, &NodeParamViewTextEdit::RequestEditInViewer, this,
                &NodeParamViewWidgetBridge::RequestEditTextInViewer);
        break;
      }
      case NodeValue::kBoolean: {
        auto* check_box = new QCheckBox(parent);
        widgets_.append(check_box);
        connect(check_box, &QCheckBox::clicked, this, &NodeParamViewWidgetBridge::WidgetCallback);
        break;
      }
      case NodeValue::kFont: {
        auto* font_combobox = new QFontComboBox(parent);
        widgets_.append(font_combobox);
        connect(font_combobox, &QFontComboBox::currentFontChanged, this, &NodeParamViewWidgetBridge::WidgetCallback);
        break;
      }
      case NodeValue::kBezier: {
        auto* bezier = new BezierWidget(parent);
        widgets_.append(bezier);

        connect(bezier->x_slider(), &FloatSlider::ValueChanged, this, &NodeParamViewWidgetBridge::WidgetCallback);
        connect(bezier->y_slider(), &FloatSlider::ValueChanged, this, &NodeParamViewWidgetBridge::WidgetCallback);
        connect(bezier->cp1_x_slider(), &FloatSlider::ValueChanged, this, &NodeParamViewWidgetBridge::WidgetCallback);
        connect(bezier->cp1_y_slider(), &FloatSlider::ValueChanged, this, &NodeParamViewWidgetBridge::WidgetCallback);
        connect(bezier->cp2_x_slider(), &FloatSlider::ValueChanged, this, &NodeParamViewWidgetBridge::WidgetCallback);
        connect(bezier->cp2_y_slider(), &FloatSlider::ValueChanged, this, &NodeParamViewWidgetBridge::WidgetCallback);
        break;
      }
    }

    // Check all properties
    UpdateProperties();

    UpdateWidgetValues();

    // Install event filter to disable widgets picking up scroll events
    foreach (QWidget* w, widgets_) {
      w->installEventFilter(&scroll_filter_);
    }
  }
}

void NodeParamViewWidgetBridge::SetInputValue(const QVariant& value, int track) {
  auto* command = new MultiUndoCommand();

  SetInputValueInternal(value, track, command, true);

  Core::instance()->undo_stack()->push(command, GetCommandName());
}

void NodeParamViewWidgetBridge::SetInputValueInternal(const QVariant& value, int track, MultiUndoCommand* command,
                                                      bool insert_on_all_tracks_if_no_key) {
  Node::SetValueAtTime(GetInnerInput(), GetCurrentTimeAsNodeTime(), value, track, command,
                       insert_on_all_tracks_if_no_key);
}

void NodeParamViewWidgetBridge::ProcessSlider(NumericSliderBase* slider, int slider_track, const QVariant& value) {
  if (slider->IsDragging()) {
    // While we're dragging, we block the input's normal signalling and create our own
    if (!dragger_.IsStarted()) {
      rational node_time = GetCurrentTimeAsNodeTime();

      dragger_.Start(NodeKeyframeTrackReference(GetInnerInput(), slider_track), node_time);
    }

    dragger_.Drag(value);

  } else if (dragger_.IsStarted()) {
    // We were dragging and just stopped
    dragger_.Drag(value);

    auto* command = new MultiUndoCommand();
    dragger_.End(command);
    Core::instance()->undo_stack()->push(command, GetCommandName());

  } else {
    // No drag was involved, we can just push the value
    SetInputValue(value, slider_track);
  }
}

void NodeParamViewWidgetBridge::WidgetCallback() {
  switch (GetDataType()) {
    // None of these inputs have applicable UI widgets
    case NodeValue::kNone:
    case NodeValue::kTexture:
    case NodeValue::kMatrix:
    case NodeValue::kSamples:
    case NodeValue::kVideoParams:
    case NodeValue::kAudioParams:
    case NodeValue::kSubtitleParams:
    case NodeValue::kBinary:
    case NodeValue::kDataTypeCount:
      break;
    case NodeValue::kInt: {
      // Widget is a IntegerSlider
      auto* slider = dynamic_cast<IntegerSlider*>(sender());

      ProcessSlider(slider, QVariant::fromValue(slider->GetValue()));
      break;
    }
    case NodeValue::kFloat: {
      // Widget is a FloatSlider
      auto* slider = dynamic_cast<FloatSlider*>(sender());

      ProcessSlider(slider, slider->GetValue());
      break;
    }
    case NodeValue::kRational: {
      // Widget is a RationalSlider
      auto* slider = dynamic_cast<RationalSlider*>(sender());
      ProcessSlider(slider, QVariant::fromValue(slider->GetValue()));
      break;
    }
    case NodeValue::kVec2: {
      // Widget is a FloatSlider
      auto* slider = dynamic_cast<FloatSlider*>(sender());

      ProcessSlider(slider, slider->GetValue());
      break;
    }
    case NodeValue::kVec3: {
      // Widget is a FloatSlider
      auto* slider = dynamic_cast<FloatSlider*>(sender());

      ProcessSlider(slider, slider->GetValue());
      break;
    }
    case NodeValue::kVec4: {
      // Widget is a FloatSlider
      auto* slider = dynamic_cast<FloatSlider*>(sender());

      ProcessSlider(slider, slider->GetValue());
      break;
    }
    case NodeValue::kFile: {
      SetInputValue(dynamic_cast<FileField*>(sender())->GetFilename(), 0);
      break;
    }
    case NodeValue::kColor: {
      // Sender is a ColorButton
      ManagedColor c = dynamic_cast<ColorButton*>(sender())->GetColor();

      auto* command = new MultiUndoCommand();

      SetInputValueInternal(c.red(), 0, command, false);
      SetInputValueInternal(c.green(), 1, command, false);
      SetInputValueInternal(c.blue(), 2, command, false);
      SetInputValueInternal(c.alpha(), 3, command, false);

      Node* n = GetInnerInput().node();
      n->blockSignals(true);
      n->SetInputProperty(GetInnerInput().input(), QStringLiteral("col_input"), c.color_input());
      n->SetInputProperty(GetInnerInput().input(), QStringLiteral("col_display"), c.color_output().display());
      n->SetInputProperty(GetInnerInput().input(), QStringLiteral("col_view"), c.color_output().view());
      n->SetInputProperty(GetInnerInput().input(), QStringLiteral("col_look"), c.color_output().look());
      n->blockSignals(false);

      Core::instance()->undo_stack()->push(command, GetCommandName());
      break;
    }
    case NodeValue::kText: {
      // Sender is a NodeParamViewRichText
      SetInputValue(dynamic_cast<NodeParamViewTextEdit*>(sender())->text(), 0);
      break;
    }
    case NodeValue::kBoolean: {
      // Widget is a QCheckBox
      SetInputValue(dynamic_cast<QCheckBox*>(sender())->isChecked(), 0);
      break;
    }
    case NodeValue::kFont: {
      // Widget is a QFontComboBox
      SetInputValue(dynamic_cast<QFontComboBox*>(sender())->currentFont().family(), 0);
      break;
    }
    case NodeValue::kCombo: {
      // Widget is a QComboBox
      auto* cb = dynamic_cast<QComboBox*>(widgets_.first());
      int index = cb->currentIndex();

      // Subtract any splitters up until this point
      for (int i = index - 1; i >= 0; i--) {
        if (cb->itemData(i, Qt::AccessibleDescriptionRole).toString() == QStringLiteral("separator")) {
          index--;
        }
      }

      SetInputValue(index, 0);
      break;
    }
    case NodeValue::kBezier: {
      // Widget is a FloatSlider (child of BezierWidget)
      auto* bw = dynamic_cast<BezierWidget*>(widgets_.first());
      auto* fs = dynamic_cast<FloatSlider*>(sender());

      int index = -1;
      if (fs == bw->x_slider()) {
        index = 0;
      } else if (fs == bw->y_slider()) {
        index = 1;
      } else if (fs == bw->cp1_x_slider()) {
        index = 2;
      } else if (fs == bw->cp1_y_slider()) {
        index = 3;
      } else if (fs == bw->cp2_x_slider()) {
        index = 4;
      } else if (fs == bw->cp2_y_slider()) {
        index = 5;
      }

      if (index != -1) {
        ProcessSlider(fs, index, fs->GetValue());
      }
      break;
    }
  }
}

template <typename T>
void NodeParamViewWidgetBridge::CreateSliders(int count, QWidget* parent) {
  for (int i = 0; i < count; i++) {
    T* fs = new T(parent);
    fs->SliderBase::SetDefaultValue(GetInnerInput().GetSplitDefaultValueForTrack(i));
    fs->SetLadderElementCount(2);

    // HACK: Force some spacing between sliders
    fs->setContentsMargins(0, 0, QtUtils::QFontMetricsWidth(fs->fontMetrics(), QStringLiteral("        ")), 0);

    widgets_.append(fs);
    connect(fs, &T::ValueChanged, this, &NodeParamViewWidgetBridge::WidgetCallback);
  }
}

void NodeParamViewWidgetBridge::UpdateWidgetValues() {
  if (GetInnerInput().IsArray() && GetInnerInput().element() == -1) {
    return;
  }

  rational node_time;
  if (GetInnerInput().IsKeyframing()) {
    node_time = GetCurrentTimeAsNodeTime();
  }

  // We assume the first data type is the "primary" type
  switch (GetDataType()) {
    // None of these inputs have applicable UI widgets
    case NodeValue::kNone:
    case NodeValue::kTexture:
    case NodeValue::kMatrix:
    case NodeValue::kSamples:
    case NodeValue::kVideoParams:
    case NodeValue::kAudioParams:
    case NodeValue::kSubtitleParams:
    case NodeValue::kBinary:
    case NodeValue::kDataTypeCount:
      break;
    case NodeValue::kInt: {
      dynamic_cast<IntegerSlider*>(widgets_.first())->SetValue(GetInnerInput().GetValueAtTime(node_time).toLongLong());
      break;
    }
    case NodeValue::kFloat: {
      dynamic_cast<FloatSlider*>(widgets_.first())->SetValue(GetInnerInput().GetValueAtTime(node_time).toDouble());
      break;
    }
    case NodeValue::kRational: {
      dynamic_cast<RationalSlider*>(widgets_.first())
          ->SetValue(GetInnerInput().GetValueAtTime(node_time).value<rational>());
      break;
    }
    case NodeValue::kVec2: {
      auto vec2 = GetInnerInput().GetValueAtTime(node_time).value<QVector2D>();

      dynamic_cast<FloatSlider*>(widgets_.at(0))->SetValue(static_cast<double>(vec2.x()));
      dynamic_cast<FloatSlider*>(widgets_.at(1))->SetValue(static_cast<double>(vec2.y()));
      break;
    }
    case NodeValue::kVec3: {
      auto vec3 = GetInnerInput().GetValueAtTime(node_time).value<QVector3D>();

      dynamic_cast<FloatSlider*>(widgets_.at(0))->SetValue(static_cast<double>(vec3.x()));
      dynamic_cast<FloatSlider*>(widgets_.at(1))->SetValue(static_cast<double>(vec3.y()));
      dynamic_cast<FloatSlider*>(widgets_.at(2))->SetValue(static_cast<double>(vec3.z()));
      break;
    }
    case NodeValue::kVec4: {
      auto vec4 = GetInnerInput().GetValueAtTime(node_time).value<QVector4D>();

      dynamic_cast<FloatSlider*>(widgets_.at(0))->SetValue(static_cast<double>(vec4.x()));
      dynamic_cast<FloatSlider*>(widgets_.at(1))->SetValue(static_cast<double>(vec4.y()));
      dynamic_cast<FloatSlider*>(widgets_.at(2))->SetValue(static_cast<double>(vec4.z()));
      dynamic_cast<FloatSlider*>(widgets_.at(3))->SetValue(static_cast<double>(vec4.w()));
      break;
    }
    case NodeValue::kFile: {
      auto* ff = dynamic_cast<FileField*>(widgets_.first());
      ff->SetFilename(GetInnerInput().GetValueAtTime(node_time).toString());
      break;
    }
    case NodeValue::kColor: {
      ManagedColor mc = ManagedColor(GetInnerInput().GetValueAtTime(node_time).value<Color>());

      mc.set_color_input(GetInnerInput().GetProperty("col_input").toString());

      QString d = GetInnerInput().GetProperty("col_display").toString();
      QString v = GetInnerInput().GetProperty("col_view").toString();
      QString l = GetInnerInput().GetProperty("col_look").toString();

      mc.set_color_output(ColorTransform(d, v, l));

      dynamic_cast<ColorButton*>(widgets_.first())->SetColor(mc);
      break;
    }
    case NodeValue::kText: {
      auto* e = dynamic_cast<NodeParamViewTextEdit*>(widgets_.first());
      e->setTextPreservingCursor(GetInnerInput().GetValueAtTime(node_time).toString());
      break;
    }
    case NodeValue::kBoolean:
      dynamic_cast<QCheckBox*>(widgets_.first())->setChecked(GetInnerInput().GetValueAtTime(node_time).toBool());
      break;
    case NodeValue::kFont: {
      auto* fc = dynamic_cast<QFontComboBox*>(widgets_.first());
      fc->blockSignals(true);
      fc->setCurrentFont(GetInnerInput().GetValueAtTime(node_time).toString());
      fc->blockSignals(false);
      break;
    }
    case NodeValue::kCombo: {
      auto* cb = dynamic_cast<QComboBox*>(widgets_.first());
      cb->blockSignals(true);
      int index = GetInnerInput().GetValueAtTime(node_time).toInt();
      for (int i = 0; i < cb->count(); i++) {
        if (cb->itemData(i).toInt() == index) {
          cb->setCurrentIndex(i);
        }
      }
      cb->blockSignals(false);
      break;
    }
    case NodeValue::kBezier: {
      auto* bw = dynamic_cast<BezierWidget*>(widgets_.first());
      bw->SetValue(GetInnerInput().GetValueAtTime(node_time).value<Bezier>());
      break;
    }
  }
}

rational NodeParamViewWidgetBridge::GetCurrentTimeAsNodeTime() const {
  if (GetTimeTarget()) {
    return GetAdjustedTime(GetTimeTarget(), GetInnerInput().node(), GetTimeTarget()->GetPlayhead(),
                           Node::kTransformTowardsInput);
  } else {
    return rational(0);
  }
}

QString NodeParamViewWidgetBridge::GetCommandName() const {
  NodeInput i = GetInnerInput();
  return tr("Edited Value Of %1 - %2").arg(i.node()->GetLabelAndName(), i.node()->GetInputName(i.input()));
}

void NodeParamViewWidgetBridge::SetTimebase(const rational& timebase) {
  if (GetDataType() == NodeValue::kRational) {
    dynamic_cast<RationalSlider*>(widgets_.first())->SetTimebase(timebase);
  }
}

void NodeParamViewWidgetBridge::TimeTargetDisconnectEvent(ViewerOutput* v) {
  disconnect(v, &ViewerOutput::PlayheadChanged, this, &NodeParamViewWidgetBridge::UpdateWidgetValues);
}

void NodeParamViewWidgetBridge::TimeTargetConnectEvent(ViewerOutput* v) {
  connect(v, &ViewerOutput::PlayheadChanged, this, &NodeParamViewWidgetBridge::UpdateWidgetValues);
}

void NodeParamViewWidgetBridge::InputValueChanged(const NodeInput& input, const TimeRange& range) {
  if (GetTimeTarget() && GetInnerInput() == input && !dragger_.IsStarted() &&
      range.in() <= GetTimeTarget()->GetPlayhead() && range.out() >= GetTimeTarget()->GetPlayhead()) {
    // We'll need to update the widgets because the values have changed on our current time
    UpdateWidgetValues();
  }
}

void NodeParamViewWidgetBridge::SetProperty(const QString& key, const QVariant& value) {
  NodeValue::Type data_type = GetDataType();

  // Parameters for all types
  bool key_is_disable = key.startsWith(QStringLiteral("disable"));
  if (key_is_disable || key.startsWith(QStringLiteral("enabled"))) {
    bool e = value.toBool();
    if (key_is_disable) {
      e = !e;
    }

    if (key.size() == 7) {  // just the word "disable" or "enabled"
      for (auto widget : widgets_) {
        widget->setEnabled(e);
      }
    } else {  // set specific track/widget
      bool ok;
      int element = key.mid(7).toInt(&ok);
      int tracks = NodeValue::get_number_of_keyframe_tracks(data_type);

      if (ok && element >= 0 && element < tracks) {
        widgets_.at(element)->setEnabled(e);
      }
    }
  }

  if (key == QStringLiteral("tooltip")) {
    for (auto widget : widgets_) {
      widget->setToolTip(value.toString());
    }
  }

  // Parameters for integers, floats, and vectors
  if (NodeValue::type_is_numeric(data_type) || NodeValue::type_is_vector(data_type)) {
    if (key == QStringLiteral("min")) {
      switch (data_type) {
        case NodeValue::kInt:
          dynamic_cast<IntegerSlider*>(widgets_.first())->SetMinimum(value.value<int64_t>());
          break;
        case NodeValue::kFloat:
          dynamic_cast<FloatSlider*>(widgets_.first())->SetMinimum(value.toDouble());
          break;
        case NodeValue::kRational:
          dynamic_cast<RationalSlider*>(widgets_.first())->SetMinimum(value.value<rational>());
          break;
        case NodeValue::kVec2: {
          auto min = value.value<QVector2D>();
          dynamic_cast<FloatSlider*>(widgets_.at(0))->SetMinimum(min.x());
          dynamic_cast<FloatSlider*>(widgets_.at(1))->SetMinimum(min.y());
          break;
        }
        case NodeValue::kVec3: {
          auto min = value.value<QVector3D>();
          dynamic_cast<FloatSlider*>(widgets_.at(0))->SetMinimum(min.x());
          dynamic_cast<FloatSlider*>(widgets_.at(1))->SetMinimum(min.y());
          dynamic_cast<FloatSlider*>(widgets_.at(2))->SetMinimum(min.z());
          break;
        }
        case NodeValue::kVec4: {
          auto min = value.value<QVector4D>();
          dynamic_cast<FloatSlider*>(widgets_.at(0))->SetMinimum(min.x());
          dynamic_cast<FloatSlider*>(widgets_.at(1))->SetMinimum(min.y());
          dynamic_cast<FloatSlider*>(widgets_.at(2))->SetMinimum(min.z());
          dynamic_cast<FloatSlider*>(widgets_.at(3))->SetMinimum(min.w());
          break;
        }
        default:
          break;
      }
    } else if (key == QStringLiteral("max")) {
      switch (data_type) {
        case NodeValue::kInt:
          dynamic_cast<IntegerSlider*>(widgets_.first())->SetMaximum(value.value<int64_t>());
          break;
        case NodeValue::kFloat:
          dynamic_cast<FloatSlider*>(widgets_.first())->SetMaximum(value.toDouble());
          break;
        case NodeValue::kRational:
          dynamic_cast<RationalSlider*>(widgets_.first())->SetMaximum(value.value<rational>());
          break;
        case NodeValue::kVec2: {
          auto max = value.value<QVector2D>();
          dynamic_cast<FloatSlider*>(widgets_.at(0))->SetMaximum(max.x());
          dynamic_cast<FloatSlider*>(widgets_.at(1))->SetMaximum(max.y());
          break;
        }
        case NodeValue::kVec3: {
          auto max = value.value<QVector3D>();
          dynamic_cast<FloatSlider*>(widgets_.at(0))->SetMaximum(max.x());
          dynamic_cast<FloatSlider*>(widgets_.at(1))->SetMaximum(max.y());
          dynamic_cast<FloatSlider*>(widgets_.at(2))->SetMaximum(max.z());
          break;
        }
        case NodeValue::kVec4: {
          auto max = value.value<QVector4D>();
          dynamic_cast<FloatSlider*>(widgets_.at(0))->SetMaximum(max.x());
          dynamic_cast<FloatSlider*>(widgets_.at(1))->SetMaximum(max.y());
          dynamic_cast<FloatSlider*>(widgets_.at(2))->SetMaximum(max.z());
          dynamic_cast<FloatSlider*>(widgets_.at(3))->SetMaximum(max.w());
          break;
        }
        default:
          break;
      }
    } else if (key == QStringLiteral("offset")) {
      int tracks = NodeValue::get_number_of_keyframe_tracks(data_type);

      QVector<QVariant> offsets = NodeValue::split_normal_value_into_track_values(data_type, value);

      for (int i = 0; i < tracks; i++) {
        dynamic_cast<NumericSliderBase*>(widgets_.at(i))->SetOffset(offsets.at(i));
      }

      UpdateWidgetValues();

    } else if (key.startsWith(QStringLiteral("color"))) {
      QColor c(value.toString());

      int tracks = NodeValue::get_number_of_keyframe_tracks(data_type);

      if (key.size() == 5) {
        // Set for all tracks
        for (int i = 0; i < tracks; i++) {
          dynamic_cast<SliderBase*>(widgets_.at(i))->SetColor(c);
        }
      } else {
        bool ok;
        int element = key.mid(5).toInt(&ok);
        if (ok && element >= 0 && element < tracks) {
          dynamic_cast<SliderBase*>(widgets_.at(element))->SetColor(c);
        }
      }

    } else if (key == QStringLiteral("base")) {
      double d = value.toDouble();
      for (auto widget : widgets_) {
        dynamic_cast<NumericSliderBase*>(widget)->SetDragMultiplier(d);
      }
    }
  }

  // ComboBox strings changing
  if (data_type == NodeValue::kCombo) {
    if (key == QStringLiteral("combo_str")) {
      auto* cb = dynamic_cast<QComboBox*>(widgets_.first());

      int old_index = cb->currentIndex();

      // Block the combobox changed signals since we anticipate the index will be the same and not require a re-render
      cb->blockSignals(true);

      cb->clear();

      QStringList items = value.toStringList();
      int index = 0;
      foreach (const QString& s, items) {
        if (s.isEmpty()) {
          cb->insertSeparator(cb->count());
          cb->setItemData(cb->count() - 1, -1);
        } else {
          cb->addItem(s, index);
          index++;
        }
      }

      cb->setCurrentIndex(old_index);

      cb->blockSignals(false);

      // In case the amount of items is LESS and the previous index cannot be set, NOW we trigger a re-cache since the
      // value has changed
      if (cb->currentIndex() != old_index) {
        WidgetCallback();
      }
    }
  }

  // Parameters for floats and vectors only
  if (data_type == NodeValue::kFloat || NodeValue::type_is_vector(data_type)) {
    if (key == QStringLiteral("view")) {
      auto display_type = static_cast<FloatSlider::DisplayType>(value.toInt());

      foreach (QWidget* w, widgets_) {
        dynamic_cast<FloatSlider*>(w)->SetDisplayType(display_type);
      }
    } else if (key == QStringLiteral("decimalplaces")) {
      int dec_places = value.toInt();

      foreach (QWidget* w, widgets_) {
        dynamic_cast<FloatSlider*>(w)->SetDecimalPlaces(dec_places);
      }
    } else if (key == QStringLiteral("autotrim")) {
      bool autotrim = value.toBool();

      foreach (QWidget* w, widgets_) {
        dynamic_cast<FloatSlider*>(w)->SetAutoTrimDecimalPlaces(autotrim);
      }
    }
  }

  if (data_type == NodeValue::kRational) {
    if (key == QStringLiteral("view")) {
      auto display_type = static_cast<RationalSlider::DisplayType>(value.toInt());

      foreach (QWidget* w, widgets_) {
        dynamic_cast<RationalSlider*>(w)->SetDisplayType(display_type);
      }
    } else if (key == QStringLiteral("viewlock")) {
      bool locked = value.toBool();

      foreach (QWidget* w, widgets_) {
        dynamic_cast<RationalSlider*>(w)->SetLockDisplayType(locked);
      }
    }
  }

  // Parameters for files
  if (data_type == NodeValue::kFile) {
    auto* ff = dynamic_cast<FileField*>(widgets_.first());

    if (key == QStringLiteral("placeholder")) {
      ff->SetPlaceholder(value.toString());
    } else if (key == QStringLiteral("directory")) {
      ff->SetDirectoryMode(value.toBool());
    }
  }

  // Parameters for text
  if (data_type == NodeValue::kText) {
    auto* tex = dynamic_cast<NodeParamViewTextEdit*>(widgets_.first());

    if (key == QStringLiteral("vieweronly")) {
      tex->SetEditInViewerOnlyMode(value.toBool());
    }
  }
}

void NodeParamViewWidgetBridge::InputDataTypeChanged(const QString& input, NodeValue::Type type) {
  if (sender() == GetOuterInput().node() && input == GetOuterInput().input()) {
    // Delete all widgets
    qDeleteAll(widgets_);
    widgets_.clear();

    // Create new widgets
    CreateWidgets();

    // Signal that widgets are new
    emit WidgetsRecreated(GetOuterInput());
  }
}

void NodeParamViewWidgetBridge::PropertyChanged(const QString& input, const QString& key, const QVariant& value) {
  bool found = false;

  for (const auto& it : input_hierarchy_) {
    if (it.input() == input) {
      found = true;
      break;
    }
  }

  if (found) {
    UpdateProperties();
  }
}

void NodeParamViewWidgetBridge::UpdateProperties() {
  // Set properties from the last entry (the innermost input) to the first (the outermost)
  for (const auto& it : std::ranges::reverse_view(input_hierarchy_)) {
    auto input_properties = it.node()->GetInputProperties(it.input());
    for (auto jt = input_properties.cbegin(); jt != input_properties.cend(); jt++) {
      SetProperty(jt.key(), jt.value());
    }
  }
}

bool NodeParamViewScrollBlocker::eventFilter(QObject* watched, QEvent* event) {
  Q_UNUSED(watched)

  if (event->type() == QEvent::Wheel) {
    // Block this event
    return true;
  }

  return false;
}

}  // namespace olive
