
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <bios.h>

#include "decl.h"
#include "TEKDECL.H"

#define HIDE 0	//status of menu display on scope
#define SHOW 1



void gpiberr(char *msg);
void dvmerr(char *msg, char *rd);
//  extern FILE *fp;

//  char     rd[512];                      /* read data buffer                */
  // msg, msg2 are multi-use string buffers.
  // DO NOT ASSUME that the buffer contains any specific
  // pre-defined characters!
  char	   msg[128];
  char	   msg2[128];
  struct   driveInfoStruct
	   {
	       char driveName[4][10];
	       unsigned char numDrives;
	       unsigned char activeDrive;
	   };
  struct driveInfoStruct driveInfo;
  unsigned char activeDrive = 1;

  const char     cont[] =  "\x3F\x5F\x40\x21\x0";
  const char     scopt[] = "\x3F\x5F\x41\x20\x0";
  const char     query[] = "EVE?";
  unsigned char	   menu = 0;	//the number of the active menu
  unsigned char	   maxMenu;     //the maximum number of menus
  unsigned char    menuItem = 0;	//the number of the active entry in the active menu
  //TODO: Make menus dynamically allocated
 // char         	menu1Text[10][128];   //menu 1 has 10 entries, up to 128 characters each
  //unsigned int 	menuLen[10];	//length of each menu entry
  //char		menuHeader[] =	"MESSAGE 12:\"---\\m\\e\\n\\u \\s\\y\\s\\t\\e\\m---\"";	//the unchanging header of the menus
//  const char	menuFooter2[] =	"MESSAGE 2:\"  SAVE            SAVE    SAVE  TOGGLE\"";	//the unchanging footer of the menu system
  const char	menuFooter1[] =	"MESSAGE 1:\"  SAVE   DRIVE    NULL    NULL    HIDE\"";	//the unchanging footer of the menu system
//  const char	menuFooter1[] =	"MESSAGE 1:\"  NEXT   SELECT   3rdx    4thx    hide\"";	//the unchanging footer of the menu system
  const char	svgMesStart[] = "MESSAGE 5:\"     \\*\\*\\*\\s\\a\\v\\i\\n\\g\\0\\g\\r\\a\\p\\h\\i\\c\\s\\0\\t\\o\\0\\d\\i\\s\\k\\*\\*\\*\"";
  const char	svgMesEnd1[] = "MESSAGE 5:\"             \\s\\a\\v\\e\\0\\c\\o\\m\\p\\l\\e\\t\\e\"";
  const char	svgMesEnd2[] = "MESSAGE 5:\"\"";
  unsigned char displayStatus = SHOW;
  int      	evnt;
  int      	bd,       /* board or device number          */
		m;        /* FOR loop counter                */


