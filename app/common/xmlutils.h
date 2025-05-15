

#ifndef XMLREADLOOP_H
#define XMLREADLOOP_H

#include <QXmlStreamReader>
#include <QXmlStreamAttribute> // 为了 QXmlStreamAttribute，在宏中展开后使用
#include <QVector>          // 为了 QXmlStreamAttributes::attributes() 返回类型 (通常是 QVector)

#include "node/param.h"        // (虽然包含，但在此头文件中的直接代码未使用其内容)
#include "render/cancelatom.h" // 包含 CancelAtom
#include "undo/undocommand.h"  // (虽然包含，但在此头文件中的直接代码未使用其内容)

namespace olive {

// 前向声明 (如果需要)
class Block;
class Node;
class NodeInput;
class NodeGroup;

/**
 * @brief 用于方便地迭代 QXmlStreamReader 当前元素所有属性的宏。
 *
 * 示例用法:
 * @code
 * QXmlStreamReader reader;
 * // ... reader 定位到一个元素 ...
 * XMLAttributeLoop(&reader, attribute) {
 * QString name = attribute.name().toString();
 * QString value = attribute.value().toString();
 * // 处理属性 name 和 value
 * }
 * @endcode
 *
 * @param reader 指向 QXmlStreamReader 对象的指针。
 * @param item 在循环中代表当前 QXmlStreamAttribute 对象的变量名。
 */
#define XMLAttributeLoop(reader, item) foreach (const QXmlStreamAttribute& item, reader->attributes())

/**
 * @brief QXmlStreamReader::readNextStartElement 的一个替代实现，用于解决其在文档末尾检测问题。
 *
 * Qt 自带的 QXmlStreamReader::readNextStartElement 函数在到达文档末尾时不会正确退出，
 * 常常会导致抛出“文档过早结束 (premature end of document)”的错误。
 * 此自定义函数旨在实现与 readNextStartElement 类似的功能，但修正了这个问题。
 *
 * @see https://stackoverflow.com/questions/46346450/qt-qxmlstreamreader-always-returns-premature-end-of-document-error
 *
 * @param reader 指向 QXmlStreamReader 对象的指针，将从此读取器中读取下一个开始元素。
 * @param cancel_atom (可选) 指向 CancelAtom 对象的指针。如果在读取过程中此原子被标记为取消，
 * 函数可能会提前返回 false。默认为 nullptr，表示不检查取消。
 * @return bool 如果成功读取到下一个开始元素，则返回 true。
 * 如果到达文档末尾、发生错误或被取消，则返回 false。
 */
bool XMLReadNextStartElement(QXmlStreamReader* reader, CancelAtom* cancel_atom = nullptr);

}  // namespace olive

#endif  // XMLREADLOOP_H