#ifndef SHADERCODE_H  // 防止头文件被重复包含的宏
#define SHADERCODE_H  // 定义 SHADERCODE_H 宏

#include <utility>  // 标准库 utility 头文件，提供 std::move

#include "common/filefunctions.h"  // 包含文件功能相关的定义 (可能用于从文件加载着色器代码，
                                   // 虽然在此类中未直接使用，但逻辑上可能相关)
// 假设 QString 已通过其他方式被间接包含 (通常来自 <QString>)。

namespace olive {  // olive 项目的命名空间

/**
 * @brief ShaderCode 类用于封装顶点着色器 (Vertex Shader) 和片段着色器 (Fragment Shader) 的源代码。
 *
 * 它提供了一个简单的数据结构来存储这两部分的 GLSL (或其他着色器语言) 代码字符串。
 * 当节点需要提供自定义着色器逻辑时 (例如，通过 Node::GetShaderCode() 方法)，
 * 它们通常会返回一个 ShaderCode 对象实例。
 *
 * Renderer (如 OpenGLRenderer) 会使用这个 ShaderCode 对象中的代码来编译和链接
 * 一个完整的着色器程序。
 */
class ShaderCode {
 public:
  /**
   * @brief 构造函数。
   * @param frag_code (可选) 片段着色器的源代码字符串。默认为空字符串。
   * @param vert_code (可选) 顶点着色器的源代码字符串。默认为空字符串。
   */
  explicit ShaderCode(QString frag_code = QString(), QString vert_code = QString())
      : frag_code_(std::move(frag_code)),
        vert_code_(std::move(vert_code))  // 使用 std::move 提高效率 (对于QString可能效果有限，但好习惯)
  {}

  /**
   * @brief 获取片段着色器的源代码。
   * @return 返回片段着色器代码字符串的常量引用。
   */
  [[nodiscard]] const QString& frag_code() const { return frag_code_; }
  /**
   * @brief 设置片段着色器的源代码。
   * @param f 新的片段着色器代码字符串。
   */
  void set_frag_code(const QString& f) { frag_code_ = f; }

  /**
   * @brief 获取顶点着色器的源代码。
   * @return 返回顶点着色器代码字符串的常量引用。
   */
  [[nodiscard]] const QString& vert_code() const { return vert_code_; }
  /**
   * @brief 设置顶点着色器的源代码。
   * @param v 新的顶点着色器代码字符串。
   */
  void set_vert_code(const QString& v) { vert_code_ = v; }

 private:
  QString frag_code_;  // 存储片段着色器的源代码
  QString vert_code_;  // 存储顶点着色器的源代码
};

}  // namespace olive

#endif  // SHADERCODE_H