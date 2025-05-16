#ifndef LOG_H
#define LOG_H

#include <iostream> // 引入 iostream 以使用 std::cerr 和 std::endl

namespace olive::core { // Olive 核心功能命名空间

/**
 * @brief 一个简单的流式日志记录类。
 *
 * Log 类提供了一种方便的方式来向标准错误流 (std::cerr) 输出带有类型前缀的日志消息。
 * 它通过重载 operator<< 来支持链式输出，并在对象销毁时自动添加换行符。
 * 通常通过静态工厂方法 (Debug(), Info(), Warning(), Error()) 来创建和使用。
 *
 * 使用示例:
 * olive::core::Log::Info() << "这是一条信息" << 123;
 * olive::core::Log::Error() << "发生了一个错误，代码：" << error_code;
 */
class Log {
public:
  /**
   * @brief 显式构造函数。
   *
   * 构造 Log 对象时，会立即向 std::cerr 输出指定的日志类型前缀。
   * 例如，如果 type 是 "DEBUG"，则会输出 "[DEBUG]"。
   * @param type 指向表示日志类型的 C 风格字符串 (例如 "DEBUG", "INFO")。
   */
  explicit Log(const char *type) { std::cerr << "[" << type << "]"; }

  /**
   * @brief 析构函数。
   *
   * 当 Log 对象生命周期结束时（通常在日志语句的末尾），
   * 此析构函数会自动向 std::cerr 输出一个换行符 (std::endl)，
   * 确保每条日志消息都以新行结束。
   */
  ~Log() { std::cerr << std::endl; }

  /**
   * @brief 重载流插入运算符 (<<)。
   *
   * 允许将各种类型的数据链式地追加到日志消息中。
   * 每个追加的数据前会有一个空格。
   * @tparam T 要追加的数据的类型。
   * @param t 要追加到日志消息的数据。
   * @return 返回对当前 Log 对象的引用，以支持链式操作。
   */
  template <typename T>
  Log &operator<<(const T &t) {
    std::cerr << " " << t; // 在数据前添加空格并输出到 std::cerr
    return *this;          // 返回自身引用以支持链式调用
  }

  /**
   * @brief 创建一个 DEBUG 级别的日志记录器。
   * @return 返回一个 Log 对象，其类型前缀为 "[DEBUG]"。
   */
  static Log Debug() { return Log("DEBUG"); }

  /**
   * @brief 创建一个 INFO 级别的日志记录器。
   * @return 返回一个 Log 对象，其类型前缀为 "[INFO]"。
   */
  static Log Info() { return Log("INFO"); }

  /**
   * @brief 创建一个 WARNING 级别的日志记录器。
   * @return 返回一个 Log 对象，其类型前缀为 "[WARNING]"。
   */
  static Log Warning() { return Log("WARNING"); }

  /**
   * @brief 创建一个 ERROR 级别的日志记录器。
   * @return 返回一个 Log 对象，其类型前缀为 "[ERROR]"。
   */
  static Log Error() { return Log("ERROR"); }
};

}  // namespace olive::core

#endif  // LOG_H
