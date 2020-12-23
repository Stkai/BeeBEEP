; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{85A0D6B6-2163-425F-92E7-E0BC4CEDCE9A}
AppName=BeeBEEP
AppVersion=5.8.4
AppVerName=BeeBEEP 5.8.4
AppPublisher=Marco Mastroddi Software
AppPublisherURL=https://www.beebeep.net/
AppSupportURL=https://www.beebeep.net/
AppUpdatesURL=https://www.beebeep.net/
DefaultDirName={pf}\BeeBEEP
DisableProgramGroupPage=yes
OutputBaseFilename=beebeep-setup-5.8.4
SetupIconFile=..\src\beebeep.ico
Compression=lzma
SolidCompression=yes
AppCopyright=Copyright (C) 2021 Marco Mastroddi
OutputDir=.
LicenseFile=..\LICENSE.txt

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "dutch"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"
Name: "italian"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "norwegian"; MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "polish"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "..\..\beebeep-5.8.4\beebeep.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\beebeep-5.8.4\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{commonprograms}\BeeBEEP"; Filename: "{app}\beebeep.exe"
Name: "{commondesktop}\BeeBEEP"; Filename: "{app}\beebeep.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\BeeBEEP"; Filename: "{app}\beebeep.exe"; Tasks: quicklaunchicon

[Run]
Filename: "{sys}\netsh.exe"; Parameters: "advfirewall firewall add rule name=""BeeBEEP"" program=""{app}\beebeep.exe"" dir=in action=allow enable=yes"; Flags: runhidden;
Filename: "{sys}\netsh.exe"; Parameters: "advfirewall firewall add rule name=""BeeBEEP NET"" program=""{app}\beebeep.exe"" dir=out action=allow enable=yes"; Flags: runhidden;
Filename: "{app}\beebeep.exe"; Description: "{cm:LaunchProgram,BeeBEEP}"; Flags: nowait postinstall skipifsilent

[UninstallRun]
Filename: "{sys}\netsh.exe"; Parameters: "advfirewall firewall delete rule name=all program=""{app}\beebeep.exe"""; Flags: runhidden;
