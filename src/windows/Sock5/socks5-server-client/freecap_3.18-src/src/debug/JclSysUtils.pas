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
{ The Original Code is JclSysUtils.pas.                                                            }
{                                                                                                  }
{ The Initial Developers of the Original Code are documented in the accompanying help file         }
{ JCLHELP.hlp. Portions created by these individuals are Copyright (C) of these individuals.       }
{                                                                                                  }
{**************************************************************************************************}
{                                                                                                  }
{ Description: Various pointer and class related routines.                                         }
{ Unit Owner: Jeroen Speldekamp                                                                    }
{                                                                                                  }
{**************************************************************************************************}

// $Id: JclSysUtils.pas,v 1.1 2005/02/14 19:26:26 bert Exp $

unit JclSysUtils;

{$I jcl.inc}

interface

uses
  {$IFDEF MSWINDOWS}
  Windows,
  {$ENDIF MSWINDOWS}
  Classes, TypInfo,
  JclBase;

//--------------------------------------------------------------------------------------------------
// Pointer manipulation
//--------------------------------------------------------------------------------------------------

procedure GetAndFillMem(var P: Pointer; const Size: Integer; const Value: Byte);
procedure FreeMemAndNil(var P: Pointer);
function PCharOrNil(const S: AnsiString): PAnsiChar;
{$IFDEF SUPPORTS_WIDESTRING}
function PWideCharOrNil(const W: WideString): PWideChar;
{$ENDIF SUPPORTS_WIDESTRING}

function SizeOfMem(const APointer: Pointer): Integer;

function WriteProtectedMemory(BaseAddress, Buffer: Pointer; Size: Cardinal;
  out WrittenBytes: Cardinal): Boolean;

//--------------------------------------------------------------------------------------------------
// Guards
//--------------------------------------------------------------------------------------------------

type
  ISafeGuard = interface
    function ReleaseItem: Pointer;
    function GetItem: Pointer;
    procedure FreeItem;
    property Item: Pointer read GetItem;
  end;

  IMultiSafeGuard = interface (IInterface)
    function AddItem(Item: Pointer): Pointer;
    procedure FreeItem(Index: Integer);
    function GetCount: Integer;
    function GetItem(Index: Integer): Pointer;
    function ReleaseItem(Index: Integer): Pointer;
    property Count: Integer read GetCount;
    property Items[Index: Integer]: Pointer read GetItem;
  end;

function Guard(Mem: Pointer; out SafeGuard: ISafeGuard): Pointer; overload;
function Guard(Obj: TObject; out SafeGuard: ISafeGuard): TObject; overload;

function Guard(Mem: Pointer; var SafeGuard: IMultiSafeGuard): Pointer; overload;
function Guard(Obj: TObject; var SafeGuard: IMultiSafeGuard): TObject; overload;

function GuardGetMem(Size: Cardinal; out SafeGuard: ISafeGuard): Pointer;
function GuardAllocMem(Size: Cardinal; out SafeGuard: ISafeGuard): Pointer;

//--------------------------------------------------------------------------------------------------
// Binary search
//--------------------------------------------------------------------------------------------------

function SearchSortedList(List: TList; SortFunc: TListSortCompare; Item: Pointer;
  Nearest: Boolean = False): Integer;

type
  TUntypedSearchCompare = function(Param: Pointer; ItemIndex: Integer; const Value): Integer;

function SearchSortedUntyped(Param: Pointer; ItemCount: Integer; SearchFunc: TUntypedSearchCompare;
  const Value; Nearest: Boolean = False): Integer;

//--------------------------------------------------------------------------------------------------
// Dynamic array sort and search routines
//--------------------------------------------------------------------------------------------------

type
  TDynArraySortCompare = function (Item1, Item2: Pointer): Integer;

procedure SortDynArray(const ArrayPtr: Pointer; ElementSize: Cardinal; SortFunc: TDynArraySortCompare);
// Usage: SortDynArray(Array, SizeOf(Array[0]), SortFunction);
function SearchDynArray(const ArrayPtr: Pointer; ElementSize: Cardinal; SortFunc: TDynArraySortCompare;
  ValuePtr: Pointer; Nearest: Boolean = False): Integer;
// Usage: SearchDynArray(Array, SizeOf(Array[0]), SortFunction, @SearchedValue);

{ Various compare functions for basic types }

function DynArrayCompareByte(Item1, Item2: Pointer): Integer;
function DynArrayCompareShortInt(Item1, Item2: Pointer): Integer;
function DynArrayCompareWord(Item1, Item2: Pointer): Integer;
function DynArrayCompareSmallInt(Item1, Item2: Pointer): Integer;
function DynArrayCompareInteger(Item1, Item2: Pointer): Integer;
function DynArrayCompareCardinal(Item1, Item2: Pointer): Integer;
function DynArrayCompareInt64(Item1, Item2: Pointer): Integer;

function DynArrayCompareSingle(Item1, Item2: Pointer): Integer;
function DynArrayCompareDouble(Item1, Item2: Pointer): Integer;
function DynArrayCompareExtended(Item1, Item2: Pointer): Integer;
function DynArrayCompareFloat(Item1, Item2: Pointer): Integer;

function DynArrayCompareAnsiString(Item1, Item2: Pointer): Integer;
function DynArrayCompareAnsiText(Item1, Item2: Pointer): Integer;
function DynArrayCompareString(Item1, Item2: Pointer): Integer;
function DynArrayCompareText(Item1, Item2: Pointer): Integer;

//--------------------------------------------------------------------------------------------------
// Object lists
//--------------------------------------------------------------------------------------------------

procedure ClearObjectList(List: TList);
procedure FreeObjectList(var List: TList);

//--------------------------------------------------------------------------------------------------
// Reference memory stream
//--------------------------------------------------------------------------------------------------

type
  TJclReferenceMemoryStream = class (TCustomMemoryStream)
  public
    constructor Create(const Ptr: Pointer; Size: Longint);
    function Write(const Buffer; Count: Longint): Longint; override;
  end;

//--------------------------------------------------------------------------------------------------
// Replacement for the C ternary conditional operator ? :
//--------------------------------------------------------------------------------------------------

function Iff(const Condition: Boolean; const TruePart, FalsePart: string): string; overload;
function Iff(const Condition: Boolean; const TruePart, FalsePart: Char): Char; overload;
function Iff(const Condition: Boolean; const TruePart, FalsePart: Byte): Byte; overload;
function Iff(const Condition: Boolean; const TruePart, FalsePart: Integer): Integer; overload;
function Iff(const Condition: Boolean; const TruePart, FalsePart: Cardinal): Cardinal; overload;
function Iff(const Condition: Boolean; const TruePart, FalsePart: Float): Float; overload;
function Iff(const Condition: Boolean; const TruePart, FalsePart: Boolean): Boolean; overload;
function Iff(const Condition: Boolean; const TruePart, FalsePart: Pointer): Pointer; overload;
function Iff(const Condition: Boolean; const TruePart, FalsePart: Int64): Int64; overload;
{$IFDEF COMPILER6_UP}
function Iff(const Condition: Boolean; const TruePart, FalsePart: Variant): Variant; overload;
{$ENDIF COMPILER6_UP}

//--------------------------------------------------------------------------------------------------
// Classes information and manipulation
//--------------------------------------------------------------------------------------------------

type
  EJclVMTError = class (EJclError);

//--------------------------------------------------------------------------------------------------
// Virtual Methods
//--------------------------------------------------------------------------------------------------

{$IFNDEF FPC}
function GetVirtualMethodCount(AClass: TClass): Integer;
{$ENDIF}
function GetVirtualMethod(AClass: TClass; const Index: Integer): Pointer;
procedure SetVirtualMethod(AClass: TClass; const Index: Integer; const Method: Pointer);

//--------------------------------------------------------------------------------------------------
// Dynamic Methods
//--------------------------------------------------------------------------------------------------

type
  TDynamicIndexList = array [0..MaxInt div 16] of Word;
  PDynamicIndexList = ^TDynamicIndexList;
  TDynamicAddressList = array [0..MaxInt div 16] of Pointer;
  PDynamicAddressList = ^TDynamicAddressList;

