#ifndef __GUARD_DIRECTORY_ENTRY_H
#define __GUARD_DIRECTORY_ENTRY_H

#include "types.h"

#define EXT2_NAME_LEN 255

struct ext2_direntry_t {
  ext2_uint32_t inode;
  ext2_uint16_t rec_len;
  ext2_uint8_t name_len;
  ext2_uint8_t file_type;
  ext2_uint8_t file_name[EXT2_NAME_LEN];
};

#endif // !__GUARD_DIRECTORY_ENTRY_H
