/* pb_arduino_decode.h -- helper functions to decode from an Arduino Stream
 *
 * 2013 Alejandro Morell Garcia <alejandro.morell@gmail.com>
 */

 // Make library cross-compatiable
 // with Arduino, GNU C++ for tests, and Spark.
 #if defined(ARDUINO) && ARDUINO >= 100
 #include "Stream.h"
 #elif defined(SPARK)
 #include "spark_wiring_stream.h"
 #endif

#ifndef PB_ARDUINO_DECODE_H_
#define PB_ARDUINO_DECODE_H_

#include "pb_decode.h"

void pb_istream_from_stream(Stream &stream, pb_istream_t &istream);

#endif // PB_ARDUINO_DECODE_H_
