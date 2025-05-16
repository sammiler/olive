#ifndef LIBOLIVECORE_SAMPLEFORMAT_H
#define LIBOLIVECORE_SAMPLEFORMAT_H

#include <stdexcept> // 引入 stdexcept，用于 from_string 中的异常处理 (std::invalid_argument)
#include <string>    // 引入 std::string，用于格式的字符串表示

namespace olive::core { // Olive 核心功能命名空间

/**
 * @brief 封装音频采样格式及其相关属性和操作。
 *
 * SampleFormat 类定义了一系列支持的音频采样格式（通过 Format 枚举），
 * 例如无符号8位、有符号16位、32位浮点等，并区分了平面（planar）
 * 和交错（packed/interleaved）存储方式。
 * 它提供了获取每采样点字节数、字符串转换、格式类型判断等功能。
 */
class SampleFormat {
 public:
  /**
   * @brief 定义支持的音频采样格式。
   *
   * 枚举值代表了单个音频采样点的存储方式、数据类型和声道组织方式。
   * 'P' 后缀表示平面（Planar）格式，即每个声道的数据分开存储在不同的内存区域。
   * 没有 'P' 后缀的表示交错（Packed/Interleaved）格式，即所有声道的采样点交替存储。
   */
  enum Format {
    INVALID = -1, ///< 无效或未指定的采样格式。

    // Planar formats / 平面格式 (每个声道数据分开存储)
    U8P,  ///< 无符号8位整型，平面。
    S16P, ///< 有符号16位整型，平面。
    S32P, ///< 有符号32位整型，平面。
    S64P, ///< 有符号64位整型，平面。
    F32P, ///< 32位浮点型，平面。
    F64P, ///< 64位浮点型 (双精度)，平面。

    // Packed/Interleaved formats / 交错格式 (各声道采样点交替存储)
    U8,  ///< 无符号8位整型，交错。
    S16, ///< 有符号16位整型，交错。
    S32, ///< 有符号32位整型，交错。
    S64, ///< 有符号64位整型，交错。
    F32, ///< 32位浮点型，交错。
    F64, ///< 64位浮点型 (双精度)，交错。

    COUNT, ///< 有效采样格式的总数，也可用作迭代或数组大小的边界。

    // Helper enum values for range checks / 用于范围检查的辅助枚举值
    PLANAR_START = U8P,          ///< 平面格式枚举值的起始标记。
    PACKED_START = U8,           ///< 交错格式枚举值的起始标记。
    PLANAR_END = PACKED_START,   ///< 平面格式枚举值的结束标记 (不包含此值)。
    PACKED_END = COUNT,          ///< 交错格式枚举值的结束标记 (不包含此值)。
  };

  /**
   * @brief 显式构造函数。
   * @param f 初始的采样格式，默认为 INVALID。
   */
  explicit SampleFormat(Format f = INVALID) { f_ = f; }

  /**
   * @brief 类型转换运算符，允许将 SampleFormat 对象隐式或显式转换为其内部的 Format 枚举值。
   * @return 返回内部存储的 Format 枚举值。
   */
  explicit operator Format() const { return f_; }

  /**
   * @brief 获取指定采样格式每个采样点（单个声道）所占的字节数。
   * @param f 要查询的采样格式 (Format 枚举)。
   * @return 返回每个采样点的字节数；如果格式无效或为 COUNT，则返回0。
   */
  static int byte_count(Format f) {
    switch (f) {
      case U8:  // 无符号8位，交错
      case U8P: // 无符号8位，平面
        return 1;
      case S16: // 有符号16位，交错
      case S16P:// 有符号16位，平面
        return 2;
      case S32: // 有符号32位，交错
      case F32: // 32位浮点，交错
      case S32P:// 有符号32位，平面
      case F32P:// 32位浮点，平面
        return 4;
      case S64: // 有符号64位，交错
      case F64: // 64位浮点，交错
      case S64P:// 有符号64位，平面
      case F64P:// 64位浮点，平面
        return 8;
      case INVALID: // 无效格式
      case COUNT:   // COUNT 不是一个实际的格式
        break;
    }
    return 0; // 对于未处理或无效的格式返回0
  }

