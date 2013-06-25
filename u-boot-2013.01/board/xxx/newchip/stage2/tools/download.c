#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __CYGWIN__
#define _WIN32
#endif
#ifdef _WIN32
#include <windows.h>
#include "getopt.h"
#endif
#ifdef __linux__
#include <sys/ioctl.h>
#include <sys/timeb.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif


//****************************************************************************
//
// lSerialPort is the serial port which is being used.
//
//****************************************************************************
#ifdef _WIN32
HANDLE hSerialPort;
#endif
#ifdef __linux__
int lSerialPort;
#endif

//****************************************************************************
//
// OpenPort opens the specified serial port.
//
//****************************************************************************
int
OpenPort(int lPort,int bUSBPort)
{
#ifdef _WIN32
    char pcName[16];

    //
    // Create the device name for the given serial port.
    //
    sprintf(pcName, "COM%d", lPort);

    //
    // Open the serial port.
    //
    hSerialPort = CreateFile(pcName, GENERIC_READ | GENERIC_WRITE, 0, 0,
                             OPEN_EXISTING, 0, 0);
    if(hSerialPort == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Could not open serial port %s.\n", pcName);
        return(0);
    }

    //
    // Success.
    //
    return(1);
#elif defined(__linux__)
    char pcName[16], pcLock[32], pcBuffer[64];
    int lLock, lCount, lPid;
    struct termios buf;

    //
    // Create the file name for the lock file.
    //
    //sprintf(pcLock, "/var/lock/LCK..ttyS%d", lPort - 1);
    if(bUSBPort==0)
    {
    	sprintf(pcLock, "/var/lock/LCK..ttyS%d", lPort - 1);
    }
    else if(bUSBPort==1)
    {
    	sprintf(pcLock, "/var/lock/LCK..ttyUSB%d", lPort);	
    }
    
    //
    // Attempt to open the lock file.
    //
    lLock = open(pcLock, O_RDONLY);
    if(lLock)
    {
        //
        // The lock file exists, so read from it and then close it.
        //
        lCount = read(lLock, pcBuffer, 63);
        close(lLock);

        //
        // See if any data was read from the lock file.
        //
        if(lCount > 0)
        {
            //
            // The PID is currently unknown, so set it to -1.
            //
            lPid = -1;

            //
            // See if the lock file contained only four bytes, indicating a
            // Kermit-style lock file.
            //
            if(lCount == 4)
            {
                //
                // Extract the PID from the lock file data.
                //
                lPid = *(long *)pcBuffer;
            }

            //
            // Otherwise, the lock file is in ASCII.
            //
            else
            {
                //
                // Make sure that the string is NULL terminated.
                //
                pcBuffer[lCount] = 0;

                //
                // Convert the PID from ASCII to an integer.
                //
                sscanf(pcBuffer, "%d", &lPid);
            }

            //
            // If a PID was found, see if the process still exists.
            //
            if((lPid > 0) && (kill((pid_t)lPid, 0) < 0) && (errno == ESRCH))
            {
                //
                // The process no longer exists, meaning that the lock is
                // stale.  Delete the lock file.
                //
                unlink(pcLock);
            }
            else
            {
                //
                // The process still exists, so the port is locked.
                //
                lCount = 0;
            }
        }

        //
        // See if the port is locked.
        //
        if(lCount == 0)
        {
            //
            // Indicate that the port is locked.
            //
            if(bUSBPort==0)
    	    {
    		fprintf(stderr, "Serial port /dev/ttyS%d is locked!\n", lPort - 1);
    	    }
    	    else if(bUSBPort==1)
    	    {
    		fprintf(stderr, "Serial port /dev/ttyUSB%d is locked!\n", lPort);	
    	    }
            //fprintf(stderr, "Serial port /dev/ttyS%d is locked!\n", lPort - 1);
            return(0);
        }
    }

    //
    // Create the lock file.
    //
    lLock = creat(pcLock, 0666);
    if(lLock >= 0)
    {
        //
        // Print our PID to the lock file.
        //
        sprintf(pcBuffer, "%ld\n", (long)getpid());
        write(lLock, pcBuffer, strlen(pcBuffer));
        close(lLock);
    }

    //
    // Create the device name for the given serial port.
    //
    //sprintf(pcName, "/dev/ttyS%d", lPort - 1);
    if(bUSBPort==0)
    {
    	sprintf(pcName, "/dev/ttyS%d", lPort - 1);
    }
    else if(bUSBPort==1)
    {
    	sprintf(pcName, "/dev/ttyUSB%d", lPort);	
    }
    
    //
    // Open the serial port.
    //
    lSerialPort = open(pcName, O_RDWR | O_NONBLOCK);
    if(lSerialPort < 0)
    {
        unlink(pcLock);
        //fprintf(stderr, "Could not open serial port %s.\n", pcName);
        if(bUSBPort==0)
        {
    	    fprintf(stderr, "Could not open normal serial port %s.\n", pcName);
        }
        else if(bUSBPort==1)
        {
    	    fprintf(stderr, "Could not open USB serial port %s.\n", pcName);	
        }
                
        return(0);
    }

    //
    // Get the attributes of the serial port.  This will fail if the serial
    // port does not exist (even though the open will succeed if the serial
    // port does not exist).
    //
    if(tcgetattr(lSerialPort, &buf))
    {
        unlink(pcLock);
        fprintf(stderr, "Could not open serial port %s.\n", pcName);
        return(0);
    }

    //
    // Success.
    //
    return(1);
#else
    //
    // There is no serial port support for the target environment, so return
    // an error.
    //
    fprintf(stderr, "No serial port support!\n");
    return(0);
#endif
}

