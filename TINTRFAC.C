

#include "TEKDECL.H"
#include "decl.h"
#include <string.h>




#define DEBUG 1

#define IN_BUFFER 1050
//#define STR_BUFFER 100

extern int bd;	//defined in main scope.c file

extern char cont[];// = "\x3F\x5F\x40\x21\x0";
extern char scopt[];//= "\x3F\x5F\x41\x20\x0";


char data_buffer[IN_BUFFER+1];  //make it just a little bigger than required for some protection






//*************************************************************************************
//*
//*     displayedChannels  checks to determine which channels are displayed on the scope
//*
//*         Receives a pointer to an array large enough to hold NUMCHANNELS bytes
//          Returns: VOID, but fills the array with a 1 if the channel is displayed
//*                   or a zero if not displayed
//*
//*
//*************************************************************************************
void displayedChannels(uint8_t *list)
{
  uint8_t channel;
  //Check main waveforms CH1, CH2, ADD, MULT
  //Check saved waveforms REF1, REF2, REF3, REF4

  for (channel=0; channel <NUMCHANNELS; channel++)
  {
      if (CheckChannel(channel)) list[channel]=1;
      else list[channel]=0;
  }
}




//********************************************************************************
//*
//*  Check Generic Channel if displayed
//*   Receives: channel name
//*   Returns: 1 if the channel is displayed, zero if not displayed
//*
//********************************************************************************
uint8_t CheckChannel(uint8_t channel) {
  uint8_t len;
  uint16_t recv;

  // Tell the client to LISTEN for incoming data
//  printf("cont pointer is: %p",cont);
//  printf("cont = <%s>\r\n",cont);
  ibcmd(bd,cont,strlen(cont));	//console talks, device listens

  // Send the client the commands
  strcpy(data_buffer,"PATH OFF");
  ibwrt(bd,data_buffer,8);

// Assemble the request

  switch (channel) {
    case CH1:
	  strcpy(data_buffer,"VMO? CH1");
	  len = 8;
      break;

    case CH2:
	  strcpy(data_buffer,"VMO? CH2");
	  len = 8;
      break;

    case ADD:
	  strcpy(data_buffer,"VMO? ADD");
	  len = 8;
      break;

    case MULT:
	  strcpy(data_buffer,"VMO? MUL");
	  len = 8;
      break;

    case REF1:
	  strcpy(data_buffer,"REFDISP? REF1");
	  len = 13;
      break;

    case REF2:
	  strcpy(data_buffer,"REFDISP? REF2");
	  len = 13;
      break;

    case REF3:
	  strcpy(data_buffer,"REFDISP? REF3");
	  len = 13;
      break;

    case REF4:
	  strcpy(data_buffer,"REFDISP? REF4");
	  len = 13;
      break;
  }
  //tell the scope what to send
  ibwrt(bd, data_buffer, strlen(data_buffer));
  //Tell the scope to start talking
  ibcmd(bd, scopt, strlen(scopt));


  //read data from bus
  ibrd(bd, data_buffer, 1050);
//  printf("Data length read: %d\r\n", ibcnt);
//  printf(" channel: %d is <%s>", channel,data_buffer);

  //if (recv ==0) Serial.println (F("Errored out"));
  if ((data_buffer[0] == 'O') && (data_buffer[1] == 'N'))
  {
    //The channel is displayed.
    //Now get its data.
    //Serial.print(F("channel "));
    //Serial.print((uint8_t)channel,DEC);
   // Serial.println(F(" is displayed"));
    return 1;
  }
  else {
    //Serial.print(F("channel "));
    //Serial.print((uint8_t)channel,DEC);
    //Serial.println(F(" is NOT displayed"));
  }
  return 0;
}






