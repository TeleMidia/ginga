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

dylibs_list = []

def get_dylib_path(ori_path):
    ori_path = ori_path.split("/")
    new_path = ""
    for i, folder in enumerate(ori_path):
        if i == 0 or i == (len(ori_path)-1):
            continue
        new_path+="/"+folder
    return new_path

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
    
    cur_path = get_dylib_path(dylib_path)

    if '/usr/lib' == cur_path:
        if not pathlib.Path('Ginga.app/Contents/Frameworks/'+dylib_name).is_file():
            print(DYLIBS_COUNT, dylib_path, "->" , 'Ginga.app/Contents/Frameworks/'+dylib_name)
            subprocess.call('cp '+dylib_path+' Ginga.app/Contents/Frameworks/', shell=True)
            dylibs_list.append('Ginga.app/Contents/Frameworks/'+dylib_name)
            subprocess.call('install_name_tool -change '+dylib_path+' @executable_path/../Frameworks/'+dylib_name+' '+binary_path, shell=True)
        return

    if not pathlib.Path('Ginga.app/Contents/Frameworks'+cur_path+'/'+dylib_name).is_file():
        subprocess.call('mkdir -p Ginga.app/Contents/Frameworks'+cur_path+'/', shell=True)
        subprocess.call('cp '+dylib_path+' Ginga.app/Contents/Frameworks'+cur_path+'/', shell=True)
        dylibs_list.append('Ginga.app/Contents/Frameworks'+cur_path+'/'+dylib_name)
        print(DYLIBS_COUNT, dylib_path, "->" , 'Ginga.app/Contents/Frameworks'+cur_path+'/'+dylib_name)
        #print('install_name_tool -change '+dylib_path+' @executable_path/../Frameworks/'+dylib_name+' '+binary_path)
        DYLIBS_COUNT = DYLIBS_COUNT + 1

    #subprocess.call('install_name_tool -change '+dylib_path+' @executable_path/../Frameworks/'+dylib_name+' '+binary_path, shell=True)

def process_binary(binary_path):
    binary_dependencies = subprocess.Popen('otool -L '+binary_path, shell=True, stdout=subprocess.PIPE ).communicate()[0].decode("utf-8")
    binary_dependencies = binary_dependencies.split('\n')

    for index,dependency in enumerate(binary_dependencies):
        if "/usr/lib/system/" not in binary_dependencies[index]: 
            process_dylib(index, dependency, binary_path)  

subprocess.call('mkdir -p Ginga.app/Contents/{MacOS,Resources,Frameworks,Frameworks/usr,Frameworks/usr/local,Frameworks/usr/local/share,Frameworks/usr/local/lib,Frameworks/usr/local/Cellar,Frameworks/usr/local/opt}', shell=True)
subprocess.call('cp ../../src-gui/gingagui Ginga.app/Contents/MacOS/Ginga', shell=True)
subprocess.call('cp Icon.icns Ginga.app/Contents/Resources/', shell=True)
subprocess.call('cp Info.plist Ginga.app/Contents/', shell=True)
subprocess.call('cp -r /usr/local/share/ginga* Ginga.app/Contents/Frameworks/usr/local/share', shell=True)
subprocess.call('cp -r /usr/local/share/glib-2.0* Ginga.app/Contents/Frameworks/usr/local/share', shell=True)
subprocess.call('cp -r /usr/local/share/icons* Ginga.app/Contents/Frameworks/usr/local/share', shell=True)
subprocess.call('cp -r /usr/local/share/lua* Ginga.app/Contents/Frameworks/usr/local/share', shell=True)
subprocess.call('cp -r /usr/local/lib/lua* Ginga.app/Contents/Frameworks/usr/local/lib', shell=True)
subprocess.call('cp -r /usr/local/lib/gdk-pixbuf-2.0* Ginga.app/Contents/Frameworks/usr/local/lib', shell=True)
subprocess.call('cp -r /usr/local/Cellar/gdk-pixbuf* Ginga.app/Contents/Frameworks/usr/local/Cellar', shell=True)
subprocess.call('cp -r /usr/local/Cellar/gst-plugins-ugly* Ginga.app/Contents/Frameworks/usr/local/Cellar', shell=True)
subprocess.call('cp -r /usr/local/Cellar/gst-plugins-bad* Ginga.app/Contents/Frameworks/usr/local/Cellar', shell=True)
subprocess.call('cp -r /usr/local/Cellar/gst-plugins-good* Ginga.app/Contents/Frameworks/usr/local/Cellar', shell=True)
subprocess.call('cp -r /usr/local/Cellar/gst-plugins-base* Ginga.app/Contents/Frameworks/usr/local/Cellar', shell=True)
subprocess.call('cp -r /usr/local/Cellar/gstreamer* Ginga.app/Contents/Frameworks/usr/local/Cellar', shell=True)
subprocess.call('cp -r /usr/local/opt/gst-plugins-ugly* Ginga.app/Contents/Frameworks/usr/local/opt', shell=True)
subprocess.call('cp -r /usr/local/opt/gst-plugins-bad* Ginga.app/Contents/Frameworks/usr/local/opt', shell=True)
subprocess.call('cp -r /usr/local/opt/gst-plugins-good* Ginga.app/Contents/Frameworks/usr/local/opt', shell=True)
subprocess.call('cp -r /usr/local/opt/gst-plugins-base* Ginga.app/Contents/Frameworks/usr/local/opt', shell=True)



#process main binary
process_binary('Ginga.app/Contents/MacOS/Ginga')

#process dylibs dependences
while(True):
    need_iterate = False
    for dylib in dylibs_list:
        if dylib not in processed_dylibs:
            processed_dylibs.append(dylib)
            #input("WILL PROCESS: "+dylib)
            process_binary(dylib)
            need_iterate = True
            break
    if not need_iterate:
        break
  
   

