#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <iostream>
#include <string>
#include <vector>

//DEFINES__________________________________________________________________
#define MAX_CONTROLLERS 4

const char* pathsep = "/";

using namespace std;
//________________________GLOBAL VARIABLES AND STRUCTURES____________________

SDL_Event event;          //This is the handle for the Event Subsystem
SDL_Window * window;     //This is a handle for the Window
SDL_Renderer * renderer;
const Uint8 * Keystate;
char * data_path;
string res_path;
int Link_Num;
bool running = true;
double DT = 0.0;

//__________________________FUNCTIONS_____________________________________

void ScreenPresent() {
	SDL_RenderPresent(renderer);
}
void ScreenClear() {
	SDL_RenderClear(renderer);
}
void ScreenState(bool fs) {
	if (fs == true) { SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP); SDL_RenderSetLogicalSize(renderer, 640, 480);
	SDL_ShowCursor(0);
	}
	else if (fs == false) { SDL_SetWindowFullscreen(window, 0); SDL_ShowCursor(1); }
}

void SaveScreenshot(SDL_Renderer * renderer,int width,int height) {
	Uint32 Pixlformat = SDL_PIXELFORMAT_ARGB8888;
	SDL_Surface* screen = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, Pixlformat);
	SDL_RenderReadPixels(renderer, NULL, Pixlformat, screen->pixels, screen->pitch);
	SDL_SaveBMP(screen, "LegendOfScreenshot.png");
	SDL_FreeSurface(screen);
}

void GetBasePath() {
	char* base_path = SDL_GetBasePath();
	if (base_path) {
		res_path = base_path;
		SDL_free(base_path);
	}
	else {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"LegendOfZelda",
			"Cannot Find File Path",
			NULL);
	}
}

string GetFileFromPath( string folder, string file) {
	string fpath;
	fpath = res_path + folder + pathsep + file;
	return fpath;
}

//_______________________MAIN_____________________________________________

