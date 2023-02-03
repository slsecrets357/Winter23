#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "disk_emu.h"
#include "sfs_api.h"


/* README

ECSE 427 assignment 3 - simple file system
author: Ze Yuan Fu - 260963676

please note:
-the maximum file name length parameter in the tests has been changed to 59
-the maximum directory size parameter in the tests has been changed to 256
-the maximum file size parameter in the tests has been changed to 274432

Wishing you a pleasant grading experience~ */


#define NUMBER_BLOCKS 2048                      // number of blocks in the file system
#define BLOCK_SIZE 1024                         // size of each block in bytes
#define DIRECTORY_SIZE 256                      // maximum number of files the directory is able to hold
#define INODE_TABLE_SIZE 256                    // maximum number of files that can be created
#define MAGIC 0xACBD0005                        // file system magic number
#define FILESYSTEMNAME "sfsdisk"                // name of the file system file name
#define MAXFILENAME 64 - sizeof(int) - 1        // maximum file name length in characters, 59
#define MAXFILESIZE 12*BLOCK_SIZE + BLOCK_SIZE*BLOCK_SIZE/sizeof(int)   // maximum size of a single file in bytes, 274432
#define FREE_BLOCKS_START_INDEX 35              // index of first block that is free to be allocated to a file
// free blocks: 35 to 2047


// data structure for the disk block, containing a char array of the predefined block size
typedef struct block {
    char data[BLOCK_SIZE];          // 1024 bytes
} block;

// data structure for the disk superblock, containing metadata about the file system
typedef struct superblock {         // size of 1024 bytes since it's a block
    int magic;                      // magic number
    int blocksize;                  // size of disk blocks
    int fssize;                     // number of blocks in the file system
    int inode_table_length;         // number of entries in the inode table
    int root_directory;             // location of the root directory
} superblock;

// data structure for the inode, containing metadata about the file and pointers to the data
typedef struct inode {              // 64 bytes -> 16 inodes per block
    int mode;                       // -1 = not in use; 0 = in use but not opened, 1 = in use and opened
    int link_cnt;                   // number of blocks linked to by this inode (-1 = not in use)
    int size;                       // size of the data contained by the file in bytes
    int direct[12];                 // list of direct data block indexes
    int indirect;                   // index of block containing data block indexes
} inode;

// data structure for directory entries, containing data about the file it points to
typedef struct directory_entry {    // 64 bytes
    char filename[MAXFILENAME+1];   // 59 characters + null terminator
    int inode;                      // index of inode corresponding to the file
} directory_entry;

// data structure for file descriptor table entries
typedef struct ofdt_entry {
    int inode;                      // index of inode corresponding to the file
    int rwpointer;                  // read/write pointer location
} ofdt_entry;


// in memory data structures
superblock *sblock;                                     // superblock, goes in disk, 1 block
inode inode_table[INODE_TABLE_SIZE];                    // inode table, goes in disk, 16 inodes per block => 16 blocks
directory_entry directory[DIRECTORY_SIZE];              // file directory, goes in disk, 64 entries per block => 16 blocks
char free_bitmap[NUMBER_BLOCKS];                        // free bitmap, goes in disk, 1024 chars per block => 2 blocks, 1 = in use, \0 = unused
ofdt_entry file_descriptor_table[INODE_TABLE_SIZE];     // file descriptor table, does not go in disk
int directory_location;                                 // index for sfs_getnextfilename()


