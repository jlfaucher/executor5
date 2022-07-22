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


parse arg args
call main args
return result

--::options trace i

::resource usage
Usage :
    tracer [-csv] [-description] [-filter] [-help] [-removePrefix] [<traceFile>]

Examples :
    (Remember : you MUST redirect stderr to stdout with 2>&1)

    Windows:
    set RXTRACE_CONCURRENCY=on
    rexx my_traced_script.rex 2>&1 | rexx tracer -csv > out.csv

    Linux, MacOs:
    RXTRACE_CONCURRENCY=on rexx my_traced_script.rex 2>&1 | rexx tracer -csv > out.csv

    rexx tracer -csv my_trace_file.txt

::END


::resource description
Description :
    Parse a trace output and rewrite each line atfer optional filtering/transformation.
    If no <traceFile> is specified then read from stdin.
    The transformed trace is sent to stdout.

    Use -csv to generate a CSV output.
    The name of the executable is repeated on each line of the CSV output.

    Use -filter to filter out the lines which are not a trace.

    Use -removePrefix to remove any text up to (including) ">" at the begining of each line

    The expected input format is something like that after removing any prefix:
    R1     T1     A1                                >I> Routine D:\lib.cls in package D:\lib.cls
    R1     T1     A1                                >I> Routine A_ROUTINE in package D:\lib.cls
    R1     T1     A2       V1           1*          >I> Method INIT with scope "The DB class" in package D:\lib.cls
    R1     T1     A2       V1           1*       44 *-* self~table = .IdentityTable~new
    R1     T2     A0                         Error 99 running D:\local\doit.rex line 17:  Translation error
    R1     T2     A0                         Error 99.916:  Unrecognized directive instruction

    See GetConcurrencyInfos in Activity.cpp:
        R   infos.interpreter        = interpreter ? interpreter->getIdntfr() : 0;
        T   infos.activity           = activity ? activity->getIdntfr() : 0;
        A   infos.activation         = activation ? activation->getIdntfr() : 0;
        V   infos.variableDictionary = variableDictionary ? variableDictionary->getIdntfr() : 0;
        n   infos.reserveCount       = activation ? activation-> getReserveCount() : 0;
        *   infos.lock               = (activation && activation->isObjectScopeLocked()) ? '*' : ' ';

    The classic trace without concurrency informations is also supported.
    That lets generate a CSV file for the classic trace, more easy to analyze/filter.
    It's not possible to repeat the name of the executable on each line of the CSV output.

::END


