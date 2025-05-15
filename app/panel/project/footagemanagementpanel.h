#ifndef FOOTAGEMANAGEMENTPANEL_H
#define FOOTAGEMANAGEMENTPANEL_H

#include <QList>

#include "node/project/footage/footage.h"

namespace olive {

class FootageManagementPanel {
 public:
  [[nodiscard]] virtual QVector<ViewerOutput *> GetSelectedFootage() const = 0;
};

}  // namespace olive

#endif  // FOOTAGEMANAGEMENTPANEL_H