  /**
   * @brief 获取当前 SampleFormat 对象实例所代表格式的每个采样点（单个声道）的字节数。
   * @return 返回每个采样点的字节数。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int byte_count() const { return byte_count(f_); }

  /**
   * @brief 将指定的采样格式转换为其字符串表示形式。
   * @param f 要转换的采样格式 (Format 枚举)。
   * @return 返回格式的字符串表示 (例如 "u8p", "s16", "f32p")；如果格式无效或为 COUNT，则返回空字符串。
   */
  static std::string to_string(Format f) {
    switch (f) {
      case INVALID:
      case COUNT:
        break;
      case U8:
        return "u8";
      case S16:
        return "s16";
      case S32:
        return "s32";
      case S64:
        return "s64";
      case F32:
        return "f32";
      case F64:
        return "f64";
      case U8P:
        return "u8p";
      case S16P:
        return "s16p";
      case S32P:
        return "s32p";
      case S64P:
        return "s64p";
      case F32P:
        return "f32p";
      case F64P:
        return "f64p";
    }
    return ""; // 对于无效或 COUNT 返回空字符串
  }

  /**
   * @brief 将当前 SampleFormat 对象实例转换为其字符串表示形式。
   * @return 返回格式的字符串表示。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] std::string to_string() const { return to_string(f_); }

  /**
   * @brief 从字符串表示形式转换回 SampleFormat 对象。
   *
   * 此函数还包含对旧版整数序列化格式的兼容性处理（已弃用）。
   * @param s 采样格式的字符串表示 (例如 "u8p", "s16")。
   * @return 返回对应的 SampleFormat 对象；如果字符串无法识别，则返回 SampleFormat(INVALID)。
   */
  static SampleFormat from_string(const std::string &s) {
    if (s == "u8") {
      return SampleFormat(U8);
    } else if (s == "s16") {
      return SampleFormat(S16);
    } else if (s == "s32") {
      return SampleFormat(S32);
    } else if (s == "s64") {
      return SampleFormat(S64);
    } else if (s == "f32") {
      return SampleFormat(F32);
    } else if (s == "f64") {
      return SampleFormat(F64);
    } else if (s == "u8p") {
      return SampleFormat(U8P);
    } else if (s == "s16p") {
      return SampleFormat(S16P);
    } else if (s == "s32p") {
      return SampleFormat(S32P);
    } else if (s == "s64p") {
      return SampleFormat(S64P);
    } else if (s == "f32p") {
      return SampleFormat(F32P);
    } else if (s == "f64p") {
      return SampleFormat(F64P);
    } else {
      // 已弃用：采样格式过去曾被序列化为整数。在此处处理，但最终可能会移除此逻辑。
      try {
        int i = std::stoi(s); // 尝试将字符串转换为整数
        if (i > INVALID && i < COUNT) { // 检查整数值是否在有效枚举范围内
          return SampleFormat(static_cast<Format>(i)); // 转换为 Format 枚举并返回
        }
      } catch (const std::invalid_argument &e) {
        // 捕获 std::stoi 可能抛出的无效参数异常（如果字符串不是有效整数）
        // 此处不执行任何操作，将继续执行后续的返回 INVALID 的逻辑
      }

      // 从字符串反序列化失败
      return SampleFormat(INVALID);
    }
  }

  /**
   * @brief 检查指定的采样格式是否为交错（Packed）类型。
   * @param f 要检查的采样格式 (Format 枚举)。
   * @return 如果格式是交错类型，则返回 true；否则返回 false。
   */
  static bool is_packed(Format f) { return f >= PACKED_START && f < PACKED_END; }

