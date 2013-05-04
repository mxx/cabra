# Microsoft Developer Studio Project File - Name="Analyzer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Analyzer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Analyzer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Analyzer.mak" CFG="Analyzer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Analyzer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Analyzer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Analyzer - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 setupapi.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Analyzer - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "./" /I "../VTDRRecord" /I "../" /I "../sqlite3/" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 setupapi.lib Ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Analyzer - Win32 Release"
# Name "Analyzer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Analyzer.cpp
# End Source File
# Begin Source File

SOURCE=.\Analyzer.rc
# End Source File
# Begin Source File

SOURCE=.\Config.cpp
# End Source File
# Begin Source File

SOURCE=.\DataCollectionDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\DataStore\DataStore.cpp
# End Source File
# Begin Source File

SOURCE=..\DataStore\DataStore.h
# End Source File
# Begin Source File

SOURCE=.\DlgSetInstDate.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSetOnPara.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSetStateName.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSetVInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\EnumSerial.cpp
# End Source File
# Begin Source File

SOURCE=.\ErrorMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\FileSettingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Login.cpp
# End Source File
# Begin Source File

SOURCE=.\MainDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\protocol\Packet.cpp
# End Source File
# Begin Source File

SOURCE=..\protocol\Protocol.cpp
# End Source File
# Begin Source File

SOURCE=.\SeleteDate.cpp
# End Source File
# Begin Source File

SOURCE=.\SerialPort.cpp
# End Source File
# Begin Source File

SOURCE=..\sqlite3\shell.c
# End Source File
# Begin Source File

SOURCE=.\SpectrumDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\sqlite3\sqlite3.c
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# End Source File
# Begin Source File

SOURCE=.\TableData.cpp
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\USBDataFilev2012.cpp
# End Source File
# Begin Source File

SOURCE=.\USBFileLoading.cpp
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRDetailRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRDriverIDRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRDriverInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDROderMeter.cpp
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDROutPoweredRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDROvertimeDriveRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRParameterModifyRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRPositionRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRPulseModulus.cpp
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRRealTime.cpp
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRSpeedRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRSpeedStatusLog.cpp
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRUniqCode.cpp
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRVehicleConfigure.cpp
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRVehicleInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRVersion.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Analyzer.h
# End Source File
# Begin Source File

SOURCE=.\Config.h
# End Source File
# Begin Source File

SOURCE=.\DataCollectionDlg.h
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\define_gbk.h
# End Source File
# Begin Source File

SOURCE=.\DlgSetInstDate.h
# End Source File
# Begin Source File

SOURCE=.\DlgSetOnPara.h
# End Source File
# Begin Source File

SOURCE=.\DlgSetStateName.h
# End Source File
# Begin Source File

SOURCE=.\DlgSetVInfo.h
# End Source File
# Begin Source File

SOURCE=.\EnumSerial.h
# End Source File
# Begin Source File

SOURCE=.\ErrorMsg.h
# End Source File
# Begin Source File

SOURCE=.\FileSettingDlg.h
# End Source File
# Begin Source File

SOURCE=.\Login.h
# End Source File
# Begin Source File

SOURCE=.\MainDlg.h
# End Source File
# Begin Source File

SOURCE=..\protocol\Packet.h
# End Source File
# Begin Source File

SOURCE=..\protocol\Protocol.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\protocol\RS232Port.h
# End Source File
# Begin Source File

SOURCE=.\SeleteDate.h
# End Source File
# Begin Source File

SOURCE=.\SerialPort.h
# End Source File
# Begin Source File

SOURCE=.\SpectrumDlg.h
# End Source File
# Begin Source File

SOURCE=..\sqlite3\sqlite3.h
# End Source File
# Begin Source File

SOURCE=..\sqlite3\sqlite3ext.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TableData.h
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\USBDataFilev2012.h
# End Source File
# Begin Source File

SOURCE=.\USBFileLoading.h
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRDetailRecord.h
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRDriverIDRecord.h
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRDriverInfo.h
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDROderMeter.h
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDROutPoweredRecord.h
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDROvertimeDriveRecord.h
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRParameterModifyRecord.h
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRPositionRecord.h
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRPulseModulus.h
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRRealTime.h
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRRecord.h
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRSpeedRecord.h
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRSpeedStatusLog.h
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRUniqCode.h
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRVehicleConfigure.h
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRVehicleInfo.h
# End Source File
# Begin Source File

SOURCE=..\VTDRRecord\VTDRVersion.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Analyzer.ico
# End Source File
# Begin Source File

SOURCE=.\res\Analyzer.rc2
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\LSC.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
