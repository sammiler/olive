

#ifndef NODEPARAM_H
#define NODEPARAM_H

#include <QString>

#include "value.h"

namespace olive {

class Node;
class NodeKeyframe;

enum InputFlag : uint64_t {
  /// By default, inputs are keyframable, connectable, and NOT arrays
  kInputFlagNormal = 0x0,
  kInputFlagArray = 0x1,
  kInputFlagNotKeyframable = 0x2,
  kInputFlagNotConnectable = 0x4,
  kInputFlagHidden = 0x8,
  kInputFlagIgnoreInvalidations = 0x10,

  kInputFlagStatic = kInputFlagNotKeyframable | kInputFlagNotConnectable
};

class InputFlags {
 public:
  explicit InputFlags() { f_ = kInputFlagNormal; }

  explicit InputFlags(uint64_t flags) { f_ = flags; }

  InputFlags operator|(const InputFlags &f) const {
    InputFlags i = *this;
    i |= f;
    return i;
  }

  InputFlags operator|(const InputFlag &f) const {
    InputFlags i = *this;
    i |= f;
    return i;
  }

  InputFlags operator|(const uint64_t &f) const {
    InputFlags i = *this;
    i |= f;
    return i;
  }

  InputFlags &operator|=(const InputFlags &f) {
    f_ |= f.f_;
    return *this;
  }

  InputFlags &operator|=(const InputFlag &f) {
    f_ |= f;
    return *this;
  }

  InputFlags &operator|=(const uint64_t &f) {
    f_ |= f;
    return *this;
  }

  InputFlags operator&(const InputFlags &f) const {
    InputFlags i = *this;
    i &= f;
    return i;
  }

  InputFlags operator&(const InputFlag &f) const {
    InputFlags i = *this;
    i &= f;
    return i;
  }

  InputFlags operator&(const uint64_t &f) const {
    InputFlags i = *this;
    i &= f;
    return i;
  }

  InputFlags &operator&=(const InputFlags &f) {
    f_ &= f.f_;
    return *this;
  }

  InputFlags &operator&=(const InputFlag &f) {
    f_ &= f;
    return *this;
  }

  InputFlags &operator&=(const uint64_t &f) {
    f_ &= f;
    return *this;
  }

  InputFlags operator~() const {
    InputFlags i = *this;
    i.f_ = ~i.f_;
    return i;
  }

  inline explicit operator bool() const { return f_; }

  [[nodiscard]] inline const uint64_t &value() const { return f_; }

 private:
  uint64_t f_;
};

struct NodeInputPair {
  bool operator==(const NodeInputPair &rhs) const { return node == rhs.node && input == rhs.input; }

  Node *node{};
  QString input;
};

/**
 * @brief Defines a Node input
 */
class NodeInput {
 public:
  NodeInput() {
    node_ = nullptr;
    element_ = -1;
  }

  NodeInput(Node *n, const QString &i, int e = -1) {
    node_ = n;
    input_ = i;
    element_ = e;
  }

  bool operator==(const NodeInput &rhs) const {
    return node_ == rhs.node_ && input_ == rhs.input_ && element_ == rhs.element_;
  }

  bool operator!=(const NodeInput &rhs) const { return !(*this == rhs); }

  bool operator<(const NodeInput &rhs) const {
    if (node_ != rhs.node_) {
      return node_ < rhs.node_;
    }

    if (input_ != rhs.input_) {
      return input_ < rhs.input_;
    }

    return element_ < rhs.element_;
  }

  [[nodiscard]] Node *node() const { return node_; }

  [[nodiscard]] NodeInputPair input_pair() const { return {node_, input_}; }

  [[nodiscard]] const QString &input() const { return input_; }

  [[nodiscard]] const int &element() const { return element_; }

  void set_node(Node *node) { node_ = node; }

  void set_input(const QString &input) { input_ = input; }

  void set_element(int e) { element_ = e; }

  [[nodiscard]] QString name() const;

  [[nodiscard]] bool IsValid() const { return node_ && !input_.isEmpty() && element_ >= -1; }

  [[nodiscard]] bool IsHidden() const;

  [[nodiscard]] bool IsConnected() const;

  [[nodiscard]] bool IsKeyframing() const;

  [[nodiscard]] bool IsArray() const;

  [[nodiscard]] InputFlags GetFlags() const;

  [[nodiscard]] QString GetInputName() const;

  [[nodiscard]] Node *GetConnectedOutput() const;

  [[nodiscard]] NodeValue::Type GetDataType() const;

  [[nodiscard]] QVariant GetDefaultValue() const;

  [[nodiscard]] QStringList GetComboBoxStrings() const;

  [[nodiscard]] QVariant GetProperty(const QString &key) const;
  [[nodiscard]] QHash<QString, QVariant> GetProperties() const;

  [[nodiscard]] QVariant GetValueAtTime(const rational &time) const;

  [[nodiscard]] NodeKeyframe *GetKeyframeAtTimeOnTrack(const rational &time, int track) const;

  [[nodiscard]] QVariant GetSplitDefaultValueForTrack(int track) const;

  [[nodiscard]] int GetArraySize() const;

  void Reset() { *this = NodeInput(); }

 private:
  Node *node_;
  QString input_;
  int element_;
};

struct InputElementPair {
  QString input;
  int element;

  bool operator<(const InputElementPair &rhs) const {
    if (input != rhs.input) {
      return input < rhs.input;
    }

    return element < rhs.element;
  }

  bool operator==(const InputElementPair &rhs) const { return input == rhs.input && element == rhs.element; }

  bool operator!=(const InputElementPair &rhs) const { return !(*this == rhs); }
};

class NodeKeyframeTrackReference {
 public:
  NodeKeyframeTrackReference() { track_ = -1; }

  explicit NodeKeyframeTrackReference(const NodeInput &input, int track = 0) {
    input_ = input;
    track_ = track;
  }

  bool operator==(const NodeKeyframeTrackReference &rhs) const { return input_ == rhs.input_ && track_ == rhs.track_; }

  [[nodiscard]] const NodeInput &input() const { return input_; }

  [[nodiscard]] int track() const { return track_; }

  [[nodiscard]] bool IsValid() const { return input_.IsValid() && track_ >= 0; }

  void Reset() { *this = NodeKeyframeTrackReference(); }

 private:
  NodeInput input_;
  int track_;
};

uint qHash(const NodeInputPair &i);
uint qHash(const NodeInput &i);
uint qHash(const NodeKeyframeTrackReference &i);

}  // namespace olive

Q_DECLARE_METATYPE(olive::NodeInput)
Q_DECLARE_METATYPE(olive::NodeKeyframeTrackReference)

#endif  // NODEPARAM_H