function GetDynamicMethodCount(AClass: TClass): Integer;
function GetDynamicIndexList(AClass: TClass): PDynamicIndexList;
function GetDynamicAddressList(AClass: TClass): PDynamicAddressList;
function HasDynamicMethod(AClass: TClass; Index: Integer): Boolean;
{$IFNDEF FPC}
function GetDynamicMethod(AClass: TClass; Index: Integer): Pointer;
{$ENDIF}

{ init table methods }

function GetInitTable(AClass: TClass): PTypeInfo;

{ field table methods }

type
  PFieldEntry = ^TFieldEntry;
  TFieldEntry = packed record
    OffSet: Integer;
    IDX: Word;
    Name: ShortString;
  end;

  PFieldClassTable = ^TFieldClassTable;
  TFieldClassTable = packed record
    Count: Smallint;
    Classes: array [0..8191] of ^TPersistentClass;
  end;

  PFieldTable = ^TFieldTable;
  TFieldTable = packed record
    EntryCount: Word;
    FieldClassTable: PFieldClassTable;
    FirstEntry: TFieldEntry;
   {Entries: array [1..65534] of TFieldEntry;}
  end;

function GetFieldTable(AClass: TClass): PFieldTable;

{ method table }

type
  PMethodEntry = ^TMethodEntry;
  TMethodEntry = packed record
    EntrySize: Word;
    Address: Pointer;
    Name: ShortString;
  end;

  PMethodTable = ^TMethodTable;
  TMethodTable = packed record
    Count: Word;
    FirstEntry: TMethodEntry;
   {Entries: array [1..65534] of TMethodEntry;}
  end;

function GetMethodTable(AClass: TClass): PMethodTable;
function GetMethodEntry(MethodTable: PMethodTable; Index: Integer): PMethodEntry;

//--------------------------------------------------------------------------------------------------
// Class Parent
//--------------------------------------------------------------------------------------------------

procedure SetClassParent(AClass: TClass; NewClassParent: TClass);
function GetClassParent(AClass: TClass): TClass;

{$IFNDEF FPC}
function IsClass(Address: Pointer): Boolean;
function IsObject(Address: Pointer): Boolean;
{$ENDIF}

//--------------------------------------------------------------------------------------------------
// Interface information
//--------------------------------------------------------------------------------------------------

function GetImplementorOfInterface(const I: IInterface): TObject;

//--------------------------------------------------------------------------------------------------
// Numeric formatting routines
//--------------------------------------------------------------------------------------------------

type
  TDigitCount = 0..255;
  TDigitValue = -1..35;  // invalid, '0'..'9', 'A'..'Z'
  TNumericSystemBase = 2..Succ(High(TDigitValue));

  TJclNumericFormat = class(TObject)
  private
    FWantedPrecision: TDigitCount;
    FPrecision: TDigitCount;
    FNumberOfFractionalDigits: TDigitCount;
    FExpDivision: Integer;
    FDigitBlockSize: TDigitCount;
    FWidth: TDigitCount;
    FSignChars: array[Boolean] of Char;
    FBase: TNumericSystemBase;
    FFractionalPartSeparator: Char;
    FDigitBlockSeparator: Char;
    FShowPositiveSign: Boolean;
    FPaddingChar: Char;
    FMultiplier: string;
    function GetDigitValue(Digit: Char): Integer;
    function GetNegativeSign: Char;
    function GetPositiveSign: Char;
    procedure InvalidDigit(Digit: Char);
    procedure SetPrecision(const Value: TDigitCount);
    procedure SetBase(const Value: TNumericSystemBase);
    procedure SetNegativeSign(const Value: Char);
    procedure SetPositiveSign(const Value: Char);
    procedure SetExpDivision(const Value: Integer);
  protected
    function IntToStr(const Value: Int64; out FirstDigitPos: Integer): string; overload;
    function ShowSign(const Value: Float): Boolean; overload;
    function ShowSign(const Value: Int64): Boolean; overload;
    function SignChar(const Value: Float): Char; overload;
    function SignChar(const Value: Int64): Char; overload;
    property WantedPrecision: TDigitCount read FWantedPrecision;
  public
    constructor Create;
    function Digit(DigitValue: TDigitValue): Char;
    function DigitValue(Digit: Char): TDigitValue;
    function IsDigit(Value: Char): Boolean;
    function Sign(Value: Char): Integer;
    procedure GetMantissaExp(const Value: Float; out Mantissa: string; out Exponent: Integer);
    function FloatToHTML(const Value: Float): string;
    function IntToStr(const Value: Int64): string; overload;
    function FloatToStr(const Value: Float): string; overload;
    function StrToInt(const Value: string): Int64;
    property Base: TNumericSystemBase read FBase write SetBase;
    property Precision: TDigitCount read FPrecision write SetPrecision;
    property NumberOfFractionalDigits: TDigitCount read FNumberOfFractionalDigits write FNumberOfFractionalDigits;
    property ExponentDivision: Integer read FExpDivision write SetExpDivision;
    property DigitBlockSize: TDigitCount read FDigitBlockSize write FDigitBlockSize;
    property DigitBlockSeparator: Char read FDigitBlockSeparator write FDigitBlockSeparator;
    property FractionalPartSeparator: Char read FFractionalPartSeparator write FFractionalPartSeparator;
    property Multiplier: string read FMultiplier write FMultiplier;
    property PaddingChar: Char read FPaddingChar write FPaddingChar;
    property ShowPositiveSign: Boolean read FShowPositiveSign write FShowPositiveSign;
    property Width: TDigitCount read FWidth write FWidth;
    property NegativeSign: Char read GetNegativeSign write SetNegativeSign;
    property PositiveSign: Char read GetPositiveSign write SetPositiveSign;
  end;

function IntToStrZeroPad(Value, Count: Integer): AnsiString;

//--------------------------------------------------------------------------------------------------
// Loading of modules (DLLs)
//--------------------------------------------------------------------------------------------------

type
{$IFDEF MSWINDOWS}
  TModuleHandle = HINST;
{$ENDIF MSWINDOWS}
{$IFDEF LINUX}
  TModuleHandle = THandle;
{$ENDIF LINUX}

const
  INVALID_MODULEHANDLE_VALUE = TModuleHandle(0);

function LoadModule(var Module: TModuleHandle; FileName: string): Boolean;
function LoadModuleEx(var Module: TModuleHandle; FileName: string; Flags: Cardinal): Boolean;
procedure UnloadModule(var Module: TModuleHandle);
function GetModuleSymbol(Module: TModuleHandle; SymbolName: string): Pointer;
function GetModuleSymbolEx(Module: TModuleHandle; SymbolName: string; var Accu: Boolean): Pointer;
function ReadModuleData(Module: TModuleHandle; SymbolName: string; var Buffer; Size: Cardinal): Boolean;
function WriteModuleData(Module: TModuleHandle; SymbolName: string; var Buffer; Size: Cardinal): Boolean;

//--------------------------------------------------------------------------------------------------
// Conversion Utilities
//--------------------------------------------------------------------------------------------------

type
  EJclConversionError = class (EJclError);

function StrToBoolean(const S: string): Boolean;
function IntToBool(I: Integer): Boolean;
function BoolToInt(B: Boolean): Integer;

//--------------------------------------------------------------------------------------------------
// RTL package information
//--------------------------------------------------------------------------------------------------

{$IFNDEF FPC}
function SystemTObjectInstance: LongWord;
function IsCompiledWithPackages: Boolean;
{$ENDIF}

implementation

uses
  {$IFDEF LINUX}
  Types, Libc,
  {$ENDIF LINUX}
  SysUtils,
  JclResources, JclStrings, JclMath;

//==================================================================================================
// Pointer manipulation
//==================================================================================================

procedure GetAndFillMem(var P: Pointer; const Size: Integer; const Value: Byte);
begin
  GetMem(P, Size);
  FillChar(P^, Size, Value);
end;

//--------------------------------------------------------------------------------------------------

procedure FreeMemAndNil(var P: Pointer);
var
  Q: Pointer;
begin
  Q := P;
  P := nil;
  FreeMem(Q);
end;

//--------------------------------------------------------------------------------------------------

function PCharOrNil(const S: AnsiString): PAnsiChar;
begin
  if Length(S) = 0 then
    Result := nil
  else
    Result := PAnsiChar(S);
end;

//--------------------------------------------------------------------------------------------------