/****

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

::routine main

csv = .false
filter = .false
removePrefix = .false
showDescription = .false
showUsage = .false
errorCount = 0

parse arg args
do forever
    parse var args current rest
    if current~left(1) == "-" then do
        -- accept -option or --option
        if current~left(2) == "--" then current = current~substr(3)
        else current = current~substr(2)

        if "csv"~caselessAbbrev(current, 1) then csv = .true
        else if "description"~caselessAbbrev(current, 1) then showDescription = .true
        else if "filter"~caselessAbbrev(current, 1) then filter = .true
        else if "help"~caselessAbbrev(current, 1) then showUsage = .true
        else if "removePrefix"~caselessAbbrev(current, 1) then removePrefix = .true
        else do
            say "[error] Invalid option : "current
            errorCount += 1
        end
        args = rest
    end
    else do
        traceFile = args
        leave
    end
end

if errorCount > 0 | showUsage then say .resources["USAGE"]
if showDescription then say .resources["DESCRIPTION"]
if errorCount > 0 | showUsage | showDescription then return errorCount <> 0

streamIn = .stdin
traceFile = .Tracer.Utility~unquoted(traceFile)
if traceFile <> "" then streamIn = .stream~new(traceFile)~~open

streamOut = .stdout
traceLineParser = .Tracer.TraceLineParser~new

if csv then .Tracer.TraceLineCsv~lineoutTitle(streamOut)
do while streamIn~state="READY"
    rawLine=streamIn~linein
    if streamIn~state="NOTREADY", rawLine == "" then leave
    if removePrefix, rawLine~word(1)~contains(">") then do
        newStart = rawLine~pos(">") + 1
        if rawLine~subchar(newStart) == " " then newStart = rawLine~verify(" ", "N", newStart)
        rawLine = rawLine~substr(newStart)
    end
    currentTrace = traceLineParser~parse(rawLine)
    currentTrace~lineOut(streamOut, csv, filter)
end

return 0


-------------------------------------------------------------------------------
::class Tracer.Utility public
-------------------------------------------------------------------------------

::method quoted class
    -- Returns a quoted string.
    -- If the string contains a double quote character, it is escaped by placing another double quote character next to it.
    -- a"bc"d --> "a""bc""d"
    use strict arg string, quote='"'
    doubleQuote = quote || quote
    return quote || string~changeStr(quote, doubleQuote) || quote


::method unquoted class
    -- Replace escaped double quotes by a double quote, and remove leading and trailing double quotes, if any.
    -- "a""bc""d" --> a"bc"d
    use strict arg string, quote='"'
    doubleQuote = quote || quote
    if string~left(1) == quote & string~right(1) == quote then
        return string~substr(2, string~length - 2)~changeStr(doubleQuote, quote)
    else
        return string


-------------------------------------------------------------------------------
::class Tracer.TraceLineParser public
-------------------------------------------------------------------------------

::attribute rawLine
    -- Specific to concurrency trace
    ::attribute interpreterId
    ::attribute threadId
    ::attribute activationId
    ::attribute varDictId
    ::attribute reserveCount
    ::attribute lock
    -- Normal trace line, without concurrency trace
    ::attribute rawTrace
        ::attribute lineNumber
        ::attribute tracePrefix
        ::attribute restOfTrace
            ::attribute routine
            ::attribute method
            ::attribute scope
            ::attribute package


::method init
    rawLine = ""
    interpreterId = ""
    threadId = ""
    activationId = ""
    varDictId = ""
    reserveCount = ""
    lock = ""
    rawTrace = ""
    lineNumber = ""
    tracePrefix = ""
    restOfTrace = ""
    routine = ""
    method = ""
    scope = ""
    package = ""


::method parseHrId
    use strict arg -- none
    parse value self~rawLine with,
            1 self~interpreterId >(.Tracer.Interpreter~hrIdWidth),
            +1 self~threadId >(.Tracer.Thread~hrIdWidth),
            +1 self~activationId >(.Tracer.Activation~hrIdWidth),
            +1 self~varDictId >(.Tracer.VariableDictionary~hrIdWidth),
            +1 self~reserveCount >(.Tracer.WithActivationInfo~reserveCountHrWidth) self~lock >1,
            +1 self~rawTrace
    return  .Tracer.Interpreter~isHrId(self~interpreterId) &,
            .Tracer.Thread~isHrId(self~threadId) &,
            .Tracer.Activation~isHrId(self~activationId) &,
            .Tracer.VariableDictionary~isHrId(self~varDictId) &,
            (self~reserveCount~datatype("W") | self~reserveCount~strip == "") &,
            (self~lock == " " | self~lock == "*")


::method clearConcurrencyTrace
    -- The various parsings have stored some invalid values, clear them
    use strict arg -- none
    self~interpreterId = ""
    self~threadId = ""
    self~activationId = ""
    self~varDictId = ""
    self~reserveCount = ""
    self~lock = ""
    self~rawTrace = self~rawLine


::method parse
    use strict arg rawLine
    self~init
    self~rawLine = rawLine
    currentTrace = .nil

    -- Several concurrency trace formats supported
    concurrencyTrace = "none"
    if self~parseHrId then concurrencyTrace = "hr" -- hr ids (parsing a trace already hr-ized)
    else self~clearConcurrencyTrace

    if self~rawTrace~pos("Error") == 1 then currentTrace = .Tracer.ErrorTrace~new
    else do -- maybe normal trace line
        parse value self~rawTrace with 1 self~lineNumber >6 +1 self~tracePrefix >3 +1 self~restOfTrace
        valid = .Tracer.TraceLine~isValidLineNumber(self~lineNumber) &,
                .Tracer.TraceLine~isValidPrefix(self~tracePrefix)
        if \valid then do
            if concurrencyTrace == "none" then currentTrace = .Tracer.UnknownFormat~new
            else currentTrace = .Tracer.InvalidTrace~new
        end
        else do -- valid trace line
            if self~tracePrefix == ">I>" then do
                if self~restOfTrace~pos("Routine ") == 1 then do
                    currentTrace = .Tracer.RoutineActivation~new
                    parse value self~restOfTrace with 'Routine "' self~routine '" in package "' self~package '"' .
                end
                else if self~restOfTrace~pos("Method ") == 1 then do
                    currentTrace = .Tracer.MethodActivation~new
                    parse value self~restOfTrace with 'Method "' self~method '" with scope "' self~scope '" in package "' self~package '"' .
                end
                else currentTrace = .Tracer.UnknownActivation~new
            end
            else currentTrace = .Tracer.GenericTrace~new
        end
    end

    currentTrace~initializeWith(self)
    return currentTrace


-------------------------------------------------------------------------------
::class Tracer.TraceLineCsv public
-------------------------------------------------------------------------------
-- Helper to generate a CSV line

::constant sep ","

-- Better to have a non-empty value, otherwise filtering may not be good (depending on your favorite tool)
::constant defaultValue "."


::method lineoutTitle class
    use strict arg stream
    stream~charout("interpreter") ; stream~charout(self~sep)
    stream~charout("thread") ; stream~charout(self~sep)
    stream~charout("activation") ; stream~charout(self~sep)
    stream~charout("varDict") ; stream~charout(self~sep)
    stream~charout("count") ; stream~charout(self~sep)
    stream~charout("lock") ; stream~charout(self~sep)
    stream~charout("kind") ; stream~charout(self~sep)
    stream~charout("scope") ; stream~charout(self~sep)
    stream~charout("executable") ; stream~charout(self~sep)
    stream~charout("line") ; stream~charout(self~sep)
    stream~charout("prefix") ; stream~charout(self~sep)
    stream~charout("trace") ; stream~charout(self~sep)
    stream~charout("package") ; stream~charout(self~sep)
    stream~charout("raw") -- ; stream~charout(self~sep)
    stream~lineout("")


::attribute interpreterId
::attribute threadId
::attribute activationId
::attribute varDictId
::attribute reserveCount
::attribute lock
::attribute kind -- of executable
::attribute scope -- of executable
::attribute executable
::attribute line
::attribute prefix
::attribute trace
::attribute package
::attribute raw


::method init
    self~interpreterId = ""
    self~threadId = ""
    self~activationId = ""
    self~varDictId = ""
    self~reserveCount = ""
    self~lock = ""
    self~kind = ""
    self~scope = ""
    self~executable = ""
    self~line = ""
    self~prefix = ""
    self~trace = ""
    self~package = ""
    self~raw = ""


::method quoted
    use strict arg value
    value = value~strip
    if value == "" then value = self~defaultValue
    return .Tracer.Utility~quoted(value)


::method charout
    use strict arg stream, value
    stream~charout(self~quoted(value))
    stream~charout(self~sep)


::method lineout
    use strict arg stream
    self~charout(stream, self~interpreterId)
    self~charout(stream, self~threadId)
    self~charout(stream, self~activationId)
    self~charout(stream, self~varDictId)
    self~charout(stream, self~reserveCount)
    self~charout(stream, self~lock)
    self~charout(stream, self~kind)
    self~charout(stream, self~scope)
    self~charout(stream, self~executable)
    self~charout(stream, self~line)
    self~charout(stream, self~prefix)
    self~charout(stream, self~trace)
    self~charout(stream, self~package)
    self~charout(stream, self~raw)
    stream~lineout("")


-------------------------------------------------------------------------------
::class Tracer.Interpreter public
-------------------------------------------------------------------------------

-- Change "I" to "R" for the RII (Rexx interpreter instance).
-- The reason is, that "I1" and "T1" (actually "I" and "T")  look quite the same,
-- whereas "R1" and "T1" very distinctive,
::constant hrIdLetter "R"
::constant hrId0 "R0"

::constant hrIdWidth 6 -- width of hr id (used for parsing and rewriting): R0..R99999


-- class attributes
::attribute counter class
::attribute directory class


::method init class
    self~counter = 0
    self~directory = .directory~new


::method fromId class
    use strict arg interpreterId
    interpreter = .Tracer.Interpreter~directory[interpreterId]
    if .nil == interpreter then do
        interpreter = .Tracer.Interpreter~new
        .Tracer.Interpreter~directory[interpreterId] = interpreter
        interpreter~id = interpreterId
        if interpreterId == "" then interpreter~hrId = ""
        else if interpreterId = 0 | interpreterId == self~hrId0 then interpreter~hrId = self~hrId0 -- Always use R0 for null pointer
        else do
            .Tracer.Interpreter~counter += 1
            interpreter~hrId = self~hrIdLetter || .Tracer.Interpreter~counter
        end
    end
    return interpreter


::method isHrId class
    use strict arg interpreterId
    return interpreterId~left(1) == self~hrIdLetter & interpreterId~substr(2)~dataType("W")  -- Ex : R1, R12, R123, ...


-- instance attributes
::attribute id
::attribute hrId -- Human-readable


::method init
    self~id = ""
    self~hrId = ""


-------------------------------------------------------------------------------
::class Tracer.Thread public
-------------------------------------------------------------------------------

::constant hrIdLetter "T"
::constant hrId0 "T0"

::constant hrIdWidth 6 -- width of hr id (used for parsing and rewriting): T0..T99999


-- class attributes
::attribute counter class
::attribute directory class


::method init class
    self~counter = 0
    self~directory = .directory~new


::method fromId class
    use strict arg threadId
    thread = .Tracer.Thread~directory[threadId]
    if .nil == thread then do
        thread = .Tracer.Thread~new
        .Tracer.Thread~directory[threadId] = thread
        thread~id = threadId
        if threadId == "" then thread~hrId = ""
        else if threadId = 0 | threadId == self~hrId0 then thread~hrId = self~hrId0 -- Always use T0 for null pointer
        else do
            .Tracer.Thread~counter += 1
            thread~hrId = self~hrIdLetter || .Tracer.Thread~counter
        end
    end
    return thread


::method isHrId class
    use strict arg threadId
    return threadId~left(1) == self~hrIdLetter & threadId~substr(2)~dataType("W")  -- Ex : T1, T12, T123, ...


-- instance attributes
::attribute id
::attribute hrId -- Human-readable


::method init
    self~id = ""
    self~hrId = ""


-------------------------------------------------------------------------------
::class Tracer.Activation public
-------------------------------------------------------------------------------

::constant hrIdLetter "A"
::constant hrId0 "A0"

::constant hrIdWidth 8 -- max width of hr id (used for parsing and rewriting): A0..A9999999


-- class attributes
::attribute counter class
::attribute directory class


::method init class
    self~counter = 0
    self~directory = .directory~new


-- Human-readable activation id
::method fromId class
    use strict arg activationId
    activation = .Tracer.Activation~directory[activationId]
    if .nil == activation then do
        activation = .Tracer.Activation~new
        .Tracer.Activation~directory[activationId] = activation
        activation~id = activationId
        if activationId == "" then activation~hrId = ""
        else if activationId = 0 | activationId == self~hrId0 then activation~hrId = self~hrId0 -- Always use A0 for null pointer
        else do
            .Tracer.Activation~counter += 1
            activation~hrId = self~hrIdLetter || .Tracer.Activation~counter
        end
    end
    return activation


::method isHrId class
    use strict arg activationId
    return activationId~left(1) == self~hrIdLetter & activationId~substr(2)~dataType("W")  -- Ex : A1, A12, A123, ...

-- instance attributes
::attribute id
::attribute hrId -- Human-readable
::attribute kind
::attribute scope
::attribute executable
::attribute package


::method init
    self~id = ""
    self~hrId = ""
    self~kind = ""
    self~scope = ""
    self~executable = ""
    self~package = ""


-------------------------------------------------------------------------------
::class Tracer.VariableDictionary public
-------------------------------------------------------------------------------

::constant hrIdLetter "V"
::constant hrId0 "V0"

::constant hrIdWidth 8 -- max width of hr id (used for parsing and rewriting): V0..V9999999


-- class attributes
::attribute counter class
::attribute directory class


::method init class
    self~counter = 0
    self~directory = .directory~new


-- Human-readable varDict id
::method fromId class
    use strict arg varDictId -- can be made of spaces, when parsing hr trace
    varDict = .Tracer.VariableDictionary~directory[varDictId]
    if .nil == varDict then do
        varDict = .Tracer.VariableDictionary~new
        .Tracer.VariableDictionary~directory[varDictId] = varDict
        varDict~id = varDictId
        /*
        Remember:
        varDictId is made only of spaces when the pointer is NULL: the test varDictId = "" returns .true (non strict equal).
        When printed, "V0" is replaced by spaces (see WithActivationInfo~charOut).
        The case varDictId == "" should never happen.
        */
        if varDictId == "" then varDict~hrId = ""
        else if varDictId = 0 | varDictId == self~hrId0 | varDictId = "" then varDict~hrId = self~hrId0 -- Always use V0 for null pointer
        else do
            .Tracer.VariableDictionary~counter += 1
            varDict~hrId = self~hrIdLetter || .Tracer.VariableDictionary~counter
        end
    end
    return varDict