void main() {


    //printf("Test the scope interface\n");
    //printf("\n");

    //strcpy(cont,"\x3F\x5F\x40\x21");
    //strcpy(scopt,"\x3F\x5F\x41\x20");
    initializeVars();

/*
 *  Assign a unique identifier to board 0 and store in the variable BD.
 *  The name 'GPIB0' is the default name of board 0.  If BD is less
 *  than zero, call GPIBERR with an error message.
 */

    bd = ibfind ("GPIB0");
    //if (ibsta & ERR) gpiberr("ibfind Error");
    if (bd<0) {
	printf("GPIB not found\r\n");
	exit(0);
    }
    else {
	//printf("GPIB found\r\n");
    }

/*
 *  Send the Interface Clear (IFC) message.  This action initializes the
 *  GPIB interface board and makes the interface board Controller-In-Charge.
 *  If the error bit ERR is set in IBSTA, call GPIBERR with an error
 *  message.
 */

    ibsic (bd);
    if (ibsta & ERR) gpiberr("ibsic Error");
    //else printf("IFC processed OK\r\n");

/*
 *  Turn on the Remote Enable (REN) signal.  The device does not actually
 *  enter remote mode until it receives its listen address.  If the
 *  error bit ERR is set in IBSTA, call GPIBERR with an error message.
 */

    ibsre (bd,1);
    if (ibsta & ERR) gpiberr("ibsre Error");
    //else printf("REN signal processed OK\r\n");

/*
 *  Inhibit front panel control with the Local Lockout (LLO) command
 *  (hex 11).  Place the Fluke 45 in remote mode by addressing it to listen
 *  (hex 21 or ASCII "!").  Send the Device Clear (DCL) message to clear
 *  internal device functions (hex 14).  Address the GPIB interface board to
 *  talk (hex 20 or ASCII "@").  These commands can be found in Appendix A of
 *  the Software Reference Manual.  If the error bit ERR is set in IBSTA,
 *  call GPIBERR with an error message.
 */
 // /021octal = 0x11 = 17d = LLO
 // "!" = listen address #1 = 33D = 0x21 = /041octal = scope listen command
 // 024octal = 0x14 = 20d = DCL (device clear)
 // "@" = Talk address #0 = 64d = 40H = /100octal = board talk command

   //ibcmd (bd,"\021!\024@",4L);
   ibcmd(bd,"!\024@",3L);	//no local lockout
    if (ibsta & ERR) gpiberr("ibcmd Error");
    //else printf("scope told to listen, device cleared, board told to talk\r\n");





    // DO INITIAL STUFF
	createMenu();
	introScreen();
	showMenu(0);




/*
*	Tell the scope to read the control buttons and check the status
*/

	//strcpy(msg,"PATH OFF; EVENT?");

	//printf("msg is: ");
	//printf("\"%s\", len: %d\r\n",msg, strlen(msg));
	//printf("cont is: ");
	//printf("\"%s\", len: %d\r\n",cont, strlen(cont));
	//printf("scopt is:");
	//printf("\"%s\"\r\n",scopt);

	//clear pending events
	do
	{
	   evnt = getEvent();
	   //printf("clear event %d\r\n",evnt);
	} while (evnt != 0);

	do {
	  do {
	     evnt = getEvent();
// break loop if event is 450<= event<=454 or event = 652
// 450..454 is a menu button press, 652 is a clear of the scope internal menu line
// if the scope's own menu is cleared, then the user menu needs to be reprinted
	  } while (!((evnt >=450 && evnt <=454)|| evnt==652));//check for one of the 5 menu buttons
	  processEvent(evnt);
	} while (TRUE); //loop forever


}


//********************************************************
//*                                                      *
//*          Custom Functions                            *
//*                                                      *
//********************************************************



//***************************************************************************************
//*
//*       initializeVars: intialize settings here
//*
//*           Receives: VOID
//*           Returns: VOID
//*
//***************************************************************************************
void initializeVars(void)
{
//unsigned int cnt;
    // INITIALIZE DRIVEINFO STRUCTURE
    // in C++ driveInfo would be a very convenient object to use!
    driveInfo.numDrives = 3;
    driveInfo.activeDrive = 1;
    // activeDrive corresponds to the DOS drive number for the
    // space available interrupt.
    // driveName[0] is not used: in DOS, ZERO is the default drive,
    // whatever that may be at the time.  I don't bother to check it
    strcpy(driveInfo.driveName[1],"A:");
    strcpy(driveInfo.driveName[2],"B:");
    strcpy(driveInfo.driveName[3],"C:");
//for (cnt = 1; cnt<driveInfo.numDrives+1; cnt++) {
//  printf("drive %d is <%s>\r\n",cnt,driveInfo.driveName[cnt]);
//}
   //
}


//***************************************************************************************
//*
//*       createMenu: in the event of having a variety of
//*                   menus available for user interratcion
//*                   the menu text would be created here
//*       **Currently this does nothing**
//*
//*           Receives: VOID
//*           Returns: VOID
//*
//***************************************************************************************

void createMenu(void)
{
     //ibcmd(bd, (char *)cont, strlen(cont));
     //strcpy(menu1Text[0],"MESSAGE 5:\"menu line 5\"");
     //menuLen[0] = strlen(menu1Text[0]);
     //printf("%s\r\n",menu1Text[0]);
     //printf("Length: %d\r\n",menuLen[0]);
}



