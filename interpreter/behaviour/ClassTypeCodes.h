

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
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
/******************************************************************************/
/* REXX  Support                                                              */
/*                                                                            */
/* Rexx primitive class id codes                                              */
/*                                                                            */
/*        -- DO NOT CHANGE THIS FILE, ALL CHANGES WILL BE LOST! --            */
/*                  -- FILE WAS GENERATED BY ClassTypeIds.xsl --              */
/******************************************************************************/

#ifndef ClassTypeCodes_Included
#define ClassTypeCodes_Included

typedef enum
{
    T_First_Primitive_Class = 0,
    T_First_Exported_Class = 0,

    T_Object = 0,
    T_ObjectClass = 1,
    T_Class = 2,
    T_ClassClass = 3,
    T_Array = 4,
    T_ArrayClass = 5,
    T_Directory = 6,
    T_DirectoryClass = 7,
    T_Integer = 8,
    T_IntegerClass = 9,
    T_List = 10,
    T_ListClass = 11,
    T_Message = 12,
    T_MessageClass = 13,
    T_Method = 14,
    T_MethodClass = 15,
    T_NumberString = 16,
    T_NumberStringClass = 17,
    T_Queue = 18,
    T_QueueClass = 19,
    T_Stem = 20,
    T_StemClass = 21,
    T_String = 22,
    T_StringClass = 23,
    T_Supplier = 24,
    T_SupplierClass = 25,
    T_Table = 26,
    T_TableClass = 27,
    T_Relation = 28,
    T_RelationClass = 29,
    T_MutableBuffer = 30,
    T_MutableBufferClass = 31,
    T_Pointer = 32,
    T_PointerClass = 33,
    T_Buffer = 34,
    T_BufferClass = 35,
    T_WeakReference = 36,
    T_WeakReferenceClass = 37,
    T_Routine = 38,
    T_RoutineClass = 39,
    T_Package = 40,
    T_PackageClass = 41,
    T_RexxContext = 42,
    T_RexxContextClass = 43,
    T_IdentityTable = 44,
    T_IdentityTableClass = 45,

    T_Last_Exported_Class = 45,
    
    T_First_Internal_Class = 46,

    T_NilObject = 46,
    T_Behaviour = 47,
    T_RexxSource = 48,
    T_LibraryPackage = 49,
    T_RexxCode = 50,
    T_NativeMethod = 51,
    T_NativeRoutine = 52,
    T_RegisteredRoutine = 53,
    T_CPPCode = 54,
    T_AttributeGetterCode = 55,
    T_AttributeSetterCode = 56,
    T_ConstantGetterCode = 57,
    T_AbstractCode = 58,
    T_HashTable = 59,
    T_ListTable = 60,
    T_SmartBuffer = 61,
    T_Variable = 62,
    T_VariableDictionary = 63,
    T_VariableTerm = 64,
    T_CompoundVariableTerm = 65,
    T_StemVariableTerm = 66,
    T_DotVariableTerm = 67,
    T_IndirectVariableTerm = 68,
    T_FunctionCallTerm = 69,
    T_MessageSendTerm = 70,
    T_UnaryOperatorTerm = 71,
    T_BinaryOperatorTerm = 72,
    T_LogicalTerm = 73,
    T_Instruction = 74,
    T_AddressInstruction = 75,
    T_AssignmentInstruction = 76,
    T_CallInstruction = 77,
    T_CommandInstruction = 78,
    T_DoInstruction = 79,
    T_DropInstruction = 80,
    T_ElseInstruction = 81,
    T_EndInstruction = 82,
    T_EndIfInstruction = 83,
    T_ExitInstruction = 84,
    T_ExposeInstruction = 85,
    T_ForwardInstruction = 86,
    T_GuardInstruction = 87,
    T_IfInstruction = 88,
    T_InterpretInstruction = 89,
    T_LabelInstruction = 90,
    T_LeaveInstruction = 91,
    T_MessageInstruction = 92,
    T_NopInstruction = 93,
    T_NumericInstruction = 94,
    T_OptionsInstruction = 95,
    T_OtherwiseInstruction = 96,
    T_ParseInstruction = 97,
    T_ProcedureInstruction = 98,
    T_QueueInstruction = 99,
    T_RaiseInstruction = 100,
    T_ReplyInstruction = 101,
    T_ReturnInstruction = 102,
    T_SayInstruction = 103,
    T_SelectInstruction = 104,
    T_SignalInstruction = 105,
    T_ThenInstruction = 106,
    T_TraceInstruction = 107,
    T_UseInstruction = 108,
    T_ClassDirective = 109,
    T_LibraryDirective = 110,
    T_RequiresDirective = 111,
    T_CompoundElement = 112,
    T_ParseTrigger = 113,

    T_Last_Internal_Class = 113,
    
    T_First_Transient_Class = 114,

    T_Memory = 114,
    T_InternalStack = 115,
    T_Stack = 116,
    T_Activity = 117,
    T_Activation = 118,
    T_NativeActivation = 119,
    T_ActivationFrameBuffer = 120,
    T_Envelope = 121,
    T_Clause = 122,
    T_Token = 123,
    T_DoBlock = 124,
    T_InterpreterInstance = 125,
    T_SecurityManager = 126,

    T_Last_Transient_Class = 126,
    T_Last_Primitive_Class = 126,
    T_Last_Class_Type = 126,
    
} ClassTypeCode;

/* -------------------------------------------------------------------------- */
/* --            ==================================================        -- */
/* --            DO NOT CHANGE THIS FILE, ALL CHANGES WILL BE LOST!        -- */
/* --            ==================================================        -- */
/* -------------------------------------------------------------------------- */
#endif

