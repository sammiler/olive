/***
Olive - 非线性视频编辑器
  Copyright (C) 2022 Olive Team

  本程序是自由软件：你可以重新分发它并且/或者修改它，遵循 GNU 通用公共许可证的条款，
  许可证版本为 3 或（根据你的选择）任何更新的版本。

  本程序的分发目的是希望它能发挥实用价值，
  但不提供任何担保，甚至不保证适销性或特定用途的适用性。详见 GNU 通用公共许可证。

  你应该已经收到一份 GNU 通用公共许可证的副本。
  如果没有，请访问 <http://www.gnu.org/licenses/> 获取。
***/

#ifndef UTIL_H
#define UTIL_H

/**
 * @brief 模板函数 mid
 *
 * 该函数用于计算两个值的中间值（平均值）。模板类型允许适用于各种数值类型。
 *
 * @tparam T 数值的类型，必须支持加法和除法操作。
 * @param a 第一个数值
 * @param b 第二个数值
 * @return 返回两数的平均值，即中间值
 */
template <typename T>
inline T mid(T a, T b) {
  return (a + b) * 0.5;
}

#endif  // UTIL_H