//***************************************************************************************
//*
//*       ShowMenu: displays the active menu on the scope
//*         **Currently this only shows the menu for the button actions**
//*
//*           Receives: the number of the active menu (not yet written)
//*           Returns: VOID
//*
//***************************************************************************************
void showMenu(unsigned char menuNum)
{
     //uses global variables: maxMenu, etc.
     //currently it does not actually USE the menuNum value

     ibcmd(bd, (char *)cont, strlen(cont));
     //printf("in showMenu function\r\n");
     if (menuNum > maxMenu) return;
     //ibwrt(bd,(char *)menuHeader,strlen(menuHeader));
     ibwrt(bd,(char *)menuFooter1,strlen(menuFooter1));
     //ibwrt(bd,(char *)menuFooter2,strlen(menuFooter2));
     //ibwrt(bd,menu1Text[0],menuLen[0]);
     menu = menuNum;
}



//***************************************************************************************
//*
//*       hideMenu: Hide/Unhide the menu on the scope
//*
//*           Receives: VOID
//*           Returns: VOID
//*
//***************************************************************************************
void hideMenu(void)
{
    char mess[80];

    ibcmd(bd, (char *)cont, strlen(cont));
//    printf("in hideMenu function, Status is: %d\r\n", displayStatus);
    if (displayStatus == SHOW)
    {
//	    printf("hiding menu\r\n");
	    displayStatus = HIDE;
	    strcpy(mess,"MESSAGE CLRSTATE; MENUOFF");
//	    printf("Message: %s, length: %d\r\n",mess, strlen(mess));
	    ibwrt(bd, mess, strlen(mess));
//          ibwrt(bd,"MESSAGE CLRSTATE",16);
    }
    else
    {
//	    printf("showing menu\r\n");
	    //change the menu
	    //strcpy(menu1Text[0],"MESSAGE 6:\"THIS LINE IS NEW\"");
	    //menuLen[0] = strlen(menu1Text[0]);
	    displayStatus = SHOW;
	    showMenu(menu);
    }

//    printf("hideMenu complete\r\n");
}