{$IFDEF SUPPORTS_WIDESTRING}

function PWideCharOrNil(const W: WideString): PWideChar;
begin
  if Length(W) = 0 then
    Result := nil
  else
    Result := PWideChar(W);
end;

{$ENDIF SUPPORTS_WIDESTRING}

//--------------------------------------------------------------------------------------------------

type
  PUsed = ^TUsed;
  TUsed = record
    SizeFlags: Integer;
  end;

const
  cThisUsedFlag = 2;
  cPrevFreeFlag = 1;
  cFillerFlag   = Integer($80000000);
  cFlags        = cThisUsedFlag or cPrevFreeFlag or cFillerFlag;

function SizeOfMem(const APointer: Pointer): Integer;
var
  U: PUsed;
begin
  if IsMemoryManagerSet then
    Result:= -1
  else
  begin
    Result := 0;
    if APointer <> nil then
    begin
      U := APointer;
      U := PUsed(PChar(U) - SizeOf(TUsed));
      if (U.SizeFlags and cThisUsedFlag) <> 0 then
        Result := (U.SizeFlags) and (not cFlags - SizeOf(TUsed));
    end;
  end;
end;

//--------------------------------------------------------------------------------------------------

function WriteProtectedMemory(BaseAddress, Buffer: Pointer;
  Size: Cardinal; out WrittenBytes: Cardinal): Boolean;
{$IFDEF MSWINDOWS}
begin
  Result := WriteProcessMemory(GetCurrentProcess, BaseAddress, Buffer, Size, WrittenBytes);
end;
{$ENDIF MSWINDOWS}
{$IFDEF LINUX}
// TODOc Author: Andreas Hausladen
{ TODO : Works so far, but causes app to hang on termination }
var
  AlignedAddress: Cardinal;
  PageSize, ProtectSize: Cardinal;
begin
  Result := False;
  WrittenBytes := 0;

  PageSize := Cardinal(getpagesize);
  AlignedAddress := Cardinal(BaseAddress) and not (PageSize - 1); // start memory page
  // get the number of needed memory pages
  ProtectSize := PageSize;
  while Cardinal(BaseAddress) + Size > AlignedAddress + ProtectSize do
    Inc(ProtectSize, PageSize);

  if mprotect(Pointer(AlignedAddress), ProtectSize,
       PROT_READ or PROT_WRITE or PROT_EXEC) = 0 then // obtain write access
  begin
    try
      Move(Buffer^, BaseAddress^, Size); // replace code
      Result := True;
      WrittenBytes := Size;
    finally
      // Is there any function that returns the current page protection?
//    mprotect(p, ProtectSize, PROT_READ or PROT_EXEC); // lock memory page
    end;
  end;
end;

//--------------------------------------------------------------------------------------------------

procedure FlushInstructionCache;
// TODOc Author: Andreas Hausladen
// Cannot find any glibc function that can flush the instruction code
{ TODO : Needs commenting }
asm
        JMP     @@Exit
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
@@Exit:
end;

{$ENDIF LINUX}

//==================================================================================================
// Guards
//==================================================================================================

type
  TSafeGuard = class (TInterfacedObject, ISafeGuard)
  private
    FItem: Pointer;
  public
    constructor Create(Mem: Pointer);
    destructor Destroy; override;
    function ReleaseItem: Pointer;
    function GetItem: Pointer;
    procedure FreeItem; virtual;
  end;

  TObjSafeGuard = class (TSafeGuard, ISafeGuard)
  public
    constructor Create(Obj: TObject);
    procedure FreeItem; override;
  end;

  TMultiSafeGuard = class (TInterfacedObject, IMultiSafeGuard)
  private
    FItems: TList;
  public
    constructor Create;
    destructor Destroy; override;
    function AddItem(Mem: Pointer): Pointer;
    procedure FreeItem(Index: Integer); virtual;
    function GetCount: Integer;
    function GetItem(Index: Integer): Pointer;
    function ReleaseItem(Index: Integer): Pointer;
  end;

  TObjMultiSafeGuard = class (TMultiSafeGuard, IMultiSafeGuard)
  public
    procedure FreeItem(Index: Integer); override;
  end;

//--------------------------------------------------------------------------------------------------
// TSafeGuard
//--------------------------------------------------------------------------------------------------

constructor TSafeGuard.Create(Mem: Pointer);
begin
  FItem := Mem;
end;

//--------------------------------------------------------------------------------------------------

destructor TSafeGuard.Destroy;
begin
  FreeItem;
  inherited Destroy;
end;

//--------------------------------------------------------------------------------------------------

function TSafeGuard.ReleaseItem: Pointer;
begin
  Result := FItem;
  FItem := nil;
end;

//--------------------------------------------------------------------------------------------------

function TSafeGuard.GetItem: Pointer;
begin
  Result := FItem;
end;

//--------------------------------------------------------------------------------------------------

procedure TSafeGuard.FreeItem;
begin
  if FItem <> nil then
    FreeMem(FItem);
  FItem := nil;
end;

//--------------------------------------------------------------------------------------------------
// TObjSafeGuard
//--------------------------------------------------------------------------------------------------

constructor TObjSafeGuard.Create(Obj: TObject);
begin
  inherited Create(Obj);
end;

//--------------------------------------------------------------------------------------------------

procedure TObjSafeGuard.FreeItem;
begin
  if FItem <> nil then
  begin
    TObject(FItem).Free;
    FItem := nil;
  end;
end;

//--------------------------------------------------------------------------------------------------
// TMultiSafeGuard
//--------------------------------------------------------------------------------------------------

function TMultiSafeGuard.AddItem(Mem: Pointer): Pointer;
begin
  Result := Mem;
  FItems.Add(Mem);
end;

//--------------------------------------------------------------------------------------------------

constructor TMultiSafeGuard.Create;
begin
  inherited Create;
  FItems := TList.Create;
end;

//--------------------------------------------------------------------------------------------------

destructor TMultiSafeGuard.Destroy;
var
  I: Integer;
begin
  for I := FItems.Count - 1 downto 0 do FreeItem(I);
  FItems.Free;
  inherited Destroy;
end;

//--------------------------------------------------------------------------------------------------

procedure TMultiSafeGuard.FreeItem(Index: Integer);
begin
  FreeMem(FItems[Index]);
  FItems.Delete(Index);
end;

//--------------------------------------------------------------------------------------------------

function TMultiSafeGuard.GetCount: Integer;
begin
  Result := FItems.Count;
end;

//--------------------------------------------------------------------------------------------------

function TMultiSafeGuard.GetItem(Index: Integer): Pointer;
begin
  Result := FItems[Index];
end;

//--------------------------------------------------------------------------------------------------

function TMultiSafeGuard.ReleaseItem(Index: Integer): Pointer;
begin
  Result := FItems[Index];
  FItems.Delete(Index);
end;

//--------------------------------------------------------------------------------------------------

function Guard(Mem: Pointer; var SafeGuard: IMultiSafeGuard): Pointer; overload;
begin
  if SafeGuard = nil then
    SafeGuard := TMultiSafeGuard.Create;
  Result := SafeGuard.AddItem(Mem);
end;

//--------------------------------------------------------------------------------------------------
// TObjMultiSafeGuard
//--------------------------------------------------------------------------------------------------

procedure TObjMultiSafeGuard.FreeItem(Index: Integer);
begin
  TObject(FItems[Index]).Free;
  FItems.Delete(Index);
end;

//--------------------------------------------------------------------------------------------------

function Guard(Obj: TObject; var SafeGuard: IMultiSafeGuard): TObject; overload;
begin
  if SafeGuard = nil then
    SafeGuard := TObjMultiSafeGuard.Create;
  Result := SafeGuard.AddItem(Obj);
end;

//--------------------------------------------------------------------------------------------------

function Guard(Mem: Pointer; out SafeGuard: ISafeGuard): Pointer; overload;
begin
  Result := Mem;
  SafeGuard := TSafeGuard.Create(Mem);
end;

//--------------------------------------------------------------------------------------------------

function Guard(Obj: TObject; out SafeGuard: ISafeGuard): TObject; overload;
begin
  Result := Obj;
  SafeGuard := TObjSafeGuard.Create(Obj);
end;

//--------------------------------------------------------------------------------------------------

