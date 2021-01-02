#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "ext2_fs.h"

int image_fd;
unsigned char curr_c;
unsigned char mask = 0x01;
struct ext2_super_block curr_superblock;
__u32 num_groups = 0;
__u32 b_size;
unsigned char *block_buffer;
__u32 in_size = sizeof(struct ext2_inode);
__u32 num_blocks;
__u32 num_inodes;
struct ext2_group_desc curr_bgroup;
off_t bgtable_offset;
unsigned char *bmap;
unsigned char *imap;
struct ext2_inode *itable;
__u32 blocks_in_group;
__u32 *single_addr;
__u32 *double_addr;
__u32 *triple_addr;

void init_superb_bgroup(__u32 group_num)
{
  if(!group_num)
    {
      //first sblock
      if(pread(image_fd, (void *) &curr_superblock, sizeof(struct ext2_super_block), 1024) < 0)
	{
	  fprintf(stderr, "Pread failed; %s\n", strerror(errno));
	  _exit(1);
	}
      
      //check validity
      if(curr_superblock.s_magic != EXT2_SUPER_MAGIC)
	{
	  fprintf(stderr, "Invalid superblock\n");
	  _exit(1);
	}

      //initialized constants
      num_blocks = curr_superblock.s_blocks_per_group;
      num_inodes = curr_superblock.s_inodes_per_group;

      //initialized num_groups and blocks_in_group
      num_groups = (curr_superblock.s_blocks_count + (num_blocks - 1)) / num_blocks;
      blocks_in_group = (num_groups == 1)? curr_superblock.s_blocks_count: num_blocks;

      //block size and block group table offset
      b_size = 1024 << curr_superblock.s_log_block_size;
      bgtable_offset = (b_size >= 2048)?  b_size: (2 * b_size);
      
      //first group
      if(pread(image_fd, (void *) &curr_bgroup, sizeof(struct ext2_group_desc), bgtable_offset) < 0)
	{
	  fprintf(stderr, "Pread failed; %s\n", strerror(errno));
	  _exit(1);
	}
    }
  else
    {
      blocks_in_group = ((group_num + 1) < num_groups)? num_blocks: (curr_superblock.s_blocks_count % num_blocks);
      bgtable_offset += sizeof(struct ext2_group_desc);
      if(pread(image_fd, (void *) &curr_bgroup, sizeof(struct ext2_group_desc), bgtable_offset) < 0)
	{
	  fprintf(stderr, "Pread failed; %s\n", strerror(errno));
	  _exit(1);
	}
    }
}

void print_superb_bgroup(__u32 group_num)
{
  //first non-reserved inode
  __u32 non_res = (curr_superblock.s_rev_level)? curr_superblock.s_first_ino: 11;

  //print 1st superblock summary
  if(!group_num)
    fprintf(stdout, "SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", curr_superblock.s_blocks_count, curr_superblock.s_inodes_count, b_size, in_size, num_blocks, num_inodes, non_res);

  //print 1st group summary
  fprintf(stdout, "GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n", group_num, blocks_in_group, num_inodes, curr_bgroup.bg_free_blocks_count, curr_bgroup.bg_free_inodes_count, curr_bgroup.bg_block_bitmap, curr_bgroup.bg_inode_bitmap, curr_bgroup.bg_inode_table);
}

void init_map_table()
{
  //read maps
  off_t read_offset = curr_bgroup.bg_block_bitmap * b_size;
  bmap = malloc(b_size * sizeof(unsigned char));
  if(pread(image_fd, (void *) bmap, b_size, read_offset) < 0)
    {
      fprintf(stderr, "Pread failed on bitmap; %s\n", strerror(errno));
      _exit(1);
    }
  read_offset = curr_bgroup.bg_inode_bitmap * b_size;
  imap = malloc(b_size * sizeof(unsigned char));
  if(pread(image_fd, (void *) imap, b_size, read_offset) < 0)
    {
      fprintf(stderr, "Pread failed on inodemap; %s\n", strerror(errno));
      _exit(1);
    }
  read_offset = curr_bgroup.bg_inode_table * b_size;
  itable = malloc(num_inodes * sizeof(struct ext2_inode));
  if(pread(image_fd, (void *) itable, num_inodes * sizeof(struct ext2_inode), read_offset) < 0)
    {
      fprintf(stderr, "Pread failed on inode table; %s\n", strerror(errno));
      _exit(1);
    }
}

void traverse_bmap()
{
  //traverse bmap
  __u32 b_count = 0;
  for(; b_count < blocks_in_group;)
    {
      curr_c = bmap[b_count / 8];
      if(!(mask & (curr_c >> (b_count % 8))))
	{
	  b_count++;
	  fprintf(stdout, "BFREE,%d\n", b_count);
	  continue;
	}
      b_count++;
    }
}

