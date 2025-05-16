#ifndef LIBOLIVECORE_PIXELFORMAT_H
#define LIBOLIVECORE_PIXELFORMAT_H

namespace olive::core { // Olive 核心功能命名空间

/**
 * @brief 封装了像素格式及其相关信息的类。
 *
 * PixelFormat 类定义了一系列支持的像素格式（通过 Format 枚举），
 * 并提供了获取每像素字节数以及判断格式是否为浮点型等实用功能。
 * 这对于图像处理和渲染管线中的内存分配和数据解释至关重要。
 */
class PixelFormat {
 public:
  /**
   * @brief 定义支持的像素格式。
   *
   * 这些枚举值代表了图像数据中单个像素颜色分量的存储方式和精度。
   */
  enum Format {
    INVALID = -1, ///< 无效或未指定的像素格式。
    U8,           ///< 无符号8位整数格式 (例如，每个颜色分量用一个字节表示，范围0-255)。
    U16,          ///< 无符号16位整数格式 (例如，每个颜色分量用两个字节表示，范围0-65535)。
    F16,          ///< 半精度浮点数格式 (16位浮点数)。
    F32,          ///< 单精度浮点数格式 (32位浮点数)。
    COUNT         ///< 有效像素格式的总数，也可用作迭代或数组大小的边界。
  };

  /**
   * @brief 显式构造函数。
   * @param f 初始的像素格式，默认为 INVALID。
   */
  explicit PixelFormat(Format f = INVALID) { f_ = f; }

  /**
   * @brief 类型转换运算符，允许将 PixelFormat 对象隐式或显式转换为其内部的 Format 枚举值。
   * @return 返回内部存储的 Format 枚举值。
   */
  explicit operator Format() const { return f_; }

  /**
   * @brief 获取指定像素格式每个颜色分量所占的字节数。
   * @param f 要查询的像素格式 (Format 枚举)。
   * @return 返回每个颜色分量的字节数；如果格式无效或为 COUNT，则返回0。
   */
  static int byte_count(Format f) {
    switch (f) {
      case INVALID: // 无效格式
      case COUNT:   // COUNT 不是一个实际的格式
        break;
      case U8: // 无符号8位整数
        return 1;
      case U16: // 无符号16位整数
      case F16: // 半精度浮点数 (16位)
        return 2;
      case F32: // 单精度浮点数 (32位)
        return 4;
    }
    return 0; // 对于未处理或无效的格式返回0
  }

  /**
   * @brief 获取当前 PixelFormat 对象实例所代表格式的每个颜色分量的字节数。
   * @return 返回每个颜色分量的字节数。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int byte_count() const { return byte_count(f_); }

  /**
   * @brief 检查指定的像素格式是否为浮点类型。
   * @param f 要检查的像素格式 (Format 枚举)。
   * @return 如果格式是 F16 或 F32，则返回 true；否则返回 false。
   */
  static bool is_float(Format f) {
    switch (f) {
      case INVALID: // 无效格式
      case COUNT:   // COUNT 不是一个实际的格式
      case U8:      // 无符号8位整数
      case U16:     // 无符号16位整数
        break;
      case F16: // 半精度浮点数
      case F32: // 单精度浮点数
        return true;
    }
    return false; // 默认为非浮点类型
  }

  /**
   * @brief 检查当前 PixelFormat 对象实例所代表的格式是否为浮点类型。
   * @return 如果格式是浮点类型，则返回 true；否则返回 false。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool is_float() const { return is_float(f_); }

 private:
  Format f_; ///< 内部存储的像素格式枚举值。
};

}  // namespace olive::core

#endif  // LIBOLIVECORE_PIXELFORMAT_H
