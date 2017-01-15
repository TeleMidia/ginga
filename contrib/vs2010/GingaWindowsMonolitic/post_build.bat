:: This script is part of ginga-vs2010-solution
:: It will copy all necessary configuration file to the directory
:: passed as parameter, e.g., where the ginga was built.

@echo off

set GINGABINPATH=%~1%
set GINGAFILESPATH=%GINGABINPATH%\files
set GINGA_BASE_DIR=%~2%
set GINGACCCONTEXTMANAGER_DIR=%~3%
set GINGACCMB_DIR=%~4%
set GINGACCPLAYER_DIR=%~5%
set CONTRIBLIB_DIR=%~6%
set GINGANCL_DIR=%~7%

set CP_COMMAND=xcopy /Y

echo %CD%

echo ::Copying ginga config files to %GINGABINPATH%

if not exist "%GINGABINPATH%" ( mkdir "%GINGABINPATH%" )
if not exist "%GINGAFILESPATH%" ( mkdir "%GINGAFILESPATH%" )
if not exist "%GINGAFILESPATH%\cfg" ( mkdir "%GINGAFILESPATH%\cfg" )
if not exist "%GINGAFILESPATH%\font" ( mkdir "%GINGAFILESPATH%\font" )
if not exist "%GINGAFILESPATH%\img" ( mkdir "%GINGAFILESPATH%\img" )
if not exist "%GINGAFILESPATH%\md" ( mkdir "%GINGAFILESPATH%\md" )

:: GingaCC-tuner
%CP_COMMAND% %GINGA_BASE_DIR%gingacc-tuner\files\img\tuning.png %GINGAFILESPATH%\tuner\
%CP_COMMAND% %GINGA_BASE_DIR%gingacc-tuner\files\network\tuner.ini %GINGAFILESPATH%\tuner\

:: GingaCC-CM
%CP_COMMAND% %GINGA_BASE_DIR%gingacc-cm\files\*.xml %GINGAFILESPATH%\

:: GingaCC-ContextManager
if not exist "%GINGAFILESPATH%\contextmanager" ( mkdir "%GINGAFILESPATH%\contextmanager" )
%CP_COMMAND% %GINGACCCONTEXTMANAGER_DIR%\files\*.ini %GINGAFILESPATH%\contextmanager\
%CP_COMMAND% %GINGACCCONTEXTMANAGER_DIR%\files\*.txt %GINGAFILESPATH%\contextmanager\

:: GingaCC-MB
if not exist "%GINGAFILESPATH%\mb" ( mkdir "%GINGAFILESPATH%\mb" )
if not exist "%GINGAFILESPATH%\mb\config" ( mkdir "%GINGAFILESPATH%\mb\config" )
%CP_COMMAND% %GINGACCMB_DIR%\files\config %GINGAFILESPATH%\mb\config

:: GingaCC-player
if not exist "%GINGAFILESPATH%\font" ( mkdir "%GINGAFILESPATH%\font" )
if not exist "%GINGAFILESPATH%\img" ( mkdir "%GINGAFILESPATH%\img" )
if not exist "%GINGAFILESPATH%\img\button" ( mkdir "%GINGAFILESPATH%\img\button" )
%CP_COMMAND% %GINGACCPLAYER_DIR%\files\fonts %GINGAFILESPATH%\font
%CP_COMMAND% %GINGACCPLAYER_DIR%\files\images\*.png %GINGAFILESPATH%\img\button

:: nclua
if not exist "%GINGABINPATH%\nclua" ( mkdir "%GINGABINPATH%\nclua" )
if not exist "%GINGABINPATH%\nclua\event" ( mkdir "%GINGABINPATH%\nclua\event" )
%CP_COMMAND% %CONTRIBLIB_DIR%\nclua\nclua\share\lua\5.2\nclua\event\*.lua %GINGABINPATH%\nclua\event

:: Ginga-NCL
if not exist "%GINGAFILESPATH%\cfg" ( mkdir "%GINGAFILESPATH%\cfg" )
if not exist "%GINGAFILESPATH%\cfg\formatter" ( mkdir "%GINGAFILESPATH%\cfg\formatter" )
if not exist "%GINGAFILESPATH%\img" ( mkdir "%GINGAFILESPATH%\img" )
if not exist "%GINGAFILESPATH%\img\epgFactory" ( mkdir "%GINGAFILESPATH%\img\epgfactory" )
if not exist "%GINGAFILESPATH%\img\roller" ( mkdir "%GINGAFILESPATH%\img\roller" )
if not exist "%GINGAFILESPATH%\md" ( mkdir "%GINGAFILESPATH%\md" )
%CP_COMMAND% %GINGANCL_DIR%\files\config\formatter\players\*.ini %GINGAFILESPATH%\cfg\formatter
%CP_COMMAND% %GINGANCL_DIR%\files\epgfactory\img\*.png %GINGAFILESPATH%\img\epgFactory
%CP_COMMAND% %GINGANCL_DIR%\files\img\*.png %GINGAFILESPATH%\img\roller
%CP_COMMAND% %GINGANCL_DIR%\files\img\active-device-reset.png %GINGAFILESPATH%\md
%CP_COMMAND% %GINGANCL_DIR%\files\img\active-device.png %GINGAFILESPATH%\md

:: Ginga
%CP_COMMAND% %GINGA_BASE_DIR%\config\mimetypes.ini %GINGAFILESPATH%\
%CP_COMMAND% /I %GINGA_BASE_DIR%\config\ginga.win.ini %GINGAFILESPATH%\ginga.ini*

echo Done!
