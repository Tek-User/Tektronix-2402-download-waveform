
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "TEKDECL.H"
//#include "decl.h"





extern char data_buffer[];  //defined in Tek_Interface module
unsigned long colors[8] = {0x00AF00,0x0000FF, 0xFF00FF, 0x00A0A0, 0xA0A000, 0x005F7F, 0xFF6600, 0xFF0066};
//uint32_t colors[8] = {0x2ECC40, 0x7FDBFF, 0x39CCCC, 0x0074D9, 0x01FF70, 0x001F3F, 0x3D9970, 0xFDC00};  //green, blue, aqua, teal, lime, navy, olive, yellow

FILE *fp;




int saveSVG (char *filePrefix) {

//printf("starting to create file with prefix <%s>\r\n", filePrefix);

      //TODO:  send message to 7-segments "REAd" to indicate it is reading the scope and transferring data
	  fp = openFile(filePrefix);
	  if (fp == NULL)
	  {
//printf("ERROR OPENING FILE: ABORTING\r\n");
	     return 1;
	  }
      startSVG();
      print_graticule();
      print_displayed_channels();
      endSVG();
//printf("Closing file\r\n");
      fclose(fp);



//printf("Done!\r\n");
     return 0;
}








//***************************************************************************************
//*
//*       print the displayed channel info and traces
//*
//*           Receives: VOID
//*           Returns: VOID
//*
//***************************************************************************************
void print_displayed_channels(void)
{
     int channel;
     uint8_t textline = 0;    //textline is used to control position and color of the channel
     uint8_t displayed[NUMCHANNELS];

    //go thru the channels and read them
    displayedChannels(displayed);

    for (channel = 0; channel < NUMCHANNELS; channel++)
    {
	if (displayed[channel] == 1)
	{
	     printf("channel %d is displayed\r\n", channel);
	     ReadWfmPre(channel);  //data is returned in global data_buffer
	     displayText(textline);
	     ReadCurve(channel);
	     displayCurve(textline);
	     textline++;
      }
   }
}





//***************************************************************************************
//*
//*       Display the trace from the scope
//*
//*           Receives: the line number to control color.  Data is in global data_buffer
//*           Returns: VOID
//*
//***************************************************************************************
void displayCurve(uint8_t textline)
{
    uint16_t index;
    fprintf(fp,"\r\n", fp);
    //Serial.println (F("<g transform = \"translate(2,2)\" stroke=\"#00AF00\" fill=\"none\" stroke-width=\"1\">"));  //start a group
    fprintf(fp,"<g transform = \"translate(2,2)\" stroke=\"#");
    //if (colors[textline] < 0x100000) TEKfile.print(F("0"));  //pad leading zeros if necessary
    //if (colors[textline] < 0x010000) TEKfile.print(F("0"));  //pad leading zeros if necessary
    //if (colors[textline] < 0x001000) TEKfile.print(F("0"));  //pad leading zeros if necessary
    //if (colors[textline] < 0x000100) TEKfile.print(F("0"));  //pad leading zeros if necessary
    //TEKfile.print(colors[textline],HEX);
    fprintf(fp,"%06x",colors[textline]);
    fprintf(fp,"\" fill=\"none\" stroke-width=\"1\">\r\n");  //start a group


    fprintf(fp,"<polyline points=\"");

    for (index = 0; index < NUMPOINTS; index++)
    {
	fprintf(fp,"%d",index);
	fprintf(fp,",");
	//TEKfile.print(2*(255-data_buffer[index]));
	fprintf(fp,"%d",(2* (uint16_t)(255-(uint8_t)data_buffer[index])));
	fprintf(fp," ");
    }

    fprintf(fp,"\"/>\r\n");
    fprintf(fp,"</g>\r\n"); //finish group
}








