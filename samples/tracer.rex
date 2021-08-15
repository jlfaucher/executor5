#!@OOREXX_SHEBANG_PROGRAM@
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2014 Rexx Language Association. All rights reserved.    */
/*                                                                            */
/* This program and the accompanying materials are made available under       */
/* the terms of the Common Public License v1.0 which accompanies this         */
/* distribution. A copy is also available at the following address:           */
/* https://www.oorexx.org/license.html                         */
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

/****
Usage :
    tracer [-csv] [-filter] [<traceFile>]

Description :
    Convert a trace to an annotated trace.
    If no <traceFile> is specified then read from stdin.
    The annotated trace is sent to stdout.

    Use -csv to generate a CSV output.

    Use -filter to filter out the lines which are not a trace

    The expected input format is something like that (in case of 32-bit pointers) :
    0000f5fc 7efb0180 7eeee7a0 00000         >I> Routine D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\generator\coroutine.cls in package D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\generator\coroutine.cls
    0000f5fc 7efb0180 7eeee7a0 00000         >I> Routine A_ROUTINE in package D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\generator\coroutine.cls
    0000f5fc 7efb29f8 7eeee7a0 00001*        >I> Method INIT with scope "The COROUTINE class" in package D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\generator\coroutine.cls
    0000f5fc 7efb29f8 7eeee7a0 00001*     44 *-* self~table = .IdentityTable~new
    00010244 00000000 00000000 00000  Error 99 running D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\trace\doit.rex line 17:  Translation error
    00010244 00000000 00000000 00000  Error 99.916:  Unrecognized directive instruction

    See RexxActivity::traceOutput
    Utilities::snprintf(buffer, sizeof buffer - 1, CONCURRENCY_TRACE,
                                                   Utilities::currentThreadId(),
                                                   activation,
                                                   (activation) ? activation->getVariableDictionary() : NULL,
                                                   (activation) ? activation->getReserveCount() : 0,
                                                   (activation && activation->isObjectScopeLocked()) ? '*' : ' ');

    The same format with 64-bit pointers is also supported.
    See common\Utilities.hpp
    #ifdef __REXX64__
    #define CONCURRENCY_TRACE "%16.16x %16.16x %16.16x %5.5hu%c "
    #else
    #define CONCURRENCY_TRACE "%8.8x %8.8x %8.8x %5.5hu%c "
    #endif


    The same format with human-readable ids is also supported :
    T1   A1                   >I> Routine D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\generator\coroutine.cls in package D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\generator\coroutine.cls
    T1   A1                   >I> Routine A_ROUTINE in package D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\generator\coroutine.cls
    T1   A2     V1     1*          >I> Method INIT with scope "The COROUTINE class" in package D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\generator\coroutine.cls
    T1   A2     V1     1*       44 *-* self~table = .IdentityTable~new
    T2   A0                 Error 99 running D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\trace\doit.rex line 17:  Translation error
    T2   A0                 Error 99.916:  Unrecognized directive instruction

    The classic trace (without any concurrency trace) is also supported.
    That lets generate a CSV file, more easy to analyze/filter.
    Without concurrency trace, it's not possible to get the name of the executable for each line of the CSV file.

Examples :
    (Remember : you MUST redirect stderr to stdout with 2>&1)

    Windows:
    set RXTRACE_CONCURRENCY=on
    rexx my_traced_script.rex 2>&1 | rexx tracer -csv > out.csv

    Linux, MacOs:
    RXTRACE_CONCURRENCY=on rexx my_traced_script.rex 2>&1 | rexx tracer -csv > out.csv

    rexx tracer -csv my_trace_file.txt
****/

csv = .false
filter = .false
parse arg args
do forever
    parse var args current rest
    if current~left(1) == "-" then do
        if current~caselessEquals("-csv") then csv = .true
        else if current~caselessEquals("-filter") then filter = .true
        else do
            say "[error] Invalid option : "current
            return 1
        end
        args = rest
    end
    else do
        traceFile = args
        leave
    end
end

streamIn = .stdin
traceFile = .Tracer.Utility~unquoted(traceFile)
if traceFile <> "" then streamIn = .stream~new(traceFile)~~open

streamOut = .stdout
traceLineParser = .Tracer.TraceLineParser~new

if csv then .Tracer.TraceLineCsv~lineoutTitle(streamOut)
do while streamIn~state="READY"
    rawLine=streamIn~linein
    if streamIn~state="NOTREADY", rawLine == "" then leave
    currentTrace = traceLineParser~parse(rawLine)
    currentTrace~lineOut(streamOut, csv, filter)
end

return 0


--::options trace i
