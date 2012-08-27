/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2009 Rexx Language Association. All rights reserved.    */
/*                                                                            */
/* This program and the accompanying materials are made available under       */
/* the terms of the Common Public License v1.0 which accompanies this         */
/* distribution. A copy is also available at the following address:           */
/* http://www.oorexx.org/license.html                          */
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
/******************************************************************************/
/* REXX Kernel                                            RexxConstants.hpp   */
/*                                                                            */
/* Global Object REXX constants                                               */
/*                                                                            */
/******************************************************************************/
#ifndef Included_RexxConstants
#define Included_RexxConstants
                                       /* macros for dual definition */
                                       /* and external prototyping   */
#ifdef DEFINING
#define CONSTCLASS
#define INIT
#define INITIAL(x)      =x
#else
#define CONSTCLASS           extern
#undef INIT
#define INITIAL(x)
#endif

#define CHARCONSTANT(name, value) CONSTCLASS char CHAR_##name[] INITIAL(value)

CHARCONSTANT(ACTIVITY, "ACTIVITY");
CHARCONSTANT(ACTIVATE, "ACTIVATE");
CHARCONSTANT(ADD, "ADD");
CHARCONSTANT(ADDCLASS, "ADDCLASS");
CHARCONSTANT(ADDROUTINE, "ADDROUTINE");
CHARCONSTANT(ADDPUBLICCLASS, "ADDPUBLICCLASS");
CHARCONSTANT(ADDPUBLICROUTINE, "ADDPUBLICROUTINE");
CHARCONSTANT(ADDPACKAGE, "ADDPACKAGE");
CHARCONSTANT(ADDITIONAL, "ADDITIONAL");
CHARCONSTANT(ALLAT, "ALLAT");
CHARCONSTANT(ALLINDEX, "ALLINDEX");
CHARCONSTANT(ALLINDEXES, "ALLINDEXES");
CHARCONSTANT(ALLITEMS, "ALLITEMS");
CHARCONSTANT(ANY, "ANY");
CHARCONSTANT(APPEND, "APPEND");
CHARCONSTANT(ARGUMENTS, "ARGUMENTS");
CHARCONSTANT(ARRAY, "ARRAY");
CHARCONSTANT(ARGS, "ARGS");
CHARCONSTANT(ARRAYSYM, "ARRAY");
CHARCONSTANT(AT, "AT");
CHARCONSTANT(ATTRIBUTE, "ATTRIBUTE");
CHARCONSTANT(AVAILABLE, "AVAILABLE");
CHARCONSTANT(BAD, "BAD");
CHARCONSTANT(BASECLASS, "BASECLASS");
CHARCONSTANT(BOOLEAN, "BOOLEAN");
CHARCONSTANT(BLANK, " ");
CHARCONSTANT(BUFFER, "BUFFER");
CHARCONSTANT(CALL, "CALL");
CHARCONSTANT(CALLWITH, "CALLWITH");
CHARCONSTANT(CASELESSCOMPARETO, "CASELESSCOMPARETO");
CHARCONSTANT(CASELESSCOMPARE, "CASELESSCOMPARE");
CHARCONSTANT(CASELESSEQUALS, "CASELESSEQUALS");
CHARCONSTANT(CASELESSLASTPOS, "CASELESSLASTPOS");
CHARCONSTANT(CASELESSMATCH, "CASELESSMATCH");
CHARCONSTANT(CASELESSMATCHCHAR, "CASELESSMATCHCHAR");
CHARCONSTANT(CASELESSPOS, "CASELESSPOS");
CHARCONSTANT(CHAR, "CHAR");
CHARCONSTANT(CLASS, "CLASS");
CHARCONSTANT(CLASSES, "CLASSES");
CHARCONSTANT(CLOSE, "CLOSE");
CHARCONSTANT(CODE, "CODE");
CHARCONSTANT(COMMAND, "COMMAND");
CHARCONSTANT(COMMON_RETRIEVERS, "COMMON_RETRIEVERS");
CHARCONSTANT(COMPARETO, "COMPARETO");
CHARCONSTANT(COMPARABLE, "COMPARABLE");
CHARCONSTANT(COMPLETED, "COMPLETED");
CHARCONSTANT(CONDITION, "CONDITION");
CHARCONSTANT(CONSTANT, "CONSTANT");
CHARCONSTANT(CONTEXT, "CONTEXT");
CHARCONSTANT(CONTINUE, "CONTINUE");
CHARCONSTANT(COPY, "COPY");
CHARCONSTANT(CSELF, "CSELF");
CHARCONSTANT(DEBUGINPUT, "DEBUGINPUT");
CHARCONSTANT(DECODEBASE64, "DECODEBASE64");
CHARCONSTANT(DEFAULTNAME, "DEFAULTNAME");
CHARCONSTANT(DEFINE, "DEFINE");
CHARCONSTANT(DEFINEDMETHODS, "DEFINEDMETHODS");
CHARCONSTANT(DEFINE_METHODS, "!DEFINE_METHODS");
CHARCONSTANT(DELAY, "DELAY");
CHARCONSTANT(DELETE, "DELETE");
CHARCONSTANT(DESCRIPTION, "DESCRIPTION");
CHARCONSTANT(DIMENSION, "DIMENSION");
CHARCONSTANT(DIRECTORY, "DIRECTORY");
CHARCONSTANT(DOUBLE, "DOUBLE");
CHARCONSTANT(EMPTY, "EMPTY");
CHARCONSTANT(ENCODEBASE64, "ENCODEBASE64");
CHARCONSTANT(ENDLOCAL, "ENDLOCAL");
CHARCONSTANT(ENGINEERING, "ENGINEERING");
CHARCONSTANT(ENHANCED, "ENHANCED");
CHARCONSTANT(ENTRY, "ENTRY");
CHARCONSTANT(ENVELOPE, "ENVELOPE");
CHARCONSTANT(ENVIRONMENT, "ENVIRONMENT");
CHARCONSTANT(ERROR, "ERROR");
CHARCONSTANT(EQUALS, "EQUALS");
CHARCONSTANT(ERRORTEXT, "ERRORTEXT");
CHARCONSTANT(ERRORCONDITION, "ERRORCONDITION");
CHARCONSTANT(EXECUTABLE, "EXECUTABLE");
CHARCONSTANT(EXCEPTION, "EXCEPTION");
CHARCONSTANT(EXIT, "EXIT");
CHARCONSTANT(EXMODE, "EXMODE");
CHARCONSTANT(EXTERNAL, "EXTERNAL");
CHARCONSTANT(FAILURENAME, "FAILURE");
CHARCONSTANT(FALSE, "FALSE");
CHARCONSTANT(FILE, "FILE");
CHARCONSTANT(FILESYSTEM, "FILESYSTEM");
CHARCONSTANT(FINDCLASS, "FINDCLASS");
CHARCONSTANT(FINDROUTINE, "FINDROUTINE");
CHARCONSTANT(FIRST, "FIRST");
CHARCONSTANT(FIRSTITEM, "FIRSTITEM");
CHARCONSTANT(FORWARD, "FORWARD");
CHARCONSTANT(FUNCTIONNAME, "FUNCTION");
CHARCONSTANT(FUNCTIONS, "FUNCTIONS");
CHARCONSTANT(GUARDED, "GUARDED");
CHARCONSTANT(GET, "GET");
CHARCONSTANT(GETBUFFERSIZE, "GETBUFFERSIZE");
CHARCONSTANT(GETDEFINEDMETHODS, "GETDEFINEDMETHODS");
CHARCONSTANT(GLOBAL_STRINGS, "GLOBAL_STRINGS");
CHARCONSTANT(HALT, "HALT");
CHARCONSTANT(HASENTRY, "HASENTRY");
CHARCONSTANT(HASERROR, "HASERROR");
CHARCONSTANT(HASINDEX, "HASINDEX");
CHARCONSTANT(HASITEM, "HASITEM");
CHARCONSTANT(HASMETHOD, "HASMETHOD");
CHARCONSTANT(ID, "ID");
CHARCONSTANT(IDENTITYTABLE, "IDENTITYTABLE");
CHARCONSTANT(IMPORTEDCLASSES, "IMPORTEDCLASSES");
CHARCONSTANT(IMPORTEDPACKAGES, "IMPORTEDPACKAGES");
CHARCONSTANT(IMPORTEDROUTINES, "IMPORTEDROUTINES");
CHARCONSTANT(INDEX, "INDEX");
CHARCONSTANT(INHERIT, "INHERIT");
CHARCONSTANT(INITINSTANCE, "INITINSTANCE");
CHARCONSTANT(INIT, "INIT");
CHARCONSTANT(INITIALADDRESS, SYSINITIALADDRESS);
CHARCONSTANT(INPUT, "INPUT");
CHARCONSTANT(INSTANCEMETHOD, "INSTANCEMETHOD");
CHARCONSTANT(INSTANCEMETHODS, "INSTANCEMETHODS");
CHARCONSTANT(INSTRUCTION, "INSTRUCTION");
CHARCONSTANT(INTERNALNAME, "INTERNALNAME");
CHARCONSTANT(INTEGER, "INTEGER");
CHARCONSTANT(ISA, "ISA");
CHARCONSTANT(ISEMPTY, "ISEMPTY");
CHARCONSTANT(ISGUARDED, "ISGUARDED");
CHARCONSTANT(ISNULL, "ISNULL");
CHARCONSTANT(ISPRIVATE, "ISPRIVATE");
CHARCONSTANT(ISPROTECTED, "ISPROTECTED");
CHARCONSTANT(ISSUBCLASSOF, "ISSUBCLASSOF");
CHARCONSTANT(ISINSTANCEOF, "ISINSTANCEOF");
CHARCONSTANT(ITEM, "ITEM");
CHARCONSTANT(ITEMS, "ITEMS");
CHARCONSTANT(KERNEL, "KERNEL");
CHARCONSTANT(LAST, "LAST");
CHARCONSTANT(LASTITEM, "LASTITEM");
CHARCONSTANT(LIBRARY, "LIBRARY");
CHARCONSTANT(LIST, "LIST");
CHARCONSTANT(LIT, "LIT");
CHARCONSTANT(LOCAL, "LOCAL");
CHARCONSTANT(LOADPACKAGE, "LOADPACKAGE");
CHARCONSTANT(LOSTDIGITS, "LOSTDIGITS");
CHARCONSTANT(MAKE, "MAKE");
CHARCONSTANT(MAKEARRAY, "MAKEARRAY");
CHARCONSTANT(MAKEINTEGER, "MAKEINTEGER");
CHARCONSTANT(MAKESTRING, "MAKESTRING");
CHARCONSTANT(MAPCOLLECTION, "MAPCOLLECTION");
CHARCONSTANT(MATCH, "MATCH");
CHARCONSTANT(MATCHCHAR, "MATCHCHAR");
CHARCONSTANT(MEMORY, "MEMORY");
CHARCONSTANT(MERGE, "MERGE");
CHARCONSTANT(MESSAGE, "MESSAGE");
CHARCONSTANT(MESSAGENAME, "MESSAGENAME");
CHARCONSTANT(METACLASS, "METACLASS");
CHARCONSTANT(METHOD, "METHOD");
CHARCONSTANT(METHODNAME, "METHOD");
CHARCONSTANT(METHODS, "METHODS");
CHARCONSTANT(MIXINCLASS, "MIXINCLASS");
CHARCONSTANT(MUTABLEBUFFER, "MUTABLEBUFFER");
CHARCONSTANT(NAME, "NAME");
CHARCONSTANT(NAME_STRINGS, "NAME_STRINGS");
CHARCONSTANT(NEW, "NEW");
CHARCONSTANT(NEWFILE, "NEWFILE");
CHARCONSTANT(NEXT, "NEXT");
CHARCONSTANT(NIL, "NIL");
CHARCONSTANT(NMETHOD, "NMETHOD");
CHARCONSTANT(NOEXMODE, "NOEXMODE");
CHARCONSTANT(NOMETHOD, "NOMETHOD");
CHARCONSTANT(NONE, "<none>");
CHARCONSTANT(NORMAL, "NORMAL");
CHARCONSTANT(NOSTRING, "NOSTRING");
CHARCONSTANT(NOTIFY, "NOTIFY");
CHARCONSTANT(NOVALUE, "NOVALUE");
CHARCONSTANT(NULLARRAY, "NULLARRAY");
CHARCONSTANT(NULLPOINTER, "NULLPOINTER");
CHARCONSTANT(NULLSTRING, "");
CHARCONSTANT(NUMBERSTRING, "NUMBERSTRING");
CHARCONSTANT(NUMERIC, "NUMERIC");
CHARCONSTANT(OBJECTSYM, "OBJECT");
CHARCONSTANT(OBJECTNAMEEQUALS, "OBJECTNAME=");
CHARCONSTANT(OBJECTNAME, "OBJECTNAME");
CHARCONSTANT(OF, "OF");
CHARCONSTANT(OFF, "OFF");
CHARCONSTANT(ON, "ON");
CHARCONSTANT(ORDEREDCOLLECTION, "ORDEREDCOLLECTION");
CHARCONSTANT(OUTPUT, "OUTPUT");
CHARCONSTANT(PACKAGE, "PACKAGE");
CHARCONSTANT(PARSE, "PARSE");
CHARCONSTANT(PEEK, "PEEK");
CHARCONSTANT(PERIOD, ".");
CHARCONSTANT(POINTER, "POINTER");
CHARCONSTANT(POSITION, "POSITION");
CHARCONSTANT(PREVIOUS, "PREVIOUS");
CHARCONSTANT(PROGRAM,  "PROGRAM");
CHARCONSTANT(PROPAGATE, "PROPAGATE");
CHARCONSTANT(PROPAGATED, "PROPAGATED");
CHARCONSTANT(PROTECTED, "PROTECTED");
CHARCONSTANT(PUBLIC, "PUBLIC");
CHARCONSTANT(PUBLICCLASSES, "PUBLICCLASSES");
CHARCONSTANT(PUBLICROUTINES, "PUBLICROUTINES");
CHARCONSTANT(PUT, "PUT");
CHARCONSTANT(QUALIFY, "QUALIFY");
CHARCONSTANT(QUEUE, "QUEUE");
CHARCONSTANT(QUEUEEXIT, "!QUEUEEXIT");
CHARCONSTANT(QUERYMIXINCLASS, "QUERYMIXINCLASS");
CHARCONSTANT(QUERY, "QUERY");
CHARCONSTANT(RC, "RC");
CHARCONSTANT(RECLAIM, "RECLAIM");
CHARCONSTANT(REGISTERED, "REGISTERED");
CHARCONSTANT(RELATION, "RELATION");
CHARCONSTANT(REMOVE, "REMOVE");
CHARCONSTANT(REMOVEITEM, "REMOVEITEM");
CHARCONSTANT(REPLACEAT, "REPLACEAT");
CHARCONSTANT(REQUEST, "REQUEST");
CHARCONSTANT(REQUIRES, "REQUIRES");
CHARCONSTANT(RESULT, "RESULT");
CHARCONSTANT(REXXCONTEXT, "REXXCONTEXT");
CHARCONSTANT(REXX, "REXX");
CHARCONSTANT(REXXQUEUE, "STDQUE");
CHARCONSTANT(REXXUTIL, "rexxutil");  // NB:  lower case for unix
CHARCONSTANT(ROUTINE, "ROUTINE");
CHARCONSTANT(ROUTINES, "ROUTINES");
CHARCONSTANT(ROUTINENAME, "SUBROUTINE");
CHARCONSTANT(RS, "RS");
CHARCONSTANT(RUN, "RUN");
CHARCONSTANT(RUNUNINIT, "RUNUNINIT");
CHARCONSTANT(RXFUNCADD, "RXFUNCADD");
CHARCONSTANT(RXFUNCDROP, "RXFUNCDROP");
CHARCONSTANT(RXFUNCQUERY, "RXFUNCQUERY");
CHARCONSTANT(SCIENTIFIC, "SCIENTIFIC");
CHARCONSTANT(SCRIPT, "SCRIPT");
CHARCONSTANT(SECURITYMANAGER, "SECURITYMANAGER");
CHARCONSTANT(SECTION, "SECTION");
CHARCONSTANT(SELF, "SELF");
CHARCONSTANT(SEND, "SEND");
CHARCONSTANT(SEQUENCE, "SEQUENCE");
CHARCONSTANT(SERVER, "SERVER");
CHARCONSTANT(SESSION, "SESSION");
CHARCONSTANT(SET, "SET");
CHARCONSTANT(SETBUFFERSIZE, "SETBUFFERSIZE");
CHARCONSTANT(SETDUMP, "SETDUMP");
CHARCONSTANT(SETENTRY, "SETENTRY");
CHARCONSTANT(SETGUARDED, "SETGUARDED");
CHARCONSTANT(SETINTERFACE, "SETINTERFACE");
CHARCONSTANT(SETLOCAL, "SETLOCAL");
CHARCONSTANT(SETMETHOD, "SETMETHOD");
CHARCONSTANT(SETPARMS, "SETPARMS");
CHARCONSTANT(SETPRIVATE, "SETPRIVATE");
CHARCONSTANT(SETPROTECTED, "SETPROTECTED");
CHARCONSTANT(SETSECURITYMANAGER, "SETSECURITYMANAGER");
CHARCONSTANT(SETUNGUARDED, "SETUNGUARDED");
CHARCONSTANT(SHRIEKREXXDEFINED, "!REXXDEFINED");
CHARCONSTANT(SHRIEK_DISPATCH, "!DISPATCH");
CHARCONSTANT(SIGL, "SIGL");
CHARCONSTANT(SIGNAL, "SIGNAL");
CHARCONSTANT(SIZE, "SIZE");
CHARCONSTANT(SORT, "SORT");
CHARCONSTANT(SORTWITH, "SORTWITH");
CHARCONSTANT(SOURCE, "SOURCE");
CHARCONSTANT(STABLESORT, "STABLESORT");
CHARCONSTANT(STABLESORTWITH, "STABLESORTWITH");
CHARCONSTANT(START, "START");
CHARCONSTANT(STARTAT, "STARTAT");
CHARCONSTANT(STATE, "STATE");
CHARCONSTANT(STATS, "STATS");
CHARCONSTANT(STDIN, "STDIN");
CHARCONSTANT(STDERR, "STDERR");
CHARCONSTANT(STDOUT, "STDOUT");
CHARCONSTANT(CSTDIN, "STDIN:");    /* standard streams with colon */
CHARCONSTANT(CSTDERR, "STDERR:");  /* standard streams with colon */
CHARCONSTANT(CSTDOUT, "STDOUT:");  /* standard streams with colon */
CHARCONSTANT(STACKFRAMES, "STACKFRAMES");
CHARCONSTANT(STEM, "STEM");
CHARCONSTANT(STREAMS, "STREAMS");
CHARCONSTANT(STRICT, "STRICT");
CHARCONSTANT(STRING, "STRING");
CHARCONSTANT(STRINGSYM, "STRING");
CHARCONSTANT(SUBCLASS, "SUBCLASS");
CHARCONSTANT(SUBCLASSES, "SUBCLASSES");
CHARCONSTANT(SUBROUTINE, "SUBROUTINE");
CHARCONSTANT(SUPER, "SUPER");
CHARCONSTANT(SUPERCLASS, "SUPERCLASS");
CHARCONSTANT(SUPERCLASSES, "SUPERCLASSES");
CHARCONSTANT(SUPPLIER, "SUPPLIER");
CHARCONSTANT(SYNTAX, "SYNTAX");
CHARCONSTANT(SYSTEM, "SYSTEM");
CHARCONSTANT(TABLE, "TABLE");
CHARCONSTANT(TARGET, "TARGET");
CHARCONSTANT(TOKENIZE_ONLY, "//T");
CHARCONSTANT(TODIRECTORY, "TODIRECTORY");
CHARCONSTANT(TOSTRING, "TOSTRING");
CHARCONSTANT(TRACEBACK, "TRACEBACK");
CHARCONSTANT(TRACEOUTPUT, "TRACEOUTPUT");
CHARCONSTANT(TRANSLATE, "TRANSLATE");
CHARCONSTANT(TRUE, "TRUE");
CHARCONSTANT(UNGUARDED, "UNGUARDED");
CHARCONSTANT(UNINHERIT, "UNINHERIT");
CHARCONSTANT(UNINIT, "UNINIT");
CHARCONSTANT(UNKNOWN, "UNKNOWN");
CHARCONSTANT(UNPACK, "UNPACK");
CHARCONSTANT(UNPROTECTED, "UNPROTECTED");
CHARCONSTANT(UNSETMETHOD, "UNSETMETHOD");
CHARCONSTANT(USER_BLANK, "USER ");
CHARCONSTANT(USERID, "USERID");
CHARCONSTANT(VALUES, "VALUES");
CHARCONSTANT(VAR, "VAR");
CHARCONSTANT(VARIABLE, "VARIABLE");
CHARCONSTANT(VARIABLES, "VARIABLES");
CHARCONSTANT(VERSION, "VERSION");
CHARCONSTANT(WEAKREFERENCE, "WEAKREFERENCE");


