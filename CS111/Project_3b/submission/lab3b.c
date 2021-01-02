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

struct inode
{
  __u32 inode_num;
  __u32 link_count;
  __u32 blocks[15];
  struct inode *next;
};

struct indirect
{
  __u32 parent_inode;
  __u32 level;
  __u32 block_num;
  __u32 offset;
  struct indirect *next;
};

struct dirent
{
  __u32 parent_inode;
  char name[256];
  __u32 inode_num;
  struct dirent *next;
};

struct blockgroup
{
  __u32 block_offset;
  __u32 inode_offset;
  __u32 g_num_blocks;                //count using local block/inode offset
  __u32 g_num_inodes;
  __u32 non_res_block;
  struct inode *inode_list;          //for list of allocated blocks + traverse inode entries for comparing with bmap&allocb + 
  struct indirect *indirect_list;    //for list of indirect allocated blocks (offset + level) (traverse this and above when init for complete bmap and bdup)
  struct dirent *dirent_list;        //list of inode references (and names and )
};                                   //bdup for dupiclate block references; indup for duplicate inode references

struct backtrace
{
  struct dirent *entry;
  struct backtrace *next;
};

FILE *csv;
struct blockgroup *bgroups = NULL;
char *line_buffer = NULL;
size_t buffer_size;
unsigned char *bfree_map = NULL;
__u32 *bref_count = NULL;
unsigned char *ifree_map = NULL;
unsigned char *i_alloc = NULL;
__u32 *iref_count = NULL;
struct backtrace **bt = NULL;

//superblock info
__u32 non_res_inode;
__u32 block_size;
__u32 blocks_p_group;
__u32 inodes_p_group;
__u32 total_groups = 1;
__u32 blocks_total;
__u32 inodes_total;

//superblock csv info
__u32 num_groups = 0;

void free_data()
{
  if(bgroups)
    {
      __u32 curr_group = 0;
      for(;curr_group < total_groups; curr_group++)
	{
	  struct inode *inode_ptr1 = bgroups[curr_group].inode_list;
	  struct indirect *indirect_ptr1 = bgroups[curr_group].indirect_list;
	  struct dirent *dirent_ptr1 = bgroups[curr_group].dirent_list;
	  struct inode *inode_ptr2;
	  struct indirect *indirect_ptr2;
	  struct dirent *dirent_ptr2;
	  while(inode_ptr1)
	    {
	      inode_ptr2 = inode_ptr1;
	      inode_ptr1 = inode_ptr1->next;
	      free(inode_ptr2);
	    }
	  while(indirect_ptr1)
	    {
	      indirect_ptr2 = indirect_ptr1;
	      indirect_ptr1 = indirect_ptr1->next;
	      free(indirect_ptr2);
	    }
	  while(dirent_ptr1)
	    {
	      dirent_ptr2 = dirent_ptr1;
	      dirent_ptr1 = dirent_ptr1->next;
	      free(dirent_ptr2);
	    }
	}
    }
  __u32 curr_bt = 0;
  for(;curr_bt < inodes_total; curr_bt++)
    {
      while(bt[curr_bt])
	{
	  struct backtrace *buffer = bt[curr_bt];
	  bt[curr_bt] = bt[curr_bt]->next;
	  free(buffer);
	}
    }
  free(bgroups);
  free(bfree_map);
  free(bref_count);
  free(ifree_map);
  free(i_alloc);
  free(iref_count);
  free(bt);
  free(line_buffer);
}

void init_general()
{
  line_buffer = malloc(20 * sizeof(char));
  buffer_size = 20;
  fseek(csv, 0, SEEK_SET);
  int sb_found = 0;
  char superblock[] = "SUPERBLOCK";
  while(!(feof(csv)))
    {
      getline(&line_buffer, &buffer_size, csv);
      if(!(strncmp(line_buffer, superblock, 10)))
	{
	  sb_found = 1;
	  break;
	}
    }
  if(!sb_found)
    {
      fprintf(stderr, "Superblock info not found\n");
      free(line_buffer);
      fclose(csv);
      _exit(1);
    }
  if(sscanf(line_buffer, "%*[^,],%u,%u,%u,%*[^,],%u,%u,%u\n", &blocks_total, &inodes_total, &block_size, &blocks_p_group, &inodes_p_group, &non_res_inode) != 6)
    {
      fprintf(stderr, "Invalid SUPERBLOCK entry\n");
      free(line_buffer);
      fclose(csv);
      _exit(1);
    }
  //total_groups = (blocks_total + blocks_p_group - 1) / blocks_p_group;
}


