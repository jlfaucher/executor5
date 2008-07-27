/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2006 Rexx Language Association. All rights reserved.    */
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
/* REXX Translator                                              ParseTarget.c    */
/*                                                                            */
/* Primitive Procedure Parse Trigger Class                                    */
/*                                                                            */
/******************************************************************************/
#include <stdlib.h>
#include "RexxCore.h"
#include "StringClass.hpp"
#include "ArrayClass.hpp"
#include "RexxActivation.hpp"
#include "ParseTarget.hpp"
#include "ParseInstruction.hpp"

#include "ASCIIDBCSStrings.hpp"

                                       /* current global settings           */
extern ACTIVATION_SETTINGS *current_settings;

void RexxTarget::init(
    RexxObject     *string,            /* target string                     */
    RexxObject    **arglist,           /* argument list                     */
    size_t          argcount,          /* size of the argument list         */
    BOOL            translate,         /* translation flag                  */
    BOOL            multiple,          /* have multiple strings             */
    RexxActivation *context,           /* execution context                 */
    RexxExpressionStack *s)            /* current expression stack          */
/******************************************************************************/
/* Function:  Initialize a parse target object                                */
/******************************************************************************/
{
  this->translate = translate;         /* save the translation flag         */
  this->arglist = arglist;             /* we have an array of strings       */
  this->argcount = argcount;
  this->string = (RexxString *)string; /* save the string also              */
  this->next_argument = 1;             /* start with the first argument     */
  this->stack = s;                     // save the expression stack for saving object references in
  this->stackTop = s->location();      // save the stack top for resets
  this->next(context);                 /* go perform needed resets          */
}

void RexxTarget::next(
    RexxActivation *context)           /* current execution context         */
/******************************************************************************/
/* Function:  Step to the "next" string to parse, resetting all of the        */
/*            cursor movement values.                                         */
/******************************************************************************/
{
  if (this->arglist != OREF_NULL) {    /* have an argument list?            */
                                       /* beyond the array bounds?          */
    if (this->next_argument > this->argcount)
      this->string = OREF_NULLSTRING;  /* just use a null string            */
    else {
                                       /* get the first element             */
      this->string = (RexxString *)this->arglist[this->next_argument - 1];
      if (this->string == OREF_NULL)   /* omitted argument?                 */
        this->string = OREF_NULLSTRING;/* use the null string               */
    }
  }
  else {
    if (this->next_argument != 1)      /* beyond the first argument         */
      this->string = OREF_NULLSTRING;  /* just use a null string            */
  }
  this->next_argument++;               /* bump the argument position        */
                                       /* make sure this is a string        */
  this->string = (RexxString *)REQUEST_STRING(this->string);
  if (this->translate == parse_upper)  /* need to uppercase?                */
                                       /* fold it up                        */
    this->string = this->string->upper();
                                       /* need to lowercase?                */
  else if (this->translate == parse_lower)
                                       /* down we go                        */
    this->string = this->string->lower();

  // reset the stack to the entry top, and push this value on to protect it.
  this->stack->setTop(this->stackTop);
  this->stack->push(string);
  this->start = 0;                     /* start at the beginning            */
  this->pattern_end = 0;               /* no pattern done yet               */
  this->pattern_start = 0;             /* save the pattern start            */
                                       /* save the over all length          */
  this->string_length = this->string->length;
  this->subcurrent = 0;                /* no sub piece to process yet       */
}

void RexxTarget::moveToEnd()
/******************************************************************************/
/* Function:  Move the current cursor to the string end                       */
/******************************************************************************/
{
  this->start = this->pattern_end;     /* start from end of last pattern    */
                                       /* pattern at the end too            */
  this->pattern_end = this->string_length;
                                       /* no pattern length either          */
  this->pattern_start = this->string_length;
  this->end = this->string_length;     /* string end is at end also         */
                                       /* save the over all length          */
                                       /* no sub piece to process yet       */
  this->subcurrent = this->start;      /* set starting point                */
}

void RexxTarget::forward(
    long offset)                       /* offset to move                    */
/******************************************************************************/
/* Arguments:  distance to move the parse pointer                             */
/******************************************************************************/
{
  this->start = this->pattern_start;   /* start position is last position   */
  if (DBCS_MODE) {                     /* need to do this in DBCS mode?     */
                                       /* increment position         */
    this->end = this->string->DBCSmovePointer(this->start, INCREMENT, offset);
  }
  else
    this->end = this->start + offset;  /* set the end position              */
  if (this->end >= this->string_length)/* take us past the end?             */
    this->end = this->string_length;   /* just use the end position         */
  if (this->end <= this->start) {      /* no forward movement?              */
    this->end = this->string_length;   /* just use the end position         */
    this->pattern_start = this->start; /* start here for the next one       */
  }
  else                                 /* normal movement                   */
    this->pattern_start = this->end;   /* this is new start position        */
                                       /* and have a zero length pattern    */
  this->pattern_end = this->pattern_start;
  this->subcurrent = this->start;      /* set the subpiece pointer          */
}

