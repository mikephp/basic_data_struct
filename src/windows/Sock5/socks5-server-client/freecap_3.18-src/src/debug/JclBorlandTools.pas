{**************************************************************************************************}
{                                                                                                  }
{ Project JEDI Code Library (JCL) extension                                                        }
{                                                                                                  }
{ The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); }
{ you may not use this file except in compliance with the License. You may obtain a copy of the    }
{ License at http://www.mozilla.org/MPL/                                                           }
{                                                                                                  }
{ Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF   }
{ ANY KIND, either express or implied. See the License for the specific language governing rights  }
{ and limitations under the License.                                                               }
{                                                                                                  }
{ The Original Code is DelphiInstall.pas.                                                          }
{                                                                                                  }
{ The Initial Developer of the Original Code is Petr Vones. Portions created by Petr Vones are     }
{ Copyright (C) of Petr Vones. All Rights Reserved.                                                }
{                                                                                                  }
{ Contributor(s): Robert Rossmair (crossplatform & BCB support)                                    }
{                                                                                                  }
{**************************************************************************************************}
{                                                                                                  }
{ Routines for getting information about installed versions of Delphi/C++Builder and performing    }
{ basic installation tasks.                                                                        }
{                                                                                                  }
{ Unit owner: Petr Vones                                                                           }
{                                                                                                  }
{**************************************************************************************************}

// $Id: JclBorlandTools.pas,v 1.1 2005/02/14 19:26:25 bert Exp $

unit JclBorlandTools;

{$I jcl.inc}

interface

uses
  {$IFDEF MSWINDOWS}
  Windows,
  {$ENDIF}
  Classes, SysUtils, IniFiles, Contnrs,
  JclBase;

//--------------------------------------------------------------------------------------------------
// Various definitions
//--------------------------------------------------------------------------------------------------

type
  TJclBorRADToolKind = (brDelphi, brCppBuilder);
  {$IFDEF KYLIX}
  TJclBorRADToolEdition = (deOPEN, dePRO, deSVR);
  {$ELSE}
  TJclBorRADToolEdition = (deSTD, dePRO, deCSS);
  {$ENDIF}
  TJclBorRADToolPath = string;

const
  // Object Repository
  BorRADToolRepositoryPagesSection    = 'Repository Pages';

  BorRADToolRepositoryDialogsPage     = 'Dialogs';
  BorRADToolRepositoryFormsPage       = 'Forms';
  BorRADToolRepositoryProjectsPage    = 'Projects';
  BorRADToolRepositoryDataModulesPage = 'Data Modules';

  BorRADToolRepositoryObjectType      = 'Type';
  BorRADToolRepositoryFormTemplate    = 'FormTemplate';
  BorRADToolRepositoryProjectTemplate = 'ProjectTemplate';
  BorRADToolRepositoryObjectName      = 'Name';
  BorRADToolRepositoryObjectPage      = 'Page';
  BorRADToolRepositoryObjectIcon      = 'Icon';
  BorRADToolRepositoryObjectDescr     = 'Description';
  BorRADToolRepositoryObjectAuthor    = 'Author';
  BorRADToolRepositoryObjectAncestor  = 'Ancestor';
  BorRADToolRepositoryObjectDesigner  = 'Designer'; // Delphi 6+ only
  BorRADToolRepositoryDesignerDfm     = 'dfm';
  BorRADToolRepositoryDesignerXfm     = 'xfm';
  BorRADToolRepositoryObjectNewForm   = 'DefaultNewForm';
  BorRADToolRepositoryObjectMainForm  = 'DefaultMainForm';

  {$IFDEF KYLIX}
  BorRADToolEditionIDs: array [TJclBorRADToolEdition] of PChar = ('OPEN', 'PRO', 'SVR');
  {$ELSE}
  BorRADToolEditionIDs: array [TJclBorRADToolEdition] of PChar = ('STD', 'PRO', 'CSS');
  {$ENDIF}

//--------------------------------------------------------------------------------------------------
// Installed versions information classes
//--------------------------------------------------------------------------------------------------

type
  TJclBorRADToolInstallation = class;

  TJclBorRADToolInstallationObject = class (TInterfacedObject)
  private
    FInstallation: TJclBorRADToolInstallation;
  protected
    constructor Create(AInstallation: TJclBorRADToolInstallation);
  public
    property Installation: TJclBorRADToolInstallation read FInstallation;
  end;

  {$IFDEF MSWINDOWS}
  TJclBorlandOpenHelp = class (TJclBorRADToolInstallationObject)
  private
    function GetContentFileName: string;
    function GetIndexFileName: string;
    function GetLinkFileName: string;
    function GetGidFileName: string;
    function GetProjectFileName: string;
    function ReadFileName(const FormatName: string): string;
  public
    function AddHelpFile(const HelpFileName, IndexName: string): Boolean;
    property ContentFileName: string read GetContentFileName;
    property GidFileName: string read GetGidFileName;
    property IndexFileName: string read GetIndexFileName;
    property LinkFileName: string read GetLinkFileName;
    property ProjectFileName: string read GetProjectFileName;
  end;
  {$ENDIF MSWINDOWS}

  TJclBorRADToolIdeTool = class (TJclBorRADToolInstallationObject)
  private
    FKey: string;
    function GetCount: Integer;
    function GetParameters(Index: Integer): string;
    function GetPath(Index: Integer): string;
    function GetTitle(Index: Integer): string;
    function GetWorkingDir(Index: Integer): string;
    procedure SetCount(const Value: Integer);
    procedure SetParameters(Index: Integer; const Value: string);
    procedure SetPath(Index: Integer; const Value: string);
    procedure SetTitle(Index: Integer; const Value: string);
    procedure SetWorkingDir(Index: Integer; const Value: string);
  protected
    constructor Create(AInstallation: TJclBorRADToolInstallation);
    procedure CheckIndex(Index: Integer);
  public
    property Count: Integer read GetCount write SetCount;
    function IndexOfPath(const Value: string): Integer;
    function IndexOfTitle(const Value: string): Integer;
    property Key: string read FKey;
    property Title[Index: Integer]: string read GetTitle write SetTitle;
    property Path[Index: Integer]: string read GetPath write SetPath;
    property Parameters[Index: Integer]: string read GetParameters write SetParameters;
    property WorkingDir[Index: Integer]: string read GetWorkingDir write SetWorkingDir;
  end;

  TJclBorRADToolIdePackages = class (TJclBorRADToolInstallationObject)
  private
    FDisabledPackages: TStringList;
    FKnownPackages: TStringList;
    function GetCount: Integer;
    function GetPackageDescriptions(Index: Integer): string;
    function GetPackageDisabled(Index: Integer): Boolean;
    function GetPackageFileNames(Index: Integer): string;
  protected
    constructor Create(AInstallation: TJclBorRADToolInstallation);
    function PackageEntryToFileName(const Entry: string): string;
    procedure ReadPackages;
    procedure RemoveDisabled(const FileName: string);
  public
    destructor Destroy; override;
    function AddPackage(const FileName, Description: string): Boolean;
    property Count: Integer read GetCount;
    property PackageDescriptions[Index: Integer]: string read GetPackageDescriptions;
    property PackageFileNames[Index: Integer]: string read GetPackageFileNames;
    property PackageDisabled[Index: Integer]: Boolean read GetPackageDisabled;
  end;

  IJclCommandLineTool = interface
    ['{A0034B09-A074-D811-847D-0030849E4592}']
    function GetExeName: string;
    function GetOptions: TStrings;
    function GetOutput: string;
    procedure AddPathOption(const Option, Path: string);
    function Execute(const CommandLine: string): Boolean;
    property ExeName: string read GetExeName;
    property Options: TStrings read GetOptions;
    property Output: string read GetOutput;
  end;

  TJclCommandLineTool = class(TInterfacedObject, IJclCommandLineTool)
  private
    FExeName: string;
    FOptions: TStrings;
    FOutput: string;
  protected
    function GetExeName: string;
    function GetOutput: string;
    function GetOptions: TStrings;
    constructor Create(const AExeName: string);
    procedure AddPathOption(const Option, Path: string);
    function Execute(const CommandLine: string): Boolean;
    property ExeName: string read GetExeName;
    property Output: string read GetOutput;
  public
    destructor Destroy; override;
  end;

  TJclBorlandCommandLineTool = class (TJclBorRADToolInstallationObject, IJclCommandLineTool)
  private
    FOptions: TStrings;
    FOutput: string;
  protected
    constructor Create(AInstallation: TJclBorRADToolInstallation); virtual;
    function GetExeName: string; virtual;
    function GetFileName: string;
    function GetOptions: TStrings;
    function GetOutput: string;
  public
    destructor Destroy; override;
    procedure AddPathOption(const Option, Path: string);
    function Execute(const CommandLine: string): Boolean; virtual;
    property FileName: string read GetFileName;
    property Output: string read FOutput;
    property Options: TStrings read FOptions;
  end;

  TJclDCC = class (TJclBorlandCommandLineTool)
  protected
    function GetExeName: string; override;
  public
    function Execute(const CommandLine: string): Boolean; override;
    function InstallPackage(const PackageName, BPLPath, DCPPath: string): Boolean;
    function SupportsLibSuffix: Boolean;
  end;

  TJclBpr2Mak = class (TJclBorlandCommandLineTool)
  protected
    function GetExeName: string; override;
  end;

  TJclBorlandMake = class (TJclBorlandCommandLineTool)
  protected
    function GetExeName: string; override;
  end;

  TJclBorRADToolPalette = class (TJclBorRADToolInstallationObject)
  private
    FKey: string;
    FTabNames: TStringList;
    function GetComponentsOnTab(Index: Integer): string;
    function GetHiddenComponentsOnTab(Index: Integer): string;
    function GetTabNameCount: Integer;
    function GetTabNames(Index: Integer): string;
    procedure ReadTabNames;
  protected
    constructor Create(AInstallation: TJclBorRADToolInstallation);
  public
    destructor Destroy; override;
    procedure ComponentsOnTabToStrings(Index: Integer; Strings: TStrings; IncludeUnitName: Boolean = False;
      IncludeHiddenComponents: Boolean = True);
    function DeleteTabName(const TabName: string): Boolean;
    function TabNameExists(const TabName: string): Boolean;
    property ComponentsOnTab[Index: Integer]: string read GetComponentsOnTab;
    property HiddenComponentsOnTab[Index: Integer]: string read GetHiddenComponentsOnTab;
    property Key: string read FKey;
    property TabNames[Index: Integer]: string read GetTabNames;
    property TabNameCount: Integer read GetTabNameCount;
  end;

  TJclBorRADToolRepository = class (TJclBorRADToolInstallationObject)
  private
    FIniFile: TIniFile;
    FFileName: string;
    FPages: TStrings;
    function GetIniFile: TIniFile;
  protected
    constructor Create(AInstallation: TJclBorRADToolInstallation);
  public
    destructor Destroy; override;
    procedure AddObject(const FileName, ObjectType, PageName, ObjectName, IconFileName, Description,
      Author, Designer: string; const Ancestor: string = '');
    procedure CloseIniFile;
    function FindPage(const Name: string; OptionalIndex: Integer): string;
    procedure RemoveObjects(const PartialPath, FileName, ObjectType: string);
    property FileName: string read FFileName;
    property IniFile: TIniFile read GetIniFile;
    property Pages: TStrings read FPages;
  end;

  TJclBorRADToolInstallation = class (TObject)
  private
    FConfigData: TCustomIniFile;
    FGlobals: TStrings;
    FRootDir: string;
    FBinFolderName: string;
    FDCC: TJclDCC;
    FMake: IJclCommandLineTool;
    FEdition: TJclBorRADToolEdition;
    FEnvironmentVariables: TStrings;
    FIdeExeFileName: string;
    FIdePackages: TJclBorRADToolIdePackages;
    FIdeTools: TJclBorRADToolIdeTool;
    FInstalledUpdatePack: Integer;
    FLatestUpdatePack: Integer;
    {$IFDEF MSWINDOWS}
    FOpenHelp: TJclBorlandOpenHelp;
    {$ENDIF}
    FPalette: TJclBorRADToolPalette;
    FRepository: TJclBorRADToolRepository;
    FVersionNumber: Integer;
    function GetBPLOutputPath: string;
    function GetDCC: TJclDCC;
    function GetDCPOutputPath: string;
    function GetDebugDCUPath: string;
    function GetDescription: string;
    function GetEditionAsText: string;
    function GetEnvironmentVariables: TStrings;
    function GetIdeExeBuildNumber: string;
    function GetIdePackages: TJclBorRADToolIdePackages;
    function GetLibrarySearchPath: TJclBorRADToolPath;
    function GetName: string;
    function GetPalette: TJclBorRADToolPalette;
    function GetRepository: TJclBorRADToolRepository;
    function GetUpdateNeeded: Boolean;
    function GetValid: Boolean;
    procedure SetLibrarySearchPath(const Value: TJclBorRADToolPath);
    function GetLibraryBrowsingPath: TJclBorRADToolPath;
    procedure SetLibraryBrowsingPath(const Value: TJclBorRADToolPath);
    procedure SetDebugDCUPath(const Value: string);
  protected
    constructor Create(const AConfigDataLocation: string);
    procedure ReadInformation;
    function AddMissingPathItems(var Path: string; const NewPath: string): Boolean;
  public
    destructor Destroy; override;
    class procedure ExtractPaths(const Path: TJclBorRADToolPath; List: TStrings);
    class function PackageSourceFileExtension: string; virtual;
    class function RadToolKind: TJclBorRadToolKind; virtual;
    class function RadToolName: string; virtual;
    function AnyInstanceRunning: Boolean;
    function AddToDebugDCUPath(const Path: string): Boolean;
    function AddToLibrarySearchPath(const Path: string): Boolean;
    function AddToLibraryBrowsingPath(const Path: string): Boolean;
    {$IFDEF KYLIX}
    function ConfigFileName(const Extension: string): string; virtual;
    {$ENDIF KYLIX}
    function FindFolderInPath(Folder: string; List: TStrings): Integer;
    function InstallPackage(const PackageName, BPLPath, DCPPath: string): Boolean; virtual;
    function SubstitutePath(const Path: string): string;
    function SupportsVisualCLX: Boolean;
    property BinFolderName: string read FBinFolderName;
    property BPLOutputPath: string read GetBPLOutputPath;
    property DCC: TJclDCC read GetDCC;
    property DebugDCUPath: string read GetDebugDCUPath write SetDebugDCUPath;
    property DCPOutputPath: string read GetDCPOutputPath;
    property Description: string read GetDescription;
    property Edition: TJclBorRADToolEdition read FEdition;
    property EditionAsText: string read GetEditionAsText;
    property EnvironmentVariables: TStrings read GetEnvironmentVariables;
    property IdePackages: TJclBorRADToolIdePackages read GetIdePackages;
    property IdeTools: TJclBorRADToolIdeTool read FIdeTools;
    property IdeExeBuildNumber: string read GetIdeExeBuildNumber;
    property IdeExeFileName: string read FIdeExeFileName;
    property InstalledUpdatePack: Integer read FInstalledUpdatePack;
    property LatestUpdatePack: Integer read FLatestUpdatePack;
    property LibrarySearchPath: TJclBorRADToolPath read GetLibrarySearchPath write SetLibrarySearchPath;
    property LibraryBrowsingPath: TJclBorRADToolPath read GetLibraryBrowsingPath write SetLibraryBrowsingPath;
    {$IFDEF MSWINDOWS}
    property OpenHelp: TJclBorlandOpenHelp read FOpenHelp;
    {$ENDIF}
    property ConfigData: TCustomIniFile read FConfigData;
    property Globals: TStrings read FGlobals;
    property Make: IJclCommandLineTool read FMake;
    property Name: string read GetName;
    property Palette: TJclBorRADToolPalette read GetPalette;
    property Repository: TJclBorRADToolRepository read GetRepository;
    property RootDir: string read FRootDir;
    property UpdateNeeded: Boolean read GetUpdateNeeded;
    property Valid: Boolean read GetValid;
    property VersionNumber: Integer read FVersionNumber;
  end;

  TJclBCBInstallation = class (TJclBorRADToolInstallation)
  private
    FBpr2Mak: TJclBpr2Mak;
  protected
    constructor Create(const AConfigDataLocation: string);
  public
    destructor Destroy; override;
    class function PackageSourceFileExtension: string; override;
    class function RadToolName: string; override;
    {$IFDEF KYLIX}
    function ConfigFileName(const Extension: string): string; override;
    {$ENDIF KYLIX}
    function InstallPackage(const PackageName, BPLPath, DCPPath: string): Boolean; override;
    property Bpr2Mak: TJclBpr2Mak read FBpr2Mak;
  end;

  TJclDelphiInstallation = class (TJclBorRADToolInstallation)
  public
    class function PackageSourceFileExtension: string; override;
    class function RadToolName: string; override;
    {$IFDEF KYLIX}
    function ConfigFileName(const Extension: string): string; override;
    {$ENDIF KYLIX}
    function InstallPackage(const PackageName, BPLPath, DCPPath: string): Boolean; override;
  end;

  TTraverseMethod = function (Installation: TJclBorRADToolInstallation): Boolean of object;

  TJclBorRADToolInstallations = class (TObject)
  private
    FList: TObjectList;
    function GetCount: Integer;
    function GetInstallations(Index: Integer): TJclBorRADToolInstallation;
    function GetBCBVersionInstalled(VersionNumber: Integer): Boolean;
    function GetDelphiVersionInstalled(VersionNumber: Integer): Boolean;
    function GetBCBInstallationFromVersion(VersionNumber: Integer): TJclBorRADToolInstallation;
    function GetDelphiInstallationFromVersion(VersionNumber: Integer): TJclBorRADToolInstallation;
  protected
    procedure ReadInstallations;
  public
    constructor Create;
    destructor Destroy; override;
    function AnyInstanceRunning: Boolean;
    function AnyUpdatePackNeeded(var Text: string): Boolean;
    function Iterate(TraverseMethod: TTraverseMethod): Boolean;
    property Count: Integer read GetCount;
    property Installations[Index: Integer]: TJclBorRADToolInstallation read GetInstallations; default;
    property BCBInstallationFromVersion[VersionNumber: Integer]: TJclBorRADToolInstallation read GetBCBInstallationFromVersion;
    property DelphiInstallationFromVersion[VersionNumber: Integer]: TJclBorRADToolInstallation read GetDelphiInstallationFromVersion;
    property BCBVersionInstalled[VersionNumber: Integer]: Boolean read GetBCBVersionInstalled;
    property DelphiVersionInstalled[VersionNumber: Integer]: Boolean read GetDelphiVersionInstalled;
  end;

function IsDelphiPackage(const FileName: string): Boolean;

implementation

uses
  SysConst,
  {$IFDEF MSWINDOWS}
  Registry,
  JclRegistry,
  JclMiscel,
  {$ENDIF}
  {$IFDEF UNIX}
  Libc,
  {$ENDIF}
  JclFileUtils, JclLogic, JclStrings, JclSysInfo, JclSysUtils;

//==================================================================================================
// Internal
//==================================================================================================

type
  TUpdatePack = record
    Version: Byte;
    LatestUpdatePack: Integer;
  end;
  {$IFDEF KYLIX}
  TKylixVersion = 1..3;
  {$ENDIF}

const
  {$IFDEF MSWINDOWS}
  {$IFNDEF COMPILER6_UP}
  PathSep = ';';
  {$ENDIF COMPILER6_UP}

  MSHelpSystemKeyName        = 'Software\Microsoft\Windows\Help';

  BCBKeyName                 = 'SOFTWARE\Borland\C++Builder';
  DelphiKeyName              = 'SOFTWARE\Borland\Delphi';
  {$ENDIF MSWINDOWS}

  {$IFDEF KYLIX}
  RootDirValueName           = 'DelphiRoot';
  {$ELSE}
  RootDirValueName           = 'RootDir';
  {$ENDIF}

  VersionValueName           = 'Version';

  DebuggingKeyName           = 'Debugging';
  DebugDCUPathValueName      = 'Debug DCUs Path';

  GlobalsKeyName             = 'Globals';

  LibraryKeyName             = 'Library';
  LibrarySearchPathValueName = 'Search Path';
  LibraryBrowsingPathValueName = 'Browsing Path';
  LibraryBPLOutputValueName  = 'Package DPL Output';
  LibraryDCPOutputValueName  = 'Package DCP Output';

  TransferKeyName            = 'Transfer';
  TransferCountValueName     = 'Count';
  TransferPathValueName      = 'Path%d';
  TransferParamsValueName    = 'Params%d';
  TransferTitleValueName     = 'Title%d';
  TransferWorkDirValueName   = 'WorkingDir%d';

  DisabledPackagesKeyName    = 'Disabled Packages';
  EnvVariablesKeyName        = 'Environment Variables';
  KnownPackagesKeyName       = 'Known Packages';

  PaletteKeyName             = 'Palette';
  PaletteHiddenTag           = '.Hidden';

  {$IFDEF MSWINDOWS}
  DelphiIdeFileName          = 'Bin\delphi32.exe';
  BCBIdeFileName             = 'Bin\bcb.exe';
  MakeExeName                = 'make.exe';
  Bpr2MakExeName             = 'bpr2mak.exe';
  DelphiOptionsFileExtension = '.dof';
  BorRADToolRepositoryFileName   = 'Bin\delphi32.dro';
  DCCFileName                = 'Bin\dcc32.exe';
  HelpContentFileName        = '%s\Help\%s%d.ohc';
  HelpIndexFileName          = '%s\Help\%s%d.ohi';
  HelpLinkFileName           = '%s\Help\%s%d.ohl';
  HelpProjectFileName        = '%s\Help\%s%d.ohp';
  HelpGidFileName            = '%s\Help\%s%d.gid';
  {$ENDIF MSWINDOWS}

  {$IFDEF KYLIX}
  IDs: array[1..3] of Integer = (60, 65, 69);

  DelphiIdeFileName          = 'bin/delphi';
  BCBIdeFileName             = 'bin/bcblin';
  MakeExeName                = 'make';
  Bpr2MakExeName             = 'bpr2mak';
  DelphiOptionsFileExtension = '.kof';

  LibSuffixes: array[TKylixVersion] of string[3] = ('6.0', '6.5', '6.9');

  DCCFileName              = 'bin/dcc';
  KylixHelpNamePart          = 'k%d';
  {$ENDIF KYLIX}

  {$IFDEF KYLIX}
  LatestUpdatePacks: array [1..3] of TUpdatePack = ( // Updated Sep 5, 2002
    (Version: 1; LatestUpdatePack: 0),
    (Version: 2; LatestUpdatePack: 0),
    (Version: 3; LatestUpdatePack: 0)
  {$ELSE}
  LatestUpdatePacks: array [TJclBorRADToolKind, 1..3] of TUpdatePack = ( // Updated 2004-03-20
    ((Version: 5; LatestUpdatePack: 1),
     (Version: 6; LatestUpdatePack: 2),
     (Version: 7; LatestUpdatePack: 0)),
    ((Version: 5; LatestUpdatePack: 0),
     (Version: 6; LatestUpdatePack: 4),
     (Version: 0; LatestUpdatePack: 0))
  {$ENDIF}
  );

resourcestring
  RsIndexOufOfRange = 'Index out of range';
  RsNeedUpdate      = 'You should install latest Update Pack #%d for %s';
  RsUpdatePackName  = 'Update Pack #%d';
  {$IFDEF KYLIX}
  RsDelphiName      = 'Kylix %d for Delphi';
  RsBCBName         = 'Kylix %d for C++Builder';
  RsOpenEdition     = 'Open Edition';
  RsServerDeveloper = 'Server Developer';
  {$ELSE}
  RsDelphiName      = 'Delphi %d';
  RsBCBName         = 'C++Builder %d';
  RsClientServer    = 'Client/Server';
  RsStandard        = 'Standard';
  {$ENDIF}
  RsArchitect       = 'Architect';
  RsEnterprise      = 'Enterprise';
  RsPersonal        = 'Personal';
  RsProfessional    = 'Professional';

const
  RsToolNames: array[TJclBorRADToolKind] of string = (RsDelphiName, RsBCBName);

//--------------------------------------------------------------------------------------------------

function IsDelphiPackage(const FileName: string): Boolean;
begin
  Result := SameText(ExtractFileExt(FileName), TJclDelphiInstallation.PackageSourceFileExtension);
  { TODO : Add some plausibility tests }
  { like
  var
    F: TextFile;
    FirstLine: string;
    
  if FileExists(FileName) then
  begin
    AssignFile(F, FileName);
    Reset(F);
    ReadLn(F, FirstLine);
    Result := Pos('package ', FirstLine) = 1;
    CloseFile(F);
  end;
  }
end;

//--------------------------------------------------------------------------------------------------

{$IFDEF MSWINDOWS}
function RegGetValueNamesAndValues(const RootKey: HKEY; const Key: string; const List: TStrings): Boolean;
var
  I: Integer;
  TempList: TStringList;
begin
  TempList := TStringList.Create;
  try
    Result := RegKeyExists(RootKey, Key) and RegGetValueNames(RootKey, Key, TempList);
    if Result then
    begin
      for I := 0 to TempList.Count - 1 do
        TempList[I] := TempList[I] + '=' + RegReadStringDef(RootKey, Key, TempList[I], '');
      List.AddStrings(TempList);
    end;
  finally
    TempList.Free;
  end;
end;
{$ENDIF MSWINDOWS}

//--------------------------------------------------------------------------------------------------

function ExecAndRedirectOutput(const CommandLine: string; var Output: string): Integer;
{$IFDEF MSWINDOWS}
begin
  Result := WinExec32AndRedirectOutput(CommandLine, Output);
end;
{$ENDIF MSWINDOWS}
{$IFDEF UNIX}
var
  Pipe: PIOFile;
  Count: Integer;
  Buffer: array[Byte] of Char;
  Cmd, TempOutput: string;
begin
  Cmd := Format('%s 2>&1', [CommandLine]);
  Pipe := Libc.popen(PChar(Cmd), 'r');
  repeat
    Count := fread_unlocked(@Buffer, 1, Length(Buffer) - 1, Pipe);
    if Count > 0 then
    begin
      Buffer[Count] := #0;
      TempOutput := TempOutput + Buffer;
    end;
  until Count < Length(Buffer) - 1;
  Output := AdjustLineBreaks(Output + TempOutput);
  Result := pclose(Pipe);
  wait(nil);
end;
{$ENDIF UNIX}

//==================================================================================================
// TJclBorRADToolInstallationObject
//==================================================================================================

constructor TJclBorRADToolInstallationObject.Create(AInstallation: TJclBorRADToolInstallation);
begin
  FInstallation := AInstallation;
end;

//==================================================================================================
// TJclBorlandOpenHelp
//==================================================================================================

{$IFDEF MSWINDOWS}

function TJclBorlandOpenHelp.AddHelpFile(const HelpFileName, IndexName: string): Boolean;
var
  CntFileName, HelpName, CntName: string;
  List: TStringList;

  procedure AddToList(const FileName, Text: string);
  var
    I, Attr: Integer;
    Found: Boolean;
  begin
    List.LoadFromFile(FileName);
    Found := False;
    for I := 0 to List.Count - 1 do
      if AnsiSameText(Trim(List[I]), Text) then
      begin
        Found := True;
        Break;
      end;
    if not Found then
    begin
      List.Add(Text);
      Attr := FileGetAttr(FileName);
      FileSetAttr(FileName, faArchive);
      List.SaveToFile(FileName);
      FileSetAttr(FileName, Attr);
    end;
  end;

begin
  CntFileName := ChangeFileExt(HelpFileName, '.cnt');
  Result := FileExists(HelpFileName) and FileExists(CntFileName);
  if Result then
  begin
    HelpName := ExtractFileName(HelpFileName);
    CntName := ExtractFileName(CntFileName);
    RegWriteString(HKEY_LOCAL_MACHINE, MSHelpSystemKeyName, HelpName, ExtractFilePath(HelpFileName));
    RegWriteString(HKEY_LOCAL_MACHINE, MSHelpSystemKeyName, CntName, ExtractFilePath(CntFileName));
    List := TStringList.Create;
    try
      AddToList(ContentFileName, Format(':Include %s', [CntName]));
      AddToList(LinkFileName, Format(':Link %s', [HelpName]));
      AddToList(IndexFileName, Format(':Index %s=%s', [IndexName, HelpName]));
      SetFileLastWrite(ProjectFileName, Now);
      DeleteFile(GidFileName);
    finally
      List.Free;
    end;
  end;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorlandOpenHelp.GetContentFileName: string;
begin
  Result := ReadFileName(HelpContentFileName);
end;

//--------------------------------------------------------------------------------------------------

function TJclBorlandOpenHelp.GetGidFileName: string;
begin
  Result := ReadFileName(HelpGidFileName);
end;

//--------------------------------------------------------------------------------------------------

function TJclBorlandOpenHelp.GetIndexFileName: string;
begin
  Result := ReadFileName(HelpIndexFileName);
end;

//--------------------------------------------------------------------------------------------------

function TJclBorlandOpenHelp.GetLinkFileName: string;
begin
  Result := ReadFileName(HelpLinkFileName);
end;

//--------------------------------------------------------------------------------------------------

function TJclBorlandOpenHelp.GetProjectFileName: string;
begin
  Result := ReadFileName(HelpProjectFileName);
end;

//--------------------------------------------------------------------------------------------------

function TJclBorlandOpenHelp.ReadFileName(const FormatName: string): string;
var
  S: string;
begin
  with Installation do
  begin
    if RADToolKind = brDelphi then
    begin
      if VersionNumber <= 6 then
        S := 'delphi'
      else
        S := 'd';
    end
    else
      S := 'bcb';
    Result := Format(FormatName, [RootDir, S, VersionNumber]);
  end;
end;

{$ENDIF MSWINDOWS}

//==================================================================================================
// TJclBorRADToolIdeTool
//==================================================================================================

procedure TJclBorRADToolIdeTool.CheckIndex(Index: Integer);
begin
  if (Index < 0) or (Index >= Count) then
    raise EJclError.CreateResRec(@RsIndexOufOfRange);
end;

//--------------------------------------------------------------------------------------------------

constructor TJclBorRADToolIdeTool.Create(AInstallation: TJclBorRADToolInstallation);
begin
  inherited;
  FKey := TransferKeyName;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolIdeTool.GetCount: Integer;
begin
  Result := Installation.ConfigData.ReadInteger(Key, TransferCountValueName, 0);
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolIdeTool.GetParameters(Index: Integer): string;
begin
  CheckIndex(Index);
  Result := Installation.ConfigData.ReadString(Key, Format(TransferParamsValueName, [Index]), '');
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolIdeTool.GetPath(Index: Integer): string;
begin
  CheckIndex(Index);
  Result := Installation.ConfigData.ReadString(Key, Format(TransferPathValueName, [Index]), '');
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolIdeTool.GetTitle(Index: Integer): string;
begin
  CheckIndex(Index);
  Result := Installation.ConfigData.ReadString(Key, Format(TransferTitleValueName, [Index]), '');
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolIdeTool.GetWorkingDir(Index: Integer): string;
begin
  CheckIndex(Index);
  Result := Installation.ConfigData.ReadString(Key, Format(TransferWorkDirValueName, [Index]), '');
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolIdeTool.IndexOfPath(const Value: string): Integer;
var
  I: Integer;
begin
  Result := -1;
  for I := 0 to Count - 1 do
    if AnsiSameText(Title[I], Value) then
    begin
      Result := I;
      Break;
    end;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolIdeTool.IndexOfTitle(const Value: string): Integer;
var
  I: Integer;
begin
  Result := -1;
  for I := 0 to Count - 1 do
    if Title[I] = Value then
    begin
      Result := I;
      Break;
    end;
end;

//--------------------------------------------------------------------------------------------------

procedure TJclBorRADToolIdeTool.SetCount(const Value: Integer);
begin
  if Value > Count then
    Installation.ConfigData.WriteInteger(Key, TransferCountValueName, Value);
end;

//--------------------------------------------------------------------------------------------------

procedure TJclBorRADToolIdeTool.SetParameters(Index: Integer; const Value: string);
begin
  CheckIndex(Index);
  Installation.ConfigData.WriteString(Key, Format(TransferParamsValueName, [Index]), Value);
end;

//--------------------------------------------------------------------------------------------------

procedure TJclBorRADToolIdeTool.SetPath(Index: Integer; const Value: string);
begin
  CheckIndex(Index);
  Installation.ConfigData.WriteString(Key, Format(TransferPathValueName, [Index]), Value);
end;

//--------------------------------------------------------------------------------------------------

procedure TJclBorRADToolIdeTool.SetTitle(Index: Integer; const Value: string);
begin
  CheckIndex(Index);
  Installation.ConfigData.WriteString(Key, Format(TransferTitleValueName, [Index]), Value);
end;

//--------------------------------------------------------------------------------------------------

procedure TJclBorRADToolIdeTool.SetWorkingDir(Index: Integer; const Value: string);
begin
  CheckIndex(Index);
  Installation.ConfigData.WriteString(Key, Format(TransferWorkDirValueName, [Index]), Value);
end;

//==================================================================================================
// TJclBorRADToolIdePackages
//==================================================================================================

function TJclBorRADToolIdePackages.AddPackage(const FileName, Description: string): Boolean;
begin
  Result := True;
  RemoveDisabled(FileName);
  Installation.ConfigData.WriteString(KnownPackagesKeyName, FileName, Description);
  ReadPackages;
end;

//--------------------------------------------------------------------------------------------------

constructor TJclBorRADToolIdePackages.Create(AInstallation: TJclBorRADToolInstallation);
begin
  inherited;
  FDisabledPackages := TStringList.Create;
  FDisabledPackages.Sorted := True;
  FDisabledPackages.Duplicates := dupIgnore;
  FKnownPackages := TStringList.Create;
  FKnownPackages.Sorted := True;
  FKnownPackages.Duplicates := dupIgnore;
  ReadPackages;
end;

//--------------------------------------------------------------------------------------------------

destructor TJclBorRADToolIdePackages.Destroy;
begin
  FreeAndNil(FDisabledPackages);
  FreeAndNil(FKnownPackages);
  inherited;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolIdePackages.GetCount: Integer;
begin
  Result := FKnownPackages.Count;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolIdePackages.GetPackageDescriptions(Index: Integer): string;
begin
  Result := FKnownPackages.Values[FKnownPackages.Names[Index]];
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolIdePackages.GetPackageDisabled(Index: Integer): Boolean;
begin
  Result := Boolean(FKnownPackages.Objects[Index]);
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolIdePackages.GetPackageFileNames(Index: Integer): string;
begin
  Result := PackageEntryToFileName(FKnownPackages.Names[Index]);
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolIdePackages.PackageEntryToFileName(const Entry: string): string;
begin
  Result := {$IFDEF MSWINDOWS} PathGetLongName2 {$ENDIF} (Installation.SubstitutePath(Entry));
end;

//--------------------------------------------------------------------------------------------------

procedure TJclBorRADToolIdePackages.ReadPackages;

  procedure ReadPackageList(const Name: string; List: TStringList);
  var
    ListIsSorted: Boolean;
  begin
    ListIsSorted := List.Sorted;
    List.Sorted := False;
    List.Clear;
    Installation.ConfigData.ReadSectionValues(Name, List);
    List.Sorted := ListIsSorted;
  end;

var
  I: Integer;
begin
  ReadPackageList(KnownPackagesKeyName, FKnownPackages);
  ReadPackageList(DisabledPackagesKeyName, FDisabledPackages);
  for I := 0 to Count - 1 do
    if FDisabledPackages.IndexOfName(FKnownPackages.Names[I]) <> -1 then
      FKnownPackages.Objects[I] := Pointer(True);
end;

//--------------------------------------------------------------------------------------------------

procedure TJclBorRADToolIdePackages.RemoveDisabled(const FileName: string);
var
  I: Integer;
begin
  for I := 0 to FDisabledPackages.Count - 1 do
    if AnsiSameText(FileName, PackageEntryToFileName(FDisabledPackages.Names[I])) then
    begin
      Installation.ConfigData.DeleteKey(DisabledPackagesKeyName, FDisabledPackages.Names[I]);
      ReadPackages;
      Break;
    end;
end;

//==================================================================================================
// TJclBorlandCommandLineTool
//==================================================================================================

procedure TJclBorlandCommandLineTool.AddPathOption(const Option, Path: string);
begin
  Options.Add(Format('-%s"%s"', [Option, PathRemoveSeparator(Path)]));
end;

//--------------------------------------------------------------------------------------------------

constructor TJclBorlandCommandLineTool.Create(AInstallation: TJclBorRADToolInstallation);
begin
  inherited;
  FOptions := TStringList.Create;
end;

//--------------------------------------------------------------------------------------------------

destructor TJclBorlandCommandLineTool.Destroy;
begin
  FreeAndNil(FOptions);
  inherited;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorlandCommandLineTool.Execute(const CommandLine: string): Boolean;
const
  {$IFDEF MSWINDOWS}
  S = '"%s" %s';
  {$ENDIF MSWINDOWS}
  {$IFDEF UNIX}
  S = '%s %s';
  {$ENDIF UNIX}
begin
  Result := ExecAndRedirectOutput(Format(S, [FileName, CommandLine]), FOutput) = 0;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorlandCommandLineTool.GetExeName: string;
begin
  Result := '';
  {$IFDEF MSWINDOWS}
  raise EAbstractError.CreateResFmt(@SAbstractError, ['']); // BCB doesn't support abstract keyword
  {$ENDIF MSWINDOWS}
end;

//--------------------------------------------------------------------------------------------------

function TJclBorlandCommandLineTool.GetFileName: string;
begin
  Result := Installation.BinFolderName + GetExeName;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorlandCommandLineTool.GetOptions: TStrings;
begin
  Result := FOptions;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorlandCommandLineTool.GetOutput: string;
begin
  Result := FOutput;
end;

//==================================================================================================
// TJclDCC
//==================================================================================================

function TJclDCC.Execute(const CommandLine: string): Boolean;
const
{$IFDEF WIN32}
  ConfFileName = 'DCC32.CFG';
{$ENDIF WIN32}
{$IFDEF KYLIX}
  ConfFileName = 'dcc.conf';
{$ENDIF KYLIX}
begin
  FOutput := '';
  FOptions.SaveToFile(ConfFileName);
  Result := inherited Execute(CommandLine);
  DeleteFile(ConfFileName);
end;

//--------------------------------------------------------------------------------------------------

function TJclDCC.GetExeName: string;
begin
  {$IFDEF MSWINDOWS}
  Result := 'dcc32.exe';
  {$ENDIF MSWINDOWS}
  {$IFDEF UNIX}
  Result := 'dcc';
  {$ENDIF UNIX}
end;

//--------------------------------------------------------------------------------------------------

function TJclDCC.InstallPackage(const PackageName, BPLPath, DCPPath: string): Boolean;
const
  DOFDirectoriesSection = 'Directories';
  UnitOutputDirName     = 'UnitOutputDir';
  SearchPathName        = 'SearchPath';
  DescriptionOption     = '{$DESCRIPTION ''';
  LibSuffixOption       = '{$LIBSUFFIX ''';
  RunOnlyOption         = '{$RUNONLY}';
var
  SaveDir, PackagePath, Description, LibSuffix, BPLFileName, S: string;
  RunOnly: Boolean;
  OptionsFile: TIniFile;
  DPKFile: TStringList;
  I: Integer;
begin
  PackagePath := PathRemoveSeparator(ExtractFilePath(PackageName));
  SaveDir := GetCurrentDir;
  SetCurrentDir(PackagePath);
  try
    OptionsFile := TIniFile.Create(ChangeFileExt(PackageName, DelphiOptionsFileExtension));
    try
      Options.Clear;
      S := OptionsFile.ReadString(DOFDirectoriesSection, SearchPathName, '');
      AddPathOption('N', OptionsFile.ReadString(DOFDirectoriesSection, UnitOutputDirName, ''));
      AddPathOption('I', S);
      AddPathOption('R', S);
      AddPathOption('LE', BPLPath);
      AddPathOption('LN', DCPPath);
      AddPathOption('U', StrEnsureSuffix(PathSep, DCPPath) + S);
    finally
      OptionsFile.Free;
    end;
    Result := Execute(PackageName);
  finally
    SetCurrentDir(SaveDir);
  end;
  if Result then
  begin
    DPKFile := TStringList.Create;
    try
      DPKFile.LoadFromFile(PackageName);
      Description := '';
      LibSuffix := '';
      RunOnly := False;
      for I := 0 to DPKFile.Count - 1 do
      begin
        S := TrimRight(DPKFile[I]);
        if Pos(DescriptionOption, S) = 1 then
          Description := Copy(S, Length(DescriptionOption), Length(S) - Length(DescriptionOption))
        else
        if Pos(LibSuffixOption, S) = 1 then
          LibSuffix := Copy(S, Length(LibSuffixOption), Length(S) - Length(LibSuffixOption))
        else
        if Pos(RunOnlyOption, S) = 1 then
          RunOnly := True;
      end;
      if not RunOnly then
      begin
        BPLFileName := PathAddSeparator(BPLPath) + PathExtractFileNameNoExt(PackageName) + LibSuffix + '.bpl';
        Result := Installation.IdePackages.AddPackage(BPLFileName, Description);
      end;
    finally
      DPKFile.Free;
    end;
  end;
end;

//--------------------------------------------------------------------------------------------------

function TJclDCC.SupportsLibSuffix: Boolean;
begin
  {$IFDEF KYLIX}
  Result := True;
  {$ELSE}
  Result := Installation.VersionNumber >= 6;
  {$ENDIF}
end;

//==================================================================================================
// TJclBorlandMake
//==================================================================================================

function TJclBorlandMake.GetExeName: string;
begin
  Result := MakeExeName;
end;

//==================================================================================================
// TJclBpr2Mak
//==================================================================================================

function TJclBpr2Mak.GetExeName: string;
begin
  Result := Bpr2MakExeName;
end;

//==================================================================================================
// TJclBorRADToolPalette
//==================================================================================================

procedure TJclBorRADToolPalette.ComponentsOnTabToStrings(Index: Integer; Strings: TStrings;
  IncludeUnitName: Boolean; IncludeHiddenComponents: Boolean);
var
  TempList: TStringList;

  procedure ProcessList(Hidden: Boolean);
  var
    D, I: Integer;
    List, S: string;
  begin
    if Hidden then
      List := HiddenComponentsOnTab[Index]
    else
      List := ComponentsOnTab[Index];
    List := StrEnsureSuffix(';', List);
    while Length(List) > 1 do
    begin
      D := Pos(';', List);
      S := Trim(Copy(List, 1, D - 1));
      if not IncludeUnitName then
        Delete(S, 1, Pos('.', S));
      if Hidden then
      begin
        I := TempList.IndexOf(S);
        if I = -1 then
          TempList.AddObject(S, Pointer(True))
        else
          TempList.Objects[I] := Pointer(True);
      end
      else
        TempList.Add(S);
      Delete(List, 1, D);
    end;
  end;

begin
  TempList := TStringList.Create;
  try
    TempList.Duplicates := dupError;
    ProcessList(False);
    TempList.Sorted := True;
    if IncludeHiddenComponents then
      ProcessList(True);
    Strings.AddStrings(TempList);
  finally
    TempList.Free;
  end;
end;

//--------------------------------------------------------------------------------------------------

constructor TJclBorRADToolPalette.Create(AInstallation: TJclBorRADToolInstallation);
begin
  inherited;
  FKey := PaletteKeyName;
  FTabNames := TStringList.Create;
  FTabNames.Sorted := True;
  ReadTabNames;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolPalette.DeleteTabName(const TabName: string): Boolean;
var
  I: Integer;
begin
  I := FTabNames.IndexOf(TabName);
  Result := I >= 0;
  if Result then
  begin
    Installation.ConfigData.DeleteKey(Key, FTabNames[I]);
    Installation.ConfigData.DeleteKey(Key, FTabNames[I] + PaletteHiddenTag);
    FTabNames.Delete(I);
  end;
end;

//--------------------------------------------------------------------------------------------------

destructor TJclBorRADToolPalette.Destroy;
begin
  FreeAndNil(FTabNames);
  inherited;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolPalette.GetComponentsOnTab(Index: Integer): string;
begin
  Result := Installation.ConfigData.ReadString(Key, FTabNames[Index], '');
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolPalette.GetHiddenComponentsOnTab(Index: Integer): string;
begin
  Result := Installation.ConfigData.ReadString(Key, FTabNames[Index] + PaletteHiddenTag, '');
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolPalette.GetTabNameCount: Integer;
begin
  Result := FTabNames.Count;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolPalette.GetTabNames(Index: Integer): string;
begin
  Result := FTabNames[Index];
end;

//--------------------------------------------------------------------------------------------------

procedure TJclBorRADToolPalette.ReadTabNames;
var
  TempList: TStringList;
  I: Integer;
  S: string;
begin
  if Installation.ConfigData.SectionExists(Key) then
  begin
    TempList := TStringList.Create;
    try
      Installation.ConfigData.ReadSection(Key, TempList);
      for I := 0 to TempList.Count - 1 do
      begin
        S := TempList[I];
        if Pos(PaletteHiddenTag, S) = 0 then
          FTabNames.Add(S);
      end;
    finally
      TempList.Free;
    end;
  end;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolPalette.TabNameExists(const TabName: string): Boolean;
begin
  Result := FTabNames.IndexOf(TabName) <> -1;
end;

//==================================================================================================
// TJclBorRADToolRepository
//==================================================================================================

procedure TJclBorRADToolRepository.AddObject(const FileName, ObjectType, PageName, ObjectName,
  IconFileName, Description, Author, Designer: string; const Ancestor: string);
var
  SectionName: string;
begin
  GetIniFile;
  SectionName := AnsiUpperCase(PathRemoveExtension(FileName));
  FIniFile.EraseSection(FileName);
  FIniFile.EraseSection(SectionName);
  FIniFile.WriteString(SectionName, BorRADToolRepositoryObjectType, ObjectType);
  FIniFile.WriteString(SectionName, BorRADToolRepositoryObjectName, ObjectName);
  FIniFile.WriteString(SectionName, BorRADToolRepositoryObjectPage, PageName);
  FIniFile.WriteString(SectionName, BorRADToolRepositoryObjectIcon, IconFileName);
  FIniFile.WriteString(SectionName, BorRADToolRepositoryObjectDescr, Description);
  FIniFile.WriteString(SectionName, BorRADToolRepositoryObjectAuthor, Author);
  if Ancestor <> '' then
    FIniFile.WriteString(SectionName, BorRADToolRepositoryObjectAncestor, Ancestor);
  if Installation.VersionNumber >= 6 then
    FIniFile.WriteString(SectionName, BorRADToolRepositoryObjectDesigner, Designer);
  FIniFile.WriteBool(SectionName, BorRADToolRepositoryObjectNewForm, False);
  FIniFile.WriteBool(SectionName, BorRADToolRepositoryObjectMainForm, False);
  CloseIniFile;
end;

//--------------------------------------------------------------------------------------------------

procedure TJclBorRADToolRepository.CloseIniFile;
begin
  FreeAndNil(FIniFile);
end;

//--------------------------------------------------------------------------------------------------

constructor TJclBorRADToolRepository.Create(AInstallation: TJclBorRADToolInstallation);
begin
  inherited;
  {$IFDEF KYLIX}
  FFileName := AInstallation.ConfigFileName('dro');
  {$ELSE}
  FFileName := PathAddSeparator(Installation.RootDir) + BorRADToolRepositoryFileName;
  {$ENDIF}
  FPages := TStringList.Create;
  IniFile.ReadSection(BorRADToolRepositoryPagesSection, FPages);
  CloseIniFile;
end;

//--------------------------------------------------------------------------------------------------

destructor TJclBorRADToolRepository.Destroy;
begin
  FreeAndNil(FPages);
  FreeAndNil(FIniFile);
  inherited;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolRepository.FindPage(const Name: string; OptionalIndex: Integer): string;
var
  I: Integer;
begin
  I := FPages.IndexOf(Name);
  if I >= 0 then
    Result := FPages[I]
  else
  begin
    if OptionalIndex < FPages.Count then
      Result := FPages[OptionalIndex]
    else
      Result := '';
  end;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolRepository.GetIniFile: TIniFile;
begin
  if not Assigned(FIniFile) then
    FIniFile := TIniFile.Create(FileName);
  Result := FIniFile;
end;

//--------------------------------------------------------------------------------------------------

procedure TJclBorRADToolRepository.RemoveObjects(const PartialPath, FileName, ObjectType: string);
var
  Sections: TStringList;
  I: Integer;
  SectionName, FileNamePart, PathPart, DialogFileName: string;
begin
  Sections := TStringList.Create;
  try
    GetIniFile;
    FIniFile.ReadSections(Sections);
    for I := 0 to Sections.Count - 1 do
    begin
      SectionName := Sections[I];
      if FIniFile.ReadString(SectionName, BorRADToolRepositoryObjectType, '') = ObjectType then
      begin
        FileNamePart := PathExtractFileNameNoExt(SectionName);
        PathPart := StrRight(PathAddSeparator(ExtractFilePath(SectionName)), Length(PartialPath));
        DialogFileName := PathExtractFileNameNoExt(FileName);
        if StrSame(FileNamePart, DialogFileName) and StrSame(PathPart, PartialPath) then
          FIniFile.EraseSection(SectionName);
      end;
    end;
  finally
    Sections.Free;
  end;
end;

//==================================================================================================
// TJclBorRADToolInstallation
//==================================================================================================

function TJclBorRADToolInstallation.AddToDebugDCUPath(const Path: string): Boolean;
var
  TempDebugDCUPath: TJclBorRADToolPath;
begin
  TempDebugDCUPath := DebugDCUPath;
  Result := AddMissingPathItems(TempDebugDCUPath, Path);
  DebugDCUPath := TempDebugDCUPath;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.AddToLibrarySearchPath(const Path: string): Boolean;
var
  TempLibraryPath: TJclBorRADToolPath;
begin
  TempLibraryPath := LibrarySearchPath;
  Result := AddMissingPathItems(TempLibraryPath, Path);
  LibrarySearchPath := TempLibraryPath;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.AddToLibraryBrowsingPath(const Path: string): Boolean;
var
  TempLibraryPath: TJclBorRADToolPath;
begin
  TempLibraryPath := LibraryBrowsingPath;
  Result := AddMissingPathItems(TempLibraryPath, Path);
  LibraryBrowsingPath := TempLibraryPath;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.AnyInstanceRunning: Boolean;
var
  Processes: TStringList;
  I: Integer;
begin
  Result := False;
  Processes := TStringList.Create;
  try
    if RunningProcessesList(Processes) then
    begin
      for I := 0 to Processes.Count - 1 do
        if AnsiSameText(IdeExeFileName, Processes[I]) then
        begin
          Result := True;
          Break;
        end;
    end;
  finally
    Processes.Free;
  end;
end;

//--------------------------------------------------------------------------------------------------

{$IFDEF KYLIX}
function TJclBorRADToolInstallation.ConfigFileName(const Extension: string): string;
begin
  Result := '';
end;
{$ENDIF KYLIX}

//--------------------------------------------------------------------------------------------------

constructor TJclBorRADToolInstallation.Create;
begin
  {$IFDEF KYLIX}
  FConfigData := TMemIniFile.Create(AConfigDataLocation);
  FMake := TJclCommandLineTool.Create('make');
  {$ELSE}
  FConfigData := TRegistryIniFile.Create(AConfigDataLocation);
  FMake := TJclBorlandMake.Create(Self);
  {$ENDIF}
  FGlobals := TStringList.Create;
  ReadInformation;
  FIdeTools := TJclBorRADToolIdeTool.Create(Self);
  {$IFNDEF KYLIX}
  FOpenHelp := TJclBorlandOpenHelp.Create(Self);
  {$ENDIF}
end;

//--------------------------------------------------------------------------------------------------

destructor TJclBorRADToolInstallation.Destroy;
begin
  FreeAndNil(FRepository);
  FreeAndNil(FDCC);
  FreeAndNil(FIdePackages);
  FreeAndNil(FIdeTools);
  {$IFDEF MSWINDOWS}
  FreeAndNil(FOpenHelp);
  {$ENDIF}
  FreeAndNil(FPalette);
  FreeAndNil(FGlobals);
  {$IFDEF KYLIX}
  FConfigData.UpdateFile; // TMemIniFile.Destroy doesn't call UpdateFile
  {$ENDIF KYLIX}
  FreeAndNil(FConfigData);
  inherited;
end;

//--------------------------------------------------------------------------------------------------

class procedure TJclBorRADToolInstallation.ExtractPaths(const Path: TJclBorRADToolPath; List: TStrings);
begin
  StrToStrings(Path, PathSep, List);
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.AddMissingPathItems(var Path: string; const NewPath: string): Boolean;
var
  PathItems, NewItems: TStringList;
  Folder: string;
  I: Integer;
  Missing: Boolean;
begin
  Result := False;
  PathItems := nil;
  NewItems := nil;
  try
    PathItems := TStringList.Create;
    NewItems := TStringList.Create;
    ExtractPaths(Path, PathItems);
    ExtractPaths(NewPath, NewItems);
    for I := 0 to NewItems.Count - 1 do
    begin
      Folder := NewItems[I];
      Missing := FindFolderInPath(Folder, PathItems) = -1;
      if Missing then
      begin
        Path := StrEnsureSuffix(PathSep, Path) + Folder;
        Result := True;
      end;
    end;
  finally
    PathItems.Free;
    NewItems.Free;
  end;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.FindFolderInPath(Folder: string; List: TStrings): Integer;
var
  I: Integer;
begin
  Result := -1;
  Folder := PathRemoveSeparator(Folder);
  for I := 0 to List.Count - 1 do
    if AnsiSameText(Folder, PathRemoveSeparator(SubstitutePath(List[I]))) then
    begin
      Result := I;
      Break;
    end;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.GetBPLOutputPath: string;
begin
  Result := SubstitutePath(ConfigData.ReadString(LibraryKeyName, LibraryBPLOutputValueName, ''));
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.GetDCC: TJclDCC;
begin
  if not Assigned(FDCC) then
    FDCC := TJclDCC.Create(Self);
  Result := FDCC;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.GetDCPOutputPath: string;
begin
  Result := SubstitutePath(ConfigData.ReadString(LibraryKeyName, LibraryDCPOutputValueName, ''));
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.GetDebugDCUPath: string;
begin
  Result := ConfigData.ReadString(DebuggingKeyName, DebugDCUPathValueName, '');
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.GetDescription: string;
begin
  Result := Format('%s %s', [Name, EditionAsText]);
  if InstalledUpdatePack > 0 then
    Result := Result + ' ' + Format(RsUpdatePackName, [InstalledUpdatePack]);
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.GetEditionAsText: string;
begin
  {$IFDEF KYLIX}
  case Edition of
    deOPEN:
      Result := RsOpenEdition;
    dePRO:
      Result := RsProfessional;
    deSVR:
      if VersionNumber >= 2 then
        Result := RsEnterprise
      else
        Result := RsServerDeveloper;
  end;
  {$ELSE KYLIX}
  case Edition of
    deSTD:
      if VersionNumber >= 6 then
        Result := RsPersonal
      else
        Result := RsStandard;
    dePRO:
      Result := RsProfessional;
    deCSS:
      if VersionNumber >= 5 then
        Result := RsEnterprise
      else
        Result := RsClientServer;
  end;
  {$ENDIF KYLIX}
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.GetEnvironmentVariables: TStrings;
var
  EnvNames: TStringList;
  EnvVarKeyName: string;
  I: Integer;
begin
  if FEnvironmentVariables = nil then
  begin
    FEnvironmentVariables := TStringList.Create;
    if (VersionNumber >= 6) and ConfigData.SectionExists(EnvVariablesKeyName) then
    begin
      EnvNames := TStringList.Create;
      try
        ConfigData.ReadSection(EnvVariablesKeyName, EnvNames);
        for I := 0 to EnvNames.Count - 1 do
          FEnvironmentVariables.Values[EnvNames[I]] := ConfigData.ReadString(EnvVarKeyName, EnvNames[I], '');
      finally
        EnvNames.Free;
      end;
    end;
    if RADToolKind = brCppBuilder then
      FEnvironmentVariables.Values['BCB'] := RootDir
    else
      FEnvironmentVariables.Values['DELPHI'] := RootDir;
  end;
  Result := FEnvironmentVariables;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.GetIdeExeBuildNumber: string;
begin
  {$IFDEF KYLIX}
  { TODO : determine Kylix IDE build # }
  Result := '?';
  {$ELSE}
  Result := VersionFixedFileInfoString(IdeExeFileName, vfFull);
  {$ENDIF}
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.GetIdePackages: TJclBorRADToolIdePackages;
begin
  if not Assigned(FIdePackages) then
    FIdePackages := TJclBorRADToolIdePackages.Create(Self);
  Result := FIdePackages;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.GetLibrarySearchPath: TJclBorRADToolPath;
begin
  Result := ConfigData.ReadString(LibraryKeyName, LibrarySearchPathValueName, '');
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.GetLibraryBrowsingPath: TJclBorRADToolPath;
begin
  Result := ConfigData.ReadString(LibraryKeyName, LibraryBrowsingPathValueName, '');
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.GetName: string;
begin
  Result := Format(RsToolNames[RadToolKind], [VersionNumber]);
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.GetPalette: TJclBorRADToolPalette;
begin
  if not Assigned(FPalette) then
    FPalette := TJclBorRADToolPalette.Create(Self);
  Result := FPalette;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.GetRepository: TJclBorRADToolRepository;
begin
  if not Assigned(FRepository) then
    FRepository := TJclBorRADToolRepository.Create(Self);
  Result := FRepository;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.GetUpdateNeeded: Boolean;
begin
  Result := InstalledUpdatePack < LatestUpdatePack;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.GetValid: Boolean;
begin
  Result := (ConfigData.FileName <> '') and (RootDir <> '') and FileExists(IdeExeFileName);
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.InstallPackage(const PackageName, BPLPath, DCPPath: string): Boolean;
begin
  {$IFDEF MSWINDOWS}
  raise EAbstractError.CreateResFmt(@SAbstractError, ['']); // BCB doesn't support abstract keyword
  {$ENDIF MSWINDOWS}
  Result := False;
end;

//--------------------------------------------------------------------------------------------------

class function TJclBorRADToolInstallation.PackageSourceFileExtension: string;
begin
  Result := '';
  {$IFDEF MSWINDOWS}
  raise EAbstractError.CreateResFmt(@SAbstractError, ['']); // BCB doesn't support abstract keyword
  {$ENDIF MSWINDOWS}
end;

//--------------------------------------------------------------------------------------------------

class function TJclBorRADToolInstallation.RADToolKind: TJclBorRADToolKind;
begin
  if InheritsFrom(TJclBCBInstallation) then
    Result := brCppBuilder
  else
    Result := brDelphi;
end;

//--------------------------------------------------------------------------------------------------

class function TJclBorRADToolInstallation.RADToolName: string;
begin
  {$IFDEF MSWINDOWS}
  raise EAbstractError.CreateResFmt(@SAbstractError, ['']); // BCB doesn't support abstract keyword
  {$ENDIF MSWINDOWS}
end;

//--------------------------------------------------------------------------------------------------

procedure TJclBorRADToolInstallation.ReadInformation;
const
  {$IFDEF KYLIX}
  BinDir = 'bin/';
  {$ELSE}
  BinDir = 'Bin\';
  {$ENDIF}
  UpdateKeyName = 'Update #';
  IdeFileNames: array[TJclBorRADToolKind] of string = (DelphiIdeFileName, BCBIdeFileName);
var
  KeyLen, I: Integer;
  Key: string;
  Ed: TJclBorRADToolEdition;
begin
  Key := ConfigData.FileName;
  {$IFDEF KYLIX}
  ConfigData.ReadSectionValues(GlobalsKeyName, Globals);
  FRootDir := Globals.Values[RootDirValueName];
  {$ELSE KYLIX}
  RegGetValueNamesAndValues(HKEY_LOCAL_MACHINE, Key, Globals);
  FRootDir := RegReadStringDef(HKEY_LOCAL_MACHINE, ConfigData.FileName, RootDirValueName, '');

  KeyLen := Length(Key);
  if (KeyLen > 3) and StrIsDigit(Key[KeyLen - 2]) and (Key[KeyLen - 1] = '.') and (Key[KeyLen] = '0') then
    FVersionNumber := Ord(Key[KeyLen - 2]) - Ord('0')
  else
    FVersionNumber := 0;
  {$ENDIF KYLIX}

  FBinFolderName := PathAddSeparator(RootDir) + BinDir;
  FIdeExeFileName := PathAddSeparator(RootDir) + IdeFileNames[RADToolKind];

  Key := Globals.Values[VersionValueName];
  for Ed := Low(Ed) to High(Ed) do
    if BorRADToolEditionIDs[Ed] = Key then
      FEdition := Ed;

  for I := 0 to Globals.Count - 1 do
  begin
    Key := Globals.Names[I];
    KeyLen := Length(UpdateKeyName);
    if (Pos(UpdateKeyName, Key) = 1) and (Length(Key) > KeyLen) and StrIsDigit(Key[KeyLen + 1]) then
      FInstalledUpdatePack := Max(FInstalledUpdatePack, Integer(Ord(Key[KeyLen + 1]) - 48));
  end;

  for I := 1 to 3 do
  {$IFDEF KYLIX}
    if LatestUpdatePacks[I].Version = VersionNumber then
    begin
      FLatestUpdatePack := LatestUpdatePacks[I].LatestUpdatePack;
      Break;
    end;
  {$ENDIF KYLIX}
  {$IFDEF MSWINDOWS}
    if LatestUpdatePacks[RADToolKind, I].Version = VersionNumber then
    begin
      FLatestUpdatePack := LatestUpdatePacks[RADToolKind, I].LatestUpdatePack;
      Break;
    end;
  {$ENDIF MSWINDOWS}
end;

//--------------------------------------------------------------------------------------------------

procedure TJclBorRADToolInstallation.SetDebugDCUPath(const Value: string);
begin
  ConfigData.WriteString(DebuggingKeyName, DebugDCUPathValueName, Value);
end;

//--------------------------------------------------------------------------------------------------

procedure TJclBorRADToolInstallation.SetLibrarySearchPath(const Value: TJclBorRADToolPath);
begin
  ConfigData.WriteString(LibraryKeyName, LibrarySearchPathValueName, Value);
end;

//--------------------------------------------------------------------------------------------------

procedure TJclBorRADToolInstallation.SetLibraryBrowsingPath(const Value: TJclBorRADToolPath);
begin
  ConfigData.WriteString(LibraryKeyName, LibraryBrowsingPathValueName, Value);
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallation.SubstitutePath(const Path: string): string;
var
  I: Integer;
  Name: string;
begin
  Result := Path;
  if Pos('$(', Result) > 0 then
    with EnvironmentVariables do
      for I := 0 to Count - 1 do
      begin
        Name := Names[I];
        Result := StringReplace(Result, Format('$(%s)', [Name]), Values[Name], [rfReplaceAll, rfIgnoreCase]);
      end;
end;

function TJclBorRADToolInstallation.SupportsVisualCLX: Boolean;
begin
  {$IFDEF KYLIX}
  Result := True;
  {$ELSE}
  Result := (Edition <> deSTD) and (VersionNumber >= 6);
  {$ENDIF}
end;

//==================================================================================================
// TJclBCBInstallation
//==================================================================================================

{$IFDEF KYLIX}
function TJclBCBInstallation.ConfigFileName(const Extension: string): string;
begin
  Result := Format('%s/.borland/bcb%d%s', [GetPersonalFolder, IDs[VersionNumber], Extension]);
end;
{$ENDIF KYLIX}

//--------------------------------------------------------------------------------------------------

constructor TJclBCBInstallation.Create(const AConfigDataLocation: string);
begin
  inherited Create(AConfigDataLocation);
  FBpr2Mak := TJclBpr2Mak.Create(Self);
end;

//--------------------------------------------------------------------------------------------------

destructor TJclBCBInstallation.Destroy;
begin
  FBpr2Mak.Free;
  inherited Destroy;
end;

//--------------------------------------------------------------------------------------------------

function TJclBCBInstallation.InstallPackage(const PackageName, BPLPath, DCPPath: string): Boolean;
var
  SaveDir, PackagePath: string;
begin
  if IsDelphiPackage(PackageName) then
  begin
    Result := DCC.InstallPackage(PackageName, BPLPath, DCPPath);
    Exit;
  end;
  PackagePath := PathRemoveSeparator(ExtractFilePath(PackageName));
  SaveDir := GetCurrentDir;
  SetCurrentDir(PackagePath);
  try
    // Kylix bpr2mak doesn't like full file names
    Result := Bpr2Mak.Execute(StringsToStr(Bpr2Mak.Options, ' ') + ' ' + ExtractFileName(PackageName));
    Result := Result and Make.Execute(Format('%s -f%s', [StringsToStr(Make.Options, ' '), ChangeFileExt(PackageName, '.mak')]));
  finally
    SetCurrentDir(SaveDir);
  end;
end;

//--------------------------------------------------------------------------------------------------

class function TJclBCBInstallation.PackageSourceFileExtension: string;
begin
  Result := '.bpk';
end;

//--------------------------------------------------------------------------------------------------

class function TJclBCBInstallation.RADToolName: string;
begin
  Result := RsBCBName;
end;

//==================================================================================================
// TJclDelphiInstallation
//==================================================================================================

{$IFDEF KYLIX}
function TJclDelphiInstallation.ConfigFileName(const Extension: string): string;
begin
  Result := Format('%s/.borland/delphi%d%s', [GetPersonalFolder, IDs[VersionNumber], Extension]);
end;
{$ENDIF KYLIX}

//--------------------------------------------------------------------------------------------------

function TJclDelphiInstallation.InstallPackage(const PackageName, BPLPath, DCPPath: string): Boolean;
begin
  Result := DCC.InstallPackage(PackageName, BPLPath, DCPPath);
end;

//--------------------------------------------------------------------------------------------------

class function TJclDelphiInstallation.PackageSourceFileExtension: string;
begin
  Result := '.dpk';
end;

//--------------------------------------------------------------------------------------------------

class function TJclDelphiInstallation.RADToolName: string;
begin
  Result := RsDelphiName;
end;

//==================================================================================================
// TDelphiInstallations
//==================================================================================================

function TJclBorRADToolInstallations.AnyInstanceRunning: Boolean;
var
  I: Integer;
begin
  Result := False;
  for I := 0 to Count - 1 do
    if Installations[I].AnyInstanceRunning then
    begin
      Result := True;
      Break;
    end;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallations.AnyUpdatePackNeeded(var Text: string): Boolean;
var
  I: Integer;
begin
  Result := False;
  for I := 0 to Count - 1 do
    if Installations[I].UpdateNeeded then
    begin
      Result := True;
      Text := Format(RsNeedUpdate, [Installations[I].LatestUpdatePack, Installations[I].Name]);
      Break;
    end;
end;

//--------------------------------------------------------------------------------------------------

constructor TJclBorRADToolInstallations.Create;
begin
  FList := TObjectList.Create;
  ReadInstallations;
end;

//--------------------------------------------------------------------------------------------------

destructor TJclBorRADToolInstallations.Destroy;
begin
  FreeAndNil(FList);
  inherited;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallations.GetCount: Integer;
begin
  Result := FList.Count;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallations.GetBCBInstallationFromVersion(VersionNumber: Integer): TJclBorRADToolInstallation;
var
  I: Integer;
begin
  Result := nil;
  for I := 0 to Count - 1 do
    if Installations[I].VersionNumber = VersionNumber then
    begin
      Result := Installations[I];
      Break;
    end;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallations.GetDelphiInstallationFromVersion(VersionNumber: Integer): TJclBorRADToolInstallation;
var
  I: Integer;
begin
  Result := nil;
  for I := 0 to Count - 1 do
    if Installations[I].VersionNumber = VersionNumber then
    begin
      Result := Installations[I];
      Break;
    end;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallations.GetInstallations(Index: Integer): TJclBorRADToolInstallation;
begin
  Result := TJclBorRADToolInstallation(FList[Index]);
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallations.GetBCBVersionInstalled(VersionNumber: Integer): Boolean;
begin
  Result := BCBInstallationFromVersion[VersionNumber] <> nil;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallations.GetDelphiVersionInstalled(VersionNumber: Integer): Boolean;
begin
  Result := DelphiInstallationFromVersion[VersionNumber] <> nil;
end;

//--------------------------------------------------------------------------------------------------

function TJclBorRADToolInstallations.Iterate(TraverseMethod: TTraverseMethod): Boolean;
var
  I: Integer;
begin
  Result := True;
  for I := 0 to Count - 1 do
    Result := Result and TraverseMethod(Installations[I]);
end;

//--------------------------------------------------------------------------------------------------

procedure TJclBorRADToolInstallations.ReadInstallations;
{$IFDEF KYLIX}

  procedure CheckForInstallation(RADToolKind: TJclBorRADToolKind; VersionNumber: Integer);
  const
    RcBaseFileNames: array[TJclBorRADToolKind] of string = ('delphi', 'bcb');
  var
    Item: TJclBorRADToolInstallation;
    RcFileName: string;
  begin
    RcFileName := Format('%s/.borland/%s%drc', [GetPersonalFolder, RcBaseFileNames[RADToolKind], IDs[VersionNumber]]);
    if FileExists(RcFileName) then
    begin
      if RADToolKind = brCppBuilder then
        Item := TJclBCBInstallation.Create(RcFileName)
      else
        Item := TJclDelphiInstallation.Create(RcFileName);
      Item.FVersionNumber := VersionNumber;
      FList.Add(Item);
    end;
  end;

var
  I: Integer;
begin
  FList.Clear;
  for I := 1 to 3 do
    CheckForInstallation(brDelphi, I);
  CheckForInstallation(brCppBuilder, 3);
end;
{$ELSE KYLIX}
const
  KeyNames: array[TJclBorRADToolKind] of string = (DelphiKeyName, BCBKeyName);
var
  VersionNumbers: TStringList;

  procedure EnumVersions(RADToolKind: TJclBorRADToolKind);
  var
    I: Integer;
    Item: TJclBorRADToolInstallation;
    VersionKeyName: string;
  begin
    if RegKeyExists(HKEY_LOCAL_MACHINE, KeyNames[RADToolKind]) and
      RegGetKeyNames(HKEY_LOCAL_MACHINE, KeyNames[RADToolKind], VersionNumbers) then
      for I := 0 to VersionNumbers.Count - 1 do
      begin
        VersionKeyName := KeyNames[RADToolKind] + PathSeparator + VersionNumbers[I];
        if RegKeyExists(HKEY_LOCAL_MACHINE, VersionKeyName) then
        begin
          if RADToolKind = brCppBuilder then
            Item := TJclBCBInstallation.Create(VersionKeyName)
          else
            Item := TJclDelphiInstallation.Create(VersionKeyName);
          FList.Add(Item);
        end;
      end;
  end;

begin
  FList.Clear;
  VersionNumbers := TStringList.Create;
  try
    EnumVersions(brDelphi);
    EnumVersions(brCppBuilder);
  finally
    VersionNumbers.Free;
  end;
end;
{$ENDIF KYLIX}

//==================================================================================================
// TJclCommandLineTool
//==================================================================================================

constructor TJclCommandLineTool.Create(const AExeName: string);
begin
  inherited Create;
  FOptions := TStringList.Create;
  FExeName := AExeName;
end;

//--------------------------------------------------------------------------------------------------

destructor TJclCommandLineTool.Destroy;
begin
  FreeAndNil(FOptions);
  inherited Destroy;
end;

//--------------------------------------------------------------------------------------------------

procedure TJclCommandLineTool.AddPathOption(const Option, Path: string);
begin
  GetOptions.Add(Format('-%s"%s"', [Option, PathRemoveSeparator(Path)]));
end;

//--------------------------------------------------------------------------------------------------

function TJclCommandLineTool.Execute(const CommandLine: string): Boolean;
const
  {$IFDEF MSWINDOWS}
  S = '"%s" %s';
  {$ENDIF MSWINDOWS}
  {$IFDEF UNIX}
  S = '%s %s';
  {$ENDIF UNIX}
begin
  Result := ExecAndRedirectOutput(Format(S, [ExeName, CommandLine]), FOutput) = 0;
end;

//--------------------------------------------------------------------------------------------------

function TJclCommandLineTool.GetExeName: string;
begin
  Result := FExeName;
end;

//--------------------------------------------------------------------------------------------------

function TJclCommandLineTool.GetOptions: TStrings;
begin
  Result := FOptions;
end;

//--------------------------------------------------------------------------------------------------

function TJclCommandLineTool.GetOutput: string;
begin
  Result := FOutput;
end;

//--------------------------------------------------------------------------------------------------

end.