//********************************************************************************
//*
//*     Read Waveform Preamble
//*         Sends commands to obtain the preamble, then parse out the extraneous characters
//*         Receives: channel name
//*         Returns: VOID, but the preamble text is in the data buffer
//*
//********************************************************************************
void ReadWfmPre(uint8_t channel)
{
  uint16_t recv;
  uint8_t len;


  // Tell the client to LISTEN for incoming data
  ibcmd(bd, cont, strlen(cont));

  // send standard setup instructions
  strcpy(data_buffer, "PATH OFF;DAT ENC:RPB");
  ibwrt(bd, data_buffer, strlen(data_buffer));


  //set data source and initiate the send of the waveform preamble
  switch (channel) {
    case CH1:
	  strcpy(data_buffer,"DAT SOU:CH1;WFM?");
	  len = 16;
      break;

    case CH2:
	  strcpy(data_buffer,"DAT SOU:CH2;WFM?");
	  len = 16;
      break;

    case ADD:
	  strcpy(data_buffer,"DAT SOU:ADD;WFM?");
	  len = 16;
      break;

    case MULT:
	  strcpy(data_buffer,"DAT SOU:MUL;WFM?");
	  len = 16;
      break;

    case REF1:
	  strcpy(data_buffer,"DAT SOU:REF1;WFM?");
	  len = 17;
      break;

    case REF2:
	  strcpy(data_buffer,"DAT SOU:REF2;WFM?");
	  len = 17;
      break;

    case REF3:
	  strcpy(data_buffer,"DAT SOU:REF3;WFM?");
	  len = 17;
      break;

    case REF4:
	  strcpy(data_buffer,"DAT SOU:REF4;WFM?");
	  len = 17;
      break;

    //default:
      //errorFunction();
  }


  ibwrt(bd, data_buffer, len);

  //Tell the client to start talking
  ibcmd(bd, scopt, strlen(scopt));

  //read string from the scope
  ibrd(bd, data_buffer, 1050);
  
  if (ibcnt != 0)  //parse the data IF data was received
  {
    ParseWFM();
    //Serial.write(data_buffer);
    //Serial.println();
  }
}








//********************************************************************************
//*
//*     Read Curve
//*         Sends commands to obtain the curve data, then parse out the extraneous characters
//*         Receives: channel name
//*         Returns: VOID, but the data is in the data buffer
//*
//********************************************************************************
void ReadCurve(uint8_t channel)
{
//  uint16_t recv;
  uint8_t len;

  // Tell the client to LISTEN for incoming data
  ibcmd(bd, cont, strlen(cont));

  // send standard setup instructions
  strcpy(data_buffer, "PATH OFF;DAT ENC:RPB");
  ibwrt(bd, data_buffer, strlen(data_buffer));

  switch (channel) {
    case CH1:
	  strcpy(data_buffer, "DAT SOU:CH1;CURV?");
      break;

    case CH2:
	  strcpy(data_buffer, "DAT SOU:CH2;CURV?");
      break;

    case ADD:
      strcpy(data_buffer, "DAT SOU:ADD;CURV?");
      break;

    case MULT:
      strcpy(data_buffer, "DAT SOU:MUL;CURV?");
      break;

    case REF1:
	  strcpy(data_buffer, "DAT SOU:REF1;CURV?");
      break;

    case REF2:
	  strcpy(data_buffer, "DAT SOU:REF2;CURV?");
      break;

    case REF3:
	  strcpy(data_buffer, "DAT SOU:REF3;CURV?");
      break;

    case REF4:
	  strcpy(data_buffer, "DAT SOU:REF4;CURV?");
      break;

    default:
      printf("invalid channel selection\r\n");
  }

  ibwrt(bd, data_buffer, strlen(data_buffer));

  //Tell the client to start talking
  ibcmd(bd, scopt, strlen(scopt));

  //recv = Read_Data();  //read string from bus
  ibrd(bd, data_buffer, 1050);

  if (ibcnt != 0)
  {
    //Serial.print(F("Data received: "));
    //Serial.print(recv);
    //Serial.println(F("bytes"));


    //now do something with the data
    ParseData();
    //now send the data the the flash drive
  }
}







//******************************************************************************
//*
//*   Parse WFMPre
//*
//******************************************************************************

uint8_t ParseWFM(void)
{
    //find the text within quotes in the WFM string
    //and move it to the beginning and null terminate it

    uint16_t pos1, pos2;
    uint16_t  x, index;
    char *ptr;


    ptr = (char *)strchr(data_buffer,'"');
    pos1 = (uint16_t) (ptr-data_buffer);
    //Serial.println(pos1);
    ptr = (char*)strchr(&data_buffer[pos1+1],'"');
    pos2 = (uint16_t) (ptr-data_buffer);
    for (x = pos1+1, index = 0; x < pos2; x++, index++) data_buffer[index] = data_buffer[x];
    data_buffer[index] = 0; //null terminate the new string
    return (uint8_t) index+1;
}







//******************************************************************************
//*
//*   Parse Data
//*
//******************************************************************************

void ParseData(void)
{
    //The data has 3 bytes of a header in the beginning
    //And one byte of a checksum at the end
    //So, move the data forward by 3 bytes to eliminate the header
    //The header is one byte of a special character, then 2 bytes of the data size in case it matters

    //The starting point is byte 3 and the number of bytes to move is 1024


//TODO: Use memmove()?  movemem()?    
    uint16_t  x, index;
    for (x = 0, index = 3; x < 1024; x++, index++) data_buffer[x] = data_buffer[index];
    //data_buffer[1024] = 0; //put a null at the end just to show where the end is.
}







