/*
Copyright 2017 Google Inc. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "OledScreenDriver.h"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>


bool OledScreenDriver::setup(int i2cFd_, uint8_t address_){
	i2cFd = i2cFd_;
	address = address_;

	if(i2cFd < 0){
		return false;
	}
	if(ioctl(i2cFd, I2C_SLAVE, address) < 0){
		return false;
	}

	// Send the screen setup commands.
	static uint8_t setup0[] = {
		0x00,0xae,0xd5,0x50,0xa8,0x3f,0xd3,0x00,0x40,0xb0,0x02,0x10,0xa1,0xc8,0xda,0x12,0x81,0x80,0xd9,0x02,0xdb,0x35,0xa4,0xa6,0xaf  //This is typical SH1106 init sequence
	};

	if(write(i2cFd, setup0, sizeof(setup0)) != sizeof(setup0)){
		return false;
	}
	return true;
}


void OledScreenDriver::draw(ofFbo &fbo){
	if(i2cFd < 0){
		return;
	}
	if(ioctl(i2cFd, I2C_SLAVE, address) < 0){
		return;
	}

	ofPixels pixels;
	fbo.readToPixels(pixels);

	uint8_t buf[133]; //SH1106 line is 132-byte + command byte
	uint8_t cmd[4]; //init command array for column address and page
	cmd[0] = 0x00;  //start cmd
	cmd[1] = 0x02;  //lower column address 
	cmd[2] = 0x10;  //higher column address
	cmd[3] = 0xAF;  //set value to 175(0xAF) which will be the PAGE0 command after increment

	// Write data command.
	buf[0] = 64;

	int pixelIdx = 0;
	for(int seg=0; seg<8; ++seg){
		// The screen is drawn in 8 horizontal segments.
		// Each segment is 128x8 pixels.
		// Each byte represents one column.

		// Clear the buffer first.
		for(unsigned int x=1; x<sizeof(buf); ++x){
			buf[x] = 0;
		}

		// Traverse the 8 rows.
		for(int y=0; y<8; ++y){
			for(int x=2; x<130; ++x){  //as SH1106 has 132 width, we center the 128-byte line from 2 to 130px
				int set = pixels[pixelIdx] >= 127;
				buf[x+1] |= set << y;
				pixelIdx += 4;
			}
		}
		//increment the PAGE command and send it
		cmd[3]+=1;
		if(write(i2cFd, cmd, sizeof(cmd)) != sizeof(cmd)){
			break;
		}

		if(write(i2cFd, buf, sizeof(buf)) != sizeof(buf)){
			break;
		}
	}

	// Flush the new data to the screen.
	static uint8_t flush[] = {
		0, 175
	};

	if(write(i2cFd, flush, sizeof(flush)) != sizeof(flush)){
		return;
	}
}
