#ifndef _DATA_STRUCT_H
#define _DATA_STRUCT_H

#include <stdint.h>

/**
 * Compute the offset of a member in a structure
 *
 * @param type   The type of the container structure a member is embedded in.
 * @param member The name of the member within the structure.
 *
 * @return Offset of the member in the structure.
 */
#define struct_offset_of(type, member) ((size_t) & ((type*)0)->member)

/**
 * Cast a member of a structure to the containing structure
 *
 * @param ptr    The pointer to the member.
 * @param type   The type of the container structure a member is embedded in.
 * @param member The name of the member within the structure.
 *
 * @return Container structure.
 */
#define struct_container_of(ptr, type, member) \
    ((type*)((void*)(ptr) - struct_offset_of(type, member)))


//


#endif