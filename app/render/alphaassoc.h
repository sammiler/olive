#ifndef ALPHAASSOC_H  // 防止头文件被重复包含的宏
#define ALPHAASSOC_H  // 定义 ALPHAASSOC_H 宏

namespace olive {  // olive 项目的命名空间

/**
 * @brief AlphaAssociated 枚举定义了Alpha通道的不同关联类型。
 *
 * 在图像处理和计算机图形学中，Alpha通道可以与颜色分量 (R, G, B) 以不同的方式关联：
 * - **Unassociated Alpha (非关联Alpha / 直通Alpha / unpremultiplied alpha):**
 *   RGB分量存储的是原始颜色值，Alpha通道仅表示透明度。进行混合时，需要先将RGB乘以Alpha。
 *   例如，一个半透明红色像素可能是 (R=1, G=0, B=0, A=0.5)。
 * - **Associated Alpha (关联Alpha / 预乘Alpha / premultiplied alpha):**
 *   RGB分量已经预先乘以了Alpha值。这可以简化某些混合操作，并能正确表示某些发光或半透明效果。
 *   例如，上述半透明红色像素在预乘Alpha下会是 (R=0.5, G=0, B=0, A=0.5)。
 * - **None (无Alpha):**
 *   图像没有Alpha通道，或者Alpha通道应被忽略 (图像完全不透明)。
 *
 * 正确处理Alpha关联类型对于实现准确的图像混合和合成至关重要。
 */
enum AlphaAssociated {
  kAlphaNone,          // 无Alpha通道或Alpha应被忽略 (图像不透明)
  kAlphaUnassociated,  // 非关联Alpha (直通Alpha / unpremultiplied alpha)
  kAlphaAssociated     // 关联Alpha (预乘Alpha / premultiplied alpha)
};

}  // namespace olive

#endif  // ALPHAASSOC_H