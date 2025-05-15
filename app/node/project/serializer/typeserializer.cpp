

#include "typeserializer.h"

namespace olive {

AudioParams TypeSerializer::LoadAudioParams(QXmlStreamReader *reader) {
  AudioParams a;

  while (XMLReadNextStartElement(reader)) {
    if (reader->name() == QStringLiteral("samplerate")) {
      a.set_sample_rate(reader->readElementText().toInt());
    } else if (reader->name() == QStringLiteral("channellayout")) {
      a.set_channel_layout(reader->readElementText().toULongLong());
    } else if (reader->name() == QStringLiteral("format")) {
      a.set_format(SampleFormat::from_string(reader->readElementText().toUtf8().constData()));
    } else if (reader->name() == QStringLiteral("enabled")) {
      a.set_enabled(reader->readElementText().toInt());
    } else if (reader->name() == QStringLiteral("streamindex")) {
      a.set_stream_index(reader->readElementText().toInt());
    } else if (reader->name() == QStringLiteral("duration")) {
      a.set_duration(reader->readElementText().toLongLong());
    } else if (reader->name() == QStringLiteral("timebase")) {
      a.set_time_base(rational::fromString(reader->readElementText().toUtf8().constData()));
    } else {
      reader->skipCurrentElement();
    }
  }

  return a;
}

void TypeSerializer::SaveAudioParams(QXmlStreamWriter *writer, const AudioParams &a) {
  writer->writeTextElement(QStringLiteral("samplerate"), QString::number(a.sample_rate()));
  writer->writeTextElement(QStringLiteral("channellayout"), QString::number(a.channel_layout()));
  writer->writeTextElement(QStringLiteral("format"), QString::fromStdString(a.format().to_string()));
  writer->writeTextElement(QStringLiteral("enabled"), QString::number(a.enabled()));
  writer->writeTextElement(QStringLiteral("streamindex"), QString::number(a.stream_index()));
  writer->writeTextElement(QStringLiteral("duration"), QString::number(a.duration()));
  writer->writeTextElement(QStringLiteral("timebase"), QString::fromStdString(a.time_base().toString()));
}

}  // namespace olive
