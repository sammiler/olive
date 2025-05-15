

#ifndef RENDERMODE_H
#define RENDERMODE_H

#include "common/define.h"

namespace olive {

class RenderMode {
 public:
  /**
   * @brief The primary different "modes" the renderer can function in
   */
  enum Mode {
    /**
     * This render is for realtime preview ONLY and does not need to be "perfect". Nodes can use lower-accuracy
     * functions to save performance when possible.
     */
    kOffline,

    /**
     * This render is some sort of export or master copy and Nodes should take time/bandwidth/system resources to
     * produce a higher accuracy version.
     */
    kOnline
  };
};

}  // namespace olive

#endif  // RENDERMODE_H
