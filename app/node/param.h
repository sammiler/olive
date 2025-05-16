#ifndef NODEPARAM_H  // 防止头文件被重复包含的宏
#define NODEPARAM_H  // 定义 NODEPARAM_H 宏

#include <QString>  // Qt 字符串类

#include "value.h"  // 包含值类型定义 (可能包含 rational 等)

namespace olive {  // olive 项目的命名空间

class Node;          // 向前声明 Node 类
class NodeKeyframe;  // 向前声明 NodeKeyframe 类

// 输入端口标志位枚举，用于定义输入端口的各种属性和行为
enum InputFlag : uint64_t {  // 使用 uint64_t 作为底层类型
  /// 默认情况下，输入是可设置关键帧、可连接且非数组的
  kInputFlagNormal = 0x0,                // 正常标志，默认状态
  kInputFlagArray = 0x1,                 // 标记此输入为数组类型
  kInputFlagNotKeyframable = 0x2,        // 标记此输入不可设置关键帧
  kInputFlagNotConnectable = 0x4,        // 标记此输入不可连接 (即不能从其他节点获取输入)
  kInputFlagHidden = 0x8,                // 标记此输入在UI中隐藏
  kInputFlagIgnoreInvalidations = 0x10,  // 标记此输入忽略来自上游节点的缓存失效信号

  // 组合标志：静态输入，既不可设置关键帧也不可连接
  kInputFlagStatic = kInputFlagNotKeyframable | kInputFlagNotConnectable
};

/**
 * @brief InputFlags 类封装了一组输入标志 (InputFlag)。
 * 它提供了方便的位操作符重载，用于组合和检查标志。
 */
class InputFlags {
 public:
  // 默认构造函数，初始化为正常标志
  explicit InputFlags() { f_ = kInputFlagNormal; }

  // 从 uint64_t 值构造 InputFlags 对象
  explicit InputFlags(uint64_t flags) { f_ = flags; }

  // 重载按位或 (|) 运算符，用于组合两个 InputFlags 对象
  InputFlags operator|(const InputFlags &f) const {
    InputFlags i = *this;  // 创建副本
    i |= f;                // 调用 |= 运算符
    return i;              // 返回结果
  }

  // 重载按位或 (|) 运算符，用于组合 InputFlags 对象和一个 InputFlag 枚举值
  InputFlags operator|(const InputFlag &f) const {
    InputFlags i = *this;
    i |= f;
    return i;
  }

  // 重载按位或 (|) 运算符，用于组合 InputFlags 对象和一个 uint64_t 值
  InputFlags operator|(const uint64_t &f) const {
    InputFlags i = *this;
    i |= f;
    return i;
  }

  // 重载按位或赋值 (|=) 运算符，用于将另一个 InputFlags 对象的标志合并到当前对象
  InputFlags &operator|=(const InputFlags &f) {
    f_ |= f.f_;    // 对底层 uint64_t 值进行按位或操作
    return *this;  // 返回当前对象的引用
  }

  // 重载按位或赋值 (|=) 运算符，用于将一个 InputFlag 枚举值的标志合并到当前对象
  InputFlags &operator|=(const InputFlag &f) {
    f_ |= f;
    return *this;
  }

  // 重载按位或赋值 (|=) 运算符，用于将一个 uint64_t 值的标志合并到当前对象
  InputFlags &operator|=(const uint64_t &f) {
    f_ |= f;
    return *this;
  }

  // 重载按位与 (&) 运算符，用于获取两个 InputFlags 对象的共同标志
  InputFlags operator&(const InputFlags &f) const {
    InputFlags i = *this;
    i &= f;
    return i;
  }

  // 重载按位与 (&) 运算符，用于检查 InputFlags 对象是否包含某个 InputFlag 枚举值
  InputFlags operator&(const InputFlag &f) const {
    InputFlags i = *this;
    i &= f;
    return i;
  }

  // 重载按位与 (&) 运算符，用于检查 InputFlags 对象是否包含某个 uint64_t 值中的标志
  InputFlags operator&(const uint64_t &f) const {
    InputFlags i = *this;
    i &= f;
    return i;
  }

  // 重载按位与赋值 (&=) 运算符，用于将当前对象的标志更新为与另一个 InputFlags 对象的共同标志
  InputFlags &operator&=(const InputFlags &f) {
    f_ &= f.f_;
    return *this;
  }

  // 重载按位与赋值 (&=) 运算符，用于将当前对象的标志更新为与某个 InputFlag 枚举值的共同标志
  InputFlags &operator&=(const InputFlag &f) {
    f_ &= f;
    return *this;
  }

  // 重载按位与赋值 (&=) 运算符，用于将当前对象的标志更新为与某个 uint64_t 值的共同标志
  InputFlags &operator&=(const uint64_t &f) {
    f_ &= f;
    return *this;
  }

