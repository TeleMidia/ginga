#define MyAppName "Ginga"
#define MyAppVersion "1.0"
#define MyAppPublisher "TeleMidia"
#define MyAppURL "github.com/TeleMidia/ginga"
#define MyAppExeName "gingagui.exe"
#define MyAppIcoName "icon.ico"

[Registry]
Root: HKCR; Subkey: ".ncl"; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKCR; Subkey: "{#MyAppName}"; ValueData: "Program {#MyAppName}";  Flags: uninsdeletekey; ValueType: string;  ValueName: ""
Root: HKCR; Subkey: "{#MyAppName}\DefaultIcon"; ValueData: "{app}\ginga\{#MyAppExeName},0"; ValueType: string;  ValueName: ""
Root: HKCR; Subkey: "{#MyAppName}\shell\open\command"; ValueData: """{app}\ginga\{#MyAppExeName}"" ""%1"""; ValueType: string; ValueName: ""

[Icons]
Name: "{userdesktop}\{#MyAppName}"; Filename: "{app}\ginga\{#MyAppExeName}"; IconFilename: "{app}\{#MyAppIcoName}"; Tasks: desktopicon
Name: "{userstartmenu}\{#MyAppName}"; Filename: "{app}\ginga\{#MyAppExeName}"; IconFilename: "{app}\{#MyAppIcoName}"

[Setup] 
AppId={{FD94E7C2-B90C-4869-B6C2-FB5DD69279B5}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={commonpf}\{#MyAppName}
DisableDirPage=no
DisableProgramGroupPage=yes
OutputDir=.\
OutputBaseFilename="ginga-v{#MyAppVersion}-win32-installer"
SetupIconFile={#SourcePath}\{#MyAppIcoName}
UninstallDisplayIcon={#SourcePath}\{#MyAppIcoName}
Compression=lzma2
; Compression=none
ChangesEnvironment=no
ChangesAssociations=yes
ArchitecturesInstallIn64BitMode=x64

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: {#SourcePath}\{#MyAppIcoName}; DestDir: "{app}\";
Source: "ginga\*"; DestDir: "{app}\ginga\"; Flags: recursesubdirs createallsubdirs
Source: "lib\*"; DestDir: "{app}\lib\"; Flags: recursesubdirs createallsubdirs
Source: "share\*"; DestDir: "{app}\share\"; Flags: recursesubdirs createallsubdirs