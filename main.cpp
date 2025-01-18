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
#include "plane.h"
#include <cstdio>
#include <vector>
#include "spline.h"
#include "geometry.h"


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

	//luce1 
	point3 light_position(-16.0, 3.0, 30.0);
	shared_ptr<point_light> punto = make_shared<point_light>(light_position, black, darkgray, black);
	worldlight.add(punto);


	point3 light_position2(0.5f, 10.0f, 0.0f);
	shared_ptr<point_light> punto2 = make_shared<point_light>(light_position2, black, darkgray, black);
	worldlight.add(punto2);

	shared_ptr<diffuse_light> diff = make_shared<diffuse_light>(vec3(-10.0, 50.0, 60.0), darkgray, white, black);
	worldlight.add(diff);
	shared_ptr<diffuse_light> diff2 = make_shared<diffuse_light>(vec3(10.0, 50.0, 60.0), darkgray, white, black);
	worldlight.add(diff2);

	
	// materiale per le superfici che dentro sono in vetro e fuori sono circondate da aria
	// inoltre questo vetro è colorato di marrone/giallo (vedi c_in)
	dielectric* m_vetro_marrone_in_aria = new dielectric();

	//consideriamo oggetti di vetro senza texture
	m_vetro_marrone_in_aria->texture = NULL;

	// colori per l'illuminazione di phong
	m_vetro_marrone_in_aria->kd = color(1.0, 1.0, 1.0) / 1.1; // /10.0;
	m_vetro_marrone_in_aria->ka = color(1.0, 1.0, 1.0) / 1.1; // / 10.0;
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
	


	// materiale per le superfici che dentro sono in vetro e fuori sono circondate da aria
	// inoltre questo vetro è colorato di celeste (vedi c_in)
	dielectric* m_vetro_caleste_in_aria = new dielectric();

	//consideriamo oggetti di vetro senza texture
	m_vetro_caleste_in_aria->texture = NULL;

	// colori per l'illuminazione di phong
	m_vetro_caleste_in_aria->kd = color(1.0, 1.0, 1.0) / 1.1;
	m_vetro_caleste_in_aria->ka = color(1.0, 1.0, 1.0) / 1.1;
	m_vetro_caleste_in_aria->ks = color(1.0, 1.0, 1.0);
	// (di solito kd e ka possono essere diminuiti per dare maggiore risalto ai giochi di
	//  riflessione e rifrazione, ad esempio se ci fosse uno sfondo o molta luce li ridurrei)

	// definiamo il coefficiente di lucentezza (come per i metalli sarà grande)
	m_vetro_caleste_in_aria->alpha = 180.0;

	// definiamo gli indici di rifrazione all'interno ed all'esterno del materiale 
	m_vetro_caleste_in_aria->eta_in = 1.51; // vetro
	m_vetro_caleste_in_aria->eta_out = 1.0; // vuoto

	// definisco i colori di filtraggio
	m_vetro_caleste_in_aria->c_out = color(1.0, 1.0, 1.0); // bianco perchè l'aria non modifica il colore
	m_vetro_caleste_in_aria->c_in = color(153.0/255.0, 203.0/255.0, 1.0);



	// materiale per le superfici che dentro sono in vetro e fuori sono circondate da aria
	// inoltre questo vetro è colorato di rosa (vedi c_in)
	dielectric* m_vetro_rosa_in_aria = new dielectric();

	//consideriamo oggetti di vetro senza texture
	m_vetro_rosa_in_aria->texture = NULL;

	// colori per l'illuminazione di phong
	m_vetro_rosa_in_aria->kd = color(1.0, 1.0, 1.0) / 1.1;
	m_vetro_rosa_in_aria->ka = color(1.0, 1.0, 1.0) / 1.1;
	m_vetro_rosa_in_aria->ks = color(1.0, 1.0, 1.0);
	// (di solito kd e ka possono essere diminuiti per dare maggiore risalto ai giochi di
	//  riflessione e rifrazione, ad esempio se ci fosse uno sfondo o molta luce li ridurrei)

	// definiamo il coefficiente di lucentezza (come per i metalli sarà grande)
	m_vetro_rosa_in_aria->alpha = 180.0;

	// definiamo gli indici di rifrazione all'interno ed all'esterno del materiale 
	m_vetro_rosa_in_aria->eta_in = 1.51; // vetro
	m_vetro_rosa_in_aria->eta_out = 1.0; // vuoto

	// definisco i colori di filtraggio
	m_vetro_rosa_in_aria->c_out = color(1.0, 1.0, 1.0); // bianco perchè l'aria non modifica il colore
	m_vetro_rosa_in_aria->c_in = color(pow(198.0 / 255.0, 3.0), pow(146.0 / 255.0, 3.0), pow(148.0 / 255.0, 3.0));



	// materiale per le superfici che dentro sono in vetro e fuori sono circondate da aria
	// inoltre questo vetro è color oro/giallo (vedi c_in)
	dielectric* m_vetro_oro_in_aria = new dielectric();

	//consideriamo oggetti di vetro senza texture
	m_vetro_oro_in_aria->texture = NULL;

	// colori per l'illuminazione di phong
	m_vetro_oro_in_aria->kd = color(1.0, 1.0, 1.0) / 1.1;
	m_vetro_oro_in_aria->ka = color(1.0, 1.0, 1.0) / 1.1;
	m_vetro_oro_in_aria->ks = color(1.0, 1.0, 1.0);
	// (di solito kd e ka possono essere diminuiti per dare maggiore risalto ai giochi di
	//  riflessione e rifrazione, ad esempio se ci fosse uno sfondo o molta luce li ridurrei)

	// definiamo il coefficiente di lucentezza (come per i metalli sarà grande)
	m_vetro_oro_in_aria->alpha = 180.0;

	// definiamo gli indici di rifrazione all'interno ed all'esterno del materiale 
	m_vetro_oro_in_aria->eta_in = 1.51; // vetro
	m_vetro_oro_in_aria->eta_out = 1.0; // vuoto

	// definisco i colori di filtraggio
	m_vetro_oro_in_aria->c_out = color(1.0, 1.0, 1.0); // bianco perchè l'aria non modifica il colore
	m_vetro_oro_in_aria->c_in = color(0.65, 0.45, 0.0); //= color(205.0 / 255.0, 164.0 / 255.0, 52.0 / 255.0);



	// materiale per le superfici che dentro sono in vetro e fuori sono circondate da aria
	// inoltre questo vetro è color grigio (vedi c_in)
	dielectric* m_vetro_grigio_in_aria = new dielectric();

	//consideriamo oggetti di vetro senza texture
	m_vetro_grigio_in_aria->texture = NULL;

	// colori per l'illuminazione di phong
	m_vetro_grigio_in_aria->kd = color(1.0, 1.0, 1.0) / 1.1;
	m_vetro_grigio_in_aria->ka = color(1.0, 1.0, 1.0) / 1.1;
	m_vetro_grigio_in_aria->ks = color(1.0, 1.0, 1.0);
	// (di solito kd e ka possono essere diminuiti per dare maggiore risalto ai giochi di
	//  riflessione e rifrazione, ad esempio se ci fosse uno sfondo o molta luce li ridurrei)

	// definiamo il coefficiente di lucentezza (come per i metalli sarà grande)
	m_vetro_grigio_in_aria->alpha = 180.0;

	// definiamo gli indici di rifrazione all'interno ed all'esterno del materiale 
	m_vetro_grigio_in_aria->eta_in = 1.51; // vetro
	m_vetro_grigio_in_aria->eta_out = 1.0; // vuoto

	// definisco i colori di filtraggio
	m_vetro_grigio_in_aria->c_out = color(1.0, 1.0, 1.0); // bianco perchè l'aria non modifica il colore
	m_vetro_grigio_in_aria->c_in = color(120.0 / 255.0, 120.0 / 255.0, 120.0 / 255.0);



	// materiale per le superfici che dentro sono in vetro e fuori sono circondate da aria
	// inoltre questo vetro non ha colore (vedi c_in)
	dielectric* m_vetro_in_aria = new dielectric();

	//consideriamo oggetti di vetro senza texture
	m_vetro_in_aria->texture = NULL;

	// colori per l'illuminazione di phong
	m_vetro_in_aria->kd = color(1.0, 1.0, 1.0) / 1.7;
	m_vetro_in_aria->ka = color(1.0, 1.0, 1.0) / 1.7;
	m_vetro_in_aria->ks = color(1.0, 1.0, 1.0);
	// (di solito kd e ka possono essere diminuiti per dare maggiore risalto ai giochi di
	//  riflessione e rifrazione, ad esempio se ci fosse uno sfondo o molta luce li ridurrei)

	// definiamo il coefficiente di lucentezza (come per i metalli sarà grande)
	m_vetro_in_aria->alpha = 180.0;

	// definiamo gli indici di rifrazione all'interno ed all'esterno del materiale 
	m_vetro_in_aria->eta_in = 1.51; // vetro
	m_vetro_in_aria->eta_out = 1.0; // vuoto

	// definisco i colori di filtraggio
	m_vetro_in_aria->c_out = color(1.0, 1.0, 1.0); // bianco perchè l'aria non modifica il colore
	m_vetro_in_aria->c_in = color(0.75, 0.75, 0.75);
	

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
	//mesh* gesu = new mesh("models/Gesu con mangiatoia.obj", "models/");
	//auto instance_ptrGesu = make_shared<instance>(gesu, m_vetro_oro_in_aria);
	//// ingrandimento
	//float scala = 2.0;
	//instance_ptrGesu->translate(0.0f, 0.0f, -0.15f);
	//instance_ptrGesu->scale(scala, scala, scala);
	//instance_ptrGesu->translate(0.0f, 0.0f, scala * 0.15f);

	//instance_ptrGesu->translate(0.0f, -0.5f, 0.0f);
	//instance_ptrGesu->scale(6.0, 6.0, 6.0);
	//instance_ptrGesu->InOmbrabile = true;
	//instance_ptrGesu->FaOmbra = true;
	//world.add(instance_ptrGesu);

	//mesh* madonna = new mesh("models/virginMary.obj", "models/");
	//auto instance_ptrMadonna = make_shared<instance>(madonna, m_vetro_rosa_in_aria);
	//// ingrandimento
	//float scala2 = 1.75;
	//instance_ptrMadonna->translate(0.1f, 0.0f, -0.05f);
	//instance_ptrMadonna->scale(scala2, scala2, scala2);
	//instance_ptrMadonna->translate( - scala2 / 10.0f, 0.0f, scala2 / 20.0f);

	//instance_ptrMadonna->translate(-0.05f, -0.5f, 0.0f);
	//instance_ptrMadonna->translate(0.0f, -0.0f, 0.12f);
	//instance_ptrMadonna->scale(6.0, 6.0, 6.0);
	//instance_ptrMadonna->InOmbrabile = true;
	//instance_ptrMadonna->FaOmbra = true;
	//world.add(instance_ptrMadonna);

	//mesh* giuseppe = new mesh("models/Giuseppe.obj", "models/");
	//auto instance_ptrGiuseppe = make_shared<instance>(giuseppe, m_vetro_caleste_in_aria);
	//// ingrandimento
	//float scala3 = 1.9;
	//instance_ptrGiuseppe->translate(-0.11f, 0.0f, -0.05f);
	//instance_ptrGiuseppe->scale(scala3, scala3, scala3);
	//instance_ptrGiuseppe->rotate_y(0.0);
	//instance_ptrGiuseppe->translate(scala3 * 0.11f, 0.0f, scala3 / 20.0f);

	////instance_ptrGiuseppe->translate(0.2f, -0.48f, 0.15f);
	//instance_ptrGiuseppe->translate(0.07f, -0.48f, 0.15f);
	//instance_ptrGiuseppe->translate(-0.07f, -0.0f, -0.20f);
	//instance_ptrGiuseppe->scale(6.0, 6.0, 6.0);
	//instance_ptrGiuseppe->InOmbrabile = true;
	//instance_ptrGiuseppe->FaOmbra = true;
	//world.add(instance_ptrGiuseppe);

	//mesh* bue = new mesh("models/bue.obj", "models/");
	//auto instance_ptrBue = make_shared<instance>(bue, m_vetro_grigio_in_aria);
	//// ingrandimento
	//instance_ptrBue->translate(-0.1f, 0.0f, 0.07f);
	//instance_ptrBue->scale(scala2, scala2, scala2);
	//instance_ptrBue->rotate_y(-15.0);
	//instance_ptrBue->translate(scala2 * 0.1f, -0.0f, - scala2 * 0.07f);

	//instance_ptrBue->translate(0.15f, -0.48f, -0.020f);
	//instance_ptrBue->translate(0.0f, -0.0f, 0.4f);
	//instance_ptrBue->scale(6.0, 6.0, 6.0);
	//instance_ptrBue->InOmbrabile = true;
	//instance_ptrBue->FaOmbra = true;
	//world.add(instance_ptrBue);

	//mesh* asinello = new mesh("models/asinello.obj", "models/");
	//auto instance_ptrAsinello = make_shared<instance>(asinello, m_vetro_grigio_in_aria);
	//// ingrandimento
	//instance_ptrAsinello->translate(0.11f, 0.0f, 0.07f);
	//instance_ptrAsinello->scale(scala3, scala3, scala3);
	//instance_ptrAsinello->rotate_y(-25.0);
	//instance_ptrAsinello->translate(- scala3 * 0.11f, 0.0f, - scala3 * 0.07f);

	//instance_ptrAsinello->translate(-0.1f, -0.5f, -0.05f);
	//instance_ptrAsinello->translate(0.15f, -0.0f, -0.07f);
	//instance_ptrAsinello->rotate_y(10);
	//instance_ptrAsinello->scale(6, 6, 6);
	//instance_ptrAsinello->InOmbrabile = true;
	//instance_ptrAsinello->FaOmbra = true;
	//world.add(instance_ptrAsinello);

	mesh* stalla = new mesh("models/stalla.obj", "models/");
	auto instance_ptrStalla = make_shared<instance>(stalla, m_vetro_in_aria);
	instance_ptrStalla->scale(0.75, 0.7, 0.7);
	/*instance_ptrStalla->translate(0.5f, 0.0f, 0.7f);*/
	instance_ptrStalla->translate(0.5f, -10.4f, 0.7f);
	instance_ptrStalla->InOmbrabile = true;
	instance_ptrStalla->FaOmbra = true;
	world.add(instance_ptrStalla);

	mesh* library = new mesh("models/library.obj", "models/");
	texture* library_tex = new image_texture("models/BaseColor.jpg");
	material* library_m = new material();
	library_m->texture = library_tex;
	library_m->ka = white;
	library_m->ks = white;
	auto library_ptr = make_shared<instance>(library, library_m);
	library_ptr->scale(3.8f, 3.8f, 3.8f);
	library_ptr->rotate_y(-90.0f);
	library_ptr->translate(28.0f, -19.0f, -18.0f);
	library_ptr->InOmbrabile = true;
	library_ptr->FaOmbra = true;
	world.add(library_ptr);

	mesh* chair = new mesh("models/chair.obj", "models/");
	texture* chair_tex = new image_texture("models/chairTexture.jpg");
	material* chair_m = new material();
	chair_m->texture = chair_tex;
	chair_m->ka = white;
	chair_m->ks = white;
	auto chair_ptr = make_shared<instance>(chair, chair_m);
	chair_ptr->scale(0.15f, 0.15f, 0.15f);
	chair_ptr->rotate_y(-150.0f);
	chair_ptr->translate(35.0f, -20.5f, 5.0f);
	chair_ptr->InOmbrabile = true;
	chair_ptr->FaOmbra = true;
	world.add(chair_ptr);

	mesh* tv = new mesh("models/TV.obj", "models/");
	texture* tv_tex = new image_texture("models/TV_tex.jpg");
	material* tv_m = new material();
	tv_m->texture = tv_tex;
	tv_m->ka = black;
	tv_m->ks = white;
	auto tv_ptr = make_shared<instance>(tv, tv_m);
	tv_ptr->scale(12.0, 12.0, 12.0);
	tv_ptr->translate(-47.5f, -8.7f, -34.0f);
	tv_ptr->rotate_y(20.0f);
	tv_ptr->InOmbrabile = true;
	tv_ptr->FaOmbra = true;
	world.add(tv_ptr);


	mesh* sideTableNativity = new mesh("models/tavolinoPresepe.obj", "models/");
	texture* sideTableNativity_tex = new image_texture("models/BaseColor.jpg");
	material* sideTableNativity_m = new material();
	sideTableNativity_m->texture = sideTableNativity_tex;
	sideTableNativity_m->ka = white;
	sideTableNativity_m->ks = black;
	auto sideTableNativity_ptr = make_shared<instance>(sideTableNativity, sideTableNativity_m);
	sideTableNativity_ptr->scale(2.2, 2.2, 2.2);
	sideTableNativity_ptr->translate(-6.0f, -16.1f, 3.0f);
	sideTableNativity_ptr->InOmbrabile = true;
	sideTableNativity_ptr->FaOmbra = true;
	world.add(sideTableNativity_ptr);

	mesh* room = new mesh("models/room.obj", "models/");
	texture* room_tex = new image_texture("models/panna.jpg");
	material* room_m = new material();
	room_m->texture = room_tex;
	room_m->ka = white;
	room_m->ks = black;
	auto room_ptr = make_shared<instance>(room, room_m);
	room_ptr->scale(3.0, 3.0, 3.0);
	room_ptr->translate(-6.0f, -14.5f, 15.0f);
	room_ptr->InOmbrabile = true;
	room_ptr->FaOmbra = true;
	world.add(room_ptr);

	mesh* wallForniture = new mesh("models/mobiliMuro.obj", "models/");
	texture* wallForniture_tex = new image_texture("models/legnoGiallo.jpg");
	material* wallForniture_m = new material();
	wallForniture_m->texture = wallForniture_tex;
	wallForniture_m->ka = white;
	wallForniture_m->ks = black;
	auto wallForniture_ptr = make_shared<instance>(wallForniture, wallForniture_m);
	wallForniture_ptr->scale(0.50, 0.50, 0.50);
	wallForniture_ptr->rotate_y(270.0f);
	wallForniture_ptr->translate(-73.0f, -28.5f, -30.0f);
	wallForniture_ptr->InOmbrabile = true;
	wallForniture_ptr->FaOmbra = true;
	world.add(wallForniture_ptr);

	mesh* chandelier = new mesh("models/chandelier.obj", "models/");
	auto instance_ptrChandelier = make_shared<instance>(chandelier, m_vetro_in_aria);
	instance_ptrChandelier->scale(29.0, 29.0, 29.0);
	/*instance_ptrChandelier->translate(0.5f, 0.0f, 0.7f);*/
	instance_ptrChandelier->translate(0.5f, 10.0f, -14.5f);
	instance_ptrChandelier->InOmbrabile = true;
	instance_ptrChandelier->FaOmbra = false;
	world.add(instance_ptrChandelier);

	//mesh* wallLamp = new mesh("models/lampadaMuro.obj", "models/");
	//auto wallLamp_ptr = make_shared<instance>(wallLamp, m_vetro_in_aria);
	//wallLamp_ptr->scale(30.0, 30.0, 30.0);
	///*wallLamp_ptr->translate(0.5f, 0.0f, 0.7f);*/
	//wallLamp_ptr->translate(15.5f, -5.0f, -5.7f); 
	//wallLamp_ptr->translate(18.0f, 0.0f, 0.0f);
	//wallLamp_ptr->InOmbrabile = true;
	//wallLamp_ptr->FaOmbra = false;
	//world.add(wallLamp_ptr);
	
	mesh* wallLamp2 = new mesh("models/lampadaMuro2.obj", "models/");
	auto wallLamp_ptr2 = make_shared<instance>(wallLamp2, m_vetro_in_aria);
	wallLamp_ptr2->scale(3.8, 3.8, 3.8);
	wallLamp_ptr2->rotate_y(-90.0f);
	/*wallLamp_ptr->translate(0.5f, 0.0f, 0.7f);*/
	wallLamp_ptr2->translate(50.0f, 0.0f, -10.0f); 
	wallLamp_ptr2->InOmbrabile = true;
	wallLamp_ptr2->FaOmbra = false;
	world.add(wallLamp_ptr2);

	camera cam;
	//vicino di fronte, guarda dritto
	/*cam.lookfrom = point3(0.0, -14.0, 35.0);*/
	//lontano di fronte, guarda centro
	cam.lookfrom = point3(0.0, 0.0, 75.0);
	cam.lookat = point3(0.0f, -10.0f, -0.15f);
	//guarda tv
	/*cam.lookfrom = point3(-30.0, -6.0, 20.0);
	cam.lookat = point3(-40.0f, -6.0f, -0.15f);*/

	cam.aspect_ratio = 16.0f / 9.0f;
	cam.image_width = 1500; // 1280;
	cam.samples_per_pixel = 10;
	cam.vfov = 50;

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