//****************************************************************************
//
// ClosePort closes the currently opened serial port.
//
//****************************************************************************
void
ClosePort(int lPort,int bUSBPort)
{
#ifdef _WIN32
    //
    // Close the serial port.
    //
    CloseHandle(hSerialPort);
#endif
#ifdef __linux__
    char pcLock[32];

    //
    // Close the serial port.
    //
    close(lSerialPort);

    //
    // Create the file name for the lock file.
    //
    if(bUSBPort==0)
    {
    	sprintf(pcLock, "/var/lock/LCK..ttyS%d", lPort - 1);
    }
    else if(bUSBPort==1)
    {
    	sprintf(pcLock, "/var/lock/LCK..ttyUSB%d", lPort);
    }
    //
    // Delete the lock file.
    //
    unlink(pcLock);
#endif
}

//****************************************************************************
//
// SetBaud sets the baud rate and data format of the serial port.
//
//****************************************************************************
void
SetBaud(long lRate)
{
#ifdef _WIN32
    DCB dcb;

    //
    // Purge any pending characters in the serial port.
    //
    PurgeComm(hSerialPort, (PURGE_TXABORT | PURGE_RXABORT |
                            PURGE_TXCLEAR | PURGE_RXCLEAR));

    //
    // Fill in the device control block.
    //
    dcb.DCBlength = sizeof(DCB);
    dcb.BaudRate = lRate;
    dcb.fBinary = TRUE;
    dcb.fParity = FALSE;
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fDsrSensitivity = FALSE;
    dcb.fTXContinueOnXoff = TRUE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    dcb.fErrorChar = FALSE;
    dcb.fNull = FALSE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcb.fAbortOnError = FALSE;
    dcb.XonLim = 0;
    dcb.XoffLim = 0;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.XonChar = 17;
    dcb.XoffChar = 19;
    dcb.ErrorChar = 0;
    dcb.EofChar = 0;
    dcb.EvtChar = 0;
    dcb.wReserved = 0;

    //
    // Set the new serial port configuration.
    //
    SetCommState(hSerialPort, &dcb);
#endif
#ifdef __linux__
    struct termios buf;
    int rate;

    //
    // Get the current settings for the serial port.
    //
    if(tcgetattr(lSerialPort, &buf))
    {
        return;
    }

    //
    // Reset to the serial port to raw mode.
    //
    buf.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP |
                     INLCR | IGNCR | ICRNL | IXON);
    buf.c_oflag &= ~OPOST;
    buf.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    buf.c_cflag &= ~(CSIZE | PARENB);
    buf.c_cflag |= CS8;

    //
    // Get the baud rate.
    //
    switch(lRate)
    {
        case 9600:
        {
            rate = B9600;
            break;
        }

        case 19200:
        {
            rate = B19200;
            break;
        }

        case 38400:
        {
            rate = B38400;
            break;
        }

        case 57600:
        {
            rate = B57600;
            break;
        }

        case 115200:
        {
            rate = B115200;
            break;
        }
    }

    //
    // Set the input and output baud rate of the serial port.
    //
    cfsetispeed(&buf, rate);
    cfsetospeed(&buf, rate);

    //
    // Set data bits to 8.
    //
    buf.c_cflag &= ~CSIZE;
    buf.c_cflag |= CS8;

    //
    // Set stop bits to one.
    //
    buf.c_cflag &= ~CSTOPB;

    //
    // Disable parity.
    //
    buf.c_cflag &= ~(PARENB | PARODD);

    //
    // Set the new settings for the serial port.
    //
    if(tcsetattr(lSerialPort, TCSADRAIN, &buf))
    {
	return;
    }

    //
    // Wait until the output buffer is empty.
    //
    tcdrain(lSerialPort);
