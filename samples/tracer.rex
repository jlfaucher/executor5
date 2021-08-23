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


Examples :
    (Remember : you MUST redirect stderr to stdout with 2>&1)

    Windows:
    set RXTRACE_CONCURRENCY=on
    rexx my_traced_script.rex 2>&1 | rexx tracer -csv > out.csv

    Linux, MacOs:
    RXTRACE_CONCURRENCY=on rexx my_traced_script.rex 2>&1 | rexx tracer -csv > out.csv

    rexx tracer -csv my_trace_file.txt


Description :
    Convert a trace to an annotated trace.
    If no <traceFile> is specified then read from stdin.
    The annotated trace is sent to stdout.

    Use -csv to generate a CSV output.

    Use -filter to filter out the lines which are not a trace

    The expected input format is something like that (in case of 32-bit pointers) :
    0bee66b0 0000f5fc 7efb0180 7eeee7a0 00000         >I> Routine D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\generator\coroutine.cls in package D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\generator\coroutine.cls
    0bee66b0 0000f5fc 7efb0180 7eeee7a0 00000         >I> Routine A_ROUTINE in package D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\generator\coroutine.cls
    0bee66b0 0000f5fc 7efb29f8 7eeee7a0 00001*        >I> Method INIT with scope "The COROUTINE class" in package D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\generator\coroutine.cls
    0bee66b0 0000f5fc 7efb29f8 7eeee7a0 00001*     44 *-* self~table = .IdentityTable~new
    0bee66b0 00010244 00000000 00000000 00000  Error 99 running D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\trace\doit.rex line 17:  Translation error
    0bee66b0 00010244 00000000 00000000 00000  Error 99.916:  Unrecognized directive instruction

    See RexxActivity::traceOutput
    Utilities::snprintf(buffer, sizeof buffer - 1, CONCURRENCY_TRACE,
                                                   (activity) ? activity->getInstance() : NULL,
                                                   Utilities::currentThreadId(),
                                                   activation,
                                                   (activation) ? activation->getVariableDictionary() : NULL,
                                                   (activation) ? activation->getReserveCount() : 0,
                                                   (activation && activation->isObjectScopeLocked()) ? '*' : ' ');

    The same format with 64-bit pointers is also supported.
    See common\Utilities.hpp
    #ifdef __REXX64__
    #define CONCURRENCY_TRACE "%16.16x %16.16x %16.16x %16.16x %5.5hu%c "
    #else
    #define CONCURRENCY_TRACE "%8.8x %8.8x %8.8x %8.8x %5.5hu%c "
    #endif


    The same format with human-readable ids is also supported :
    I1     T1     A1                          >I> Routine D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\generator\coroutine.cls in package D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\generator\coroutine.cls
    I1     T1     A1                          >I> Routine A_ROUTINE in package D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\generator\coroutine.cls
    I1     T1     A2       V1          1*          >I> Method INIT with scope "The COROUTINE class" in package D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\generator\coroutine.cls
    I1     T1     A2       V1          1*       44 *-* self~table = .IdentityTable~new
    I1     T2     A0                        Error 99 running D:\local\Rexx\ooRexx\svn\sandbox\jlf\samples\trace\doit.rex line 17:  Translation error
    I1     T2     A0                        Error 99.916:  Unrecognized directive instruction

    The classic trace (without any concurrency trace) is also supported.
    That lets generate a CSV file, more easy to analyze/filter.
    Without concurrency trace, it's not possible to get the name of the executable for each line of the CSV file.