void RexxTarget::forwardLength(
    long offset)                       /* offset to move                    */
/******************************************************************************/
/* Arguments:  distance to move the parse pointer                             */
/******************************************************************************/
{
  this->start = this->pattern_start;   /* start position is last position   */
  this->end = this->start + offset;    /* set the end position              */
  if (this->end >= this->string_length)/* take us past the end?             */
    this->end = this->string_length;   /* just use the end position         */
  this->pattern_start = this->end;     /* this is new start position        */
                                       /* and have a zero length pattern    */
  this->pattern_end = this->pattern_start;
  this->subcurrent = this->start;      /* set the subpiece pointer          */
}


void RexxTarget::absolute(
    long offset)                       /* offset to move                    */
/******************************************************************************/
/* Arguments:  new parse position                                             */
/******************************************************************************/
{
  if (offset > 0)                      /* positive offset?                  */
    offset--;                          /* make origin zero                  */
  this->start = this->pattern_end;     /* start position is last position   */
  if (DBCS_MODE) {                     /* need to do this in DBCS mode?     */
                                       /* find actual character position    */
    offset = this->string->DBCSmovePointer(0, INCREMENT, offset);
  }
  if ((size_t)offset <= this->start) { /* backward movement?                */
    this->end = this->string_length;   /* matches to the end                */
    this->pattern_start = offset;      /* pattern start is actual position  */
  }
  else {                               /* forward movement                  */
    this->end = offset;                /* use the specified position        */
                                       /* take us past the end?             */
    if (this->end >= this->string_length)
                                       /* just use the end position         */
      this->end = this->string_length;
    this->pattern_start = this->end;   /* this is new start position        */
  }
                                       /* and have a zero length pattern    */
  this->pattern_end = this->pattern_start;
  this->subcurrent = this->start;      /* set the subpiece pointer          */
}

void RexxTarget::backward(
    long offset)                       /* offset to move                    */
/******************************************************************************/
/* Arguments:  distance to move the parse pointer                             */
/******************************************************************************/
{
  this->start = this->pattern_start;   /* start position is last position   */
  this->end = this->string_length;     /* negatives always use to the end   */
  if (DBCS_MODE) {                     /* need to do this in DBCS mode?     */
                                       /* increment position         */
    this->pattern_start = this->string->DBCSmovePointer(this->pattern_start, DECREMENT, offset);
  }
  else {
    /* go past start of string?          */
    if ((size_t)offset > this->pattern_start)
      this->pattern_start = 0;         /* this resets to the start          */
    else
      this->pattern_start -= offset;   /* just back up                      */
  }
                                       /* and have a zero length pattern    */
  this->pattern_end = this->pattern_start;
  this->subcurrent = this->start;      /* set the subpiece pointer          */
}


void RexxTarget::backwardLength(
    long offset)                       /* offset to move                    */
/******************************************************************************/
/* Arguments:  distance to move the parse pointer                             */
/******************************************************************************/
{
  this->start = this->pattern_start;   /* start position is last position   */
  this->end = this->string_length;     /* negatives always use to the end   */
                                       /* go past start of string?          */
  if ((size_t)offset > this->pattern_start)
  {
      this->start = 0;
  }
  else
  {
      this->start = this->pattern_start - (size_t)offset;
  }
  this->end = this->pattern_start;     // the end is the starting location
                                       /* and have a zero length pattern    */
  this->pattern_end = this->pattern_start;
  this->subcurrent = this->start;      /* set the subpiece pointer          */
}


void RexxTarget::search(
    RexxString *string)                /* target search string              */
/******************************************************************************/
/* Arguments:  target location string                                         */
/******************************************************************************/
{
                                       /* start position for strings is the */
  this->start = this->pattern_end;     /* end of the last pattern           */
                                       /* search for the string trigger     */
  /* use DBCS character count as index into DBCS string */
  if (DBCS_MODE) {
    this->end = this->string->pos(string,
            DBCS_CharacterCount((PUCHAR)this->string->stringData, this->start));
  } else {
    this->end = this->string->pos(string, this->start);
  }
  if (this->end == 0) {                /* not found?                        */
    this->end = this->string_length;   /* that is the end position          */
                                       /* next pattern is end also          */
    this->pattern_start = this->string_length;
                                       /* and the end pattern is also there */
    this->pattern_end = this->string_length;
  }
  else {
    this->end--;                       /* convert to origin zero            */
    if (DBCS_MODE)                     /* need to do this in DBCS mode?     */
                                       /* increment position         */
      this->end = this->string->DBCSmovePointer(0, INCREMENT, this->end);
    this->pattern_start = this->end;   /* this is the starting point        */
                                       /* end is start + trigger length     */
    this->pattern_end = this->pattern_start + string->length;
  }
  this->subcurrent = this->start;      /* set the subpiece pointer          */
}