function GuardGetMem(Size: Cardinal; out SafeGuard: ISafeGuard): Pointer;
begin
  GetMem(Result, Size);
  Guard(Result, SafeGuard);
end;

//--------------------------------------------------------------------------------------------------

function GuardAllocMem(Size: Cardinal; out SafeGuard: ISafeGuard): Pointer;
begin
  Result := AllocMem(Size);
  Guard(Result, SafeGuard);
end;

//==================================================================================================
// Binary search
//==================================================================================================

function SearchSortedList(List: TList; SortFunc: TListSortCompare; Item: Pointer; Nearest: Boolean): Integer;
var
  L, H, I, C: Integer;
  B: Boolean;
begin
  Result := -1;
  if List <> nil then
  begin
    L := 0;
    H := List.Count - 1;
    B := False;
    while L <= H do
    begin
      I := (L + H) shr 1;
      C := SortFunc(List.List^[I], Item);
      if C < 0 then
        L := I + 1
      else
      begin
        H := I - 1;
        if C = 0 then
        begin
          B := True;
          L := I;
        end;
      end;
    end;
    if B then
      Result := L
    else
    if Nearest and (H >= 0) then
      Result := H;
  end;
end;

//--------------------------------------------------------------------------------------------------

function SearchSortedUntyped(Param: Pointer; ItemCount: Integer; SearchFunc: TUntypedSearchCompare;
  const Value; Nearest: Boolean): Integer;
var
  L, H, I, C: Integer;
  B: Boolean;
begin
  Result := -1;
  if ItemCount > 0 then
  begin
    L := 0;
    H := ItemCount - 1;
    B := False;
    while L <= H do
    begin
      I := (L + H) shr 1;
      C := SearchFunc(Param, I, Value);
      if C < 0 then
        L := I + 1
      else
      begin
        H := I - 1;
        if C = 0 then
        begin
          B := True;
          L := I;
        end;
      end;
    end;
    if B then
      Result := L
    else
    if Nearest and (H >= 0) then
      Result := H;
  end;
end;

//==================================================================================================
// Dynamic array sort and search routines
//==================================================================================================

procedure SortDynArray(const ArrayPtr: Pointer; ElementSize: Cardinal; SortFunc: TDynArraySortCompare);
var
  TempBuf: TDynByteArray;

  function ArrayItemPointer(Item: Integer): Pointer;
  begin
    Result := Pointer(Cardinal(ArrayPtr) + (Cardinal(Item) * ElementSize));
  end;

  procedure QuickSort(L, R: Integer);
  var
    I, J, T: Integer;
    P, IPtr, JPtr: Pointer;
  begin
    repeat
      I := L;
      J := R;
      P := ArrayItemPointer((L + R) shr 1);
      repeat
        while SortFunc(ArrayItemPointer(I), P) < 0 do
          Inc(I);
        while SortFunc(ArrayItemPointer(J), P) > 0 do
          Dec(J);
        if I <= J then
        begin
          IPtr := ArrayItemPointer(I);
          JPtr := ArrayItemPointer(J);
          case ElementSize of
            SizeOf(Byte):
              begin
                T := PByte(IPtr)^;
                PByte(IPtr)^ := PByte(JPtr)^;
                PByte(JPtr)^ := T;
              end;
            SizeOf(Word):
              begin
                T := PWord(IPtr)^;
                PWord(IPtr)^ := PWord(JPtr)^;
                PWord(JPtr)^ := T;
              end;
            SizeOf(Integer):
              begin
                T := PInteger(IPtr)^;
                PInteger(IPtr)^ := PInteger(JPtr)^;
                PInteger(JPtr)^ := T;
              end;
          else
            Move(IPtr^, TempBuf[0], ElementSize);
            Move(JPtr^, IPtr^, ElementSize);
            Move(TempBuf[0], JPtr^, ElementSize);
          end;
          if P = IPtr then
            P := JPtr
          else
          if P = JPtr then
            P := IPtr;
          Inc(I);
          Dec(J);
        end;
      until I > J;
      if L < J then
        QuickSort(L, J);
      L := I;
    until I >= R;
  end;

begin
  if ArrayPtr <> nil then
  begin
    SetLength(TempBuf, ElementSize);
    QuickSort(0, PInteger(Cardinal(ArrayPtr) - 4)^ - 1);
  end;
end;

//--------------------------------------------------------------------------------------------------

function SearchDynArray(const ArrayPtr: Pointer; ElementSize: Cardinal; SortFunc: TDynArraySortCompare;
  ValuePtr: Pointer; Nearest: Boolean): Integer;
var
  L, H, I, C: Integer;
  B: Boolean;
begin
  Result := -1;
  if ArrayPtr <> nil then
  begin
    L := 0;
    H := PInteger(Cardinal(ArrayPtr) - 4)^ - 1;
    B := False;
    while L <= H do
    begin
      I := (L + H) shr 1;
      C := SortFunc(Pointer(Cardinal(ArrayPtr) + (Cardinal(I) * ElementSize)), ValuePtr);
      if C < 0 then
        L := I + 1
      else
      begin
        H := I - 1;
        if C = 0 then
        begin
          B := True;
          L := I;
        end;
      end;
    end;
    if B then
      Result := L
    else
    if Nearest and (H >= 0) then
      Result := H;
  end;
end;

//--------------------------------------------------------------------------------------------------

{ Various compare functions for basic types }

function DynArrayCompareByte(Item1, Item2: Pointer): Integer;
begin
  Result := PByte(Item1)^ - PByte(Item2)^;
end;

//--------------------------------------------------------------------------------------------------

function DynArrayCompareShortInt(Item1, Item2: Pointer): Integer;
begin
  Result := PShortInt(Item1)^ - PShortInt(Item2)^;
end;

//--------------------------------------------------------------------------------------------------

function DynArrayCompareWord(Item1, Item2: Pointer): Integer;
begin
  Result := PWord(Item1)^ - PWord(Item2)^;
end;

//--------------------------------------------------------------------------------------------------

function DynArrayCompareSmallInt(Item1, Item2: Pointer): Integer;
begin
  Result := PSmallInt(Item1)^ - PSmallInt(Item2)^;
end;

//--------------------------------------------------------------------------------------------------

function DynArrayCompareInteger(Item1, Item2: Pointer): Integer;
begin
  Result := PInteger(Item1)^ - PInteger(Item2)^;
end;

//--------------------------------------------------------------------------------------------------

function DynArrayCompareCardinal(Item1, Item2: Pointer): Integer;
begin
  Result := PInteger(Item1)^ - PInteger(Item2)^;
end;

//--------------------------------------------------------------------------------------------------

function DynArrayCompareInt64(Item1, Item2: Pointer): Integer;
begin
  Result := PInt64(Item1)^ - PInt64(Item2)^;
end;

//--------------------------------------------------------------------------------------------------

function DynArrayCompareSingle(Item1, Item2: Pointer): Integer;
begin
  if PSingle(Item1)^ < PSingle(Item2)^ then
    Result := -1
  else
  if PSingle(Item1)^ > PSingle(Item2)^ then
    Result := 1
  else
    Result := 0;
end;

//--------------------------------------------------------------------------------------------------

function DynArrayCompareDouble(Item1, Item2: Pointer): Integer;
begin
  if PDouble(Item1)^ < PDouble(Item2)^ then
    Result := -1
  else
  if PDouble(Item1)^ > PDouble(Item2)^ then
    Result := 1
  else
    Result := 0;
end;

//--------------------------------------------------------------------------------------------------

function DynArrayCompareExtended(Item1, Item2: Pointer): Integer;
begin
  if PExtended(Item1)^ < PExtended(Item2)^ then
    Result := -1
  else
  if PExtended(Item1)^ > PExtended(Item2)^ then
    Result := 1
  else
    Result := 0;
end;

//--------------------------------------------------------------------------------------------------

function DynArrayCompareFloat(Item1, Item2: Pointer): Integer;
begin
  if PFloat(Item1)^ < PFloat(Item2)^ then
    Result := -1
  else
  if PFloat(Item1)^ > PFloat(Item2)^ then
    Result := 1
  else
    Result := 0;
end;

//--------------------------------------------------------------------------------------------------

function DynArrayCompareAnsiString(Item1, Item2: Pointer): Integer;
begin
  Result := AnsiCompareStr(PAnsiString(Item1)^, PAnsiString(Item2)^);
