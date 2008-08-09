/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 2005-2006 Rexx Language Association. All rights reserved.    */
/*                                                                            */
/* This program and the accompanying materials are made available under       */
/* the terms of the Common Public License v1.0 which accompanies this         */
/* distribution. A copy is also available at the following address:           */
/* http://www.ibm.com/developerworks/oss/CPLv1.0.htm                          */
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

#ifndef SysCSStream_Included
#define SysCSStream_Included

// Client Server error codes
typedef enum
{
    CSERROR_OK = 0,
    CSERROR_CONNX_EXISTS,
    CSERROR_CONNX_FAILED,
    CSERROR_IO_FAILED,
    CSERROR_OPEN_FAILED,
    CSERROR_HOSTNAME_PORT,
    CSERROR_INTERNAL,
    CSERROR_UNKNOWN
} CSErrorCodeT;


// This is the Client TCP/IP Stream class
class SysClientStream
{
protected:
    CSErrorCodeT errcode;
    SOCKET c; // client socket
    int domain; // the socket domain
    int type; // the socket type
    int protocol; // the socket protocol

public:
    SysClientStream();
    SysClientStream(const char *name);
    SysClientStream(const char *host, int port);
    ~SysClientStream();
    CSErrorCodeT getError(void)
    {
        return errcode;
    };
    bool open(const char *);
    bool open(const char *, int);
    bool close();
    bool read(void *buf, size_t bufsize, size_t *bytesread);
    bool write(void *buf, size_t bufsize, size_t *byteswritten);
    // the following APIs are usually not used but are here for completeness
    // they should be called prior to calling the Open method
    void setDomain(int newdomain)
    {
        domain = newdomain;
    };
    void setType(int newtype)
    {
        type = newtype;
    };
    void setProtocol(int newprotocol)
    {
        protocol = newprotocol;
    };
    inline bool isClean()
    {
        return errcode == CSERROR_OK;
    }
};

class SysServerStream;

/**
 * Class to manage a single instance of a server connection.
 * These are created any time a server stream object accepts
 * a connection.
 */
class SysServerConnection
{
public:
    SysServerConnection(SysServerStream *s, SOCKET socket);
    ~SysServerConnection();

    bool read(void *buf, size_t bufsize, size_t *bytesread);
    bool write(void *buf, size_t bufsize, size_t *byteswritten);
    bool isLocalConnection();
    bool disconnect(void);

protected:
    SysServerStream *server;
    SOCKET c; // client socket
    CSErrorCodeT errcode;
};

// This is the Server TCP/IP Stream class
class SysServerStream
{
protected:
    CSErrorCodeT errcode;
    SOCKET s; // server socket
    int domain; // the socket domain
    int type; // the socket type
    int protocol; // the socket protocol
    int backlog; // backlog for connecting clients

public:
    SysServerStream();
    SysServerStream(const char *name);
    SysServerStream(int port);
    ~SysServerStream();
    CSErrorCodeT getError(void)
    {
        return errcode;
    };
    bool make(const char *);
    bool make(int);
    SysServerConnection *connect();
    bool close();
    // the following APIs are usually not used but are here for completeness
    // they should be called prior to calling the Make method
    inline void setDomain(int newdomain)
    {
        domain = newdomain;
    };
    inline void setType(int newtype)
    {
        type = newtype;
    };
    inline void setProtocol(int newprotocol)
    {
        protocol = newprotocol;
    };
    inline void setBackLog(int newbacklog)
    {
        backlog = newbacklog;
    };
    inline bool isClean()
    {
        return errcode == CSERROR_OK;
    }
};

#endif
