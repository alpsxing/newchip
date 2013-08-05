/**********************************************************************
 Filename: ib_test.h
 Description: Declare the test base method.
 Author: Xing Qianqian
**********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef int (*IB_TEST_CALLBACK_FUNC)(int argc, char **argv);

typedef struct
{
    const char *name;
    IB_TEST_CALLBACK_FUNC func;
    const char *description;
} STRU_IB_TEST_COMMAND;

#define IB_TEST_MAX_COMMAND_LINE_LENGTH         1024
#define IB_TEST_MAX_ARG_NUM                     16
#define IB_TEST_MAX_ARG_LENGTH                  256
#define IB_TEST_COMMAND_HELP                    "help"
#define IB_TEST_COMMAND_EXIT                    "exit"
#define IB_TEST_COMMAND_PROMPT                  "--->"

class IBTestFramework
{
public:
    IBTestFramework(STRU_IB_TEST_COMMAND *commands);
    int Run(int argc, char **argv);

protected:
    virtual void Help(int argc, char**argv);
    virtual void Banner();
    
private:
    STRU_IB_TEST_COMMAND *GetCommand(const char *name);
	int RunCommand(const char *name, int argc, char **argv);
    int RunCommand(const char *command_line);
    STRU_IB_TEST_COMMAND *m_commands;
};

