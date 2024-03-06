#include "iEE_I2C.h"
void iEE_I2C::begin(uint32_t speed){
  // Set SDA and SCL as inputs with pullup (Arduino A4/A5 pins)
  DDRC  &= ~((1 << PC4) | (1 << PC5));
  PORTC |=   (1 << PC4) | (1 << PC5);
  // Set TWPS[1:0] = "00" (Prescaler=1)
  TWSR = 0x00;
  // Set TWI Bit Rate Register
  TWBR = TWBR_VALUE(speed);
}

uint8_t iEE_I2C::WaitDataComplete() {
  uint8_t timeout = CHECK_TIMEOUT;
  // Wait until the TWINT bit in TWCR goes HIGH
  while (!(TWCR & (1 << TWINT))) {
    if (timeout == 0) {
      return 1;  // timeout occurred
    }
    timeout--;
    _delay_ms(1);
  }
  return 0;  // ok
}

uint8_t iEE_I2C::SendStart() {
  // Enable TWI, clear TWINT bit and send START condition
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  // Wait for end of data transmission
  if (WaitDataComplete()) {
    return 1;  // timeout
  }
  // Check if the start condition was successfully transmitted
  if ((TWSR & TW_STATUS_MASK) != TW_START) {
    return 1; // Return with start condition error
  }
  return 0; // ok
}

uint8_t iEE_I2C::StartTransmission(uint8_t addr){
  // Send I2C start condition
  if (SendStart()) {
    return 1;  // return with start condition error
  }
  // Load the first byte (slave address + R/W bit) into data register
  TWDR = addr;
  // Start transmission of the first byte
  TWCR = (1 << TWINT) | (1 << TWEN);
  // Wait for end of data transmission
  if (WaitDataComplete()) {
    return 2;  // Return with TWI data tranmission error
  }
  // Check if the device has acknowledged
  if ( (addr & 1) == TW_WRITE ) {
    if ((TWSR & TW_STATUS_MASK) != TW_MT_SLA_ACK) {
      return 3; // Return with NACK error (No ACK received)
    }
  } else {
    if ((TWSR & TW_STATUS_MASK) != TW_MR_SLA_ACK) {
      return 4; // Return with NACK error (No ACK received)
    }
  }
  return 0;  // ok
}
inline void iEE_I2C::SendStop(void) {
  // Send I2C stop condition
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}
void    iEE_I2C::EndTransmission(void){
  SendStop();
}

uint8_t iEE_I2C::write(uint8_t data){
  // Load the data byte into data register
  TWDR = data;
  // Start I2C transmission of data
  TWCR = (1 << TWINT) | (1 << TWEN);
  // Wait for end of I2C data transmission
  if (WaitDataComplete()) {
    return 1;  // Return with TWI data transmission error
  }
  if ((TWSR & TW_STATUS_MASK) != TW_MT_DATA_ACK) {
    return 1;  // Return with NACK error (No ACK received)
  }
  return 0;
}

uint8_t iEE_I2C::read(uint8_t *data, uint8_t ack){
  TWCR = (1 << TWINT) | (1 << TWEN) | (ack ? (1 << TWEA) : 0);
  if (WaitDataComplete()) {
    return 1;  // Return with TWI data reception error
  }
  *data = TWDR;  // Save the received data byte
  return 0; // ok
}

uint8_t iEE_I2C::readReg(uint8_t addr, uint8_t reg_addr){
  uint8_t data = 0;
  // Send the start condition
  SendStart();
  // Write the register address
  write((addr << 1) | TW_WRITE); // TW_WRITE is 0.
  write(reg_addr);
  // Send the repeated start condition
  SendStart();
  // Read one data byte from the slave device
  write((addr << 1) | TW_READ); // TW_READ is 1.
  read(&data, 0 /*no ack*/);
  // Send the STOP condition
  SendStop();
  return data;
}