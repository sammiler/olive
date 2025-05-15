#include "conform.h"

#include <utility>

namespace olive {

ConformTask::ConformTask(QString decoder_id, const Decoder::CodecStream &stream, AudioParams params,
                         const QVector<QString> &output_filenames)
    : decoder_id_(std::move(decoder_id)),
      stream_(stream),
      params_(std::move(params)),
      output_filenames_(output_filenames) {
  SetTitle(tr("Conforming Audio %1:%2").arg(stream.filename(), QString::number(stream.stream())));
}

bool ConformTask::Run() {
  DecoderPtr decoder = Decoder::CreateFromID(decoder_id_);

  if (!decoder->Open(stream_)) {
    SetError(tr("Failed to open decoder for audio conform"));
    return false;
  }

  connect(decoder.get(), &Decoder::IndexProgress, this, &ConformTask::ProgressChanged);

  qDebug() << "Starting conform of" << stream_.filename() << stream_.stream();

  bool ret = decoder->ConformAudio(output_filenames_, params_, GetCancelAtom());

  decoder->Close();

  return ret;
}

}  // namespace olive
