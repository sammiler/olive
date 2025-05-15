#ifndef PROJECTSERIALIZER230220_H
#define PROJECTSERIALIZER230220_H

#include "serializer.h"

namespace olive {

class ProjectSerializer230220 : public ProjectSerializer {
 public:
  ProjectSerializer230220() = default;

 protected:
  LoadData Load(Project *project, QXmlStreamReader *reader, LoadType load_type, void *reserved) const override;

  void Save(QXmlStreamWriter *writer, const SaveData &data, void *reserved) const override;

  [[nodiscard]] uint Version() const override { return 230220; }

 private:
  static void PostConnect(const QVector<Node *> &nodes, SerializedData *project_data);
};

}  // namespace olive

#endif  // PROJECTSERIALIZER230220_H
