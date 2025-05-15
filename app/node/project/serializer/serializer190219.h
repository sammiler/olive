#ifndef PROJECTSERIALIZER190219_H
#define PROJECTSERIALIZER190219_H

#include "serializer.h"

namespace olive {

class ProjectSerializer190219 : public ProjectSerializer {
 public:
  ProjectSerializer190219() = default;

 protected:
  LoadData Load(Project *project, QXmlStreamReader *reader, LoadType load_type, void *reserved) const override;

  [[nodiscard]] uint Version() const override { return 190219; }
};

}  // namespace olive

#endif  // SERIALIZER190219_H
