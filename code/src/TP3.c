// modbusTP.c
//
#include <string.h>

#include "Modbus.h"
#include "TP3.h"

/*	 user defines begin	 */

#define SRL_BAUDRATE 9600
#define SRL_BYTE_SZ 8
#define SRL_PARITY 0
#define SRL_STOPBIT 1

#define TIMEOUT 1000
#define RCV_VAL_TYPE 0

#define MODBUSREG_ADRESS 1

/*	 user defines end	 */

int Globale_ip_clientserver_choice;

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

SOCKET connectionTCPIpPort()
{
    BOOL connexionOk = FALSE;
    SOCKET idConnexionSocket = INVALID_SOCKET;

    // A COMPLETER ok

	int isClient, port = 0;

	activeWinsocket();

	int protocole_type;
	printf("\nProtocole TCP (0) ou UDP (1)? : ");
	scanf("%d",&protocole_type);

	printf("\nVous etes serveur (0) ou client (1)? : ");
	scanf("%d",&isClient);

	if (protocole_type == 0)
	{		
		printf("\nEntrez le numero de port : ");
		scanf("%d",&port);

		idConnexionSocket = createSocket(512,0/*TCP*/,TIMEOUT,TIMEOUT);

		switch (isClient)
		{
		case 0: /* serveur */
			idConnexionSocket = acceptSocket(idConnexionSocket,port,5);
			break;

		case 1: /* client */
			char ipAdress[16];
			printf("\nEntrez l'adresse IP du serveur : ");
			scanf("%s",&ipAdress);

			int connexionOk = connectSocket(idConnexionSocket,ipAdress,port);
			if (connexionOk != 1)
				printf("\nConnection failed");
			break;
		
		default:
			break;
		}
	}

    return idConnexionSocket;
}

HANDLE connectionSerialPort()
{
    BOOL connexionOk = FALSE;
    HANDLE handleSerialPort = NULL;

     // A COMPLETER	ok

	printf("\nnumero de port com? : ");
	int com_port;
	scanf("%d",&com_port);

	handleSerialPort = createSerialPort(com_port);

	if (setParamSerialPort(handleSerialPort, SRL_BAUDRATE, SRL_BYTE_SZ, SRL_PARITY, SRL_STOPBIT) == 0)
		return NULL;

    return handleSerialPort;
}


/**
 * It asks the user for a request type, a starting address, a type of value to read/write, and a value
 * to write. It then creates a trame to send to the PLC
 * 
 * @param i_requestType the type of request (read or write)
 * @param i_trameSend the trame to be sent
 * @param i_typeVal the type of the value to be read or written.
 * 
 * @return The length of the trame.
 */
int createRequestTrame(TypeRequest i_requestType, char* i_trameSend, TypeVal* i_typeVal)
{
	int lengthTrameSend, startAdress, nbParamsToread, codeFunction;

	/* USER CODE END */

	switch(i_requestType)
	{
		// Demande de lecture:
		case REQUEST_READ:{
			printf("\n DEMANDE DE LECTURE\n");

			printf("\nA partir de quelle adresse souhaitez-vous lire? : ");
            scanf("%d", &startAdress);

			//printf("\nQuel type de parametre voulez-vous lire? 0 (short) / 1 (int) / 2 (float)? ");
            //scanf("%d", i_typeVal);
			*i_typeVal = RCV_VAL_TYPE;

			int nb_parameters;
			printf("\nnombre de valeurs a lire: ");
            scanf("%d", &nb_parameters);

            // A COMPLETER ok

			lengthTrameSend = makeTrameLecModBus(MODBUSREG_ADRESS, MODBUS_FUNCTION_READ_NWORDS, startAdress, nb_parameters, i_trameSend, INTEL);
			

			break;}

		// Deamnde d'ecriture
		case REQUEST_WRITE:{
			printf("\n DEMANDE D'ECRITURE\n");

			printf("\nA partir de quelle adresse souhaitez-vous ecrire? : ");
            scanf("%d", &startAdress);

			//printf("Quel type de parametre voulez-vous ecrire? 0 (short) / 1 (int) / 2 (float)\n");
            //scanf("%d", i_typeVal);
			*i_typeVal = RCV_VAL_TYPE;

			short value_to_write;
			printf("\nEntre la valeur a ecrire? : ");
			scanf("%d",&value_to_write);

            // A COMPLETER

			lengthTrameSend = makeTrameEcrModBusFromShort(MODBUSREG_ADRESS, MODBUS_FUNCTION_WRITE_WORD, startAdress, value_to_write, i_trameSend, INTEL);

			break;}
		default:
			printf("\nimpossible choice...");
			return 0;
			break;
	}

	return lengthTrameSend;
}

ErrorComm parseModbusResponse(char* i_trameReceive, int i_lengthTrameReceived, TypeRequest i_requestType, TypeVal i_typeVal)
{
	ErrorComm codret = ERRORCOMM_ERROR;

	char value_buffer[100];
	int  nb_values_buffer;
	int  adress_buffer;
	int  codeFunction_buffer;


    // A COMPLETER

	switch (i_requestType)
	{
	case REQUEST_READ:
		switch (i_typeVal)
		{
		case TYPE_SHORT:
			
			short readen_value= 0;

			codret = parseTrameModBus(i_trameReceive, i_lengthTrameReceived, value_buffer, &nb_values_buffer, &adress_buffer, &codeFunction_buffer, INTEL);

			for (int nb_param = 0;nb_param<nb_values_buffer;nb_param++)
			{
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
