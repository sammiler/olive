#ifndef NODEVALUE_H // 防止头文件被重复包含的宏
#define NODEVALUE_H // 定义 NODEVALUE_H 宏

#include <QMatrix4x4> // Qt 4x4 矩阵类
#include <QString>    // Qt 字符串类
#include <QVariant>   // Qt 通用数据类型 QVariant
#include <QVector>    // Qt 动态数组容器 QVector
#include <utility>    // 标准库 utility 头文件，提供 std::move 等

#include "common/qtutils.h"   // Qt 工具类 (可能包含 Color, Bezier 等自定义类型)
#include "node/splitvalue.h"  // SplitValue 类型定义
#include "render/texture.h"   // Texture (纹理) 和 SampleBuffer (音频样本) 相关定义

namespace olive { // olive 项目的命名空间

class Node;           // 向前声明 Node 类
class NodeValue;      // 向前声明 NodeValue 类自身 (用于类型别名)
class NodeValueTable; // 向前声明 NodeValueTable 类

// 类型别名：NodeValueArray 代表一个从整数索引到 NodeValue 的映射 (通常用于表示数组类型的输入值)
using NodeValueArray = std::map<int, NodeValue>;
// 类型别名：NodeValueTableArray 代表一个从整数索引到 NodeValueTable 的映射 (通常用于表示数组类型的输入的完整值表)
using NodeValueTableArray = std::map<int, NodeValueTable>;

/**
 * @brief NodeValue 类封装了可以在节点之间传递的各种类型的数据。
 *
 * 它内部使用 QVariant 来存储实际数据，并通过一个 Type 枚举来标识数据的具体类型。
 * NodeValue 还可能包含关于数据来源节点 (from_) 和一个可选的标签 (tag_) 的信息。
 * 这个类是节点图数据流的基础。
 */
class NodeValue {
 public:
  /**
   * @brief 定义了可以在节点之间传递的数据类型枚举。
   */
  enum Type {
    kNone, // 无类型或未定义类型

    /**
     ****************************** SPECIFIC IDENTIFIERS (特定标识符) ******************************
     */

    /**
     * @brief 整数类型。
     * 实际解析为 int64_t。
     */
    kInt,

    /**
     * @brief 十进制 (浮点数) 类型。
     * 实际解析为 `double`。
     */
    kFloat,

    /**
     * @brief 有理数类型 (通常用于表示精确的时间或比率)。
     * 实际解析为 `olive::core::rational` (或 `double`，取决于具体实现)。
     */
    kRational,

    /**
     * @brief 布尔类型。
     * 实际解析为 `bool`。
     */
    kBoolean,

    /**
     * @brief 颜色类型。
     * 实际解析为 `olive::core::Color`。
     * 节点间传递的颜色应始终处于参考色彩空间。
     */
    kColor,

    /**
     * @brief 矩阵类型。
     * 实际解析为 `QMatrix4x4`。
     */
    kMatrix,

    /**
     * @brief 文本类型。
     * 实际解析为 `QString`。
     */
    kText,

    /**
     * @brief 字体类型。
     * 实际解析为 `QFont`。
     */
    kFont,

    /**
     * @brief 文件路径类型。
     * 实际解析为包含绝对文件路径的 `QString`。
     */
    kFile,

    /**
     * @brief 图像缓冲区类型 (纹理)。
     * 实际的值类型取决于所使用的渲染引擎 (例如，可能是 OpenGL 纹理ID或指向GPU内存的指针)。
     * 通常封装在 TexturePtr 中。
     */
    kTexture,

    /**
     * @brief 音频样本类型。
     * 实际解析为 `SampleBuffer` (或 `SampleBufferPtr`)。
     */
    kSamples,

    /**
     * @brief 二维向量 (XY) 类型。
     * 实际解析为 `QVector2D`。
     */
    kVec2,

    /**
     * @brief 三维向量 (XYZ) 类型。
     * 实际解析为 `QVector3D`。
     */
    kVec3,

