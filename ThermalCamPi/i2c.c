/* 
 * File: i2c.c
 * -----------
 *
 * Authored by Anna
 * Bug fix by Chris Gregg and Tom Welch
 *
 * Modifications in March 2019 by Shreya Ravi (sravi2@stanford.edu), CS 107E
 */

#include "gpio.h"
#include "i2c.h"
#include "timer.h"


struct I2C { // I2C registers
    int control;
    int status;
    int data_length;
    int slave_address;
    int data_fifo;
    int clock_divider;
    int data_delay;
    int clock_stretch_timeout;
};

#define	CONTROL_READ				0x0001 // 1
#define CONTROL_CLEAR_FIFO	0x0010 // 16
#define CONTROL_START				0x0080 // 
#define CONTROL_ENABLE			0x8000

#define STATUS_TRANSFER_ACTIVE	0x001
#define STATUS_TRANSFER_DONE		0x002
#define STATUS_FIFO_NEED_WRITE	0x004
#define STATUS_FIFO_NEED_READ		0x008
#define STATUS_FIFO_CAN_WRITE		0x010
#define STATUS_FIFO_CAN_READ		0x020
#define STATUS_FIFO_EMPTY				0x040
#define STATUS_FIFO_FULL				0x080
#define STATUS_ERROR_SLAVE_ACK	0x100
#define STATUS_TIMEOUT					0x200

#define FIFO_MAX_SIZE 16

/*
 * There are three I2C masters on the Raspberry Pi, with the following
 * virtual memory addresses:
 * 	• BSC0: 0x7E20_5000
 * 	• BSC1: 0x7E80_4000
 * 	• BSC2: 0x7E80_5000
 * We are using BSC1. The other two do not have available GPIO pins
 * for use on the A+ model.
*/

#define SDA GPIO_PIN2
#define SCL GPIO_PIN3
#define BSC_BASE 0x20804000

static volatile struct I2C *i2c = (struct I2C *) BSC_BASE;

#define NORM_DELAY 500

/*
* Initializes I2C pins and enables I2C communcation.
* 
* Same as given library implementation.
*/
void i2c_init(void) {
    gpio_set_function(SDA, GPIO_FUNC_ALT0);
    gpio_set_function(SCL, GPIO_FUNC_ALT0);
    i2c->control = CONTROL_ENABLE;
}

/*
* Completes I2C read from slave address byte by byte and stores in char array
* 
* Same as given library implementation except delays are shortened as much as possible to
* speed up data transmission.
*/
void i2c_read(unsigned slave_address, char *data, int data_length) {
    // clear out the FIFO
    i2c->control |= CONTROL_CLEAR_FIFO;
    while (!(i2c->status & STATUS_FIFO_EMPTY))
        ;
    // clear previous transfer's flags
    i2c->status |= STATUS_TRANSFER_DONE |
                   STATUS_ERROR_SLAVE_ACK |
                   STATUS_TIMEOUT;

    // set slave address + data length
    i2c->slave_address = slave_address;
    i2c->data_length = data_length;
    int data_index = 0;

    // begin read
    i2c->control |= CONTROL_READ | CONTROL_START;

    timer_delay_us(25);
    // keep reading until transfer is complete
    while ((i2c->status & STATUS_FIFO_CAN_READ) &&
            (!(i2c->status & STATUS_TRANSFER_DONE) ||
             (data_index < data_length))) {
        timer_delay_us(5);
        data[data_index++] = i2c->data_fifo;
    }
#if 0
    // inform end user of potential responses
    if (i2c->status & STATUS_ERROR_SLAVE_ACK) {
        printf("NACK received.\n");
    }
    if (i2c->status & STATUS_TIMEOUT) {
        printf("Clock timed out.\n");
    }
    if (data_index < data_length) {
        printf("Data transfer incomplete.\n");
    }
#endif
}

/*
* Sets I2C frequency by setting associated hardware register
*/ 
void i2c_set_freq(int freq_khz) {
    i2c->clock_divider = (150000 / freq_khz);
}

/*
* Completes I2C write from slave address byte by byte and from given char array
* 
* Same as given library implementation.
*/
void i2c_write(unsigned slave_address, char *data, int data_length) {
    // clear out the FIFO
    i2c->control |= CONTROL_CLEAR_FIFO;
    while (!(i2c->status & STATUS_FIFO_EMPTY))
        ;
    // clear previous transfer's flags
    i2c->status |= STATUS_TRANSFER_DONE |
                   STATUS_ERROR_SLAVE_ACK |
                   STATUS_TIMEOUT;

    // set slave address + data length
    i2c->slave_address = slave_address;
    i2c->data_length = data_length;
    int data_index = 0;

    // write first 16 chunks into FIFO
    while ((data_index < FIFO_MAX_SIZE) &&
            (data_index < data_length)) {
        i2c->data_fifo = data[data_index++];
    }

    // begin write
    i2c->control &= ~CONTROL_READ;
    i2c->control |= CONTROL_START;

    // as fifo clears up, continue transferring until done
    while (!(i2c->status & STATUS_TRANSFER_DONE) &&
            (i2c->status & STATUS_FIFO_CAN_WRITE) &&
            (data_index < data_length)) {
        i2c->data_fifo = data[data_index++];
    }

    // wait until the FIFO's contents are emptied by the slave
    while (!(i2c->status & STATUS_FIFO_EMPTY));
}
