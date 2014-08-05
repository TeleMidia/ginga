::This is a batch script that fetches the deps
::from telemidiatc1 for build Ginga on Windows.
@echo off

set gingaVer=0.13.6
set contribVer=2
set archive=contrib_v%gingaVer%.%contribVer%.zip

set remoteHost=\\telemidiatc1
set remotePath=TeleMidia\deps\ginga\windows

echo Copying %remoteHost%\%remotePath%\%archive% to current directory.
copy /Z /-Y %remoteHost%\%remotePath%\%archive% .

pause