::method isHrId class
    use strict arg varDictId
    if varDictId~verify(" ") == 0 then return .true -- special case, when parsing hr trace.
    return varDictId~left(1) == self~hrIdLetter & varDictId~substr(2)~dataType("W")  -- Ex : V1, V12, V123, ...

-- instance attributes
::attribute id
::attribute hrId -- Human-readable


::method init
    self~id = ""
    self~hrId = ""


-------------------------------------------------------------------------------
::class Tracer.TraceLine public abstract
-------------------------------------------------------------------------------
-- TraceLine
--    UnknownFormat
--    WithActivationInfo
--        ErrorTrace
--        InvalidTrace
--        GenericTrace
--            RoutineActivation
--            MethodActivation
--            UnknownActivation


::method isValidLineNumber class
    use strict arg lineNumber
    return lineNumber~datatype("W") | lineNumber~strip == ""


::method isValidPrefix class
    use strict arg prefix
    -- The last "..." is not a standard trace prefix. This is the prefix used by the debug output of SysSemaphore and SysMutex.
    -- New ooRexx5: >K> <R>
    -- Collision: >N> is named argument for executor, namespace for ooRexx5
    return prefix~space(0)~length == 3 & "*-* +++ >I> >>> >=> >.> >A> >C> >E> >F> >L> >M> >N> >O> >P> >V> >K> >R> ..."~pos(prefix) <> 0