#endif
}

//****************************************************************************
//
// SendChar sends a character to the serial port.
//
//****************************************************************************
void
SendChar(char cChar)
{
#ifdef _WIN32
    DWORD dwLen;

    //
    // Send this character to the serial port.
    //
    WriteFile(hSerialPort, &cChar, 1, &dwLen, NULL);
#endif
#ifdef __linux__
    //
    // Send this character to the serial port.
    //
    write(lSerialPort, &cChar, 1);
#endif
}

void
SendInt(int i)
{
#ifdef _WIN32
    DWORD dwLen;

    //
    // Send this character to the serial port.
    //
    WriteFile(hSerialPort, &i, 4, &dwLen, NULL);
#endif
#ifdef __linux__
    //
    // Send this character to the serial port.
    //
    write(lSerialPort, &i, 4);
#endif
}


//****************************************************************************
//
// ReceiveChar reads a character from the serial port.
//
//****************************************************************************
char
ReceiveChar(long lTimeout)
{
#ifdef _WIN32
    COMMTIMEOUTS sTimeouts;
    char cChar;
    DWORD dwLen;

    //
    // Fill in the timeout structure based on the timeout requested for this
    // read.
    //
    sTimeouts.ReadIntervalTimeout = 0;
    sTimeouts.ReadTotalTimeoutMultiplier = 0;
    sTimeouts.ReadTotalTimeoutConstant = lTimeout;
    sTimeouts.WriteTotalTimeoutMultiplier = 0;
    sTimeouts.WriteTotalTimeoutConstant = 0;

    //
    // Set the timeout for this read.
    //
    SetCommTimeouts(hSerialPort, &sTimeouts);

    //
    // Read a character.
    //
    if(!ReadFile(hSerialPort, &cChar, 1, &dwLen, NULL))
    {
        //
        // The read failed, so set the read character to a NULL.
        //
        cChar = 0;
    }

    //
    // If we did not read a character, then set the character to NULL.
    //
    if(dwLen != 1)
    {
        cChar = 0;
    }

    //
    // Return the character we read.
    //
    return(cChar);
#endif
#ifdef __linux__
    struct timeb sTime;
    char cChar;
    int res;
    time_t tStart, tNow;

    //
    // Get the current time.
    //
    ftime(&sTime);
    tStart = (sTime.time * 1000) + sTime.millitm;

    //
    // Read the next character from the serial port.
    //
    while(1)
    {
        //
        // Try to read a character from the serial port.
        //
        res = read(lSerialPort, &cChar, 1);
        if(res == 1)
        {
            //
            // We read a character, so break out of the loop.
            //
            break;
        }

        //
        // Get the current time.
        //
        ftime(&sTime);
        tNow = (sTime.time * 1000) + sTime.millitm;

        //
        // See if the timeout has expired.
        //
        if(lTimeout && ((tStart + lTimeout) < tNow))
        {
            cChar = 0;
            break;
        }
    }

    //
    // Return the character read from the serial port.
    //
    return(cChar);
#endif
}

//****************************************************************************
//
// WaitTillEmpty waits until the serial port's output buffer is empty.
//
//****************************************************************************
void
WaitTillEmpty(void)
{
#ifdef _WIN32
    //
    // Wait for 10ms so the output buffer can drain.
    //
    Sleep(10);
#endif
#ifdef __linux__
    //
    // Wait until the output buffer is empty.
    //
    tcdrain(lSerialPort);
#endif
}

