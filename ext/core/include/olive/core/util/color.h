#ifndef LIBOLIVECORE_COLOR_H
#define LIBOLIVECORE_COLOR_H

#include "../render/pixelformat.h"  // 引入 PixelFormat 类的定义，用于处理不同像素格式的颜色数据转换

namespace olive::core {  // Olive 核心功能命名空间

// 前向声明 PixelFormat，如果 "../render/pixelformat.h" 中已完整定义则无需再次声明
// class PixelFormat;

/**
 * @brief 基于高精度32位数据类型 (DataType) 的 RGBA 颜色值类。
 *
 * Color 类用于表示和操作 RGBA (红、绿、蓝、透明度) 颜色。
 * 它通常使用浮点数作为内部数据类型以保证精度，并提供了
 * 从不同格式的原始数据构造颜色、在不同颜色模型（RGB, HSV, HSL）之间转换、
 * 以及执行基本颜色算术运算的功能。
 */
class Color {
 public:
  using DataType = float;                  ///< 定义颜色分量的数据类型，此处为 float，表示单精度浮点数。
  static constexpr unsigned int RGBA = 4;  ///< 定义 RGBA 颜色包含的分量数量（红、绿、蓝、透明度）。

  /**
   * @brief 默认构造函数。
   *
   * 初始化所有颜色分量 (R, G, B, A) 为 0.0。
   */
  Color() {
    for (float& i : data_) {  // 遍历内部数据数组
      i = 0.0;                // 将每个分量设置为0.0
    }
  }

  /**
   * @brief 构造函数，使用指定的 R, G, B, A 值初始化颜色。
   * @param r 红色分量的值。
   * @param g 绿色分量的值。
   * @param b 蓝色分量的值。
   * @param a 透明度分量的值，默认为 1.0f (完全不透明)。
   */
  Color(const DataType& r, const DataType& g, const DataType& b, const DataType& a = 1.0f) {
    data_[0] = r;  // 设置红色分量
    data_[1] = g;  // 设置绿色分量
    data_[2] = b;  // 设置蓝色分量
    data_[3] = a;  // 设置透明度分量
  }

  /**
   * @brief 构造函数，从原始字符数据、像素格式和通道布局创建颜色。
   *
   * 此构造函数的具体实现未在此头文件中给出，但其目的是从
   * 不同格式的像素数据（例如从图像文件读取的）中提取颜色信息。
   * @param data 指向原始字符数据的指针。
   * @param format 原始数据的像素格式 (PixelFormat)。
   * @param ch_layout 通道布局（例如，指示 RGB、BGR、RGBA、BGRA 等顺序）。
   */
  Color(const char* data, const PixelFormat& format, int ch_layout);

  /**
   * @brief 静态工厂方法，从 HSV (色相、饱和度、明度) 值创建 Color 对象。
   *
   * 色相 (Hue) 期望的值范围是 0.0 到 360.0。
   * 饱和度 (Saturation) 和明度 (Value) 期望的值范围是 0.0 到 1.0。
   * @param h 色相值。
   * @param s 饱和度值。
   * @param v 明度值。
   * @return 返回根据 HSV 值创建的 Color 对象。
   */
  static Color fromHsv(const DataType& h, const DataType& s, const DataType& v);

  /** @brief 获取红色分量的值。 @return 红色分量值的常量引用。 [[nodiscard]] */
  [[nodiscard]] const DataType& red() const { return data_[0]; }
  /** @brief 获取绿色分量的值。 @return 绿色分量值的常量引用。 [[nodiscard]] */
  [[nodiscard]] const DataType& green() const { return data_[1]; }
  /** @brief 获取蓝色分量的值。 @return 蓝色分量值的常量引用。 [[nodiscard]] */
  [[nodiscard]] const DataType& blue() const { return data_[2]; }
  /** @brief 获取透明度分量的值。 @return 透明度分量值的常量引用。 [[nodiscard]] */
  [[nodiscard]] const DataType& alpha() const { return data_[3]; }

  /**
   * @brief 将当前颜色（RGB）转换为 HSV (色相、饱和度、明度) 表示。
   * @param hue 指向用于存储计算得到的色相值的指针。
   * @param sat 指向用于存储计算得到的饱和度值的指针。
   * @param val 指向用于存储计算得到的明度值的指针。
   */
  void toHsv(DataType* hue, DataType* sat, DataType* val) const;

  /**
   * @brief 获取当前颜色的 HSV 色相值。
   * @return 返回色相值 (通常在 0.0 到 360.0 之间)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] DataType hsv_hue() const;

  /**
   * @brief 获取当前颜色的 HSV 饱和度值。
   * @return 返回饱和度值 (通常在 0.0 到 1.0 之间)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] DataType hsv_saturation() const;

  /**
   * @brief 获取当前颜色的 HSV 明度值 (Value)。
   * @return 返回明度值 (通常在 0.0 到 1.0 之间)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] DataType value() const;

  /**
   * @brief 将当前颜色（RGB）转换为 HSL (色相、饱和度、亮度) 表示。
   * @param hue 指向用于存储计算得到的色相值的指针。
   * @param sat 指向用于存储计算得到的饱和度值的指针。
   * @param lightness 指向用于存储计算得到的亮度值的指针。
   */
  void toHsl(DataType* hue, DataType* sat, DataType* lightness) const;

