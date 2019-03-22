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

   Modified March 2019 by Shreya Ravi (sravi2@stanford.edu) and Alexa Ramachandran (arama@stanford.edu)

*/
#ifndef _MLX90640_I2C_Driver_H_
#define _MLX90640_I2C_Driver_H_

#include <stdint.h>

#define I2C_BUFFER_LENGTH 32

/*
 * Initialize I2C module for Pi (provided by CS 107E)
 */
void MLX90640_I2CInit(void);

/*
 * Read a number of words from startAddress. Store into Data array.
 * Returns 0 if successful, -1 if error
 * Uses I2C module for Pi (provided by CS 107E)
 */
int MLX90640_I2CRead(uint8_t slaveAddr, unsigned int startAddress, unsigned int nWordsRead, uint16_t *data);

/*
 * Write two bytes to a two byte address
 * Uses I2C module for Pi (provided by CS 107E)
 */
int MLX90640_I2CWrite(uint8_t slaveAddr, unsigned int writeAddress, uint16_t data);

/*
 * Set I2C Freq, in kHz
 * MLX90640_I2CFreqSet(1000) sets frequency to 1MHz
 * Uses I2C module for Pi (provided by CS 107E, custom addition to enable this)
 */
void MLX90640_I2CFreqSet(int freq);
#endif