//***************************************************************************************
//*
//*       introScreen:  suspends scope functions and displays an intro screen on the scope
//*
//*           Receives: VOID
//*           Returns: VOID
//*
//***************************************************************************************
void introScreen(void)
{
    char mess[120]="";
    char dispInten[50]="";
    char trigMode[50] ="";
    char runMode[50]="";

    //*** read some basic settings
    //*** store them, and later restore them
    //*** This is to stop any acquisitions and blank the waveform display
    strcpy(mess,"PATH ON; INTENSITY? DISPLAY");
    ibcmd(bd, (char *)cont, strlen(cont));
    ibwrt(bd, mess, strlen(mess));
    ibcmd(bd, (char *)scopt, strlen(scopt));
    ibrd(bd,mess, 60);
    if (ibcnt >0)
    {
	mess[ibcnt] = 0;
	strcpy(dispInten,mess);
	//printf("dispInten: %s\r\n", dispInten);
    }

    strcpy(mess,"RUN?");
    ibcmd(bd, (char *)cont, strlen(cont));
    ibwrt(bd, mess, strlen(mess));
    ibcmd(bd, (char *)scopt, strlen(scopt));
    ibrd(bd, mess, 50);
    if (ibcnt> 0)
    {
	mess[ibcnt]=0;
	strcpy(runMode,mess);
	//printf("runMode: %s\r\n",runMode);
    }

    strcpy(mess,"ATR? MODE");
    ibcmd(bd, (char *)cont, strlen(cont));
    ibwrt(bd, mess, strlen(mess));
    ibcmd(bd, (char *)scopt, strlen(scopt));
    ibrd(bd, mess, 60);
    if(ibcnt>0)
    {
	mess[ibcnt]=0;
	strcpy(trigMode,mess);
	//printf("trig mode: %s\r\n",trigMode);
    }

    //printf("trigMode: %s\r\n",trigMode);
    //printf("characters received %d\r\n",ibcnt);

    //*** now send the intro screen ***
    ibcmd(bd, (char *)cont, strlen(cont));
    strcpy(mess, "INTENSITY DISP: 0; ATR MOD: SGL; MEN; MESS CLR");
    //printf("sending :\"%s\"\r\n",mess);
    ibwrt(bd, mess, strlen(mess));
    strcpy(mess, "READOUT ON; MESSAGE CLRSTATE; MENUOFF");
    //printf("sending:\"%s\"\r\n",mess);
    ibwrt(bd, mess, strlen(mess));
    // create underline with double backslash to place a single
    // backslash in the actual string, followed by a character which will
    // be translated by the scope into an underlined character.
    // A-Z are underlines simply with the preceeding backslash.
    // space and number use special characters preceeded by the backslash
    // this is documented in the 2430 or 2432 or 2440 programmers reference
    strcpy(mess, "MESSAGE 16:\"\\0\\0\\0\\t\\e\\k\\t\\r\\o\\n\\i\\x\\0\\B\\D\\@\\B\\a\\0\\g\\r\\a\\p\\h\\i\\c\\s\\0\\s\\o\\f\\t\\w\\a\\r\\e\\0\\0\\0\\0\"");
    //strcpy(mess, "MESSAGE 12:\"\\0\\0\\0\\g\\r\\aphics\"");
    ibwrt(bd, mess, strlen(mess));
    strcpy(mess, "MESSAGE 15:\"\\0\\0\\0\\0\\0\\c\\o\\p\\y\\r\\i\\g\\h\\t\\0\\B\\@\\A\\I\\0\\a\\n\\t\\h\\o\\n\\y\\0\\r\\h\\o\\d\\e\\s\\0\\0\\0\\0\\0\\0\"");
    //max length is 40 chars  "1234567890123456789012345678901234567890\"");
    ibwrt(bd, mess, strlen(mess));
    strcpy(mess, "MESSAGE 14:\"\"");
    ibwrt(bd, mess, strlen(mess));
    strcpy(mess, "MESSAGE 13:\" be SURE to have a formatted disk\"");
    ibwrt(bd, mess, strlen(mess));
    strcpy(mess, "MESSAGE 12:\" present in the selected drive\"");
    ibwrt(bd, mess, strlen(mess));
    strcpy(mess, "MESSAGE 11:\" without this it will appear to crash\"");
    ibwrt(bd, mess, strlen(mess));
    strcpy(mess, "MESSAGE 10:\" This will store all displayed traces\"");
    ibwrt(bd, mess, strlen(mess));
    strcpy(mess,  "MESSAGE 9:\" on the oscilloscope in an SVG type\"");
    ibwrt(bd, mess, strlen(mess));
    strcpy(mess,  "MESSAGE 8:\" graphics file\"");
    ibwrt(bd, mess, strlen(mess));
//    strcpy(mess, "MESSAGE 10:\"   crash,\"");
//    ibwrt(bd, mess, strlen(mess));
    delay(10000);	//delay 10 seconds... LESS?

    //*** restore everything to the way it was
    strcpy(mess,"PATH OFF;MESSAGE CLRSTATE; MENUOFF; READOUT ON");
    ibwrt(bd, mess, strlen(mess));
    //printf("Sending \"%s\"\r\n",dispInten);
    ibwrt(bd,dispInten,strlen(dispInten));
    //printf("sending: %s\r\n",trigMode);
    ibwrt(bd, trigMode, strlen(trigMode));
    //printf("sending %s\r\n",runMode);
    ibwrt(bd, runMode, strlen(runMode));
    //printf("exiting introScreen\r\n");
}




//***************************************************************************************
//*
//*       processEvent:  handles an event sent by the scope
//*			(processing may depend on the current active menu( not yet written))
//*
//*           Receives: Event number
//*           Returns: VOID
//*
//***************************************************************************************
void processEvent(int evnt)
{
    switch (evnt)
    {
	case 652:
	    displayStatus = HIDE;
	    //printf("case 652\r\n");
	    break;
	case 450:	//print the currently displayed traces
	    processButton1();
	    break;
	case 451:      // step through the destination drives
	    processButton2();
	    break;
	case 452:
	    //no current function assigned,
	    //but use it as a mechanism to exit for development
	    exit(0);
	    break;
	case 453:
	    //no current function assigned
	    break;
	case 454:
	    //hide or show the list of the functions of the buttons
	    hideMenu();
	    break;
    }
}