// initializes disk, superblock, inode table, free bitmap, directory and file descriptor table
void mksfs(int fresh) {
    // if a fresh file system was specified
    if (fresh) {
        // if a disk already exists, delete it
        if (access(FILESYSTEMNAME, F_OK) == 0) {
            remove(FILESYSTEMNAME);
        }

        // initialize new file system disk
        if (init_fresh_disk(FILESYSTEMNAME, BLOCK_SIZE, NUMBER_BLOCKS)) {
            perror("failed to create file system");
            return;
        }

        // create superblock (block 0) and initialize its fields
        sblock = (superblock *) malloc(sizeof(block));
        sblock->magic = MAGIC;
        sblock->blocksize = BLOCK_SIZE;
        sblock->fssize = NUMBER_BLOCKS;
        sblock->inode_table_length = 0;
        sblock->root_directory = 19;

        // write superblock to disk
        if (write_blocks(0, 1, sblock) == -1) {
            printf("failed to write superblock \n");
            return;
        }

        // create inode table (blocks 1 to 16) and initialize each inode's fields
        for (int i = 0; i < INODE_TABLE_SIZE; i++) {
            inode_table[i].mode = -1;
            inode_table[i].link_cnt = 0;
            inode_table[i].size = 0;
            inode_table[i].indirect = -1;
            for (int j = 0; j < 12; j++) {
                inode_table[i].direct[j] = -1;
            }
        }

        // write inode table to disk
        if (write_blocks(1, 16, &inode_table) == -1) {
            printf("failed to write inodes \n");
            return;
        }

        // create free bitmap (blocks 17 and 18) and set initial values
        memset(&free_bitmap, '\0', NUMBER_BLOCKS);
        for (int i = 0; i < FREE_BLOCKS_START_INDEX; i++) {
            free_bitmap[i] = 1;       // for system occupied blocks
        }

        // write free bitmap to disk
        if (write_blocks(17, 2, &free_bitmap) == -1) {
            printf("failed to write bitmap \n");
            return;
        }

        //create directory (blocks 19 to 34) and initialize each entry's fields
        for (int i = 0; i < DIRECTORY_SIZE; i++) {
            directory[i].inode = -1;
            memset(directory[i].filename, 0, MAXFILENAME+1);
            //strcpy(directory[i].filename, "\0");
            // for (int j = 0; j < MAXFILENAME; j++) {
            //     directory[i].filename[j] = '\0';
            // }
        }

        // write directory to disk
        if (write_blocks(19, 16, &directory) == -1) {
            printf("failed to write directories \n");
            return;
        }
    } else {    // loading file system from existing file
        // initialize disk from file
        if (init_disk(FILESYSTEMNAME, BLOCK_SIZE, NUMBER_BLOCKS)) {
            printf("failed to load file system \n");
            return;
        }

        // read superblock
        sblock = (superblock *) malloc(sizeof(block));
        if (read_blocks(0, 1, sblock) == -1) {
            printf("failed to load superblock \n");
            return;
        }
        printf("superblock magic: %x \n", sblock->magic);

        // read inode table blocks
        if (read_blocks(1, 16, inode_table) == -1) {
            printf("failed to load inode table \n");
            return;
        }

        // read free bitmap blocks
        if (read_blocks(17, 2, &free_bitmap) == -1) {
            printf("failed to load bitmap \n");
            return;
        }

        // read directory blocks
        if (read_blocks(19, 16, &directory) == -1) {
            printf("failed to load directory \n");
            return;
        }
    }

    directory_location = 0;     // set dirloc index for getnextfilename

    // create file descriptor table and set initial entry fields
    for (int i = 0; i < INODE_TABLE_SIZE; i++) {
        file_descriptor_table[i].inode = -1;
        file_descriptor_table[i].rwpointer = 0;
    }
    printf("loaded file system with %d inodes \n", sblock->inode_table_length);

}

// returns the name of the next file in the directory
int sfs_getnextfilename(char *fname) {
    // if the starting directory location is a valid index, run
    if (directory_location < DIRECTORY_SIZE){
        // iterate through the directory until we find the next occupied entry
		while (directory[directory_location].inode < 0){
			directory_location++;
            // if we exceeded the highest index, reset directory location and return 0
			if (directory_location >= DIRECTORY_SIZE){
				directory_location = 0;
				return 0;
			}
		}
        // found a file
        //printf("found file at dirloc %d: %s with inode %d \n", directory_location, directory[directory_location].filename, directory[directory_location].inode);
		strcpy(fname, directory[directory_location].filename);      // copy file name into provided buffer
		directory_location++;                                       // increment directory location index for the next run
        return 1;
	} else {    // otherwise reset it to zero and return 0
		directory_location = 0;
		return 0;
	}
}

// returns the size of the file specified in path
int sfs_getfilesize(const char *path) {
    for (int i = 0; i < DIRECTORY_SIZE; i++) {              // iterate over each file
        if (strcmp(directory[i].filename, path) == 0) {
            return inode_table[directory[i].inode].size;    // if file name matches parameter, return its size
        }
    }
    return -1;      // file not found
}

