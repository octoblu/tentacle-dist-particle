/* Common parts of the nanopb library. Most of these are quite low-level
 * stuff. For the high-level interface, see pb_encode.h and pb_decode.h.
 */

 typedef enum _Action {
     Action_digitalRead = 0,
     Action_digitalReadPullup = 1,
     Action_digitalWrite = 2,
     Action_analogRead = 3,
     Action_analogReadPullup = 4,
     Action_analogWrite = 5,
     Action_servoWrite = 6,
     Action_pwmWrite = 7,
     Action_i2cWrite = 8,
     Action_i2cRead = 9,
     Action_ignore = 10
 } Action;

#ifndef PB_H_INCLUDED
#define PB_H_INCLUDED

/*****************************************************************
 * Nanopb compilation time options. You can change these here by *
 * uncommenting the lines, or on the compiler command line.      *
 *****************************************************************/

/* Enable support for dynamically allocated fields */
/* #define PB_ENABLE_MALLOC 1 */

/* Define this if your CPU architecture is big endian, i.e. it
 * stores the most-significant byte first. */
/* #define __BIG_ENDIAN__ 1 */

/* Define this if your CPU / compiler combination does not support
 * unaligned memory access to packed structures. */
/* #define PB_NO_PACKED_STRUCTS 1 */

/* Increase the number of required fields that are tracked.
 * A compiler warning will tell if you need this. */
/* #define PB_MAX_REQUIRED_FIELDS 256 */

/* Add support for tag numbers > 255 and fields larger than 255 bytes. */
/* #define PB_FIELD_16BIT 1 */

/* Add support for tag numbers > 65536 and fields larger than 65536 bytes. */
/* #define PB_FIELD_32BIT 1 */

/* Disable support for error messages in order to save some code space. */
/* #define PB_NO_ERRMSG 1 */

/* Disable support for custom streams (support only memory buffers). */
/* #define PB_BUFFER_ONLY 1 */

/* Switch back to the old-style callback function signature.
 * This was the default until nanopb-0.2.1. */
/* #define PB_OLD_CALLBACK_STYLE */


/******************************************************************
 * You usually don't need to change anything below this line.     *
 * Feel free to look around and use the defined macros, though.   *
 ******************************************************************/


/* Version of the nanopb library. Just in case you want to check it in
 * your own program. */
#define NANOPB_VERSION nanopb-0.3.3

/* Include all the system headers needed by nanopb. You will need the
 * definitions of the following:
 * - strlen, memcpy, memset functions
 * - [u]int8_t, [u]int16_t, [u]int32_t, [u]int64_t
 * - size_t
 * - bool
 *
 * If you don't have the standard header files, you can instead provide
 * a custom header that defines or includes all this. In that case,
 * define PB_SYSTEM_HEADER to the path of this file.
 */
#ifdef PB_SYSTEM_HEADER
#include PB_SYSTEM_HEADER
#else
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#ifdef PB_ENABLE_MALLOC
#include <stdlib.h>
#endif
#endif

/* Macro for defining packed structures (compiler dependent).
 * This just reduces memory requirements, but is not required.
 */
#if defined(PB_NO_PACKED_STRUCTS)
    /* Disable struct packing */
#   define PB_PACKED_STRUCT_START
#   define PB_PACKED_STRUCT_END
#   define pb_packed
#elif defined(__GNUC__) || defined(__clang__)
    /* For GCC and clang */
#   define PB_PACKED_STRUCT_START
#   define PB_PACKED_STRUCT_END
#   define pb_packed __attribute__((packed))
#elif defined(__ICCARM__) || defined(__CC_ARM)
    /* For IAR ARM and Keil MDK-ARM compilers */
#   define PB_PACKED_STRUCT_START _Pragma("pack(push, 1)")
#   define PB_PACKED_STRUCT_END _Pragma("pack(pop)")
#   define pb_packed
#elif defined(_MSC_VER) && (_MSC_VER >= 1500)
    /* For Microsoft Visual C++ */
#   define PB_PACKED_STRUCT_START __pragma(pack(push, 1))
#   define PB_PACKED_STRUCT_END __pragma(pack(pop))
#   define pb_packed
#else
    /* Unknown compiler */
#   define PB_PACKED_STRUCT_START
#   define PB_PACKED_STRUCT_END
#   define pb_packed
#endif

/* Handly macro for suppressing unreferenced-parameter compiler warnings. */
#ifndef PB_UNUSED
#define PB_UNUSED(x) (void)(x)
#endif

/* Compile-time assertion, used for checking compatible compilation options.
 * If this does not work properly on your compiler, use
 * #define PB_NO_STATIC_ASSERT to disable it.
 *
 * But before doing that, check carefully the error message / place where it
 * comes from to see if the error has a real cause. Unfortunately the error
 * message is not always very clear to read, but you can see the reason better
 * in the place where the PB_STATIC_ASSERT macro was called.
 */
#ifndef PB_NO_STATIC_ASSERT
#ifndef PB_STATIC_ASSERT
#define PB_STATIC_ASSERT(COND,MSG) typedef char PB_STATIC_ASSERT_MSG(MSG, __LINE__, __COUNTER__)[(COND)?1:-1];
#define PB_STATIC_ASSERT_MSG(MSG, LINE, COUNTER) PB_STATIC_ASSERT_MSG_(MSG, LINE, COUNTER)
#define PB_STATIC_ASSERT_MSG_(MSG, LINE, COUNTER) pb_static_assertion_##MSG##LINE##COUNTER
#endif
#else
#define PB_STATIC_ASSERT(COND,MSG)
#endif

