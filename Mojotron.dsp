# Microsoft Developer Studio Project File - Name="Mojotron" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
#
# ** DO NOT EDIT **
# I JUST DID  - CRAIG
# (needed to add a source file, don't have VC to change this with)
# 

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Mojotron - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Mojotron.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Mojotron.mak" CFG="Mojotron - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Mojotron - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Mojotron - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Mojotron - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x414 /d "NDEBUG"
# ADD RSC /l 0x414 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 clanCore.lib clanApp.lib clanDisplay.lib clanSound.lib clanMikmod.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Mojotron - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x414 /d "_DEBUG"
# ADD RSC /l 0x414 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 clanCored.lib clanAppd.lib clanDisplayd.lib clanSoundd.lib clanMikmodd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Mojotron - Win32 Release"
# Name "Mojotron - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Barricade.cpp
# End Source File
# Begin Source File

SOURCE=.\Bonus.cpp
# End Source File
# Begin Source File

SOURCE=.\Bullet.cpp
# End Source File
# Begin Source File

SOURCE=.\Explosion.cpp
# End Source File
# Begin Source File

SOURCE=.\Globals.cpp
# End Source File
# Begin Source File

SOURCE=.\Hazards.cpp
# End Source File
# Begin Source File

SOURCE=.\InputState.cpp
# End Source File
# Begin Source File

SOURCE=.\Intro.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\Menu.cpp
# End Source File
# Begin Source File

SOURCE=.\MiscDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectList.cpp
# End Source File
# Begin Source File

SOURCE=.\Player.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerStats.cpp
# End Source File
# Begin Source File

SOURCE=.\ScoreBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\SkillLevel.cpp
# End Source File
# Begin Source File

SOURCE=.\Sprite.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound.cpp
# End Source File
# Begin Source File

SOURCE=.\Thing.cpp
# End Source File
# Begin Source File

SOURCE=.\World.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Barricade.hh
# End Source File
# Begin Source File

SOURCE=.\Bonus.hh
# End Source File
# Begin Source File

SOURCE=.\Bullet.hh
# End Source File
# Begin Source File

SOURCE=.\Explosion.hh
# End Source File
# Begin Source File

SOURCE=.\Globals.hh
# End Source File
# Begin Source File

SOURCE=.\Hazards.hh
# End Source File
# Begin Source File

SOURCE=.\InputState.hh
# End Source File
# Begin Source File

SOURCE=.\Intro.hh
# End Source File
# Begin Source File

SOURCE=.\Menu.hh
# End Source File
# Begin Source File

SOURCE=.\MiscDisplay.hh
# End Source File
# Begin Source File

SOURCE=.\ObjectList.hh
# End Source File
# Begin Source File

SOURCE=.\Player.hh
# End Source File
# Begin Source File

SOURCE=.\PlayerStats.hh
# End Source File
# Begin Source File

SOURCE=.\ScoreBoard.hh
# End Source File
# Begin Source File

SOURCE=.\SkillLevel.hh
# End Source File
# Begin Source File

SOURCE=.\Sprite.hh
# End Source File
# Begin Source File

SOURCE=.\Sound.hh
# End Source File
# Begin Source File

SOURCE=.\Thing.hh
# End Source File
# Begin Source File

SOURCE=.\World.hh
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
SOURCE=.\Thing.hh
# End Source File
# Begin Source File

SOURCE=.\World.hh
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
