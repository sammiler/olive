#ifndef SUBTITLEPARAMS_H
#define SUBTITLEPARAMS_H

#include <olive/core/core.h>
#include <QRect>
#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <utility>

using namespace olive::core;

namespace olive {

class Subtitle {
 public:
  Subtitle() = default;

  Subtitle(const TimeRange &time, QString text) : range_(time), text_(std::move(text)) {}

  [[nodiscard]] const TimeRange &time() const { return range_; }
  void set_time(const TimeRange &t) { range_ = t; }

  [[nodiscard]] const QString &text() const { return text_; }
  void set_text(const QString &t) { text_ = t; }

 private:
  TimeRange range_;

  QString text_;
};

class SubtitleParams : public std::vector<Subtitle> {
 public:
  SubtitleParams() {
    stream_index_ = 0;
    enabled_ = true;
  }

  static QString GenerateASSHeader();

  void Load(QXmlStreamReader *reader);

  void Save(QXmlStreamWriter *writer) const;

  [[nodiscard]] bool is_valid() const { return !this->empty(); }

  [[nodiscard]] rational duration() const {
    if (this->empty()) {
      return rational(0);
    } else {
      return back().time().out();
    }
  }

  [[nodiscard]] int stream_index() const { return stream_index_; }
  void set_stream_index(int i) { stream_index_ = i; }

  [[nodiscard]] bool enabled() const { return enabled_; }
  void set_enabled(bool e) { enabled_ = e; }

 private:
  int stream_index_;

  bool enabled_;
};

}  // namespace olive

Q_DECLARE_METATYPE(olive::Subtitle)
Q_DECLARE_METATYPE(olive::SubtitleParams)

#endif  // SUBTITLEPARAMS_H
