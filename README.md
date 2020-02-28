# Simple-Sandbox

## Usage
```
usage: ./sandbox [-p sopath] [-d basedir] [--] cmd [cmd args ...]
    -p: set the path to sandbox.so, default = ./sandbox.so
    -d: restrict directory, default = .
    --: seperate the arguments for sandbox and for the executed command"
```

## Features

* execute cmds (in /bin) with sandbox
* the sandbox blocks the access to outside the current directory and some system calls
* the scope of the accessible directory can be set to other directory with option [-d basdir]
* The monitored system calls are showed in follows:
```
opendir open stat fopen chdir chmod chown creat link mkdir openat 
readlink remove rename rmdir symlink unlink
```
* However following system calls are absolutely blocked
```
execvp execve execl execle execlp execv system
```