void RexxTarget::caselessSearch(
    RexxString *string)                /* target search string              */
/******************************************************************************/
/* Arguments:  target location string                                         */
/******************************************************************************/
{
                                       /* start position for strings is the */
  this->start = this->pattern_end;     /* end of the last pattern           */
                                       /* search for the string trigger     */
  /* use DBCS character count as index into DBCS string */
  if (DBCS_MODE) {
    this->end = this->string->caselessPos(string,
              DBCS_CharacterCount((PUCHAR)this->string->stringData, this->start));
  } else {
    this->end = this->string->caselessPos(string, this->start);
  }
  if (this->end == 0) {                /* not found?                        */
    this->end = this->string_length;   /* that is the end position          */
                                       /* next pattern is end also          */
    this->pattern_start = this->string_length;
                                       /* and the end pattern is also there */
    this->pattern_end = this->string_length;
  }
  else {
    this->end--;                       /* convert to origin zero            */
    if (DBCS_MODE)                     /* need to do this in DBCS mode?     */
                                       /* increment position         */
      this->end = this->string->DBCSmovePointer(0, INCREMENT, this->end);
    this->pattern_start = this->end;   /* this is the starting point        */
                                       /* end is start + trigger length     */
    this->pattern_end = this->pattern_start + string->length;
  }
  this->subcurrent = this->start;      /* set the subpiece pointer          */
}

RexxString *RexxTarget::getWord()
/******************************************************************************/
/*  Returned:  Next word extracted from parsed substring                      */
/******************************************************************************/
{
  RexxString *word;                    /* extracted word                    */
  size_t  string_start;                /* string starting offset            */
  size_t  length;                      /* word length                       */
  PUCHAR  scan;                        /* scan pointer                      */
  PUCHAR  endScan;                     /* end of string location            */

  if (this->subcurrent >= this->end)   /* already used up?                  */
    word = OREF_NULLSTRING;            /* just return a null string         */
  else {                               /* need to scan off a word           */
    if (DBCS_MODE) {                   /* need DBCS mode?                   */
                                       /* point to the current position     */
      scan = (PUCHAR)this->string->stringData + this->subcurrent;
                                       /* get the subpiece length           */
      length = this->end - this->subcurrent;
      DBCS_SkipBlanks(&scan, &length); /* skip over blanks                  */
      if (length == 0)                 /* blanks only?                      */
        word = OREF_NULLSTRING;        /* assign a null string              */
      else {                           /* have a real word                  */
                                       /* save word start                   */
        string_start = scan - (PUCHAR)(this->string->stringData);

        DBCS_SkipNonBlanks(&scan, &length);
                                       /* get the ending position           */
        this->subcurrent = scan - (PUCHAR)(this->string->stringData);
                                        /* get the subpiece length           */
        length = this->subcurrent - string_start;
                                       /* extract the subpiece              */
        word = this->string->extract(string_start, length);
        if (length) {                  /* length left?                      */
          if (IsDBCSBlank(scan))       /* DBCS blank?                       */
                                       /* step over longer char             */
            this->subcurrent += DBCS_BYTELEN;
          else
            this->subcurrent++;        /* gobble one space                  */
        }
                                       /* skip over rest of word            */
      }
      return word;                     /* return the word                   */
    }
                                       /* point to the current position     */
    scan = (PUCHAR)(this->string->stringData) + this->subcurrent;
                                       /* and the scan end point            */
    endScan = (PUCHAR)(this->string->stringData) + this->end;
    /* NOTE:  All string objects have a terminating NULL, so the */
    /* scan for nonblanks is guaranteed to stop before getting into */
    /* trouble, which eliminates the need to check against the */
    /* length */
    while (*scan == ' ' || *scan == '\t') {
      scan++;                          /* step for each match found         */
    }
                                       /* set the new location              */
    this->subcurrent = scan - (PUCHAR)(this->string->stringData);
    if (this->subcurrent >= this->end) /* already used up?                  */
      word = OREF_NULLSTRING;          /* just return a null string         */
    else {                             /* have a real word                  */
                                       /* look for the next blank           */
      endScan = NULL;
      PUCHAR scanner = scan;
      PUCHAR endPosition = (PUCHAR)string->stringData + this->end;
      while (scanner < endPosition)
      {
          if (*scanner == ' ' || *scanner == '\t')
          {
              endScan = scanner;
              break;
          }
          scanner++;
      }
      if (endScan == NULL) {           /* no match?                         */
                                       /* calculate the length              */
        length = this->end - this->subcurrent;
        this->subcurrent = this->end;  /* use the rest of it                */
      }
      else {
                                       /* set the new location              */
        this->subcurrent = endScan - (PUCHAR)(this->string->stringData);
        length = endScan - scan;       /* calculate from the pointers       */
      }
                                       /* step past terminating blank...note*/
                                       /* that this is done unconditionally,*/
                                       /* but safely, since the check at the*/
                                       /* start will catch the out of bounds*/
      this->subcurrent++;              /* condition                         */
                                       /* this the entire string?           */
      if (length == this->string_length)
        word = this->string;           /* just return it directly           */
      else
                                       /* extract the subpiece              */
        word = new_string((PCHAR)scan, length);
    }
  }
  return word;                         /* give this word back               */
}