//***************************************************************************************
//*
//*       processButton1:  handles pressing button 1
//*
//*           Receives: void
//*           Returns: void
//*
//***************************************************************************************
void processButton1(void)
{
    int saveError;
    unsigned long free;

    ibcmd(bd, (char *)cont, strlen(cont));
    // worst case is maybe 170,031 bytes for 7 channels displayed:
    // a little over 8K per channel:  reserve 8 channels at ~10K
    // check if >180K is free
    free = getDiskFree(driveInfo.activeDrive);
    // warn of low disk space: space remains for one, maybe 2, more captures
    if ((free > 180000ul) && (free < 360000ul))
    {
	strcpy(msg,"MESSAGE 5:\"    ***\\d\\i\\s\\k\\0\\n\\e\\a\\r\\l\\y\\0\\f\\u\\l\\l***\";bel");
	ibwrt(bd, msg, strlen(msg));
	delay(2000);
	ibwrt(bd, (char *)svgMesEnd2, strlen(svgMesEnd2));
    }

    if (free > 180000ul)
    {
	saveError = 0;
	ibwrt(bd, (char *)svgMesStart, strlen(svgMesStart));
	saveError = saveSVG(driveInfo.driveName[driveInfo.activeDrive]);
	ibcmd(bd, (char *)cont, strlen(cont));
	if (saveError == 0)
	{
	    ibwrt(bd, (char *)svgMesEnd1, strlen(svgMesEnd1));
	    delay(2000);
	}
	ibwrt(bd, (char *)svgMesEnd2, strlen(svgMesEnd2));
	//printf("finished writing disk\r\n");
    }
    else saveError = 2;

    if (saveError >0)
    {
	ibcmd(bd, (char *)cont, strlen(cont));
	// if more than two errors need to be handled then use switch/case
	if (saveError == 1)  strcpy(msg,"MESSAGE 5:\"    ***\\f\\i\\l\\e\\0\\c\\r\\e\\a\\t\\i\\o\\n\\0\\e\\r\\r\\o\\r**\";bel");
	else strcpy(msg,"MESSAGE 5:\"    ***\\d\\i\\s\\k\\0\\t\\o\\o\\0\\f\\u\\l\\l***\";bel");
	ibwrt(bd, msg, strlen(msg));
	delay(4000);
	ibwrt(bd, (char *)svgMesEnd2, strlen(svgMesEnd2));
    }
}



//***************************************************************************************
//*
//*       processButton2:  handles pressing button 2
//*           It will increment the destination drive
//*           And wrap around
//*
//*           Receives: void
//*           Returns: void
//*
//***************************************************************************************
void processButton2(void)
{
    driveInfo.activeDrive++;
    if (driveInfo.activeDrive > driveInfo.numDrives) {
      driveInfo.activeDrive = 1;
    }
    strcpy(msg,"MESSAGE 5:\"    ACTIVE DRIVE: <");
    strcat(msg, driveInfo.driveName[driveInfo.activeDrive]);
    strcat(msg, ">\"");
//printf("%s\r\n",msg);
    ibcmd(bd, (char *)cont, strlen(cont));
    ibwrt(bd, msg, strlen(msg));
    delay(1500);
    ibwrt(bd, (char *)svgMesEnd2, strlen(svgMesEnd2));
}



//***************************************************************************************
//*
//*       getEvent:  reads event code from the scope
//*
//*           Receives: void
//*           Returns: int event number
//*
//***************************************************************************************
unsigned int getEvent(void)
{
	ibcmd(bd, (char *)cont, strlen(cont));
	ibwrt(bd, (char *)query, strlen(query));
	ibcmd(bd, (char *)scopt, strlen(scopt));
	ibrd(bd, msg2, 10);
	msg2[ibcnt] = 0; //add null to end of string in case it is not there
	return atoi(msg2);
}




//***************************************************************************************
//*
//*       getDiskFree:  reads event code from the scope
//*
//*           Receives: drive number (0=default, 1=A, 2=B...)
//*           Returns: unsigned long free bytes
//*
//***************************************************************************************
unsigned long getDiskFree(unsigned char drv)
{
     struct dfree dtab;
     //struct diskfree_t df;
     unsigned long avail;
     //unsigned int result;

     //result = absread(0,1,0,msg);
     //printf("absread result is %i\r\n",result);
     //printf("the first byte is 0x%x\r\n",msg[0]);

     getdfree(drv,&dtab);
     //printf("clusters avail: %u\r\n",dtab.df_avail);
     //printf("sectors/cluster: %u\r\n",dtab.df_sclus);
     //printf("bytes/sector: %d\r\n",dtab.df_bsec);
     avail = (unsigned long)dtab.df_avail * (unsigned long)dtab.df_sclus * (unsigned long)dtab.df_bsec;
     //printf("bytes available: %lu\r\n",avail);
     return avail;
/*
 printf("\r\nusing getdiskfree()\r\n");
 result = _dos_getdiskfree(1,&df);
 printf("clusters avail: %u\r\n",df.avail_clusters);
 printf("sectors/cluster: %u\r\n",df.sectors_per_cluster);
 printf("bytes/sector: %d\r\n",df.bytes_per_sector);
 avail = (unsigned long)df.avail_clusters * (unsigned long)df.sectors_per_cluster * (unsigned long)df.bytes_per_sector;
 //avail = (unsigned long)dtab.df_avail;
 printf("bytes available: %lu\r\n",avail);
 printf("result is: %i\r\n", result);
*/
 }