Implementation notes:
    traceLineParser = .Tracer.TraceLineParser~new -- you can create a new instance for each line to parse (if you want to keep the attributes), or reuse the same instance (the attributes are reset before parsing)
    currentTrace = traceLineParser~parse(rawLine) -- updates the attributes of traceLineParser and returns an instance of a subclass of Tracer.TraceLine
    currentTrace~lineOut(streamOut, csv, filter)  -- csv: .true or .false, filter: .true or .false


    Attributes of Tracer.TraceLineParser.
    These attributes are analyzed by the class Tracer.TraceLine and its subclasses.
    The parser instantiates the most specialized subclass in function of what have been parsed successfully.
    This subclass analyzes the attributes in its perimeter, and then delegates to its superclass, until reaching the class Tracer.TraceLine.
    ::attribute rawLine                                    ::class Tracer.UnknownFormat
        Specific to concurrency trace
        ::attribute interpreterId                          ::class Tracer.WithActivationInfo
        ::attribute threadId                               ::class Tracer.WithActivationInfo
        ::attribute activationId                           ::class Tracer.WithActivationInfo
        ::attribute varDictId                              ::class Tracer.WithActivationInfo
        ::attribute reserveCount                           ::class Tracer.WithActivationInfo
        ::attribute lock                                   ::class Tracer.WithActivationInfo
        Normal trace line, without concurrency trace
        ::attribute rawTrace                               ::class Tracer.ErrorTrace, ::class Tracer.InvalidTrace
            ::attribute lineNumber                         ::class Tracer.GenericTrace
            ::attribute tracePrefix                        ::class Tracer.GenericTrace
            ::attribute restOfTrace                        ::class Tracer.GenericTrace
                ::attribute routine                        ::class Tracer.RoutineActivation
                ::attribute method                         ::class Tracer.MethodActivation
                ::attribute scope                          ::class Tracer.MethodActivation
                ::attribute package                        ::class Tracer.MethodActivation, ::class Tracer.RoutineActivation


    The parser returns an instance of the most specialized subclass in function of what have been parsed successfully:
    ::class Tracer.TraceLine (abstract)
        ::class Tracer.UnknownFormat
        ::class Tracer.WithActivationInfo
            ::class Tracer.ErrorTrace
            ::class Tracer.InvalidTrace
            ::class Tracer.GenericTrace
                ::class Tracer.RoutineActivation
                ::class Tracer.MethodActivation
                ::class Tracer.UnknownActivation


    Helper classes to manage the interpreter, task, activation and variable's dictionnary identifiers:
        Tracer.Interpreter
        Tracer.Thread
        Tracer.Activation
        Tracer.VariableDictionary
    Each manages a mapping between hexadecimal pointers and human-readable identifiers.
    Their directories is always growing.
    You can reset their directories by sending the message "init" to each class.


    Output of a reworked trace line.
    Done by the method charout of the most specialized subclass instanciated by the parser.
    This subclass outputs its own attributes, and then delegates to its superclass, until reaching the class Tracer.TraceLine.


    Output of the CSV line.
    Done in three steps with the class Tracer.TraceLineCsv:
    - create an instance of Tracer.TraceLineCsv
    - collect the attributes to display
    - display the attributes
    Attributes of a Tracer.TraceLineCsv.
    They are filled by the method prepareCsv of the most specialized subclass instanciated by the parser.
    This subclass stores its own attributes, and then delegates to its superclass, until reaching the class Tracer.TraceLine.
        ::attribute interpreterId   .Tracer.Interpreter~fromId(traceLineParser~interpreterId)~hrId
        ::attribute threadId        .Tracer.Thread~fromId(traceLineParser~threadId)~hrId
        ::attribute activationId    .Tracer.Activation~fromId(traceLineParser~activationId)~hrId
        ::attribute varDictId       .Tracer.VariableDictionary~fromId(traceLineParser~varDictId)~hrId
        ::attribute reserveCount    traceLineParser~reserveCount
        ::attribute lock            traceLineParser~lock
        ::attribute kind            of executable : "method" or "routine"
        ::attribute scope           of executable -- traceLineParser~scope
        ::attribute executable      traceLineParser~method or traceLineParser~routine
        ::attribute line            traceLineParser~lineNumber
        ::attribute prefix          traceLineParser~traceprefix
        ::attribute trace           traceLineParser~restOfTrace -- empty when kind, scope, executable and package are provided (because redundant)
        ::attribute package         traceLineParser~package
        ::attribute raw             traceLineParser~rawline or traceLineParser~rawTrace

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