  /**
   * @brief 获取当前颜色的 HSL 色相值。
   * @return 返回色相值 (通常在 0.0 到 360.0 之间)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] DataType hsl_hue() const;

  /**
   * @brief 获取当前颜色的 HSL 饱和度值。
   * @return 返回饱和度值 (通常在 0.0 到 1.0 之间)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] DataType hsl_saturation() const;

  /**
   * @brief 获取当前颜色的 HSL 亮度值 (Lightness)。
   * @return 返回亮度值 (通常在 0.0 到 1.0 之间)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] DataType lightness() const;

  /** @brief 设置红色分量的值。 @param red 新的红色分量值。 */
  void set_red(const DataType& red) { data_[0] = red; }
  /** @brief 设置绿色分量的值。 @param green 新的绿色分量值。 */
  void set_green(const DataType& green) { data_[1] = green; }
  /** @brief 设置蓝色分量的值。 @param blue 新的蓝色分量值。 */
  void set_blue(const DataType& blue) { data_[2] = blue; }
  /** @brief 设置透明度分量的值。 @param alpha 新的透明度分量值。 */
  void set_alpha(const DataType& alpha) { data_[3] = alpha; }

  /**
   * @brief 获取指向内部 RGBA 数据数组的指针（可修改）。
   * @return 返回指向 DataType 数组的指针。
   */
  DataType* data() { return data_; }

  /**
   * @brief 获取指向内部 RGBA 数据数组的指针（不可修改）。
   * @return 返回指向 DataType 数组的 const 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const DataType* data() const { return data_; }

  /**
   * @brief 将颜色数据根据指定的像素格式和通道数写入到外部字符缓冲区。
   * @param out 指向目标字符缓冲区的指针。
   * @param format 要转换成的目标像素格式。
   * @param nb_channels 要写入的通道数量 (例如，3 表示 RGB，4 表示 RGBA)。
   */
  void toData(char* out, const PixelFormat& format, unsigned int nb_channels) const;

  /**
   * @brief 静态方法，从原始字符数据、像素格式和通道数创建 Color 对象。
   * @param in 指向源字符数据的指针。
   * @param format 源数据的像素格式。
   * @param nb_channels 源数据中的通道数量。
   * @return 返回根据原始数据创建的 Color 对象。
   */
  static Color fromData(const char* in, const PixelFormat& format, unsigned int nb_channels);

  /**
   * @brief 获取一个粗略的亮度值，主要用于 UI 判断（例如，决定覆盖文本使用黑色还是白色）。
   *
   * 这通常是一个简化的亮度计算，不一定符合严格的色彩学标准。
   * @return 返回一个 DataType 类型的粗略亮度值。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] DataType GetRoughLuminance() const;

  // Assignment math operators / 赋值算术运算符
  /** @brief 颜色加法赋值。 @param rhs 右操作数 (另一个 Color 对象)。 @return 修改后的 Color 对象引用。 */
  Color& operator+=(const Color& rhs);
  /** @brief 颜色减法赋值。 @param rhs 右操作数 (另一个 Color 对象)。 @return 修改后的 Color 对象引用。 */
  Color& operator-=(const Color& rhs);
  /** @brief 颜色与标量加法赋值 (所有分量加标量)。 @param rhs 右操作数 (DataType 标量)。 @return 修改后的 Color
   * 对象引用。 */
  Color& operator+=(const DataType& rhs);
  /** @brief 颜色与标量减法赋值 (所有分量减标量)。 @param rhs 右操作数 (DataType 标量)。 @return 修改后的 Color
   * 对象引用。 */
  Color& operator-=(const DataType& rhs);
  /** @brief 颜色与标量乘法赋值 (所有分量乘标量)。 @param rhs 右操作数 (DataType 标量)。 @return 修改后的 Color
   * 对象引用。 */
  Color& operator*=(const DataType& rhs);
  /** @brief 颜色与标量除法赋值 (所有分量除以标量)。 @param rhs 右操作数 (DataType 标量)。 @return 修改后的 Color
   * 对象引用。 */
  Color& operator/=(const DataType& rhs);

  // Binary math operators / 二元算术运算符
  /** @brief 颜色加法。 @param rhs 右操作数。 @return 新的 Color 对象结果。 */
  Color operator+(const Color& rhs) const {
    Color c(*this);  // 创建副本
    c += rhs;        // 使用加法赋值运算符
    return c;        // 返回结果
  }

  /** @brief 颜色减法。 @param rhs 右操作数。 @return 新的 Color 对象结果。 */
  Color operator-(const Color& rhs) const {
    Color c(*this);  // 创建副本
    c -= rhs;        // 使用减法赋值运算符
    return c;        // 返回结果
  }

  /** @brief 颜色与标量加法。 @param rhs 右操作数。 @return 新的 Color 对象结果。 */
  Color operator+(const DataType& rhs) const {
    Color c(*this);  // 创建副本
    c += rhs;        // 使用加法赋值运算符
    return c;        // 返回结果
  }

  /** @brief 颜色与标量减法。 @param rhs 右操作数。 @return 新的 Color 对象结果。 */
  Color operator-(const DataType& rhs) const {
    Color c(*this);  // 创建副本
    c -= rhs;        // 使用减法赋值运算符
    return c;        // 返回结果
  }

  /** @brief 颜色与标量乘法。 @param rhs 右操作数。 @return 新的 Color 对象结果。 */
  Color operator*(const DataType& rhs) const {
    Color c(*this);  // 创建副本
    c *= rhs;        // 使用乘法赋值运算符
    return c;        // 返回结果
  }

  /** @brief 颜色与标量除法。 @param rhs 右操作数。 @return 新的 Color 对象结果。 */
  Color operator/(const DataType& rhs) const {
    Color c(*this);  // 创建副本
    c /= rhs;        // 使用除法赋值运算符
    return c;        // 返回结果
  }

 private:
  /**
   * @brief 内部数据存储，一个包含 RGBA 四个颜色分量的 DataType 数组。
   * 初始化为0。
   */
  DataType data_[RGBA]{};
};

}  // namespace olive::core

#endif  // LIBOLIVECORE_COLOR_H