void read_dcontent(__u32 inode_num)
{
  off_t read_offset;
  __u32 print_inode_num = inode_num + 1;
  struct ext2_dir_entry curr_entry;
  __u32 block_offset = 0;
  __u32 block_offset_single = 0;
  __u32 block_offset_double = 0;
  __u32 block_offset_triple = 0;
  int in_ptr;
  __u32 num_ptrs = b_size / 4;
  for(in_ptr = 0; in_ptr < 12; in_ptr++)
    {
      if(itable[inode_num].i_block[in_ptr])
	{
	  read_offset = itable[inode_num].i_block[in_ptr] * b_size;
	  if(pread(image_fd, (void *) block_buffer, b_size, read_offset) < 0)
	    {
	      fprintf(stderr, "Pread failed on block; %s\n", strerror(errno));
	      _exit(1);
	    }
	  while(block_offset < b_size)
	    {
	      memcpy((void *) &curr_entry, (void *) &block_buffer[block_offset], sizeof(struct ext2_dir_entry));
	      if(curr_entry.name_len)
		{ 
		  fprintf(stdout, "DIRENT,%d,%d,%d,%d,%d,'%s'\n", print_inode_num, block_offset, curr_entry.inode, curr_entry.rec_len, curr_entry.name_len, curr_entry.name);
		}
	      block_offset += curr_entry.rec_len;
	    }
	  block_offset = 0;
	}
    }

  if(itable[inode_num].i_block[12])
    {
      block_offset_single = 0;
      block_offset = 0;
      read_offset = itable[inode_num].i_block[12] * b_size;
      if(pread(image_fd, (void *) single_addr, b_size, read_offset) < 0)
	{
	  fprintf(stderr, "Pread failed on block; %s\n", strerror(errno));
	  _exit(1);
	}
      while(block_offset_single < num_ptrs)
	{
	  __u32 block_num_single = single_addr[block_offset_single];
	  if(block_num_single)
	    {
	      read_offset = block_num_single * b_size;
	      if(pread(image_fd, (void *) block_buffer, b_size, read_offset) < 0)
		{
		  fprintf(stderr, "Pread failed on block; %s\n", strerror(errno));
		  _exit(1);
		}
	      while(block_offset < b_size)
		{
		  memcpy((void *) &curr_entry, (void *) &block_buffer[block_offset], sizeof(struct ext2_dir_entry));
		  if(curr_entry.name_len)
		    { 
		      fprintf(stdout, "DIRENT,%d,%d,%d,%d,%d,%s\n", print_inode_num, block_offset, curr_entry.inode, curr_entry.rec_len, curr_entry.name_len, curr_entry.name);
		    }
		  block_offset += curr_entry.rec_len;
		}
	      block_offset = 0;
	    }
	  block_offset_single++;
	}
    }

  if(itable[inode_num].i_block[13])
    {
      block_offset_double = 0;
      block_offset_single = 0;
      block_offset = 0;
      read_offset = itable[inode_num].i_block[13] * b_size;
      if(pread(image_fd, (void *) double_addr, b_size, read_offset) < 0)
	{
	  fprintf(stderr, "Pread failed on block; %s\n", strerror(errno));
	  _exit(1);
	}
      while(block_offset_double < num_ptrs)
	{
	  __u32 block_num_double = double_addr[block_offset_double];
	  if(block_num_double)
	    {
	      read_offset = block_num_double * b_size;
	      if(pread(image_fd, (void *) single_addr, b_size, read_offset) < 0)
		{
		  fprintf(stderr, "Pread failed on block; %s\n", strerror(errno));
		  _exit(1);
		}
	      while(block_offset_single < num_ptrs)
		{
		  __u32 block_num_single = single_addr[block_offset_single];
		  if(block_num_single)
		    {
		      read_offset = block_num_single * b_size;
		      if(pread(image_fd, (void *) block_buffer, b_size, read_offset) < 0)
			{
			  fprintf(stderr, "Pread failed on block; %s\n", strerror(errno));
			  _exit(1);
			}
		      while(block_offset < b_size)
			{
			  memcpy((void *) &curr_entry, (void *) &block_buffer[block_offset], sizeof(struct ext2_dir_entry));
			  if(curr_entry.name_len)
			    { 
			      fprintf(stdout, "DIRENT,%d,%d,%d,%d,%d,%s\n", print_inode_num, block_offset, curr_entry.inode, curr_entry.rec_len, curr_entry.name_len, curr_entry.name);
			    }
			  block_offset += curr_entry.rec_len;
			}
		      block_offset = 0;
		    }
		  block_offset_single++;
		}
	    }
	  block_offset_double++;
	}
    }

  if(itable[inode_num].i_block[14])
    {
      block_offset_triple = 0;
      block_offset_double = 0;
      block_offset_single = 0;
      block_offset = 0;
      read_offset = itable[inode_num].i_block[14] * b_size;
      if(pread(image_fd, (void *) triple_addr, b_size, read_offset) < 0)
	{
	  fprintf(stderr, "Pread failed on block; %s\n", strerror(errno));
	  _exit(1);
	}
      while(block_offset_triple < num_ptrs)
	{
	  __u32 block_num_triple = triple_addr[block_offset_triple];
	  if(block_num_triple)
	    {
	      read_offset = block_num_triple * b_size;
	      if(pread(image_fd, (void *) triple_addr, b_size, read_offset) < 0)
		{
		  fprintf(stderr, "Pread failed on block; %s\n", strerror(errno));
		  _exit(1);
		}
	      while(block_offset_double < num_ptrs)
		{
		  __u32 block_num_double = double_addr[block_offset_double];
		  if(block_num_double)
		    {
		      read_offset = block_num_double * b_size;
		      if(pread(image_fd, (void *) single_addr, b_size, read_offset) < 0)
			{
			  fprintf(stderr, "Pread failed on block; %s\n", strerror(errno));
			  _exit(1);
			}
		      while(block_offset_single < num_ptrs)
			{
			  __u32 block_num_single = single_addr[block_offset_single];
			  if(block_num_single)
			    {
			      read_offset = block_num_single * b_size;
			      if(pread(image_fd, (void *) block_buffer, b_size, read_offset) < 0)
				{
				  fprintf(stderr, "Pread failed on block; %s\n", strerror(errno));
				  _exit(1);
				}
			      while(block_offset < b_size)
				{
				  memcpy((void *) &curr_entry, (void *) &block_buffer[block_offset], sizeof(struct ext2_dir_entry));
				  if(curr_entry.name_len)
				    { 
				      fprintf(stdout, "DIRENT,%d,%d,%d,%d,%d,%s\n", print_inode_num, block_offset, curr_entry.inode, curr_entry.rec_len, curr_entry.name_len, curr_entry.name);
				    }
				  block_offset += curr_entry.rec_len;
				}
			      block_offset = 0;
			    }
			  block_offset_single++;
			}
		    }
		  block_offset_double++;
		}
	    }
	  block_offset_triple++;
	}
    }


}

