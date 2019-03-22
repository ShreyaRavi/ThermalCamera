/**
   @copyright (C) 2017 Melexis N.V.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   Modified March 2019 by Shreya Ravi and Alexa Ramachandran (sravi2@stanford.edu, arama@stanford.edu), Stanford University, CS 107E 
   - Added support for Raspberry Pi baremetal I2C communication using I07E I2C library
*/


#include "MLX90640_I2C_Driver.h"
#include "i2c.h"
#include "timer.h"

/*
 * Initialize I2C module for Pi
 */
void MLX90640_I2CInit(void)
{
  i2c_init();
}

/*
 * Read a number of words from startAddress. Store into Data array.
 * Returns 0 if successful, -1 if error
 */
int MLX90640_I2CRead(uint8_t _deviceAddress, unsigned int startAddress, unsigned int nWordsRead, uint16_t *data)
{
  char write_start_addr[2];

  //Caller passes number of 'unsigned ints to read', increase this to 'bytes to read'
  uint16_t bytesRemaining = nWordsRead * 2;

  //It doesn't look like sequential read works. Do we need to re-issue the address command each time?

  uint16_t dataSpot = 0; //Start at beginning of array

  //Setup a series of chunked I2C_BUFFER_LENGTH byte reads
  while (bytesRemaining > 0)
  {
    write_start_addr[0] = startAddress >> 8;
    write_start_addr[1] = startAddress & 0xFF;
    i2c_write(_deviceAddress, write_start_addr, 2);
    
    uint16_t numberOfBytesToRead = bytesRemaining;
    if (numberOfBytesToRead > I2C_BUFFER_LENGTH) {
      numberOfBytesToRead = I2C_BUFFER_LENGTH;
    }

    // writes each byte to char array with MSByte first, LSByte second
    i2c_read(_deviceAddress, &(((char*) data)[dataSpot]), numberOfBytesToRead);

    // ARM architecture is little endian,
    // so when interpreting this as a 16-bit value (uint_16_t),
    // the LSByte is interpreted as the MSByte and vice versa

    // To fix this, flip every pair of bytes in the char array
    for (uint16_t x = 0; x < numberOfBytesToRead; x += 2) {
      char temp = ((char *) data)[dataSpot + x];
      ((char *) data)[dataSpot + x] = ((char *) data)[dataSpot + x + 1];
      ((char *) data)[dataSpot + x + 1] = temp;
    }

    dataSpot += numberOfBytesToRead;

    bytesRemaining -= numberOfBytesToRead;

    startAddress += numberOfBytesToRead / 2;
  }


  return (0); //Success

}

/*
 * Set I2C Freq, in kHz
 * MLX90640_I2CFreqSet(1000) sets frequency to 1MHz
 */
void MLX90640_I2CFreqSet(int freq)
{
  i2c_set_freq(freq);
}

/*
 * Write two bytes to a two byte address
 */
int MLX90640_I2CWrite(uint8_t _deviceAddress, unsigned int writeAddress, uint16_t data)
{

  // Write MSByte first, LSByte second
  char write_data[4];
  write_data[0] = writeAddress >> 8;
  write_data[1] = writeAddress & 0xFF;

  write_data[2] = data >> 8;
  write_data[3] = data & 0xFF;

  i2c_write(_deviceAddress, write_data, 4);

  // check that data was written correctly by reading it 
  // and checking if what is read is the same as what is expected
  uint16_t dataCheck = 0;
  MLX90640_I2CRead(_deviceAddress, writeAddress, 1, &dataCheck);
  if (dataCheck != data)
  {
    return -2;
  }

  return 0;

}