//***************************************************************************************
//*
//*       Display a text line below the tracings and graticule
//*
//*           Receives: the line number for the text and to control color.  Data is in global data_buffer
//*           Returns: VOID
//*
//***************************************************************************************
void displayText(uint8_t textline)
{
//TODO:  Stroke and Fill should be variables based on text line number
    fprintf(fp,"\r\n");
    //Serial.println (F("<g transform = \"translate(2,2)\" stroke=\"#00AF00\" fill=\"#00AF00\" stroke-width=\"1\" font-size=\"20px\"  style=\"font-family: Arial, Tahoma, Verdana, Sans-Serif; font-weight: normal; font-style: normal\">"));  //start a group
    fprintf(fp,"<g transform = \"translate(2,2)\" stroke=\"#");
    //if (colors[textline] < 0x100000) TEKfile.print(F("0"));  //pad leading zeros if necessary
    //if (colors[textline] < 0x010000) TEKfile.print(F("0"));  //pad leading zeros if necessary
    //if (colors[textline] < 0x001000) TEKfile.print(F("0"));  //pad leading zeros if necessary
    //if (colors[textline] < 0x000100) TEKfile.print(F("0"));  //pad leading zeros if necessary
    fprintf(fp, "%06x",colors[textline]);
    fprintf(fp,"\" fill=\"#");
    //if (colors[textline] < 0x100000) TEKfile.print(F("0"));  //pad leading zeros if necessary
    //if (colors[textline] < 0x010000) TEKfile.print(F("0"));  //pad leading zeros if necessary
    //if (colors[textline] < 0x001000) TEKfile.print(F("0"));  //pad leading zeros if necessary
    //if (colors[textline] < 0x000100) TEKfile.print(F("0"));  //pad leading zeros if necessary
    fprintf(fp, "%06x",colors[textline]);
    fprintf(fp,"\" stroke-width=\"1\" font-size=\"20px\"  style=\"font-family: Arial, Tahoma, Verdana, Sans-Serif; font-weight: normal; font-style: normal\">\r\n");  //start a group
    fprintf(fp,"<text x=\"20\" y=\"");
    fprintf(fp,"%d",(535+(textline*25)));
    fprintf(fp,"\">");
    fprintf(fp,"%s",data_buffer);
    fprintf(fp,"</text>\r\n");
    fprintf(fp,"</g>\r\n"); //finish group
}





//***************************************************************************************
//*
//*       print the start of the SVG file
//*
//*           Receives: VOID
//*           Returns: VOID
//*
//***************************************************************************************
void startSVG(void)
{
    // setup the SVG vector graphics file
    fprintf(fp,"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n");
    // this is the entire page size, be sure to make it just big enough for all the stuff or else it is likely to be cropped!
    fprintf(fp,"<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.2\" baseProfile=\"full\" width=\"1027px\" height=\"715px\">\r\n");
    fprintf(fp,"\r\n");
}




//***************************************************************************************
//*
//*       print the end of the SVG file
//*
//*           Receives: VOID
//*           Returns: VOID
//*
//***************************************************************************************
void endSVG(void)
{
     //End the SVG vector graphics file
     fprintf(fp,"\r\n");
     fprintf(fp,"</svg>\r\n");   //signifies the end of the SVG graphics
}






