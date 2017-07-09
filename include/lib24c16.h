
#ifndef LIB24C16_HEADER_FILE
#define LIB24C16_HEADER_FILE




/*
	Check if we have a valid block number
	The 24C16 has 8 blocks of 256 Bytes each
	and 16 pages per block of 16 bytes each
*/
int check_block_number(int block_number);

/*
	Check if the address number within the current
	page, witin the current block is ok. Any number
	greater than 255 (0xff) is invalid
*/
int check_address_number(int address_number);

/*
    Check, if data of a specific lenght,
    can be written to the current block.
*/
int veryfy_block_space(int start_address, int lenght);

/*
	Write a single byte to an address within a specific block and a
	specific page witin that block.
*/
int write_byte(char* bus_number,int block_number,char* address, int value);

/*
	Write an array of bytes, starting from a specific address within a
	specific page, within a specific block
*/
int write_byte_array(char* bus_number,int block_number, char* address, int bytes_to_write[], int length);

/*
	Read a byte from a specific page within a specific block.
*/
int read_byte(char* bus_number,int block_number, char* address);

/*
	Read a specific amount of bytes, starting from a specific address within
	a specific page, within a specific block
*/
int* read_bytes(char* bus_number,int block_number, char*  address, int length);

/*
	Clear the entire 8 blocks, with each 256 bytes
	at once fill each byte with 0xff
*/
int clear_eeprom(char* bus_number);

#endif