// opens a file with the given name, returning the index of its entry in the file descriptor table
int sfs_fopen(char *name) {
    // check if file name is valid
    if (strlen(name) > MAXFILENAME) {
        printf("file name too long \n");
        return -1;
    }

    // check if the file exists in the directory
    int directory_index = -1;
    for (int i = 0; i < DIRECTORY_SIZE; i++) {
        if (strcmp(directory[i].filename, name) == 0) {
            directory_index = i;        // file found
            break;
        }
    }
    if (directory_index > -1) {     // file found in directory
        // check if already open
        for (int i = 0; i < INODE_TABLE_SIZE; i++) {
            if (file_descriptor_table[i].inode == directory[directory_index].inode && file_descriptor_table[i].inode != -1) {
                return i;           // file already open
            }
        }
        // open existing file
        for (int i = 0; i < INODE_TABLE_SIZE; i++) {
            if (file_descriptor_table[i].inode < 0) {       // found empty fdt slot
                file_descriptor_table[i].inode = directory[directory_index].inode;
                file_descriptor_table[i].rwpointer = inode_table[directory[directory_index].inode].size;    // set to end of file
                //printf("opened file with size %d at dirloc %d, inode index %d \n", inode_table[directory[directory_index].inode].size, directory_index, directory[directory_index].inode);
                return i;       // return fdt index
            }
        }
        printf("no space in fdt \n");
        return -1;

    } else {        //create new file
        // indexes for search
        int new_inode_index = -1;
        int new_fdt_index = -1;
        int new_block_index = -1;

        for (int i = 0; i < DIRECTORY_SIZE; i++) {      // search for empty directory slot
            if (directory[i].inode < 0) {
                // found empty directory slot
                directory_index = i;
                break;
            }
        }

        for (int i = 0; i < INODE_TABLE_SIZE; i++) {    // search for empty inode table slot
            if (inode_table[i].mode < 0) {
                // found empty inode
                new_inode_index = i;
                break;
            }
        }

        for (int i = 0; i < INODE_TABLE_SIZE; i++) {    // search for empty fdt slot
            if (file_descriptor_table[i].inode < 0) {
                // found empty fdt slot
                new_fdt_index = i;
                break;
            }
        }

        for (int i = FREE_BLOCKS_START_INDEX; i < NUMBER_BLOCKS; i++) {      // search for free block to allocate for file
            if (free_bitmap[i] == '\0') {
                // found free block
                free_bitmap[i] = 1;
                new_block_index = i;
                break;
            }
        }

        // if all resources were found successfully
        if (directory_index > -1 && new_inode_index > -1 && new_fdt_index > -1 && new_block_index > -1) {
            // set directory and inode fields
            strcpy(directory[directory_index].filename, name);
            //memcpy(directory[directory_index].filename, name, MAXFILENAME);
            directory[directory_index].inode = new_inode_index;
            inode_table[new_inode_index].mode = 1;
            inode_table[new_inode_index].link_cnt = 1;
            inode_table[new_inode_index].size = 0;
            inode_table[new_inode_index].indirect = -1;
            // allocate initial block
            inode_table[new_inode_index].direct[0] = new_block_index;
            for (int i = 1; i < 12; i++) {
                inode_table[new_inode_index].direct[i] = -1;
            }
            file_descriptor_table[new_fdt_index].inode = new_inode_index;
            file_descriptor_table[new_fdt_index].rwpointer = 0;
            sblock->inode_table_length++;
            
            // write superblock to disk
            if (write_blocks(0, 1, sblock) == -1) {
                printf("failed to write superblock \n");
                return -1;
            }

            // write inode table to disk
            if (write_blocks(1, 16, &inode_table) == -1) {
                printf("failed to write inodes \n");
                return -1;
            }

            // write bitmap to disk
            if (write_blocks(17, 2, &free_bitmap) == -1) {
                printf("failed to write bitmap \n");
                return -1;
            }

            // write directories to disk
            if (write_blocks(19, 16, &directory) == -1) {
                printf("failed to write directories \n");
                return -1;
            }

            //printf("created new file with dirloc %d and inode index %d \n", directory_index, new_inode_index);
            return new_fdt_index;

        } else {        // if some or all resources were not found
            printf("insufficient directory, inode, block or fdt space \n");
            return -1;
        }
    }
}

