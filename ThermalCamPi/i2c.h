#ifndef I2C_H
#define I2C_H

/*
 * I2C driver for the Raspberry Pi. Use this to interface with sensors
 * and other peripherals that use the I2C protocol.
 *
 * Author: Anna Zeng <zeng@cs.stanford.edu>
 * Date: May 14, 2016
 */


/*
* Initializes I2C pins and enables I2C communcation.
* 
* Same as given library implementation.
*/
void i2c_init(void);

/*
* Completes I2C read from slave address byte by byte and stores in char array
* 
* Same as given library implementation except delays are shortened as much as possible to
* speed up data transmission.
*/
void i2c_read(unsigned slave_address, char *data, int data_length);

/*
* Completes I2C write from slave address byte by byte and from given char array
* 
* Same as given library implementation.
*/
void i2c_write(unsigned slave_address, char *data, int data_length);

/*
* Sets I2C frequency by setting associated hardware register
*/ 
void i2c_set_freq(int freq_khz);

#endif