/*  Language operators  */

CHARCONSTANT(AND, "&");
CHARCONSTANT(ASSIGNMENT_AND, "&=");
CHARCONSTANT(BACKSLASH, "\\");
CHARCONSTANT(BACKSLASH_EQUAL, "\\=");
CHARCONSTANT(BACKSLASH_GREATERTHAN, "\\>");
CHARCONSTANT(BACKSLASH_LESSTHAN, "\\<");
CHARCONSTANT(BRACKETS, "[]");
CHARCONSTANT(BRACKETSEQUAL, "[]=");
CHARCONSTANT(CONCATENATE, "||");
CHARCONSTANT(ASSIGNMENT_CONCATENATE, "||=");
CHARCONSTANT(DIVIDE, "/");
CHARCONSTANT(ASSIGNMENT_DIVIDE, "/=");
CHARCONSTANT(EQUAL, "=");
CHARCONSTANT(GREATERTHAN, ">");
CHARCONSTANT(GREATERTHAN_EQUAL, ">=");
CHARCONSTANT(GREATERTHAN_LESSTHAN, "><");
CHARCONSTANT(INTDIV, "%");
CHARCONSTANT(ASSIGNMENT_INTDIV, "%=");
CHARCONSTANT(LESSTHAN, "<");
CHARCONSTANT(LESSTHAN_EQUAL, "<=");
CHARCONSTANT(LESSTHAN_GREATERTHAN, "<>");
CHARCONSTANT(MULTIPLY, "*");
CHARCONSTANT(ASSIGNMENT_MULTIPLY, "*=");
CHARCONSTANT(OR, "|");
CHARCONSTANT(ASSIGNMENT_OR, "|=");
CHARCONSTANT(PLUS, "+");
CHARCONSTANT(ASSIGNMENT_PLUS, "+=");
CHARCONSTANT(POWER, "**");
CHARCONSTANT(ASSIGNMENT_POWER, "**=");
CHARCONSTANT(REMAINDER, "//");
CHARCONSTANT(ASSIGNMENT_REMAINDER, "//=");
CHARCONSTANT(STRICT_BACKSLASH_EQUAL, "\\==");
CHARCONSTANT(STRICT_BACKSLASH_GREATERTHAN, "\\>>");
CHARCONSTANT(STRICT_BACKSLASH_LESSTHAN, "\\<<");
CHARCONSTANT(STRICT_EQUAL, "==");
CHARCONSTANT(STRICT_GREATERTHAN, ">>");
CHARCONSTANT(STRICT_GREATERTHAN_EQUAL, ">>=");
CHARCONSTANT(STRICT_LESSTHAN, "<<");
CHARCONSTANT(STRICT_LESSTHAN_EQUAL, "<<=");
CHARCONSTANT(SUBTRACT, "-");
CHARCONSTANT(ASSIGNMENT_SUBTRACT, "-=");
CHARCONSTANT(XOR, "&&");
CHARCONSTANT(ASSIGNMENT_XOR, "&&=");
CHARCONSTANT(ELLIPSIS, "...");


                                          /*now names for the builtin functions   */