    /**
     * @brief 四维向量 (XYZW) 类型。
     * 实际解析为 `QVector4D`。
     */
    kVec4,

    /**
     * @brief 三次贝塞尔曲线类型，包含一个主点和两个控制点 (每个点都是X/Y坐标)。
     * 实际解析为 `Bezier` (自定义类型)。
     */
    kBezier,

    /**
     * @brief 下拉框 (ComboBox) 类型。
     * 实际解析为 `int`，代表当前选中的选项索引。
     */
    kCombo,

    /**
     * @brief 视频参数类型。
     * 实际解析为 `VideoParams` (自定义类型，包含分辨率、帧率等)。
     */
    kVideoParams,

    /**
     * @brief 音频参数类型。
     * 实际解析为 `AudioParams` (自定义类型，包含采样率、通道数等)。
     */
    kAudioParams,

    /**
     * @brief 字幕参数类型。
     * 实际解析为 `SubtitleParams` (自定义类型)。
     */
    kSubtitleParams,

    /**
     * @brief 二进制数据类型。
     * 通常用于存储无法归类到其他特定类型的数据块。
     */
    kBinary,

    /**
     * @brief 数据类型列表的末尾标记，常用于计数或迭代。
     */
    kDataTypeCount
  };

  // 默认构造函数，创建一个 kNone 类型的 NodeValue
  NodeValue() : type_(kNone), from_(nullptr), array_(false) {}

  /**
   * @brief 模板构造函数，用于创建指定类型和数据的 NodeValue。
   * @tparam T 实际数据的类型。
   * @param type NodeValue 的枚举类型 (NodeValue::Type)。
   * @param data 实际存储的数据。
   * @param from (可选) 指向源节点的指针。
   * @param array (可选) 标记此值是否代表数组的一个元素或整个数组。
   * @param tag (可选) 与此值关联的标签字符串。
   */
  template <typename T>
  NodeValue(Type type, const T& data, const Node* from = nullptr, bool array = false, QString tag = QString())
      : type_(type), from_(from), tag_(std::move(tag)), array_(array) {
    set_value(data); // 将数据存入内部的 QVariant
  }

  /**
   * @brief 模板构造函数 (简化版，array 默认为 false)。
   */
  template <typename T>
  NodeValue(Type type, const T& data, const Node* from, const QString& tag) : NodeValue(type, data, from, false, tag) {}

  // 获取 NodeValue 的枚举类型
  [[nodiscard]] Type type() const { return type_; }

  /**
   * @brief 模板成员函数，获取存储的实际数据值。
   * @tparam T 期望获取的数据类型。
   * @return 返回转换后的数据值。如果类型不匹配或无法转换，行为未定义 (QVariant::value() 的行为)。
   */
  template <typename T>
  [[nodiscard]] T value() const {
    return data_.value<T>(); // 从 QVariant 获取值
  }

  /**
   * @brief 模板成员函数，设置存储的实际数据值。
   * @tparam T 要设置的数据的类型。
   * @param v 要设置的数据值。
   */
  template <typename T>
  void set_value(const T& v) {
    data_ = QVariant::fromValue(v); // 将数据包装成 QVariant
  }

  // 获取内部存储的 QVariant 数据的常量引用
  [[nodiscard]] const QVariant& data() const { return data_; }

  /**
   * @brief 模板成员函数，检查内部的 QVariant 是否能转换为指定的类型 T。
   * @tparam T 目标转换类型。
   * @return 如果可以转换，则返回 true；否则返回 false。
   */
  template <typename T>
  [[nodiscard]] bool canConvert() const {
    return data_.canConvert<T>();
  }

  // 获取与此 NodeValue 关联的标签字符串
  [[nodiscard]] const QString& tag() const { return tag_; }

  // 设置与此 NodeValue 关联的标签字符串
  void set_tag(const QString& tag) { tag_ = tag; }

