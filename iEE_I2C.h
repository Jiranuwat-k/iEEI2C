/*
  iEE_I2C.h - TWI/I2C library for iEE-M328
  Modified 2024 by Jiranuwat.k (https://jiranuwats.gitbook.io/docs/)
  Ref. https://iot-kmutnb.github.io/blogs/arduino/avr_gcc_part-5
  This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License.
*/
#ifndef IEE_I2C_H
#define IEE_I2C_H
#include <avr/io.h>
#include <util/delay.h>  // for _delay_ms()
#include <util/twi.h>    // for macros/constants for TWI
  class iEE_I2C
    {
      private:
        #define TWBR_VALUE(fscl)   (((F_CPU / (fscl)) - 16) / 2)
        #define CHECK_TIMEOUT      (4) // timeout in msec
        uint8_t WaitDataComplete();
        uint8_t SendStart();
        inline void SendStop();
      public:
        void    begin(uint32_t speed = 400000);
        uint8_t StartTransmission(uint8_t);
        void    EndTransmission(void);
        uint8_t write(uint8_t);
        uint8_t read(uint8_t *, uint8_t);
        uint8_t readReg(uint8_t addr, uint8_t reg_addr);
    };

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_I2C)
extern iEE_I2C i2c;
#endif
#endif