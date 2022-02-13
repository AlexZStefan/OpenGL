#pragma once
#include "Camera.h"
#include "Renderer.h"
#include "InputSystem.h"

#include <SDL.h>
#include <glew.h>
#include <SDL_opengl.h>
#include <gl\glu.h>
#include <stdio.h>
#include <string>
#include <wglew.h>
#include <iostream>

namespace GE {

	int windowHeight = 960;
	int windowWidth = 1280;


	class GameEngine {
	public:
		SDL_Surface* screen;
		// -1 addaptive Sync, 1 for updated sync with vertical retrace, 0 for immediate updates
		GameEngine(int setVSync);
		GameEngine(const GameEngine&);

		virtual ~GameEngine();

		bool init();
		bool keep_running();
		void update();
		void draw();
		void exit();

		void set_window_title(const char*);

	private:

		std::unique_ptr<Renderer> renderer;
		Camera* main_cam ;
		std::unique_ptr<InputSystem> controlls ;

		SDL_Event input_event;
		SDL_Window* window;

		SDL_GLContext glContext;

		
		int tick;

		int setVsync;

	};

	int time();

	void display_info_message(const char* msg);
}

