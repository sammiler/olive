#ifndef LIBOLIVECORE_RATIONAL_H
#define LIBOLIVECORE_RATIONAL_H

// 以 C 链接方式包含 FFmpeg libavutil 库中的 rational 头文件。
// extern "C" 确保C++编译器以C语言的方式处理这个头文件中的声明，
// 这对于链接C库 (如 FFmpeg) 是必需的。
extern "C" {
#include <libavutil/rational.h>  // FFmpeg 中用于表示有理数的结构 AVRational
}

#include <iostream>   // 引入 iostream 用于 std::ostream 的友元函数 operator<<
#include <limits>     // 引入 std::numeric_limits，可能用于 RATIONAL_MIN/MAX 的定义或内部实现
#include <stdexcept>  // 引入 stdexcept 用于 fromString 中的异常处理 (虽然在此文件中未直接看到，但 std::stoi 可能抛出)
#include <string>     // 引入 std::string 用于 toString 和 fromString

#ifdef USE_OTIO                     // 仅当启用了 OpenTimelineIO 支持时编译以下部分
#include <opentime/rationalTime.h>  // 引入 OpenTimelineIO 中的 RationalTime 类
#endif

namespace olive::core {  // Olive 核心功能命名空间

/**
 * @brief 表示有理数（分数）的类，用于精确计算。
 *
 * rational 类封装了一个分子 (numerator) 和一个分母 (denominator)，
 * 并提供了算术运算、比较运算、类型转换（到 double, AVRational, OpenTimelineIO::RationalTime）
 * 以及字符串转换等功能。它内部使用 FFmpeg 的 AVRational 结构进行存储，
 * 并确保分数始终以约简和符号标准化的形式存在。
 */
class rational {
 public:
  /**
   * @brief 显式构造函数，从一个整数创建有理数（分母为1）。
   * @param numerator 分子，默认为0。
   */
  explicit rational(const int &numerator = 0) {
    r_.num = numerator;  // 设置分子
    r_.den = 1;          // 设置分母为1
  }

  /**
   * @brief 构造函数，从指定的分子和分母创建有理数。
   *
   * 创建后会立即进行符号修正和约简。
   * @param numerator 分子。
   * @param denominator 分母。如果为0，则表示一个无效的有理数 (NaN)。
   */
  rational(const int &numerator, const int &denominator) {
    r_.num = numerator;
    r_.den = denominator;

    fix_signs();  // 修正符号，通常确保分母为正
    reduce();     // 将分数约简到最简形式
  }

  /**
   * @brief 默认拷贝构造函数。
   * @param rhs 要拷贝的另一个 rational 对象。
   */
  rational(const rational &rhs) = default;

  /**
   * @brief 显式构造函数，从 FFmpeg 的 AVRational 结构创建 rational 对象。
   * @param r 一个 AVRational 结构体。
   */
  explicit rational(const AVRational &r) {
    r_ = r;
    fix_signs();  // 构造后修正符号
  }

  /**
   * @brief 计算有理数的绝对值。
   * @param r 输入的有理数。
   * @return 返回 r 的绝对值。
   */
  static rational qAbs(const rational &r) {
    if (r >= rational(0)) {  // 如果 r 大于或等于 0
      return r;
    }
    return -r;  // 返回 r 的相反数 (需要 rational 支持一元负号运算符)
  }

  /**
   * @brief 静态工厂方法，从双精度浮点数创建 rational 对象。
   *
   * 尝试找到最接近给定浮点数的有理数表示。
   * @param flt 要转换的双精度浮点数。
   * @param ok 可选的 bool 指针，用于指示转换是否成功。如果转换失败（例如，输入是 NaN 或无穷大），*ok 会被设为 false。
   * @return 返回转换得到的 rational 对象。
   */
  static rational fromDouble(const double &flt, bool *ok = nullptr);