//****************************************************************************
//
// WaitFor waits until a specific character is read from the serial port.
//
//****************************************************************************
void
WaitFor(char cWaitChar)
{
    char cChar;

    //
    // Wait until we read a specific character from the serial port.
    //
    while(1)
    {
        //
        // Read a character.
        //
        cChar = ReceiveChar(0);

        //
        // Stop waiting if we received the character.
        //
        if(cChar == cWaitChar)
        {
            break;
        }
    }
}

//****************************************************************************
//
// Prints out a usage message for the program.
//
//****************************************************************************
void
Usage(void)
{
    fprintf(stdout, "\
Usage: download {-b <baud>} {-h} {-p <port>} {-v} {-r} <filename>\n\
\n\
  -b <baud>     Use the specified baud rate (default is \"115200\"). Valid \n\
                values are 9600, 19200, 38400, 57600, and 115200.\n\
\n\
  -h            Prints this usage message.\n\
\n\
  -p <port>     Use the specified serial port.\n\
		Append the specified port string to the serial port prefix,\n\
		/dev/tty for Linux and COM for Windows.\n");
#ifdef _WIN32
    fprintf(stdout, "\
                For Windows, default is \"1\" which implies COM1.\n\
                Valid values are 1, 2, 3, 4, 5, 6,...etc.\n");
#endif
#ifdef __linux__
    fprintf(stdout, "\
                For Linux, default is \"S0\" which implies /dev/ttyS0.\n\
                Valid values are S0, S1,...USB0,USB1,...etc.\n");
#endif
    fprintf(stdout,"\
  -r		Run the code after download.\n\
  -v            Prints the version of this program.\n\
\n\
  filename	The name of the file to be download.\n");
}

static void *load_file(const char *fn, unsigned *_sz)
{
    char *data;
    int sz;
    int fd;

    data = 0;
    fd = open(fn, O_RDONLY);
    if(fd < 0) return 0;

    sz = lseek(fd, 0, SEEK_END);
    if(sz < 0) goto oops;

    if(lseek(fd, 0, SEEK_SET) != 0) goto oops;

    data = (char*) malloc(sz);
    if(data == 0) goto oops;

    if(read(fd, data, sz) != sz) goto oops;
    close(fd);

    if(_sz) *_sz = sz;
    return data;

oops:
    close(fd);
    if(data != 0) free(data);
    return 0;
}

typedef struct {
	char	     tag[4];    // 'XFUN'
	unsigned int offset;	//
	unsigned int entry; 	//where to place 
	unsigned int size;	 //size of binary 
	unsigned int loader_cksum; //chsum of binary
	unsigned int header_cksum; //cksum of first 16 bytes of header
}xl_header;

#define XL_HEAD_SIZE (sizeof(xl_header))
#define E_OK        ('O' + 'K' * 256)
#define E_CHECKSUM  ('E' + '1' * 256)
#define E_BADCMD    ('E' + '2' * 256)
#define E_TIMEOUT   ('E' + '3' * 256)
#define E_DEBUG     ('D' + 'B' * 256)

#define XCMD_CONNECT      ('H' + 'I' * 256)
#define XCMD_SET_BAUD     ('S' + 'B' * 256)
#define XCMD_DOWNLOAD     ('D' + 'L' * 256)
#define XCMD_UPLOAD       ('U' + 'L' * 256)
#define XCMD_SOC_MEMSET   ('M' + 'S' * 256)
#define XCMD_SOC_CALL     ('B' + 'L' * 256)

int WaitReply(unsigned short r, int ms)
{
	char c0,c1;
	c0 = ReceiveChar(ms);
	if(c0 == 0)
	{
		printf("wait replay timeout!");
		return 0;
	}
	c1 = ReceiveChar(ms);
	if(c1 == 0)
	{
		printf("wait replay timeout!");
		return 0;
	}
	if(r == (c0+c1*256))
		return 1;
	else
		printf("replay is %c%c\n",c0,c1);
	return 0;
	
}

