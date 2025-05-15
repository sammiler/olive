

#include <cstring>

#include "util/tests.h"
#include "util/timecodefunctions.h"

using namespace olive::core;

bool timecodefunctions_time_to_timecode_test() {
  rational drop_frame_30(1001, 30000);

  std::string timecode = Timecode::time_to_timecode(rational(1), drop_frame_30, Timecode::kTimecodeDropFrame);
  if (strcmp(timecode.c_str(), "00:00:01;00") != 0) {
    return false;
  }

  return true;
}

bool timecodefunctions_time_to_timecode_test2() {
  rational bizarre_timebase(156632219);

  std::string timecode = Timecode::time_to_timecode(rational(0), bizarre_timebase, Timecode::kTimecodeDropFrame);
  if (strcmp(timecode.c_str(), "INVALID TIMEBASE") != 0) {
    return false;
  }

  return true;
}

int main() {
  Tester t;

  t.add("Timecode::time_to_timecode", timecodefunctions_time_to_timecode_test);
  t.add("Timecode::time_to_timecode2", timecodefunctions_time_to_timecode_test2);

  return t.exec();
}
