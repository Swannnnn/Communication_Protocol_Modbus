#include <string.h>
#include <stdio.h>
#include <sys/types.h>

#include "tramesBuilding.h"
#include "Modbus.h"

int main (int argc, char** argv)
{
    int isSoketPort = 0;
    BOOL connexionOk = FALSE;

    SOCKET idConnexionSocket = INVALID_SOCKET;
    HANDLE handleSerialPort = NULL;

    printf("****************************************************************************\n");
    printf("*                             PROTOCOLE MODBUS                             *\n");
    printf("****************************************************************************\n");
    printf("baudrate: %d, bite size: %d, bit parity: %d, stop bit: %d, regulator adress: %d", SRL_BAUDRATE, SRL_BYTE_SZ, SRL_PARITY, SRL_STOPBIT, MODBUSREG_ADRESS);
    

    //*******************************************************************************
        // Creation et ouverture du support de communication

        handleSerialPort = connectionSerialPort();

    //*******************************************************************************

    if (handleSerialPort || (idConnexionSocket != INVALID_SOCKET) )
    {
        TypeRequest requestType = NO_REQUEST;
        TypeVal typeVal = NO_TYPE;

        while (requestType != REQUEST_QUIT)
        {
            char trameToSend[ARRAY_MAX_SIZE];
            int lengthTrameToSend = 0;
            char trameReceived[ARRAY_MAX_SIZE];
            int lengthTrameReceived = 99;
            memset(trameReceived,'\0',sizeof(trameReceived));

            int i = 0;
            ErrorComm codret = ERRORCOMM_ERROR;

            printf("\n****************************************************************************\n");
            printf("   1. Demande de lecture.\n");
            printf("   2. Demande d'ecriture.\n");
            printf("   3. Quitter.\n");
            printf("Que souhaitez-vous faire?\n");
            scanf("%d", &requestType);

            //*******************************************************************************
            // Creation de la trame de requete Modbus
            if (requestType == REQUEST_READ || requestType == REQUEST_WRITE)
                lengthTrameToSend = createRequestTrame(requestType, trameToSend, &typeVal);
            else
                continue;

            printf("\n Send trame (length = %i):", lengthTrameToSend);
            for (i = 0; i < lengthTrameToSend ; i++)
            {
                printf("%02X ",((unsigned char)trameToSend[i]));
            }
            printf("\n");

            //*******************************************************************************
            // Envoie de la requete Modbus sur le supporte de communication et reception de la trame reponse

            if (!lengthTrameToSend)
            {
                printf("\nError when sending the trame, trame is empty...");
                return 1;
            }

            /* Sending the trameToSend to the serial port and receiving the response in trameReceived. */
            codret = sendAndReceiveSerialPort(handleSerialPort, TIMEOUT, trameToSend, lengthTrameToSend, trameReceived, &lengthTrameReceived);
            
            if (codret != ERRORCOMM_NOERROR)
                printState(codret);

            //*******************************************************************************
            //Decodage de la trame reçue
            if (codret!=ERRORCOMM_NOERROR || lengthTrameReceived==0)
            {
                printState(codret);
            }
            else
            {
                printf("\n Receive trame (length = %i): ", lengthTrameReceived);
                for  (i = 0; i < lengthTrameReceived; i++)
                    printf("%02X ",(unsigned char)trameReceived[i]);
                printf("\n");

                /* Parsing the response from the Modbus device. */
                if (requestType == REQUEST_READ)
                    codret = parseModbusResponse(trameReceived, lengthTrameReceived, requestType, typeVal);

                if (codret != ERRORCOMM_NOERROR)
                    printState(codret);
            //*******************************************************************************
            }
        }

        //*******************************************************************************
        // Fermeture du support de communication
        
        // A COMPLETER
        
        //*******************************************************************************
    }
    else
        printf("Echec de connexion.");

    return 0;
}