end;

//--------------------------------------------------------------------------------------------------

function DynArrayCompareAnsiText(Item1, Item2: Pointer): Integer;
begin
  Result := AnsiCompareText(PAnsiString(Item1)^, PAnsiString(Item2)^);
end;

//--------------------------------------------------------------------------------------------------

function DynArrayCompareString(Item1, Item2: Pointer): Integer;
begin
  Result := CompareStr(PAnsiString(Item1)^, PAnsiString(Item2)^);
end;

//--------------------------------------------------------------------------------------------------

function DynArrayCompareText(Item1, Item2: Pointer): Integer;
begin
  Result := CompareText(PAnsiString(Item1)^, PAnsiString(Item2)^);
end;

//==================================================================================================
// Object lists
//==================================================================================================

procedure ClearObjectList(List: TList);
var
  I: Integer;
begin
  if List <> nil then
  begin
    for I := 0 to List.Count - 1 do
    begin
      if List[I] <> nil then
      begin
        if TObject(List[I]) is TList then
        begin
          // recursively delete TList sublists
          ClearObjectList(TList(List[I]));
        end;
        TObject(List[I]).Free;
        List[I] := nil;
      end;
    end;
    List.Clear;
  end;
end;

//--------------------------------------------------------------------------------------------------

procedure FreeObjectList(var List: TList);
begin
  if List <> nil then
  begin
    ClearObjectList(List);
    FreeAndNil(List);
  end;
end;

//==================================================================================================
// TJclReferenceMemoryStream
//==================================================================================================

constructor TJclReferenceMemoryStream.Create(const Ptr: Pointer; Size: Longint);
begin
  {$IFDEF MSWINDOWS}
  Assert(not IsBadReadPtr(Ptr, Size));
  {$ENDIF MSWINDOWS}
  inherited Create;
  SetPointer(Ptr, Size);
end;

//--------------------------------------------------------------------------------------------------

function TJclReferenceMemoryStream.Write(const Buffer; Count: Longint): Longint;
begin
  raise EJclError.CreateResRec(@RsCannotWriteRefStream);
end;

//==================================================================================================
// replacement for the C distfix operator ? :
//==================================================================================================

function Iff(const Condition: Boolean; const TruePart, FalsePart: string): string;
begin
  if Condition then
    Result := TruePart
  else
    Result := FalsePart;
end;

//--------------------------------------------------------------------------------------------------

function Iff(const Condition: Boolean; const TruePart, FalsePart: Char): Char;
begin
  if Condition then
    Result := TruePart
  else
    Result := FalsePart;
end;

//--------------------------------------------------------------------------------------------------

function Iff(const Condition: Boolean; const TruePart, FalsePart: Byte): Byte;
begin
  if Condition then
    Result := TruePart
  else
    Result := FalsePart;
end;

//--------------------------------------------------------------------------------------------------

function Iff(const Condition: Boolean; const TruePart, FalsePart: Integer): Integer;
begin
  if Condition then
    Result := TruePart
  else
    Result := FalsePart;
end;

//--------------------------------------------------------------------------------------------------

function Iff(const Condition: Boolean; const TruePart, FalsePart: Cardinal): Cardinal;
begin
  if Condition then
    Result := TruePart
  else
    Result := FalsePart;
end;

//--------------------------------------------------------------------------------------------------

function Iff(const Condition: Boolean; const TruePart, FalsePart: Float): Float;
begin
  if Condition then
    Result := TruePart
  else
    Result := FalsePart;
end;

//--------------------------------------------------------------------------------------------------

function Iff(const Condition: Boolean; const TruePart, FalsePart: Boolean): Boolean;
begin
  if Condition then
    Result := TruePart
  else
    Result := FalsePart;
end;

//--------------------------------------------------------------------------------------------------

function Iff(const Condition: Boolean; const TruePart, FalsePart: Pointer): Pointer;
begin
  if Condition then
    Result := TruePart
  else
    Result := FalsePart;
end;

//--------------------------------------------------------------------------------------------------

function Iff(const Condition: Boolean; const TruePart, FalsePart: Int64): Int64;
begin
  if Condition then
    Result := TruePart
  else
    Result := FalsePart;
end;

//--------------------------------------------------------------------------------------------------

{$IFDEF COMPILER6_UP}
function Iff(const Condition: Boolean; const TruePart, FalsePart: Variant): Variant;
begin
  if Condition then
    Result := TruePart
  else
    Result := FalsePart;
end;
{$ENDIF COMPILER6_UP}

//==================================================================================================
// Classes information and manipulation
//==================================================================================================

//==================================================================================================
// Virtual Methods
//==================================================================================================

// Helper method

procedure SetVMTPointer(AClass: TClass; Offset: Integer; Value: Pointer);
var
  WrittenBytes: DWORD;
  PatchAddress: PPointer;
begin
  PatchAddress := Pointer(Integer(AClass) + Offset);
  //! StH: WriteProcessMemory IMO is not exactly the politically correct approach;
  // better VirtualProtect, direct patch, VirtualProtect
  if not WriteProtectedMemory(PatchAddress, @Value, SizeOf(Value), WrittenBytes) then
    raise EJclVMTError.CreateResRecFmt(@RsVMTMemoryWriteError, [SysErrorMessage(GetLastError)]);

  if WrittenBytes <> SizeOf(Pointer) then
    raise EJclVMTError.CreateResRecFmt(@RsVMTMemoryWriteError, [IntToStr(WrittenBytes)]);

  // make sure that everything keeps working in a dual processor setting
  FlushInstructionCache{$IFDEF MSWINDOWS}(GetCurrentProcess, PatchAddress, SizeOf(Pointer)){$ENDIF};
end;

//--------------------------------------------------------------------------------------------------

{$IFNDEF FPC}
function GetVirtualMethodCount(AClass: TClass): Integer;
var
  BeginVMT: Longint;
  EndVMT: Longint;
  TablePointer: Longint;
  I: Integer;
begin
  BeginVMT := Longint(AClass);

  // Scan the offset entries in the class table for the various fields,
  // namely vmtIntfTable, vmtAutoTable, ..., vmtDynamicTable
  // The last entry is always the vmtClassName, so stop once we got there
  // After the last virtual method there is one of these entries.

  EndVMT := PLongint(Longint(AClass) + vmtClassName)^;
  // Set iterator to first item behind VMT table pointer
  I := vmtSelfPtr + SizeOf(Pointer);
  repeat
    TablePointer := PLongint(Longint(AClass) + I)^;
    if (TablePointer <> 0) and (TablePointer >= BeginVMT) and
       (TablePointer < EndVMT) then
      EndVMT := Longint(TablePointer);
    Inc(I, SizeOf(Pointer));
  until I >= vmtClassName;

  Result := (EndVMT - BeginVMT) div SizeOf(Pointer);
end;
{$ENDIF}

//--------------------------------------------------------------------------------------------------

function GetVirtualMethod(AClass: TClass; const Index: Integer): Pointer;
begin
  Result := PPointer(Integer(AClass) + Index * SizeOf(Pointer))^;
end;

//--------------------------------------------------------------------------------------------------

procedure SetVirtualMethod(AClass: TClass; const Index: Integer; const Method: Pointer);
begin
  SetVMTPointer(AClass, Index * SizeOf(Pointer), Method);
end;

//==================================================================================================
// Dynamic Methods
//==================================================================================================

type
  TvmtDynamicTable = packed record
    Count: Word;
   {IndexList: array [1..Count] of Word;
    AddressList: array [1..Count] of Pointer;}
  end;

//--------------------------------------------------------------------------------------------------

function GetDynamicMethodCount(AClass: TClass): Integer; assembler;
asm
        MOV     EAX, [EAX].vmtDynamicTable
        TEST    EAX, EAX
        JE      @@Exit
        MOVZX   EAX, WORD PTR [EAX]
@@Exit:
end;

//--------------------------------------------------------------------------------------------------

function GetDynamicIndexList(AClass: TClass): PDynamicIndexList; assembler;
asm
        MOV     EAX, [EAX].vmtDynamicTable
        ADD     EAX, 2
end;

//--------------------------------------------------------------------------------------------------