void init_data()
{
  bgroups = malloc(total_groups * sizeof(struct blockgroup));
  __u32 group_num = 0;
  for(; group_num < total_groups; group_num++)
    {
      bgroups[group_num].block_offset = group_num * blocks_p_group;
      bgroups[group_num].inode_offset = group_num * inodes_p_group;
      bgroups[group_num].g_num_blocks = 0;
      bgroups[group_num].g_num_inodes = 0;
      bgroups[group_num].non_res_block = bgroups[group_num].block_offset;
      bgroups[group_num].inode_list = NULL;
      bgroups[group_num].indirect_list = NULL;
      bgroups[group_num].dirent_list = NULL;
    }
  bfree_map = malloc(blocks_total * sizeof(unsigned char));
  memset(bfree_map, -1, blocks_total * sizeof(unsigned char));
  bref_count = malloc(blocks_total * sizeof(__u32));
  memset(bref_count, 0, blocks_total * sizeof(__u32));
  ifree_map = malloc(inodes_total * sizeof(unsigned char));
  memset(ifree_map, -1, inodes_total * sizeof(unsigned char));
  i_alloc = malloc(inodes_total * sizeof(unsigned char));
  memset(i_alloc, 0, inodes_total * sizeof(unsigned char));
  iref_count = malloc(inodes_total * sizeof(__u32));
  memset(iref_count, 0, inodes_total * sizeof(__u32));
  bt = malloc(inodes_total * sizeof(struct backtrace*));
  memset(bt, 0, inodes_total * sizeof(struct backtrace*));

  fseek(csv, 0, SEEK_SET);
  char str_superblock[] = "SUPERBLOCK";
  char str_group[] = "GROUP";
  char str_bfree[] = "BFREE";
  char str_ifree[] = "IFREE";
  char str_inode[] = "INODE";
  char str_dirent[] = "DIRENT";
  char str_indirect[] = "INDIRECT";
  getline(&line_buffer, &buffer_size, csv);
  while(!(feof(csv)))
    {
      if(feof(csv))
	break;
      if(!(strncmp(line_buffer, str_superblock, 10)))
	{
	  getline(&line_buffer, &buffer_size, csv);
	  continue;
	}
      else if(!(strncmp(line_buffer, str_group, 5)))
	{
	  __u32 group_index;
	  __u32 blocks;
	  __u32 inodes;
	  __u32 first_inode;
	  if((sscanf(line_buffer, "%*[^,],%u,%u,%u,%*[^,],%*[^,],%*[^,],%u\n", &group_index, &blocks, &inodes, &first_inode) != 4) || group_index >= total_groups)
	    {
	      fprintf(stderr, "Invalid GROUP entry\n");
	      free_data();
	      fclose(csv);
	      _exit(1);
	    }
	  if(group_index < total_groups)
	    {
	      bgroups[group_index].g_num_blocks = blocks;
	      bgroups[group_index].g_num_inodes = inodes;
		  bgroups[group_index].non_res_block = first_inode + ((inodes * sizeof(struct ext2_inode))/block_size) + 1;
		  __u32 curr_res;
		  __u32 max_res = bgroups[group_index].inode_offset + non_res_inode - 1;
		  for(curr_res = bgroups[group_index].inode_offset; curr_res < max_res; curr_res++)
		    {
			  if(curr_res < inodes_total)
				i_alloc[curr_res] = 'a';
		    }
	    }
	}
      else if(!(strncmp(line_buffer, str_bfree, 5)))
	{
	  __u32 free_block;
	  if((sscanf(line_buffer, "%*[^,],%u\n", &free_block) != 1) || free_block >= blocks_total)
	    {
	      fprintf(stderr, "Invalid BFREE entry\n");
	      free_data();
	      fclose(csv);
	      _exit(1);
	    }
	  if(free_block < blocks_total)
	    bfree_map[free_block] = '\0';
	}
      else if(!(strncmp(line_buffer, str_ifree, 5)))
	{
	  __u32 free_inode;
	  if((sscanf(line_buffer, "%*[^,],%u\n", &free_inode) != 1) || free_inode > inodes_total)
	    {
	      fprintf(stderr, "Invalid IFREE entry\n");
	      free_data();
	      fclose(csv);
	      _exit(1);
	    }
	  free_inode--;
	  if(free_inode < inodes_total)
	    ifree_map[free_inode] = '\0';
	}
      else if(!(strncmp(line_buffer, str_inode, 5)))
	{
	  struct inode *in_buffer = malloc(sizeof(struct inode));
	  if((sscanf(line_buffer, "%*[^,],%u,%*[^,],%*[^,],%*[^,],%*[^,],%u,%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\n", &in_buffer->inode_num, &in_buffer->link_count, &in_buffer->blocks[0], &in_buffer->blocks[1], &in_buffer->blocks[2], &in_buffer->blocks[3], &in_buffer->blocks[4], &in_buffer->blocks[5], &in_buffer->blocks[6], &in_buffer->blocks[7], &in_buffer->blocks[8], &in_buffer->blocks[9], &in_buffer->blocks[10], &in_buffer->blocks[11], &in_buffer->blocks[12], &in_buffer->blocks[13], &in_buffer->blocks[14]) != 17))
	    {
	      fprintf(stderr, "Invalid INODE entry\n");
	      free_data();
	      fclose(csv);
	      _exit(1);
	    }
	  in_buffer->next = bgroups[0].inode_list;
	  bgroups[0].inode_list = in_buffer;
	  __u32 inode_block = 0;
	  for(; inode_block < 15; inode_block++)
	    {
	      if(in_buffer->blocks[inode_block] && (in_buffer->blocks[inode_block] < blocks_total))
		    bref_count[in_buffer->blocks[inode_block]]++;
	    }
	  if ((in_buffer->inode_num - 1) < inodes_total)
		  i_alloc[in_buffer->inode_num - 1] = 'a';
	}
      else if(!(strncmp(line_buffer, str_dirent, 6)))
	{
	  struct dirent *dirent_buffer = malloc(sizeof(struct dirent));
	  if(sscanf(line_buffer, "%*[^,],%u,%*[^,],%u,%*[^,],%*[^,],\'%[^\']\'\n", &dirent_buffer->parent_inode, &dirent_buffer->inode_num, (char *) &dirent_buffer->name) != 3)
	    {
	      fprintf(stderr, "Invalid DIRENT entry\n");
	      free_data();
	      fclose(csv);
	      _exit(1);
	    }
      dirent_buffer->next = bgroups[0].dirent_list;
	  bgroups[0].dirent_list = dirent_buffer;
	  if((dirent_buffer->inode_num - 1) < inodes_total)
	    {
	     
	      iref_count[dirent_buffer->inode_num - 1]++;
	      struct backtrace *bt_buffer = malloc(sizeof(struct backtrace));
	      bt_buffer->entry = dirent_buffer;
	      bt_buffer->next = bt[dirent_buffer->inode_num - 1];
	      bt[dirent_buffer->inode_num - 1] = bt_buffer;
	    }
	}
      else if(!(strncmp(line_buffer, str_indirect, 8)))
	{
	  struct indirect *indirect_buffer = malloc(sizeof(struct indirect));
	  if(sscanf(line_buffer, "%*[^,],%u,%u,%u,%*[^,],%u\n", &indirect_buffer->parent_inode, &indirect_buffer->level, &indirect_buffer->offset, &indirect_buffer->block_num) != 4)
	    {
	      fprintf(stderr, "Invalid INDIRECT entry\n");
	      free_data();
	      fclose(csv);
	      _exit(1);
	    }
	  if(indirect_buffer->block_num < blocks_total)
	    {
	      indirect_buffer->next = bgroups[indirect_buffer->block_num / blocks_p_group].indirect_list;
	      bgroups[indirect_buffer->block_num / blocks_p_group].indirect_list = indirect_buffer;
	      bref_count[indirect_buffer->block_num]++;
	    }
	}
      else
	{
	  fprintf(stderr, "Invalid csv entry\n");
	  free_data();
	  fclose(csv);
	  _exit(1);
	}
      getline(&line_buffer, &buffer_size, csv);
    }
}

