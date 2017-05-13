# Ginga

The iTV middleware.

* http://www.ginga.org.br
* http://www.ncl.org.br
* http://www.telemidia.puc-rio.br


## Dependencies

Required:

* glib >= 2.32
* SDL2 >= 2.0
* SDL2_image >= 2.0
* gstreamer >= 1.8
* xercesc >= 2.7
* nclua >= 1.0, http://github.com/gflima/nclua

Optional:

* cairo >= 1.10
* cef >= 3.0, https://bitbucket.org/chromiumembedded/cef
* librsvg >= 2.40
* pango >= 1.30

## Building

We currently support both autotools and cmake build systems.

### autotools

    $ ./bootstrap
    $ ./configure
    $ make

To build with cef (chromium embedded framework) support change the above
configure step above to:

    $ ./configure --with-cef=${CEF_ROOT}

### cmake

    $ mkdir _build && cd _build
    $ cmake ../build-cmake
    $ make

When using cmake, nclua and chromium embedded will be automatically downloaded
and built.
    
---
Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

Permission is granted to copy, distribute and/or modify this document under the
terms of the GNU Free Documentation License, Version 1.3 or any later version
published by the Free Software Foundation; with no Invariant Sections, with no
Front-Cover Texts, and with no Back-Cover Texts. A copy of the license is
included in the "GNU Free Documentation License" file as part of this
distribution.
