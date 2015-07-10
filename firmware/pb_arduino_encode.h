/* pb_arduino_encode.h -- helper functions to encode to an Arduino Print object
 *
 * 2013 Alejandro Morell Garcia <alejandro.morell@gmail.com>
 */

#ifndef PB_ARDUINO_ENCODE_H_
#define PB_ARDUINO_ENCODE_H_

// Make library cross-compatiable
// with Arduino, GNU C++ for tests, and Spark.
#if defined(ARDUINO) && ARDUINO >= 100
#include "Stream.h"
#elif defined(SPARK)
#include "spark_wiring_stream.h"
#endif

#include "pb_encode.h"

void pb_ostream_from_stream(Print &stream, pb_ostream_t &ostream);

#endif // PB_ARDUINO_ENCODE_H_