//***************************************************************************************
//*
//*       print the graticule
//*
//*           Receives: VOID
//*           Returns: VOID
//*
//***************************************************************************************
void print_graticule(void) {
  #define max_x 1024
  #define max_y 511
  uint16_t x_offset = 12;
  uint16_t y_offset = 5;
  uint16_t x_pos = 0;
  uint16_t y_pos = 0;
  char x_str[10];
  char str_2[10];
  char y_str[10];



  //move the origin to 1,1 (absolute) so the entire bounding box will show for sure
  fprintf(fp,"<g transform = \"translate(1,1)\" stroke=\"#FF0000\" stroke-width=\"1\">\r\n");
  fprintf(fp,"<rect x=\"0\" y=\"0\" width=\"1025\" height=\"512\" fill=\"white\" stroke-width=\"2\" stroke=\"#FF0000\" />\r\n");
  fprintf(fp,"</g>\r\n");
  fprintf(fp,"\r\n");
  //TODO: VERIFY THE BOUNDING BOX IS JUST OUTSIDE THE DRAWING SPACE FOR THE GRATICULES AND DATA


   //and draw the graticules: 4 wide, 2 high:  50 samples per DIV, 5 div per major line, 1000 total.  Add an x_offset if desired.
   //Draw the vertical lines
   //Always position the origin inside the bounding box... for all further screen-related drawing including the actual data

   //Draw the minor vertical lines
   //interval = 50 points per div and avoid first and last lines
  fprintf(fp,"<g transform = \"translate(2,2)\" stroke=\"#FFBFBF\" stroke-width=\"1\">\r\n");
  for (x_pos = 50; x_pos < max_x-50; x_pos+= 50)
  {
     fprintf(fp,"<line x1=\"");
     itoa(x_pos+x_offset, x_str,10);
     fprintf(fp,"%s",x_str);
     fprintf(fp,"\" y1=\"0\" x2=\"");
     fprintf(fp,"%s",x_str);
     fprintf(fp,"\" y2=\"510\"/>\r\n");

     //draw the minor ticks
     itoa(x_pos+x_offset-1, x_str,10);
     itoa(x_pos+x_offset+1, str_2,10);
     for (y_pos = 0; y_pos < max_y-y_offset; y_pos += 10)
     {
	  itoa(y_pos+y_offset, y_str,10);
	  fprintf(fp,"<line x1=\"");
	  fprintf(fp,"%s",x_str);
	  fprintf(fp,"\" y1=\"");
	  fprintf(fp,"%s",y_str);
	  fprintf(fp,"\" x2=\"");
	  fprintf(fp,"%s",str_2);
	  fprintf(fp,"\" y2=\"");
	  fprintf(fp,"%s",y_str);
	  fprintf(fp,"\"/>\r\n");
     }
   }

  fprintf(fp,"</g>\r\n");
  fprintf(fp,"\r\n");

  //draw the minor horizontal lines and ticks here
  //interval = 25 points per div and avoid first and last lines
  //multiply y coord by 2 for plotting
  fprintf(fp,"<g transform = \"translate(2,2)\" stroke=\"#FFBFBF\" stroke-width=\"1\">\r\n");
  for (y_pos = 50; y_pos < max_y-50; y_pos+= 50)
  {
     fprintf(fp,"<line x1=\"0\" y1=\"");
     itoa(y_pos+y_offset, y_str,10);
     fprintf(fp,"%s",y_str);
     fprintf(fp,"\" x2=\"1023\" y2=\"");
     fprintf(fp,"%s",y_str);
     fprintf(fp,"\"/>\r\n");

     //draw the minor ticks
     itoa(y_pos+y_offset-1, y_str,10);
     itoa(y_pos+y_offset+1, str_2,10);
     for (x_pos = 0; x_pos < max_x-x_offset; x_pos += 10)
     {
	  itoa(x_pos+x_offset, x_str,10);
	  fprintf(fp,"<line x1=\"");
	  fprintf(fp,"%s",x_str);
	  fprintf(fp,"\" y1=\"");
	  fprintf(fp,"%s",y_str);
	  fprintf(fp,"\" x2=\"");
	  fprintf(fp,"%s",x_str);
	  fprintf(fp,"\" y2=\"");
	  fprintf(fp,"%s",str_2);
          fprintf(fp,"\"/>\r\n"); 
     }
     
   }

  fprintf(fp,"</g>\r\n");
  fprintf(fp,"\r\n");

  


   //Draw the major vertical lines
   //interval = 5div * 50 points per div = 250
  fprintf(fp,"<g transform = \"translate(2,2)\" stroke=\"#FF5F5F\" stroke-width=\"1\">/r/n");
  for (x_pos = 0; x_pos < max_x; x_pos+= 250)
  {
     fprintf(fp,"<line x1=\"");
     itoa(x_pos+x_offset, x_str,10);
     fprintf(fp,"%s",x_str);
     fprintf(fp,"\" y1=\"0\" x2=\"");
     fprintf(fp,"%s",x_str);
     fprintf(fp,"\" y2=\"510\"/>/r/n");
     //draw the major ticks here
     itoa(x_pos+x_offset-3, x_str,10);
     itoa(x_pos+x_offset+3, str_2,10);
     //if(x_pos>0 && x_pos < max_x-250)
     {
	for (y_pos = 0; y_pos < max_y-y_offset; y_pos += 10)
        {
             itoa(y_pos+y_offset, y_str,10);
	     fprintf(fp,"<line x1=\"");
             fprintf(fp,"%s",x_str);
             fprintf(fp,"\" y1=\"");
             fprintf(fp,"%s",y_str);
	     fprintf(fp,"\" x2=\"");
	     fprintf(fp,"%s",str_2);
	     fprintf(fp,"\" y2=\"");
	     fprintf(fp,"%s",y_str);
	     fprintf(fp,"\"/>\r\n");
	}
     }
  }
  fprintf(fp,"</g>\r\n");
  fprintf(fp,"/r/n");

  //Draw the major horizontal lines and ticks here
  fprintf(fp,"<g transform = \"translate(2,2)\" stroke=\"#FF5F5F\" stroke-width=\"1\">\r\n");
  for (y_pos = 0; y_pos < max_y; y_pos+= 250)
  {
     fprintf(fp,"<line x1=\"0\" y1=\"");
     itoa(y_pos+y_offset, y_str,10);
     fprintf(fp,"%s",y_str);
     fprintf(fp,"\" x2=\"1023\" y2=\"");
     fprintf(fp,"%s",y_str);
     fprintf(fp,"\"/>\r\n");

     //draw the Major ticks here
     itoa(y_pos+y_offset-3, y_str,10);
     itoa(y_pos+y_offset+3, str_2,10);
     for (x_pos = 0; x_pos < max_x-x_offset; x_pos += 10)
     {
	  itoa(x_pos+x_offset, x_str,10);
	  fprintf(fp,"<line x1=\"");
	  fprintf(fp,"%s",x_str);
	  fprintf(fp,"\" y1=\"");
	  fprintf(fp,"%s",y_str);
	  fprintf(fp,"\" x2=\"");
	  fprintf(fp,"%s",x_str);
	  fprintf(fp,"\" y2=\"");
	  fprintf(fp,"%s",str_2);
	  fprintf(fp,"\"/>\r\n");
     }
   }

  fprintf(fp,"</g>\r\n");
  fprintf(fp,"\r\n");



}




