// 防止多重包含
#ifndef DEBUG_H
#define DEBUG_H

// 包含必要的头文件
#include <QDebug>

// 包含项目自定义的定义
#include "common/define.h"

// 命名空间开始
namespace olive {

/**
 * @brief 自定义调试信息处理器
 *
 * 该函数用于处理不同类型的调试信息，根据信息的类型和内容进行相应的处理。
 *
 * @param type 消息类型，如调试、警告、关键错误等
 * @param context 消息的上下文信息，包括文件名、行号等
 * @param msg 消息内容
 */
void DebugHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

}  // namespace olive

// 防止多重包含结束
#endif  // DEBUG_H
