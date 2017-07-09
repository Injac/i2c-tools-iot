/*
    i2cset.c - A user-space program to write an I2C register.
    Copyright (C) 2001-2003  Frodo Looijaard <frodol@dds.nl>, and
                             Mark D. Studebaker <mdsxyz123@yahoo.com>
    Copyright (C) 2004-2010  Jean Delvare <khali@linux-fr.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA 02110-1301 USA.
*/



#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>	/* for NAME_MAX */
#include <string.h>
#include <strings.h>	/* for strcasecmp() */
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include "i2cbusses.h"
#include "util.h"
#include "../version.h"
#include "../include/lib24c16.h"
#include <unistd.h>

int force = 0;

/*
	Check if we have a valid block number
	The 24C16 has 8 blocks of 256 Bytes each
	and 16 pages per block of 16 bytes each
*/
int check_block_number(int block_number)
{
	if (block_number > 0x57)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/*
	Check if the address number within the current
	page, witin the current block is ok. Any number
	greater than 255 (0xff) is invalid
*/
int check_address_number(int address_number)
{
	if (address_number > 0xff)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/*
    Check, if data of a specific lenght,
    can be written to the current block.
*/
int veryfy_block_space(int start_address, int lenght)
{
	int capacity_left = 0xff - start_address;

	int can_be_done = capacity_left - lenght;

	if (can_be_done <= -1)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

/*
	Write a single byte to an address within a specific block and a
	specific page witin that block.
*/
int write_byte(char* bus_number, int block_number, char* address, int value)
{
	int res, i2cbus, file,set_slave_ok,daddress;
	char filename[20];

	//printf("START!\n");

	//printf("WRITE BYTE I2c name %s \n",bus_number);

	if (!check_block_number(block_number) && !check_address_number(daddress))
	{
		return -1;
	}

	//printf("AFTER CHECK!\n");

	//Get the i2cbus
	i2cbus = lookup_i2c_bus(bus_number);

	//printf("AFTER LOOKUP BUS! %d \n",i2cbus);

	if (i2cbus < 0)
	{
		return -1;
	}

	//Parse data address
	daddress = (int)*address;

	//printf("AFTER PARSE i2c address!\n");

	//printf("DADDRESS: %d \n",daddress);

	if (address < 0)
	{
		return -1;
	}

	//Open the device
	file = open_i2c_dev(i2cbus, filename, sizeof(filename), 0);

	//printf("FILE: %d \n", file);

	if (file < 0)
	{
		return -1;
	}

	set_slave_ok = set_slave_addr(file, block_number, force);

	if(set_slave_ok < 0)
	{
		return -1;
	}

	//Write one byte to the specific block, within the specific page
	res = i2c_smbus_write_byte_data(file, daddress, value);

	//On error, close the device and exit
	if (res < 0)
	{
		close(file);
		return -1;
	}
	else
	{
		close(file);
		return 1;
	}
}

/*
	Write an array of bytes, starting from a specific address within a
	specific page, within a specific block
*/
int write_byte_array(char* bus_number,int block_number, char* address, int bytes_to_write[], int length)
{

	if (!check_block_number(block_number) && !check_address_number(*address))
	{
		return -1;
	}

	if(veryfy_block_space((int)*address, length) < 0)
	{
		return -1;
	}

	int res, i2cbus,file,daddress,set_slave_ok;
	char filename[20];

	//Get the i2cbus
	i2cbus = lookup_i2c_bus(bus_number);

	if (i2cbus < 0)
	{
		return -1;
	}

	//Parse block address
	daddress = (int)*address;

	if (address < 0)
	{
		return -1;
	}

	//Open the device
	file = open_i2c_dev(i2cbus, filename, sizeof(filename), 0);

	if (file < 0)
	{
		return -1;
	}

	set_slave_ok = set_slave_addr(file, block_number, force);

	if(set_slave_ok < 0)
	{
		return -1;
	}

	int counter;
	int max = length - 1;

	for (counter = 0; counter <= max;counter++)
	{
		//Write one byte to the specific block, within the specific page
		res = i2c_smbus_write_byte_data(file, daddress, bytes_to_write[counter]);


        //sleep, otherwhise the value will be garbled
        usleep(3000);

		if (res < 0)
		{
			close(file);
			return -1;
		}

		//increase the address to write to
		daddress += 1;
	}

	close(file);
	return 1;
}

/*
	Read a byte from a specific page within a specific block.
*/
int read_byte(char* bus_number,int block_number, char* address)
{
	int res, i2cbus, file,daddress,set_slave_ok;
	char filename[20];

	if (!check_block_number(block_number) && !check_address_number((int)*address))
	{
		return -1;
	}

	//Get the i2cbus
	i2cbus = lookup_i2c_bus(bus_number);

	if (i2cbus < 0)
	{
		return -1;
	}

	//Parse block address
	daddress = (int)*address;

	if (address < 0)
	{
		return -1;
	}

	//Open the device
	file = open_i2c_dev(i2cbus, filename, sizeof(filename), 0);

	if (file < 0)
	{
		return -1;
	}

	set_slave_ok = set_slave_addr(file, block_number, force);

	if(set_slave_ok < 0)
	{
		return -1;
	}

	//Write one byte to the specific block, within the specific page
	res = i2c_smbus_read_byte_data(file, daddress);

	//On error, close the device and exit
	if (res < 0)
	{
		close(file);
		return -1;
	}
	else
	{
		close(file);
		return res;
	}
}

/*
	Read a specific amount of bytes, starting from a specific address within
	a specific page, within a specific block
*/
int *read_bytes(char* bus_number,int block_number, char* address, int length)
{
	//Some new stuff added
	if (!check_block_number(block_number) && !check_address_number((int)*address))
	{
		return NULL;
	}

	if (veryfy_block_space((int)*address, length) < 0)
	{
		return NULL;
	}

	int res, i2cbus, file,daddress,set_slave_ok;
	char filename[20];
	int*  read_bytes = (int* ) malloc(sizeof(int) * length);

	//Get the i2cbus
	i2cbus = lookup_i2c_bus(bus_number);

	if (i2cbus < 0)
	{
		return NULL;
	}

	//Parse data address
	daddress = (int)*address;

	if (address < 0)
	{
		return NULL;
	}

	//Open the device
	file = open_i2c_dev(i2cbus, filename, sizeof(filename), 0);

	if (file < 0)
	{
		return NULL;
	}

	set_slave_ok = set_slave_addr(file, block_number, force);

	if(set_slave_ok < 0)
	{
		return NULL;
	}

	int counter;
	int max = length;

	for (counter = 0; counter < max;counter++)
	{
		//Write one byte to the specific block, within the specific page
		res = i2c_smbus_read_byte_data(file, daddress);

        usleep(3000);
		if (res < 0)
		{
			close(file);
			return NULL;
		}

		//increase the address to write to
		daddress += 1;
		read_bytes[counter] = res;
	}

	close(file);

	return read_bytes;
}

/*
	Clear the entire 8 blocks, with each 256 bytes
	at once fill each byte with 0xff
*/
int clear_eeprom(char* bus_number)
{

	/*int res, i2cbus, address, file, set_slave_ok;

	char filename[20];

	//Get the i2cbus
	i2cbus = lookup_i2c_bus(bus_number);

	if (i2cbus < 0)
	{
		return -1;
	}


	//Open the device
	file = open_i2c_dev(i2cbus, filename, sizeof(filename), 0);

	if (file < 0)
	{
		return -1;
	}

    set_slave_ok = set_slave_addr(file, 0x50, force);

	if(set_slave_ok < 0)
	{
		return -1;
	}

	//Available blocks of the 32c16 eeprom
	int block_addresses[] = {0x51,0x52,0x53,0x54,0x55,0x56,0x57};


	int counter;
	int block_counter;
	int number_of_blocks = sizeof(block_addresses) / sizeof(block_addresses[0]);
	int max_addresses_per_block = 255;

	for (counter = 0; counter < number_of_blocks; counter++)
	{

		for (block_counter = 0; block_counter <= max_addresses_per_block;block_counter++)
		{
			//Write one byte to the specific block, within the specific page
			res = i2c_smbus_write_byte_data(file,block_counter, 0xff);

            usleep(3000);

			if (res < 0)
			{
				close(file);
				return -1;
			}
		}

        set_slave_ok = set_slave_addr(file, block_addresses[counter], force);

	    if(set_slave_ok < 0)
	    {
		    return -1;
	    }

	}

	close(file);
    */
	return 1;
}
