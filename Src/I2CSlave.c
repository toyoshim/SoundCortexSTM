// Copyright 2019, Takashi Toyoshima <toyoshim@gmail.com>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of the authors nor the names of its contributors
// may be used to endorse or promote products derived from this software
// without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
#include "I2CSlave.h"
#include "stm32f0xx_hal.h"

I2C_HandleTypeDef hi2c1;

static uint8_t tx_data[1];
static uint8_t rx_data[2];

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
  I2CSlaveWrite(rx_data[0]);
  I2CSlaveWrite(rx_data[1]);
}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c) {
  I2CSlaveStop();
  HAL_I2C_EnableListen_IT(hi2c);
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode) {
  // W: Addr -> RX -> Ack -> Data(Addr) -> Ack -> Addr -> TX -> Ack -> Data(Val)
  // R: Addr -> RX -> Ack -> Data(Addr) -> Ack -> Data (Val) -> Ack
  I2CSlaveStart(AddrMatchCode >> 1);
  if (TransferDirection) {
    I2CSlaveWrite(rx_data[0]);
    I2CSlaveRead(tx_data);
    // TODO: Send NACK when I2CSlaveRead() fails above.
    HAL_I2C_Slave_Sequential_Transmit_IT(hi2c, tx_data, 1, I2C_LAST_FRAME);
  } else {
    HAL_I2C_Slave_Sequential_Receive_IT(hi2c, rx_data, 2, I2C_NEXT_FRAME);
  }
}

void I2CSlaveInit(uint8_t address1, uint8_t address2) {
  HAL_I2C_EnableListen_IT(&hi2c1);
}

__attribute__ ((weak)) void I2CSlaveStart(uint8_t addr) {}
__attribute__ ((weak)) void I2CSlaveStop() {}
__attribute__ ((weak)) bool I2CSlaveWrite(uint8_t data) { return true; }
__attribute__ ((weak)) bool I2CSlaveRead(uint8_t* data) { return false; }