// closes the file with the specified fdt index
int sfs_fclose(int fileID) {
    // check for valid fdt index
    if (fileID < 0 || fileID >= INODE_TABLE_SIZE || file_descriptor_table[fileID].inode < 0 || file_descriptor_table[fileID].inode < 0) {   // check for issues
        printf("invalid file id \n");
        return -1;
    }
    
    // reset fdt entry fields
    inode_table[file_descriptor_table[fileID].inode].mode = 0;
    file_descriptor_table[fileID].inode = -1;
    file_descriptor_table[fileID].rwpointer = 0;

    // write inode table to disk
    if (write_blocks(1, 16, &inode_table) == -1) {
        printf("failed to write inodes \n");
        return -1;
    }

    if (file_descriptor_table[fileID].inode == -1 && file_descriptor_table[fileID].rwpointer == 0) {
        return 0;
    } else {
        return -1;
    }
}

// writes length bytes from data buffer buf to the file specified by the fdt index fileID
int sfs_fwrite(int fileID, const char *buf, int length) {
    // check for valid fdt index and parameters
    if (fileID < 0 || fileID >= INODE_TABLE_SIZE || length < 0) {
        printf("invalid file id or length \n");
        return -1;
    }

    // check if an inode is associated with the fdt entry
    if (file_descriptor_table[fileID].inode < 0) {
        printf("no inode associated with fdt entry \n");
        return -1;
    }

    // check if the write would exceed the max file size
    if (file_descriptor_table[fileID].rwpointer + length > MAXFILESIZE) {
        printf("write exceeds max file size \n");
        return -1;
    }

    int inode_index = file_descriptor_table[fileID].inode;                              // index of inode in inode table
    int start_block = file_descriptor_table[fileID].rwpointer / BLOCK_SIZE;             // index of start block
    int start_block_offset = file_descriptor_table[fileID].rwpointer % BLOCK_SIZE;      // location to start writing from in the first block to write to
    int current_last_block_index = inode_table[inode_index].link_cnt-1;                 // index (direct/indirect) of the last data block of the file, pre-write
    int future_last_block_index = (file_descriptor_table[fileID].rwpointer + length) / BLOCK_SIZE;      // index (direct/indirect) of the last data block of the file, post-write

    // if the file has data addressed by indirect data blocks, load the block indexes into memory
    int indirect_pointers[BLOCK_SIZE/sizeof(int)];
    if (inode_table[inode_index].link_cnt > 12) {
        read_blocks(inode_table[inode_index].indirect, 1, indirect_pointers);
    } else if (future_last_block_index >= 12) {         // if the file does not have data addressed by indirect blocks but will need to address them by indirect blocks, allocate a new block for the indirect pointers
        int new_indirect_index = -1;
        for (int i = FREE_BLOCKS_START_INDEX; i < NUMBER_BLOCKS; i++) {
            if (free_bitmap[i] == '\0') {
                free_bitmap[i] = 1;
                new_indirect_index = i;
                break;
            }
        }
        // no blocks free for the indirect pointer block
        if (new_indirect_index < 0) {
            printf("no free blocks to allocate for indirect \n");
            return -1;
        }
        inode_table[inode_index].indirect = new_indirect_index;
    }

    // allocate new blocks for data to be written
    if (future_last_block_index-current_last_block_index > 0) {
        for (int i = inode_table[inode_index].link_cnt; i <= future_last_block_index; i++) {
            int new_block_index = -1;
            for (int j = FREE_BLOCKS_START_INDEX; j < NUMBER_BLOCKS; j++) {
                if (free_bitmap[j] == '\0') {
                    free_bitmap[j] = 1;
                    new_block_index = j;
                    break;
                }
            }
            // not enough blocks free for data
            if (new_block_index < 0) {
                printf("no free blocks to allocate for data \n");
                return -1;
            }
            // write block index to inode
            if (i < 12) {   // direct pointer
                inode_table[inode_index].direct[i] = new_block_index;
            } else {        // indirect pointer
                indirect_pointers[i-12] = new_block_index;
            }
        }
    }

    char *data_buffer = malloc(BLOCK_SIZE*(future_last_block_index+1));         // data buffer to store file data

    // read data from disk 
    for (int i = start_block; i < inode_table[inode_index].link_cnt; i++) {
        if (i < 12) {   // read data from direct pointer
            read_blocks(inode_table[inode_index].direct[i], 1, data_buffer+BLOCK_SIZE*(i-start_block));
        } else {        // read data from indirect pointer
            read_blocks(indirect_pointers[i-12], 1, data_buffer+BLOCK_SIZE*(i-start_block));
        }
    }

    // copy data from given data buffer to file data buffer starting at the start offset
    memcpy(data_buffer+start_block_offset, buf, length);

    // flush data to disk
    for (int i = start_block; i <= future_last_block_index; i++) {
        if (i < 12) {   // write data to direct pointer
            write_blocks(inode_table[inode_index].direct[i], 1, data_buffer+BLOCK_SIZE*(i-start_block));
        } else {        // write data to indirect pointer
            write_blocks(indirect_pointers[i-12], 1, data_buffer+BLOCK_SIZE*(i-start_block));
        }
    }

    // free allocated memory and update inode and fdt
    free(data_buffer);
    file_descriptor_table[fileID].rwpointer += length;
    inode_table[inode_index].link_cnt = future_last_block_index+1;

    // if we extended the file size, update the file size field in the inode
    if (inode_table[inode_index].size < file_descriptor_table[fileID].rwpointer) {
        inode_table[inode_index].size = file_descriptor_table[fileID].rwpointer;
    }

    // if data is addressed by indirect pointers, write pointer indexes to indirect pointer block
    if (inode_table[inode_index].link_cnt > 12) {
        write_blocks(inode_table[inode_index].indirect, 1, indirect_pointers);
    }

    // write inode table to disk
    if (write_blocks(1, 16, &inode_table) == -1) {
        printf("failed to write inodes \n");
        return -1;
    }

    // write free bitmap to disk
    if (write_blocks(17, 2, &free_bitmap) == -1) {
        printf("failed to write bitmap \n");
        return -1;
    }

    //printf("wrote %d bytes to file, total size: %d bytes, rwpointer: %d \n", length, inode_table[inode_index].size, file_descriptor_table[fileID].rwpointer);
    return length;
}

