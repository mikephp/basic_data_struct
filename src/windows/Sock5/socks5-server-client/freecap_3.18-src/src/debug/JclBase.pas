{**************************************************************************************************}
{                                                                                                  }
{ Project JEDI Code Library (JCL)                                                                  }
{                                                                                                  }
{ The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); }
{ you may not use this file except in compliance with the License. You may obtain a copy of the    }
{ License at http://www.mozilla.org/MPL/                                                           }
{                                                                                                  }
{ Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF   }
{ ANY KIND, either express or implied. See the License for the specific language governing rights  }
{ and limitations under the License.                                                               }
{                                                                                                  }
{ The Original Code is JclBase.pas.                                                                }
{                                                                                                  }
{ The Initial Developers of the Original Code are documented in the accompanying help file         }
{ JCLHELP.hlp. Portions created by these individuals are Copyright (C) of these individuals.       }
{                                                                                                  }
{**************************************************************************************************}
{                                                                                                  }
{ This unit contains generic JCL base classes and routines to support earlier                      }
{ versions of Delphi as well as FPC.                                                               }
{                                                                                                  }
{ Unit owner: Marcel van Brakel                                                                    }
{                                                                                                  }
{**************************************************************************************************}

// $Id: JclBase.pas,v 1.1 2005/02/14 19:26:25 bert Exp $

unit JclBase;

{$I jcl.inc}

{$IFDEF SUPPORTS_WEAKPACKAGEUNIT}
  {$WEAKPACKAGEUNIT ON}
{$ENDIF SUPPORTS_WEAKPACKAGEUNIT}

interface

uses
  {$IFDEF MSWINDOWS}
  Windows,
  {$ENDIF MSWINDOWS}
  SysUtils;

//--------------------------------------------------------------------------------------------------
// Version
//--------------------------------------------------------------------------------------------------

const
  JclVersionMajor   = 1;    // 0=pre-release|beta/1, 2, ...=final
  JclVersionMinor   = 90;   // Forth minor release JCL 1.20
  JclVersionRelease = 0;    // 0=pre-release|beta/1=release
  JclVersionBuild   = 1497; // build number, days since march 1, 2000
  JclVersion = (JclVersionMajor shl 24) or (JclVersionMinor shl 16) or
    (JclVersionRelease shl 15) or (JclVersionBuild shl 0);

//--------------------------------------------------------------------------------------------------
// FreePascal Support
//--------------------------------------------------------------------------------------------------

{$IFDEF FPC}

type
  PResStringRec = ^string;

function SysErrorMessage(ErrNo: Integer): string;

{$IFDEF MSWINDOWS}
procedure RaiseLastWin32Error;
function Win32Check(RetVal: BOOL): BOOL;

procedure QueryPerformanceCounter(var C: Int64);
function QueryPerformanceFrequency(var Frequency: Int64): Boolean;
{$ENDIF MSWINDOWS}

var
  Default8087CW: Word;
{$ENDIF FPC}

//--------------------------------------------------------------------------------------------------
// EJclError
//--------------------------------------------------------------------------------------------------

type
  EJclError = class(Exception)
  public
    constructor CreateResRec(ResStringRec: PResStringRec);
    constructor CreateResRecFmt(ResStringRec: PResStringRec; const Args: array of const);
  end;

//--------------------------------------------------------------------------------------------------
// EJclWin32Error
//--------------------------------------------------------------------------------------------------

{$IFDEF MSWINDOWS}

type
  EJclWin32Error = class(EJclError)
  private
    FLastError: DWORD;
    FLastErrorMsg: string;
  public
    constructor Create(const Msg: string);
    constructor CreateFmt(const Msg: string; const Args: array of const);
    constructor CreateRes(Ident: Integer);
    constructor CreateResRec(ResStringRec: PResStringRec);
    property LastError: DWORD read FLastError;
    property LastErrorMsg: string read FLastErrorMsg;
  end;

{$ENDIF MSWINDOWS}

//--------------------------------------------------------------------------------------------------
// Types
//--------------------------------------------------------------------------------------------------

type
  {$IFDEF MATH_EXTENDED_PRECISION}
  Float = Extended;
  {$ENDIF MATH_EXTENDED_PRECISION}
  {$IFDEF MATH_DOUBLE_PRECISION}
  Float = Double;
  {$ENDIF MATH_DOUBLE_PRECISION}
  {$IFDEF MATH_SINGLE_PRECISION}
  Float = Single;
  {$ENDIF MATH_SINGLE_PRECISION}

  PFloat = ^Float;

{$IFDEF FPC}
type
  Largeint    = Int64;
  LongWord    = Cardinal;
  TSysCharSet = set of Char;
{$ENDIF FPC}

type
  PPointer = ^Pointer;

  {$IFNDEF COMPILER6_UP}
  PBoolean = ^Boolean;
  {$ENDIF COMPILER6_UP}


//--------------------------------------------------------------------------------------------------
// Int64 support
//--------------------------------------------------------------------------------------------------

procedure I64ToCardinals(I: Int64; var LowPart, HighPart: Cardinal);
procedure CardinalsToI64(var I: Int64; const LowPart, HighPart: Cardinal);

// Redefinition of TLargeInteger to relieve dependency on Windows.pas

type
  PLargeInteger = ^TLargeInteger;
  TLargeInteger = Int64;

// Redefinition of TULargeInteger to relieve dependency on Windows.pas

type
  PULargeInteger = ^TULargeInteger;
  TULargeInteger = record
    case Integer of
    0:
     (LowPart: LongWord;
      HighPart: LongWord);
    1:
     (QuadPart: Int64);
  end;

//--------------------------------------------------------------------------------------------------
// Dynamic Array support
//--------------------------------------------------------------------------------------------------

type
  TDynByteArray     = array of Byte;
  TDynShortintArray = array of Shortint;
  TDynSmallintArray = array of Smallint;
  TDynWordArray     = array of Word;
  TDynIntegerArray  = array of Integer;
  TDynLongintArray  = array of Longint;
  TDynCardinalArray = array of Cardinal;
  TDynInt64Array    = array of Int64;
  TDynExtendedArray = array of Extended;
  TDynDoubleArray   = array of Double;
  TDynSingleArray   = array of Single;
  TDynFloatArray    = array of Float;
  TDynPointerArray  = array of Pointer;
  TDynStringArray   = array of string;

//--------------------------------------------------------------------------------------------------
// Cross-Platform Compatibility
//--------------------------------------------------------------------------------------------------

{$IFNDEF COMPILER6_UP}
procedure RaiseLastOSError;
{$ENDIF COMPILER6_UP}

//--------------------------------------------------------------------------------------------------
// Interface compatibility
//--------------------------------------------------------------------------------------------------

{$IFDEF SUPPORTS_INTERFACE}
{$IFNDEF COMPILER6_UP}

type
  IInterface = IUnknown;

{$ENDIF COMPILER6_UP}
{$ENDIF SUPPORTS_INTERFACE}

implementation

uses
  JclResources;

//==================================================================================================
// EJclError
//==================================================================================================

constructor EJclError.CreateResRec(ResStringRec: PResStringRec);
begin
  {$IFDEF FPC}
  inherited Create(ResStringRec^);
  {$ELSE FPC}
  inherited Create(LoadResString(ResStringRec));
  {$ENDIF FPC}
end;

constructor EJclError.CreateResRecFmt(ResStringRec: PResStringRec; const Args: array of const);
begin
  {$IFDEF FPC}
  inherited CreateFmt(ResStringRec^, Args);
  {$ELSE FPC}
  inherited CreateFmt(LoadResString(ResStringRec), Args);
  {$ENDIF FPC}
end;

//==================================================================================================
// FreePascal support
//==================================================================================================

{$IFDEF FPC}
{$IFDEF MSWINDOWS}

function SysErrorMessage(ErrNo: Integer): string;
var
  Size: Integer;
  Buffer: PChar;
begin
  GetMem(Buffer, 4000);
  Size := FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM or FORMAT_MESSAGE_ARGUMENT_ARRAY, nil, ErrNo,
    0, Buffer, 4000, nil);
  SetString(Result, Buffer, Size);
