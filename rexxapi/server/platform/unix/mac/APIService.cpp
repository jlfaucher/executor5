/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2014 Rexx Language Association. All rights reserved.    */
/*                                                                            */
/* This program and the accompanying materials are made available under       */
/* the terms of the Common Public License v1.0 which accompanies this         */
/* distribution. A copy is also available at the following address:           */
/* http://www.oorexx.org/license.html                                         */
/*                                                                            */
/* Redistribution and use in source and binary forms, with or                 */
/* without modification, are permitted provided that the following            */
/* conditions are met:                                                        */
/*                                                                            */
/* Redistributions of source code must retain the above copyright             */
/* notice, this list of conditions and the following disclaimer.              */
/* Redistributions in binary form must reproduce the above copyright          */
/* notice, this list of conditions and the following disclaimer in            */
/* the documentation and/or other materials provided with the distribution.   */
/*                                                                            */
/* Neither the name of Rexx Language Association nor the names                */
/* of its contributors may be used to endorse or promote products             */
/* derived from this software without specific prior written permission.      */
/*                                                                            */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS        */
/* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT          */
/* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS          */
/* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT   */
/* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,      */
/* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,        */
/* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY     */
/* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING    */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         */
/* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <pwd.h>
#include "APIServer.hpp"
#include "stdio.h"
#define OOREXX_PIDFILE "/tmp/ooRexx.pid"

APIServer apiServer;             // the real server instance


/*==========================================================================*
 *  Function: Run
 *
 *  Purpose:
 *
 *  handles the original RXAPI functions.
 *    Perform the message loop
 *
 *
 *==========================================================================*/
void Run (bool asService)
{
    try
    {
        apiServer.initServer();               // start up the server
        apiServer.listenForConnections();     // go into the message loop
    }
    catch (ServiceException *e)
    {
        delete e;
    }
    apiServer.terminateServer();     // shut everything down
}

/*==========================================================================*
 *  Function: Stop
 *
 *  Purpose:
 *
 *  handles the stop request.
 *
 *
 *==========================================================================*/
void Stop(int signo)
{
    apiServer.terminateServer();     // shut everything down

    exit(1);
}


/*==========================================================================*
 *  Function: main
 *
 *  Purpose:
 *
 *  Main entry point.
 *
 *==========================================================================*/
int main(int argc, char *argv[])
{
    char pid_buf[256];
    int pfile, len;
    pid_t pid = 0;
    struct stat st;
    struct sigaction sa;
    // Get the command line args
    if (argc > 1) {
        printf("Error: Invalid command line option(s).\n");
        printf("       Aborting execution.\n\n");
        return -1;
    }

    // see if we are already running
    if ((pfile = open(OOREXX_PIDFILE, O_RDONLY)) > 0) {
            len = read(pfile, pid_buf, sizeof(pid_buf) - 1);
            close(pfile);
            pid_buf[len] = '\0';
            pid = (pid_t)atoi(pid_buf);
            if (pid && (pid == getpid() || kill(pid, 0) < 0)) {
                    unlink(OOREXX_PIDFILE);
            } else {
                    // there is already a server running
                    printf("Error: There is already a server running.\n");
                    printf("       Aborting execution.\n");
                    return -1;
            }
    }

    // write the pid file
    pfile = open(OOREXX_PIDFILE, O_WRONLY | O_CREAT,
                 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if (pfile == -1) {
            // cannot open pid file
            printf("Error: Cannot open PID file %s.\n", OOREXX_PIDFILE);
            printf("       Aborting execution.\n\n");
            return -1;
    }
    snprintf(pid_buf, sizeof(pid_buf), "%d\n", (int)getpid());
    write(pfile, pid_buf, strlen(pid_buf));
    close(pfile);

    // handle kill -15
    (void) sigemptyset(&sa.sa_mask);
    (void) sigaddset(&sa.sa_mask, SIGTERM);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = Stop;
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        exit(1);
    }

    Run(false);

    return 0;
}