void scan_data()
{
  //scan block bitmap and count
  __u32 curr_group = 0;
  for(; curr_group < total_groups; curr_group++)
    {
      __u32 block_min = bgroups[curr_group].block_offset;
      __u32 block_max = bgroups[curr_group].block_offset + bgroups[curr_group].g_num_blocks;
      __u32 inode_min = bgroups[curr_group].inode_offset;
      __u32 inode_max = bgroups[curr_group].inode_offset + bgroups[curr_group].g_num_inodes;
      struct inode *curr_inode = bgroups[curr_group].inode_list;
      while(curr_inode)
	{
	  __u32 block_ptr = 0;
	  __u32 print_offset = 0;
	  for(;block_ptr < 15; block_ptr++)
	    {
	      if(curr_inode->blocks[block_ptr])
		{
		  if((curr_inode->blocks[block_ptr] < block_min) || (curr_inode->blocks[block_ptr] >= block_max))
		    {
			  if(block_ptr < 12)
			    printf("INVALID BLOCK %d IN INODE %d AT OFFSET %d\n", curr_inode->blocks[block_ptr], curr_inode->inode_num, print_offset);
			  else if(block_ptr == 12)
				printf("INVALID INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", curr_inode->blocks[block_ptr], curr_inode->inode_num, print_offset);
			  else if(block_ptr == 13)
				printf("INVALID DOUBLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", curr_inode->blocks[block_ptr], curr_inode->inode_num, print_offset);
			  else if(block_ptr == 14)
				printf("INVALID TRIPLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", curr_inode->blocks[block_ptr], curr_inode->inode_num, print_offset);
		    }
		  else if(curr_inode->blocks[block_ptr] < (bgroups[curr_group].non_res_block))
		    {
			  if(block_ptr < 12)
				printf("RESERVED BLOCK %d IN INODE %d AT OFFSET %d\n", curr_inode->blocks[block_ptr], curr_inode->inode_num, print_offset);
			  else if(block_ptr == 12)
				printf("RESERVED INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", curr_inode->blocks[block_ptr], curr_inode->inode_num, print_offset);
			  else if(block_ptr == 13)
				printf("RESERVED DOUBLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", curr_inode->blocks[block_ptr], curr_inode->inode_num, print_offset);
			  else if(block_ptr == 14)
				printf("RESERVED TRIPLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", curr_inode->blocks[block_ptr], curr_inode->inode_num, print_offset);
		    }
		  if((bref_count[curr_inode->blocks[block_ptr]] < blocks_total) && (bref_count[curr_inode->blocks[block_ptr]] > 1))
		    {
			  if(block_ptr < 12)
			    printf("DUPLICATE BLOCK %d IN INODE %d AT OFFSET %d\n", curr_inode->blocks[block_ptr], curr_inode->inode_num, print_offset);
			  else if(block_ptr == 12)
				printf("DUPLICATE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", curr_inode->blocks[block_ptr], curr_inode->inode_num, print_offset);
			  else if(block_ptr == 13)
				printf("DUPLICATE DOUBLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", curr_inode->blocks[block_ptr], curr_inode->inode_num, print_offset);
			  else if(block_ptr == 14)
				printf("DUPLICATE TRIPLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", curr_inode->blocks[block_ptr], curr_inode->inode_num, print_offset);
		    }
		}
	      if(block_ptr < 12)
		print_offset += 1;
	      else if(block_ptr == 12)
		print_offset += block_size / 4;
	      else if(block_ptr == 13)
		print_offset += (block_size * block_size) / 16;
	    }
	  curr_inode = curr_inode->next;
	}
      struct indirect *curr_ind_list = bgroups[curr_group].indirect_list;
      while(curr_ind_list)
	{
	  if((curr_ind_list->block_num < block_min) || (curr_ind_list->block_num >= block_max))
	    {
	      if(curr_ind_list->level == 1)
		printf("INVALID INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", curr_ind_list->block_num, curr_ind_list->parent_inode, curr_ind_list->offset);
	      else if(curr_ind_list->level == 2)
		printf("INVALID DOUBLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", curr_ind_list->block_num, curr_ind_list->parent_inode, curr_ind_list->offset);
	      else if(curr_ind_list->level == 3)
		printf("INVALID TRIPLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", curr_ind_list->block_num, curr_ind_list->parent_inode, curr_ind_list->offset);
	    }
	  else if(curr_ind_list->block_num < (bgroups[curr_group].non_res_block))
	    {
	      if(curr_ind_list->level == 1)
		printf("RESERVED INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", curr_ind_list->block_num, curr_ind_list->parent_inode, curr_ind_list->offset);
	      else if(curr_ind_list->level == 2)
		printf("RESERVED DOUBLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", curr_ind_list->block_num, curr_ind_list->parent_inode, curr_ind_list->offset);
	      else if(curr_ind_list->level == 3)
		printf("RESERVED TRIPLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", curr_ind_list->block_num, curr_ind_list->parent_inode, curr_ind_list->offset);
	    }
	  if((curr_ind_list->block_num < blocks_total) && (bref_count[curr_ind_list->block_num] > 1))
	    {
	      if(curr_ind_list->level == 1)
		printf("DUPLICATE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", curr_ind_list->block_num, curr_ind_list->parent_inode, curr_ind_list->offset);
	      else if(curr_ind_list->level == 2)
		printf("DUPLICATE DOUBLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", curr_ind_list->block_num, curr_ind_list->parent_inode, curr_ind_list->offset);
	      else if(curr_ind_list->level == 3)
		printf("DEPLICATE TRIPLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", curr_ind_list->block_num, curr_ind_list->parent_inode, curr_ind_list->offset);
	    }
	  curr_ind_list = curr_ind_list->next;
	}
      __u32 curr_block = bgroups[curr_group].non_res_block;
      for(;curr_block < block_max; curr_block++)
	{
	  if(bfree_map[curr_block] && !(bref_count[curr_block]))
	    printf("UNREFERENCED BLOCK %d\n", curr_block);
	  else if(!(bfree_map[curr_block]) && bref_count[curr_block])
	    printf("ALLOCATED BLOCK %d ON FREELIST\n", curr_block);
	}
      __u32 inode_num = inode_min;
      for(; inode_num < inode_max; inode_num++)
	{
	  if(ifree_map[inode_num] && !(i_alloc[inode_num]))
	    {
	      inode_num++;
	      printf("UNALLOCATED INODE %d NOT ON FREELIST\n", inode_num);
	      inode_num--;
	    }
	  else if(!(ifree_map[inode_num]) && i_alloc[inode_num])
	    {
	      inode_num++;
	      printf("ALLOCATED INODE %d ON FREELIST\n", inode_num);
	      inode_num--;
	      ifree_map[inode_num] = 'a';
	    }
	}
      curr_inode = bgroups[curr_group].inode_list;
      while(curr_inode)
	{
	  if(curr_inode->link_count != iref_count[curr_inode->inode_num - 1])
	    printf("INODE %d HAS %d LINKS BUT LINKCOUNT IS %d\n", curr_inode->inode_num, iref_count[curr_inode->inode_num - 1], curr_inode->link_count);
	  curr_inode = curr_inode->next;
	}
      struct dirent *curr_dirent = bgroups[curr_group].dirent_list;
      char string1[] = ".";
      char string2[] = "..";
      while(curr_dirent)
	{
	  if(curr_dirent->inode_num > inode_max)
	    printf("DIRECTORY INODE %d NAME \'%s\' INVALID INODE %d\n", curr_dirent->parent_inode, curr_dirent->name, curr_dirent->inode_num);
	  else if(!(ifree_map[curr_dirent->inode_num - 1]))
	    printf("DIRECTORY INODE %d NAME \'%s\' UNALLOCATED INODE %d\n", curr_dirent->parent_inode, curr_dirent->name, curr_dirent->inode_num);
	  if(!(strcmp(curr_dirent->name, string1)))
	    {
	      if(curr_dirent->parent_inode != curr_dirent->inode_num)
		printf("DIRECTORY INODE %d NAME \'.\' LINK TO INODE %d SHOULD BE %d\n", curr_dirent->parent_inode, curr_dirent->inode_num, curr_dirent->parent_inode);
	    }
	  else if(!(strcmp(curr_dirent->name, string2)))
	    {
	      if(curr_dirent->parent_inode == 2)
		{
		  if(curr_dirent->inode_num != 2)
		    printf("DIRECTORY INODE 2 NAME \'..\' LINK TO INODE %d SHOULD BE 2\n", curr_dirent->inode_num);
		}
	      else
		{
		  if(bt[curr_dirent->parent_inode - 1] && bt[curr_dirent->parent_inode - 1]->entry->inode_num != curr_dirent->parent_inode)
		    printf("DIRECTORY INODE %d NAME \'..\' LINK TO INODE %d SHOULD BE %d\n", curr_dirent->parent_inode, curr_dirent->inode_num, bt[curr_dirent->parent_inode - 1]->entry->parent_inode);
		}
	    }
	  curr_dirent = curr_dirent->next;
	}
    }
}

int main(int argc, char *argv[])
{
  if(argc != 2)
    {
      fprintf(stderr, "Invalid number of arguments\n");
      _exit(1);
    }
  if((csv = fopen(argv[1], "r")) == NULL)
    {
      fprintf(stderr, "Open failed; %s\n", strerror(errno));
      _exit(1);
    }
  
  init_general();
  init_data();
  scan_data();
  free_data();
  fclose(csv);
  fflush(stdout);
  _exit(0);
}

//Some of the sites referenced
//http://man7.org/linux/man-pages/man3/getline.3.html
//http://man7.org/linux/man-pages/man3/fopen.3.html
//http://man7.org/linux/man-pages/man3/scanf.3.html     (use sscanf)
//https://www.tutorialspoint.com/c_standard_library/c_function_fseek.htm
//http://man7.org/linux/man-pages/man3/feof.3p.html
//https://www.tutorialspoint.com/c_standard_library/c_function_strstr.htm
