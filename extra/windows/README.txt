1 - Make install on Mingw
2 - Change ginga and gingagui icons using (Remember to be
running terminal with admin privileges):
extra/windows/rcedit-x64.exe /mingw64/bin/ginga.exe --set-icon extra/windows/icon.ico
extra/windows/rcedit-x64.exe /mingw64/bin/gingagui.exe --set-icon extra/windows/icon.ico

1 - On Inno Setup select ginga_win_deploy_script.iss.
2 - Press the green play arrow to start building.
3 - At the end of the build process the program setup is automatically
started.

Note 1: The output file is located on the same directory as the script is,
but a directory named Output in created.
Note 2: If your Windows drive is not C:\ the script must be modified
accordingly.
