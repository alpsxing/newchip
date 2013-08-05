/**********************************************************************
 Filename: ib_test.cpp
 Description: Defines the test base method.
 Author: Xing Qianqian
**********************************************************************/

#include "ib_test.h"

IBTestFramework::IBTestFramework(STRU_IB_TEST_COMMAND *commands) :
    m_commands(commands)
{
    ;
}

int IBTestFramework::Run(int argc, char **argv)
{
    char line[IB_TEST_MAX_COMMAND_LINE_LENGTH+1];
    if(argc >= 1)
    {
        return RunCommand(argv[0], argc-1, argv+1);
    }
    
    Banner();
    fprintf(stderr, IB_TEST_COMMAND_PROMPT);
    
    while(fgets(line, IB_TEST_MAX_COMMAND_LINE_LENGTH, stdin))
    {
        int len = strlen(line);
        while((len > 0) && ((line[len-1] == '\r') || (line[len-1] == '\n')))
        {
            line[len-1] = '\0';
            len --;
        }
        if(!strcmp(line, IB_TEST_COMMAND_EXIT))
        {
            break;
        }
        
        if(strlen(line))
        {
            RunCommand(line);
        }
        fprintf(stderr, IB_TEST_COMMAND_PROMPT);
    }
	
	return 0;
}

void IBTestFramework::Help(int argc, char **argv)
{
    int i;
    STRU_IB_TEST_COMMAND *command;
    fprintf(stderr, "Help information:\r\n");
    if(argc >= 1)
    {
        for(i = 0; i < argc; i ++)
        {
            command = GetCommand(argv[i]);
            if(command != NULL)
            {
                fprintf(stderr, "%s: %s\r\n", command->name, command->description);
            }
            else
            {
                fprintf(stderr, "%s: Unknown command.\r\n", argv[i]);
            }
        }
        
        return;
    }
    
    command = m_commands;
    while((command != NULL) && (command->name != NULL))
    {
        fprintf(stderr, "%s: %s\r\n", command->name, command->description);
        command ++;
    }
    fprintf(stderr, "help: Get this information.\r\n");
    fprintf(stderr, "exit: Exit to shell.\r\n");
    fprintf(stderr, "End of help information.\r\n");
}

void IBTestFramework::Banner()
{
    fprintf(stderr, "Welcome to InnovBox test framework...\r\n");
    fprintf(stderr, "Type 'help' to get the command list.\r\n");
    fprintf(stderr, "Please start to input your command...\r\n");
    return;
}

STRU_IB_TEST_COMMAND *IBTestFramework::GetCommand(const char *name)
{
    STRU_IB_TEST_COMMAND *command = m_commands;
    
    while((command != NULL) && (command->name != NULL))
    {
        if(!strcmp(name, command->name))
        {
            return command;
        }
        else
        {
            command ++;
        }
    }
    
    return NULL;
}

int IBTestFramework::RunCommand(const char *name, int argc, char **argv)
{
    if((name == NULL) || (strlen(name) <= 0))
    {
        fprintf(stderr, "Null command.\r\n");
        return -1;
    }

    STRU_IB_TEST_COMMAND *command = GetCommand(name);
    if(command == NULL)
    {
        if(!strcmp(name, IB_TEST_COMMAND_HELP))
        {
            Help(argc, argv);
            return 0;
        }
        else
        {
            fprintf(stderr, "Unknown command: %s.\r\n", name);
            return -1;
        }
    }
    
    fprintf(stderr, "Running command: %s...\r\n", command->name);
    return command->func(argc, argv);
}

int IBTestFramework::RunCommand(const char *command_line)
{
    char line[IB_TEST_MAX_COMMAND_LINE_LENGTH+1];
    char *command, *arg, argv[IB_TEST_MAX_ARG_NUM][IB_TEST_MAX_ARG_LENGTH];
    char *args[IB_TEST_MAX_ARG_NUM];
    int argc, i;

    if((command_line == NULL) || (strlen(command_line) <= 0))
    {
        fprintf(stderr, "Null command.\r\n");
        return -1;
    }
    
    if(strlen(command_line) > IB_TEST_MAX_COMMAND_LINE_LENGTH)
    {
        fprintf(stderr, "Too long command: %s.\r\n", command_line);
        return -1;
    }
    
    strcpy(line, command_line);
    command = strtok(line, " ");

    if((command == NULL) || (strlen(command) == 0))
    {
        fprintf(stderr, "Extract NULL command from user's input.\r\n");
        return -1;
    }
    
    for(argc = 0; argc < IB_TEST_MAX_ARG_NUM; argc ++)
    {
        arg = strtok(NULL, " ");
        if((arg == NULL) || (strlen(arg) == 0))
        {
            break;
        }
        strcpy(argv[argc], arg);
    }
    
    fprintf(stderr, "COMMAND: %s\r\n", command);
    fprintf(stderr, "ARG NUM: %d\r\n", argc);
    for(i = 0; i < argc; i ++)
    {
        fprintf(stderr, "ARG%d: %s\r\n", i, argv[i]);
        args[i] = argv[i];
    }
    
    return RunCommand(command, argc, (char **)args);
}