end;

//--------------------------------------------------------------------------------------------------

procedure RaiseLastWin32Error;
begin
end;

//--------------------------------------------------------------------------------------------------

function QueryPerformanceFrequency(var Frequency: Int64): Boolean;
var
  T: TULargeInteger;
begin
  Windows.QueryPerformanceFrequency(@T);
  CardinalsToI64(Frequency, T.LowPart, T.HighPart);
end;

//--------------------------------------------------------------------------------------------------

procedure QueryPerformanceCounter(var C: Int64);
var
  T: TULargeInteger;
begin
  Windows.QueryPerformanceCounter(@T);
  CardinalsToI64(C, T.LowPart, T.HighPart);
end;

//--------------------------------------------------------------------------------------------------

function Win32Check(RetVal: BOOL): BOOL;
begin
  if not RetVal then RaiseLastOSError;
  Result := RetVal;
end;

{$ELSE MSWINDOWS}

function SysErrorMessage(ErrNo: Integer): string;
begin
  Result := Format(RsSysErrorMessageFmt, [ErrNo, ErrNo]);
end;

{$ENDIF MSWINDOWS}
{$ENDIF FPC}

//==================================================================================================
// EJclWin32Error
//==================================================================================================

{$IFDEF MSWINDOWS}

constructor EJclWin32Error.Create(const Msg: string);
begin
  FLastError := GetLastError;
  FLastErrorMsg := SysErrorMessage(FLastError);
  inherited CreateFmt(Msg + #13 + RsWin32Prefix, [FLastErrorMsg, FLastError]);
end;

//--------------------------------------------------------------------------------------------------

constructor EJclWin32Error.CreateFmt(const Msg: string; const Args: array of const);
begin
  FLastError := GetLastError;
  FLastErrorMsg := SysErrorMessage(FLastError);
  inherited CreateFmt(Msg + #13 + Format(RsWin32Prefix, [FLastErrorMsg, FLastError]), Args);
end;

//--------------------------------------------------------------------------------------------------

constructor EJclWin32Error.CreateRes(Ident: Integer);
begin
  FLastError := GetLastError;
  FLastErrorMsg := SysErrorMessage(FLastError);
  inherited CreateFmt(LoadStr(Ident) + #13 + RsWin32Prefix, [FLastErrorMsg, FLastError]);
end;

//--------------------------------------------------------------------------------------------------

constructor EJclWin32Error.CreateResRec(ResStringRec: PResStringRec);
begin
  FLastError := GetLastError;
  FLastErrorMsg := SysErrorMessage(FLastError);
  {$IFDEF FPC}
  inherited CreateFmt(ResStringRec^ + #13 + RsWin32Prefix, [FLastErrorMsg, FLastError]);
  {$ELSE FPC}
  inherited CreateFmt(LoadResString(ResStringRec) + #13 + RsWin32Prefix, [FLastErrorMsg, FLastError]);
  {$ENDIF FPC}
end;

{$ENDIF MSWINDOWS}

//==================================================================================================
// Int64 support
//==================================================================================================

procedure I64ToCardinals(I: Int64; var LowPart, HighPart: Cardinal);
begin
  LowPart := TULargeInteger(I).LowPart;
  HighPart := TULargeInteger(I).HighPart;
end;

//--------------------------------------------------------------------------------------------------

procedure CardinalsToI64(var I: Int64; const LowPart, HighPart: Cardinal);
begin
  TULargeInteger(I).LowPart := LowPart;
  TULargeInteger(I).HighPart := HighPart;
end;

//==================================================================================================
// Cross=Platform Compatibility
//==================================================================================================

{$IFNDEF COMPILER6_UP}

procedure RaiseLastOSError;
begin
  RaiseLastWin32Error;
end;

{$ENDIF COMPILER6_UP}

end.
