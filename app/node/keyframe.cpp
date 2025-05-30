#include "keyframe.h"

#include <utility>

#include "node.h"

namespace olive {

const NodeKeyframe::Type NodeKeyframe::kDefaultType = kLinear;

NodeKeyframe::NodeKeyframe(const rational &time, QVariant value, Type type, int track, int element, QString input,
                           QObject *parent)
    : time_(time),
      value_(std::move(value)),
      type_(type),
      bezier_control_in_(QPointF(0.0, 0.0)),
      bezier_control_out_(QPointF(0.0, 0.0)),
      input_(std::move(input)),
      track_(track),
      element_(element),
      previous_(nullptr),
      next_(nullptr) {
  setParent(parent);
}

NodeKeyframe::NodeKeyframe() { type_ = NodeKeyframe::kLinear; }

NodeKeyframe::~NodeKeyframe() { setParent(nullptr); }

NodeKeyframe *NodeKeyframe::copy(int element, QObject *parent) const {
  auto *copy = new NodeKeyframe(time_, value_, type_, track_, element, input_, parent);
  copy->bezier_control_in_ = bezier_control_in_;
  copy->bezier_control_out_ = bezier_control_out_;
  return copy;
}

NodeKeyframe *NodeKeyframe::copy(QObject *parent) const { return copy(element_, parent); }

Node *NodeKeyframe::parent() const { return dynamic_cast<Node *>(QObject::parent()); }

const rational &NodeKeyframe::time() const { return time_; }

void NodeKeyframe::set_time(const rational &time) {
  time_ = time;
  emit TimeChanged(time_);
}

const QVariant &NodeKeyframe::value() const { return value_; }

void NodeKeyframe::set_value(const QVariant &value) {
  value_ = value;
  emit ValueChanged(value_);
}

const NodeKeyframe::Type &NodeKeyframe::type() const { return type_; }

void NodeKeyframe::set_type(const NodeKeyframe::Type &type) {
  if (type_ != type) {
    set_type_no_bezier_adj(type);

    if (type_ == kBezier) {
      // Set some sane defaults if this keyframe existed in the track and was just changed
      if (bezier_control_in_.isNull()) {
        if (previous_) {
          // Set the in point to be half way between
          set_bezier_control_in(QPointF((previous_->time().toDouble() - this->time().toDouble()) * 0.5, 0.0));
        } else {
          set_bezier_control_in(QPointF(-1.0, 0.0));
        }
      }

      if (bezier_control_out_.isNull()) {
        if (next_) {
          set_bezier_control_out(QPointF((next_->time().toDouble() - this->time().toDouble()) * 0.5, 0.0));
        } else {
          set_bezier_control_out(QPointF(1.0, 0.0));
        }
      }
    }
  }
}

void NodeKeyframe::set_type_no_bezier_adj(const Type &type) {
  type_ = type;
  emit TypeChanged(type_);
}

const QPointF &NodeKeyframe::bezier_control_in() const { return bezier_control_in_; }

void NodeKeyframe::set_bezier_control_in(const QPointF &control) {
  bezier_control_in_ = control;
  emit BezierControlInChanged(bezier_control_in_);
}

const QPointF &NodeKeyframe::bezier_control_out() const { return bezier_control_out_; }

void NodeKeyframe::set_bezier_control_out(const QPointF &control) {
  bezier_control_out_ = control;
  emit BezierControlOutChanged(bezier_control_out_);
}

QPointF NodeKeyframe::valid_bezier_control_in() const {
  double t = time().toDouble();
  qreal adjusted_x = t + bezier_control_in_.x();

  if (previous_) {
    // Limit to the point of that keyframe
    adjusted_x = qMax(adjusted_x, previous_->time().toDouble());
  }

  return {adjusted_x - t, bezier_control_in_.y()};
}

QPointF NodeKeyframe::valid_bezier_control_out() const {
  double t = time().toDouble();
  qreal adjusted_x = t + bezier_control_out_.x();

  if (next_) {
    // Limit to the point of that keyframe
    adjusted_x = qMin(adjusted_x, next_->time().toDouble());
  }

  return {adjusted_x - t, bezier_control_out_.y()};
}

const QPointF &NodeKeyframe::bezier_control(NodeKeyframe::BezierType type) const {
  if (type == kInHandle) {
    return bezier_control_in();
  } else {
    return bezier_control_out();
  }
}

void NodeKeyframe::set_bezier_control(NodeKeyframe::BezierType type, const QPointF &control) {
  if (type == kInHandle) {
    set_bezier_control_in(control);
  } else {
    set_bezier_control_out(control);
  }
}

NodeKeyframe::BezierType NodeKeyframe::get_opposing_bezier_type(NodeKeyframe::BezierType type) {
  if (type == kInHandle) {
    return kOutHandle;
  } else {
    return kInHandle;
  }
}

bool NodeKeyframe::has_sibling_at_time(const rational &t) const {
  NodeKeyframe *k = parent()->GetKeyframeAtTimeOnTrack(input(), t, track(), element());
  return k && k != this;
}

bool NodeKeyframe::load(QXmlStreamReader *reader, NodeValue::Type data_type) {
  QString key_input;
  QPointF key_in_handle;
  QPointF key_out_handle;

  XMLAttributeLoop(reader, attr) {
    if (attr.name() == QStringLiteral("input")) {
      key_input = attr.value().toString();
    } else if (attr.name() == QStringLiteral("time")) {
      this->set_time(rational::fromString(attr.value().toString().toUtf8().constData()));
    } else if (attr.name() == QStringLiteral("type")) {
      this->set_type_no_bezier_adj(static_cast<NodeKeyframe::Type>(attr.value().toInt()));
    } else if (attr.name() == QStringLiteral("inhandlex")) {
      key_in_handle.setX(attr.value().toDouble());
    } else if (attr.name() == QStringLiteral("inhandley")) {
      key_in_handle.setY(attr.value().toDouble());
    } else if (attr.name() == QStringLiteral("outhandlex")) {
      key_out_handle.setX(attr.value().toDouble());
    } else if (attr.name() == QStringLiteral("outhandley")) {
      key_out_handle.setY(attr.value().toDouble());
    }
  }

  this->set_value(NodeValue::StringToValue(data_type, reader->readElementText(), true));

  this->set_bezier_control_in(key_in_handle);
  this->set_bezier_control_out(key_out_handle);

  return true;
}

void NodeKeyframe::save(QXmlStreamWriter *writer, NodeValue::Type data_type) const {
  writer->writeAttribute(QStringLiteral("input"), this->input());
  writer->writeAttribute(QStringLiteral("time"), QString::fromStdString(this->time().toString()));
  writer->writeAttribute(QStringLiteral("type"), QString::number(this->type()));
  writer->writeAttribute(QStringLiteral("inhandlex"), QString::number(this->bezier_control_in().x()));
  writer->writeAttribute(QStringLiteral("inhandley"), QString::number(this->bezier_control_in().y()));
  writer->writeAttribute(QStringLiteral("outhandlex"), QString::number(this->bezier_control_out().x()));
  writer->writeAttribute(QStringLiteral("outhandley"), QString::number(this->bezier_control_out().y()));

  writer->writeCharacters(NodeValue::ValueToString(data_type, this->value(), true));
}

}  // namespace olive
