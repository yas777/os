#include "emufs.h"

/**************************** File system helper functions ****************************/

//	Function to encrypt a block of data 
int encrypt(char* input, char* encrypted)
{
	for(int i=0; i<BLOCKSIZE; i++)
	{
		encrypted[i] = ~input[i];
	}
}

//	Function to decrypt a block of data 
int decrypt(char* input, char* decrypted)
{
	for(int i=0; i<BLOCKSIZE; i++)
	{
		decrypted[i] = ~input[i];
	}
}

//	The following helper functions are used to read and write 
//	superblock and metadata block. 
//	Change the function definitions with required arguments
struct superblock_t* readSuperblock(int dev_fd)
{
	/*
		* Read 0th block from the device into a BLOCKSIZE buffer
		* Create superblock_t variable and fill it using reader buffer
		* Return the superblock_t variable
	*/
	struct superblock_t* su_block = (struct superblock_t*)malloc(sizeof(struct superblock_t));

	char buffer[BLOCKSIZE];
	
	int rt = readblock(dev_fd,0,buffer);

	if(rt == -1) {
		printf("Error during reading super block\n");
		// return -1;
	}

	memcpy(su_block, buffer, sizeof(struct superblock_t));

	return su_block;
}

int writeSuperblock(int dev_fd, struct superblock_t* su_block)
{
	/*
		* Read the 0th block from device into a buffer
		* Write the superblock into the buffer
		* Write back the buffer into block 0
	*/
	char buffer[BLOCKSIZE];

	memcpy(buffer, su_block, sizeof(struct superblock_t));

	int rt = writeblock(dev_fd, 0, buffer);

	if(rt == -1){
		printf("Error during writing the super block\n");
		return -1;
	}

	return 1;
}

struct metadata_t* readMetadata(int dev_fd, int enc)
{
	// Same as readSuperBlock(), but it is stored on block 1
	// Need to decrypt if emufs-encrypted is used  
	struct metadata_t* me_block = (struct metadata_t*)malloc(sizeof(struct metadata_t));

	char buffer[BLOCKSIZE];

	int rt = readblock(dev_fd, 1, buffer);
	if (rt == -1){
		printf("Error during reading meatada \n");
		// return -1;
	}
	else if (enc == 0){
		memcpy(me_block, buffer, sizeof(struct metadata_t));
	}
	else{
		char dec_buffer[BLOCKSIZE];
		rt = decrypt(buffer,dec_buffer);
		memcpy(me_block, dec_buffer, sizeof(struct metadata_t));
	}
	return me_block;
}

int writeMetadata(int dev_fd, int enc, struct metadata_t* me_block)
{
	// Same as writeSuperblock(), but it is stored on block 1
	// Need to decrypt/encrypt if emufs-encrypted is used  
	char enc_buffer[BLOCKSIZE],buffer[BLOCKSIZE];

	memcpy(buffer, me_block, sizeof(struct metadata_t));
	int rt;
	if (enc == 1){
		rt = encrypt(buffer, enc_buffer);
		rt = writeblock(dev_fd, 1, enc_buffer);
	}
	else{
		rt = writeblock(dev_fd, 1, buffer);
	}
	if (rt == -1){
		printf("Error while writing metadata\n");
		return -1;
	}
	return 1;
}

int free_block(int dev_fd)
{
	struct superblock_t* su_block = readSuperblock(dev_fd);
	for (int i=2;i<MAX_BLOCKS;i++)
	{
		if (su_block->bitmap[i] == '0') {
			su_block->bitmap[i] = '1';
			writeSuperblock(dev_fd, su_block);
			return i;
		}
	}
	return -1;
}

/**************************** File system API ****************************/

int create_file_system(struct mount_t *mount_point, int fs_number)
{
	/*
	   	* Read the superblock.
	    * Set file system number on superblock
		* Clear the bitmaps.  values on the bitmap will be either '0', or '1', or'x'. 
		* Create metadata block in disk
		* Write superblock and metadata block back to disk.

		* Return value: -1,		error
						 1, 	success
	*/
	struct superblock_t* su_block = readSuperblock(mount_point->device_fd);

	su_block->fs_number = fs_number;
	mount_point->fs_number = fs_number;
	// printf("BItmap values -- ");
	for (int i=0;i<MAX_BLOCKS;i++){
		if (i == 0 || i == 1){
			su_block->bitmap[i] = '1';	
		}
		else if(i >= su_block->disk_size) {
			su_block->bitmap[i] = 'x';
		}
		else{
			su_block->bitmap[i] = '0';
		}
		// printf("%c ",su_block->bitmap[i]);
	}
	// printf("\n");

	int rt2 = writeSuperblock(mount_point->device_fd, su_block);
	if (rt2 == -1) return -1;
	struct metadata_t* me_block = (struct metadata_t*)malloc(sizeof(struct metadata_t));

	for(int i=0;i<MAX_FILES;i++){
		me_block->inodes[i].status = 0;
		for(int j=0;j<4;j++) me_block->inodes[i].blocks[j] = -1;
		me_block->inodes[i].file_size=0;
 	}

	int rt1 = writeMetadata(mount_point->device_fd, fs_number, me_block);
	if(rt1 == -1) return -1;
	printf("[%s] File system created. \n", mount_point->device_name);
	return 1;
}