  // 获取此 NodeValue 的源节点指针
  [[nodiscard]] const Node* source() const { return from_; }

  // 检查此 NodeValue 是否表示数组的一部分
  [[nodiscard]] bool array() const { return array_; }

  // 重载等于 (==) 运算符，比较两个 NodeValue 对象是否相等 (类型、标签和数据都相等)
  bool operator==(const NodeValue& rhs) const { return type_ == rhs.type_ && tag_ == rhs.tag_ && data_ == rhs.data_; }

  // 重载 bool 类型转换运算符，用于检查 NodeValue 是否包含有效 (非空) 数据
  explicit operator bool() const { return !data_.isNull(); }

  // (静态) 获取数据类型的用户友好显示名称 (例如，用于UI)
  static QString GetPrettyDataTypeName(Type type);

  // (静态) 获取数据类型的内部名称 (例如，用于序列化)
  static QString GetDataTypeName(Type type);
  // (静态) 根据内部名称获取数据类型的枚举值
  static NodeValue::Type GetDataTypeFromName(const QString& n);

  // (静态) 将指定类型的值转换为字符串表示
  // @param data_type 数据的枚举类型。
  // @param value 存储在 QVariant 中的值。
  // @param value_is_a_key_track 指示该值是否代表关键帧轨道上的值 (可能影响字符串格式)。
  // @return 值的字符串表示。
  static QString ValueToString(Type data_type, const QVariant& value, bool value_is_a_key_track);
  // (静态) 重载版本，直接从 NodeValue 对象转换
  static QString ValueToString(const NodeValue& v, bool value_is_a_key_track) {
    return ValueToString(v.type_, v.data_, value_is_a_key_track);
  }

  // (静态) 将字符串表示转换回指定类型的值 (存储在 QVariant 中)
  static QVariant StringToValue(Type data_type, const QString& string, bool value_is_a_key_track);

  // (静态) 将一个“普通”的 QVariant 值 (例如颜色、向量) 分解为其各个轨道/分量的值 (存储在 QVector<QVariant> 中)
  static QVector<QVariant> split_normal_value_into_track_values(Type type, const QVariant& value);

  // (静态) 将一个包含多个轨道/分量值的 QVector<QVariant> (SplitValue) 合并回一个“普通”的 QVariant 值
  static QVariant combine_track_values_into_normal_value(Type type, const QVector<QVariant>& split);

  // 将当前 NodeValue 的数据转换为 SplitValue (QVector<QVariant>)
  [[nodiscard]] SplitValue to_split_value() const { return split_normal_value_into_track_values(type_, data_); }

  /**
   * @brief (静态) 返回一个数据类型是否可以被插值。
   * 例如，数字、向量、颜色通常可以插值，而布尔值、文件路径则不能。
   * @param type 要检查的数据类型。
   * @return 如果可以插值，则返回 true。
   */
  static bool type_can_be_interpolated(NodeValue::Type type) {
    return type == kFloat || type == kVec2 || type == kVec3 || type == kVec4 || type == kBezier || type == kColor ||
           type == kRational;
  }

  // (静态) 检查数据类型是否为数值类型 (整数、浮点数、有理数)
  static bool type_is_numeric(NodeValue::Type type) { return type == kFloat || type == kInt || type == kRational; }

  // (静态) 检查数据类型是否为向量类型 (Vec2, Vec3, Vec4)
  static bool type_is_vector(NodeValue::Type type) { return type == kVec2 || type == kVec3 || type == kVec4; }

  // (静态) 检查数据类型是否为缓冲区类型 (纹理、音频样本)
  static bool type_is_buffer(NodeValue::Type type) { return type == kTexture || type == kSamples; }

  // (静态) 获取指定数据类型对应的默认关键帧轨道数量
  // 例如，kColor (RGBA) 通常有4个轨道，kVec2 有2个轨道，kFloat 有1个轨道。
  static int get_number_of_keyframe_tracks(Type type);

