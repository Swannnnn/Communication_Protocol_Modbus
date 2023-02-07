/*
 *  ModbusTp.h
 *  
 *
 *  Created by SIEPRAWSKI Audrey on 18/03/2015.
 *
 */

#ifndef MODBUSTP_H
#define MODBUSTP_H

#include "Common.h"
#include "SerialPort.h"

/*	 user defines begin	 */

#define SRL_BAUDRATE 9600
#define SRL_BYTE_SZ 8
#define SRL_PARITY 0
#define SRL_STOPBIT 1

#define TIMEOUT 1000
#define RCV_VAL_TYPE 0
#define ARRAY_MAX_SIZE 100

#define MODBUSREG_ADRESS 1
#define MODBUSREG_OFFSET 1024
#define MODBUSREG_CHANNEL_SZ 3

/*	 user defines end	 */

typedef enum typerequest {
  REQUEST_READ = 1,
  REQUEST_WRITE,
  REQUEST_QUIT,
  NO_REQUEST

  } TypeRequest;


typedef enum typeval {
  TYPE_SHORT = 0,
  TYPE_INT,
  TYPE_FLOAT,
  NO_TYPE

  } TypeVal;

  
typedef struct{
    char trameToSend[ARRAY_MAX_SIZE];
    int lengthTrameToSend;
    char trameReceived[ARRAY_MAX_SIZE];
    int lengthTrameReceived;
}TRAMES_HANDLER;


void printState(ErrorComm codret);

HANDLE connectionSerialPort();

ErrorComm createRequestTrame(TypeRequest i_requestType, TRAMES_HANDLER * trames);
ErrorComm parseModbusResponse(char* i_trameReceive, int i_lengthTrameReceived, TypeRequest i_requestType, TypeVal i_typeVal);

#endif
