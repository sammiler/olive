

#ifndef CONFORMTASK_H
#define CONFORMTASK_H

#include "codec/decoder.h"
#include "node/project/footage/footage.h"
#include "task/task.h"

namespace olive {

class ConformTask : public Task {
  Q_OBJECT
 public:
  ConformTask(QString decoder_id, const Decoder::CodecStream &stream, AudioParams params,
              const QVector<QString> &output_filenames);

 protected:
  bool Run() override;

 private:
  QString decoder_id_;

  Decoder::CodecStream stream_;

  AudioParams params_;

  QVector<QString> output_filenames_;
};

}  // namespace olive

#endif  // CONFORMTASK_H