::method initializeWith
    use strict arg _traceLineParser
    return


::method lineout
    use strict arg stream, csv, filter
    if csv then do
        csv = .Tracer.TraceLineCsv~new
        if self~prepareCsv(csv, filter) then csv~lineout(stream)
    end
    else do
        if self~charout(stream, filter) then stream~lineOut("")
    end


::method charout
    use strict arg stream, _filter
    return .false -- nothing displayed here


::method prepareCsv
    use strict arg _csv, _filter
    return .false -- nothing stored here


-------------------------------------------------------------------------------
::class Tracer.UnknownFormat subclass Tracer.TraceLine public
-------------------------------------------------------------------------------

::attribute rawLine


::method initializeWith
    use strict arg traceLineParser
    self~rawLine = traceLineParser~rawline
    forward class (super)


::method charout
    use strict arg stream, filter
    if filter then return .false
    forward class (super) continue
    if result == .true then stream~charout(" ")
    -- Unknown structure : print as-is
    stream~charout(self~rawLine)
    return .true


::method prepareCsv
    use strict arg csv, filter
    if filter then return .false
    forward class (super) continue
    -- Unknown structure : put all in 'raw' column
    csv~raw = self~rawLine
    return .true


-------------------------------------------------------------------------------
::class Tracer.WithActivationInfo subclass Tracer.TraceLine public
-------------------------------------------------------------------------------

