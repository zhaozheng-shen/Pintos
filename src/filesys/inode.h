#ifndef FILESYS_INODE_H
#define FILESYS_INODE_H

#include <stdbool.h>
#include "filesys/off_t.h"
#include "devices/block.h"
#include <list.h>
#include <round.h>

/* Identifies an inode. */
#define INODE_MAGIC 0x494e4f44
#define DIRECT_BLOCK 120      /* An inode has DIRECT_BLOCK direct entries. */
#define INDIRECT_BLOCK 128         /* An inode has a INDIRECT_BLOCK entry. */
#define DOUBLE_INDIRECT 128 * 128 /* An inode has a DOUBLE_INDIRECT entry. */


struct bitmap;

/* On-disk inode.
   Must be exactly BLOCK_SECTOR_SIZE bytes long. */
struct inode_disk
  {
    block_sector_t direct_part[DIRECT_BLOCK];  /* direct part of an inode. */
    block_sector_t indirect_part;            /* indirect part of an inode. */
    block_sector_t double_indirect_part; /*double direct part of an inode. */

    // block_sector_t start;               /* First data sector. */
    off_t length;                       /* File size in bytes. */
    unsigned magic;                     /* Magic number. */
    // uint32_t unused[125];               /* Not used. */
    block_sector_t parent;
    bool dir_or_file;
  };

/* the struct of the indirect part stored in one inode. */
struct inode_indirect
{
  block_sector_t indirect_inode[INDIRECT_BLOCK];  /* indirect inode entry. */
};

/* Returns the number of sectors to allocate for an inode SIZE
   bytes long. */
static inline size_t
bytes_to_sectors (off_t size)
{
  return DIV_ROUND_UP (size, BLOCK_SECTOR_SIZE);
}

/* In-memory inode. */
struct inode 
  {
    struct list_elem elem;            /* Element in inode list. */
    block_sector_t sector;            /* Sector number of disk location. */
    int open_cnt;                     /* Number of openers. */
    bool removed;                     /* True if deleted, false otherwise. */
    int deny_write_cnt;               /* 0: writes ok, >0: deny writes. */
    struct inode_disk data;           /* Inode content. */
  };

void inode_init (void);
bool inode_create (block_sector_t, off_t, 
                  block_sector_t parent, bool dir_or_file);
struct inode *inode_open (block_sector_t);
struct inode *inode_reopen (struct inode *);
block_sector_t inode_get_inumber (const struct inode *);
void inode_close (struct inode *);
void inode_remove (struct inode *);
off_t inode_read_at (struct inode *, void *, off_t size, off_t offset);
off_t inode_write_at (struct inode *, const void *, off_t size, off_t offset);
void inode_deny_write (struct inode *);
void inode_allow_write (struct inode *);
off_t inode_length (const struct inode *);

/* Helper functions for write operation for the cache. */
void write_indirect(block_sector_t* sectors, int size);
void write_double(block_sector_t* sectors, int size);

#endif /* filesys/inode.h */