// reads length bytes from file specified by the fdt index fileID to data buffer buf
int sfs_fread(int fileID, char *buf, int length) {
    // check for valid fdt index and parameters
    if (fileID < 0 || fileID >= INODE_TABLE_SIZE || length < 0) {
        printf("invalid file id or length \n");
        return -1;
    }

    // check if an inode is associated with the fdt entry
    if (file_descriptor_table[fileID].inode < 0) {
        printf("no inode associated with fdt entry \n");
        return -1;
    }

    // check if the read would exceed the max file size
    if (file_descriptor_table[fileID].rwpointer + length > MAXFILESIZE) {
        printf("read exceeds max file size \n");
        return -1;
    }

    int inode_index = file_descriptor_table[fileID].inode;                              // index of inode in inode table
    int start_block = file_descriptor_table[fileID].rwpointer / BLOCK_SIZE;             // index of start block
    int start_block_offset = file_descriptor_table[fileID].rwpointer % BLOCK_SIZE;      // location to start reading from in the first block to read from
    int num_bytes_read = length;                                                        // number of bytes read to return

    // if total read exceeds size, reduce amount to read to the non-exceeding amount
    if (file_descriptor_table[fileID].rwpointer + length > inode_table[inode_index].size) {
        num_bytes_read = inode_table[inode_index].size - file_descriptor_table[fileID].rwpointer;
    }

    // if the file has data addressed by indirect data blocks, load the block indexes into memory
    int indirect_pointers[BLOCK_SIZE/sizeof(int)];
    if (inode_table[inode_index].link_cnt > 12) {
        read_blocks(inode_table[inode_index].indirect, 1, indirect_pointers);
    } 

    char *data_buffer = malloc(BLOCK_SIZE*inode_table[inode_index].link_cnt);           // data buffer to store file data

    // load file data from start block to end of file into data buffer
    for (int i = start_block; i < inode_table[inode_index].link_cnt; i++) {
        if (i < 12) {           // read data from direct pointer
            read_blocks(inode_table[inode_index].direct[i], 1, data_buffer+BLOCK_SIZE*(i-start_block));
        } else {                // read data from indirect pointer
            read_blocks(indirect_pointers[i-12], 1, data_buffer+BLOCK_SIZE*(i-start_block));
        }
    }

    // finished reading, copy data from file data buffer to buffer provided to the function
    memcpy(buf, data_buffer+start_block_offset, num_bytes_read);
    free(data_buffer);
    file_descriptor_table[fileID].rwpointer += num_bytes_read;      // increment rw pointer by amount read
    //printf("read %d bytes to file, total size: %d bytes, rwpointer: %d \n", num_bytes_read, inode_table[inode_index].size, file_descriptor_table[fileID].rwpointer);
    return num_bytes_read;
}

