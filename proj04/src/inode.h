#ifndef __GUARD_INODE_H
#define __GUARD_INODE_H

#include "types.h"

#define EXT2_ROOT_INO 2 // Root directory inode

struct ext2_inode_t {
  ext2_uint16_t i_mode;
  ext2_uint16_t i_uid;
  ext2_uint32_t i_size;
  ext2_uint32_t i_atime;
  ext2_uint32_t i_ctime;
  ext2_uint32_t i_mtime;
  ext2_uint32_t i_dtime;
  ext2_uint16_t i_gid;
  ext2_uint16_t i_links_count;
  ext2_uint16_t i_blocks;
  ext2_uint32_t i_flags;

  union {
    struct {
      ext2_uint32_t  l_i_reserved1;
    } linux1;
    struct {
      ext2_uint32_t  h_i_translator;
    } hurd1;
    struct {
      ext2_uint32_t  m_i_reserved1;
    } masix1;
  } i_osdi1;

  ext2_uint32_t i_block[15];
  ext2_uint32_t i_generation;
  ext2_uint32_t i_file_acl;
  ext2_uint32_t i_dir_acl;
  ext2_uint32_t i_faddr;

  union {
    struct {
      ext2_uint8_t    l_i_frag;
      ext2_uint8_t    l_i_fsize;
      ext2_uint16_t   i_pad1;
      ext2_uint16_t   l_i_uid_high;
      ext2_uint16_t   l_i_gid_high;
      ext2_uint32_t   l_i_reserved2;
    } linux2;
    struct {
      ext2_uint8_t    h_i_frag;
      ext2_uint8_t    h_i_fsize;
      ext2_uint16_t   h_i_mode_high;
      ext2_uint16_t   h_i_uid_high;
      ext2_uint16_t   h_i_gid_high;
      ext2_uint32_t   h_i_author;
    } hurd2;
    struct {
      ext2_uint8_t    m_i_frag;
      ext2_uint8_t    m_i_fsize;
      ext2_uint16_t   m_pad1;
      ext2_uint32_t   m_i_reserved2[2];
    } masix2;
  } i_osd2;
};

#endif // !__GUARD_INODE_H