function GetDynamicAddressList(AClass: TClass): PDynamicAddressList; assembler;
asm
        MOV     EAX, [EAX].vmtDynamicTable
        MOVZX   EDX, Word ptr [EAX]
        ADD     EAX, EDX
        ADD     EAX, EDX
        ADD     EAX, 2
end;

//--------------------------------------------------------------------------------------------------

function HasDynamicMethod(AClass: TClass; Index: Integer): Boolean; assembler;
// Mainly copied from System.GetDynaMethod
asm
        { ->    EAX     vmt of class            }
        {       DX      dynamic method index    }

        PUSH    EDI
        XCHG    EAX, EDX
        JMP     @@HaveVMT
@@OuterLoop:
        MOV     EDX, [EDX]
@@HaveVMT:
        MOV     EDI, [EDX].vmtDynamicTable
        TEST    EDI, EDI
        JE      @@Parent
        MOVZX   ECX, WORD PTR [EDI]
        PUSH    ECX
        ADD     EDI,2
        REPNE   SCASW
        JE      @@Found
        POP     ECX
@@Parent:
        MOV     EDX,[EDX].vmtParent
        TEST    EDX,EDX
        JNE     @@OuterLoop
        MOV     EAX, 0
        JMP     @@Exit
@@Found:
        POP     EAX
        MOV     EAX, 1
@@Exit:
        POP     EDI
end;

//--------------------------------------------------------------------------------------------------

{$IFNDEF FPC}
function GetDynamicMethod(AClass: TClass; Index: Integer): Pointer; assembler;
asm
        CALL    System.@FindDynaClass
end;
{$ENDIF}

//==================================================================================================
// Interface Table
//==================================================================================================

function GetInitTable(AClass: TClass): PTypeInfo; assembler;
asm
        MOV     EAX, [EAX].vmtInitTable
end;

//--------------------------------------------------------------------------------------------------

function GetFieldTable(AClass: TClass): PFieldTable; assembler;
asm
        MOV     EAX, [EAX].vmtFieldTable
end;

//--------------------------------------------------------------------------------------------------

function GetMethodTable(AClass: TClass): PMethodTable; assembler;
asm
        MOV     EAX, [EAX].vmtMethodTable
end;

//--------------------------------------------------------------------------------------------------

function GetMethodEntry(MethodTable: PMethodTable; Index: Integer): PMethodEntry;
begin
  Result := Pointer(Cardinal(MethodTable) + 2);
  for Index := Index downto 1 do
    Inc(Cardinal(Result), Result^.EntrySize);
end;

//==================================================================================================
// Class Parent methods
//==================================================================================================

procedure SetClassParent(AClass: TClass; NewClassParent: TClass);
var
  WrittenBytes: DWORD;
  PatchAddress: Pointer;
begin
  PatchAddress := PPointer(Integer(AClass) + vmtParent)^;
  //! StH: WriteProcessMemory IMO is not exactly the politically correct approach;
  // better VirtualProtect, direct patch, VirtualProtect
  if not WriteProtectedMemory(PatchAddress, @NewClassParent, SizeOf(Pointer), WrittenBytes) then
    raise EJclVMTError.CreateResRecFmt(@RsVMTMemoryWriteError, [SysErrorMessage(GetLastError)]);
  if WrittenBytes <> SizeOf(Pointer) then
    raise EJclVMTError.CreateResRecFmt(@RsVMTMemoryWriteError, [IntToStr(WrittenBytes)]);
  // make sure that everything keeps working in a dual processor setting
  FlushInstructionCache{$IFDEF MSWINDOWS}(GetCurrentProcess, PatchAddress, SizeOf(Pointer)){$ENDIF};
end;

//--------------------------------------------------------------------------------------------------

function GetClassParent(AClass: TClass): TClass; assembler;
asm
        MOV     EAX, [EAX].vmtParent
        TEST    EAX, EAX
        JE      @@Exit
        MOV     EAX, [EAX]
@@Exit:
end;

//--------------------------------------------------------------------------------------------------

{$IFNDEF FPC}
function IsClass(Address: Pointer): Boolean; assembler;
asm
        CMP     Address, Address.vmtSelfPtr
        JNZ     @False
        MOV     Result, True
        JMP     @Exit
@False:
        MOV     Result, False
@Exit:
end;
{$ENDIF}

//--------------------------------------------------------------------------------------------------

{$IFNDEF FPC}
function IsObject(Address: Pointer): Boolean; assembler;
asm
// or IsClass(Pointer(Address^));
        MOV     EAX, [Address]
        CMP     EAX, EAX.vmtSelfPtr
        JNZ     @False
        MOV     Result, True
        JMP     @Exit
@False:
        MOV     Result, False
@Exit:
end;
{$ENDIF}

//==================================================================================================
// Interface information
//==================================================================================================

function GetImplementorOfInterface(const I: IInterface): TObject;
{ TODO -cDOC : Original code by Hallvard Vassbotn }
{ TODO -cTesting : Check the implemetation for any further version of compiler }
const
  AddByte = $04244483; // opcode for ADD DWORD PTR [ESP+4], Shortint
  AddLong = $04244481; // opcode for ADD DWORD PTR [ESP+4], Longint
type
  PAdjustSelfThunk = ^TAdjustSelfThunk;
  TAdjustSelfThunk = packed record
    case AddInstruction: LongInt of
      AddByte: (AdjustmentByte: ShortInt);
      AddLong: (AdjustmentLong: LongInt);
  end;
  PInterfaceMT = ^TInterfaceMT;
  TInterfaceMT = packed record
    QueryInterfaceThunk: PAdjustSelfThunk;
  end;
  TInterfaceRef = ^PInterfaceMT;
var
  QueryInterfaceThunk: PAdjustSelfThunk;
begin
  try
    Result := Pointer(I);
    if Assigned(Result) then
    begin
      QueryInterfaceThunk := TInterfaceRef(I)^.QueryInterfaceThunk;
      case QueryInterfaceThunk.AddInstruction of
        AddByte:
          Inc(PChar(Result), QueryInterfaceThunk.AdjustmentByte);
        AddLong:
          Inc(PChar(Result), QueryInterfaceThunk.AdjustmentLong);
      else
        Result := nil;
      end;
    end;
  except
    Result := nil;
  end;
end;

//==================================================================================================
// Numeric formatting routines
//==================================================================================================

function IntToStrZeroPad(Value, Count: Integer): AnsiString;
begin
  Result := IntToStr(Value);
  if Length(Result) < Count then
    Result := StrFillChar('0', Count - Length(Result)) + Result;
end;

//--------------------------------------------------------------------------------------------------

{ TJclNumericFormat }

{ TODOc Author: Robert Rossmair

  Digit:         converts a digit value (number) to a digit (char)
  DigitValue:    converts a digit (char) into a number (digit value)
  IntToStr,
  FloatToStr,
  FloatToHTML:   converts a numeric value to a base <Base> numeric representation with formating options
  StrToIn:       converts a base <Base> numeric representation into an integer, if possible
  GetMantisseExponent: similar to AsString, but returns the Exponent separately as an integer
}
const
{$IFDEF MATH_EXTENDED_PRECISION}
  BinaryPrecision = 64;
{$ENDIF MATH_EXTENDED_PRECISION}
{$IFDEF MATH_DOUBLE_PRECISION}
  BinaryPrecision = 53;
{$ENDIF MATH_DOUBLE_PRECISION}
{$IFDEF MATH_SINGLE_PRECISION}
  BinaryPrecision = 24;
{$ENDIF MATH_SINGLE_PRECISION}

//--------------------------------------------------------------------------------------------------

constructor TJclNumericFormat.Create;
begin
  inherited Create;
  { TODO:  Initialize, when possible, from locale info }
  FBase := 10;
  FExpDivision := 1;
  SetPrecision(6);
  FNumberOfFractionalDigits := BinaryPrecision;
  FSignChars[False] := '-';
  FSignChars[True] := '+';
  FPaddingChar := ' ';
  FMultiplier := '�';
  FFractionalPartSeparator := DecimalSeparator;
  FDigitBlockSeparator := ThousandSeparator;
end;

//--------------------------------------------------------------------------------------------------

procedure TJclNumericFormat.InvalidDigit(Digit: Char);
begin
  raise EConvertError.CreateResFmt(@RsInvalidDigit, [Base, Digit]);