  /**
   * @brief 静态工厂方法，从字符串（例如 "num/den" 格式）创建 rational 对象。
   * @param str 包含有理数表示的字符串。
   * @param ok 可选的 bool 指针，用于指示转换是否成功。
   * @return 返回转换得到的 rational 对象。
   */
  static rational fromString(const std::string &str, bool *ok = nullptr);

  /**
   * @brief 表示“非数字”（Not a Number）的静态常量有理数。
   * 通常其分母为0。
   */
  static const rational NaN;

  // Assignment Operators / 赋值运算符
  /** @brief 赋值运算符。 @param rhs 右操作数。 @return 当前对象的引用。 */
  const rational &operator=(const rational &rhs);
  /** @brief 加法赋值运算符。 @param rhs 右操作数。 @return 当前对象的引用。 */
  const rational &operator+=(const rational &rhs);
  /** @brief 减法赋值运算符。 @param rhs 右操作数。 @return 当前对象的引用。 */
  const rational &operator-=(const rational &rhs);
  /** @brief 除法赋值运算符。 @param rhs 右操作数。 @return 当前对象的引用。 */
  const rational &operator/=(const rational &rhs);
  /** @brief 乘法赋值运算符。 @param rhs 右操作数。 @return 当前对象的引用。 */
  const rational &operator*=(const rational &rhs);

  // Binary math operators / 二元算术运算符
  /** @brief 加法运算符。 @param rhs 右操作数。 @return 新的 rational 对象作为结果。 */
  rational operator+(const rational &rhs) const;
  /** @brief 减法运算符。 @param rhs 右操作数。 @return 新的 rational 对象作为结果。 */
  rational operator-(const rational &rhs) const;
  /** @brief 除法运算符。 @param rhs 右操作数。 @return 新的 rational 对象作为结果。 */
  rational operator/(const rational &rhs) const;
  /** @brief 乘法运算符。 @param rhs 右操作数。 @return 新的 rational 对象作为结果。 */
  rational operator*(const rational &rhs) const;

  // Relational and equality operators / 关系和相等运算符
  /** @brief 小于运算符。 @param rhs 右操作数。 @return 如果当前对象小于 rhs 则返回 true。 */
  bool operator<(const rational &rhs) const;
  /** @brief 小于等于运算符。 @param rhs 右操作数。 @return 如果当前对象小于等于 rhs 则返回 true。 */
  bool operator<=(const rational &rhs) const;
  /** @brief 大于运算符。 @param rhs 右操作数。 @return 如果当前对象大于 rhs 则返回 true。 */
  bool operator>(const rational &rhs) const;
  /** @brief 大于等于运算符。 @param rhs 右操作数。 @return 如果当前对象大于等于 rhs 则返回 true。 */
  bool operator>=(const rational &rhs) const;
  /** @brief 等于运算符。 @param rhs 右操作数。 @return 如果当前对象等于 rhs 则返回 true。 */
  bool operator==(const rational &rhs) const;
  /** @brief 不等于运算符。 @param rhs 右操作数。 @return 如果当前对象不等于 rhs 则返回 true。 */
  bool operator!=(const rational &rhs) const;

  // Unary operators / 一元运算符
  /** @brief 一元正号运算符。 @return 当前对象的常量引用。 */
  const rational &operator+() const { return *this; }
  /** @brief 一元负号运算符（取反）。 @return 一个新的 rational 对象，其值为当前对象的相反数。 */
  rational operator-() const {
    return rational(-r_.num, r_.den);
  }  // 修正：通常相反数是 (-num, den) 或 (num, -den) 后规范化
  /** @brief 逻辑非运算符。 @return 如果分子为0，则返回 true；否则返回 false。 */
  bool operator!() const { return !r_.num; }

