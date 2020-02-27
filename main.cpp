#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include<bits/stdc++.h>
#include <stdio.h>
#include <string.h>

using namespace std;

char** argv_parser(char** cmds, int& cmds_len){

    char **parsed_argv = (char**)malloc(sizeof(char*) * 200);
    int command_location = 0 ;

    for(int i = 1 ; i < cmds_len ; i++){
        int sandbox_args = 0;  //1 means is sandbox args; 0 means is not sandbox args

        //### deal with sandbox args ####
        if (i == 1 && strncmp("-", cmds[i], 1)==0){
            if( strncmp("--", cmds[i], 2)==0){
                sandbox_args = 1;
            }
            else if (strncmp("-d", cmds[i], 2)==0){ //not complete yet
                sandbox_args = 1;
                setenv("DIR_ALLOWED", cmds[i+1], 1);
                //cout<<cmds[i+1]<<endl;
                i = i + 1;
            }
            else if(strncmp("-p", cmds[i], 2)==0){
                sandbox_args = 1;
                setenv("LD_PRELOAD", cmds[i+1], 1);
                i = i + 1;
            }
            else {
                sandbox_args = 1;
                string str (cmds[i]);
                cout<<"./sandbox: invalid option -- '"<<str.at(1)<<"'"<<endl;
                cout<<"usage: ./sandbox [-p sopath] [-d basedir] [--] cmd [cmd args ...]"<<endl;
                cout<<"        -p: set the path to sandbox.so, default = ./sandbox.so"<<endl;
                cout<<"        -d: restrict directory, default = ."<<endl;
                cout<<"        --: seperate the arguments for sandbox and for the executed command"<<endl;
            }
        }
        //### deal with command args ###
        if(sandbox_args == 0){
            parsed_argv[command_location] = cmds[i];
            command_location++;
        }
    }
    parsed_argv[command_location] = NULL;
    cmds_len = cmds_len - command_location;
    return parsed_argv;
}




int main( int argc, char** argv){
    const char* dir_here = getenv("PWD");
    setenv("LD_PRELOAD", "./sandbox.so", 1);
    setenv("HERE", dir_here, 1);

    char **parsed_argv = (char**)malloc(sizeof(char*) * 200);
    parsed_argv = argv_parser(argv, argc); 
    if (argv[1] == NULL){
        printf("no command given.\n");
    }
    else if (argv[argc] != NULL){
        execvp(argv[argc], parsed_argv);
    }
    return 0;
}