void RexxTarget::skipWord()
/******************************************************************************/
/*  Returned:  Next word extracted from parsed substring                      */
/******************************************************************************/
{
  size_t  length;                      /* scanned over length               */
  PUCHAR  scan;                        /* scan pointer                      */
  PUCHAR  endScan;                     /* end of string location            */

  if (this->subcurrent < this->end) {  /* something left?                   */
    if (DBCS_MODE) {                   /* need DBCS mode?                   */
                                       /* point to the current position     */
      scan = (PUCHAR)this->string->stringData + this->subcurrent;
                                       /* get the subpiece length           */
      length = this->end - this->subcurrent;
      DBCS_SkipBlanks(&scan, &length); /* skip over blanks                  */
      if (length != 0) {               /* something there?                  */
                                       /* skip over rest of word            */
        DBCS_SkipNonBlanks(&scan, &length);
                                       /* get the ending position           */
        this->subcurrent = scan - (PUCHAR)(this->string->stringData);
                                       /* length left?                      */
        if (this->subcurrent < this->end) {
          if (IsDBCSBlank(scan))       /* DBCS blank?                       */
                                       /* step over longer char             */
            this->subcurrent += DBCS_BYTELEN;
          else
            this->subcurrent++;        /* gobble one space                  */
        }
      }
      return;                          /* return the word                   */
    }
                                       /* point to the current position     */
    scan = (PUCHAR)(this->string->stringData) + this->subcurrent;
                                       /* and the scan end point            */
    endScan = (PUCHAR)(this->string->stringData) + this->end;
    /* NOTE:  All string objects have a terminating NULL, so the */
    /* scan for nonblanks is guaranteed to stop before getting into */
    /* trouble, which eliminates the need to check against the */
    /* length */
    while (*scan == ' ' || *scan == '\t') {
      scan++;                          /* step for each match found         */
    }
                                       /* set the new location              */
    this->subcurrent = scan - (PUCHAR)(this->string->stringData);
    if (this->subcurrent < this->end) {/* something left over?              */
                                       /* look for the next blank           */
      endScan = NULL;
      PUCHAR scanner = scan;
      PUCHAR endPosition = (PUCHAR)string->stringData + this->end;
      while (scanner < endPosition)
      {
          if (*scanner == ' ' || *scanner == '\t')
          {
              endScan = scanner;
              break;
          }
          scanner++;
      }
      if (endScan == NULL)             /* no match?                         */
        this->subcurrent = this->end;  /* use the rest of it                */
      else
                                       /* set the new location              */
        this->subcurrent = endScan - (PUCHAR)(this->string->stringData);
                                       /* step past terminating blank...note*/
                                       /* that this is done unconditionally,*/
                                       /* but safely, since the check at the*/
                                       /* start will catch the out of bounds*/
      this->subcurrent++;              /* condition                         */
    }
  }
}

RexxString *RexxTarget::remainder()
/******************************************************************************/
/*  Returned:  Remaining portion of parsed substring                          */
/******************************************************************************/
{
  RexxString *word;                    /* extracted word                    */
  size_t  length;                      /* length to extract                 */

  if (this->subcurrent >= this->end)   /* already used up?                  */
    word = OREF_NULLSTRING;            /* just return a null string         */
  else {                               /* extract the remaining piece       */
                                       /* calculate the length              */
    length = this->end - this->subcurrent;
    if (length == this->string_length) /* this the entire string?           */
      word = this->string;             /* just return it directly           */
    else                               /* need to extract a piece           */
      word = this->string->extract(this->subcurrent, length);
    this->subcurrent = this->end;      /* eat the remainder piece           */
  }
  return word;                         /* give this word back               */
}
