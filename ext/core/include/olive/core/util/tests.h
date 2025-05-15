

#ifndef LIBOLIVECORE_TESTS_H
#define LIBOLIVECORE_TESTS_H

#include <list>

namespace olive::core {

class Tester {
 public:
  Tester() = default;

  typedef bool (*test_t)();

  void add(const char *name, test_t test_function) {
    test_names_.push_back(name);
    test_functions_.push_back(test_function);
  }

  bool run();

  int exec() {
    if (run()) {
      return 0;
    } else {
      return 1;
    }
  }

  static void echo(const char *fmt, ...);

 private:
  std::list<const char *> test_names_;
  std::list<test_t> test_functions_;
};

}  // namespace olive::core

#endif  // LIBOLIVECORE_TESTS_H
