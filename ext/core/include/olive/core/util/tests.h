#ifndef LIBOLIVECORE_TESTS_H
#define LIBOLIVECORE_TESTS_H

#include <list>     // 引入 std::list，用于存储测试名称和测试函数指针
#include <string>   // 引入 std::string (虽然在此文件中未直接使用，但 echo 函数的实现可能会用到)
#include <cstdio>   // 引入 C 风格 IO 函数 (例如 vprintf) 用于 echo 函数
#include <cstdarg>  // 引入可变参数宏 (va_list, va_start, va_end) 用于 echo 函数

namespace olive::core { // Olive 核心功能命名空间

/**
 * @brief 一个简单的测试运行器类。
 *
 * Tester 类允许注册一系列测试函数，然后统一运行它们，并报告结果。
 * 每个测试函数都应该是一个返回 bool（表示测试通过或失败）且不接受参数的函数。
 */
class Tester {
public:
  /**
   * @brief 默认构造函数。
   */
  Tester() = default;

  /**
   * @brief 定义测试函数的函数指针类型。
   *
   * test_t 是一个指向不接受参数并返回 bool 类型的函数的指针。
   * 返回 true 表示测试通过，false 表示测试失败。
   */
  typedef bool (*test_t)();

  /**
   * @brief 添加一个测试用例。
   *
   * @param name 测试用例的名称 (C 风格字符串)，用于在输出中标识测试。
   * @param test_function 指向实际测试逻辑的函数指针 (类型为 test_t)。
   */
  void add(const char *name, test_t test_function) {
    test_names_.push_back(name);         // 将测试名称添加到名称列表中
    test_functions_.push_back(test_function); // 将测试函数指针添加到函数列表中
  }

  /**
   * @brief 运行所有已添加的测试用例。
   *
   * 此函数会遍历所有注册的测试函数，执行它们，并输出每个测试的通过/失败状态。
   * @return 如果所有测试都通过，则返回 true；只要有一个测试失败，就返回 false。
   */
  bool run();

  /**
   * @brief 执行所有测试并返回一个适合作为程序退出代码的值。
   * @return 如果所有测试通过，返回 0；否则返回 1。
   */
  int exec() {
    if (run()) { // 运行所有测试
      return 0;  // 所有测试通过，返回0
    } else {
      return 1;  // 至少一个测试失败，返回1
    }
  }

  /**
   * @brief 静态方法，用于在测试执行期间输出格式化的消息。
   *
   * 类似于 printf，但通常输出到标准输出或标准错误，用于显示测试信息或调试输出。
   * @param fmt C 风格的格式化字符串。
   * @param ... 替换格式说明符的可变参数。
   */
  static void echo(const char *fmt, ...);

private:
  std::list<const char *> test_names_; ///< 存储测试用例名称的列表。
  std::list<test_t> test_functions_;   ///< 存储测试用例函数指针的列表。
};

}  // namespace olive::core

#endif  // LIBOLIVECORE_TESTS_H