  // (静态) 验证向量字符串列表的格式和数量是否正确
  // @param list 指向 QStringList 的指针，包含从字符串解析出的向量分量。此列表可能会被修改。
  // @param count 期望的向量分量数量。
  static void ValidateVectorString(QStringList* list, int count);

  // 便捷的类型转换函数 (内部调用 value<T>())
  [[nodiscard]] TexturePtr toTexture() const { return value<TexturePtr>(); }
  [[nodiscard]] SampleBuffer toSamples() const { return value<SampleBuffer>(); }
  [[nodiscard]] bool toBool() const { return value<bool>(); }
  [[nodiscard]] double toDouble() const { return value<double>(); }
  [[nodiscard]] int64_t toInt() const { return value<int64_t>(); }
  [[nodiscard]] rational toRational() const { return value<olive::core::rational>(); }
  [[nodiscard]] QString toString() const { return value<QString>(); }
  [[nodiscard]] Color toColor() const { return value<olive::core::Color>(); }
  [[nodiscard]] QMatrix4x4 toMatrix() const { return value<QMatrix4x4>(); }
  [[nodiscard]] VideoParams toVideoParams() const { return value<VideoParams>(); }
  [[nodiscard]] AudioParams toAudioParams() const { return value<AudioParams>(); }
  [[nodiscard]] QVector2D toVec2() const { return value<QVector2D>(); }
  [[nodiscard]] QVector3D toVec3() const { return value<QVector3D>(); }
  [[nodiscard]] QVector4D toVec4() const { return value<QVector4D>(); }
  [[nodiscard]] Bezier toBezier() const { return value<Bezier>(); }
  [[nodiscard]] NodeValueArray toArray() const { return value<NodeValueArray>(); } // 获取数组类型的值

 private:
  Type type_;         // NodeValue 的枚举类型
  QVariant data_;     // 实际存储数据的 QVariant 对象
  const Node* from_;  // 指向源节点的指针 (可选)
  QString tag_;       // 与此值关联的标签字符串 (可选)
  bool array_;        // 标记此值是否代表数组的一部分
};

/**
 * @brief NodeValueTable 类是一个 NodeValue 对象的集合 (通常是 QVector)。
 *
 * 它用于表示一个节点在某个时间点或时间范围内所有输入端口的值，或者一个特定输入端口在多个时间点的值。
 * NodeTraverser 在遍历节点图时会生成和使用 NodeValueTable。
 * 它提供了 Push (添加)、Get (获取)、Take (获取并移除) 等操作。
 */
class NodeValueTable {
 public:
  // 默认构造函数
  NodeValueTable() = default;

  /**
   * @brief 从表中获取指定类型和可选标签的第一个 NodeValue。
   * @param type 期望获取的 NodeValue 的枚举类型。
   * @param tag (可选) 期望获取的 NodeValue 的标签。
   * @return 返回找到的 NodeValue。如果未找到，返回一个无效的 NodeValue (Type 为 kNone)。
   */
  [[nodiscard]] NodeValue Get(NodeValue::Type type, const QString& tag = QString()) const {
    QVector<NodeValue::Type> types = {type}; // 包装成类型列表
    return Get(types, tag);
  }

  /**
   * @brief 从表中获取与给定类型列表和可选标签匹配的第一个 NodeValue。
   * @param type 期望获取的 NodeValue 的类型列表 (匹配其中任何一个即可)。
   * @param tag (可选) 期望获取的 NodeValue 的标签。
   * @return 返回找到的 NodeValue。
   */
  [[nodiscard]] NodeValue Get(const QVector<NodeValue::Type>& type, const QString& tag = QString()) const;

  /**
   * @brief 从表中获取并移除指定类型和可选标签的第一个 NodeValue。
   * @param type 期望获取的 NodeValue 的枚举类型。
   * @param tag (可选) 期望获取的 NodeValue 的标签。
   * @return 返回找到并移除的 NodeValue。
   */
  NodeValue Take(NodeValue::Type type, const QString& tag = QString()) {
    QVector<NodeValue::Type> types = {type};
    return Take(types, tag);
  }

