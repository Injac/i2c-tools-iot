I2C TOOLS FOR LINUX
===================

This package contains an heterogeneous set of I2C tools for the Linux kernel.
These tools were originally part of the lm-sensors project but were finally
split into their own package for convenience. They compile, run and have been
tested on GNU/Linux.


CONTENTS
--------

The various tools included in this package are grouped by category, each
category has its own sub-directory:

* eeprom
  Perl scripts for decoding different types of EEPROMs (SPD, EDID...) These
  scripts rely on the "eeprom" kernel driver. They are installed by default.

* eepromer
  Tools for writing to EEPROMs. These tools rely on the "i2c-dev" kernel
  driver. They are not installed by default.

* include
  C/C++ header files for I2C and SMBus access over i2c-dev. Installed by
  default.

* py-smbus
  Python wrapper for SMBus access over i2c-dev. Not installed by default.

* stub
  A helper script to use with the i2c-stub kernel driver. Installed by
  default.

* tools
  I2C device detection and register dump tools. These tools rely on the
  "i2c-dev" kernel driver. They are installed by default.


INSTALLATION
------------

There's no configure script, so simply run "make" to build the tools, and
"make install" to install them. You also can use "make uninstall" to remove
all the files you installed. By default, files are installed in /usr/local
but you can change this behavior by editing the Makefile file and setting
prefix to wherever you want. You may change the C compiler and the
compilation flags as well.

Optionally, you can run "make strip" prior to "make install" if you want
smaller binaries. However, be aware that this will prevent any further
attempt to debug the programs.

If you wish to include sub-directories that are not enabled by default, then
just set them via the EXTRA make variable. For example, to build py-smbus,
do:
  $ make EXTRA="py-smbus"


INSTALLATION OF LIB24C16
------------------------

To Compile and use the library, simply cd into the folder *tools* and do the following:
You will have to have a working cmake installed on your machine.

* mkdir build
* cmake ..
* make
* sudo make install
* Add /usr/local/lib to your /etc/ld.so.conf file
* sudo ldconfig

Here is a sample on how to use the library:


```C

#include <limits.h>
#include <dirent.h>
#include <fcntl.h>

#include <stdio.h>
#include <unistd.h>


#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "util.h"

#include "lib24c16.h"


int main()
{
    	char bus = '1';
    	char address = 0x00;
	char address_write_bytes = 0x00;
    	int valuemy = 0;    

	int values_written[10] = {0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x29,0x30,0x31};

    	int* values_read;
    


    	write_byte(&bus,0x50,&address,0x21);
   
    	usleep(3000);

    	valuemy = read_byte(&bus,0x50,&address);

    	usleep(3000);

	printf("0x%x\n",  valuemy); 
    
    	printf("%d \n",valuemy);


	printf("Writing byte array, this will take some time\n");

	int written = write_byte_array(&bus,0x50,&address_write_bytes,values_written,10);
	

	if(written > 0)
	{
		printf("Bytes successfully written!\n");
	}
 	else
	{
		printf("No bytes written\n");
	}


	usleep(3000);

	values_read = read_bytes(&bus,0x50,&address,10);

	usleep(5000);

	if(values_read != NULL)
	{
		printf("bytes have been read!\n");		
	}

	int read_counter = 0;

	for(read_counter = 0;read_counter < 10;read_counter++)
	{
			printf("Counter:%d  0x%x\n",read_counter,  *(values_read+read_counter)); 
	}
		

	free(values_read);	
    	
	return 0;
}
```
Then compile with:

```bash
gcc -g   -L/usr/local/lib -I/usr/local/include main.c -l24c16  -o test
```

As you can see, it is now possible to read a bunch of bytes or to write a bunch of bytes add a specific address within a specific block of the EEPROM. Please not that the code is still experimental and good enough for my project.

DOCUMENTATION
-------------

The main tools have manual pages, which are installed by "make install".
See these manual pages for command line interface details and tool specific
information.

The other tools come with simple text documentation, which isn't installed.


QUESTIONS AND BUG REPORTS
-------------------------

Please post your questions and bug reports to the linux-i2c mailing list:
  linux-i2c@vger.kernel.org
For additional information about this list, see:
  http://vger.kernel.org/vger-lists.html#linux-i2c
