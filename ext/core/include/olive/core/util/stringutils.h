#ifndef LIBOLIVECORE_STRINGUTILS_H
#define LIBOLIVECORE_STRINGUTILS_H

#include <algorithm>  // 引入 STL 算法库，例如 std::find_if 用于 trim 函数
#include <cctype>     // 引入 isspace 等字符分类函数
#include <cstdarg>    // 引入可变参数宏 (va_list, va_start, va_end) 用于 format 函数
#include <cstdio>     // 引入 C 风格 IO 函数 (例如 vsnprintf) 用于 format 函数
#include <regex>      // 引入 C++ 正则表达式库
#include <string>     // 引入 std::string 类
#include <vector>     // 引入 std::vector 容器

namespace olive::core {  // Olive 核心功能命名空间

/**
 * @brief 提供字符串处理相关的静态工具函数。
 *
 * StringUtils 类包含了一系列用于常见字符串操作的静态方法，
 * 例如分割字符串、将字符串转换为整数、格式化字符串以及修剪字符串两端的空白字符。
 * 此类中的所有方法都是静态的，因此不需要创建 StringUtils 的实例即可使用。
 */
class StringUtils {
 public:
  /**
   * @brief 使用指定的分隔符将字符串分割成字符串列表。
   *
   * @param s 要分割的源字符串。
   * @param separator 用于分割字符串的字符。
   * @return 返回一个 std::vector<std::string>，其中包含按分隔符分割后的子字符串。
   */
  static std::vector<std::string> split(const std::string &s, char separator);

  /**
   * @brief 使用正则表达式将字符串分割成字符串列表。
   *
   * @param s 要分割的源字符串。
   * @param regex 用于分割字符串的 std::regex 正则表达式对象。
   * @return 返回一个 std::vector<std::string>，其中包含在正则表达式匹配处分割的子字符串。
   */
  static std::vector<std::string> split_regex(const std::string &s, const std::regex &regex);

  /**
   * @brief 将字符串转换为整数，使用 bool 指针指示成功与否，而不是抛出异常。
   *
   * @param s 要从中解析整数的字符串。
   * @param base 字符串中数字的进制 (通常为 10，或 16 表示十六进制)。
   * @param ok (可选) 一个布尔输出参数，指示转换是否成功。如果转换失败，*ok 将被设置为 false。
   * @return 如果解析成功，返回从字符串解析得到的整数；如果解析错误，则返回 0 (并将 *ok 设置为 false)。
   */
  static int to_int(const std::string &s, int base, bool *ok = nullptr);

  /**
   * @brief to_int 的重载函数，默认使用十进制进行转换。
   *
   * @param s 要从中解析整数的字符串。
   * @param ok (可选) 一个布尔输出参数，指示转换是否成功。
   * @return 如果解析成功，返回从字符串解析得到的整数；如果解析错误，则返回 0 (并将 *ok 设置为 false)。
   */
  static int to_int(const std::string &s, bool *ok = nullptr) { return to_int(s, 10, ok); }

  /**
   * @brief 将数字转换为带有左侧填充的字符串。
   *
   * 通常用于将数字转换为带有前导零的字符串。
   *
   * @tparam T 要转换的数字的类型 (例如 int, long, float, double 等)。
   * @param val 要转换的数字。
   * @param padding 字符串期望的总长度。例如，设置为 `2` 将确保字符串至少有2个字符长，
   * 必要时使用 `c` 字符在左侧填充。
   * @param c 用于填充的字符，默认为 '0'，假设此函数主要用于创建前导零。
   * @return 返回填充后的字符串。
   */
  template <typename T>
  static std::string to_string_leftpad(T val, size_t padding, char c = '0') {
    std::string s = std::to_string(val);  // 将数字转换为字符串

    if (s.size() < padding) {              // 如果字符串长度小于期望的填充长度
      s.insert(0, padding - s.size(), c);  // 在字符串开头插入所需数量的填充字符 c
    }

    return s;  // 返回处理后的字符串
  }

  /**
   * @brief 格式化字符串。
   *
   * 这是一个 sprintf 的包装器，返回一个 std::string。
   * 注意：使用可变参数时需要小心，确保格式字符串与后续参数类型和数量匹配，以避免未定义行为。
   * @param fmt C 风格的格式化字符串 (类似于 printf 的格式)。
   * @param ... 替换格式说明符的可变参数。
   * @return 返回格式化后的 std::string。
   */
  static std::string format(const char *fmt, ...);

  /**
   * @brief 从字符串的开头移除空白字符 (原地修改)。
   * @param s 要修剪的 std::string 引用。
   */
  static inline void ltrim(std::string &s) {
    // s.erase 会移除从 s.begin() 到第一个非空白字符的范围
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
  }

  /**
   * @brief 从字符串的末尾移除空白字符 (原地修改)。
   * @param s 要修剪的 std::string 引用。
   */
  static inline void rtrim(std::string &s) {
    // std::find_if 从反向迭代器开始查找第一个非空白字符
    // .base() 将反向迭代器转换为其对应的正向迭代器（指向找到的非空白字符的下一个位置）
    // s.erase 会移除从该位置到 s.end() 的范围
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
  }

  /**
   * @brief 从字符串的两端移除空白字符 (原地修改)。
   * @param s 要修剪的 std::string 引用。
   */
  static inline void trim(std::string &s) {
    rtrim(s);  // 先移除尾部空白
    ltrim(s);  // 再移除头部空白
  }

  /**
   * @brief 从字符串的开头移除空白字符 (返回副本)。
   * @param s 源 std::string。
   * @return 返回移除了头部空白的新 std::string。
   */
  static inline std::string ltrimmed(std::string s) {
    ltrim(s);  // 对副本进行原地修剪
    return s;  // 返回修剪后的副本
  }

  /**
   * @brief 从字符串的末尾移除空白字符 (返回副本)。
   * @param s 源 std::string。
   * @return 返回移除了尾部空白的新 std::string。
   */
  static inline std::string rtrimmed(std::string s) {
    rtrim(s);  // 对副本进行原地修剪
    return s;  // 返回修剪后的副本
  }

  /**
   * @brief 从字符串的两端移除空白字符 (返回副本)。
   * @param s 源 std::string。
   * @return 返回移除了两端空白的新 std::string。
   */
  static inline std::string trimmed(std::string s) {
    trim(s);   // 对副本进行原地修剪
    return s;  // 返回修剪后的副本
  }
};

}  // namespace olive::core

#endif  // LIBOLIVECORE_STRINGUTILS_H