/* Number of required fields to keep track of. */
#ifndef PB_MAX_REQUIRED_FIELDS
#define PB_MAX_REQUIRED_FIELDS 64
#endif

#if PB_MAX_REQUIRED_FIELDS < 64
#error You should not lower PB_MAX_REQUIRED_FIELDS from the default value (64).
#endif

/* List of possible field types. These are used in the autogenerated code.
 * Least-significant 4 bits tell the scalar type
 * Most-significant 4 bits specify repeated/required/packed etc.
 */

typedef uint8_t pb_type_t;

/**** Field data types ****/

/* Numeric types */
#define PB_LTYPE_VARINT  0x00 /* int32, int64, enum, bool */
#define PB_LTYPE_UVARINT 0x01 /* uint32, uint64 */
#define PB_LTYPE_SVARINT 0x02 /* sint32, sint64 */
#define PB_LTYPE_FIXED32 0x03 /* fixed32, sfixed32, float */
#define PB_LTYPE_FIXED64 0x04 /* fixed64, sfixed64, double */

/* Marker for last packable field type. */
#define PB_LTYPE_LAST_PACKABLE 0x04

/* Byte array with pre-allocated buffer.
 * data_size is the length of the allocated PB_BYTES_ARRAY structure. */
#define PB_LTYPE_BYTES 0x05

/* String with pre-allocated buffer.
 * data_size is the maximum length. */
#define PB_LTYPE_STRING 0x06

/* Submessage
 * submsg_fields is pointer to field descriptions */
#define PB_LTYPE_SUBMESSAGE 0x07

/* Extension pseudo-field
 * The field contains a pointer to pb_extension_t */
#define PB_LTYPE_EXTENSION 0x08

/* Number of declared LTYPES */
#define PB_LTYPES_COUNT 9
#define PB_LTYPE_MASK 0x0F

/**** Field repetition rules ****/

#define PB_HTYPE_REQUIRED 0x00
#define PB_HTYPE_OPTIONAL 0x10
#define PB_HTYPE_REPEATED 0x20
#define PB_HTYPE_ONEOF    0x30
#define PB_HTYPE_MASK     0x30

/**** Field allocation types ****/

#define PB_ATYPE_STATIC   0x00
#define PB_ATYPE_POINTER  0x80
#define PB_ATYPE_CALLBACK 0x40
#define PB_ATYPE_MASK     0xC0

#define PB_ATYPE(x) ((x) & PB_ATYPE_MASK)
#define PB_HTYPE(x) ((x) & PB_HTYPE_MASK)
#define PB_LTYPE(x) ((x) & PB_LTYPE_MASK)

/* Data type used for storing sizes of struct fields
 * and array counts.
 */
#if defined(PB_FIELD_32BIT)
#define PB_SIZE_MAX ((uint32_t)-1)
    typedef uint32_t pb_size_t;
    typedef int32_t pb_ssize_t;
#elif defined(PB_FIELD_16BIT)
#define PB_SIZE_MAX ((uint16_t)-1)
    typedef uint16_t pb_size_t;
    typedef int16_t pb_ssize_t;
#else
#define PB_SIZE_MAX ((uint8_t)-1)
    typedef uint8_t pb_size_t;
    typedef int8_t pb_ssize_t;
#endif

/* This structure is used in auto-generated constants
 * to specify struct fields.
 * You can change field sizes if you need structures
 * larger than 256 bytes or field tags larger than 256.
 * The compiler should complain if your .proto has such
 * structures. Fix that by defining PB_FIELD_16BIT or
 * PB_FIELD_32BIT.
 */
PB_PACKED_STRUCT_START
typedef struct pb_field_s pb_field_t;
struct pb_field_s {
    pb_size_t tag;
    pb_type_t type;
    pb_size_t data_offset; /* Offset of field data, relative to previous field. */
    pb_ssize_t size_offset; /* Offset of array size or has-boolean, relative to data */
    pb_size_t data_size; /* Data size in bytes for a single item */
    pb_size_t array_size; /* Maximum number of entries in array */

    /* Field definitions for submessage
     * OR default value for all other non-array, non-callback types
     * If null, then field will zeroed. */
    const void *ptr;
} pb_packed;
PB_PACKED_STRUCT_END

/* Make sure that the standard integer types are of the expected sizes.
 * All kinds of things may break otherwise.. atleast all fixed* types.
 *
 * If you get errors here, it probably means that your stdint.h is not
 * correct for your platform.
 */
PB_STATIC_ASSERT(sizeof(int8_t) == 1, INT8_T_WRONG_SIZE)
PB_STATIC_ASSERT(sizeof(uint8_t) == 1, UINT8_T_WRONG_SIZE)
PB_STATIC_ASSERT(sizeof(int16_t) == 2, INT16_T_WRONG_SIZE)
PB_STATIC_ASSERT(sizeof(uint16_t) == 2, UINT16_T_WRONG_SIZE)
PB_STATIC_ASSERT(sizeof(int32_t) == 4, INT32_T_WRONG_SIZE)
PB_STATIC_ASSERT(sizeof(uint32_t) == 4, UINT32_T_WRONG_SIZE)
PB_STATIC_ASSERT(sizeof(int64_t) == 8, INT64_T_WRONG_SIZE)
PB_STATIC_ASSERT(sizeof(uint64_t) == 8, UINT64_T_WRONG_SIZE)

/* This structure is used for 'bytes' arrays.
 * It has the number of bytes in the beginning, and after that an array.
 * Note that actual structs used will have a different length of bytes array.
 */