end;

//--------------------------------------------------------------------------------------------------

function TJclNumericFormat.Digit(DigitValue: TDigitValue): Char;
begin
  Assert(DigitValue < Base, Format(RsInvalidDigitValue, [Base, DigitValue]));
  if DigitValue > 9 then
    Result := Chr(Ord('A') + DigitValue - 10)
  else
    Result := Chr(Ord('0') + DigitValue);
end;

//--------------------------------------------------------------------------------------------------

function TJclNumericFormat.GetDigitValue(Digit: Char): Integer;
begin
  Result := -1;
  if Digit in ['0'..'9'] then
    Result := Ord(Digit) - Ord('0')
  else
  begin
    Digit := UpCase(Digit);
    if Digit in ['A'..'Z'] then
      Result := Ord(Digit) - Ord('A') + 10;
  end;
  if Result >= Base then
    Result := -1;
end;

//--------------------------------------------------------------------------------------------------

function TJclNumericFormat.DigitValue(Digit: Char): TDigitValue;
begin
  Result := GetDigitValue(Digit);
  if Result = -1 then
    InvalidDigit(Digit);
end;

//--------------------------------------------------------------------------------------------------

function TJclNumericFormat.IsDigit(Value: Char): Boolean;
begin
  Result := GetDigitValue(Value) <> -1;
end;

//--------------------------------------------------------------------------------------------------

function TJclNumericFormat.FloatToHTML(const Value: Float): string;
var
  Mantissa: string;
  Exponent: Integer;
begin
  GetMantissaExp(Value, Mantissa, Exponent);
  Result := Format('%s %s %d<sup>%d</sup>', [Mantissa, Multiplier, Base, Exponent]);
end;

//--------------------------------------------------------------------------------------------------

procedure TJclNumericFormat.GetMantissaExp(const Value: Float;
  out Mantissa: string; out Exponent: Integer);
const
  {$IFDEF FPC}
  InfMantissa: array[Boolean] of string[4] = ('inf', '-inf');
  {$ElSE}
  InfMantissa: array[Boolean] of string = ('inf', '-inf');
  {$ENDIF}
var
  BlockDigits: TDigitCount;
  IntDigits, FracDigits: Integer;
  FirstDigitPos, Prec: Integer;
  I, J, N: Integer;
  K: Int64;
  X: Extended;
  HighDigit: Char;

  function GetDigit(X: Extended): Char;
  var
    N: Integer;
  begin
    N := Trunc(X);
    if N > 9 then
      Result := Chr(Ord('A') + N - 10)
    else
      Result := Chr(Ord('0') + N);
  end;

begin
  X := Abs(Value);

  if X > MaxFloatingPoint then
  begin
    Mantissa := InfMantissa[Value < 0];
    Exponent := 1;
    Exit;
  end
  else
  if X < MinFloatingPoint then
  begin
    Mantissa := Format('%.*f', [Precision, 0.0]);
    Exponent := 1;
    Exit;
  end;

  IntDigits := 1;
  Prec := Precision;

  Exponent := Trunc(LogBaseN(Base, X));
  if FExpDivision > 1 then
  begin
    N := Exponent mod FExpDivision;
    Dec(Exponent, N);
    Inc(IntDigits, N);
  end;
  X := X / Power(Base, Exponent);

  if X < 1.0 then
  begin
    Dec(Exponent, FExpDivision);
    X := X * PowerInt(Base, FExpDivision);
    Inc(IntDigits, FExpDivision - 1);
  end;

{ TODO : Here's a problem if X > High(Int64).
It *seems* to surface only if ExponentDivision > 12, but it
has not been investigated if ExponentDivision <= 12 is safe. }
  K := Trunc(X);
  if Value < 0 then
    K := -K;
  Mantissa := IntToStr(K, FirstDigitPos);

  FracDigits := Prec - IntDigits;
  if FracDigits > NumberOfFractionalDigits then
    FracDigits := NumberOfFractionalDigits;

  if FracDigits > 0 then
  begin
    J := Length(Mantissa) + 1;
    // allocate sufficient space for point + digits + digit block separators
    SetLength(Mantissa, FracDigits * 2 + J);
    Mantissa[J] := FractionalPartSeparator;
    I := J + 1;
    BlockDigits := 0;
    while FracDigits > 0 do
    begin
      if (BlockDigits > 0) and (BlockDigits = DigitBlockSize) then
      begin
        Mantissa[I] := DigitBlockSeparator;
        Inc(I);
        BlockDigits := 0;
      end;
      X := Frac(X) * Base;
      Mantissa[I] := GetDigit(X);
      Inc(I);
      Inc(BlockDigits);
      Dec(FracDigits);
    end;
    Mantissa[I] := #0;
    StrResetLength(Mantissa);
  end;

  if Frac(X) >= 0.5 then
  // round up
  begin
    HighDigit := Digit(Base - 1);
    for I := Length(Mantissa) downto 1 do
    begin
      if Mantissa[I] = HighDigit then
        if (I = FirstDigitPos) then
        begin
          Mantissa[I] := '1';
          Inc(Exponent);
          Break;
        end
        else
          Mantissa[I] := '0'
      else
      if Mantissa[I] in [DigitBlockSeparator, FractionalPartSeparator] then
        Continue
      else
      begin
        if Mantissa[I] = '9' then
          Mantissa[I] := 'A'
        else
          Mantissa[I] := Succ(Mantissa[I]);
        Break;
      end;
    end;
  end;
end;

//--------------------------------------------------------------------------------------------------

function TJclNumericFormat.FloatToStr(const Value: Float): string;
var
  Mantissa: string;
  Exponent: Integer;
begin
  GetMantissaExp(Value, Mantissa, Exponent);
  Result := Format('%s %s %d^%d', [Mantissa, Multiplier, Base, Exponent]);
end;

//--------------------------------------------------------------------------------------------------

function TJclNumericFormat.IntToStr(const Value: Int64): string;
var
  FirstDigitPos: Integer;
begin
  IntToStr(Value, FirstDigitPos);
end;

//--------------------------------------------------------------------------------------------------

function TJclNumericFormat.IntToStr(const Value: Int64; out FirstDigitPos: Integer): string;
const
  MaxResultLen = 64 + 63 + 1; // max. digits + max. group separators + sign
var
  Remainder: Int64;
  I, N: Integer;
  Chars, Digits: Cardinal;
  LoopFinished, HasSign, SpacePadding: Boolean;
begin
  SpacePadding := PaddingChar = ' ';
  HasSign := ShowSign(Value);
  Chars := MaxResultLen;
  if Width > Chars then
    Chars := Width;
  SetLength(Result, Chars);
  FillChar(Result[1], Chars, ' ');

  Remainder := Abs(Value);
  Digits := 0;

  Chars := 0;
  if HasSign then
    Chars := 1;

  I := MaxResultLen;

  while True do
  begin
    N := Remainder mod Base;
    Remainder := Remainder div Base;
    if N > 9 then
      Result[I] := Chr(Ord('A') + N - 10)
    else
      Result[I] := Chr(Ord('0') + N);
    Dec(I);
    Inc(Digits);
    Inc(Chars);
    if (Remainder = 0) and (SpacePadding or (Chars >= Width)) then Break;
    if (Digits = DigitBlockSize) then
    begin
      Inc(Chars);
      LoopFinished := (Remainder = 0) and (Chars = Width);
      if LoopFinished then
        Result[I] := ' '
      else
        Result[I] := DigitBlockSeparator;
      Dec(I);
      if LoopFinished then
        Break;
      Digits := 0;
    end;
  end;

  FirstDigitPos := I + 1;
  
  if HasSign then
    Result[I] := SignChar(Value)
  else
    Inc(I);
  N := MaxResultLen - Width + 1;
  if N < I then
    I := N;
  Result := Copy(Result, I, MaxResultLen);
  Dec(FirstDigitPos, I - 1);
end;

//--------------------------------------------------------------------------------------------------

procedure TJclNumericFormat.SetBase(const Value: TNumericSystemBase);
begin
  FBase := Value;
  SetPrecision(FWantedPrecision);
end;

//--------------------------------------------------------------------------------------------------

