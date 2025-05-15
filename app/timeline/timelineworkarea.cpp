#include "timelineworkarea.h"

#include "common/xmlutils.h"

namespace olive {

const rational TimelineWorkArea::kResetIn = rational(0);
const rational TimelineWorkArea::kResetOut = RATIONAL_MAX;

TimelineWorkArea::TimelineWorkArea(QObject *parent) : QObject(parent), workarea_enabled_(false) {}

bool TimelineWorkArea::enabled() const { return workarea_enabled_; }

void TimelineWorkArea::set_enabled(bool e) {
  workarea_enabled_ = e;
  emit EnabledChanged(workarea_enabled_);
}

const TimeRange &TimelineWorkArea::range() const { return workarea_range_; }

void TimelineWorkArea::set_range(const TimeRange &range) {
  workarea_range_ = range;
  emit RangeChanged(workarea_range_);
}

bool TimelineWorkArea::load(QXmlStreamReader *reader) {
  rational range_in = this->in();
  rational range_out = this->out();

  uint version = 0;
  XMLAttributeLoop(reader, attr) {
    if (attr.name() == QStringLiteral("version")) {
      version = attr.value().toUInt();
    }
  }
  Q_UNUSED(version)

  while (XMLReadNextStartElement(reader)) {
    if (reader->name() == QStringLiteral("enabled")) {
      this->set_enabled(reader->readElementText() != QStringLiteral("0"));
    } else if (reader->name() == QStringLiteral("in")) {
      range_in = rational::fromString(reader->readElementText().toUtf8().constData());
    } else if (reader->name() == QStringLiteral("out")) {
      range_out = rational::fromString(reader->readElementText().toUtf8().constData());
    } else {
      reader->skipCurrentElement();
    }
  }

  TimeRange loaded_workarea(range_in, range_out);

  if (loaded_workarea != this->range()) {
    this->set_range(loaded_workarea);
  }

  return true;
}

void TimelineWorkArea::save(QXmlStreamWriter *writer) const {
  writer->writeAttribute(QStringLiteral("version"), QString::number(1));

  writer->writeTextElement(QStringLiteral("enabled"), QString::number(this->enabled()));
  writer->writeTextElement(QStringLiteral("in"), QString::fromStdString(this->in().toString()));
  writer->writeTextElement(QStringLiteral("out"), QString::fromStdString(this->out().toString()));
}

const rational &TimelineWorkArea::in() const { return workarea_range_.in(); }

const rational &TimelineWorkArea::out() const { return workarea_range_.out(); }

const rational &TimelineWorkArea::length() const { return workarea_range_.length(); }

}  // namespace olive
