/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef SDLWINDOW_H_
#define SDLWINDOW_H_

#include "util/Color.h"
using namespace ::br::pucrio::telemidia::util;

#include "SDL.h"

#include "SDLSurface.h"

typedef uint32_t Uint32;
typedef Uint32 SDL_WindowID;

#include <iostream>
#include <vector>
using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_CORE_MB_BEGIN

  class SDLWindow {
  public:
    static const short DDT_LINE = 0;
    static const short DDT_RECT = 1;
    static const short DDT_FILL_RECT = 2;

  private:
    SDL_Texture* texture;
    SDL_Surface* curSur;

    GingaSurfaceID winISur;

    bool textureUpdate;
    bool textureOwner;

    int borderWidth;
    Color* bgColor;
    Color* borderColor;
    Color* winColor;
    Color* colorKey;

    GingaScreenID myScreen;
    GingaWindowID windowId;

    SDL_Rect rect;

    float z;
    int transparencyValue;
    bool visible;
    bool ghost;

    SDLSurface* childSurface;
    bool fit;
    bool stretch;
    int caps;

    set<SDLWindow*> mirrors;
    SDLWindow* mirrorSrc;

    pthread_mutex_t mutex;    //external mutex
    pthread_mutex_t mutexC;   //childs mutex
    pthread_mutex_t texMutex; //texture mutex
    pthread_mutex_t surMutex; //underlying surface mutex

    bool isWaiting;
    pthread_mutex_t rMutex; //render mutex
    pthread_mutex_t cMutex; //condition mutex
    pthread_cond_t cond;

  public:
    SDLWindow(
              GingaWindowID underlyingWindowID,
              GingaWindowID parentWindowID,
              GingaScreenID screenId,
              int x, int y, int width, int height,
              float z);

    virtual ~SDLWindow();

  private:
    void initialize(
                    GingaWindowID underlyingWindowID,
                    GingaWindowID parentWindowID,
                    GingaScreenID screenId,
                    int x, int y, int width, int height,
                    float z);

    void releaseWinISur();
    void releaseBGColor();
    void releaseWinColor();
    void releaseColorKey();
    void releaseBorderColor();

  public:
    void addMirror(SDLWindow* window);
    bool removeMirror(SDLWindow* window);
    void setMirrorSrc(SDLWindow* mirrorSrc);
    SDLWindow* getMirrorSrc();

    void setBgColor(int r, int g, int b, int alpha);
    Color* getBgColor();
    void setColorKey(int r, int g, int b);
    Color* getColorKey();
    void setWindowColor(int r, int g, int b, int alpha);
    Color* getWindowColor();
    void setBorder(int r, int g, int b, int alpha=255, int bWidth=0);
    void getBorder(int* r, int* g, int* b, int* alpha, int* bWidth);

    GingaScreenID getScreen();
    void revertContent();
    void setChildSurface(SDLSurface* iSur);
    int getCap(string cap);
    void setCaps(int caps);
    void addCaps(int capability);
    int getCaps();

    void draw();
    void setBounds(int x, int y, int width, int height);
    void moveTo(int x, int y);
    void resize(int width, int height);
    void raiseToTop();
    void lowerToBottom();
    void setCurrentTransparency(int alpha);
    int getTransparencyValue();
    GingaWindowID getId();
    void show();
    void hide();

    int getX();
    int getY();
    int getW();
    int getH();
    float getZ();

    void setX(int x);
    void setY(int y);
    void setW(int w);
    void setH(int h);
    void setZ(float z);

    bool isGhostWindow();
    void setGhostWindow(bool ghost);
    bool isVisible();
    void validate();

  private:
    void unprotectedValidate();

  public:
    vector<DrawData*>* createDrawDataList();
    void setStretch(bool stretchTo);
    bool getStretch();
    void setFit(bool fitTo);
    bool getFit();
    void clearContent();
    void setRenderedSurface(SDL_Surface* uSur);
    void* getContent();
    void setTexture(SDL_Texture* texture);
    SDL_Texture* getTexture(SDL_Renderer* renderer);
    bool isTextureOwner(SDL_Texture* texture);

  private:
    bool isMine(SDLSurface* surface);

  public:
    void renderImgFile(string serializedImageUrl);
    void renderFrom(SDLSurface* s);

    void blit(SDLWindow* src);
    void stretchBlit(SDLWindow* src);
    string getDumpFileUri(int quality, int dumpW, int dumpH);

    void lock();
    void unlock();

    void lockChilds();
    void unlockChilds();

    bool rendered();

  private:
    void waitRenderer();

    void lockTexture();
    void unlockTexture();

    void lockSurface();
    void unlockSurface();
  };

BR_PUCRIO_TELEMIDIA_GINGA_CORE_MB_END
#endif /*SDLWINDOW_H_*/