::constant reserveCountRawWidth 5 -- unsigned short : 0...65535 : 5 digits
::constant reserveCountHrWidth 5


::attribute interpreterId
::attribute threadId
::attribute activationId
::attribute varDictId
::attribute reserveCount
::attribute lock


::method initializeWith
    use strict arg traceLineParser
    self~interpreterId = traceLineParser~interpreterId
    self~threadId = traceLineParser~threadId
    self~activationId = traceLineParser~activationId
    self~varDictId = traceLineParser~varDictId
    self~reserveCount = traceLineParser~reserveCount
    self~lock = traceLineParser~lock
    forward class (super)


::method noConcurrencyTrace
    return self~interpreterId == "",
         & self~threadId == "",
         & self~activationId == "",
         & self~varDictId == "",
         & self~reserveCount == "",
         & self~lock == ""


::method charout
    use strict arg stream, _filter

    forward class (super) continue
    spaceNeeded = result

    if self~noConcurrencyTrace then return .false

    interpreter = .Tracer.Interpreter~fromId(self~interpreterId)
    thread = .Tracer.Thread~fromId(self~threadId)
    activation = .Tracer.Activation~fromId(self~activationId)

    varDict = .Tracer.VariableDictionary~fromId(self~varDictId)
    varDictHrId = varDict~hrId
    if varDictHrId == "V0" then varDictHrId = ""

    reserveCount = self~reserveCount
    if reserveCount = 0 then reserveCount = ""
    if reserveCount <> "" then reserveCount = reserveCount~format(.Tracer.WithActivationInfo~reserveCountHrWidth)

    if spaceNeeded then stream~charout(" ")
    stream~charout(interpreter~hrId~left(.Tracer.Interpreter~hrIdWidth),
                   thread~hrId~left(.Tracer.Thread~hrIdWidth),
                   activation~hrId~left(.Tracer.Activation~hrIdWidth),
                   varDictHrId~left(.Tracer.VariableDictionary~hrIdWidth),
                   reserveCount~left(.Tracer.WithActivationInfo~reserveCountHrWidth) || self~lock)
    return .true


