#include <cstring>

#include "util/stringutils.h"
#include "util/tests.h"

using namespace olive::core;

bool stringutils_format_test() {
  const char *expected = "Hello, world!";
  std::string f = StringUtils::format("%s, %s!", "Hello", "world");
  if (strcmp(f.c_str(), expected) != 0) {
    return false;
  }

  return true;
}

int main() {
  Tester t;

  t.add("StringUtils::format", stringutils_format_test);

  return t.exec();
}