  /**
   * @brief 检查当前 SampleFormat 对象实例所代表的格式是否为交错类型。
   * @return 如果格式是交错类型，则返回 true；否则返回 false。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool is_packed() const { return is_packed(f_); }

  /**
   * @brief 检查指定的采样格式是否为平面（Planar）类型。
   * @param f 要检查的采样格式 (Format 枚举)。
   * @return 如果格式是平面类型，则返回 true；否则返回 false。
   */
  static bool is_planar(Format f) { return f >= PLANAR_START && f < PLANAR_END; }

  /**
   * @brief 检查当前 SampleFormat 对象实例所代表的格式是否为平面类型。
   * @return 如果格式是平面类型，则返回 true；否则返回 false。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool is_planar() const { return is_planar(f_); }

  /**
   * @brief 将给定的 SampleFormat 转换为其等效的交错（Packed）格式。
   * 如果输入已经是交错格式，则直接返回输入。
   * @param fmt 要转换的 SampleFormat 对象。
   * @return 返回等效的交错格式 SampleFormat 对象；如果输入无效，则返回 SampleFormat(INVALID)。
   */
  static SampleFormat to_packed_equivalent(SampleFormat fmt) {
    switch (static_cast<Format>(fmt)) { // 将 SampleFormat 对象转换为其底层的 Format 枚举值进行比较
      // 对于交错格式输入，直接返回输入
      case U8:
      case S16:
      case S32:
      case S64:
      case F32:
      case F64:
        return fmt;

      // 将平面格式转换为对应的交错格式
      case U8P:
        return SampleFormat(U8);
      case S16P:
        return SampleFormat(S16);
      case S32P:
        return SampleFormat(S32);
      case S64P:
        return SampleFormat(S64);
      case F32P:
        return SampleFormat(F32);
      case F64P:
        return SampleFormat(F64);

      case INVALID: // 无效格式
      case COUNT:   // COUNT 不是一个实际的格式
        break;
    }
    return SampleFormat(INVALID); // 对于无效输入或无法转换的情况返回 INVALID
  }

  /**
   * @brief 将当前 SampleFormat 对象实例转换为其等效的交错格式。
   * @return 返回等效的交错格式 SampleFormat 对象。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] SampleFormat to_packed_equivalent() const { return to_packed_equivalent(SampleFormat(f_)); }

  /**
   * @brief 将给定的 SampleFormat 转换为其等效的平面（Planar）格式。
   * 如果输入已经是平面格式，则直接返回输入。
   * @param fmt 要转换的 SampleFormat 对象。
   * @return 返回等效的平面格式 SampleFormat 对象；如果输入无效，则返回 SampleFormat(INVALID)。
   */
  static SampleFormat to_planar_equivalent(SampleFormat fmt) {
    switch (static_cast<Format>(fmt)) { // 将 SampleFormat 对象转换为其底层的 Format 枚举值进行比较
      // 将交错格式转换为对应的平面格式
      case U8:
        return SampleFormat(U8P);
      case S16:
        return SampleFormat(S16P);
      case S32:
        return SampleFormat(S32P);
      case S64:
        return SampleFormat(S64P);
      case F32:
        return SampleFormat(F32P);
      case F64:
        return SampleFormat(F64P);

      // 对于平面格式输入，直接返回输入
      case U8P:
      case S16P:
      case S32P:
      case S64P:
      case F32P:
      case F64P:
        return fmt;

      case INVALID: // 无效格式
      case COUNT:   // COUNT 不是一个实际的格式
        break;
    }
    return SampleFormat(INVALID); // 对于无效输入或无法转换的情况返回 INVALID
  }

  /**
   * @brief 将当前 SampleFormat 对象实例转换为其等效的平面格式。
   * @return 返回等效的平面格式 SampleFormat 对象。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] SampleFormat to_planar_equivalent() const { return to_planar_equivalent(SampleFormat(f_)); }

 private:
  Format f_; ///< 内部存储的音频采样格式枚举值。
};

}  // namespace olive::core

#endif  // LIBOLIVECORE_SAMPLEFORMAT_H
