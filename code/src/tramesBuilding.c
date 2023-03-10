// modbusTP.c
//
#include <string.h>

#include "Modbus.h"
#include "tramesBuilding.h"



void printState(ErrorComm codret)
{
	switch(codret)
	{
		case ERRORCOMM_BCC:{
			printf("\nError BCC\n");
		break;}

		case ERRORCOMM_TIMEOUT:{
			printf("\nError Timeout\n");
		break;}

		case ERRORCOMM_NOERROR:{
			printf("\nNo error\n");
		break;}

		case ERRORCOMM_ERROR:{
			printf("\nError\n");
		break;}

		default:{
			printf("\nError\n");
		break;}
	}
	return;
}


/**
 * It opens a serial port, sets the parameters, and returns the handle
 * 
 * @return A handle to the serial port.
 */
HANDLE connectionSerialPort()
{
    BOOL connexionOk = FALSE;
    HANDLE handleSerialPort = NULL;

	/* Asking the user to enter the COM port number. */
	printf("\nCOM Port number? : ");
	int com_port;
	scanf("%d",&com_port);

	/* Creating a serial port. */
	handleSerialPort = createSerialPort(com_port);

	/* Setting the parameters of the serial port. */
	if (setParamSerialPort(handleSerialPort, SRL_BAUDRATE, SRL_BYTE_SZ, SRL_PARITY, SRL_STOPBIT) == 0)
		return NULL;

    return handleSerialPort;
}



/**
 * It creates trames to send to the regulator
 * 
 * @param i_requestType the type of request to be made.
 * @param trames the array of trames to send to the regulator.
 * 
 * @return the error code.
 */
ErrorComm createRequestTrame(TypeRequest i_requestType, TRAMES_HANDLER * trames)
{
	int startAdress, nbParamsToread;

	switch(i_requestType)
	{
		// Demande de lecture:
		case REQUEST_READ:{
			printf("\n READING DEMAND\n");

			printf("\nFrom which address you want to read? : ");
            scanf("%d", &startAdress);

			/* Asking the user to enter the number of values to read. */
			int nb_parameters;
			printf("\nNumber of values to read?: ");
            scanf("%d", &nb_parameters);

			/* Creating the trames to send to the regulator. */
			for (int channel=0; channel < MODBUSREG_CHANNEL_SZ; channel++)
				trames[channel].lengthTrameToSend = makeTrameLecModBus(MODBUSREG_ADRESS, MODBUS_FUNCTION_READ_NWORDS, startAdress + (MODBUSREG_OFFSET * channel), nb_parameters, trames[channel].trameToSend, INTEL);
			
			break;}

		// Deamnde d'ecriture
		case REQUEST_WRITE:{
			printf("\n WRITING DEMAND\n");

			printf("\nFrom which address you want to read? : ");
            scanf("%d", &startAdress);

			/* Asking the user to enter the number of values to write. */
			int values_sz;
			do
			{
				printf("\nNumber of values to write? : ");
				scanf("%d",&values_sz);

			} while (values_sz > ARRAY_MAX_SIZE); //array size limit
			

			/* Asking the user to enter the values to write. */
			short values_arr[ARRAY_MAX_SIZE];
			for (int val_arr_index = 0; val_arr_index < values_sz; val_arr_index++)
			{
				do
				{
					printf("\nEnter the value [%d]: ",val_arr_index);
					scanf("%d",&values_arr[val_arr_index]);

				}while (abs(values_arr[val_arr_index]) > 32767); //type limit
			}

			/* Creating the trames to send to the regulator. */
			for (int channel=0; channel < MODBUSREG_CHANNEL_SZ; channel++)
				trames[channel].lengthTrameToSend = makeTrameEcrModBusFromShortTab(MODBUSREG_ADRESS, MODBUS_FUNCTION_WRITE_WORDS, startAdress + (MODBUSREG_OFFSET * channel), values_arr, values_sz, trames[channel].trameToSend, INTEL);

			break;}
		default:
			printf("\nimpossible choice...");
			return ERRORCOMM_EVENT;
			break;
	}

	return ERRORCOMM_NOERROR;
}

/**
 * It parses the received trames and prints the values received from the regulator
 * 
 * @param i_trameReceive The received trame.
 * @param i_lengthTrameReceived the length of the received trame
 * @param i_requestType REQUEST_READ
 * @param i_typeVal Type of value to be read.
 * 
 * @return the error code of the parsing.
 */
ErrorComm parseModbusResponse(char* i_trameReceive, int i_lengthTrameReceived, TypeRequest i_requestType, TypeVal i_typeVal)
{
	ErrorComm codret = ERRORCOMM_ERROR;

	char value_buffer[ARRAY_MAX_SIZE];
	int  nb_values_buffer, adress_buffer, codeFunction_buffer;

	switch (i_requestType)
	{
	case REQUEST_READ:
		switch (i_typeVal)
		{
		case TYPE_SHORT:
			
			short readen_value= 0;

			/* Parsing the received trame. */
			codret = parseTrameModBus(i_trameReceive, i_lengthTrameReceived, value_buffer, &nb_values_buffer, &adress_buffer, &codeFunction_buffer, INTEL);

			/* Printing the values received from the PLC. */
			for (int nb_param = 0;nb_param<nb_values_buffer;nb_param++)
			{
				/* Converting the received value from the PLC to a short value. */
				readen_value = ModBusShortAsciiToIeee(&value_buffer[nb_param*2], INTEL);

				printf("\nReceived value = %d", readen_value);
			}
			
			
			
			break;
		
		default:
			break;
		}
		break;
	
	default:
		break;
	}

	

	return codret;
}