procedure TJclNumericFormat.SetExpDivision(const Value: Integer);
begin
  if Value <= 1 then
    FExpDivision := 1
  else
  // see TODO in GetMantissaExp
  if Value > 12 then
    FExpDivision := 12
  else
    FExpDivision := Value;
end;

//--------------------------------------------------------------------------------------------------

procedure TJclNumericFormat.SetPrecision(const Value: TDigitCount);
begin
  FWantedPrecision := Value;
  // Do not display more digits than Float precision justifies
  if Base = 2 then
    FPrecision := BinaryPrecision
  else
    FPrecision := Trunc(BinaryPrecision / LogBase2(Base));
  if Value < FPrecision then
    FPrecision := Value;
end;

//--------------------------------------------------------------------------------------------------

function TJclNumericFormat.Sign(Value: Char): Integer;
begin
  Result := 0;
  if Value = FSignChars[False] then
    Result := -1;
  if Value = FSignChars[True] then
    Result := +1;
end;

//--------------------------------------------------------------------------------------------------

function TJclNumericFormat.StrToInt(const Value: string): Int64;
var
  I, N: Integer;
  C: Char;
begin
  Result := 0;
  N := 0;
  I := 1;
  if Value[I] in ['+', '-'] then
    Inc(I);
  for I := I to Length(Value) do
  begin
    C := Value[I];
    if C in ['0'..'9'] then
      N := Ord(C) - Ord('0')
    else
    begin
      C := UpCase(C);
      if C in ['A'..'Z'] then
      begin
        N := Ord(C) - Ord('A') + 10;
        if N >= Base then
          InvalidDigit(C);
      end
      else
        if C = DigitBlockSeparator then
          Continue
      else
        InvalidDigit(C);
    end;
    Result := Result * Base + N;
  end;
  if Value[1] = '-' then
    Result := -Result;
end;

//--------------------------------------------------------------------------------------------------

function TJclNumericFormat.ShowSign(const Value: Float): Boolean;
begin
  Result := FShowPositiveSign or (Value < 0);
end;

//--------------------------------------------------------------------------------------------------

function TJclNumericFormat.ShowSign(const Value: Int64): Boolean;
begin
  Result := FShowPositiveSign or (Value < 0);
end;

//--------------------------------------------------------------------------------------------------

function TJclNumericFormat.SignChar(const Value: Float): Char;
begin
  Result := FSignChars[Value >= 0];
end;

//--------------------------------------------------------------------------------------------------

function TJclNumericFormat.SignChar(const Value: Int64): Char;
begin
  Result := FSignChars[Value >= 0];
end;

//--------------------------------------------------------------------------------------------------

function TJclNumericFormat.GetNegativeSign: Char;
begin
  Result := FSignChars[False];
end;

//--------------------------------------------------------------------------------------------------

function TJclNumericFormat.GetPositiveSign: Char;
begin
  Result := FSignChars[True];
end;

//--------------------------------------------------------------------------------------------------

procedure TJclNumericFormat.SetNegativeSign(const Value: Char);
begin
  FSignChars[False] := Value;
end;

//--------------------------------------------------------------------------------------------------

procedure TJclNumericFormat.SetPositiveSign(const Value: Char);
begin
  FSignChars[True] := Value;
end;

//==================================================================================================
// Loading of modules (DLLs)
//==================================================================================================

function LoadModule(var Module: TModuleHandle; FileName: string): Boolean;
begin
  if Module = INVALID_MODULEHANDLE_VALUE then
    Module := LoadLibrary(PChar(FileName));
  Result := Module <> INVALID_MODULEHANDLE_VALUE;
end;

//--------------------------------------------------------------------------------------------------

function LoadModuleEx(var Module: TModuleHandle; FileName: string; Flags: Cardinal): Boolean;
begin
{$IFDEF MSWINDOWS}
  if Module = INVALID_MODULEHANDLE_VALUE then
    Module := LoadLibraryEx(PChar(FileName), 0, Flags);
  Result := Module <> INVALID_MODULEHANDLE_VALUE;
{$ENDIF MSWINDOWS}
{$IFDEF LINUX}
  Result := LoadModule(Module, Filename); // ignore Flags
{$ENDIF LINUX}
end;

//--------------------------------------------------------------------------------------------------

procedure UnloadModule(var Module: TModuleHandle);
begin
  if Module <> INVALID_MODULEHANDLE_VALUE then
    FreeLibrary(Module);
  Module := INVALID_MODULEHANDLE_VALUE;
end;

//--------------------------------------------------------------------------------------------------

function GetModuleSymbol(Module: TModuleHandle; SymbolName: string): Pointer;
begin
  Result := nil;
  if Module <> INVALID_MODULEHANDLE_VALUE then
    Result := GetProcAddress(Module, PChar(SymbolName));
end;

//--------------------------------------------------------------------------------------------------

function GetModuleSymbolEx(Module: TModuleHandle; SymbolName: string; var Accu: Boolean): Pointer;
begin
  Result := nil;
  if Module <> INVALID_MODULEHANDLE_VALUE then
    Result := GetProcAddress(Module, PChar(SymbolName));
  Accu := Accu and (Result <> nil);
end;

//--------------------------------------------------------------------------------------------------

function ReadModuleData(Module: TModuleHandle; SymbolName: string; var Buffer; Size: Cardinal): Boolean;
var
  Sym: Pointer;
begin
  Result := True;
  Sym := GetModuleSymbolEx(Module, SymbolName, Result);
  if Result then
    Move(Sym^, Buffer, Size);
end;

//--------------------------------------------------------------------------------------------------

function WriteModuleData(Module: TModuleHandle; SymbolName: string; var Buffer; Size: Cardinal): Boolean;
var
  Sym: Pointer;
begin
  Result := True;
  Sym := GetModuleSymbolEx(Module, SymbolName, Result);
  if Result then
    Move(Buffer, Sym^, Size);
end;

//==================================================================================================
// Conversion Utilities
//==================================================================================================

{ TODOC
  Author: Jeff

  StrToBoolean: converts a string S to a boolean. S may be 'Yes/No', 'True/False' or '0/1' or 'T/F' or 'Y/N'.
                raises an EJclConversionError exception on failure.
  IntToBool: converts an integer to a boolean where 0 means false and anything else is tue.
  BoolToInt: converts a boolean to an integer: True=>1 and False=>0
}

const
  DefaultTrueBoolStr   = 'True';  // DO NOT LOCALIZE
  DefaultFalseBoolStr  = 'False'; // DO NOT LOCALIZE

  DefaultYesBoolStr    = 'Yes';   // DO NOT LOCALIZE
  DefaultNoBoolStr     = 'No';    // DO NOT LOCALIZE

//--------------------------------------------------------------------------------------------------

function StrToBoolean(const S: string): Boolean;
var
  LowerCasedText: string;
begin
  LowerCasedText := LowerCase(S);
  Result := ((S = '1') or
    (LowerCasedText = LowerCase(DefaultTrueBoolStr)) or (LowerCasedText = LowerCase(DefaultYesBoolStr))) or
    (LowerCasedText = LowerCase(DefaultTrueBoolStr[1])) or (LowerCasedText = LowerCase(DefaultYesBoolStr[1]));
  if not Result then
  begin
    Result := not ((S = '0') or
      (LowerCasedText = LowerCase(DefaultFalseBoolStr)) or (LowerCasedText = LowerCase(DefaultNoBoolStr)) or
      (LowerCasedText = LowerCase(DefaultFalseBoolStr[1])) or (LowerCasedText = LowerCase(DefaultNoBoolStr[1])));
    if Result then
      raise EJclConversionError.CreateResRecFmt(@RsStringToBoolean, [S]);
  end;
end;

//--------------------------------------------------------------------------------------------------

function IntToBool(I: Integer): Boolean;
begin
  Result := I <> 0;
end;

//--------------------------------------------------------------------------------------------------

function BoolToInt(B: Boolean): Integer;
begin
  Result := Ord(B);
end;

//==================================================================================================
// RTL package information
//==================================================================================================

{$IFNDEF FPC}

function SystemTObjectInstance: LongWord;
begin
  Result := FindClassHInstance(System.TObject);
end;

//--------------------------------------------------------------------------------------------------

function IsCompiledWithPackages: Boolean;
begin
  Result := SystemTObjectInstance <> HInstance;
end;

{$ENDIF}

end.