int main(int argc, char ** argv) {

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER|SDL_INIT_AUDIO);
	SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "opengl", SDL_HINT_OVERRIDE);
	GetBasePath();

	//Forward Declerations
	int WIDTH = 640, HEIGHT = 480, tps = 1000;
	bool Fullscreen = false, movement = false;
	string direction, message; int L = 0;
	Uint32 LastTime = SDL_GetTicks();

	//Command line argument processing

	for (int i = 0; i < argc; i++) {
		if (argc == 1) { break; }
		else {
			string arg = argv[i];
			if (arg == "-fs") {
				Fullscreen = true;
			}
		}
	}

	//Creating a window and renderer

	window = SDL_CreateWindow("Zelda Engine", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);

	if (window) {
		string path = GetFileFromPath("Icon", "icon.bmp");
		SDL_Surface* icon = SDL_LoadBMP(path.c_str());
		SDL_SetWindowIcon(window, icon);
		SDL_FreeSurface(icon);

		renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

		if (!renderer) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
				"LegendOfZelda",
				"Error! \n \n Renderer cannot be created! \n \n It's possible there is no OpenGL driver on your system (please download one).",
				NULL);
			running = false;
		}
	}
	else {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"LegendOfZelda",
			"Error! \n \n Window cannot be created!",
			NULL);
		running = false;
	}



	//CLASSES_______________________________________________________
	//Player Class__________________________________________________

	class Player {

		//Forward declarations for the class

		int W, H, X, Y, R_W, R_H, S_X, S_Y, Weapon, A_Rate, Velocity;
		bool Moving, Animate, Attacking;
		SDL_Surface * image; SDL_Texture * texture;
		SDL_Rect SRC_RECT, DES_RECT; string Direction;
		string path;
	public:
		Player(int w, int h, int x, int y) {
			W = w;
			H = h;
			X = x;
			Y = y;
			R_W = w;
			R_H = h;
			S_X = 0;
			S_Y = 0;
			Moving = false;
			Animate = true;
			Attacking = false;
			Weapon = 1;
			A_Rate = 0;
			path = GetFileFromPath("Resources", "Link.bmp");
			image = SDL_LoadBMP(path.c_str());
			if (image) {
				texture = SDL_CreateTextureFromSurface(renderer, image);
				SDL_FreeSurface(image);
			}
			else {
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
					"LegendOfZelda",
					"Link Bitmap Not Loaded",
					window);
				running = false;
			}
		}

		void Render() {
			SRC_RECT = { S_X, S_Y, W, H };
			DES_RECT = { X, Y, 50, 50 };
			SDL_RenderCopy(renderer, texture, &SRC_RECT, &DES_RECT);
		}

		void Quit() {
			SDL_DestroyTexture(texture);
		}

		void Logic(bool state, string direction) {
			Movement(state, direction);
			Animation();
		}

		void Movement(bool state, string direction) {
			Direction = direction;
			if (state == true) {
				Velocity = 2 * DT;
				Moving = true;
				Animate = true;
				if (Direction == "up") { Y -= Velocity; }
				else if (Direction == "down") { Y += Velocity; }
				else if (Direction == "left") { X -= Velocity; }
				else if (Direction == "right") { X += Velocity; }
			}
			else {
				Moving = false;
				Animate = false;
			}
		}

		void Animation() {
			if (Animate == true) { A_Rate += 1 ; }

			//Animations for movement

			if (Moving = true) {
				if (A_Rate == 6 ) {
					A_Rate = 0;
					if (Direction == "up") {
						S_X = 60;
						S_Y = 0;
						S_Y += 28;
						if (SRC_RECT.y == 28) { S_Y = 0; }
					}
					else if (Direction == "down") {
						S_X = 0;
						S_Y = 0;
						S_Y += 28;
						if (SRC_RECT.y == 28) { S_Y = 0; }
					}
					else if (Direction == "left") {
						S_X = 30;
						S_Y = 0;
						S_Y += 28;
						if (SRC_RECT.y == 28) { S_Y = 0; }
					}
					else if (Direction == "right") {
						S_Y = 28;
						S_X = 90;
						S_Y -= 28;
						if (SRC_RECT.y == 0) { S_Y = 28; }
					}
				}
			}
		}
	};

	class Background {
		SDL_Surface * image; SDL_Texture * texture;
		string path;
	public:
		Background() {
			path = GetFileFromPath("Resources", "Terrain.bmp");
			image = SDL_LoadBMP(path.c_str());
			if (image) {
				texture = SDL_CreateTextureFromSurface(renderer, image);
				SDL_FreeSurface(image);
			}
			else {
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
					"LegendOfZelda",

					"Terrain Bitmap Not Loaded",
					window);
				running = false;
			}
		}
		void Render() {
			SDL_RenderCopy(renderer, texture, NULL, NULL);
		}
		void Quit() {
			SDL_DestroyTexture(texture);
		}
	};

	//Controller Class___________________________________________________

	class GameController {

		SDL_GameController *Controllers[MAX_CONTROLLERS];
		int MaxJoysticks = SDL_NumJoysticks();

	public:
		int ControllerIndex = 0;

		GameController() {
			for (int JoystickIndex = 0; JoystickIndex < MaxJoysticks; ++JoystickIndex) {
				if (!SDL_IsGameController(JoystickIndex)) { continue; }
				if (ControllerIndex >= MAX_CONTROLLERS) { break; }
				Controllers[ControllerIndex] = SDL_GameControllerOpen(JoystickIndex);
				ControllerIndex++;
			}
		}


		bool GetButton(int ControllerNum, string Button) {
			ControllerNum = ControllerNum - 1;
			if (Button == "A") { return SDL_GameControllerGetButton(Controllers[ControllerNum], SDL_CONTROLLER_BUTTON_A); }
			if (Button == "B") { return SDL_GameControllerGetButton(Controllers[ControllerNum], SDL_CONTROLLER_BUTTON_B); }
			if (Button == "X") { return SDL_GameControllerGetButton(Controllers[ControllerNum], SDL_CONTROLLER_BUTTON_X); }
			if (Button == "Y") { return SDL_GameControllerGetButton(Controllers[ControllerNum], SDL_CONTROLLER_BUTTON_Y); }

			if (Button == "Up") { return SDL_GameControllerGetButton(Controllers[ControllerNum], SDL_CONTROLLER_BUTTON_DPAD_UP); }
			if (Button == "Down") { return SDL_GameControllerGetButton(Controllers[ControllerNum], SDL_CONTROLLER_BUTTON_DPAD_DOWN); }
			if (Button == "Left") { return SDL_GameControllerGetButton(Controllers[ControllerNum], SDL_CONTROLLER_BUTTON_DPAD_LEFT); }
			if (Button == "Right") { return SDL_GameControllerGetButton(Controllers[ControllerNum], SDL_CONTROLLER_BUTTON_DPAD_RIGHT); }
			else { return false; }
		}
	};
	//OBJECTS__________________________________________________

	vector <Player> Links{ Player(30, 30, 40, 120) };
	Background BG;

	double LT = SDL_GetTicks();

	//Event System_____________________________________________
	while (running) {

		GameController Gamepad = GameController();
		direction = "";
		movement = false;

		//_____________________________________________________
		double CT = SDL_GetTicks();
		DT = (CT - LT)/10;
		LT = CT;

		//EVENTS_______________________________________________
		//_______KEY EVENTS____________________________________
		Keystate = SDL_GetKeyboardState(NULL);

		if (Keystate[SDL_SCANCODE_ESCAPE]) {
			for (auto & Link : Links) { Link.Quit(); }
			BG.Quit();
			running = false;
			break;
		}

		if (Keystate[SDL_SCANCODE_UP]) {
			movement = true;
			direction = "up";
		}
		else if (Keystate[SDL_SCANCODE_DOWN]) {
			movement = true;
			direction = "down";
		}
		else if (Keystate[SDL_SCANCODE_LEFT]) {
			movement = true;
			direction = "left";
		}
		else if (Keystate[SDL_SCANCODE_RIGHT]) {
			movement = true;
			direction = "right";
		}

		//PAD EVENTS__________________________________________________________

		for (int i = 1; i < Gamepad.ControllerIndex + 1; i++) {
			if (Gamepad.GetButton(i, "Up") == true) {
				movement = true;
				direction = "up";
			}
			else if (Gamepad.GetButton(i, "Down") == true) {
				movement = true;
				direction = "down";
			}
			else if (Gamepad.GetButton(i, "Left") == true) {
				movement = true;
				direction = "left";
			}
			else if (Gamepad.GetButton(i, "Right") == true) {
				movement = true;
				direction = "right";
			}
		}
		//___________________________________________________________________

		ScreenState(Fullscreen);

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_a) {
					Links.push_back(Player(30, 30, 40, 120));
					Link_Num++;
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_1) {
					if (L < Link_Num) { L += 1; }
					else if (L >= Link_Num) { L = 0; }
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_F12) {
					if (Fullscreen == false) {
						Fullscreen = true; SDL_GetWindowSize(window, &event.window.data1, &event.window.data2); }
					else if (Fullscreen == true) {
						Fullscreen = false; SDL_GetWindowSize(window, &event.window.data1, &event.window.data2); }
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_F11) {
					SaveScreenshot(renderer, WIDTH, HEIGHT);
				}
			}
			if (event.type == SDL_CONTROLLERBUTTONDOWN) {
				if (event.cbutton.button == SDL_CONTROLLER_BUTTON_X) {
					Links.push_back(Player(30, 30, 40, 120));
					Link_Num++;
				}
				else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_Y) {
					if (L < Link_Num) { L += 1; }
					else if (L >= Link_Num) { L = 0; }
				}
			}
			if (event.type == SDL_QUIT) {
				for (auto & Link : Links) { Link.Quit(); }
				BG.Quit();
				running = false;
				break;
			}
		}
		SDL_Delay(10);

		//LOGIC_____________________________________________________

		Links[L].Logic(movement, direction);

		//RENDERING_________________________________________________

		ScreenClear();

		BG.Render();
		for (auto & Link : Links) { Link.Render(); }

		ScreenPresent();
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