struct file_t* eopen(struct mount_t* mount_point, char* filename)
{
	/* 
		* If file exist, get the inode number. inode number is the index of inode in the metadata.
		* If file does not exist, 
			* find free inode.
			* allocate the free inode as USED
			* if free id not found, print the error and return -1
		* Create the file hander (struct file_t)
		* Initialize offset in the file hander
		* Return file handler.

		* Return NULL on error.
	*/

	struct metadata_t* me_block = (struct metadata_t*)malloc(sizeof(struct metadata_t));
	struct superblock_t* su_block = (struct superblock_t*)malloc(sizeof(struct superblock_t));

	su_block = readSuperblock(mount_point->device_fd);
	me_block = readMetadata(mount_point->device_fd, mount_point->fs_number);

	int flag = 0,flag1=0,in=0,rt;
	struct file_t* file = (struct file_t*)malloc(sizeof(struct file_t));
	for (int i=0;i<MAX_FILES;i++){
		if (me_block->inodes[i].status == 1){
			if(!strcmp(filename, me_block->inodes[i].name)) {
				file->offset = 0;
				file->inode_number = i;
				file->mount_point = mount_point;
				flag = 1;
			}
		}
	}
	if (flag == 0) {
		for (int i=0;i<MAX_FILES;i++){
			if(me_block->inodes[i].status == 0) {
				flag = 1;
				in = i;
				break;
			}
		}
		if (flag == 0){
			printf("No free inodes\n");
			return NULL;
		}
		else{
			file->offset = 0;
			file->inode_number = in;
			file->mount_point = mount_point;
			me_block->inodes[in].status = 1;
			strcpy(me_block->inodes[in].name, filename);
			me_block->inodes[in].file_size = 0;
			time(&(me_block->inodes[in].modtime));
			rt = writeMetadata(mount_point->device_fd, mount_point->fs_number, me_block);
		}
	}
	printf("File '%s' Opened \n",filename);
	return file;
}

int ewrite(struct file_t* file, char* data, int size)
{
	// You do not need to implement partial writes in case file exceeds 4 blocks
	// or no free block is available in the disk. 

	// Return value: 	-1,  error
	//					Number of bytes written
	char buffer[BLOCKSIZE];
	char enc_buffer[BLOCKSIZE];

	int d_fd = file->mount_point->device_fd;
	int fs_num = file->mount_point->fs_number;
	int in_num = file->inode_number;

	struct metadata_t* me_block = (struct metadata_t*)malloc(sizeof(struct metadata_t));
	struct superblock_t* su_block = readSuperblock(d_fd);

	me_block = readMetadata(d_fd, fs_num);

	int fl_off = file->offset;
	int flsize = me_block->inodes[in_num].file_size;
	int min_blocknum = fl_off/BLOCKSIZE;
	int max_blocknum = (fl_off + size)/BLOCKSIZE;

	if(fl_off > flsize) {
		printf("ERROR: Invalid offset\n");
		return -1;
	}
	if(fl_off + size > (4*BLOCKSIZE)) {
		printf("ERROR: Maximum File Size Exceeded\n");
		return -1;
	}

	int enc = file->mount_point->fs_number;
	int* fr_blk = malloc((max_blocknum - min_blocknum)*sizeof(int));
	int rt,c=0;
	for (int i=min_blocknum;i<max_blocknum;i++){
		if(me_block->inodes[in_num].blocks[i] == -1){
			int temp = free_block(d_fd);
			fr_blk[i] = temp;
			if(fr_blk[i] == -1) {
				for (int j=min_blocknum;j<=i;j++) su_block->bitmap[j] = '0';
				writeSuperblock(d_fd,su_block);
				printf("ERROR: Out of diskspace\n");
				return -1;
			}
		}
	}
	for (int i=min_blocknum;i<max_blocknum;i++){
		memcpy(buffer, data+(c*BLOCKSIZE), BLOCKSIZE);

		if(enc == 1) rt = encrypt(buffer, enc_buffer);
		else memcpy(enc_buffer, buffer, BLOCKSIZE);		

		if(me_block->inodes[in_num].blocks[i] == -1){
			// su_block->bitmap[fr_blk[i]] = '1';
			rt = writeblock(d_fd, fr_blk[i], enc_buffer);
			me_block->inodes[in_num].blocks[i] = fr_blk[i];
			me_block->inodes[in_num].file_size+=BLOCKSIZE;
			rt = writeMetadata(d_fd, fs_num, me_block);
			// printf("Entered block number %d %d %d\n",i, me_block->inodes[in_num].blocks[i],me_block->inodes[in_num].file_size);
		}
		else{

			rt = writeblock(d_fd, me_block->inodes[in_num].blocks[i], enc_buffer);
		}
		c++;
		file->offset+=BLOCKSIZE;
	}
	// writeSuperblock(d_fd, su_block);
	time(&(me_block->inodes[in_num].modtime));
	return size;
}