//****************************** Supplied Functions ****************

void gpiberr(char *msg) {

    printf ("%s\n", msg);

//    printf ("ibsta = &H%x  <", ibsta);
//    if (ibsta & ERR ) printf (" ERR");
//    if (ibsta & TIMO) printf (" TIMO");
//    if (ibsta & END ) printf (" END");
//    if (ibsta & SRQI) printf (" SRQI");
//    if (ibsta & RQS ) printf (" RQS");
//    if (ibsta & CMPL) printf (" CMPL");
//    if (ibsta & LOK ) printf (" LOK");
//    if (ibsta & REM ) printf (" REM");
//    if (ibsta & CIC ) printf (" CIC");
//    if (ibsta & ATN ) printf (" ATN");
//    if (ibsta & TACS) printf (" TACS");
//    if (ibsta & LACS) printf (" LACS");
//    if (ibsta & DTAS) printf (" DTAS");
//    if (ibsta & DCAS) printf (" DCAS");
//    printf (" >\n");

//    printf ("iberr = %d", iberr);
//    if (iberr == EDVR) printf (" EDVR <DOS Error>\n");
//    if (iberr == ECIC) printf (" ECIC <Not CIC>\n");
//    if (iberr == ENOL) printf (" ENOL <No Listener>\n");
//    if (iberr == EADR) printf (" EADR <Address error>\n");
//    if (iberr == EARG) printf (" EARG <Invalid argument>\n");
//    if (iberr == ESAC) printf (" ESAC <Not Sys Ctrlr>\n");
//    if (iberr == EABO) printf (" EABO <Op. aborted>\n");
//    if (iberr == ENEB) printf (" ENEB <No GPIB board>\n");
//    if (iberr == EOIP) printf (" EOIP <Async I/O in prg>\n");
//    if (iberr == ECAP) printf (" ECAP <No capability>\n");
//    if (iberr == EFSO) printf (" EFSO <File sys. error>\n");
//    if (iberr == EBUS) printf (" EBUS <Command error>\n");
//    if (iberr == ESTB) printf (" ESTB <Status byte lost>\n");
//    if (iberr == ESRQ) printf (" ESRQ <SRQ stuck on>\n");
//    if (iberr == ETAB) printf (" ETAB <Table Overflow>\n");

//    printf ("ibcnt = %d\n", ibcnt);
//    printf ("\n");

/*  Call the ibonl function to disable the hardware and software.           */

//    ibonl (bd,0);

//    exit(1);

}


/*
 * ===========================================================================
 *                     Function DVMERR
 *  This function will notify you that the Fluke 45 returned an invalid
 *  serial poll response byte.  The error message will be printed along with
 *  the serial poll response byte.
 *
 *  The NI-488 function IBONL is called to disable the hardware and software.
 *
 *  The EXIT function will terminate this program.
 * ===========================================================================
 */

void dvmerr(char *msg,char *rd) {

//    printf ("%s\n", msg);
//    printf("Status byte = %x\n", rd[0]);

/*  Call the ibonl function to disable the hardware and software.           */

//    ibonl (bd,0);

//    exit(1);
}


/*
//test the file functions
	printf("opening files....\r\n");
	fp = openFile();
	if (fp == NULL)
	{
		printf("first attempt to open a file failed\r\n");
		exit(0);
	}
	fputs("this is the first file to be written",fp);
	fclose(fp);

	fp = openFile();
	if (fp == NULL)
	{
		printf("second attempt to open a file failed\r\n");
		exit(0);
	}
	fputs("this is the second file to be written",fp);
	fclose(fp);
	printf("completed opening files\r\n");
	exit(0);
*/
