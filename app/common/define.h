#ifndef OLIVECOMMONDEFINE_H
#define OLIVECOMMONDEFINE_H

namespace olive {

/// The minimum size an icon in ProjectExplorer can be
const int kProjectIconSizeMinimum = 16;

/// The maximum size an icon in ProjectExplorer can be
const int kProjectIconSizeMaximum = 256;

/// The default size an icon in ProjectExplorer can be
const int kProjectIconSizeDefault = 64;

const int kBytesInGigabyte = 1073741824;

}  // namespace olive

#define MACRO_NAME_AS_STR(s) #s
#define MACRO_VAL_AS_STR(s) MACRO_NAME_AS_STR(s)

#define OLIVE_NS_CONST_ARG(x, y) QArgument<const olive::x>("const " MACRO_VAL_AS_STR(olive) "::" #x, y)
#define OLIVE_NS_ARG(x, y) QArgument<olive::x>(MACRO_VAL_AS_STR(olive) "::" #x, y)
#define OLIVE_NS_RETURN_ARG(x, y) QReturnArgument<olive::x>(MACRO_VAL_AS_STR(olive) "::" #x, y)

/**
 * Copy/move deleters. Similar to Q_DISABLE_COPY_MOVE, et al. but those functions are not present in Qt < 5.13 so we
 * use our own functions for portability.
 */

#define DISABLE_COPY(Class)      \
  Class(const Class &) = delete; \
  Class &operator=(const Class &) = delete;

#define DISABLE_MOVE(Class) \
  Class(Class &&) = delete; \
  Class &operator=(Class &&) = delete;

#define DISABLE_COPY_MOVE(Class) \
  DISABLE_COPY(Class)            \
  DISABLE_MOVE(Class)

#endif  // OLIVECOMMONDEFINE_H