int eread(struct file_t* file, char* data, int size)
{
	// NO partial READS.

	// Return value: 	-1,  error
	//					Number of bytes read

	char buffer[BLOCKSIZE];
	char dec_buffer[BLOCKSIZE];

	int d_fd = file->mount_point->device_fd;
	int fs_num = file->mount_point->fs_number;

	struct metadata_t* me_block = (struct metadata_t*)malloc(sizeof(struct metadata_t));
	me_block = readMetadata(d_fd,fs_num);

	// printf("File size --- %d \n", me_block->inodes[file->inode_number].file_size);

	int in_num = file->inode_number;
	int fl_off = file->offset;
	int flsize = me_block->inodes[in_num].file_size;

	int min_blocknum = fl_off/BLOCKSIZE;
	int max_blocknum = (fl_off + size)/BLOCKSIZE;

	if(fl_off >= flsize) {
		printf("ERROR: Invalid offset\n");
		return -1;
	}
	if(fl_off + size > flsize) {
		printf("ERROR: Maximum file size Exceeded\n");
		return -1;
	}


	int rt,c=0;
	for(int i=min_blocknum;i<max_blocknum;i++){
		rt = readblock(d_fd, me_block->inodes[in_num].blocks[i], buffer);
		if (fs_num == 1) encrypt(buffer,dec_buffer);
		else memcpy(dec_buffer, buffer, BLOCKSIZE);
		memcpy(data + (c*BLOCKSIZE), dec_buffer, BLOCKSIZE);
		c++;
		file->offset+=BLOCKSIZE;
	}
	return size;
	// struct metadata* me_block = (struct metadata_t*)malloc(sizeof(struct metadata_t));

	// me_block = readMetadata(file->mount_point->device_fd, file->mount_point->fs_number);

	// int buffer[BLOCKSIZE];
	// int dec_buffer[BLOCKSIZE];
	// int rt;

	// int flsize = me_block[file->inode_number].file_size;
	// int fl_off = me_block[file->inode_number].offset;
	// int d_fd = file->mount_point->device_fd;
	// int f_enc = file->mount_point->fs_number;

	// if(fl_off + size > flsize) return -1;

	// int min_blocknum = fl_off/BLOCKSIZE;
	// int max_blocknum = (fl_off+size)/BLOCKSIZE +1;
	// int blockoff = fl_off%BLOCKSIZE;

	// int cpy_off = blockoff;
	// int rescpy_off = 0;
	// int rescpy_size = 0;
	// int cpy_size = size;
	// if (max_blocknum = min_blocknum + 1) rescpy_size = size - fl_off;
	// else rescpy_size = BLOCKSIZE - offset;

	// char* buffer[BLOCKSIZE];
	// for (int i= min_blocknum;i<=max_blocknum;i++) {
	// 	rt = readblock(d_fd, blocknum, buffer);
	// 	if(f_enc == 1) {
	// 		rt = decrypt(buffer,dec_buffer);
	// 	}
	// 	else{
	// 		memcpy(dec_buffer,buffer,BLOCKSIZE);
	// 	}
	// 	memcpy(data + rescpy_off, dec_buffer + cpy_off, rescpy_size);
	// 	cpy_off = 0;
	// 	rescpy_off+=rescpy_size;
	// 	if (max_blocknum = i+1) rescpy_size = cpy_size;
	// 	else rescpy_size = BLOCKSIZE;
	// 	cpy_size-=rescpy_size;
	// }
	// return size;

}

void eclose(struct file_t* file)
{
	// free the memory allocated for the file handler 'file'
	free(file);
	printf("File Closed \n");
}

int eseek(struct file_t *file, int offset)
{
	// Change the offset in file hanlder 'file'
	file->offset = offset;
	return 1;
}

void fsdump(struct mount_t* mount_point)
{

	printf("\n[%s] fsdump \n", mount_point->device_name);
	printf("%-10s %6s \t[%s] \t%s\n", "  NAME", "SIZE", "BLOCKS", "LAST MODIFIED");
	struct metadata_t* me_block = readMetadata(mount_point->device_fd, mount_point->fs_number);
	for (int i=0;i<MAX_FILES;i++){
		if(me_block->inodes[i].status == 1) {

		
			printf("%-10s %6d \t[%d %d %d %d] \t%s", 
				me_block->inodes[i].name, 
				me_block->inodes[i].file_size,
				me_block->inodes[i].blocks[0],
				me_block->inodes[i].blocks[1],
				me_block->inodes[i].blocks[2],
				me_block->inodes[i].blocks[3],
				asctime(localtime(&(me_block->inodes[i].modtime))));
		}
	}
}
