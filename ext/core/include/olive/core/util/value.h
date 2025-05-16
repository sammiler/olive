#ifndef LIBOLIVECORE_VALUE_H
#define LIBOLIVECORE_VALUE_H

#include <stdint.h> // 引入标准整数类型定义，如 int64_t, uint8_t
#include <string.h> // 引入 C 风格字符串处理函数，如 memcpy, strlen
#include <map>      // 引入 std::map 容器
#include <string>   // 引入 std::string 类
#include <vector>   // 引入 std::vector 容器，用于存储原始数据

namespace olive::core { // Olive 核心功能命名空间

/**
 * @brief 通用类型容器类。
 *
 * Value 类设计为一个可以存储不同基本类型数据（如整数、浮点数、字符串）的容器。
 * 它内部使用一个字节向量 (std::vector<uint8_t>) 来存储原始数据，
 *并通过一个枚举 (Type) 来标识当前存储的数据类型。
 *这使得它可以在需要动态类型或异构数据存储的场景中使用，例如节点参数。
 */
class Value {
 public:
  /**
   * @brief 定义 Value 对象可以存储的数据类型。
   */
  enum Type {
    NONE,   ///< 空类型，表示没有数据或未初始化。
    INT,    ///< 有符号64位整数 (int64_t)。
    FLOAT,  ///< 双精度浮点数 (double)。
    STRING  ///< UTF-8 编码的字符串 (std::string 或 const char*)。
  };

  /**
   * @brief 默认构造函数。
   *
   * 创建一个类型为 NONE 的 Value 对象，不包含任何数据。
   */
  Value() { type_ = NONE; }

  /**
   * @brief 显式构造函数，用于创建一个存储 int64_t 整数的 Value 对象。
   * @param v 要存储的 int64_t 值。
   */
  explicit Value(int64_t v) {
    data_.resize(sizeof(int64_t)); // 调整内部数据向量的大小以容纳 int64_t
    memcpy(data_.data(), &v, sizeof(int64_t)); // 将整数的字节拷贝到数据向量中
    type_ = INT; // 设置类型为 INT
  }

  /**
   * @brief 显式构造函数，用于创建一个存储 double 浮点数的 Value 对象。
   * @param v 要存储的 double 值。
   */
  explicit Value(double v) {
    data_.resize(sizeof(double)); // 调整内部数据向量的大小以容纳 double
    // 注意：这里 memcpy 的大小是 sizeof(int64_t)，对于 double 应该是 sizeof(double)。这是一个潜在的bug。
    // 为了遵循“不要修改代码”的原则，此处保留原样，但标记出来。
    // 正确的应为: memcpy(data_.data(), &v, sizeof(double));
    memcpy(data_.data(), &v, sizeof(int64_t)); // 将浮点数的字节拷贝到数据向量中
    type_ = FLOAT; // 设置类型为 FLOAT
  }

  /**
   * @brief 显式构造函数，用于创建一个存储 C 风格字符串 (const char*) 的 Value 对象。
   * @param s 指向要存储的以 null 结尾的 C 风格字符串的指针。
   */
  explicit Value(const char *s) {
    size_t sz = strlen(s); // 获取字符串长度（不包括 null 终止符）
    data_.resize(sz);      // 调整内部数据向量的大小
    memcpy(data_.data(), s, sz); // 将字符串内容拷贝到数据向量中
    type_ = STRING; // 设置类型为 STRING
  }

  /**
   * @brief 显式构造函数，用于创建一个存储 std::string 的 Value 对象。
   * @param s 要存储的 std::string 对象。
   */
  explicit Value(const std::string &s) {
    data_.resize(s.size()); // 调整内部数据向量的大小以匹配字符串长度
    memcpy(data_.data(), s.data(), data_.size()); // 将字符串内容拷贝到数据向量中
    type_ = STRING; // 设置类型为 STRING
  }

  // 成员函数，例如 GetType(), AsInt(), AsFloat(), AsString() 等，在此头文件中未声明，
  // 但在一个完整的 Value 类实现中通常会存在，用于安全地获取存储的值。

 private:
  std::vector<uint8_t> data_; ///< 存储原始数据的字节向量。
  Type type_;                 ///< 标识 data_ 中存储的数据的实际类型。
};

/**
 * @brief 定义一个从字符串键到 Value 对象的映射类型。
 *
 * ValueMap 常用于存储一组命名的参数或属性，其中每个参数可以有不同的数据类型。
 */
using ValueMap = std::map<std::string, Value>;

}  // namespace olive::core

#endif  // LIBOLIVECORE_VALUE_H
