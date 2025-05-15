#ifndef SUBTITLEPARAMS_H // 防止头文件被重复包含的宏
#define SUBTITLEPARAMS_H // 定义 SUBTITLEPARAMS_H 宏

#include <olive/core/core.h> // 包含 Olive 核心定义 (可能包含 TimeRange, rational)
#include <QRect>             // Qt 矩形类 (虽然在此文件中未直接使用，但字幕渲染时可能需要)
#include <QString>           // Qt 字符串类
#include <QXmlStreamReader>  // Qt XML 流读取器 (用于从XML加载字幕)
#include <QXmlStreamWriter>  // Qt XML 流写入器 (用于将字幕保存到XML)
#include <utility>           // 标准库 utility 头文件，提供 std::move
#include <vector>            // 标准库 vector 容器 (SubtitleParams 继承自它)

using namespace olive::core; // 使用 olive::core 命名空间中的类型 (如 TimeRange, rational)

namespace olive { // olive 项目的命名空间

/**
 * @brief Subtitle 类代表一个单独的字幕条目。
 *
 * 它包含字幕文本以及该字幕在时间轴上显示的时间范围。
 */
class Subtitle {
 public:
  // 默认构造函数
  Subtitle() = default;

  /**
   * @brief 构造函数，初始化字幕的时间范围和文本内容。
   * @param time 字幕显示的时间范围 (TimeRange)。
   * @param text 字幕的文本内容 (QString)。
   */
  Subtitle(const TimeRange &time, QString text) : range_(time), text_(std::move(text)) // 使用 std::move 提高效率
  {}

  /**
   * @brief 获取字幕显示的时间范围。
   * @return 返回 TimeRange 对象的常量引用。
   */
  [[nodiscard]] const TimeRange &time() const { return range_; }
  /**
   * @brief 设置字幕显示的时间范围。
   * @param t 新的时间范围。
   */
  void set_time(const TimeRange &t) { range_ = t; }

  /**
   * @brief 获取字幕的文本内容。
   * @return 返回 QString 对象的常量引用。
   */
  [[nodiscard]] const QString &text() const { return text_; }
  /**
   * @brief 设置字幕的文本内容。
   * @param t 新的文本内容。
   */
  void set_text(const QString &t) { text_ = t; }

 private:
  TimeRange range_; // 字幕显示的时间范围
  QString text_;    // 字幕的文本内容
};

/**
 * @brief SubtitleParams 类代表一组字幕参数，它是一个 Subtitle 对象的集合。
 *
 * 它继承自 std::vector<Subtitle>，因此拥有向量的所有标准操作。
 * 此外，它还包含了一些额外的元数据，如流索引 (`stream_index_`) 和启用状态 (`enabled_`)。
 * 这个类用于管理整个字幕轨道或字幕文件中的所有字幕条目。
 * 它还提供了加载和保存字幕数据到 XML 的功能，以及生成 ASS (Advanced SubStation Alpha) 字幕文件头的方法。
 */
class SubtitleParams : public std::vector<Subtitle> { // SubtitleParams 继承自 std::vector<Subtitle>
 public:
  // 构造函数，初始化流索引为0，启用状态为true
  SubtitleParams() {
    stream_index_ = 0;    // 默认流索引为0
    enabled_ = true;      // 默认启用字幕
  }

  /**
   * @brief (静态) 生成 ASS (Advanced SubStation Alpha) 字幕格式的文件头字符串。
   * ASS 是一种常见的字幕格式，其文件头包含样式、脚本信息等。
   * @return 返回包含 ASS 文件头内容的 QString。
   */
  static QString GenerateASSHeader();

  /**
   * @brief 从 XML 流中加载字幕参数和所有字幕条目。
   * @param reader 指向 QXmlStreamReader 的指针，用于读取 XML 数据。
   */
  void Load(QXmlStreamReader *reader);

  /**
   * @brief 将字幕参数和所有字幕条目保存到 XML 流中。
   * @param writer 指向 QXmlStreamWriter 的指针，用于写入 XML 数据。
   */
  void Save(QXmlStreamWriter *writer) const;

  /**
   * @brief 检查当前的字幕参数集合是否有效 (即是否包含任何字幕条目)。
   * @return 如果字幕列表不为空，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool is_valid() const { return !this->empty(); } // 使用 std::vector::empty()

  /**
   * @brief 获取整个字幕序列的总时长。
   * 通常计算为最后一个字幕条目的结束时间。
   * @return 返回 rational 类型的总时长。如果列表为空，则返回0。
   */
  [[nodiscard]] rational duration() const {
    if (this->empty()) { // 如果列表为空
      return rational(0); // 返回时长0
    } else {
      // 返回最后一个字幕条目的结束时间作为总时长
      return back().time().out(); // back() 获取最后一个元素
    }
  }

  /**
   * @brief 获取字幕流的索引。
   * 在包含多个字幕轨道的媒体文件中，每个字幕轨道会有一个索引。
   * @return 返回整数类型的流索引。
   */
  [[nodiscard]] int stream_index() const { return stream_index_; }
  /**
   * @brief 设置字幕流的索引。
   * @param i 新的流索引。
   */
  void set_stream_index(int i) { stream_index_ = i; }

  /**
   * @brief 检查字幕是否已启用。
   * @return 如果已启用，则返回 true。
   */
  [[nodiscard]] bool enabled() const { return enabled_; }
  /**
   * @brief 设置字幕的启用状态。
   * @param e 如果为 true，则启用字幕。
   */
  void set_enabled(bool e) { enabled_ = e; }

 private:
  int stream_index_; // 字幕流的索引 (例如，在多字幕轨道的视频文件中)
  bool enabled_;     // 标记此字幕轨道是否启用
};

}  // namespace olive

// 声明 Subtitle 类型为元类型，以便在 QVariant 中使用或在信号槽中传递
Q_DECLARE_METATYPE(olive::Subtitle)
// 声明 SubtitleParams 类型为元类型
Q_DECLARE_METATYPE(olive::SubtitleParams)

#endif  // SUBTITLEPARAMS_H