void print_lv1(__u32 inode_num, __u32 block_num, __u32 offset)
{
  __u32 print_inode = inode_num + 1;
  __u32 num_ptrs = b_size / 4;
  __u32 block_offset_single = 0;
  off_t read_offset = block_num * b_size;
  if(pread(image_fd, (void *) single_addr, b_size, read_offset) < 0)
    {
      fprintf(stderr, "Pread failed on block; %s\n", strerror(errno));
      _exit(1);
    }
  while(block_offset_single < num_ptrs)
    {
      __u32 block_num_single = single_addr[block_offset_single];
      if(block_num_single)
	{
	  fprintf(stdout, "INDIRECT,%d,1,%d,%d,%d\n", print_inode, offset, block_num, block_num_single);
	}
      block_offset_single++;
      offset++;
    }
}

void print_lv2(__u32 inode_num, __u32 block_num, __u32 offset)
{
__u32 print_inode = inode_num + 1;
  __u32 num_ptrs = b_size / 4;
  __u32 block_offset_double = 0;
  off_t read_offset = block_num * b_size;
  if(pread(image_fd, (void *) double_addr, b_size, read_offset) < 0)
    {
      fprintf(stderr, "Pread failed on block; %s\n", strerror(errno));
      _exit(1);
    }
  while(block_offset_double < num_ptrs)
    {
      __u32 block_num_double = double_addr[block_offset_double];
      if(block_num_double)
	{
	  fprintf(stdout, "INDIRECT,%d,2,%d,%d,%d\n", print_inode, offset, block_num, block_num_double);
	  print_lv1(inode_num, block_num_double, offset);
	}
      block_offset_double++;
      offset += num_ptrs;
    }
}