// moves the read/write pointer of the file specified by the given fdt index to the specified position
int sfs_fseek(int fileID, int loc) {
    // check if fdt index is valid
    if (fileID < 0 || fileID >= INODE_TABLE_SIZE || file_descriptor_table[fileID].inode < 0) {
        printf("invalid file id \n");
        return -1;
    }

    // set rwpointer
    file_descriptor_table[fileID].rwpointer = loc;

    if (file_descriptor_table[fileID].rwpointer == loc) {
        return 0;
    } else {
        return -1;
    }
}

// removes file from the directory
int sfs_remove(char *file) {
    // inode and directory indexes
    int inode_location = -1;
    //int dirloc = -1;      // debug

    // search for file in directory
    for (int i = 0; i < DIRECTORY_SIZE; i++) {
        if (strcmp(directory[i].filename, file) == 0) { // file found
            //dirloc = i;       // debug
            inode_location = directory[i].inode;
            // reset directory fields
            directory[i].inode = -1;
            memset(directory[i].filename, 0, MAXFILENAME+1);
            //strcpy(directory[i].filename, "\0");
            // for (int j = 0; j < MAXFILENAME; j++) {
            //     directory[i].filename[j] = '\0';
            // }
            break;
        }
    }

    // if file has an associated inode, release it and the data blocks it contains
    if (inode_location >= 0) {
        block *empty = malloc(sizeof(block));           // empty block to write to disk to free disk blocks
        memset(empty, 0, sizeof(block));                // see if this works
        
        // erase direct blocks by writing the empty block to them
        for (int i = 0; i < 12; i++) {
            if (inode_table[inode_location].direct[i] != -1) {
                free_bitmap[inode_table[inode_location].direct[i]] = '\0';           // set bitmap index to unused
                write_blocks(inode_table[inode_location].direct[i], 1, empty);      // write empty block
                inode_table[inode_location].direct[i] = -1;                         // set direct index to -1
            }
        }

        // erase indirect blocks by writing the empty block to blocks addressed by indirect pointers
        if (inode_table[inode_location].link_cnt > 12) {
            int indirect_pointers[BLOCK_SIZE/sizeof(int)];
            read_blocks(inode_table[inode_location].indirect, 1, indirect_pointers);

            for (int i = 0; i < inode_table[inode_location].link_cnt-12; i++) {
                free_bitmap[indirect_pointers[i]] = '\0';                        // set bitmap index of data block to unused
                write_blocks(indirect_pointers[i], 1, empty);                   // write empty block
            }
            free_bitmap[inode_table[inode_location].indirect] = '\0';            // set bitmap index of indirect pointer block to unused
            write_blocks(inode_table[inode_location].indirect, 1, empty);       // write empty block
        }

        // reset inode fields to the unused state values
        inode_table[inode_location].indirect = -1;
        inode_table[inode_location].mode = -1;
        inode_table[inode_location].link_cnt = 0;
        inode_table[inode_location].size = 0;
        free(empty);

        sblock->inode_table_length--;       // decrement inode table length field

        // write superblock to disk
        if (write_blocks(0, 1, sblock) == -1) {
            printf("failed to write superblock \n");
            return -1;
        }

        // write inode table to disk
        if (write_blocks(1, 16, &inode_table) == -1) {
            printf("failed to write inodes \n");
            return -1;
        }

        // write bitmap to disk
        if (write_blocks(17, 2, &free_bitmap) == -1) {
            printf("failed to write bitmap \n");
            return -1;
        }

        // write directory to disk
        if (write_blocks(19, 16, &directory) == -1) {
            printf("failed to write directories \n");
            return -1;
        }

        //printf("removed file, %d files remain, file dirloc: %d, inode index: %d \n", sblock->inode_table_length, dirloc, inode_location);
        return 0;

    } else {
        printf("file does not exist \n");
        return -1;
    }
}


