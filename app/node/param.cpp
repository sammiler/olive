

#include "param.h"

#include "node.h"

namespace olive {

QString NodeInput::name() const {
  if (IsValid()) {
    return node_->GetInputName(input_);
  } else {
    return {};
  }
}

bool NodeInput::IsHidden() const {
  if (IsValid()) {
    return node_->IsInputHidden(input_);
  } else {
    return false;
  }
}

bool NodeInput::IsConnected() const {
  if (IsValid()) {
    return node_->IsInputConnected(*this);
  } else {
    return false;
  }
}

bool NodeInput::IsKeyframing() const {
  if (IsValid()) {
    return node_->IsInputKeyframing(*this);
  } else {
    return false;
  }
}

bool NodeInput::IsArray() const {
  if (IsValid()) {
    return node_->InputIsArray(input_);
  } else {
    return false;
  }
}

InputFlags NodeInput::GetFlags() const {
  if (IsValid()) {
    return node_->GetInputFlags(input_);
  } else {
    return InputFlags(kInputFlagNormal);
  }
}

QString NodeInput::GetInputName() const {
  if (IsValid()) {
    return node_->GetInputName(input_);
  } else {
    return {};
  }
}

Node *NodeInput::GetConnectedOutput() const {
  if (IsValid()) {
    return node_->GetConnectedOutput(*this);
  } else {
    return nullptr;
  }
}

NodeValue::Type NodeInput::GetDataType() const {
  if (IsValid()) {
    return node_->GetInputDataType(input_);
  } else {
    return NodeValue::kNone;
  }
}

QVariant NodeInput::GetDefaultValue() const {
  if (IsValid()) {
    return node_->GetDefaultValue(input_);
  } else {
    return {};
  }
}

QStringList NodeInput::GetComboBoxStrings() const {
  if (IsValid()) {
    return node_->GetComboBoxStrings(input_);
  } else {
    return {};
  }
}

QVariant NodeInput::GetProperty(const QString &key) const {
  if (IsValid()) {
    return node_->GetInputProperty(input_, key);
  } else {
    return {};
  }
}

QHash<QString, QVariant> NodeInput::GetProperties() const {
  if (IsValid()) {
    return node_->GetInputProperties(input_);
  } else {
    return {};
  }
}

QVariant NodeInput::GetValueAtTime(const rational &time) const {
  if (IsValid()) {
    return node_->GetValueAtTime(*this, time);
  } else {
    return {};
  }
}

NodeKeyframe *NodeInput::GetKeyframeAtTimeOnTrack(const rational &time, int track) const {
  if (IsValid()) {
    return node_->GetKeyframeAtTimeOnTrack(*this, time, track);
  } else {
    return nullptr;
  }
}

QVariant NodeInput::GetSplitDefaultValueForTrack(int track) const {
  if (IsValid()) {
    return node_->GetSplitDefaultValueOnTrack(input_, track);
  } else {
    return {};
  }
}

int NodeInput::GetArraySize() const {
  if (IsValid() && element_ == -1) {
    return node_->InputArraySize(input_);
  } else {
    return 0;
  }
}

uint qHash(const NodeInput &i) { return qHash(i.node()) ^ qHash(i.input()) ^ ::qHash(i.element()); }

uint qHash(const NodeKeyframeTrackReference &i) { return qHash(i.input()) & ::qHash(i.track()); }

uint qHash(const NodeInputPair &i) { return qHash(i.node) & qHash(i.input); }

}  // namespace olive
