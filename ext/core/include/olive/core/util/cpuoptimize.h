

#ifndef LIBOLIVECORE_CPUOPTIMIZE_H
#define LIBOLIVECORE_CPUOPTIMIZE_H

#if defined(__x86_64__) || defined(__i386__)
#define OLIVE_PROCESSOR_X86
#include <xmmintrin.h>
#elif defined(__aarch64__)
#define OLIVE_PROCESSOR_ARM
#include "sse2neon.h"
#endif

#endif  // LIBOLIVECORE_CPUOPTIMIZE_H
