#ifndef GENERATEJOB_H  // 防止头文件被重复包含的宏
#define GENERATEJOB_H  // 定义 GENERATEJOB_H 宏

#include "acceleratedjob.h"  // 包含 AcceleratedJob 基类的定义
#include "codec/frame.h"     // 包含 Frame (或 FramePtr) 相关的定义，
                             // 虽然在此类中未直接使用 Frame 成员，但生成器节点通常会产生帧数据。

namespace olive {  // olive 项目的命名空间

/**
 * @brief GenerateJob 类代表一个“生成”类型的加速任务。
 *
 * 它继承自 AcceleratedJob，因此可以携带执行任务所需的参数值 (通过 `value_map_`)。
 * "生成"任务通常指的是那些不依赖于外部输入纹理或文件，而是根据参数程序化地创建
 * 图像或数据的节点。例如：
 * - 纯色生成器 (Solid Color)
 * - 渐变生成器 (Gradient)
 * - 噪点生成器 (Noise)
 * - 文本生成器 (Text)
 *
 * 当 NodeTraverser 处理这类生成器节点时，会创建一个 GenerateJob，并将节点参数
 * (如颜色值、渐变类型、噪点参数等) 存入其 `value_map_`。
 * 随后，渲染器 (例如 NodeTraverser 的 ProcessFrameGeneration 方法，或具体节点的 GenerateFrame 方法)
 * 会使用这个 Job 中的参数来生成最终的图像数据。
 *
 * 这个类本身结构比较简单，其主要功能和数据承载通过继承自 AcceleratedJob 实现。
 */
class GenerateJob : public AcceleratedJob {  // GenerateJob 继承自 AcceleratedJob
 public:
  // 默认构造函数
  GenerateJob() = default;

  /**
   * @brief 构造函数，使用一个 NodeValueRow 初始化任务参数。
   * NodeValueRow 中包含了执行生成操作所需的参数值。
   * @param row 包含任务参数的 NodeValueRow。
   */
  explicit GenerateJob(const NodeValueRow &row) : GenerateJob() { Insert(row); }  // 调用默认构造函数后，插入参数
  // 这里的 : GenerateJob() 是委托构造函数 (C++11)
  // 它会先调用默认构造函数 GenerateJob()，
  // 然后再执行当前构造函数体内的 Insert(row)。
};

}  // namespace olive

#endif  // GENERATEJOB_H