  // 重载按位非 (~) 运算符，用于反转所有标志位
  InputFlags operator~() const {
    InputFlags i = *this;
    i.f_ = ~i.f_;  // 对底层 uint64_t 值进行按位非操作
    return i;
  }

  // 重载 bool 类型转换运算符，用于检查是否有任何标志位被设置 (即 f_ 不为0)
  inline explicit operator bool() const { return f_; }

  // 获取底层的 uint64_t 标志值
  [[nodiscard]] inline const uint64_t &value() const { return f_; }

 private:
  uint64_t f_;  // 存储组合标志的 uint64_t 值
};

// 节点输入对结构体，用于唯一标识一个节点的特定输入端口 (不含元素索引)
struct NodeInputPair {
  // 重载等于 (==) 运算符，比较两个 NodeInputPair 是否相同
  bool operator==(const NodeInputPair &rhs) const { return node == rhs.node && input == rhs.input; }

  Node *node{};   // 指向节点的指针
  QString input;  // 输入端口的ID字符串
};

/**
 * @brief NodeInput 类定义了一个节点的具体输入实例。
 * 它包含了节点指针、输入ID字符串以及元素索引 (用于数组类型的输入)。
 * 这个类常用于参数传递、连接管理和值获取。
 */
class NodeInput {
 public:
  // 默认构造函数，创建一个无效的 NodeInput 对象
  NodeInput() {
    node_ = nullptr;  // 节点指针初始化为空
    element_ = -1;    // 元素索引初始化为-1 (表示非数组元素或整个数组)
  }

  /**
   * @brief 构造函数，创建一个指向特定节点输入端口 (和可选元素) 的 NodeInput 对象。
   * @param n 指向节点的指针。
   * @param i 输入端口的ID字符串。
   * @param e 元素索引，默认为-1。对于非数组输入或表示整个数组时使用-1。
   */
  NodeInput(Node *n, const QString &i, int e = -1) {
    node_ = n;
    input_ = i;
    element_ = e;
  }

  // 重载等于 (==) 运算符，比较两个 NodeInput 对象是否完全相同 (节点、输入ID、元素索引)
  bool operator==(const NodeInput &rhs) const {
    return node_ == rhs.node_ && input_ == rhs.input_ && element_ == rhs.element_;
  }

  // 重载不等于 (!=) 运算符
  bool operator!=(const NodeInput &rhs) const { return !(*this == rhs); }

  // 重载小于 (<) 运算符，主要用于在STL容器 (如 std::map) 中排序
  bool operator<(const NodeInput &rhs) const {
    if (node_ != rhs.node_) {  // 首先比较节点指针
      return node_ < rhs.node_;
    }

    if (input_ != rhs.input_) {  // 然后比较输入ID字符串
      return input_ < rhs.input_;
    }

    return element_ < rhs.element_;  // 最后比较元素索引
  }

  // 获取节点指针
  [[nodiscard]] Node *node() const { return node_; }

  // 获取 NodeInputPair (不含元素索引)
  [[nodiscard]] NodeInputPair input_pair() const { return {node_, input_}; }

  // 获取输入端口的ID字符串
  [[nodiscard]] const QString &input() const { return input_; }

  // 获取元素索引
  [[nodiscard]] const int &element() const { return element_; }

  // 设置节点指针
  void set_node(Node *node) { node_ = node; }

  // 设置输入端口的ID字符串
  void set_input(const QString &input) { input_ = input; }

  // 设置元素索引
  void set_element(int e) { element_ = e; }

  // 获取此输入的显示名称 (通常由 Node 类提供)
  [[nodiscard]] QString name() const;

  // 检查此 NodeInput 对象是否有效 (节点指针非空，输入ID非空，元素索引有效)
  [[nodiscard]] bool IsValid() const { return node_ && !input_.isEmpty() && element_ >= -1; }

  // 检查此输入是否被标记为隐藏 (查询 Node 类)
  [[nodiscard]] bool IsHidden() const;

  // 检查此输入当前是否已连接到其他节点的输出 (查询 Node 类)
  [[nodiscard]] bool IsConnected() const;

  // 检查此输入当前是否正在使用关键帧 (查询 Node 类)
  [[nodiscard]] bool IsKeyframing() const;

  // 检查此输入是否为数组类型 (查询 Node 类)
  [[nodiscard]] bool IsArray() const;

  // 获取此输入的标志位 (查询 Node 类)
  [[nodiscard]] InputFlags GetFlags() const;

  // 获取此输入的原始名称 (通常是ID，或由 Node 类定义的特定名称)
  [[nodiscard]] QString GetInputName() const;

  // 获取连接到此输入的上游输出节点 (查询 Node 类)
  [[nodiscard]] Node *GetConnectedOutput() const;