int
main(int argc, char *argv[])
{
    int lPort = 1,lUSBPort=10, lRate = 115200;
    int lFileSize,lIdx;
    char cChar, cFirstChar, cRateChar,cBuffer[1024],  *pcString,PortString[30];
    int bError = 0, bHaveFile = 0, bRun = 0;
    int bUSBPort = 0;
    unsigned char bBuffer[1024];
    char *fcData;
    xl_header *hdr;

    //
    // First, set stdout to be unbuffered, so that our status messages are
    // always displayed immediately.
    //
    setbuf(stdout, NULL);

    //
    // Prevent getopt from printing out any errors.
    //
    opterr = 0;

    //
    // See if there are any flags specified on the command line.
    //
    while((cChar = getopt(argc, argv, "b:p:rhv")) != -1)
    {
    	
        //
        // Determine which flag was specified.
        //
        switch(cChar)
        {
            //
            // See if this argument is "-b".
            //
            case 'b':
            {
                //
                // Get the baud rate from the command line.
                //
                lRate = atoi(optarg);

                //
                // Make sure that the specified baud rate is valid.
                //
                if((lRate != 9600) && (lRate != 19200) && (lRate != 38400) &&
                   (lRate != 57600) && (lRate != 115200))
                {
                    //
                    // Tell the user that the baud rate is invalid.
                    //
                    fprintf(stderr, "Invalid baud rate '%s'.\n\n", optarg);

                    //
                    // Print the usage message.
                    //
                    Usage();

                    //
                    // We're done.
                    //
                    return(1);
                }

                //
                // We're done handling this argument.
                //
                break;
            }

            //
            // See if this argument is "-h".
            //
            case 'h':
            {
                //
                // Print the usage message.
                //
                Usage();

                //
                // We're done.
                //
                return(1);
            }
            //
            // See if this argument is "-p".
            //
            case 'p':
            {
                //
                // Get the port number from the command line.
                //
                lPort = atoi(optarg);

                if(lPort==0)
                {
                    //lUSBPort=strtol(optarg,&PortString,1);
                    strcpy(PortString,optarg);
                    if((strlen(PortString)==4)&&
                       ((PortString[0]=='u')||(PortString[0]=='U'))&&
                       ((PortString[1]=='s')||(PortString[1]=='S'))&&
                       ((PortString[2]=='b')||(PortString[2]=='B'))
                       )
                    {
                    	//lUSBPort = atoi(PortString);
                    	if((PortString[3]<='9')&&(PortString[3]>='0'))
                    	{
                    	    lUSBPort=PortString[3]-'0';
                    	    bUSBPort=1;
                    	    fprintf(stderr, "Using USB Serial Port %d\n",lUSBPort);
                    	}
                    	else
                    	{
                    	    lUSBPort=10;
                    	    
                    	}
                    }
                    else if((strlen(PortString)==2)&&
                       ((PortString[0]=='s')||(PortString[0]=='S'))
                       )
                    {
                    	//lUSBPort = atoi(PortString);
                    	if((PortString[1]<='9')&&(PortString[1]>='0'))
                    	{
                    	    lPort=PortString[1]-'0';
                    	    lPort=lPort+1;
                    	    fprintf(stderr, "Using Serial Port %d\n",lPort);
                    	}
                    	else
                    	{
                    	    lPort=0;
                    	    lUSBPort=10;
                    	}
                    }
                    else
                    {
                    	lUSBPort=10;
                    	lPort=0;
                    }
                    
                }
		else
		{
		    fprintf(stderr, "Using Serial Port %d\n",lPort);
		}
                //
                // Make sure that the specified port number is valid.
                //
                if((lPort != 1) && (lPort != 2) && (lPort != 3) &&
                   (lPort != 4) && (lPort != 5) && (lPort != 6) &&
                   (lPort != 7) && (lPort != 8) && (lPort != 9) &&
                   (lUSBPort != 1) && (lUSBPort != 2) && (lUSBPort != 3) &&
                   (lUSBPort != 4) && (lUSBPort != 5) && (lUSBPort != 6) &&
                   (lUSBPort != 7) && (lUSBPort != 8) && (lUSBPort != 9) &&
                   (lUSBPort != 0) && (lPort != 10)                  
                   )
                {
                    //
                    // Tell the user that the port number is invalid.
                    //
                    fprintf(stderr, "Invalid serial port '%s'.\n\n", optarg);

                    //
                    // Print the usage message.
                    //
                    Usage();

                    //
                    // We're done.
                    //
                    return(1);
                }
		
		if(bUSBPort==1)
		{
		    lPort=lUSBPort;
		}
                //
                // We're done handling this argument.
                //
                break;
            }
	    case 'r':
	    {
		bRun = 1;
		break;
            }
            //
            // See if this argument is "-v".
            //
            case 'v':
            {
                //
                // Print the version of this program.
                //
                printf("Version 1.0 @");
#ifdef _WIN32
                printf("Win32.\n");
#endif
#ifdef __linux__
                printf("Linux.\n");
#endif
                //
                // We're done.
                //
                return(0);
            }

            //
            // An unrecognized flag was specifed.
            //
            default:
            {
                    //
                    // Print the usage message.
                    //
                    Usage();

                    //
                    // We're done.
                    //
                    return(1);
            }
        }

    }

    //
    // See if a filename was specified.
    //
    bHaveFile = ((optind + 1) == argc);

    //
    // Open the serial port to be used.
    //
    if(OpenPort(lPort,bUSBPort) != 1)
    {
        return(1);
    }

    //
    // See if there is a file.
    //
    if(bHaveFile)
    {
        //
        // Open the file to be downloaded.
        //
	fcData = load_file(argv[argc-1],&lFileSize);
        if(!fcData)
        {
            fprintf(stderr, "Could not open file '%s'.\n", argv[argc - 1]);
            ClosePort(lPort,bUSBPort);
            return(1);
        }
	hdr = (xl_header *)fcData;
	printf("file size is %d\n",lFileSize);	
	printf("header:tag = 0x%x\n", *(unsigned int*)(hdr->tag));
	printf("header:offset = 0x%x\n", hdr->offset);
	printf("header:entry = 0x%x\n", hdr->entry);
	printf("header:size = 0x%x\n", hdr->size);
	printf("header:loader_cksum = 0x%x\n", hdr->loader_cksum);
	printf("header:header_cksum = 0x%x\n", hdr->header_cksum);
    }
    SetBaud(115200);

    printf("Connecting to the board...\n");
    //cmd HI
    SendChar('H');
    SendChar('I');
    WaitTillEmpty();
    if(!WaitReply(E_OK,1000))
    {
	printf("Connect to the board timeout!\n");
	bError = 1;
	goto ERROR_RETURN;
    }
    printf("Conneced to the board OK.\n");

    if(bHaveFile)
    {
	unsigned char *code = fcData + hdr->offset;
	    printf("\r                                  \r");
	    printf("Downloading the code...(  0%%)");
	    //cmd DL
	    SendChar('D');
	    SendChar('L');
	    //addr
	    SendInt(hdr->entry);
	    //size
	    SendInt(hdr->size);
	    //cksum
	    SendInt(hdr->loader_cksum);
	    WaitTillEmpty();
	    for(lIdx = 0; lIdx < hdr->size; lIdx++)
	    {
		    //
		    // Write this character.
		    //
		    SendChar(code[lIdx]);

		    //
		    // Periodically print out our progress.
		    //
		    //if((lIdx & 127) == 127)
		    if((lIdx & 15) == 15)
		    {
			    //
			    // Wait until the transmit buffer is empty.
			    //
			    WaitTillEmpty();

			    //
			    // Print the updated status.
			    //
			    printf("\b\b\b\b\b%3d%%)", ((lIdx + 1) * 100) / hdr->size);
		    }
	    }
	    printf("\r                              \r");
	    WaitTillEmpty();
	    if(!WaitReply(E_OK,5000))
	    {
		    printf("download file to the board timeout!\n");
		    bError = 1;
		    goto ERROR_RETURN;
	    }
	    //skip the debug output.
	    while((cChar = ReceiveChar(10)) != 0)
	    {
		printf("%c",cChar);
	    }
	    if(bRun)
	    {
		    //cmd call
		    SendChar('B');
		    SendChar('L');
		    SendInt(hdr->entry);
		    WaitTillEmpty();
		    if(!WaitReply(E_OK,5000))
		    {
			    printf("send run cmd to the board timeout!\n");
			    bError = 1;
			    goto ERROR_RETURN;
		    }
		    while((cChar = ReceiveChar(1000)) != 0)
		    {
			    printf("%c",cChar);
		    }
	    }
    }

    //
    //WaitFor('<');

ERROR_RETURN:    
    //
    // Close the file.
    //
    if(bHaveFile)
    {
	    free(fcData);
    }

    //
    // Close the serial port.
    //
    ClosePort(lPort,bUSBPort);

    //
    // Tell the user we are done.
    //
    if(!bError)
    {
	    return(0);
    }
    else
    {
	    return(1);
    }

}