#define PB_BYTES_ARRAY_T(n) struct { pb_size_t size; uint8_t bytes[n]; }
#define PB_BYTES_ARRAY_T_ALLOCSIZE(n) ((size_t)n + offsetof(pb_bytes_array_t, bytes))

struct pb_bytes_array_s {
    pb_size_t size;
    uint8_t bytes[1];
};
typedef struct pb_bytes_array_s pb_bytes_array_t;

/* This structure is used for giving the callback function.
 * It is stored in the message structure and filled in by the method that
 * calls pb_decode.
 *
 * The decoding callback will be given a limited-length stream
 * If the wire type was string, the length is the length of the string.
 * If the wire type was a varint/fixed32/fixed64, the length is the length
 * of the actual value.
 * The function may be called multiple times (especially for repeated types,
 * but also otherwise if the message happens to contain the field multiple
 * times.)
 *
 * The encoding callback will receive the actual output stream.
 * It should write all the data in one call, including the field tag and
 * wire type. It can write multiple fields.
 *
 * The callback can be null if you want to skip a field.
 */
typedef struct pb_istream_s pb_istream_t;
typedef struct pb_ostream_s pb_ostream_t;
typedef struct pb_callback_s pb_callback_t;
struct pb_callback_s {
#ifdef PB_OLD_CALLBACK_STYLE
    /* Deprecated since nanopb-0.2.1 */
    union {
        bool (*decode)(pb_istream_t *stream, const pb_field_t *field, void *arg);
        bool (*encode)(pb_ostream_t *stream, const pb_field_t *field, const void *arg);
    } funcs;
#else
    /* New function signature, which allows modifying arg contents in callback. */
    union {
        bool (*decode)(pb_istream_t *stream, const pb_field_t *field, void **arg);
        bool (*encode)(pb_ostream_t *stream, const pb_field_t *field, void * const *arg);
    } funcs;
#endif

    /* Free arg for use by callback */
    void *arg;
};

/* Wire types. Library user needs these only in encoder callbacks. */
typedef enum {
    PB_WT_VARINT = 0,
    PB_WT_64BIT  = 1,
    PB_WT_STRING = 2,
    PB_WT_32BIT  = 5
} pb_wire_type_t;

/* Structure for defining the handling of unknown/extension fields.
 * Usually the pb_extension_type_t structure is automatically generated,
 * while the pb_extension_t structure is created by the user. However,
 * if you want to catch all unknown fields, you can also create a custom
 * pb_extension_type_t with your own callback.
 */
typedef struct pb_extension_type_s pb_extension_type_t;
typedef struct pb_extension_s pb_extension_t;
struct pb_extension_type_s {
    /* Called for each unknown field in the message.
     * If you handle the field, read off all of its data and return true.
     * If you do not handle the field, do not read anything and return true.
     * If you run into an error, return false.
     * Set to NULL for default handler.
     */
    bool (*decode)(pb_istream_t *stream, pb_extension_t *extension,
                   uint32_t tag, pb_wire_type_t wire_type);

    /* Called once after all regular fields have been encoded.
     * If you have something to write, do so and return true.
     * If you do not have anything to write, just return true.
     * If you run into an error, return false.
     * Set to NULL for default handler.
     */
    bool (*encode)(pb_ostream_t *stream, const pb_extension_t *extension);

    /* Free field for use by the callback. */
    const void *arg;
};

struct pb_extension_s {
    /* Type describing the extension field. Usually you'll initialize
     * this to a pointer to the automatically generated structure. */
    const pb_extension_type_t *type;

    /* Destination for the decoded data. This must match the datatype
     * of the extension field. */
    void *dest;

    /* Pointer to the next extension handler, or NULL.
     * If this extension does not match a field, the next handler is
     * automatically called. */
    pb_extension_t *next;

    /* The decoder sets this to true if the extension was found.
     * Ignored for encoding. */
    bool found;
};

/* Memory allocation functions to use. You can define pb_realloc and
 * pb_free to custom functions if you want. */
#ifdef PB_ENABLE_MALLOC
#   ifndef pb_realloc
#       define pb_realloc(ptr, size) realloc(ptr, size)
#   endif
#   ifndef pb_free
#       define pb_free(ptr) free(ptr)
#   endif
#endif

/* This is used to inform about need to regenerate .pb.h/.pb.c files. */
#define PB_PROTO_HEADER_VERSION 30

/* These macros are used to declare pb_field_t's in the constant array. */
/* Size of a structure member, in bytes. */
#define pb_membersize(st, m) (sizeof ((st*)0)->m)
/* Number of entries in an array. */
#define pb_arraysize(st, m) (pb_membersize(st, m) / pb_membersize(st, m[0]))
/* Delta from start of one member to the start of another member. */
#define pb_delta(st, m1, m2) ((int)offsetof(st, m1) - (int)offsetof(st, m2))
/* Marks the end of the field list */
#define PB_LAST_FIELD {0,(pb_type_t) 0,0,0,0,0,0}

/* Macros for filling in the data_offset field */
/* data_offset for first field in a message */
#define PB_DATAOFFSET_FIRST(st, m1, m2) (offsetof(st, m1))
/* data_offset for subsequent fields */
#define PB_DATAOFFSET_OTHER(st, m1, m2) (offsetof(st, m1) - offsetof(st, m2) - pb_membersize(st, m2))
/* Choose first/other based on m1 == m2 (deprecated, remains for backwards compatibility) */
#define PB_DATAOFFSET_CHOOSE(st, m1, m2) (int)(offsetof(st, m1) == offsetof(st, m2) \
                                  ? PB_DATAOFFSET_FIRST(st, m1, m2) \
                                  : PB_DATAOFFSET_OTHER(st, m1, m2))