  // 获取此输入的数据类型 (查询 Node 类)
  [[nodiscard]] NodeValue::Type GetDataType() const;

  // 获取此输入的默认值 (查询 Node 类)
  [[nodiscard]] QVariant GetDefaultValue() const;

  // 如果此输入是下拉框类型，获取其选项列表 (查询 Node 类)
  [[nodiscard]] QStringList GetComboBoxStrings() const;

  // 获取此输入的指定属性值 (查询 Node 类)
  [[nodiscard]] QVariant GetProperty(const QString &key) const;
  // 获取此输入的所有属性 (查询 Node 类)
  [[nodiscard]] QHash<QString, QVariant> GetProperties() const;

  // 获取此输入在指定时间的值 (查询 Node 类，会处理连接和关键帧)
  [[nodiscard]] QVariant GetValueAtTime(const rational &time) const;

  // 获取此输入在指定时间、指定轨道上的关键帧 (查询 Node 类)
  [[nodiscard]] NodeKeyframe *GetKeyframeAtTimeOnTrack(const rational &time, int track) const;

  // 获取此输入指定轨道的默认分离值 (查询 Node 类)
  [[nodiscard]] QVariant GetSplitDefaultValueForTrack(int track) const;

  // 如果此输入是数组类型，获取其大小 (查询 Node 类)
  [[nodiscard]] int GetArraySize() const;

  // 将 NodeInput 对象重置为无效状态 (默认构造状态)
  void Reset() { *this = NodeInput(); }

 private:
  Node *node_;     // 指向所属节点的指针
  QString input_;  // 输入端口的ID字符串
  int element_;    // 元素索引 (-1 表示非数组元素或整个数组)
};

// 输入ID和元素索引的组合结构体，用于在节点内部唯一标识一个输入元素
// (例如作为 QMap 的键)
struct InputElementPair {
  QString input;  // 输入端口的ID字符串
  int element;    // 元素索引

  // 重载小于 (<) 运算符，用于排序
  bool operator<(const InputElementPair &rhs) const {
    if (input != rhs.input) {  // 先比较输入ID
      return input < rhs.input;
    }
    return element < rhs.element;  // 再比较元素索引
  }

  // 重载等于 (==) 运算符
  bool operator==(const InputElementPair &rhs) const { return input == rhs.input && element == rhs.element; }

  // 重载不等于 (!=) 运算符
  bool operator!=(const InputElementPair &rhs) const { return !(*this == rhs); }
};

/**
 * @brief NodeKeyframeTrackReference 类用于引用一个特定的关键帧轨道。
 * 它由一个 NodeInput (标识节点、输入和元素) 和一个轨道索引组成。
 */
class NodeKeyframeTrackReference {
 public:
  // 默认构造函数，创建一个无效的引用
  NodeKeyframeTrackReference() { track_ = -1; }  // 轨道索引初始化为-1

  /**
   * @brief 构造函数。
   * @param input NodeInput 对象，指定了节点、输入ID和元素。
   * @param track 轨道索引，默认为0 (例如，对于单值参数，通常只有轨道0)。
   */
  explicit NodeKeyframeTrackReference(const NodeInput &input, int track = 0) {
    input_ = input;
    track_ = track;
  }

  // 重载等于 (==) 运算符，比较两个引用是否指向同一个关键帧轨道
  bool operator==(const NodeKeyframeTrackReference &rhs) const { return input_ == rhs.input_ && track_ == rhs.track_; }

  // 获取 NodeInput 部分
  [[nodiscard]] const NodeInput &input() const { return input_; }

  // 获取轨道索引
  [[nodiscard]] int track() const { return track_; }

  // 检查此引用是否有效 (NodeInput 有效且轨道索引非负)
  [[nodiscard]] bool IsValid() const { return input_.IsValid() && track_ >= 0; }

  // 将引用重置为无效状态
  void Reset() { *this = NodeKeyframeTrackReference(); }

 private:
  NodeInput input_;  // 相关的 NodeInput (节点、输入ID、元素)
  int track_;        // 关键帧轨道的索引
};

// 为 NodeInputPair 提供 qHash 函数，使其能用作 QHash 的键
uint qHash(const NodeInputPair &i);
// 为 NodeInput 提供 qHash 函数
uint qHash(const NodeInput &i);
// 为 NodeKeyframeTrackReference 提供 qHash 函数
uint qHash(const NodeKeyframeTrackReference &i);

}  // namespace olive

// 声明 NodeInput 类型为元类型，以便在 QVariant 中使用
Q_DECLARE_METATYPE(olive::NodeInput)
// 声明 NodeKeyframeTrackReference 类型为元类型
Q_DECLARE_METATYPE(olive::NodeKeyframeTrackReference)

#endif  // NODEPARAM_H