#include <iostream>
#include <time.h>
#include <stdio.h>
#include "SDL.h" 
#include "SDL_image.h"
#include "float.h"
#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "sphere.h"
#include "hittable.h"
#include "hittable_list.h"
#include "instance.h"
#include "mesh.h"
#include "raster.h"
#include "camera.h"
#include "color.h"
#include "texture.h"

#include <cstdio>
#include <vector>
#include "spline.h"


using namespace std;

int init(int width, int height) {
	/* // Initialize SDL2. */
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	/* Create the window where we will draw. */
	window = SDL_CreateWindow("Texture Mesh", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	/* We must call SDL_CreateRenderer in order for draw calls to affect this window. */
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		SDL_DestroyWindow(window);
		cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
	return 0;
}


void close() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* argv[])
{

	// World
	hittable_list world;
	list_light worldlight;
	
	// materiale per le superfici che dentro sono in vetro e fuori sono circondate da aria
	// inoltre questo vetro è colorato di marrone/giallo (vedi c_in)
	dielectric* m_vetro_marrone_in_aria = new dielectric();

	//consideriamo oggetti di vetro senza texture
	m_vetro_marrone_in_aria->texture = NULL;

	// colori per l'illuminazione di phong
	m_vetro_marrone_in_aria->kd = color(1.0, 1.0, 1.0)/1.1;
	m_vetro_marrone_in_aria->ka = color(1.0, 1.0, 1.0)/1.1;
	m_vetro_marrone_in_aria->ks = color(1.0, 1.0, 1.0);
	// (di solito kd e ka possono essere diminuiti per dare maggiore risalto ai giochi di
	//  riflessione e rifrazione, ad esempio se ci fosse uno sfondo o molta luce li ridurrei)
	
	// definiamo il coefficiente di lucentezza (come per i metalli sarà grande)
	m_vetro_marrone_in_aria->alpha = 180.0;

	// definiamo gli indici di rifrazione all'interno ed all'esterno del materiale 
	m_vetro_marrone_in_aria->eta_in = 1.51; // vetro
	m_vetro_marrone_in_aria->eta_out = 1.0; // vuoto

	// definisco i colori di filtraggio
	m_vetro_marrone_in_aria->c_out = color(1.0, 1.0, 1.0); // bianco perchè l'aria non modifica il colore
	m_vetro_marrone_in_aria->c_in = color(0.65, 0.45, 0.0);
	
	

	/*mesh* gatto = new mesh("models/cat.obj", "models/");
	auto instance_ptrG = make_shared<instance>(gatto, m_vetro_marrone_in_aria);
	instance_ptrG->rotate_x(-90.0f);
	instance_ptrG->translate(0.0f, -17.0f, 0.0f);
	instance_ptrG->scale(1.0 / 10.0f, 1.0 / 10.0f, 1.0 / 10.0f);
	instance_ptrG->InOmbrabile = true;
	instance_ptrG->FaOmbra = true;
	world.add(instance_ptrG);*/


	/*mesh* con = new mesh("models/bunny.obj", "models/");
	auto instance_ptrG = make_shared<instance>(con, m_vetro_marrone_in_aria);
	instance_ptrG->translate(0.0f, -0.5f, 0.0f);
	instance_ptrG->scale(3.0, 3.0, 3.0);
	instance_ptrG->InOmbrabile = true;
	instance_ptrG->FaOmbra = true;
	world.add(instance_ptrG);*/

	/*mesh* con = new mesh("models/bunny2.obj", "models/");
	auto instance_ptrG = make_shared<instance>(con, m_vetro_marrone_in_aria);
	instance_ptrG->translate(0.0f, -0.07f, 0.0f);
	instance_ptrG->scale(30.0f, 30.0f, 30.0f);
	instance_ptrG->InOmbrabile = true;
	instance_ptrG->FaOmbra = true;
	world.add(instance_ptrG);*/

	//NATIVITA'
	mesh* gesu = new mesh("models/Gesu con mangiatoia.obj", "models/");
	auto instance_ptrGesu = make_shared<instance>(gesu, m_vetro_marrone_in_aria);
	instance_ptrGesu->translate(0.0f, -0.5f, 0.0f);
	instance_ptrGesu->scale(6.0, 6.0, 6.0);
	instance_ptrGesu->InOmbrabile = true;
	instance_ptrGesu->FaOmbra = true;
	world.add(instance_ptrGesu);

	mesh* madonna = new mesh("models/virginMary.obj", "models/");
	auto instance_ptrMadonna = make_shared<instance>(madonna, m_vetro_marrone_in_aria);
	instance_ptrMadonna->translate(-0.3f, -0.5f, 0.0f);
	instance_ptrMadonna->scale(6.0, 6.0, 6.0);
	instance_ptrMadonna->InOmbrabile = true;
	instance_ptrMadonna->FaOmbra = true;
	world.add(instance_ptrMadonna);

	mesh* giuseppe = new mesh("models/Giuseppe.obj", "models/");
	auto instance_ptrGiuseppe = make_shared<instance>(giuseppe, m_vetro_marrone_in_aria);
	//instance_ptrGiuseppe->translate(0.2f, -0.48f, 0.15f);
	instance_ptrGiuseppe->translate(0.2f, -0.48f, 0.15f);
	instance_ptrGiuseppe->scale(6.0, 6.0, 6.0);
	instance_ptrGiuseppe->InOmbrabile = true;
	instance_ptrGiuseppe->FaOmbra = true;
	world.add(instance_ptrGiuseppe);

	mesh* bue = new mesh("models/bue.obj", "models/");
	auto instance_ptrBue = make_shared<instance>(bue, m_vetro_marrone_in_aria);
	instance_ptrBue->translate(0.15f, -0.48f, -0.020f);
	instance_ptrBue->scale(6.0, 6.0, 6.0);
	instance_ptrBue->InOmbrabile = true;
	instance_ptrBue->FaOmbra = true;
	world.add(instance_ptrBue);

	mesh* asinello = new mesh("models/asinello.obj", "models/");
	auto instance_ptrAsinello = make_shared<instance>(asinello, m_vetro_marrone_in_aria);
	instance_ptrAsinello->translate(-0.1f, -0.5f, -0.05f);
	instance_ptrAsinello->rotate_y(10);
	instance_ptrAsinello->scale(6, 6, 6);
	instance_ptrAsinello->InOmbrabile = true;
	instance_ptrAsinello->FaOmbra = true;
	world.add(instance_ptrAsinello);

	mesh* stalla = new mesh("models/stalla.obj", "models/");
	auto instance_ptrStalla = make_shared<instance>(stalla, m_vetro_marrone_in_aria);
	instance_ptrStalla->translate(0.0f, 0.0f, 0.0f);
	instance_ptrStalla->scale(0.7, 0.7, 0.7);
	instance_ptrStalla->InOmbrabile = true;
	instance_ptrStalla->FaOmbra = true;
	world.add(instance_ptrStalla);


	camera cam;
	cam.lookfrom = point3(0.0, 0.0, 10.0);
	cam.lookat = point3(0.0f, 0.0f, 0.0f);

	cam.aspect_ratio = 16.0f / 9.0f;
	cam.image_width = 1500; // 1280;
	cam.samples_per_pixel = 10;
	cam.vfov = 60;

	cam.initialize();

	if (init(cam.image_width, cam.image_height) == 1) {
		cout << "App Error! " << std::endl;
		return 1;
	}

	cout << "Image Resolution: " << cam.image_width << "x" << cam.image_height << "\n255\n";

	time_t start, end;
	time(&start);

	cam.parallel_render(world, worldlight);
	//cam.render(world, *worldlight);

	time(&end);
	double dif = difftime(end, start);
	cout << "\n" << "Rendering time: " << dif << "\n";

	SDL_Event event;
	bool quit = false;

	/* Poll for events */
	while (SDL_PollEvent(&event) || (!quit)) {

		switch (event.type) {

		case SDL_QUIT:
			quit = true;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				quit = true;
				break;
				// cases for other keypresses

			case SDLK_s:
				saveScreenshotBMP("screenshot.bmp");
				cout << "Screenshot saved!" << endl;
				break;
			}
		}
	}

	close();
	return 0;
}