CHARCONSTANT(ABBREV, "ABBREV");
CHARCONSTANT(CASELESSABBREV, "CASELESSABBREV");
CHARCONSTANT(ABS, "ABS");
CHARCONSTANT(ABSTRACT, "ABSTRACT");
CHARCONSTANT(ADDRESS, "ADDRESS");
CHARCONSTANT(ARG, "ARG");
CHARCONSTANT(B2X, "B2X");
CHARCONSTANT(BITAND, "BITAND");
CHARCONSTANT(BITOR, "BITOR");
CHARCONSTANT(BITXOR, "BITXOR");
CHARCONSTANT(C2D, "C2D");
CHARCONSTANT(C2X, "C2X");
CHARCONSTANT(CENTER, "CENTER");
CHARCONSTANT(CENTRE, "CENTRE");
CHARCONSTANT(CHANGESTR, "CHANGESTR");
CHARCONSTANT(CASELESSCHANGESTR, "CASELESSCHANGESTR");
CHARCONSTANT(CHARIN, "CHARIN");
CHARCONSTANT(CHAROUT, "CHAROUT");
CHARCONSTANT(CHARS, "CHARS");
CHARCONSTANT(COMPARE, "COMPARE");
CHARCONSTANT(COPIES, "COPIES");
CHARCONSTANT(COUNTSTR, "COUNTSTR");
CHARCONSTANT(CASELESSCOUNTSTR, "CASELESSCOUNTSTR");
CHARCONSTANT(D2C, "D2C");
CHARCONSTANT(D2X, "D2X");
CHARCONSTANT(DATATYPE, "DATATYPE");
CHARCONSTANT(DATE, "DATE");
CHARCONSTANT(DELSTR, "DELSTR");
CHARCONSTANT(DELWORD, "DELWORD");
CHARCONSTANT(DIGITS, "DIGITS");
CHARCONSTANT(FORM, "FORM");
CHARCONSTANT(FORMAT, "FORMAT");
CHARCONSTANT(FUZZ, "FUZZ");
CHARCONSTANT(HASHCODE, "HASHCODE");
CHARCONSTANT(IDENTITYHASH, "IDENTITYHASH");
CHARCONSTANT(INSERT, "INSERT");
CHARCONSTANT(LASTPOS, "LASTPOS");
CHARCONSTANT(LEFT, "LEFT");
CHARCONSTANT(LENGTH, "LENGTH");
CHARCONSTANT(LINEIN, "LINEIN");
CHARCONSTANT(LINEOUT, "LINEOUT");
CHARCONSTANT(LINES, "LINES");
CHARCONSTANT(LINE, "LINE");
CHARCONSTANT(ORXMAX, "MAX");
CHARCONSTANT(ORXMIN, "MIN");
CHARCONSTANT(OVERLAY, "OVERLAY");
CHARCONSTANT(POS, "POS");
CHARCONSTANT(QUEUED, "QUEUED");
CHARCONSTANT(RANDOM, "RANDOM");
CHARCONSTANT(REVERSE, "REVERSE");
CHARCONSTANT(RIGHT, "RIGHT");
CHARCONSTANT(SAY, "SAY");
CHARCONSTANT(SIGN, "SIGN");
CHARCONSTANT(SOURCELINE, "SOURCELINE");
CHARCONSTANT(SOURCESIZE, "SOURCESIZE");
CHARCONSTANT(SPACE, "SPACE");
CHARCONSTANT(STREAM, "STREAM");
CHARCONSTANT(STRIP, "STRIP");
CHARCONSTANT(SUBCHAR, "SUBCHAR");
CHARCONSTANT(SUBSTR, "SUBSTR");
CHARCONSTANT(SUBWORD, "SUBWORD");
CHARCONSTANT(SYMBOL, "SYMBOL");
CHARCONSTANT(TIME, "TIME");
CHARCONSTANT(TRACE, "TRACE");
CHARCONSTANT(TRUNC, "TRUNC");
CHARCONSTANT(VALUE, "VALUE");
CHARCONSTANT(VERIFY, "VERIFY");
CHARCONSTANT(WORD, "WORD");
CHARCONSTANT(WORDINDEX, "WORDINDEX");
CHARCONSTANT(WORDLENGTH, "WORDLENGTH");
CHARCONSTANT(WORDPOS, "WORDPOS");
CHARCONSTANT(CASELESSWORDPOS, "CASELESSWORDPOS");
CHARCONSTANT(WORDS, "WORDS");
CHARCONSTANT(X2B, "X2B");
CHARCONSTANT(X2C, "X2C");
CHARCONSTANT(X2D, "X2D");
CHARCONSTANT(XRANGE, "XRANGE");



                              /*  Now KEYWORD string constants  */

