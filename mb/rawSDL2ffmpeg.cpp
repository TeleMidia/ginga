/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

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

#include "config.h"

#include <vld.h>

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "SDL2ffmpeg.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

extern "C" {
	#include "SDL.h"
}

#ifdef main
#undef main
#endif

#include <string>
#include <iostream>
using namespace std;

int main(int argc, char** argv) {
	string videoUri = "";

	//SETTING LOG INFO TO A FILE
	SystemCompat::setLogTo(SystemCompat::LOG_FILE);

	/* PROCESSING MAIN PARAMETERS*/
	for (int i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "--enable-log") == 0) && ((i + 1) < argc)) {
			if (strcmp(argv[i + 1], "stdout") == 0) {
				SystemCompat::setLogTo(SystemCompat::LOG_STDO);

			} else if (strcmp(argv[i + 1], "file") == 0) {
				SystemCompat::setLogTo(SystemCompat::LOG_FILE);
			}

		} else if ((strcmp(argv[i], "--src") == 0) && ((i + 1) < argc)) {
			videoUri.assign(argv[i + 1]);
		}
	}

	if (videoUri == "") {
		cout << "Please specify the video URI with --src parameter" << endl;
		exit(0);
	}

	/* SDL stuffs */
	SDL_Window *win = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *texture = NULL;
	int posX = 100, posY = 100, width = 320, height = 240;

	/* creating window */
	win = SDL_CreateWindow("SDL2 Window to SDL2ffmpeg", posX, posY, width, height, 0);

	/* creating renderer */
	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

	/* creating texture */
	texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGB24,
		SDL_TEXTUREACCESS_STREAMING,
		width, height);

	/* allowing alpha */
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	/* SDL2ffmpeg stuffs */
	double rt;
	SDL2ffmpeg decoder(videoUri.c_str());
	decoder.setTexture(texture);

	SystemCompat::initMemCheck();
	decoder.play();

	while (1) {
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}
		}

		SDL2ffmpeg::video_refresh(&decoder, &rt);

		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	SystemCompat::finishMemCheck();

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);

	cout << "gingacc-mb test all done. ";
	cout << "press enter to exit";
	cout << endl;

	//TODO: more tests

	getchar();
	return 0;
}
