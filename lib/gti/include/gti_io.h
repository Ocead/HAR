//
// Created by Johannes on 02.12.2019.
//
#pragma once

#ifndef HAR_GTI_IO_H
#define HAR_GTI_IO_H

typedef void * DSCB;

typedef enum {
    PA = 0,
    PB = 1,
    PC = 2
} PORT;

typedef unsigned char uint8_t;

DSCB initIO();

uint8_t inputByte(DSCB boardHandle,
                  PORT portName,
                  uint8_t * val);

uint8_t outputByte(DSCB boardHandle,
                   PORT portName,
                   uint8_t val);

#endif //HAR_GTI_IO_H
