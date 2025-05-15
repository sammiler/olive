

#ifndef CRASHPADUTILS_H
#define CRASHPADUTILS_H

#include <client/crashpad_client.h>

// Copied from base::FilePath to match its macro
#if BUILDFLAG(IS_POSIX)
// On most platforms, native pathnames are char arrays, and the encoding
// may or may not be specified.  On Mac OS X, native pathnames are encoded
// in UTF-8.
#define QSTRING_TO_BASE_STRING(x) x.toUtf8().constData()
#define BASE_STRING_TO_QSTRING(x) QString::fromStdString(x)
#elif BUILDFLAG(IS_WIN)
// On Windows, for Unicode-aware applications, native pathnames are wchar_t
// arrays encoded in UTF-16.
#define QSTRING_TO_BASE_STRING(x) x.toStdWString()
#define BASE_STRING_TO_QSTRING(x) QString::fromStdWString(x)
#endif  // BUILDFLAG(IS_WIN)

#endif  // CRASHPADUTILS_H