//***************************************************************************************
//*
//*       Open a unique file name
//*
//*           Receives: VOID
//*           Returns: a pointer to the file handle
//*
//***************************************************************************************
FILE* openFile(char *filePrefix)
{
    // opens a file name in the scheme of "TEKnn.svg"
    // where nn is in the range of "00" to "99"
    // returns the file handle of the file
    // or returns NULL for an ERROR (invalid name or insufficient space on the drive, etc)

    //Consider passing the prefix so that the path is able to be modified by the main program

    char fileName[20];
    //char prefix[] = "TEK";
    char midName[10];
    char suffix[] = ".svg";	//NOTE: IMPORTANT make this ".svg" in SMALL LETTERS!!
    unsigned char i;
    FILE *fd;
    long length;

    //attempt to open a file name for reading
    //if the attempt fails then the file does not exist and
    //the name is available for writing a new file.
    //NOTE: pre-existing zero length files are also
    //considered available.

    length = 0;
    fd = NULL;
    i=0;
    do {
	if (fd) fclose(fd); //close an open file from previous iteration.
	// start with the prefix which is the path
	strcpy(fileName,filePrefix);
	sprintf(midName, "TEK%02d%s",i,suffix);
//printf("combining <%s> <%02d> <%s>\r\n",fileName,i,suffix);
	strcat(fileName,midName);
//printf("to make <%s>\r\n",fileName);
	//open a file for reading, binary mode
	fd = fopen(fileName,"rb");
	//NULL file handle probably means the file
	//does not exist and is OK to use.
	//NULL could also indicate an error.
	if (fd!= NULL)	//check for zero file length: OK to use
	{
	    fseek(fd,0L,SEEK_END);
	    length = ftell(fd);
	    if (length == 0)
	    {
		fclose(fd);
		fd = NULL; //make the handle NULL to indicate a usable file name
	    }
	}
	i++;
    } while ((fd!=NULL) && (i<100));

    if (fd!=NULL) //only happens if the iteration limit is exceeded
    {
	fclose(fd);
	fd = NULL;
	return fd; //no valid handle available
    }

    //now open the fileName for writing, binary mode (no character translation)
    fd = fopen(fileName,"wb");
    return fd;	//returns NULL if there was an error opening it
}

