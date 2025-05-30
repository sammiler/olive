#include "node.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QGuiApplication>
#include <ranges>

#include "common/lerp.h"
#include "config/config.h"
#include "core.h"
#include "node/group/group.h"
#include "node/project/serializer/typeserializer.h"
#include "nodeundo.h"
#include "project.h"
#include "serializeddata.h"
#include "ui/colorcoding.h"
#include "ui/icons/icons.h"

namespace olive {

#define super QObject

const QString Node::kEnabledInput = QStringLiteral("enabled_in");

Node::Node() : override_color_(-1), folder_(nullptr), flags_(kNone), caches_enabled_(true) {
  AddInput(kEnabledInput, NodeValue::kBoolean, true);

  video_cache_ = new FrameHashCache(this);
  thumbnail_cache_ = new ThumbnailCache(this);
  audio_cache_ = new AudioPlaybackCache(this);
  waveform_cache_ = new AudioWaveformCache(this);

  waveform_cache_->SetSavingEnabled(false);
}

Node::~Node() {
  // Disconnect all edges
  DisconnectAll();

  // Remove self from anything while we're still a node rather than a base QObject
  setParent(nullptr);

  // Remove all immediates
  foreach (NodeInputImmediate *i, standard_immediates_) {
    delete i;
  }
  for (auto it = array_immediates_.cbegin(); it != array_immediates_.cend(); it++) {
    foreach (NodeInputImmediate *i, it.value()) {
      delete i;
    }
  }
}

Project *Node::parent() const { return dynamic_cast<Project *>(QObject::parent()); }

Project *Node::project() const { return Project::GetProjectFromObject(this); }

QString Node::ShortName() const { return Name(); }

QString Node::Description() const {
  // Return an empty string by default
  return {};
}

void Node::Retranslate() { SetInputName(kEnabledInput, tr("Enabled")); }

QVariant Node::data(const DataType &d) const {
  if (d == ICON) {
    // Just a meaningless default icon to be used where necessary
    return icon::New;
  }

  return {};
}

bool Node::SetNodePositionInContext(Node *node, const QPointF &pos) {
  Position p = context_positions_.value(node);

  p.position = pos;

  return SetNodePositionInContext(node, p);
}

bool Node::SetNodePositionInContext(Node *node, const Position &pos) {
  bool added = !ContextContainsNode(node);
  context_positions_.insert(node, pos);

  if (added) {
    emit NodeAddedToContext(node);
  }

  emit NodePositionInContextChanged(node, pos.position);

  return added;
}

bool Node::RemoveNodeFromContext(Node *node) {
  if (ContextContainsNode(node)) {
    context_positions_.remove(node);
    emit NodeRemovedFromContext(node);
    return true;
  } else {
    return false;
  }
}

Color Node::color() const {
  int c;

  if (override_color_ >= 0) {
    c = override_color_;
  } else {
    c = OLIVE_CONFIG_STR(QStringLiteral("CatColor%1").arg(this->Category().first())).toInt();
  }

  return ColorCoding::GetColor(c);
}

QLinearGradient Node::gradient_color(qreal top, qreal bottom) const {
  QLinearGradient grad;

  grad.setStart(0, top);
  grad.setFinalStop(0, bottom);

  QColor c = QtUtils::toQColor(color());

  grad.setColorAt(0.0, c.lighter());
  grad.setColorAt(1.0, c);

  return grad;
}

QBrush Node::brush(qreal top, qreal bottom) const {
  if (OLIVE_CONFIG("UseGradients").toBool()) {
    return gradient_color(top, bottom);
  } else {
    return QtUtils::toQColor(color());
  }
}

void Node::ConnectEdge(Node *output, const NodeInput &input) {
  // Ensure graph is the same
  Q_ASSERT(input.node()->parent() == output->parent());

  // Ensure a connection isn't getting overwritten
  Q_ASSERT(input.node()->input_connections().find(input) == input.node()->input_connections().end());

  // Insert connection on both sides
  input.node()->input_connections_[input] = output;
  output->output_connections_.push_back(std::pair<Node *, NodeInput>({output, input}));

  // Call internal events
  input.node()->InputConnectedEvent(input.input(), input.element(), output);
  output->OutputConnectedEvent(input);

  // Emit signals
  emit input.node()->InputConnected(output, input);
  emit output->OutputConnected(output, input);

  // Invalidate all if this node isn't ignoring this input
  if (!(input.node()->GetInputFlags(input.input()) & kInputFlagIgnoreInvalidations)) {
    input.node()->InvalidateAll(input.input(), input.element());
  }
}

void Node::DisconnectEdge(Node *output, const NodeInput &input) {
  // Ensure graph is the same
  Q_ASSERT(input.node()->parent() == output->parent());

  // Ensure connection exists
  Q_ASSERT(input.node()->input_connections().at(input) == output);

  // Remove connection from both sides
  InputConnections &inputs = input.node()->input_connections_;
  inputs.erase(inputs.find(input));

  OutputConnections &outputs = output->output_connections_;
  outputs.erase(std::find(outputs.begin(), outputs.end(), std::pair<Node *, NodeInput>({output, input})));

  // Call internal events
  input.node()->InputDisconnectedEvent(input.input(), input.element(), output);
  output->OutputDisconnectedEvent(input);

  emit input.node()->InputDisconnected(output, input);
  emit output->OutputDisconnected(output, input);

  if (!(input.node()->GetInputFlags(input.input()) & kInputFlagIgnoreInvalidations)) {
    input.node()->InvalidateAll(input.input(), input.element());
  }
}

void Node::CopyCacheUuidsFrom(Node *n) {
  video_cache_->SetUuid(n->video_cache_->GetUuid());
  audio_cache_->SetUuid(n->audio_cache_->GetUuid());
  thumbnail_cache_->SetUuid(n->thumbnail_cache_->GetUuid());
  waveform_cache_->SetUuid(n->waveform_cache_->GetUuid());
}

QString Node::GetInputName(const QString &id) const {
  const Input *i = GetInternalInputData(id);

  if (i) {
    return i->human_name;
  } else {
    ReportInvalidInput("get name of", id, -1);
    return {};
  }
}

bool Node::IsInputHidden(const QString &input) const { return (GetInputFlags(input).value() & kInputFlagHidden); }

bool Node::IsInputConnectable(const QString &input) const { return !(GetInputFlags(input) & kInputFlagNotConnectable); }

bool Node::IsInputKeyframable(const QString &input) const { return !(GetInputFlags(input) & kInputFlagNotKeyframable); }

bool Node::IsInputKeyframing(const QString &input, int element) const {
  NodeInputImmediate *imm = GetImmediate(input, element);

  if (imm) {
    return imm->is_keyframing();
  } else {
    ReportInvalidInput("get keyframing state of", input, element);
    return false;
  }
}

void Node::SetInputIsKeyframing(const QString &input, bool e, int element) {
  if (!IsInputKeyframable(input)) {
    qDebug() << "Ignored set keyframing of" << input << "because this input is not keyframable";
    return;
  }

  NodeInputImmediate *imm = GetImmediate(input, element);

  if (imm) {
    imm->set_is_keyframing(e);

    emit KeyframeEnableChanged(NodeInput(this, input, element), e);
  } else {
    ReportInvalidInput("set keyframing state of", input, element);
  }
}

bool Node::IsInputConnected(const QString &input, int element) const { return GetConnectedOutput(input, element); }

Node *Node::GetConnectedOutput(const QString &input, int element) const {
  for (const auto &input_connection : input_connections_) {
    if (input_connection.first.input() == input && input_connection.first.element() == element) {
      return input_connection.second;
    }
  }

  return nullptr;
}

bool Node::IsUsingStandardValue(const QString &input, int track, int element) const {
  NodeInputImmediate *imm = GetImmediate(input, element);

  if (imm) {
    return imm->is_using_standard_value(track);
  } else {
    ReportInvalidInput("determine whether using standard value in", input, element);
    return true;
  }
}

NodeValue::Type Node::GetInputDataType(const QString &id) const {
  const Input *i = GetInternalInputData(id);

  if (i) {
    return i->type;
  } else {
    ReportInvalidInput("get data type of", id, -1);
    return NodeValue::kNone;
  }
}

void Node::SetInputDataType(const QString &id, const NodeValue::Type &type) {
  Input *input_meta = GetInternalInputData(id);

  if (input_meta) {
    input_meta->type = type;

    int array_sz = InputArraySize(id);
    for (int i = -1; i < array_sz; i++) {
      GetImmediate(id, i)->set_data_type(type);
    }

    emit InputDataTypeChanged(id, type);
  } else {
    ReportInvalidInput("set data type of", id, -1);
  }
}

bool Node::HasInputProperty(const QString &id, const QString &name) const {
  const Input *i = GetInternalInputData(id);

  if (i) {
    return i->properties.contains(name);
  } else {
    ReportInvalidInput("get property of", id, -1);
    return false;
  }
}

QHash<QString, QVariant> Node::GetInputProperties(const QString &id) const {
  const Input *i = GetInternalInputData(id);

  if (i) {
    return i->properties;
  } else {
    ReportInvalidInput("get property table of", id, -1);
    return {};
  }
}

QVariant Node::GetInputProperty(const QString &id, const QString &name) const {
  const Input *i = GetInternalInputData(id);

  if (i) {
    return i->properties.value(name);
  } else {
    ReportInvalidInput("get property of", id, -1);
    return {};
  }
}

void Node::SetInputProperty(const QString &id, const QString &name, const QVariant &value) {
  Input *i = GetInternalInputData(id);

  if (i) {
    i->properties.insert(name, value);

    emit InputPropertyChanged(id, name, value);
  } else {
    ReportInvalidInput("set property of", id, -1);
  }
}

SplitValue Node::GetSplitValueAtTime(const QString &input, const rational &time, int element) const {
  SplitValue vals;

  int nb_tracks = GetNumberOfKeyframeTracks(input);

  for (int i = 0; i < nb_tracks; i++) {
    vals.append(GetSplitValueAtTimeOnTrack(input, time, i, element));
  }

  return vals;
}

QVariant Node::GetSplitValueAtTimeOnTrack(const QString &input, const rational &time, int track, int element) const {
  if (!IsUsingStandardValue(input, track, element)) {
    const NodeKeyframeTrack &key_track = GetKeyframeTracks(input, element).at(track);

    if (key_track.first()->time() >= time) {
      // This time precedes any keyframe, so we just return the first value
      return key_track.first()->value();
    }

    if (key_track.last()->time() <= time) {
      // This time is after any keyframes so we return the last value
      return key_track.last()->value();
    }

    NodeValue::Type type = GetInputDataType(input);

    // If we're here, the time must be somewhere in between the keyframes
    NodeKeyframe *before = nullptr, *after = nullptr;

    int low = 0;
    int high = key_track.size() - 1;
    while (low <= high) {
      int mid = low + (high - low) / 2;
      NodeKeyframe *mid_key = key_track.at(mid);
      NodeKeyframe *next_key = key_track.at(mid + 1);

      if (mid_key->time() <= time && next_key->time() > time) {
        before = mid_key;
        after = next_key;
        break;
      } else if (mid_key->time() < time) {
        low = mid + 1;
      } else {
        high = mid - 1;
      }
    }

    if (before) {
      if (before->time() == time ||
          ((!NodeValue::type_can_be_interpolated(type) || before->type() == NodeKeyframe::kHold) &&
           after->time() > time)) {
        // Time == keyframe time, so value is precise
        return before->value();

      } else if (after->time() == time) {
        // Time == keyframe time, so value is precise
        return after->value();

      } else if (before->time() < time && after->time() > time) {
        // We must interpolate between these keyframes

        double before_val, after_val, interpolated;
        if (type == NodeValue::kRational) {
          before_val = before->value().value<rational>().toDouble();
          after_val = after->value().value<rational>().toDouble();
        } else {
          before_val = before->value().toDouble();
          after_val = after->value().toDouble();
        }

        if (before->type() == NodeKeyframe::kBezier && after->type() == NodeKeyframe::kBezier) {
          // Perform a cubic bezier with two control points
          interpolated =
              Bezier::CubicXtoY(time.toDouble(), Imath::V2d(before->time().toDouble(), before_val),
                                Imath::V2d(before->time().toDouble() + before->valid_bezier_control_out().x(),
                                           before_val + before->valid_bezier_control_out().y()),
                                Imath::V2d(after->time().toDouble() + after->valid_bezier_control_in().x(),
                                           after_val + after->valid_bezier_control_in().y()),
                                Imath::V2d(after->time().toDouble(), after_val));

        } else if (before->type() == NodeKeyframe::kBezier || after->type() == NodeKeyframe::kBezier) {
          // Perform a quadratic bezier with only one control point

          Imath::V2d control_point;

          if (before->type() == NodeKeyframe::kBezier) {
            control_point.x = (before->valid_bezier_control_out().x() + before->time().toDouble());
            control_point.y = (before->valid_bezier_control_out().y() + before_val);
          } else {
            control_point.x = (after->valid_bezier_control_in().x() + after->time().toDouble());
            control_point.y = (after->valid_bezier_control_in().y() + after_val);
          }

          // Interpolate value using quadratic beziers
          interpolated = Bezier::QuadraticXtoY(time.toDouble(), Imath::V2d(before->time().toDouble(), before_val),
                                               control_point, Imath::V2d(after->time().toDouble(), after_val));

        } else {
          // To have arrived here, the keyframes must both be linear
          qreal period_progress =
              (time.toDouble() - before->time().toDouble()) / (after->time().toDouble() - before->time().toDouble());

          interpolated = lerp(before_val, after_val, period_progress);
        }

        if (type == NodeValue::kRational) {
          return QVariant::fromValue(rational::fromDouble(interpolated));
        } else {
          return interpolated;
        }
      }
    } else {
      qWarning() << "Binary search for keyframes failed";
    }
  }

  return GetSplitStandardValueOnTrack(input, track, element);
}

QVariant Node::GetDefaultValue(const QString &input) const {
  NodeValue::Type type = GetInputDataType(input);

  return NodeValue::combine_track_values_into_normal_value(type, GetSplitDefaultValue(input));
}

SplitValue Node::GetSplitDefaultValue(const QString &input) const {
  const Input *i = GetInternalInputData(input);

  if (i) {
    return i->default_value;
  } else {
    ReportInvalidInput("retrieve default value of", input, -1);
    return {};
  }
}

QVariant Node::GetSplitDefaultValueOnTrack(const QString &input, int track) const {
  SplitValue val = GetSplitDefaultValue(input);
  if (track < val.size()) {
    return val.at(track);
  } else {
    return {};
  }
}

void Node::SetDefaultValue(const QString &input, const QVariant &val) {
  NodeValue::Type type = GetInputDataType(input);

  SetSplitDefaultValue(input, NodeValue::split_normal_value_into_track_values(type, val));
}

void Node::SetSplitDefaultValue(const QString &input, const SplitValue &val) {
  Input *i = GetInternalInputData(input);

  if (i) {
    i->default_value = val;
  } else {
    ReportInvalidInput("set default value of", input, -1);
  }
}

void Node::SetSplitDefaultValueOnTrack(const QString &input, const QVariant &val, int track) {
  Input *i = GetInternalInputData(input);

  if (i) {
    if (track < i->default_value.size()) {
      i->default_value[track] = val;
    }
  } else {
    ReportInvalidInput("set default value on track of", input, -1);
  }
}

const QVector<NodeKeyframeTrack> &Node::GetKeyframeTracks(const QString &input, int element) const {
  return GetImmediate(input, element)->keyframe_tracks();
}

QVector<NodeKeyframe *> Node::GetKeyframesAtTime(const QString &input, const rational &time, int element) const {
  NodeInputImmediate *imm = GetImmediate(input, element);

  if (imm) {
    return imm->get_keyframe_at_time(time);
  } else {
    ReportInvalidInput("get keyframes at time from", input, element);
    return {};
  }
}

NodeKeyframe *Node::GetKeyframeAtTimeOnTrack(const QString &input, const rational &time, int track, int element) const {
  NodeInputImmediate *imm = GetImmediate(input, element);

  if (imm) {
    return imm->get_keyframe_at_time_on_track(time, track);
  } else {
    ReportInvalidInput("get keyframe at time on track from", input, element);
    return nullptr;
  }
}

NodeKeyframe::Type Node::GetBestKeyframeTypeForTimeOnTrack(const QString &input, const rational &time, int track,
                                                           int element) const {
  NodeInputImmediate *imm = GetImmediate(input, element);

  if (imm) {
    return imm->get_best_keyframe_type_for_time(time, track);
  } else {
    ReportInvalidInput("get closest keyframe before a time from", input, element);
    return NodeKeyframe::kDefaultType;
  }
}

int Node::GetNumberOfKeyframeTracks(const QString &id) const {
  return NodeValue::get_number_of_keyframe_tracks(GetInputDataType(id));
}

NodeKeyframe *Node::GetEarliestKeyframe(const QString &id, int element) const {
  NodeInputImmediate *imm = GetImmediate(id, element);

  if (imm) {
    return imm->get_earliest_keyframe();
  } else {
    ReportInvalidInput("get earliest keyframe from", id, element);
    return nullptr;
  }
}

NodeKeyframe *Node::GetLatestKeyframe(const QString &id, int element) const {
  NodeInputImmediate *imm = GetImmediate(id, element);

  if (imm) {
    return imm->get_latest_keyframe();
  } else {
    ReportInvalidInput("get latest keyframe from", id, element);
    return nullptr;
  }
}

NodeKeyframe *Node::GetClosestKeyframeBeforeTime(const QString &id, const rational &time, int element) const {
  NodeInputImmediate *imm = GetImmediate(id, element);

  if (imm) {
    return imm->get_closest_keyframe_before_time(time);
  } else {
    ReportInvalidInput("get closest keyframe before a time from", id, element);
    return nullptr;
  }
}

NodeKeyframe *Node::GetClosestKeyframeAfterTime(const QString &id, const rational &time, int element) const {
  NodeInputImmediate *imm = GetImmediate(id, element);

  if (imm) {
    return imm->get_closest_keyframe_after_time(time);
  } else {
    ReportInvalidInput("get closest keyframe after a time from", id, element);
    return nullptr;
  }
}

bool Node::HasKeyframeAtTime(const QString &id, const rational &time, int element) const {
  NodeInputImmediate *imm = GetImmediate(id, element);

  if (imm) {
    return imm->has_keyframe_at_time(time);
  } else {
    ReportInvalidInput("determine if it has a keyframe at a time from", id, element);
    return false;
  }
}

QStringList Node::GetComboBoxStrings(const QString &id) const {
  return GetInputProperty(id, QStringLiteral("combo_str")).toStringList();
}

QVariant Node::GetStandardValue(const QString &id, int element) const {
  NodeValue::Type type = GetInputDataType(id);

  return NodeValue::combine_track_values_into_normal_value(type, GetSplitStandardValue(id, element));
}

SplitValue Node::GetSplitStandardValue(const QString &id, int element) const {
  NodeInputImmediate *imm = GetImmediate(id, element);

  if (imm) {
    return imm->get_split_standard_value();
  } else {
    ReportInvalidInput("get standard value of", id, element);
    return {};
  }
}

QVariant Node::GetSplitStandardValueOnTrack(const QString &input, int track, int element) const {
  NodeInputImmediate *imm = GetImmediate(input, element);

  if (imm) {
    return imm->get_split_standard_value_on_track(track);
  } else {
    ReportInvalidInput("get standard value of", input, element);
    return {};
  }
}

void Node::SetStandardValue(const QString &id, const QVariant &value, int element) {
  NodeValue::Type type = GetInputDataType(id);

  SetSplitStandardValue(id, NodeValue::split_normal_value_into_track_values(type, value), element);
}

void Node::SetSplitStandardValue(const QString &id, const SplitValue &value, int element) {
  NodeInputImmediate *imm = GetImmediate(id, element);

  if (imm) {
    imm->set_split_standard_value(value);

    for (int i = 0; i < value.size(); i++) {
      if (IsUsingStandardValue(id, i, element)) {
        // If this standard value is being used, we need to send a value changed signal
        ParameterValueChanged(id, element, TimeRange(RATIONAL_MIN, RATIONAL_MAX));
        break;
      }
    }
  } else {
    ReportInvalidInput("set standard value of", id, element);
  }
}

void Node::SetSplitStandardValueOnTrack(const QString &id, int track, const QVariant &value, int element) {
  NodeInputImmediate *imm = GetImmediate(id, element);

  if (imm) {
    imm->set_standard_value_on_track(value, track);

    if (IsUsingStandardValue(id, track, element)) {
      // If this standard value is being used, we need to send a value changed signal
      ParameterValueChanged(id, element, TimeRange(RATIONAL_MIN, RATIONAL_MAX));
    }
  } else {
    ReportInvalidInput("set standard value of", id, element);
  }
}

bool Node::InputIsArray(const QString &id) const { return GetInputFlags(id).value() & kInputFlagArray; }

void Node::InputArrayInsert(const QString &id, int index) {
  // Add new input
  ArrayResizeInternal(id, InputArraySize(id) + 1);

  // Move connections down
  InputConnections copied_edges = input_connections();
  for (const auto &copied_edge : std::ranges::reverse_view(copied_edges)) {
    if (copied_edge.first.input() == id && copied_edge.first.element() >= index) {
      // Disconnect this and reconnect it one element down
      NodeInput new_edge = copied_edge.first;
      new_edge.set_element(new_edge.element() + 1);

      DisconnectEdge(copied_edge.second, copied_edge.first);
      ConnectEdge(copied_edge.second, new_edge);
    }
  }

  // Shift values and keyframes up one element
  for (int i = InputArraySize(id) - 1; i > index; i--) {
    CopyValuesOfElement(this, this, id, i - 1, i);
  }

  // Reset value of element we just "inserted"
  ClearElement(id, index);
}

void Node::InputArrayResize(const QString &id, int size) {
  if (InputArraySize(id) == size) {
    return;
  }

  auto *c = new NodeArrayResizeCommand(this, id, size);
  c->redo_now();
  delete c;
}

void Node::InputArrayRemove(const QString &id, int index) {
  // Remove input
  ArrayResizeInternal(id, InputArraySize(id) - 1);

  // Move connections up
  InputConnections copied_edges = input_connections();
  for (const auto &copied_edge : copied_edges) {
    if (copied_edge.first.input() == id && copied_edge.first.element() >= index) {
      // Disconnect this and reconnect it one element up if it's not the element being removed
      DisconnectEdge(copied_edge.second, copied_edge.first);

      if (copied_edge.first.element() > index) {
        NodeInput new_edge = copied_edge.first;
        new_edge.set_element(new_edge.element() - 1);

        ConnectEdge(copied_edge.second, new_edge);
      }
    }
  }

  // Shift values and keyframes down one element
  int arr_sz = InputArraySize(id);
  for (int i = index; i < arr_sz; i++) {
    // Copying ArraySize()+1 is actually legal because immediates are never deleted
    CopyValuesOfElement(this, this, id, i + 1, i);
  }

  // Reset value of last element
  ClearElement(id, arr_sz);
}

int Node::InputArraySize(const QString &id) const {
  const Input *i = GetInternalInputData(id);

  if (i) {
    return i->array_size;
  } else {
    ReportInvalidInput("retrieve array size of", id, -1);
    return 0;
  }
}

void Node::SetValueHintForInput(const QString &input, const ValueHint &hint, int element) {
  value_hints_.insert({input, element}, hint);

  emit InputValueHintChanged(NodeInput(this, input, element));

  InvalidateAll(input, element);
}

const NodeKeyframeTrack &Node::GetTrackFromKeyframe(NodeKeyframe *key) const {
  return GetImmediate(key->input(), key->element())->keyframe_tracks().at(key->track());
}

NodeInputImmediate *Node::GetImmediate(const QString &input, int element) const {
  if (element == -1) {
    return standard_immediates_.value(input, nullptr);
  } else if (array_immediates_.contains(input)) {
    const QVector<NodeInputImmediate *> &imm_arr = array_immediates_.value(input);

    if (element >= 0 && element < imm_arr.size()) {
      return imm_arr.at(element);
    }
  }

  return nullptr;
}

InputFlags Node::GetInputFlags(const QString &input) const {
  const Input *i = GetInternalInputData(input);

  if (i) {
    return i->flags;
  } else {
    ReportInvalidInput("retrieve flags of", input, -1);
    return InputFlags(kInputFlagNormal);
  }
}

void Node::SetInputFlag(const QString &input, InputFlag f, bool on) {
  Input *i = GetInternalInputData(input);

  if (i) {
    if (on) {
      i->flags |= f;
    } else {
      i->flags &= ~f;
    }
    emit InputFlagsChanged(input, i->flags);
  } else {
    ReportInvalidInput("set flags of", input, -1);
  }
}

void Node::Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const {
  // Do nothing
  Q_UNUSED(value)
  Q_UNUSED(globals)
  Q_UNUSED(table)
}

void Node::InvalidateCache(const TimeRange &range, const QString &from, int element, InvalidateCacheOptions options) {
  Q_UNUSED(from)
  Q_UNUSED(element)

  if (AreCachesEnabled()) {
    if (range.in() != range.out()) {
      TimeRange vr = range.Intersected(GetVideoCacheRange());
      if (vr.length() != rational(0)) {
        video_frame_cache()->Invalidate(vr);
        thumbnail_cache()->Invalidate(vr);
      }
      TimeRange ar = range.Intersected(GetAudioCacheRange());
      if (ar.length() != rational(0)) {
        audio_playback_cache()->Invalidate(ar);
        waveform_cache()->Invalidate(ar);
      }
    }
  }

  SendInvalidateCache(range, options);
}

TimeRange Node::InputTimeAdjustment(const QString &, int, const TimeRange &input_time, bool clamp) const {
  // Default behavior is no time adjustment at all
  return input_time;
}

TimeRange Node::OutputTimeAdjustment(const QString &, int, const TimeRange &input_time) const {
  // Default behavior is no time adjustment at all
  return input_time;
}

QVector<Node *> Node::CopyDependencyGraph(const QVector<Node *> &nodes, MultiUndoCommand *command) {
  int nb_nodes = nodes.size();

  QVector<Node *> copies(nb_nodes);

  for (int i = 0; i < nb_nodes; i++) {
    // Create another of the same node
    Node *c = nodes.at(i)->copy();

    // Copy the values, but NOT the connections, since we'll be connecting to our own clones later
    Node::CopyInputs(nodes.at(i), c, false);

    // Add to graph
    Project *graph = nodes.at(i)->parent();
    if (command) {
      command->add_child(new NodeAddCommand(graph, c));
    } else {
      c->setParent(graph);
    }

    // Store in array at the same index as source
    copies[i] = c;
  }

  CopyDependencyGraph(nodes, copies, command);

  return copies;
}

void Node::CopyDependencyGraph(const QVector<Node *> &src, const QVector<Node *> &dst, MultiUndoCommand *command) {
  for (int i = 0; i < src.size(); i++) {
    Node *src_node = src.at(i);
    Node *dst_node = dst.at(i);

    for (auto it = src_node->input_connections_.cbegin(); it != src_node->input_connections_.cend(); it++) {
      // Determine if the connected node is in our src list
      int connection_index = src.indexOf(it->second);

      if (connection_index > -1) {
        // Find the equivalent node in the dst list
        Node *copied_output = dst.at(connection_index);
        NodeInput copied_input = NodeInput(dst_node, it->first.input(), it->first.element());

        if (command) {
          command->add_child(new NodeEdgeAddCommand(copied_output, copied_input));
          command->add_child(new NodeSetValueHintCommand(
              copied_input, src_node->GetValueHintForInput(copied_input.input(), copied_input.element())));
        } else {
          ConnectEdge(copied_output, copied_input);
          copied_input.node()->SetValueHintForInput(
              copied_input.input(), src_node->GetValueHintForInput(copied_input.input(), copied_input.element()),
              copied_input.element());
        }
      }
    }
  }
}

Node *Node::CopyNodeAndDependencyGraphMinusItemsInternal(QMap<Node *, Node *> &created, Node *node,
                                                         MultiUndoCommand *command) {
  // Make a new node of the same type
  Node *copy = node->copy();

  // Add to map
  created.insert(node, copy);

  // Add it to the same graph
  command->add_child(new NodeAddCommand(node->parent(), copy));

  // Copy context children
  const PositionMap &map = node->GetContextPositions();
  for (auto it = map.cbegin(); it != map.cend(); it++) {
    // Add either the copy (if it exists) or the original node to the context
    Node *child;

    if (it.key()->IsItem()) {
      child = it.key();
    } else {
      child = created.value(it.key());
      if (!child) {
        child = CopyNodeAndDependencyGraphMinusItemsInternal(created, it.key(), command);
      }
    }

    command->add_child(new NodeSetPositionCommand(child, copy, it.value()));
  }

  // If this is a group, copy input and output passthroughs
  if (auto *src_group = dynamic_cast<NodeGroup *>(node)) {
    auto *dst_group = dynamic_cast<NodeGroup *>(copy);

    for (const auto &it : src_group->GetInputPassthroughs()) {
      // This node should have been created by the context loop above
      NodeInput input = it.second;
      input.set_node(created.value(input.node()));
      command->add_child(new NodeGroupAddInputPassthrough(dst_group, input, it.first));
    }

    command->add_child(new NodeGroupSetOutputPassthrough(dst_group, created.value(src_group->GetOutputPassthrough())));
  }

  // Copy values to the clone
  CopyInputs(node, copy, false, command);

  // Go through input connections and copy if non-item and connect if item
  for (auto it = node->input_connections_.cbegin(); it != node->input_connections_.cend(); it++) {
    NodeInput input = it->first;
    Node *connected = it->second;
    Node *connected_copy;

    if (connected->IsItem()) {
      // This is an item and we avoid copying those and just connect to them directly
      connected_copy = connected;
    } else {
      // Non-item, we want to clone this too
      connected_copy = created.value(connected, nullptr);
      if (!connected_copy) {
        connected_copy = CopyNodeAndDependencyGraphMinusItemsInternal(created, connected, command);
      }
    }

    NodeInput copied_input = input;
    copied_input.set_node(copy);
    command->add_child(new NodeEdgeAddCommand(connected_copy, copied_input));
    command->add_child(
        new NodeSetValueHintCommand(copied_input, node->GetValueHintForInput(input.input(), input.element())));
  }

  return copy;
}

Node *Node::CopyNodeAndDependencyGraphMinusItems(Node *node, MultiUndoCommand *command) {
  QMap<Node *, Node *> created;

  return CopyNodeAndDependencyGraphMinusItemsInternal(created, node, command);
}

Node *Node::CopyNodeInGraph(Node *node, MultiUndoCommand *command) {
  Node *copy;

  if (OLIVE_CONFIG("SplitClipsCopyNodes").toBool()) {
    copy = Node::CopyNodeAndDependencyGraphMinusItems(node, command);
  } else {
    copy = node->copy();

    command->add_child(new NodeAddCommand(node->parent(), copy));

    CopyInputs(node, copy, true, command);

    const PositionMap &map = node->GetContextPositions();
    for (auto it = map.cbegin(); it != map.cend(); it++) {
      // Add to the context
      command->add_child(new NodeSetPositionCommand(it.key(), copy, it.value()));
    }
  }

  return copy;
}

void Node::SendInvalidateCache(const TimeRange &range, const InvalidateCacheOptions &options) {
  for (const OutputConnection &conn : output_connections_) {
    // Send clear cache signal to the Node
    const NodeInput &in = conn.second;

    in.node()->InvalidateCache(range, in.input(), in.element(), options);
  }
}

void Node::InvalidateAll(const QString &input, int element) {
  InvalidateCache(TimeRange(RATIONAL_MIN, RATIONAL_MAX), input, element);
}

bool Node::Link(Node *a, Node *b) {
  if (a == b || !a || !b) {
    return false;
  }

  if (AreLinked(a, b)) {
    return false;
  }

  a->links_.append(b);
  b->links_.append(a);

  a->LinkChangeEvent();
  b->LinkChangeEvent();

  emit a->LinksChanged();
  emit b->LinksChanged();

  return true;
}

bool Node::Unlink(Node *a, Node *b) {
  if (!AreLinked(a, b)) {
    return false;
  }

  a->links_.removeOne(b);
  b->links_.removeOne(a);

  a->LinkChangeEvent();
  b->LinkChangeEvent();

  emit a->LinksChanged();
  emit b->LinksChanged();

  return true;
}

bool Node::AreLinked(Node *a, Node *b) { return a->links_.contains(b); }

bool Node::Load(QXmlStreamReader *reader, SerializedData *data) {
  uint version = 0;

  XMLAttributeLoop(reader, attr) {
    if (attr.name() == QStringLiteral("ptr")) {
      quintptr ptr = attr.value().toULongLong();
      data->node_ptrs.insert(ptr, this);
    } else if (attr.name() == QStringLiteral("version")) {
      version = attr.value().toUInt();
    }
  }

  Q_UNUSED(version)

  while (XMLReadNextStartElement(reader)) {
    if (reader->name() == QStringLiteral("input")) {
      LoadInput(reader, data);
    } else if (reader->name() == QStringLiteral("label")) {
      this->SetLabel(reader->readElementText());
    } else if (reader->name() == QStringLiteral("color")) {
      this->SetOverrideColor(reader->readElementText().toInt());
    } else if (reader->name() == QStringLiteral("links")) {
      while (XMLReadNextStartElement(reader)) {
        if (reader->name() == QStringLiteral("link")) {
          data->block_links.append({this, reader->readElementText().toULongLong()});
        } else {
          reader->skipCurrentElement();
        }
      }
    } else if (reader->name() == QStringLiteral("custom")) {
      if (!LoadCustom(reader, data)) {
        return false;
      }
    } else if (reader->name() == QStringLiteral("connections")) {
      // Load connections
      while (XMLReadNextStartElement(reader)) {
        if (reader->name() == QStringLiteral("connection")) {
          QString param_id;
          int ele = -1;

          XMLAttributeLoop(reader, attr) {
            if (attr.name() == QStringLiteral("element")) {
              ele = attr.value().toInt();
            } else if (attr.name() == QStringLiteral("input")) {
              param_id = attr.value().toString();
            }
          }

          QString output_node_id;

          while (XMLReadNextStartElement(reader)) {
            if (reader->name() == QStringLiteral("output")) {
              output_node_id = reader->readElementText();
            } else {
              reader->skipCurrentElement();
            }
          }

          data->desired_connections.append({NodeInput(this, param_id, ele), output_node_id.toULongLong()});
        } else {
          reader->skipCurrentElement();
        }
      }
    } else if (reader->name() == QStringLiteral("hints")) {
      while (XMLReadNextStartElement(reader)) {
        if (reader->name() == QStringLiteral("hint")) {
          QString input;
          int element = -1;

          XMLAttributeLoop(reader, attr) {
            if (attr.name() == QStringLiteral("input")) {
              input = attr.value().toString();
            } else if (attr.name() == QStringLiteral("element")) {
              element = attr.value().toInt();
            }
          }

          Node::ValueHint vh;
          if (!vh.load(reader)) {
            return false;
          }
          this->SetValueHintForInput(input, vh, element);
        } else {
          reader->skipCurrentElement();
        }
      }
    } else if (reader->name() == QStringLiteral("context")) {
      while (XMLReadNextStartElement(reader)) {
        if (reader->name() == QStringLiteral("node")) {
          quintptr node_ptr = 0;

          XMLAttributeLoop(reader, attr) {
            if (attr.name() == QStringLiteral("ptr")) {
              node_ptr = attr.value().toULongLong();
            }
          }

          if (node_ptr) {
            Node::Position node_pos;
            if (!node_pos.load(reader)) {
              return false;
            }
            data->positions[this].insert(node_ptr, node_pos);
          } else {
            return false;
          }
        } else {
          reader->skipCurrentElement();
        }
      }
    } else if (reader->name() == QStringLiteral("caches")) {
      while (XMLReadNextStartElement(reader)) {
        if (reader->name() == QStringLiteral("audio")) {
          this->audio_playback_cache()->SetUuid(QUuid::fromString(reader->readElementText()));
        } else if (reader->name() == QStringLiteral("video")) {
          this->video_frame_cache()->SetUuid(QUuid::fromString(reader->readElementText()));
        } else if (reader->name() == QStringLiteral("thumb")) {
          this->thumbnail_cache()->SetUuid(QUuid::fromString(reader->readElementText()));
        } else if (reader->name() == QStringLiteral("waveform")) {
          this->waveform_cache()->SetUuid(QUuid::fromString(reader->readElementText()));
        } else {
          reader->skipCurrentElement();
        }
      }
    } else {
      reader->skipCurrentElement();
    }
  }

  this->LoadFinishedEvent();

  return true;
}

void Node::Save(QXmlStreamWriter *writer) const {
  writer->writeAttribute(QStringLiteral("version"), QString::number(1));
  writer->writeAttribute(QStringLiteral("id"), this->id());
  writer->writeAttribute(QStringLiteral("ptr"), QString::number(reinterpret_cast<quintptr>(this)));

  if (!this->GetLabel().isEmpty()) {
    writer->writeTextElement(QStringLiteral("label"), this->GetLabel());
  }

  if (this->GetOverrideColor() != -1) {
    writer->writeTextElement(QStringLiteral("color"), QString::number(this->GetOverrideColor()));
  }

  foreach (const QString &input, this->inputs()) {
    writer->writeStartElement(QStringLiteral("input"));

    SaveInput(writer, input);

    writer->writeEndElement();  // input
  }

  if (!this->links().empty()) {
    writer->writeStartElement(QStringLiteral("links"));
    foreach (Node *link, this->links()) {
      writer->writeTextElement(QStringLiteral("link"), QString::number(reinterpret_cast<quintptr>(link)));
    }
    writer->writeEndElement();  // links
  }

  if (!this->input_connections().empty()) {
    writer->writeStartElement(QStringLiteral("connections"));
    for (const auto &it : this->input_connections()) {
      writer->writeStartElement(QStringLiteral("connection"));

      writer->writeAttribute(QStringLiteral("input"), it.first.input());
      writer->writeAttribute(QStringLiteral("element"), QString::number(it.first.element()));

      writer->writeTextElement(QStringLiteral("output"), QString::number(reinterpret_cast<quintptr>(it.second)));

      writer->writeEndElement();  // connection
    }
    writer->writeEndElement();  // connections
  }

  if (!this->GetValueHints().empty()) {
    writer->writeStartElement(QStringLiteral("hints"));
    for (auto it = this->GetValueHints().cbegin(); it != this->GetValueHints().cend(); it++) {
      writer->writeStartElement(QStringLiteral("hint"));

      writer->writeAttribute(QStringLiteral("input"), it.key().input);
      writer->writeAttribute(QStringLiteral("element"), QString::number(it.key().element));

      it.value().save(writer);

      writer->writeEndElement();  // hint
    }
    writer->writeEndElement();  // hints
  }

  const Node::PositionMap &map = this->GetContextPositions();

  if (!map.isEmpty()) {
    writer->writeStartElement(QStringLiteral("context"));

    for (auto jt = map.cbegin(); jt != map.cend(); jt++) {
      writer->writeStartElement(QStringLiteral("node"));
      writer->writeAttribute(QStringLiteral("ptr"), QString::number(reinterpret_cast<quintptr>(jt.key())));
      jt.value().save(writer);
      writer->writeEndElement();  // node
    }

    writer->writeEndElement();  // context
  }

  writer->writeStartElement(QStringLiteral("caches"));

  writer->writeTextElement(QStringLiteral("audio"), this->audio_playback_cache()->GetUuid().toString());
  writer->writeTextElement(QStringLiteral("video"), this->video_frame_cache()->GetUuid().toString());
  writer->writeTextElement(QStringLiteral("thumb"), this->thumbnail_cache()->GetUuid().toString());
  writer->writeTextElement(QStringLiteral("waveform"), this->waveform_cache()->GetUuid().toString());

  writer->writeEndElement();  // caches

  writer->writeStartElement(QStringLiteral("custom"));

  SaveCustom(writer);

  writer->writeEndElement();  // custom
}

bool Node::LoadCustom(QXmlStreamReader *reader, SerializedData *data) {
  reader->skipCurrentElement();
  return true;
}

void Node::PostLoadEvent(SerializedData *data) {
  // Resolve positions
  const QMap<quintptr, Node::Position> &positions = data->positions.value(this);

  for (auto jt = positions.cbegin(); jt != positions.cend(); jt++) {
    Node *n = data->node_ptrs.value(jt.key());
    if (n) {
      this->SetNodePositionInContext(n, jt.value());
    }
  }
}

bool Node::LoadInput(QXmlStreamReader *reader, SerializedData *data) {
  if (dynamic_cast<NodeGroup *>(this)) {
    // Ignore input of group
    reader->skipCurrentElement();
    return true;
  }

  QString param_id;

  XMLAttributeLoop(reader, attr) {
    if (attr.name() == QStringLiteral("id")) {
      param_id = attr.value().toString();

      break;
    }
  }

  if (param_id.isEmpty()) {
    qWarning() << "Failed to load parameter with missing ID";
    reader->skipCurrentElement();
    return false;
  }

  if (!this->HasInputWithID(param_id)) {
    qWarning() << "Failed to load parameter that didn't exist:" << param_id;
    reader->skipCurrentElement();
    return false;
  }

  while (XMLReadNextStartElement(reader)) {
    if (reader->name() == QStringLiteral("primary")) {
      // Load primary immediate
      if (!LoadImmediate(reader, param_id, -1, data)) {
        return false;
      }
    } else if (reader->name() == QStringLiteral("subelements")) {
      // Load subelements
      XMLAttributeLoop(reader, attr) {
        if (attr.name() == QStringLiteral("count")) {
          this->InputArrayResize(param_id, attr.value().toInt());
        }
      }

      int element_counter = 0;

      while (XMLReadNextStartElement(reader)) {
        if (reader->name() == QStringLiteral("element")) {
          if (!LoadImmediate(reader, param_id, element_counter, data)) {
            return false;
          }

          element_counter++;
        } else {
          reader->skipCurrentElement();
        }
      }
    } else {
      reader->skipCurrentElement();
    }
  }

  return true;
}

void Node::SaveInput(QXmlStreamWriter *writer, const QString &id) const {
  writer->writeAttribute(QStringLiteral("id"), id);

  writer->writeStartElement(QStringLiteral("primary"));

  SaveImmediate(writer, id, -1);

  writer->writeEndElement();  // primary

  int arr_sz = this->InputArraySize(id);

  if (arr_sz > 0) {
    writer->writeStartElement(QStringLiteral("subelements"));

    writer->writeAttribute(QStringLiteral("count"), QString::number(arr_sz));

    for (int i = 0; i < arr_sz; i++) {
      writer->writeStartElement(QStringLiteral("element"));

      SaveImmediate(writer, id, i);

      writer->writeEndElement();  // element
    }

    writer->writeEndElement();  // subelements
  }
}

bool Node::LoadImmediate(QXmlStreamReader *reader, const QString &input, int element, SerializedData *data) {
  NodeValue::Type data_type = this->GetInputDataType(input);

  // HACK: SubtitleParams contain the actual subtitle data, so loading/replacing it will overwrite
  //       the valid subtitles. We hack around it by simply skipping loading subtitles, we'll see
  //       if this ends up being an issue in the future.
  if (data_type == NodeValue::kSubtitleParams) {
    reader->skipCurrentElement();
    return true;
  }

  while (XMLReadNextStartElement(reader)) {
    if (reader->name() == QStringLiteral("standard")) {
      // Load standard value
      int val_index = 0;

      while (XMLReadNextStartElement(reader)) {
        if (reader->name() == QStringLiteral("track")) {
          QVariant value_on_track;

          if (data_type == NodeValue::kVideoParams) {
            VideoParams vp;
            vp.Load(reader);
            value_on_track = QVariant::fromValue(vp);
          } else if (data_type == NodeValue::kAudioParams) {
            AudioParams ap = TypeSerializer::LoadAudioParams(reader);
            value_on_track = QVariant::fromValue(ap);
          } else {
            QString value_text = reader->readElementText();

            if (!value_text.isEmpty()) {
              value_on_track = NodeValue::StringToValue(data_type, value_text, true);
            }
          }

          this->SetSplitStandardValueOnTrack(input, val_index, value_on_track, element);

          val_index++;
        } else {
          reader->skipCurrentElement();
        }
      }
    } else if (reader->name() == QStringLiteral("keyframing")) {
      bool k = reader->readElementText().toInt();
      if (this->IsInputKeyframable(input)) {
        this->SetInputIsKeyframing(input, k, element);
      }
    } else if (reader->name() == QStringLiteral("keyframes")) {
      int track = 0;

      while (XMLReadNextStartElement(reader)) {
        if (reader->name() == QStringLiteral("track")) {
          while (XMLReadNextStartElement(reader)) {
            if (reader->name() == QStringLiteral("key")) {
              auto *key = new NodeKeyframe();
              key->set_input(input);
              key->set_element(element);
              key->set_track(track);

              key->load(reader, data_type);
              key->setParent(this);
            } else {
              reader->skipCurrentElement();
            }
          }

          track++;
        } else {
          reader->skipCurrentElement();
        }
      }
    } else if (reader->name() == QStringLiteral("csinput")) {
      this->SetInputProperty(input, QStringLiteral("col_input"), reader->readElementText());
    } else if (reader->name() == QStringLiteral("csdisplay")) {
      this->SetInputProperty(input, QStringLiteral("col_display"), reader->readElementText());
    } else if (reader->name() == QStringLiteral("csview")) {
      this->SetInputProperty(input, QStringLiteral("col_view"), reader->readElementText());
    } else if (reader->name() == QStringLiteral("cslook")) {
      this->SetInputProperty(input, QStringLiteral("col_look"), reader->readElementText());
    } else {
      reader->skipCurrentElement();
    }
  }

  return true;
}

void Node::SaveImmediate(QXmlStreamWriter *writer, const QString &input, int element) const {
  bool is_keyframing = this->IsInputKeyframing(input, element);

  if (this->IsInputKeyframable(input)) {
    writer->writeTextElement(QStringLiteral("keyframing"), QString::number(is_keyframing));
  }

  NodeValue::Type data_type = this->GetInputDataType(input);

  // Write standard value
  writer->writeStartElement(QStringLiteral("standard"));

  foreach (const QVariant &v, this->GetSplitStandardValue(input, element)) {
    writer->writeStartElement(QStringLiteral("track"));

    if (data_type == NodeValue::kVideoParams) {
      v.value<VideoParams>().Save(writer);
    } else if (data_type == NodeValue::kAudioParams) {
      TypeSerializer::SaveAudioParams(writer, v.value<AudioParams>());
    } else {
      writer->writeCharacters(NodeValue::ValueToString(data_type, v, true));
    }

    writer->writeEndElement();  // track
  }

  writer->writeEndElement();  // standard

  // Write keyframes
  if (is_keyframing) {
    writer->writeStartElement(QStringLiteral("keyframes"));

    for (const NodeKeyframeTrack &track : this->GetKeyframeTracks(input, element)) {
      writer->writeStartElement(QStringLiteral("track"));

      for (NodeKeyframe *key : track) {
        writer->writeStartElement(QStringLiteral("key"));

        key->save(writer, data_type);

        writer->writeEndElement();  // key
      }

      writer->writeEndElement();  // track
    }

    writer->writeEndElement();  // keyframes
  }

  if (data_type == NodeValue::kColor) {
    // Save color management information
    writer->writeTextElement(QStringLiteral("csinput"),
                             this->GetInputProperty(input, QStringLiteral("col_input")).toString());
    writer->writeTextElement(QStringLiteral("csdisplay"),
                             this->GetInputProperty(input, QStringLiteral("col_display")).toString());
    writer->writeTextElement(QStringLiteral("csview"),
                             this->GetInputProperty(input, QStringLiteral("col_view")).toString());
    writer->writeTextElement(QStringLiteral("cslook"),
                             this->GetInputProperty(input, QStringLiteral("col_look")).toString());
  }
}

void Node::InsertInput(const QString &id, NodeValue::Type type, const QVariant &default_value, InputFlags flags,
                       int index) {
  if (id.isEmpty()) {
    qWarning() << "Rejected adding input with an empty ID on node" << this->id();
    return;
  }

  if (HasParamWithID(id)) {
    qWarning() << "Failed to add input to node" << this->id() << "- param with ID" << id << "already exists";
    return;
  }

  Node::Input i;

  i.type = type;
  i.default_value = NodeValue::split_normal_value_into_track_values(type, default_value);
  i.flags = flags;
  i.array_size = 0;

  input_ids_.insert(index, id);
  input_data_.insert(index, i);

  if (!standard_immediates_.value(id, nullptr)) {
    standard_immediates_.insert(id, CreateImmediate(id));
  }

  emit InputAdded(id);
}

void Node::RemoveInput(const QString &id) {
  int index = input_ids_.indexOf(id);

  if (index == -1) {
    ReportInvalidInput("remove", id, -1);
    return;
  }

  input_ids_.removeAt(index);
  input_data_.removeAt(index);

  emit InputRemoved(id);
}

void Node::ReportInvalidInput(const char *attempted_action, const QString &id, int element) const {
  qWarning() << "Failed to" << attempted_action << "parameter" << id << "element" << element << "in node" << this->id()
             << "- input doesn't exist";
}

NodeInputImmediate *Node::CreateImmediate(const QString &input) {
  const Input *i = GetInternalInputData(input);

  if (i) {
    return new NodeInputImmediate(i->type, i->default_value);
  } else {
    ReportInvalidInput("create immediate", input, -1);
    return nullptr;
  }
}

void Node::ArrayResizeInternal(const QString &id, int size) {
  Input *imm = GetInternalInputData(id);

  if (!imm) {
    ReportInvalidInput("set array size", id, -1);
    return;
  }

  if (imm->array_size != size) {
    // Update array size
    if (imm->array_size < size) {
      // Size is larger, create any immediates that don't exist
      QVector<NodeInputImmediate *> &subinputs = array_immediates_[id];
      for (int i = subinputs.size(); i < size; i++) {
        subinputs.append(CreateImmediate(id));
      }

      // Note that we do not delete any immediates when decreasing size since the user might still
      // want that data. Therefore it's important to note that array_size_ does NOT necessarily
      // equal subinputs_.size()
    }

    int old_sz = imm->array_size;
    imm->array_size = size;
    emit InputArraySizeChanged(id, old_sz, size);
    ParameterValueChanged(id, -1, TimeRange(RATIONAL_MIN, RATIONAL_MAX));
  }
}

QString Node::GetConnectCommandString(Node *output, const NodeInput &input) {
  return tr("Connected %1 to %2 - %3")
      .arg(output->GetLabelAndName(), input.node()->GetLabelAndName(), input.GetInputName());
}

QString Node::GetDisconnectCommandString(Node *output, const NodeInput &input) {
  return tr("Disconnected %1 from %2 - %3")
      .arg(output->GetLabelAndName(), input.node()->GetLabelAndName(), input.GetInputName());
}

int Node::GetInternalInputArraySize(const QString &input) { return array_immediates_.value(input).size(); }

void FindWaysNodeArrivesHereRecursively(const Node *output, const Node *input, QVector<NodeInput> &v) {
  for (const auto &it : input->input_connections()) {
    if (it.second == output) {
      v.append(it.first);
    } else {
      FindWaysNodeArrivesHereRecursively(output, it.second, v);
    }
  }
}

QVector<NodeInput> Node::FindWaysNodeArrivesHere(const Node *output) const {
  QVector<NodeInput> v;

  FindWaysNodeArrivesHereRecursively(output, this, v);

  return v;
}

void Node::SetInputName(const QString &id, const QString &name) {
  Input *i = GetInternalInputData(id);

  if (i) {
    i->human_name = name;

    emit InputNameChanged(id, name);
  } else {
    ReportInvalidInput("set name of", id, -1);
  }
}

const QString &Node::GetLabel() const { return label_; }

void Node::SetLabel(const QString &s) {
  if (label_ != s) {
    label_ = s;

    emit LabelChanged(label_);
  }
}

QString Node::GetLabelAndName() const {
  if (GetLabel().isEmpty()) {
    return Name();
  } else {
    return tr("%1 (%2)").arg(GetLabel(), Name());
  }
}

QString Node::GetLabelOrName() const {
  if (GetLabel().isEmpty()) {
    return Name();
  }
  return GetLabel();
}

void Node::CopyInputs(const Node *source, Node *destination, bool include_connections, MultiUndoCommand *command) {
  Q_ASSERT(source->id() == destination->id());

  foreach (const QString &input, source->inputs()) {
    // NOTE: This assert is to ensure that inputs in the source also exist in the destination, which
    //       they should. If they don't and you hit this assert, check if you're handling group
    //       passthroughs correctly.
    Q_ASSERT(destination->HasInputWithID(input));

    CopyInput(source, destination, input, include_connections, true, command);
  }

  if (command) {
    command->add_child(new NodeRenameCommand(destination, source->GetLabel()));
  } else {
    destination->SetLabel(source->GetLabel());
  }

  if (command) {
    command->add_child(new NodeOverrideColorCommand(destination, source->GetOverrideColor()));
  } else {
    destination->SetOverrideColor(source->GetOverrideColor());
  }
}

void Node::CopyInput(const Node *src, Node *dst, const QString &input, bool include_connections, bool traverse_arrays,
                     MultiUndoCommand *command) {
  Q_ASSERT(src->id() == dst->id());

  CopyValuesOfElement(src, dst, input, -1, command);

  // Copy array size
  if (src->InputIsArray(input) && traverse_arrays) {
    int src_array_sz = src->InputArraySize(input);

    for (int i = 0; i < src_array_sz; i++) {
      CopyValuesOfElement(src, dst, input, i, command);
    }
  }

  // Copy connections
  if (include_connections) {
    // Copy all connections
    for (const auto &it : src->input_connections()) {
      if (!traverse_arrays && it.first.element() != -1) {
        continue;
      }

      auto conn_output = it.second;
      NodeInput conn_input(dst, input, it.first.element());

      if (command) {
        command->add_child(new NodeEdgeAddCommand(conn_output, conn_input));
      } else {
        ConnectEdge(conn_output, conn_input);
      }
    }
  }
}

void Node::CopyValuesOfElement(const Node *src, Node *dst, const QString &input, int src_element, int dst_element,
                               MultiUndoCommand *command) {
  if (dst_element >= dst->GetInternalInputArraySize(input)) {
    qDebug() << "Ignored destination element that was out of array bounds";
    return;
  }

  NodeInput dst_input(dst, input, dst_element);

  // Copy standard value
  SplitValue standard = src->GetSplitStandardValue(input, src_element);
  if (command) {
    command->add_child(new NodeParamSetSplitStandardValueCommand(dst_input, standard));
  } else {
    dst->SetSplitStandardValue(input, standard, dst_element);
  }

  // Copy keyframes
  if (NodeInputImmediate *immediate = dst->GetImmediate(input, dst_element)) {
    if (command) {
      command->add_child(new NodeImmediateRemoveAllKeyframesCommand(immediate));
    } else {
      immediate->delete_all_keyframes();
    }
  }

  for (const NodeKeyframeTrack &track : src->GetImmediate(input, src_element)->keyframe_tracks()) {
    for (NodeKeyframe *key : track) {
      NodeKeyframe *copy = key->copy(dst_element, command ? nullptr : dst);
      if (command) {
        command->add_child(new NodeParamInsertKeyframeCommand(dst, copy));
      }
    }
  }

  // Copy keyframing state
  if (src->IsInputKeyframable(input)) {
    bool is_keying = src->IsInputKeyframing(input, src_element);
    if (command) {
      command->add_child(new NodeParamSetKeyframingCommand(dst_input, is_keying));
    } else {
      dst->SetInputIsKeyframing(input, is_keying, dst_element);
    }
  }

  // If this is the root of an array, copy the array size
  if (src_element == -1 && dst_element == -1) {
    int array_sz = src->InputArraySize(input);
    if (command) {
      command->add_child(new NodeArrayResizeCommand(dst, input, array_sz));
    } else {
      dst->ArrayResizeInternal(input, array_sz);
    }
  }

  // Copy value hint
  Node::ValueHint vh = src->GetValueHintForInput(input, src_element);
  if (command) {
    command->add_child(new NodeSetValueHintCommand(dst_input, vh));
  } else {
    dst->SetValueHintForInput(input, vh, dst_element);
  }
}

void GetDependenciesRecursively(QVector<Node *> &list, const Node *node, bool traverse, bool exclusive_only) {
  for (const auto &it : node->input_connections()) {
    Node *connected_node = it.second;

    if (!exclusive_only || !connected_node->IsItem()) {
      if (!list.contains(connected_node)) {
        list.append(connected_node);

        if (traverse) {
          GetDependenciesRecursively(list, connected_node, traverse, exclusive_only);
        }
      }
    }
  }
}

/**
 * @brief Recursively collects dependencies of Node `n` and appends them to QList `list`
 *
 * @param traverse
 *
 * TRUE to recursively traverse each node for a complete dependency graph. FALSE to return only the immediate
 * dependencies.
 */
QVector<Node *> Node::GetDependenciesInternal(bool traverse, bool exclusive_only) const {
  QVector<Node *> list;

  GetDependenciesRecursively(list, this, traverse, exclusive_only);

  return list;
}

QVector<Node *> Node::GetDependencies() const { return GetDependenciesInternal(true, false); }

QVector<Node *> Node::GetExclusiveDependencies() const { return GetDependenciesInternal(true, true); }

QVector<Node *> Node::GetImmediateDependencies() const { return GetDependenciesInternal(false, false); }

ShaderCode Node::GetShaderCode(const ShaderRequest &request) const { return ShaderCode(QString(), QString()); }

void Node::ProcessSamples(const NodeValueRow &, const SampleBuffer &, SampleBuffer &, int) const {}

void Node::GenerateFrame(FramePtr frame, const GenerateJob &job) const {
  Q_UNUSED(frame)
  Q_UNUSED(job)
}

bool Node::InputsFrom(Node *n, bool recursively) const {
  for (const auto &input_connection : input_connections_) {
    Node *connected = input_connection.second;

    if (connected == n) {
      return true;
    } else if (recursively && connected->InputsFrom(n, recursively)) {
      return true;
    }
  }

  return false;
}

bool Node::InputsFrom(const QString &id, bool recursively) const {
  for (const auto &input_connection : input_connections_) {
    Node *connected = input_connection.second;

    if (connected->id() == id) {
      return true;
    } else if (recursively && connected->InputsFrom(id, recursively)) {
      return true;
    }
  }

  return false;
}

void Node::DisconnectAll() {
  // Disconnect inputs (copy map since internal map will change as we disconnect)
  InputConnections copy = input_connections_;
  for (const auto &it : copy) {
    DisconnectEdge(it.second, it.first);
  }

  while (!output_connections_.empty()) {
    OutputConnection conn = output_connections_.back();
    DisconnectEdge(conn.first, conn.second);
  }
}

QString Node::GetCategoryName(const CategoryID &c) {
  switch (c) {
    case kCategoryOutput:
      return tr("Output");
    case kCategoryDistort:
      return tr("Distort");
    case kCategoryMath:
      return tr("Math");
    case kCategoryKeying:
      return tr("Keying");
    case kCategoryColor:
      return tr("Color");
    case kCategoryFilter:
      return tr("Filter");
    case kCategoryTimeline:
      return tr("Timeline");
    case kCategoryGenerator:
      return tr("Generator");
    case kCategoryTransition:
      return tr("Transition");
    case kCategoryProject:
      return tr("Project");
    case kCategoryTime:
      return tr("Time");
    case kCategoryUnknown:
    case kCategoryCount:
      break;
  }

  return tr("Uncategorized");
}

TimeRange Node::TransformTimeTo(TimeRange time, Node *target, TransformTimeDirection dir, int path_index) {
  Node *from = this;
  Node *to = target;

  if (dir == kTransformTowardsInput) {
    std::swap(from, to);
  }

  std::list<NodeInput> path = FindPath(from, to, path_index);

  if (!path.empty()) {
    if (dir == kTransformTowardsInput) {
      for (const auto &i : std::ranges::reverse_view(path)) {
        time = i.node()->InputTimeAdjustment(i.input(), i.element(), time, false);
      }
    } else {
      // Traverse in output direction
      for (const auto &i : path) {
        time = i.node()->OutputTimeAdjustment(i.input(), i.element(), time);
      }
    }
  }

  return time;
}

void Node::ParameterValueChanged(const QString &input, int element, const TimeRange &range) {
  InputValueChangedEvent(input, element);

  emit ValueChanged(NodeInput(this, input, element), range);

  if (GetInputFlags(input) & kInputFlagIgnoreInvalidations) {
    return;
  }

  InvalidateCache(range, input, element);
}

TimeRange Node::GetRangeAffectedByKeyframe(NodeKeyframe *key) const {
  const NodeKeyframeTrack &key_track = GetTrackFromKeyframe(key);
  int keyframe_index = key_track.indexOf(key);

  TimeRange range = GetRangeAroundIndex(key->input(), keyframe_index, key->track(), key->element());

  // If a previous key exists and it's a hold, we don't need to invalidate those frames
  if (key_track.size() > 1 && keyframe_index > 0 && key_track.at(keyframe_index - 1)->type() == NodeKeyframe::kHold) {
    range.set_in(key->time());
  }

  return range;
}

TimeRange Node::GetRangeAroundIndex(const QString &input, int index, int track, int element) const {
  rational range_begin = RATIONAL_MIN;
  rational range_end = RATIONAL_MAX;

  const NodeKeyframeTrack &key_track = GetImmediate(input, element)->keyframe_tracks().at(track);

  if (key_track.size() > 1) {
    if (index > 0) {
      // If this is not the first key, we'll need to limit it to the key just before
      range_begin = key_track.at(index - 1)->time();
    }
    if (index < key_track.size() - 1) {
      // If this is not the last key, we'll need to limit it to the key just after
      range_end = key_track.at(index + 1)->time();
    }
  }

  return TimeRange(range_begin, range_end);
}

void Node::ClearElement(const QString &input, int index) {
  GetImmediate(input, index)->delete_all_keyframes();

  if (IsInputKeyframable(input)) {
    SetInputIsKeyframing(input, false, index);
  }

  SetSplitStandardValue(input, GetSplitDefaultValue(input), index);
}

void Node::InputValueChangedEvent(const QString &input, int element) {
  Q_UNUSED(input)
  Q_UNUSED(element)
}

void Node::InputConnectedEvent(const QString &input, int element, Node *output) {
  Q_UNUSED(input)
  Q_UNUSED(element)
  Q_UNUSED(output)
}

void Node::InputDisconnectedEvent(const QString &input, int element, Node *output) {
  Q_UNUSED(input)
  Q_UNUSED(element)
  Q_UNUSED(output)
}

void Node::OutputConnectedEvent(const NodeInput &input) { Q_UNUSED(input) }

void Node::OutputDisconnectedEvent(const NodeInput &input) { Q_UNUSED(input) }

void Node::childEvent(QChildEvent *event) {
  super::childEvent(event);

  if (auto *key = dynamic_cast<NodeKeyframe *>(event->child())) {
    NodeInput i(this, key->input(), key->element());

    if (event->type() == QEvent::ChildAdded) {
      GetImmediate(key->input(), key->element())->insert_keyframe(key);

      connect(key, &NodeKeyframe::TimeChanged, this, &Node::InvalidateFromKeyframeTimeChange);
      connect(key, &NodeKeyframe::ValueChanged, this, &Node::InvalidateFromKeyframeValueChange);
      connect(key, &NodeKeyframe::TypeChanged, this, &Node::InvalidateFromKeyframeTypeChanged);
      connect(key, &NodeKeyframe::BezierControlInChanged, this, &Node::InvalidateFromKeyframeBezierInChange);
      connect(key, &NodeKeyframe::BezierControlOutChanged, this, &Node::InvalidateFromKeyframeBezierOutChange);

      emit KeyframeAdded(key);
      ParameterValueChanged(i, GetRangeAffectedByKeyframe(key));
    } else if (event->type() == QEvent::ChildRemoved) {
      TimeRange time_affected = GetRangeAffectedByKeyframe(key);

      disconnect(key, &NodeKeyframe::TimeChanged, this, &Node::InvalidateFromKeyframeTimeChange);
      disconnect(key, &NodeKeyframe::ValueChanged, this, &Node::InvalidateFromKeyframeValueChange);
      disconnect(key, &NodeKeyframe::TypeChanged, this, &Node::InvalidateFromKeyframeTypeChanged);
      disconnect(key, &NodeKeyframe::BezierControlInChanged, this, &Node::InvalidateFromKeyframeBezierInChange);
      disconnect(key, &NodeKeyframe::BezierControlOutChanged, this, &Node::InvalidateFromKeyframeBezierOutChange);

      emit KeyframeRemoved(key);

      GetImmediate(key->input(), key->element())->remove_keyframe(key);
      ParameterValueChanged(i, time_affected);
    }
  } else if (auto *gizmo = dynamic_cast<NodeGizmo *>(event->child())) {
    if (event->type() == QEvent::ChildAdded) {
      gizmos_.append(gizmo);
    } else if (event->type() == QEvent::ChildRemoved) {
      gizmos_.removeOne(gizmo);
    }
  }
}

void Node::InvalidateFromKeyframeBezierInChange() {
  auto *key = dynamic_cast<NodeKeyframe *>(sender());
  const NodeKeyframeTrack &track = GetTrackFromKeyframe(key);
  int keyframe_index = track.indexOf(key);

  rational start = RATIONAL_MIN;
  rational end = key->time();

  if (keyframe_index > 0) {
    start = track.at(keyframe_index - 1)->time();
  }

  ParameterValueChanged(key->key_track_ref().input(), TimeRange(start, end));
}

void Node::InvalidateFromKeyframeBezierOutChange() {
  auto *key = dynamic_cast<NodeKeyframe *>(sender());
  const NodeKeyframeTrack &track = GetTrackFromKeyframe(key);
  int keyframe_index = track.indexOf(key);

  rational start = key->time();
  rational end = RATIONAL_MAX;

  if (keyframe_index < track.size() - 1) {
    end = track.at(keyframe_index + 1)->time();
  }

  ParameterValueChanged(key->key_track_ref().input(), TimeRange(start, end));
}

void Node::InvalidateFromKeyframeTimeChange() {
  auto *key = dynamic_cast<NodeKeyframe *>(sender());
  NodeInputImmediate *immediate = GetImmediate(key->input(), key->element());
  TimeRange original_range = GetRangeAffectedByKeyframe(key);

  TimeRangeList invalidate_range;
  invalidate_range.insert(original_range);

  if (!(original_range.in() < key->time() && original_range.out() > key->time())) {
    // This keyframe needs resorting, store it and remove it from the list
    immediate->remove_keyframe(key);

    // Automatically insertion sort
    immediate->insert_keyframe(key);

    // Invalidate new area that the keyframe has been moved to
    invalidate_range.insert(GetRangeAffectedByKeyframe(key));
  }

  // Invalidate entire area surrounding the keyframe (either where it currently is, or where it used to be before it
  // was resorted in the if block above)
  foreach (const TimeRange &r, invalidate_range) {
    ParameterValueChanged(key->key_track_ref().input(), r);
  }

  emit KeyframeTimeChanged(key);
}

void Node::InvalidateFromKeyframeValueChange() {
  auto *key = dynamic_cast<NodeKeyframe *>(sender());
  ParameterValueChanged(key->key_track_ref().input(), GetRangeAffectedByKeyframe(key));

  emit KeyframeValueChanged(key);
}

void Node::InvalidateFromKeyframeTypeChanged() {
  auto *key = dynamic_cast<NodeKeyframe *>(sender());
  const NodeKeyframeTrack &track = GetTrackFromKeyframe(key);

  if (track.size() == 1) {
    // If there are no other frames, the interpolation won't do anything
    return;
  }

  // Invalidate entire range
  ParameterValueChanged(key->key_track_ref().input(),
                        GetRangeAroundIndex(key->input(), track.indexOf(key), key->track(), key->element()));

  emit KeyframeTypeChanged(key);
}

void Node::SetValueAtTime(const NodeInput &input, const rational &time, const QVariant &value, int track,
                          MultiUndoCommand *command, bool insert_on_all_tracks_if_no_key) {
  if (input.IsKeyframing()) {
    rational node_time = time;

    NodeKeyframe *existing_key = input.GetKeyframeAtTimeOnTrack(node_time, track);

    if (existing_key) {
      command->add_child(new NodeParamSetKeyframeValueCommand(existing_key, value));
    } else {
      // No existing key, create a new one
      int nb_tracks = NodeValue::get_number_of_keyframe_tracks(input.node()->GetInputDataType(input.input()));
      for (int i = 0; i < nb_tracks; i++) {
        QVariant track_value;

        if (i == track) {
          track_value = value;
        } else if (!insert_on_all_tracks_if_no_key) {
          continue;
        } else {
          track_value = input.node()->GetSplitValueAtTimeOnTrack(input.input(), node_time, i, input.element());
        }

        auto *new_key = new NodeKeyframe(
            node_time, track_value,
            input.node()->GetBestKeyframeTypeForTimeOnTrack(NodeKeyframeTrackReference(input, i), node_time), i,
            input.element(), input.input());

        command->add_child(new NodeParamInsertKeyframeCommand(input.node(), new_key));
      }
    }
  } else {
    command->add_child(new NodeParamSetStandardValueCommand(NodeKeyframeTrackReference(input, track), value));
  }
}

bool FindPathInternal(std::list<NodeInput> &vec, Node *from, Node *to, int &path_index) {
  for (const auto &it : from->output_connections()) {
    const NodeInput &next = it.second;

    vec.push_back(next);

    if (next.node() == to) {
      // Found a path! Determine if it's the index we want
      if (path_index == 0) {
        // It is!
        return true;
      } else {
        // It isn't, keep looking...
        path_index--;
      }
    }

    if (FindPathInternal(vec, next.node(), to, path_index)) {
      return true;
    }

    vec.pop_back();
  }

  return false;
}

std::list<NodeInput> Node::FindPath(Node *from, Node *to, int path_index) {
  std::list<NodeInput> v;

  FindPathInternal(v, from, to, path_index);

  return v;
}

bool Node::ValueHint::load(QXmlStreamReader *reader) {
  uint version = 0;
  XMLAttributeLoop(reader, attr) { version = attr.value().toUInt(); }

  Q_UNUSED(version)

  while (XMLReadNextStartElement(reader)) {
    if (reader->name() == QStringLiteral("types")) {
      QVector<NodeValue::Type> types;
      while (XMLReadNextStartElement(reader)) {
        if (reader->name() == QStringLiteral("type")) {
          types.append(static_cast<NodeValue::Type>(reader->readElementText().toInt()));
        } else {
          reader->skipCurrentElement();
        }
      }
      this->set_type(types);
    } else if (reader->name() == QStringLiteral("index")) {
      this->set_index(reader->readElementText().toInt());
    } else if (reader->name() == QStringLiteral("tag")) {
      this->set_tag(reader->readElementText());
    } else {
      reader->skipCurrentElement();
    }
  }

  return true;
}

void Node::ValueHint::save(QXmlStreamWriter *writer) const {
  writer->writeAttribute(QStringLiteral("version"), QString::number(1));

  writer->writeStartElement(QStringLiteral("types"));

  for (auto it : this->types()) {
    writer->writeTextElement(QStringLiteral("type"), QString::number(it));
  }

  writer->writeEndElement();  // types

  writer->writeTextElement(QStringLiteral("index"), QString::number(this->index()));

  writer->writeTextElement(QStringLiteral("tag"), this->tag());
}

bool Node::Position::load(QXmlStreamReader *reader) {
  bool got_pos_x = false;
  bool got_pos_y = false;

  while (XMLReadNextStartElement(reader)) {
    if (reader->name() == QStringLiteral("x")) {
      this->position.setX(reader->readElementText().toDouble());
      got_pos_x = true;
    } else if (reader->name() == QStringLiteral("y")) {
      this->position.setY(reader->readElementText().toDouble());
      got_pos_y = true;
    } else if (reader->name() == QStringLiteral("expanded")) {
      this->expanded = reader->readElementText().toInt();
    } else {
      reader->skipCurrentElement();
    }
  }

  return got_pos_x && got_pos_y;
}

void Node::Position::save(QXmlStreamWriter *writer) const {
  writer->writeTextElement(QStringLiteral("x"), QString::number(this->position.x()));
  writer->writeTextElement(QStringLiteral("y"), QString::number(this->position.y()));
  writer->writeTextElement(QStringLiteral("expanded"), QString::number(this->expanded));
}

}  // namespace olive
