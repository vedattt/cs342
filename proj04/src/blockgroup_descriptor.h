#ifndef __GUARD_BLOCKGROUP_DESCRIPTOR
#define __GUARD_BLOCKGROUP_DESCRIPTOR

#include "types.h"

struct ext2_blockgroup_descriptor_t {
  ext2_uint32_t bg_block_bitmap;
  ext2_uint32_t bg_inode_bitmap;
  ext2_uint32_t bg_inode_table;
  ext2_uint16_t bg_free_blocks_count;
  ext2_uint16_t bg_free_inodes_count;
  ext2_uint16_t bg_used_dirs_count;
  ext2_uint16_t bg_pad;
  ext2_uint8_t bg_reserved[12];
};

#endif // !__GUARD_BLOCKGROUP_DESCRIPTOR
