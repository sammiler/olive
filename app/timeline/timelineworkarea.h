

#ifndef TIMELINEWORKAREA_H
#define TIMELINEWORKAREA_H

#include <olive/core/core.h>
#include <QObject>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace olive {

using namespace core;

class TimelineWorkArea : public QObject {
  Q_OBJECT
 public:
  explicit TimelineWorkArea(QObject* parent = nullptr);

  [[nodiscard]] bool enabled() const;
  void set_enabled(bool e);

  [[nodiscard]] const rational& in() const;
  [[nodiscard]] const rational& out() const;
  [[nodiscard]] const rational& length() const;
  [[nodiscard]] const TimeRange& range() const;
  void set_range(const TimeRange& range);

  bool load(QXmlStreamReader* reader);
  void save(QXmlStreamWriter* writer) const;

  static const rational kResetIn;
  static const rational kResetOut;

 signals:
  void EnabledChanged(bool e);

  void RangeChanged(const TimeRange& r);

 private:
  bool workarea_enabled_;

  TimeRange workarea_range_;
};

}  // namespace olive

#endif  // TIMELINEWORKAREA_H
