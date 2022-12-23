#ifndef __GUARD_SUPERBLOCK_H
#define __GUARD_SUPERBLOCK_H

#include "types.h"

struct ext2_superblock_t {
  ext2_uint32_t s_inodes_count;
  ext2_uint32_t s_blocks_count;
  ext2_uint32_t s_r_blocks_count;
  ext2_uint32_t s_free_blocks_count;
  ext2_uint32_t s_free_inodes_count;
  ext2_uint32_t s_first_data_block;
  ext2_uint32_t s_log_block_size;
  ext2_int32_t s_log_frag_size;
  ext2_uint32_t s_blocks_per_group;
  ext2_uint32_t s_frags_per_group;
  ext2_uint32_t s_inodes_per_group;
  ext2_uint32_t s_mtime;
  ext2_uint32_t s_wtime;
  ext2_uint16_t s_mnt_count;
  ext2_uint16_t s_max_mnt_count;
  ext2_uint16_t s_magic;
  ext2_uint16_t s_state;
  ext2_uint16_t s_errors;
  ext2_uint16_t s_minor_rev_level;
  ext2_uint32_t s_lastcheck;
  ext2_uint32_t s_checkinterval;
  ext2_uint32_t s_creator_os;
  ext2_uint32_t s_rev_level;
  ext2_uint16_t s_def_resuid;
  ext2_uint16_t s_def_resgid;
  ext2_uint32_t s_first_ino;
  ext2_uint16_t s_inode_size;
  ext2_uint16_t s_block_group_nr;
  ext2_uint32_t s_feature_compat;
  ext2_uint32_t s_feature_incompat;
  ext2_uint32_t s_feature_ro_compat;
  ext2_uint8_t s_uuid[16];
  ext2_uint8_t s_volume_name[16];
  ext2_uint8_t s_last_mounted[64];
  ext2_uint32_t s_algo_bitmap;
  ext2_uint8_t s_prealloc_blocks;
  ext2_uint8_t s_prealloc_dir_blocks;
  ext2_uint16_t s_padding_1;
  ext2_uint8_t s_journal_uuid[16];
  ext2_uint32_t s_journal_inum;
  ext2_uint32_t s_journal_dev;
  ext2_uint32_t s_last_orphan;
  ext2_uint32_t s_hash_seed[4];
  ext2_uint8_t s_def_hash_version;
  ext2_uint8_t s_padding_2[3];
  ext2_uint32_t s_default_mount_options;
  ext2_uint32_t s_first_meta_bg;
  ext2_uint8_t s_unused[760];
};

#endif // !__GUARD_SUPERBLOCK_H