  /**
   * @brief 将有理数转换为双精度浮点数。
   * @return 返回有理数的浮点表示。如果分母为0，行为可能未定义或返回 NaN/Infinity。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] double toDouble() const;

  /**
   * @brief 将 rational 对象转换为 FFmpeg 的 AVRational 结构。
   * @return 返回等效的 AVRational 结构。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] AVRational toAVRational() const;

#ifdef USE_OTIO  // 仅当启用了 OpenTimelineIO 支持时编译以下部分
  /**
   * @brief 静态工厂方法，从 OpenTimelineIO 的 RationalTime 对象创建 rational 对象。
   *
   * 注意：注释中提到实现方式可能不是最佳的（通过转换为秒再转回）。
   * @param t 要转换的 opentime::RationalTime 对象。
   * @return 返回转换得到的 rational 对象。
   */
  static rational fromRationalTime(const opentime::RationalTime &t) {
    // 这种转换方式是否最佳？
    return fromDouble(t.to_seconds());
  }

  /**
   * @brief 将 rational 对象转换为 OpenTimelineIO 的 RationalTime 对象。
   * @param framerate 目标 RationalTime 的帧率，默认为 24。
   * @return 返回转换得到的 opentime::RationalTime 对象。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] opentime::RationalTime toRationalTime(double framerate = 24) const;
#endif

  /**
   * @brief 返回此有理数的“翻转”版本（即分子和分母互换）。
   * @return 返回一个新的 rational 对象，其值为原始有理数的倒数。
   * 如果原始分子为0，则翻转后的有理数分母为0 (NaN)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] rational flipped() const;

  /**
   * @brief 就地“翻转”此有理数（分子和分母互换）。
   * 如果原始分子为0，则翻转后分母为0 (NaN)。
   */
  void flip();

  /**
   * @brief 检查有理数是否为零（分子为0）。
   *
   * 注意：NaN（分母为0）的有理数如果分子也为0，则此函数也会返回 true。
   * 一个 NaN 总是一个空值 (isNull)，但一个空值不总是一个 NaN。
   * @return 如果分子为0，则返回 true；否则返回 false。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool isNull() const { return r_.num == 0; }

  /**
   * @brief 检查此有理数是否为“非数字”（NaN）。
   *
   * 当分母为0时，有理数被认为是 NaN。
   * @return 如果分母为0，则返回 true；否则返回 false。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool isNaN() const { return r_.den == 0; }

  /**
   * @brief 获取分子。
   * @return 返回分子的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const int &numerator() const { return r_.num; }

  /**
   * @brief 获取分母。
   * @return 返回分母的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const int &denominator() const { return r_.den; }

  /**
   * @brief 将有理数转换为字符串表示形式（例如 "num/den"）。
   * @return 返回有理数的 std::string 表示。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] std::string toString() const;

  /**
   * @brief 重载流输出运算符，用于将 rational 对象输出到 std::ostream。
   * @param out 输出流。
   * @param value 要输出的 rational 对象。
   * @return 返回输出流的引用。
   */
  friend std::ostream &operator<<(std::ostream &out, const rational &value) {
    out << value.r_.num << '/' << value.r_.den;  // 以 "num/den" 格式输出
    return out;
  }

 private:
  /**
   * @brief 修正有理数的符号。
   *
   * 通常确保分母为正。如果分子和分母都为负，则都转为正。
   * 如果分母为负而分子为正，则将负号移至分子。
   */
  void fix_signs();

  /**
   * @brief 将有理数约简到最简形式。
   *
   * 通过分子和分母除以它们的最大公约数来实现。
   * 如果分母为0，则不进行操作。
   */
  void reduce();

  AVRational r_{};  ///< 内部存储，使用 FFmpeg 的 AVRational 结构体 {int num, int den}。
};

/**
 * @brief 定义有理数的最小值（使用 INT_MIN 作为分子，1作为分母）。
 */
#define RATIONAL_MIN rational(INT_MIN)

/**
 * @brief 定义有理数的最大值（使用 INT_MAX 作为分子，1作为分母）。
 */
#define RATIONAL_MAX rational(INT_MAX)

}  // namespace olive::core

#endif  // LIBOLIVECORE_RATIONAL_H