::method prepareCsv
    use strict arg csv, _filter
    forward class (super) continue
    if self~noConcurrencyTrace then return .false

    interpreter = .Tracer.Interpreter~fromId(self~interpreterId)
    csv~interpreterId = interpreter~hrId

    thread = .Tracer.Thread~fromId(self~threadId)
    csv~threadId = thread~hrId

    activation = .Tracer.Activation~fromId(self~activationId)
    csv~activationId = activation~hrId

    varDict = .Tracer.VariableDictionary~fromId(self~varDictId)
    if varDict~hrId <> "V0" then csv~varDictId = varDict~hrId

    if self~reserveCount <> 0 then csv~reserveCount = self~reserveCount
    csv~lock = self~lock
    csv~kind = activation~kind
    csv~scope = activation~scope
    csv~executable = activation~executable
    csv~package = activation~package
    return .true


-------------------------------------------------------------------------------
::class Tracer.ErrorTrace subclass Tracer.WithActivationInfo public
-------------------------------------------------------------------------------

::attribute rawTrace


::method initializeWith
    use strict arg traceLineParser
    self~rawTrace = traceLineParser~rawTrace
    forward class (super)


::method charout
    use strict arg stream, _filter
    forward class (super) continue
    if result == .true then stream~charout(" ")
    stream~charout(self~rawTrace)
    return .true


::method prepareCsv
    use strict arg csv, _filter
    forward class (super) continue
    csv~raw = self~rawTrace
    return .true


