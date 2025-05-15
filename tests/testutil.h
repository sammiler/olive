#include <iostream>

#define OLIVE_TEST_SUCCESS -1

#define OLIVE_ASSERT(x) if (!(x)) return __LINE__
#define OLIVE_ASSERT_EQUAL(x, y) if (x != y) {std::cout << " - Equal assert failed: " << x << " != " << y; return __LINE__;}void()
#define OLIVE_TEST_END return OLIVE_TEST_SUCCESS

#define OLIVE_ADD_TEST(x) int Test##x()
#define OLIVE_ADD_DISABLED_TEST(x) int Test##x()
