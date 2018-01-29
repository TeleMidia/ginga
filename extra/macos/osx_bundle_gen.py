'''
Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.
''' 

import subprocess
import pathlib
import glob

DYLIBS_COUNT = 0
processed_dylibs = []

def process_dylib(line_index, otool_line, binary_path):
    global DYLIBS_COUNT
    if otool_line == "" :
        return    

    dylib_path = otool_line.split(' ')[0].replace('\t', '')
    dylib_name = dylib_path.split('/')[-1].replace('\t', '')

    if not dylib_path[:5] == "/usr/":
        return 

    if not dylib_name.endswith('.dylib'):
        #print("ERRO: ", dylib_path, "is system reserved! Next!")
        return
    
    if not pathlib.Path('Ginga.app/Contents/Frameworks/'+dylib_name).is_file():
        subprocess.call('cp '+dylib_path+' Ginga.app/Contents/Frameworks/', shell=True)
        print(DYLIBS_COUNT, dylib_path)
        DYLIBS_COUNT = DYLIBS_COUNT + 1

    subprocess.call('install_name_tool -change '+dylib_path+' @executable_path/../Frameworks/'+dylib_name+' '+binary_path, shell=True)

def process_binary(binary_path):
    binary_dependencies = subprocess.Popen('otool -L '+binary_path, shell=True, stdout=subprocess.PIPE ).communicate()[0].decode("utf-8")
    binary_dependencies = binary_dependencies.split('\n')

    for index,dependency in enumerate(binary_dependencies):
        process_dylib(index, dependency, binary_path)  

subprocess.call('mkdir -p Ginga.app/Contents/{MacOS,Resources,Frameworks,Resources/share}', shell=True)
subprocess.call('cp ../../src-gui/.libs/gingagui Ginga.app/Contents/MacOS/Ginga', shell=True)
subprocess.call('cp Icon.icns Ginga.app/Contents/Resources/', shell=True)
subprocess.call('cp Info.plist Ginga.app/Contents/', shell=True)
subprocess.call('cp -r /usr/local/share/ginga* Ginga.app/Contents/Resources/share', shell=True)
subprocess.call('cp -r /usr/local/share/glib-2.0* Ginga.app/Contents/Resources/share', shell=True)
subprocess.call('cp -r /usr/local/share/icons* Ginga.app/Contents/Resources/share', shell=True)

#process main binary
process_binary('Ginga.app/Contents/MacOS/Ginga')

#process dylibs dependences
while(True):
    need_iterate = False
    dylibs_list = glob.glob("Ginga.app/Contents/Frameworks/*.dylib")
    for dylib in dylibs_list:
        if dylib not in processed_dylibs:
            processed_dylibs.append(dylib)
            #input("WILL PROCESS: "+dylib)
            process_binary(dylib)
            need_iterate = True
            break
    if not need_iterate:
        break
  
   

