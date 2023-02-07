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

    if (handleSerialPort)
    {
        TypeRequest requestType = NO_REQUEST;
        TypeVal typeVal = TYPE_SHORT;

        while (requestType != REQUEST_QUIT)
        {

            /* Initializing the array of trames. */
            TRAMES_HANDLER trames[MODBUSREG_CHANNEL_SZ];
            for (int init_index=0; init_index < MODBUSREG_CHANNEL_SZ; init_index++)
            {
                trames[init_index].lengthTrameToSend = 0;
                trames[init_index].lengthTrameReceived = 99;
                memset(trames[init_index].trameReceived,'\0',sizeof(trames[init_index].trameReceived)); 
            }

            int i = 0;
            ErrorComm codret = ERRORCOMM_ERROR;

            printf("\n****************************************************************************\n");
            printf("   1. Demande de lecture.\n");
            printf("   2. Demande d'ecriture.\n");
            printf("   3. Quitter.\n");
            printf("Que souhaitez-vous faire?\n");
            scanf("%d", &requestType);

            //*******************************************************************************
            // Creation des trames de requete Modbus et envoie de ces trames

            /* Checking if the request type is read or write. If it is, it will call the function
            createRequestTrame. If it is not, it will continue. */
            if (requestType == REQUEST_READ || requestType == REQUEST_WRITE)
                codret = createRequestTrame(requestType, trames);
            else
                continue;

            if (codret != ERRORCOMM_NOERROR)
            {
                printState(codret);
                break;
            }

            /* Sending each trame to the Modbus device by channel */
            for (int channel=0; channel < MODBUSREG_CHANNEL_SZ; channel++)
            {
                /* Printing the trame to send. */
                printf("\n Send trame (length = %i):", trames[channel].lengthTrameToSend);
                for (i = 0; i < trames[channel].lengthTrameToSend ; i++)
                {
                    printf("%02X ",((unsigned char)trames[channel].trameToSend[i]));
                }

                /* Sending the trame to the Modbus device. */
                codret = sendAndReceiveSerialPort(handleSerialPort, TIMEOUT, trames[channel].trameToSend, trames[channel].lengthTrameToSend, trames[channel].trameReceived, &trames[channel].lengthTrameReceived);
                
                if (codret != ERRORCOMM_NOERROR)
                {
                    printState(codret);
                    break;
                }
            }

            //*******************************************************************************
            //Decodage des trames reçue
            if (requestType == REQUEST_WRITE)
                continue;
                        
            /* Parsing and Printing all the received trames. */
            for (int channel=0; channel < MODBUSREG_CHANNEL_SZ; channel++)
            {
                printf("\n############### channel %d ###############", channel);
                if (codret!=ERRORCOMM_NOERROR || trames[channel].lengthTrameReceived==0)
                {
                    printState(codret);
                }
                else
                {
                    /* Printing the received trame. */
                    printf("\n Receive trame (length = %i): ", trames[channel].lengthTrameReceived);
                    for  (i = 0; i < trames[channel].lengthTrameReceived; i++)
                        printf("%02X ",(unsigned char)trames[channel].trameReceived[i]);
                    printf("\n");

                    
                    /* Parsing the response from the Modbus device. */
                    codret = parseModbusResponse(trames[channel].trameReceived, trames[channel].lengthTrameReceived, requestType, typeVal);

                    if (codret != ERRORCOMM_NOERROR)
                        printState(codret);
                    
                //*******************************************************************************
                }
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