void print_lv3(__u32 inode_num, __u32 block_num, __u32 offset)
{
  __u32 print_inode = inode_num + 1;
  __u32 num_ptrs = b_size / 4;
  __u32 inc_offset = num_ptrs * num_ptrs;
  __u32 block_offset_triple = 0;
  off_t read_offset = block_num * b_size;
  if(pread(image_fd, (void *) triple_addr, b_size, read_offset) < 0)
    {
      fprintf(stderr, "Pread failed on block; %s\n", strerror(errno));
      _exit(1);
    }
  while(block_offset_triple < num_ptrs)
    {
      __u32 block_num_triple = triple_addr[block_offset_triple];
      if(block_num_triple)
	{
	  fprintf(stdout, "INDIRECT,%d,3,%d,%d,%d\n", print_inode, offset, block_num, block_num_triple);
	  print_lv2(inode_num, block_num_triple, offset);
	}
      block_offset_triple++;
      offset += inc_offset;
    }
}

void print_indirect(__u32 inode_num)
{
  if(itable[inode_num].i_block[12])
    {
      print_lv1(inode_num, itable[inode_num].i_block[12], 12);
    }
  if(itable[inode_num].i_block[13])
    {
      print_lv2(inode_num, itable[inode_num].i_block[13], 268);
    }
  if(itable[inode_num].i_block[14])
    {
      print_lv3(inode_num, itable[inode_num].i_block[14], 65804);
    }
}

void traverse_imap()
{
  //traverse imap for free inodes
  __u32 i_count = 0;
  for(; i_count < num_inodes;)
    {
      curr_c = imap[i_count / 8];
      if(!(mask & (curr_c >> (i_count % 8))))
	{
	  i_count++;
	  fprintf(stdout, "IFREE,%d\n", i_count);
	  continue;
	}
      unsigned char type = '?';
      unsigned int mode = itable[i_count].i_mode;
      if((mode & 0xA000) == 0xA000)
	type = 's';
      else if((mode & 0x8000))
	type = 'f';
      else if((mode & 0x4000))
	type = 'd';
      if(type == '?')
	{
	  i_count++;
	  continue;
	}
      mode = mode & 0x0FFF;
      struct tm tbuffer;
      time_t tsec;
      char crtime[100];
      tsec = itable[i_count].i_ctime;
      tbuffer = *gmtime(&tsec);
      strftime(crtime, 100, "%m/%d/%y %H:%M:%S", &tbuffer);
      char modtime[100];
      tsec = itable[i_count].i_mtime;
      tbuffer = *gmtime(&tsec);
      strftime(modtime, 100, "%m/%d/%y %H:%M:%S", &tbuffer);
      char actime[100];
      tsec = itable[i_count].i_atime;
      tbuffer = *gmtime(&tsec);
      strftime(actime, 100, "%m/%d/%y %H:%M:%S", &tbuffer);
      __u32 inode_blocks = itable[i_count].i_blocks / (1 << curr_superblock.s_log_block_size);
      i_count++;
      fprintf(stdout, "INODE,%d,%c,%o,%d,%d,%d,%s,%s,%s,%d,%d,", i_count, type, mode, itable[i_count - 1].i_uid, itable[i_count - 1].i_gid, itable[i_count - 1].i_links_count, crtime, modtime, actime, itable[i_count - 1].i_size, inode_blocks);
      i_count--;
      int in_ptr = 0;
      for(;in_ptr < 14; in_ptr++)
	{
	  fprintf(stdout, "%d,",itable[i_count].i_block[in_ptr]);
	}
      fprintf(stdout, "%d\n", itable[i_count].i_block[14]);

      
      //if directory
      if(type == 'd')
	{
	  read_dcontent(i_count);
	}
      else
	{
	  print_indirect(i_count);
	}
      i_count++;
    }
}

int main(int argc, char *argv[])
{
  //check arguments and open file
  if(argc != 2)
    {
      fprintf(stderr, "Invalid number of arguments\n");
      _exit(1);
    }
  if((image_fd = open(argv[1], O_RDONLY)) < 0)
    {
      fprintf(stderr, "Open failed; %s\n", strerror(errno));
      _exit(1);
    }

  init_superb_bgroup(0);
  init_map_table();
  block_buffer = malloc(b_size * sizeof(unsigned char));
  single_addr = malloc(b_size);
  double_addr = malloc(b_size);
  triple_addr = malloc(b_size);
  print_superb_bgroup(0);
  traverse_bmap();
  traverse_imap();
  __u32 g_num = 1;
  for(;g_num < num_groups; g_num++)
    {
      init_superb_bgroup(0);
      init_map_table();
      print_superb_bgroup(0);
      traverse_bmap();
      traverse_imap();
    }
  close(image_fd);
  free(block_buffer);
  free(single_addr);
  free(double_addr);
  free(triple_addr);
  free(bmap);
  free(imap);
  free(itable);
  fflush(stdout);
  _exit(0);
}