/* Thank you for reading through my assignment! Please enjoy this ascii art for your time

⠀ ⠘⡀⠀why yes, i graded this ecse ⡜⠀⠀⠀
⠀⠀⠀⠑⡀⠀  427 assignment, how   ⡔⠁⠀⠀⠀
⠀⠀⠀⠀⠈⠢⢄⠀ could you tell?  ⣀⠴⠊⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠀⢀⣀⣀⣀⣀⣀⡀⠤⠄⠒⠈⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠘⣀⠄⠊⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀
⣿⣿⣿⣿⣿⣿⣿⣿⡿⠿⠛⠛⠛⠋⠉⠈⠉⠉⠉⠉⠛⠻⢿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⡿⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠛⢿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⡏⣀⠀⠀⠀⠀⠀⠀⠀⣀⣤⣤⣤⣄⡀⠀⠀⠀⠀⠀⠀⠀⠙⢿⣿⣿⣿⣿
⣿⣿⣿⢏⣴⣿⣷⠀⠀⠀⠀⠀⢾⣿⣿⣿⣿⣿⣿⡆⠀⠀⠀⠀⠀⠀⠀⠈⣿⣿⣿⣿
⣿⣿⣟⣾⣿⡟⠁⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⣿⣿⣷⢢⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿
⣿⣿⣿⣿⣟⠀⡴⠄⠀⠀⠀⠀⠀⠀⠙⠻⣿⣿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿
⣿⣿⣿⠟⠻⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠶⢴⣿⣿⣿⣿⣿⣧⠀⠀⠀⠀⠀⠀⣿⣿⣿
⣿⣁⡀⠀⠀⢰⢠⣦⠀⠀⠀⠀⠀⠀⠀⠀⢀⣼⣿⣿⣿⣿⣿⡄⠀⣴⣶⣿⡄⣿⣿⣿
⣿⡋⠀⠀⠀⠎⢸⣿⡆⠀⠀⠀⠀⠀⠀⣴⣿⣿⣿⣿⣿⣿⣿⠗⢘⣿⣟⠛⠿⣼⣿
⣿⣿⠋⢀⡌⢰⣿⡿⢿⡀⠀⠀⠀⠀⠀⠙⠿⣿⣿⣿⣿⣿⡇⠀⢸⣿⣿⣧⢀⣼⣿
⣿⣿⣷⢻⠄⠘⠛⠋⠛⠃⠀⠀⠀⠀⠀⢿⣧⠈⠉⠙⠛⠋⠀⠀⠀⣿⣿⣿⣿⣿⣿
⣿⣿⣧⠀⠈⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠟⠀⠀⠀⠀⢀⢃⠀⠀⢸⣿⣿⣿⣿⣿⣿
⣿⣿⡿⠀⠴⢗⣠⣤⣴⡶⠶⠖⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⡸⠀⣿⣿⣿⣿⣿⣿
⣿⣿⣿⡀⢠⣾⣿⠏⠀⠠⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠛⠉⠀⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣧⠈⢹⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⡄⠈⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣴⣾⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣧⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣷⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣦⣄⣀⣀⣀⣀⠀⠀⠀⠀⠘⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡄⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⠀⠀⠙⣿⣿⡟⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠇⠀⠁⠀⠀⠹⣿⠃⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⡿⠛⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⢐⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⠿⠛⠉⠉⠁⠀⢻⣿⡇⠀⠀⠀⠀⠀⠀⢀⠈⣿⣿⡿⠉⠛⠛⠛⠉⠉⣿
⣿⡿⠋⠁⠀⠀⢀⣀⣠⡴⣸⣿⣇⡄⠀⠀⠀⠀⢀⡿⠄⠙⠛⠀⣀⣠⣤⣤⠄⠀⣿

^^ you cuz you're awesome :D

*/