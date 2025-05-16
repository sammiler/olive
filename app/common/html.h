#ifndef HTML_H
#define HTML_H

#include <QMap>              // 用于CSS属性映射
#include <QStringList>       // 字符串列表
#include <QTextBlock>        // QTextDocument 的组成部分
#include <QTextBlockFormat>  // 块格式
#include <QTextCharFormat>   // 字符格式
#include <QTextDocument>
#include <QTextFragment>
#include <QVector>               // 用于 kBlockTags
#include <QXmlStreamAttributes>  // XML属性
#include <QXmlStreamReader>      // 用于解析HTML (作为XML处理)
#include <QXmlStreamWriter>      // 用于生成HTML (作为XML处理)

namespace olive {

/**
 * @brief 提供 HTML 与 QTextDocument 之间相互转换功能的工具类。
 *
 * Qt 本身包含此类转换函数，但它们存在一些局限性。我们希望支持的某些特性
 * (例如字间距/字符间距调整和字体拉伸) 在 Qt 的 QTextHtmlExporter 和
 * QTextHtmlParser 中并未实现。此外，由于这些函数并非 Qt 公共 API 的一部分，
 * 并且大量引用了 Qt 其他非公共 API 的部分，因此无法在不整体派生 (fork) Qt
 * 的情况下对其功能进行子类化或扩展。
 *
 * 因此，有必要编写一个自定义类来进行这种转换，以确保我们能够支持所需的功能。
 *
 * 如果有人希望扩展此类以支持更多功能，欢迎提交拉取请求 (pull request)。
 * 但请注意，此类并非旨在成为一个详尽的 HTML 实现，其主要设计目标是
 * 以标准格式存储富文本，用于视频文本格式化。
 */
class Html {
 public:
  /**
   * @brief 将 QTextDocument 对象转换为 HTML 字符串。
   * @param doc 指向要转换的 QTextDocument 对象的常量指针。
   * @return QString 生成的 HTML 字符串。
   */
  static QString DocToHtml(const QTextDocument *doc);

  /**
   * @brief 将 HTML 字符串内容解析并应用到指定的 QTextDocument 对象。
   * @param doc 指向要填充内容的 QTextDocument 对象的指针。
   * @param html 包含要解析的 HTML 内容的字符串。
   */
  static void HtmlToDoc(QTextDocument *doc, const QString &html);

 private:
  /**
   * @brief 将 QTextBlock 对象的内容和格式写入到 QXmlStreamWriter。
   * @param writer 指向 QXmlStreamWriter 的指针，用于写入 XML/HTML 数据。
   * @param block 要写入的 QTextBlock 对象。
   */
  static void WriteBlock(QXmlStreamWriter *writer, const QTextBlock &block);

  /**
   * @brief 将 QTextFragment 对象的内容和格式写入到 QXmlStreamWriter。
   * @param writer 指向 QXmlStreamWriter 的指针，用于写入 XML/HTML 数据。
   * @param fragment 要写入的 QTextFragment 对象。
   */
  static void WriteFragment(QXmlStreamWriter *writer, const QTextFragment &fragment);

  /**
   * @brief 将一个 CSS 属性（键和值列表）写入到样式字符串中。
   * @param style 指向目标样式字符串的指针，CSS 属性将追加到此字符串。
   * @param key CSS 属性的键名 (例如 "font-family")。
   * @param value CSS 属性的值列表 (例如 {"Arial", "sans-serif"})。
   */
  static void WriteCSSProperty(QString *style, const QString &key, const QStringList &value);

  /**
   * @brief 将一个 CSS 属性（键和单个值）写入到样式字符串中。
   * 此为 WriteCSSProperty 的重载版本，方便处理单个值的属性。
   * @param style 指向目标样式字符串的指针，CSS 属性将追加到此字符串。
   * @param key CSS 属性的键名。
   * @param value CSS 属性的单个值。
   */
  static void WriteCSSProperty(QString *style, const QString &key, const QString &value) {
    WriteCSSProperty(style, key, QStringList({value}));
  }

  /**
   * @brief 将 QTextCharFormat 对象的格式信息转换为 CSS 样式并写入到样式字符串。
   * @param style 指向目标样式字符串的指针。
   * @param fmt 要转换的 QTextCharFormat 对象。
   */
  static void WriteCharFormat(QString *style, const QTextCharFormat &fmt);

  /**
   * @brief 从 QXmlStreamAttributes (通常是 HTML 标签的属性) 中读取样式信息，并转换为 QTextCharFormat。
   * @param attributes 包含样式属性的 QXmlStreamAttributes 对象。
   * @return QTextCharFormat 根据读取的属性生成的字符格式对象。
   */
  static QTextCharFormat ReadCharFormat(const QXmlStreamAttributes &attributes);

  /**
   * @brief 从 QXmlStreamAttributes 中读取样式信息，并转换为 QTextBlockFormat。
   * @param attributes 包含样式属性的 QXmlStreamAttributes 对象。
   * @return QTextBlockFormat 根据读取的属性生成的文本块格式对象。
   */
  static QTextBlockFormat ReadBlockFormat(const QXmlStreamAttributes &attributes);

  /**
   * @brief 将一个字符串追加到另一个字符串，并根据需要自动在它们之间添加空格。
   * @param s 指向目标字符串的指针。
   * @param append 要追加的字符串。
   */
  static void AppendStringAutoSpace(QString *s, const QString &append);

  /**
   * @brief 从 CSS 样式字符串 (例如 "color: red; font-size: 12px;") 中解析出 CSS 属性键值对。
   * @param s 包含 CSS 样式的字符串。
   * @return QMap<QString, QStringList> 一个映射，键是 CSS 属性名，值是该属性对应的值列表。
   */
  static QMap<QString, QStringList> GetCSSFromStyle(const QString &s);

  /**
   * @brief 存储被认为是块级 HTML 标签的名称的向量。
   * 例如 "p", "div", "h1" 等。
   */
  static const QVector<QString> kBlockTags;
};

}  // namespace olive

#endif  // HTML_H