-------------------------------------------------------------------------------
::class Tracer.InvalidTrace subclass Tracer.WithActivationInfo public
-------------------------------------------------------------------------------

::attribute rawTrace


::method initializeWith
    use strict arg traceLineParser
    self~rawTrace = traceLineParser~rawTrace
    forward class (super)


::method charout
    use strict arg stream, filter
    if filter then return .false
    forward class (super) continue
    if result == .true then stream~charout(" ")
    stream~charout(self~rawTrace)
    return .true


::method prepareCsv
    use strict arg csv, filter
    if filter then return .false
    forward class (super) continue
    csv~raw = self~rawTrace
    return .true


-------------------------------------------------------------------------------
::class Tracer.GenericTrace subclass Tracer.WithActivationInfo public
-------------------------------------------------------------------------------

::attribute lineNumber
::attribute tracePrefix
::attribute restOfTrace


::method initializeWith
    use strict arg traceLineParser
    self~lineNumber = traceLineParser~lineNumber
    self~traceprefix = traceLineParser~traceprefix
    self~restOfTrace = traceLineParser~restOfTrace
    forward class (super)


::method charout
    use strict arg stream, _filter
    forward class (super) continue
    if result == .true then stream~charout(" ")
    stream~charout(self~lineNumber,
                   self~tracePrefix,
                   self~restOfTrace)
    return .true


::method prepareCsv
    use strict arg csv, _filter
    forward class (super) continue
    csv~line = self~lineNumber
    csv~prefix = self~tracePrefix
    csv~trace = self~restOfTrace
    return .true


-------------------------------------------------------------------------------
::class Tracer.RoutineActivation subclass Tracer.GenericTrace public
-------------------------------------------------------------------------------

::attribute routine
::attribute package


::method initializeWith
    use strict arg traceLineParser
    self~routine = traceLineParser~routine
    self~package = traceLineParser~package
    forward class (super) continue

    if self~noConcurrencyTrace then return

    -- Stores other infos of the activation's trace on the activation, for use in following trace lines
    activation = .Tracer.Activation~fromId(self~activationId)
    activation~kind = "routine"
    activation~scope = ""
    activation~executable = self~routine
    activation~package = self~package


::method prepareCsv
    use strict arg csv, _filter
    forward class (super) continue

    csv~trace = "" -- self~restOfTrace -- redundant with kind, executable, package.

    -- The superclass WithActivationInfo manages the following attributes,
    -- but only when the concurrency trace is available.
    -- Here we can store these values even when no concurrency trace (they come from >I> trace)
    -- but this is ONLY for the current trace line, not for the next trace lines.
    csv~kind = "routine"
    csv~executable = self~routine
    csv~package = self~package
    return .true


-------------------------------------------------------------------------------
::class Tracer.MethodActivation subclass Tracer.GenericTrace public
-------------------------------------------------------------------------------

::attribute method
::attribute scope
::attribute package


::method initializeWith
    use strict arg traceLineParser
    self~method = traceLineParser~method
    self~scope = traceLineParser~scope
    self~package = traceLineParser~package
    forward class (super) continue

    -- Stores other infos of the activation's trace on the activation, for use in following trace lines
    activation = .Tracer.Activation~fromId(self~activationId)
    activation~kind = "method"
    activation~scope = self~scope
    activation~executable = self~method
    activation~package = self~package


::method prepareCsv
    use strict arg csv, _filter
    forward class (super) continue

    csv~trace = "" -- self~restOfTrace -- redundant with kind, scope, executable, package.

    -- The superclass WithActivationInfo manages the following attributes,
    -- but only when the concurrency trace is available.
    -- Here we can store these values even when no concurrency trace (they come from >I> trace)
    -- but this is ONLY for the current trace line, not for the next trace lines.
    csv~kind = "method"
    csv~scope = self~scope
    csv~executable = self~method
    csv~package = self~package
    return .true


-------------------------------------------------------------------------------
::class Tracer.UnknownActivation subclass Tracer.GenericTrace public
-------------------------------------------------------------------------------
