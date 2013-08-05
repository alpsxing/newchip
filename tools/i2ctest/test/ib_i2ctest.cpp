/**********************************************************************
 Filename: ib_mcuctl.cpp
 Description: Test program for MCU
 Author: Xing Qianqian
**********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "ib_test.h"
#include "ib_i2cctl.h"

IBI2CControl i2cControl;

static int CheckString(const char *value, int isHex)
{
    int start = 0;
    if(isHex)
    {
        start = 2;
    }
    
    for(; start < strlen(value); start ++)
    {
        if((value[start] >= '0') && (value[start] <= '9'))
        {
            continue;
        }
        
        if(((value[start] >= 'a') && (value[start] <= 'f')) ||
           ((value[start] >= 'A') && (value[start] <= 'F')))
        {
            if(isHex)
            {
                continue;
            }
            else
            {
                return -1;
            }
        }
        
        return -1;
    }
    
    return 0;
}

static int StrToByte(const char *value, unsigned char &result)
{
    unsigned int ret;
    int len = strlen(value);
    int i;
    int isHex = 0;
    
    if(len > 2)
    {
        if((value[0] == '0') && ((value[1] == 'x') || (value[1] == 'X')))
        {
            isHex = 1;
        }
    }
    
    if(CheckString(value, isHex) < 0)
    {
        return -1;
    }
    
    if(isHex)
    {
        ret = strtoul(&(value[2]), NULL, 16);
    }
    else
    {
        ret = strtoul(&(value[0]), NULL, 10);
    }
    
    if(ret > 0xFF)
    {
        return -1;
    }
    
    result = (unsigned char)ret;
    
    return 0;
}

static int StrToWord(const char *value, unsigned short &result)
{
    unsigned int ret;
    int len = strlen(value);
    int i;
    int isHex = 0;
    
    if(len > 2)
    {
        if((value[0] == '0') && ((value[1] == 'x') || (value[1] == 'X')))
        {
            isHex = 1;
        }
    }
    
    if(CheckString(value, isHex) < 0)
    {
        return -1;
    }
    
    if(isHex)
    {
        ret = strtoul(&(value[2]), NULL, 16);
    }
    else
    {
        ret = strtoul(&(value[0]), NULL, 10);
    }
    
    if(ret > 0xFFFF)
    {
        return -1;
    }
    
    result = (unsigned short)ret;
    
    return 0;
}

static int i2cgetbc(int argc, char **argv)
{
    unsigned char address, command;
    int result; 
    if(argc != 2)
    {
        fprintf(stderr, "Paras:\r\n");
        fprintf(stderr, "address: The i2c address.\r\n");
        fprintf(stderr, "command: The i2c command.\r\n");
        return -1;
    }
    
    if(StrToByte(argv[0], address) < 0)
    {
        fprintf(stderr, "Invalid address(%s).\r\n", argv[0]);
        return -1;
    }

    if(StrToByte(argv[1], command) < 0)
    {
        fprintf(stderr, "Invalid command(%s).\r\n", argv[1]);
        return -1;
    }
    
    fprintf(stderr, "Address=0x%02x, Command=0x%02x.\r\n", address, command);
    
    result = i2cControl.i2cget(address, command, 'b');
    if((result < 0) || (result > 0xFF))
    {
        fprintf(stderr, "Fail to execute.\r\n");
        return -1;
    }
    
    fprintf(stderr, "Result: 0x%02x\r\n", result);
    return 0;
}

static int i2cgetwc(int argc, char **argv)
{
    unsigned char address, command;
    int result;
    
    if(argc != 2)
    {
        fprintf(stderr, "Paras:\r\n");
        fprintf(stderr, "address: The i2c address.\r\n");
        fprintf(stderr, "command: The i2c command.\r\n");
        return -1;
    }
    
    if(StrToByte(argv[0], address) < 0)
    {
        fprintf(stderr, "Invalid address(%s).\r\n", argv[0]);
        return -1;
    }

    if(StrToByte(argv[1], command) < 0)
    {
        fprintf(stderr, "Invalid command(%s).\r\n", argv[1]);
        return -1;
    }
    
    fprintf(stderr, "Address=0x%02x, Command=0x%02x.\r\n", address, command);
    
    result = i2cControl.i2cget(address, command, 'w');
    if((result < 0) || (result > 0xFFFF))
    {
        fprintf(stderr, "Fail to execute.\r\n");
        return -1;
    }
    
    fprintf(stderr, "Result: 0x%04x\r\n", result);
    return 0;
}

static int i2cgetbuffer(int argc, char **argv)
{
    unsigned char address, length;
    int result;
    unsigned char buf[IB_MAX_I2C_NODE_LENGTH];
    int i;
    
    if(argc != 2)
    {
        fprintf(stderr, "Paras:\r\n");
        fprintf(stderr, "address: The i2c address.\r\n");
        fprintf(stderr, "length: The length.\r\n");
        return -1;
    }
    
    if(StrToByte(argv[0], address) < 0)
    {
        fprintf(stderr, "Invalid address(%s).\r\n", argv[0]);
        return -1;
    }

    if((StrToByte(argv[1], length) < 0) || (length > IB_MAX_I2C_NODE_LENGTH))
    {
        fprintf(stderr, "Invalid length(%s).\r\n", argv[1]);
        return -1;
    }
    
    fprintf(stderr, "Address=0x%02x, Length=0x%02x.\r\n", address, length);
    
    result = i2cControl.i2cget(address, buf, length);
    if(result < 0)
    {
        fprintf(stderr, "Fail to execute.\r\n");
        return -1;
    }
    
    fprintf(stderr, "Result:\r\n", result);
    for(i = 0; i < length; i ++)
    {
        fprintf(stderr, "0x%02x", buf[i]);
        if((i % 16) == 15)
        {
            fprintf(stderr, "\r\n");
        }
    }
    
    if(length % 16)
    {
        fprintf(stderr, "\r\n");
    }
    
    return 0;
}

static int i2cgetbufferc(int argc, char **argv)
{
    unsigned char address, command, length;
    int result;
    unsigned char buf[IB_MAX_I2C_NODE_LENGTH];
    int i;
    
    if(argc != 3)
    {
        fprintf(stderr, "Paras:\r\n");
        fprintf(stderr, "address: The i2c address.\r\n");
        fprintf(stderr, "command: The i2c command.\r\n");
        fprintf(stderr, "length: The length.\r\n");
        return -1;
    }
    
    if(StrToByte(argv[0], address) < 0)
    {
        fprintf(stderr, "Invalid address(%s).\r\n", argv[0]);
        return -1;
    }

    if(StrToByte(argv[1], command) < 0)
    {
        fprintf(stderr, "Invalid address(%s).\r\n", argv[1]);
        return -1;
    }

    if((StrToByte(argv[2], length) < 0) || (length > IB_MAX_I2C_NODE_LENGTH))
    {
        fprintf(stderr, "Invalid length(%s).\r\n", argv[2]);
        return -1;
    }
    
    fprintf(stderr, "Address=0x%02x, Command=0x%02x, Length=0x%02x.\r\n", address, command, length);
    
    result = i2cControl.i2cget(address, command, buf, length);
    if(result < 0)
    {
        fprintf(stderr, "Fail to execute.\r\n");
        return -1;
    }
    
    fprintf(stderr, "Result:\r\n", result);
    for(i = 0; i < length; i ++)
    {
        fprintf(stderr, "0x%02x ", buf[i]);
        if((i % 16) == 15)
        {
            fprintf(stderr, "\r\n");
        }
    }
    
    if(length % 16)
    {
        fprintf(stderr, "\r\n");
    }
    
    return 0;
}

static int i2csetbc(int argc, char **argv)
{
    unsigned char address, command, value;
    int result; 
    if(argc != 3)
    {
        fprintf(stderr, "Paras:\r\n");
        fprintf(stderr, "address: The i2c address.\r\n");
        fprintf(stderr, "command: The i2c command.\r\n");
        fprintf(stderr, "value: The i2c data.\r\n");
        return -1;
    }
    
    if(StrToByte(argv[0], address) < 0)
    {
        fprintf(stderr, "Invalid address(%s).\r\n", argv[0]);
        return -1;
    }

    if(StrToByte(argv[1], command) < 0)
    {
        fprintf(stderr, "Invalid command(%s).\r\n", argv[1]);
        return -1;
    }

    if(StrToByte(argv[2], value) < 0)
    {
        fprintf(stderr, "Invalid value(%s).\r\n", argv[2]);
        return -1;
    }
    
    fprintf(stderr, "Address=0x%02x, Command=0x%02x, Value=0x%02x.\r\n", address, command, value);
    
    result = i2cControl.i2cset(address, command, (int)value, 'b');
    if(result < 0)
    {
        fprintf(stderr, "Fail to execute.\r\n");
        return -1;
    }
    
    fprintf(stderr, "Succeed!\r\n");
    return 0;
}

static int i2csetwc(int argc, char **argv)
{
    unsigned char address, command;
    unsigned short value;
    int result; 
    if(argc != 3)
    {
        fprintf(stderr, "Paras:\r\n");
        fprintf(stderr, "address: The i2c address.\r\n");
        fprintf(stderr, "command: The i2c command.\r\n");
        fprintf(stderr, "value: The i2c data.\r\n");
        return -1;
    }
    
    if(StrToByte(argv[0], address) < 0)
    {
        fprintf(stderr, "Invalid address(%s).\r\n", argv[0]);
        return -1;
    }

    if(StrToByte(argv[1], command) < 0)
    {
        fprintf(stderr, "Invalid command(%s).\r\n", argv[1]);
        return -1;
    }

    if(StrToWord(argv[2], value) < 0)
    {
        fprintf(stderr, "Invalid value(%s).\r\n", argv[2]);
        return -1;
    }
    
    fprintf(stderr, "Address=0x%02x, Command=0x%02x, Value=0x%04x.\r\n", address, command, value);
    
    result = i2cControl.i2cset(address, command, (int)value, 'w');
    if(result < 0)
    {
        fprintf(stderr, "Fail to execute.\r\n");
        return -1;
    }
    
    fprintf(stderr, "Succeed!\r\n");
    return 0;
}

static int i2csetbuffer(int argc, char **argv)
{
    unsigned char address, length;
    int result;
    unsigned char buf[IB_MAX_I2C_NODE_LENGTH];
    int i;
    
    if(argc < 2)
    {
        fprintf(stderr, "Paras:\r\n");
        fprintf(stderr, "address: The i2c address.\r\n");
        fprintf(stderr, "values: The i2c value sequences seperated by black.\r\n");
        return -1;
    }
    
    if(StrToByte(argv[0], address) < 0)
    {
        fprintf(stderr, "Invalid address(%s).\r\n", argv[0]);
        return -1;
    }

    if((argc - 1) > IB_MAX_I2C_NODE_LENGTH)
    {
        fprintf(stderr, "Too many values.\r\n");
        return -1;
    }
    
    length = (unsigned char)(argc - 1);
    
    memset(buf, 0, IB_MAX_I2C_NODE_LENGTH);
    for(i = 0; i < length; i ++)
    {
        if(StrToByte(argv[i+1], buf[i]) < 0)
        {
            fprintf(stderr, "Invalid value(%s).\r\n", argv[i+1]);
            return -1;
        }
    }
    
    fprintf(stderr, "Address=0x%02x, Length=0x%02x.\r\n", address, length);
    fprintf(stderr, "Values:\r\n");
    for(i = 0; i < length; i ++)
    {
        fprintf(stderr, "0x%02x ", buf[i]);
        if((i % 16) == 15)
        {
            fprintf(stderr, "\r\n");
        }
    }
    
    if(length % 16)
    {
        fprintf(stderr, "\r\n");
    }
    
    result = i2cControl.i2cset(address, buf, length);
    if(result < 0)
    {
        fprintf(stderr, "Fail to execute.\r\n");
        return -1;
    }

    fprintf(stderr, "Succeed!\r\n");

    return 0;
}

static int i2csetbufferc(int argc, char **argv)
{
    unsigned char address, command, length;
    int result;
    unsigned char buf[IB_MAX_I2C_NODE_LENGTH];
    int i;
    
    if(argc < 3)
    {
        fprintf(stderr, "Paras:\r\n");
        fprintf(stderr, "address: The i2c address.\r\n");
        fprintf(stderr, "command: The i2c command.\r\n");
        fprintf(stderr, "values: The i2c value sequences seperated by black.\r\n");
        return -1;
    }
    
    if(StrToByte(argv[0], address) < 0)
    {
        fprintf(stderr, "Invalid address(%s).\r\n", argv[0]);
        return -1;
    }

    if(StrToByte(argv[1], command) < 0)
    {
        fprintf(stderr, "Invalid command(%s).\r\n", argv[1]);
        return -1;
    }

    if((argc - 2) > IB_MAX_I2C_NODE_LENGTH)
    {
        fprintf(stderr, "Too many values.\r\n");
        return -1;
    }
    
    length = (unsigned char)(argc - 2);
    
    memset(buf, 0, IB_MAX_I2C_NODE_LENGTH);
    for(i = 0; i < length; i ++)
    {
        if(StrToByte(argv[i+2], buf[i]) < 0)
        {
            fprintf(stderr, "Invalid value(%s).\r\n", argv[i+2]);
            return -1;
        }
    }
    
    fprintf(stderr, "Address=0x%02x, Command=0x%02x, Length=0x%02x.\r\n", address, command, length);
    fprintf(stderr, "Values:\r\n");
    for(i = 0; i < length; i ++)
    {
        fprintf(stderr, "0x%02x ", buf[i]);
        if((i % 16) == 15)
        {
            fprintf(stderr, "\r\n");
        }
    }
    
    if(length % 16)
    {
        fprintf(stderr, "\r\n");
    }
    
    result = i2cControl.i2cset(address, command, buf, length);
    if(result < 0)
    {
        fprintf(stderr, "Fail to execute.\r\n");
        return -1;
    }

    fprintf(stderr, "Succeed!\r\n");

    return 0;
}

static int i2csetget(int argc, char **argv)
{
    unsigned char address, command, getlength, length;
    int result;
    unsigned char buf[IB_MAX_I2C_NODE_LENGTH];
    unsigned char getbuf[IB_MAX_I2C_NODE_LENGTH];
    int i;
    
    if(argc < 3)
    {
        fprintf(stderr, "Paras:\r\n");
        fprintf(stderr, "address: The i2c address.\r\n");
        fprintf(stderr, "getlen: The get data length.\r\n");
        fprintf(stderr, "setvalues: The set data values.\r\n");
        return -1;
    }
    
    if(StrToByte(argv[0], address) < 0)
    {
        fprintf(stderr, "Invalid address(%s).\r\n", argv[0]);
        return -1;
    }

    if((StrToByte(argv[1], getlength) < 0) || (getlength > IB_MAX_I2C_NODE_LENGTH))
    {
        fprintf(stderr, "Invalid get length(%s).\r\n", argv[1]);
        return -1;
    }

    length = (unsigned char)(argc - 2);
    
    memset(buf, 0, IB_MAX_I2C_NODE_LENGTH);
    for(i = 0; i < length; i ++)
    {
        if(StrToByte(argv[i+2], buf[i]) < 0)
        {
            fprintf(stderr, "Invalid value(%s).\r\n", argv[i+2]);
            return -1;
        }
    }
    
    fprintf(stderr, "Address=0x%02x, GetLength=0x%02x.\r\n", address, getlength);
    fprintf(stderr, "Values:\r\n");
    for(i = 0; i < length; i ++)
    {
        fprintf(stderr, "0x%02x ", buf[i]);
        if((i % 16) == 15)
        {
            fprintf(stderr, "\r\n");
        }
    }
    
    if(length % 16)
    {
        fprintf(stderr, "\r\n");
    }
    
    result = i2cControl.i2cget((int)address, length, buf, getlength, getbuf);
    if(result < 0)
    {
        fprintf(stderr, "Fail to execute.\r\n");
        return -1;
    }
    
    fprintf(stderr, "Result:\r\n");
    for(i = 0; i < getlength; i ++)
    {
        fprintf(stderr, "0x%02x ", getbuf[i]);
        if((i % 16) == 15)
        {
            fprintf(stderr, "\r\n");
        }
    }
    
    if(getlength % 16)
    {
        fprintf(stderr, "\r\n");
    }

    return 0;
}

static STRU_IB_TEST_COMMAND ibTestCommands[] = {
    {"getbc", i2cgetbc, "Get byte by address and command pair."},
    {"getwc", i2cgetwc, "Get word by address and command pair."},
    {"getbuffer", i2cgetbuffer, "Get buffer by address and length pair."},
    {"getbufferc", i2cgetbufferc, "Get buffer by address, command and length triplex."},
    {"setbc", i2csetbc, "Set byte by address, command and value."},
    {"setwc", i2csetwc, "Set word by address, command and value"},
    {"setbuffer", i2csetbuffer, "Set buffer by address and value sequences seperated by black."},
    {"setbufferc", i2csetbufferc, "Set buffer by address, command and value sequences seperated by black."},
    {"setget", i2csetget, "Set then get."},
    {NULL, NULL, NULL}
};

int main(int argc, char **argv)
{
    IBTestFramework ibTest(ibTestCommands);
    return ibTest.Run(argc-1, argv+1);
}