  /**
   * @brief 从表中获取并移除与给定类型列表和可选标签匹配的第一个 NodeValue。
   * @param type 期望获取的 NodeValue 的类型列表。
   * @param tag (可选) 期望获取的 NodeValue 的标签。
   * @return 返回找到并移除的 NodeValue。
   */
  NodeValue Take(const QVector<NodeValue::Type>& type, const QString& tag = QString());

  // 将一个 NodeValue 添加到表的末尾
  void Push(const NodeValue& value) { values_.append(value); }

  // 将另一个 NodeValueTable 中的所有值添加到当前表的末尾
  void Push(const NodeValueTable& value) { values_.append(value.values_); }

  // (模板) 便捷函数，创建一个 NodeValue 并添加到表的末尾
  template <typename T>
  void Push(NodeValue::Type type, const T& data, const Node* from, bool array = false, const QString& tag = QString()) {
    Push(NodeValue(type, data, from, array, tag));
  }
  // (模板) 便捷函数 (简化版)
  template <typename T>
  void Push(NodeValue::Type type, const T& data, const Node* from, const QString& tag) {
    Push(NodeValue(type, data, from, false, tag));
  }

  // 将一个 NodeValue 添加到表的开头
  void Prepend(const NodeValue& value) { values_.prepend(value); }

  // (模板) 便捷函数，创建一个 NodeValue 并添加到表的开头
  template <typename T>
  void Prepend(NodeValue::Type type, const T& data, const Node* from, bool array = false,
               const QString& tag = QString()) {
    Prepend(NodeValue(type, data, from, array, tag));
  }
  // (模板) 便捷函数 (简化版)
  template <typename T>
  void Prepend(NodeValue::Type type, const T& data, const Node* from, const QString& tag) {
    Prepend(NodeValue(type, data, from, false, tag));
  }

  // 获取表中指定索引处的 NodeValue 的常量引用
  [[nodiscard]] const NodeValue& at(int index) const { return values_.at(index); }
  // 获取并移除表中指定索引处的 NodeValue
  NodeValue TakeAt(int index) { return values_.takeAt(index); }

  // 获取表中 NodeValue 的数量
  [[nodiscard]] int Count() const { return values_.size(); }

  // 检查表中是否包含指定类型的 NodeValue
  [[nodiscard]] bool Has(NodeValue::Type type) const;
  // 从表中移除指定的 NodeValue (第一个匹配项)
  void Remove(const NodeValue& v);

  // 清空表中的所有 NodeValue
  void Clear() { values_.clear(); }

  // 检查表是否为空
  [[nodiscard]] bool isEmpty() const { return values_.isEmpty(); }

  // 获取与给定类型列表和可选标签匹配的第一个 NodeValue 在表中的索引
  [[nodiscard]] int GetValueIndex(const QVector<NodeValue::Type>& type, const QString& tag) const;

  // (静态) 合并多个 NodeValueTable 列表为一个新的 NodeValueTable
  static NodeValueTable Merge(QList<NodeValueTable> tables);

 private:
  QVector<NodeValue> values_; // 存储 NodeValue 对象的动态数组
};

// 类型别名：NodeValueRow 代表一个从输入端口ID (QString) 到其对应 NodeValue 的映射。
// 这通常表示一个节点在特定时刻所有输入的值。
using NodeValueRow = QHash<QString, NodeValue>;

}  // namespace olive

// 声明 NodeValue 类型为元类型，以便在 QVariant 中使用或在信号槽中传递
Q_DECLARE_METATYPE(olive::NodeValue)
// 声明 NodeValueTable 类型为元类型
Q_DECLARE_METATYPE(olive::NodeValueTable)

#endif  // NODEVALUE_H