CHARCONSTANT(BY,         "BY");
CHARCONSTANT(CASELESS,   "CASELESS");
CHARCONSTANT(DO,         "DO");
CHARCONSTANT(DROP,       "DROP");
CHARCONSTANT(ELSE,       "ELSE");
CHARCONSTANT(END,        "END");
CHARCONSTANT(EXPOSE,     "EXPOSE");
CHARCONSTANT(FAILURE,    "FAILURE");
CHARCONSTANT(FOR,        "FOR");
CHARCONSTANT(FOREVER,    "FOREVER");
CHARCONSTANT(GUARD,      "GUARD");
CHARCONSTANT(IF,         "IF");
CHARCONSTANT(INTERPRET,  "INTERPRET");
CHARCONSTANT(ITERATE,    "ITERATE");
CHARCONSTANT(PULL,       "PULL");
CHARCONSTANT(PUSH,       "PUSH");
CHARCONSTANT(LABEL,      "LABEL");
CHARCONSTANT(LEAVE,      "LEAVE");
CHARCONSTANT(LOOP,       "LOOP");
CHARCONSTANT(LOWER,      "LOWER");
CHARCONSTANT(NOP,        "NOP");
CHARCONSTANT(NOTREADY,   "NOTREADY");
CHARCONSTANT(OBJECT,     "OBJECT");
CHARCONSTANT(OPTIONS,    "OPTIONS");
CHARCONSTANT(OTHERWISE,  "OTHERWISE");
CHARCONSTANT(OVER,       "OVER");
CHARCONSTANT(PRIVATE,    "PRIVATE");
CHARCONSTANT(PROCEDURE,  "PROCEDURE");
CHARCONSTANT(RAISE,      "RAISE");
CHARCONSTANT(REPLY,      "REPLY");
CHARCONSTANT(RETURN,     "RETURN");
CHARCONSTANT(SELECT,     "SELECT");
CHARCONSTANT(THEN,       "THEN");
CHARCONSTANT(TO,         "TO");
CHARCONSTANT(UNTIL,      "UNTIL");
CHARCONSTANT(UPPER,      "UPPER");
CHARCONSTANT(USE,        "USE");
CHARCONSTANT(USER,       "USER");
CHARCONSTANT(WHEN,       "WHEN");
CHARCONSTANT(WHILE,      "WHILE");
CHARCONSTANT(WITH,       "WITH");

CHARCONSTANT(ZERO, "0");

CHARCONSTANT(PUBLIC_ROUTINES, "PUBLIC_ROUTINES");  /* Name for the directory that holds public routines */
#endif