/* Required fields are the simplest. They just have delta (padding) from
 * previous field end, and the size of the field. Pointer is used for
 * submessages and default values.
 */
#define PB_REQUIRED_STATIC(tag, st, m, fd, ltype, ptr) \
    {tag, PB_ATYPE_STATIC | PB_HTYPE_REQUIRED | ltype, \
    fd, 0, pb_membersize(st, m), 0, ptr}

/* Optional fields add the delta to the has_ variable. */
#define PB_OPTIONAL_STATIC(tag, st, m, fd, ltype, ptr) \
    {tag, PB_ATYPE_STATIC | PB_HTYPE_OPTIONAL | ltype, \
    fd, \
    pb_delta(st, has_ ## m, m), \
    pb_membersize(st, m), 0, ptr}

/* Repeated fields have a _count field and also the maximum number of entries. */
#define PB_REPEATED_STATIC(tag, st, m, fd, ltype, ptr) \
    {tag, PB_ATYPE_STATIC | PB_HTYPE_REPEATED | ltype, \
    fd, \
    pb_delta(st, m ## _count, m), \
    pb_membersize(st, m[0]), \
    pb_arraysize(st, m), ptr}

/* Allocated fields carry the size of the actual data, not the pointer */
#define PB_REQUIRED_POINTER(tag, st, m, fd, ltype, ptr) \
    {tag, PB_ATYPE_POINTER | PB_HTYPE_REQUIRED | ltype, \
    fd, 0, pb_membersize(st, m[0]), 0, ptr}

/* Optional fields don't need a has_ variable, as information would be redundant */
#define PB_OPTIONAL_POINTER(tag, st, m, fd, ltype, ptr) \
    {tag, PB_ATYPE_POINTER | PB_HTYPE_OPTIONAL | ltype, \
    fd, 0, pb_membersize(st, m[0]), 0, ptr}

/* Repeated fields have a _count field and a pointer to array of pointers */
#define PB_REPEATED_POINTER(tag, st, m, fd, ltype, ptr) \
    {tag, PB_ATYPE_POINTER | PB_HTYPE_REPEATED | ltype, \
    fd, pb_delta(st, m ## _count, m), \
    pb_membersize(st, m[0]), 0, ptr}

/* Callbacks are much like required fields except with special datatype. */
#define PB_REQUIRED_CALLBACK(tag, st, m, fd, ltype, ptr) \
    {tag, PB_ATYPE_CALLBACK | PB_HTYPE_REQUIRED | ltype, \
    fd, 0, pb_membersize(st, m), 0, ptr}

#define PB_OPTIONAL_CALLBACK(tag, st, m, fd, ltype, ptr) \
    {tag, PB_ATYPE_CALLBACK | PB_HTYPE_OPTIONAL | ltype, \
    fd, 0, pb_membersize(st, m), 0, ptr}

#define PB_REPEATED_CALLBACK(tag, st, m, fd, ltype, ptr) \
    {tag, PB_ATYPE_CALLBACK | PB_HTYPE_REPEATED | ltype, \
    fd, 0, pb_membersize(st, m), 0, ptr}

/* Optional extensions don't have the has_ field, as that would be redundant. */
#define PB_OPTEXT_STATIC(tag, st, m, fd, ltype, ptr) \
    {tag, PB_ATYPE_STATIC | PB_HTYPE_OPTIONAL | ltype, \
    0, \
    0, \
    pb_membersize(st, m), 0, ptr}

#define PB_OPTEXT_POINTER(tag, st, m, fd, ltype, ptr) \
    PB_OPTIONAL_POINTER(tag, st, m, fd, ltype, ptr)

#define PB_OPTEXT_CALLBACK(tag, st, m, fd, ltype, ptr) \
    PB_OPTIONAL_CALLBACK(tag, st, m, fd, ltype, ptr)

/* The mapping from protobuf types to LTYPEs is done using these macros. */
#define PB_LTYPE_MAP_BOOL       PB_LTYPE_VARINT
#define PB_LTYPE_MAP_BYTES      PB_LTYPE_BYTES
#define PB_LTYPE_MAP_DOUBLE     PB_LTYPE_FIXED64
#define PB_LTYPE_MAP_ENUM       PB_LTYPE_VARINT
#define PB_LTYPE_MAP_FIXED32    PB_LTYPE_FIXED32
#define PB_LTYPE_MAP_FIXED64    PB_LTYPE_FIXED64
#define PB_LTYPE_MAP_FLOAT      PB_LTYPE_FIXED32
#define PB_LTYPE_MAP_INT32      PB_LTYPE_VARINT
#define PB_LTYPE_MAP_INT64      PB_LTYPE_VARINT
#define PB_LTYPE_MAP_MESSAGE    PB_LTYPE_SUBMESSAGE
#define PB_LTYPE_MAP_SFIXED32   PB_LTYPE_FIXED32
#define PB_LTYPE_MAP_SFIXED64   PB_LTYPE_FIXED64
#define PB_LTYPE_MAP_SINT32     PB_LTYPE_SVARINT
#define PB_LTYPE_MAP_SINT64     PB_LTYPE_SVARINT
#define PB_LTYPE_MAP_STRING     PB_LTYPE_STRING
#define PB_LTYPE_MAP_UINT32     PB_LTYPE_UVARINT
#define PB_LTYPE_MAP_UINT64     PB_LTYPE_UVARINT
#define PB_LTYPE_MAP_EXTENSION  PB_LTYPE_EXTENSION

/* This is the actual macro used in field descriptions.
 * It takes these arguments:
 * - Field tag number
 * - Field type:   BOOL, BYTES, DOUBLE, ENUM, FIXED32, FIXED64,
 *                 FLOAT, INT32, INT64, MESSAGE, SFIXED32, SFIXED64
 *                 SINT32, SINT64, STRING, UINT32, UINT64 or EXTENSION
 * - Field rules:  REQUIRED, OPTIONAL or REPEATED
 * - Allocation:   STATIC or CALLBACK
 * - Placement: FIRST or OTHER, depending on if this is the first field in structure.
 * - Message name
 * - Field name
 * - Previous field name (or field name again for first field)
 * - Pointer to default value or submsg fields.
 */

#define PB_FIELD(tag, type, rules, allocation, placement, message, field, prevfield, ptr) \
        PB_ ## rules ## _ ## allocation(tag, message, field, \
        PB_DATAOFFSET_ ## placement(message, field, prevfield), \
        PB_LTYPE_MAP_ ## type, ptr)

/* Field description for oneof fields. This requires taking into account the
 * union name also, that's why a separate set of macros is needed.
 */
#define PB_ONEOF_STATIC(u, tag, st, m, fd, ltype, ptr) \
    {tag, PB_ATYPE_STATIC | PB_HTYPE_ONEOF | ltype, \
    fd, pb_delta(st, which_ ## u, u.m), \
    pb_membersize(st, u.m), 0, ptr}

#define PB_ONEOF_POINTER(u, tag, st, m, fd, ltype, ptr) \
    {tag, PB_ATYPE_POINTER | PB_HTYPE_ONEOF | ltype, \
    fd, pb_delta(st, which_ ## u, u.m), \
    pb_membersize(st, u.m[0]), 0, ptr}

#define PB_ONEOF_FIELD(union_name, tag, type, rules, allocation, placement, message, field, prevfield, ptr) \
        PB_ ## rules ## _ ## allocation(union_name, tag, message, field, \
        PB_DATAOFFSET_ ## placement(message, union_name.field, prevfield), \
        PB_LTYPE_MAP_ ## type, ptr)

/* These macros are used for giving out error messages.
 * They are mostly a debugging aid; the main error information
 * is the true/false return value from functions.
 * Some code space can be saved by disabling the error
 * messages if not used.
 *
 * PB_SET_ERROR() sets the error message if none has been set yet.
 *                msg must be a constant string literal.
 * PB_GET_ERROR() always returns a pointer to a string.
 * PB_RETURN_ERROR() sets the error and returns false from current
 *                   function.
 */
#ifdef PB_NO_ERRMSG
#define PB_SET_ERROR(stream, msg) PB_UNUSED(stream)
#define PB_GET_ERROR(stream) "(errmsg disabled)"
#else
#define PB_SET_ERROR(stream, msg) (stream->errmsg = (stream)->errmsg ? (stream)->errmsg : (msg))
#define PB_GET_ERROR(stream) ((stream)->errmsg ? (stream)->errmsg : "(none)")
#endif

#define PB_RETURN_ERROR(stream, msg) return PB_SET_ERROR(stream, msg), false

#endif
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

//include file removed

void pb_istream_from_stream(Stream &stream, pb_istream_t &istream);

#endif // PB_ARDUINO_DECODE_H_
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

//include file removed

void pb_ostream_from_stream(Print &stream, pb_ostream_t &ostream);

#endif // PB_ARDUINO_ENCODE_H_
/* pb_common.h: Common support functions for pb_encode.c and pb_decode.c.
 * These functions are rarely needed by applications directly.
 */

#ifndef PB_COMMON_H_INCLUDED
#define PB_COMMON_H_INCLUDED

//include file removed

#ifdef __cplusplus
extern "C" {
#endif

/* Iterator for pb_field_t list */
struct pb_field_iter_s {
    const pb_field_t *start;       /* Start of the pb_field_t array */
    const pb_field_t *pos;         /* Current position of the iterator */
    unsigned required_field_index; /* Zero-based index that counts only the required fields */
    void *dest_struct;             /* Pointer to start of the structure */
    void *pData;                   /* Pointer to current field value */
    void *pSize;                   /* Pointer to count/has field */
};
typedef struct pb_field_iter_s pb_field_iter_t;

/* Initialize the field iterator structure to beginning.
 * Returns false if the message type is empty. */
bool pb_field_iter_begin(pb_field_iter_t *iter, const pb_field_t *fields, void *dest_struct);

/* Advance the iterator to the next field.
 * Returns false when the iterator wraps back to the first field. */
bool pb_field_iter_next(pb_field_iter_t *iter);

/* Advance the iterator until it points at a field with the given tag.
 * Returns false if no such field exists. */
bool pb_field_iter_find(pb_field_iter_t *iter, uint32_t tag);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

/* pb_decode.h: Functions to decode protocol buffers. Depends on pb_decode.c.
 * The main function is pb_decode. You also need an input stream, and the
 * field descriptions created by nanopb_generator.py.
 */

#ifndef PB_DECODE_H_INCLUDED
#define PB_DECODE_H_INCLUDED

//include file removed

#ifdef __cplusplus
extern "C" {
#endif

/* Structure for defining custom input streams. You will need to provide
 * a callback function to read the bytes from your storage, which can be
 * for example a file or a network socket.
 *
 * The callback must conform to these rules:
 *
 * 1) Return false on IO errors. This will cause decoding to abort.
 * 2) You can use state to store your own data (e.g. buffer pointer),
 *    and rely on pb_read to verify that no-body reads past bytes_left.
 * 3) Your callback may be used with substreams, in which case bytes_left
 *    is different than from the main stream. Don't use bytes_left to compute
 *    any pointers.
 */
struct pb_istream_s
{
#ifdef PB_BUFFER_ONLY
    /* Callback pointer is not used in buffer-only configuration.
     * Having an int pointer here allows binary compatibility but
     * gives an error if someone tries to assign callback function.
     */
    int *callback;
#else
    bool (*callback)(pb_istream_t *stream, uint8_t *buf, size_t count);
#endif

    void *state; /* Free field for use by callback implementation */
    size_t bytes_left;

#ifndef PB_NO_ERRMSG
    const char *errmsg;
#endif
};

/***************************
 * Main decoding functions *
 ***************************/

/* Decode a single protocol buffers message from input stream into a C structure.
 * Returns true on success, false on any failure.
 * The actual struct pointed to by dest must match the description in fields.
 * Callback fields of the destination structure must be initialized by caller.
 * All other fields will be initialized by this function.
 *
 * Example usage:
 *    MyMessage msg = {};
 *    uint8_t buffer[64];
 *    pb_istream_t stream;
 *
 *    // ... read some data into buffer ...
 *
 *    stream = pb_istream_from_buffer(buffer, count);
 *    pb_decode(&stream, MyMessage_fields, &msg);
 */
bool pb_decode(pb_istream_t *stream, const pb_field_t fields[], void *dest_struct);

/* Same as pb_decode, except does not initialize the destination structure
 * to default values. This is slightly faster if you need no default values
 * and just do memset(struct, 0, sizeof(struct)) yourself.
 *
 * This can also be used for 'merging' two messages, i.e. update only the
 * fields that exist in the new message.
 *
 * Note: If this function returns with an error, it will not release any
 * dynamically allocated fields. You will need to call pb_release() yourself.
 */
bool pb_decode_noinit(pb_istream_t *stream, const pb_field_t fields[], void *dest_struct);

/* Same as pb_decode, except expects the stream to start with the message size
 * encoded as varint. Corresponds to parseDelimitedFrom() in Google's
 * protobuf API.
 */
bool pb_decode_delimited(pb_istream_t *stream, const pb_field_t fields[], void *dest_struct);

#ifdef PB_ENABLE_MALLOC
/* Release any allocated pointer fields. If you use dynamic allocation, you should
 * call this for any successfully decoded message when you are done with it. If
 * pb_decode() returns with an error, the message is already released.
 */
void pb_release(const pb_field_t fields[], void *dest_struct);
#endif


/**************************************
 * Functions for manipulating streams *
 **************************************/

/* Create an input stream for reading from a memory buffer.
 *
 * Alternatively, you can use a custom stream that reads directly from e.g.
 * a file or a network socket.
 */
pb_istream_t pb_istream_from_buffer(uint8_t *buf, size_t bufsize);

/* Function to read from a pb_istream_t. You can use this if you need to
 * read some custom header data, or to read data in field callbacks.
 */
bool pb_read(pb_istream_t *stream, uint8_t *buf, size_t count);


/************************************************
 * Helper functions for writing field callbacks *
 ************************************************/

/* Decode the tag for the next field in the stream. Gives the wire type and
 * field tag. At end of the message, returns false and sets eof to true. */
bool pb_decode_tag(pb_istream_t *stream, pb_wire_type_t *wire_type, uint32_t *tag, bool *eof);

/* Skip the field payload data, given the wire type. */
bool pb_skip_field(pb_istream_t *stream, pb_wire_type_t wire_type);

/* Decode an integer in the varint format. This works for bool, enum, int32,
 * int64, uint32 and uint64 field types. */
bool pb_decode_varint(pb_istream_t *stream, uint64_t *dest);

/* Decode an integer in the zig-zagged svarint format. This works for sint32
 * and sint64. */
bool pb_decode_svarint(pb_istream_t *stream, int64_t *dest);

/* Decode a fixed32, sfixed32 or float value. You need to pass a pointer to
 * a 4-byte wide C variable. */
bool pb_decode_fixed32(pb_istream_t *stream, void *dest);

/* Decode a fixed64, sfixed64 or double value. You need to pass a pointer to
 * a 8-byte wide C variable. */
bool pb_decode_fixed64(pb_istream_t *stream, void *dest);

/* Make a limited-length substream for reading a PB_WT_STRING field. */
bool pb_make_string_substream(pb_istream_t *stream, pb_istream_t *substream);
void pb_close_string_substream(pb_istream_t *stream, pb_istream_t *substream);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
/* pb_encode.h: Functions to encode protocol buffers. Depends on pb_encode.c.
 * The main function is pb_encode. You also need an output stream, and the
 * field descriptions created by nanopb_generator.py.
 */

#ifndef PB_ENCODE_H_INCLUDED
#define PB_ENCODE_H_INCLUDED

//include file removed

#ifdef __cplusplus
extern "C" {
#endif

/* Structure for defining custom output streams. You will need to provide
 * a callback function to write the bytes to your storage, which can be
 * for example a file or a network socket.
 *
 * The callback must conform to these rules:
 *
 * 1) Return false on IO errors. This will cause encoding to abort.
 * 2) You can use state to store your own data (e.g. buffer pointer).
 * 3) pb_write will update bytes_written after your callback runs.
 * 4) Substreams will modify max_size and bytes_written. Don't use them
 *    to calculate any pointers.
 */
struct pb_ostream_s
{
#ifdef PB_BUFFER_ONLY
    /* Callback pointer is not used in buffer-only configuration.
     * Having an int pointer here allows binary compatibility but
     * gives an error if someone tries to assign callback function.
     * Also, NULL pointer marks a 'sizing stream' that does not
     * write anything.
     */
    int *callback;
#else
    bool (*callback)(pb_ostream_t *stream, const uint8_t *buf, size_t count);
#endif
    void *state;          /* Free field for use by callback implementation. */
    size_t max_size;      /* Limit number of output bytes written (or use SIZE_MAX). */
    size_t bytes_written; /* Number of bytes written so far. */

#ifndef PB_NO_ERRMSG
    const char *errmsg;
#endif
};

/***************************
 * Main encoding functions *
 ***************************/

/* Encode a single protocol buffers message from C structure into a stream.
 * Returns true on success, false on any failure.
 * The actual struct pointed to by src_struct must match the description in fields.
 * All required fields in the struct are assumed to have been filled in.
 *
 * Example usage:
 *    MyMessage msg = {};
 *    uint8_t buffer[64];
 *    pb_ostream_t stream;
 *
 *    msg.field1 = 42;
 *    stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
 *    pb_encode(&stream, MyMessage_fields, &msg);
 */
bool pb_encode(pb_ostream_t *stream, const pb_field_t fields[], const void *src_struct);

/* Same as pb_encode, but prepends the length of the message as a varint.
 * Corresponds to writeDelimitedTo() in Google's protobuf API.
 */
bool pb_encode_delimited(pb_ostream_t *stream, const pb_field_t fields[], const void *src_struct);

/* Encode the message to get the size of the encoded data, but do not store
 * the data. */
bool pb_get_encoded_size(size_t *size, const pb_field_t fields[], const void *src_struct);

/**************************************
 * Functions for manipulating streams *
 **************************************/

/* Create an output stream for writing into a memory buffer.
 * The number of bytes written can be found in stream.bytes_written after
 * encoding the message.
 *
 * Alternatively, you can use a custom stream that writes directly to e.g.
 * a file or a network socket.
 */
pb_ostream_t pb_ostream_from_buffer(uint8_t *buf, size_t bufsize);

/* Pseudo-stream for measuring the size of a message without actually storing
 * the encoded data.
 *
 * Example usage:
 *    MyMessage msg = {};
 *    pb_ostream_t stream = PB_OSTREAM_SIZING;
 *    pb_encode(&stream, MyMessage_fields, &msg);
 *    printf("Message size is %d\n", stream.bytes_written);
 */
#ifndef PB_NO_ERRMSG
#define PB_OSTREAM_SIZING {0,0,0,0,0}
#else
#define PB_OSTREAM_SIZING {0,0,0,0}
#endif

/* Function to write into a pb_ostream_t stream. You can use this if you need
 * to append or prepend some custom headers to the message.
 */
bool pb_write(pb_ostream_t *stream, const uint8_t *buf, size_t count);


/************************************************
 * Helper functions for writing field callbacks *
 ************************************************/

/* Encode field header based on type and field number defined in the field
 * structure. Call this from the callback before writing out field contents. */
bool pb_encode_tag_for_field(pb_ostream_t *stream, const pb_field_t *field);

/* Encode field header by manually specifing wire type. You need to use this
 * if you want to write out packed arrays from a callback field. */
bool pb_encode_tag(pb_ostream_t *stream, pb_wire_type_t wiretype, uint32_t field_number);

/* Encode an integer in the varint format.
 * This works for bool, enum, int32, int64, uint32 and uint64 field types. */
bool pb_encode_varint(pb_ostream_t *stream, uint64_t value);

/* Encode an integer in the zig-zagged svarint format.
 * This works for sint32 and sint64. */
bool pb_encode_svarint(pb_ostream_t *stream, int64_t value);

/* Encode a string or bytes type field. For strings, pass strlen(s) as size. */
bool pb_encode_string(pb_ostream_t *stream, const uint8_t *buffer, size_t size);

/* Encode a fixed32, sfixed32 or float value.
 * You need to pass a pointer to a 4-byte wide C variable. */
bool pb_encode_fixed32(pb_ostream_t *stream, const void *value);

/* Encode a fixed64, sfixed64 or double value.
 * You need to pass a pointer to a 8-byte wide C variable. */
bool pb_encode_fixed64(pb_ostream_t *stream, const void *value);

/* Encode a submessage field.
 * You need to pass the pb_field_t array and pointer to struct, just like
 * with pb_encode(). This internally encodes the submessage twice, first to
 * calculate message size and then to actually write it out.
 */
bool pb_encode_submessage(pb_ostream_t *stream, const pb_field_t fields[], const void *src_struct);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
#ifndef MESHBLU_CREDENTIALS_H
#define MESHBLU_CREDENTIALS_H
#include <stddef.h>

struct MeshbluCredentials {

public:
  MeshbluCredentials(const char* uuid=NULL, const char* token=NULL);
  const char* getUuid() const;
  const char* getToken() const;

private:
  const char* uuid;
  const char* token;
};
#endif
#ifndef tentacle_h
#define tentacle_h

//include file removed
//include file removed


class Tentacle {
  public:
    Tentacle(size_t numPins);
    ~Tentacle();

    Tentacle& configurePin(int number, Action action);
    Tentacle& configurePins(Action* actions);

    Action* getConfiguredPinActions();

    int processPin(int number, int value);
    int processPin(int pin);


    int getNumPins() const;

    virtual const MeshbluCredentials& getCredentials() = 0;
    virtual Tentacle& setCredentials(const char* uuid, const char* token) = 0;

    virtual Tentacle& setMode(int number, Action action) = 0;
    virtual Tentacle& digitalWrite(int number, int value) = 0;
    virtual Tentacle& analogWrite(int number, int value) = 0;
    virtual bool digitalRead(int number) = 0;
    virtual int analogRead(int number) = 0;


  protected:
    int numPins;
    Action* configuredPinActions;
    void resetPinActions();

};

#endif
//include file removed

#ifndef tentacle_arduino_h
#define tentacle_arduino_h

class TentacleArduino : public Tentacle {
  public:
    TentacleArduino();

    const MeshbluCredentials& getCredentials();
    Tentacle& setCredentials(const char* uuid, const char* token);

    Tentacle& setMode(int number, Action action);
    Tentacle& digitalWrite(int number, int value);
    Tentacle& analogWrite(int number, int value);
    bool digitalRead(int number);
    int analogRead(int number);
};

#endif
//include file removed
/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.3 at Sun Jul 19 23:12:30 2015. */

#ifndef PB_TENTACLE_MESSAGE_PB_H_INCLUDED
#define PB_TENTACLE_MESSAGE_PB_H_INCLUDED
//include file removed

#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Enum definitions */
typedef enum _TentacleMessageTopic {
    TentacleMessageTopic_config = 0,
    TentacleMessageTopic_action = 1,
    TentacleMessageTopic_authentication = 2,
    TentacleMessageTopic_ping = 3
} TentacleMessageTopic;

/* Struct definitions */
typedef struct _MeshbluAuthentication {
    bool has_uuid;
    char uuid[64];
    bool has_token;
    char token[64];
} MeshbluAuthentication;

typedef struct _Pin {
    bool has_number;
    uint32_t number;
    bool has_value;
    uint32_t value;
    bool has_action;
    Action action;
} Pin;

typedef struct _TentacleMessage {
    bool has_version;
    uint32_t version;
    bool has_topic;
    TentacleMessageTopic topic;
    bool has_response;
    bool response;
    pb_callback_t pins;
    bool has_authentication;
    MeshbluAuthentication authentication;
    bool has_broadcastPins;
    bool broadcastPins;
    bool has_broadcastInterval;
    uint32_t broadcastInterval;
    pb_callback_t customData;
} TentacleMessage;

/* Default values for struct fields */

/* Initializer values for message structs */
#define TentacleMessage_init_default             {false, 0, false, (TentacleMessageTopic)0, false, 0, {{NULL}, NULL}, false, MeshbluAuthentication_init_default, false, 0, false, 0, {{NULL}, NULL}}
#define Pin_init_default                         {false, 0, false, 0, false, (Action)0}
#define MeshbluAuthentication_init_default       {false, "", false, ""}
#define TentacleMessage_init_zero                {false, 0, false, (TentacleMessageTopic)0, false, 0, {{NULL}, NULL}, false, MeshbluAuthentication_init_zero, false, 0, false, 0, {{NULL}, NULL}}
#define Pin_init_zero                            {false, 0, false, 0, false, (Action)0}
#define MeshbluAuthentication_init_zero          {false, "", false, ""}

/* Field tags (for use in manual encoding/decoding) */
#define MeshbluAuthentication_uuid_tag           1
#define MeshbluAuthentication_token_tag          2
#define Pin_number_tag                           1
#define Pin_value_tag                            2
#define Pin_action_tag                           3
#define TentacleMessage_version_tag              1
#define TentacleMessage_topic_tag                2
#define TentacleMessage_response_tag             3
#define TentacleMessage_pins_tag                 4
#define TentacleMessage_authentication_tag       5
#define TentacleMessage_broadcastPins_tag        6
#define TentacleMessage_broadcastInterval_tag    7
#define TentacleMessage_customData_tag           8

/* Struct field encoding specification for nanopb */
extern const pb_field_t TentacleMessage_fields[9];
extern const pb_field_t Pin_fields[4];
extern const pb_field_t MeshbluAuthentication_fields[3];

/* Maximum encoded size of messages (where known) */
#define Pin_size                                 18
#define MeshbluAuthentication_size               132

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define TENTACLE_MESSAGE_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
#ifndef TENTACLE_PSEUDOPOD_H
#define TENTACLE_PSEUDOPOD_H

extern "C" {
//include file removed
//include file removed
}

#include <stddef.h>

//include file removed
//include file removed

//include file removed
//include file removed

// Make library cross-compatiable
// with Arduino, GNU C++ for tests, and Spark.
#if defined(ARDUINO) && ARDUINO >= 100
#include "Stream.h"
#elif defined(SPARK)
#include "spark_wiring_stream.h"
#endif

#ifndef BROADCAST_INTERVAL_DEFAULT
#define BROADCAST_INTERVAL_DEFAULT 2000
#endif

class Pseudopod {
  public:
    Pseudopod(Stream& input, Print& output, Tentacle& tentacle);

    size_t sendConfiguredPins();
    size_t sendPins(Action* pinActions);
    size_t sendPins();

    TentacleMessageTopic readMessage();
    int getBroadcastInterval();
    bool shouldBroadcastPins();
    bool isConfigured();
    size_t requestConfiguration();
    size_t authenticate(const char* uuid, const char *token);
    size_t registerDevice();

    bool isConnected();

   private:
    pb_ostream_t pbOutput;
    pb_istream_t pbInput;
    Tentacle* tentacle;
    Action* messagePinActions;
    bool broadcastPins = false;
    bool configured = false;
    unsigned int broadcastInterval = BROADCAST_INTERVAL_DEFAULT;
    void resetPinActions();
    static void printPin(const Pin& pin);

    TentacleMessage currentMessage;

    static bool pinEncode(pb_ostream_t *stream, const pb_field_t *field, void * const *arg);
    static bool pinDecode(pb_istream_t *stream, const pb_field_t *field, void **arg);
};

#endif
