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
#include "texture.h"
//#include "plane.h"
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

	//luce1 da sinistra avanti (simula lampada a muro)
	point3 light_position(40.5, 10.0, -2.5);
	shared_ptr<point_light> punto = make_shared<point_light>(light_position, black, darkgray, darkgray);
	worldlight.add(punto);

	// luce2 da sopra (simula lampadario)
	point3 light_position2(0.5f, 20.0f, 0.0f);
	shared_ptr<point_light> punto2 = make_shared<point_light>(light_position2, black, darkgray, darkgray);
	worldlight.add(punto2);

	shared_ptr<diffuse_light> diff = make_shared<diffuse_light>(vec3(-10.0, 50.0, 60.0), darkgray, white, black);
	worldlight.add(diff);
	shared_ptr<diffuse_light> diff2 = make_shared<diffuse_light>(vec3(10.0, 50.0, 60.0), darkgray, white, black);
	worldlight.add(diff2);

	// imposto che tutte le luci provino ad usare la texture (dove presente) per il colore ambientale
	for (int i = 0; i < worldlight.size; i++) {
		worldlight.lights[i]->ambTex = true;
	}

	//MATERIALI

	// Vetro Marrone
	// materiale per le superfici che dentro sono in vetro e fuori sono circondate da aria
	// inoltre questo vetro è colorato di marrone (vedi c_in)
	dielectric* m_vetro_marrone_in_aria = new dielectric();
	// consideriamo oggetti di vetro senza texture
	m_vetro_marrone_in_aria->texture = NULL;
	// colori per l'illuminazione di Phong
	m_vetro_marrone_in_aria->kd = color(1.0, 1.0, 1.0) / 1.1;
	m_vetro_marrone_in_aria->ka = color(1.0, 1.0, 1.0) / 1.1;
	m_vetro_marrone_in_aria->ks = color(1.0, 1.0, 1.0);
	// definiamo il coefficiente di lucentezza
	m_vetro_marrone_in_aria->alpha = 180.0;
	// definiamo gli indici di rifrazione
	m_vetro_marrone_in_aria->eta_in = 1.51;
	m_vetro_marrone_in_aria->eta_out = 1.0;
	// definisco i colori di filtraggio
	m_vetro_marrone_in_aria->c_out = color(1.0, 1.0, 1.0);
	m_vetro_marrone_in_aria->c_in = color(139.0 / 255.0, 69.0 / 255.0, 19.0 / 255.0);

	//Vetro verde
	// materiale per le superfici che dentro sono in vetro e fuori sono circondate da aria
	// inoltre questo vetro è colorato di verde (vedi c_in)
	dielectric* m_vetro_verde_in_aria = new dielectric();
	// consideriamo oggetti di vetro senza texture
	m_vetro_verde_in_aria->texture = NULL;
	// colori per l'illuminazione di Phong
	m_vetro_verde_in_aria->kd = color(1.0, 1.0, 1.0) / 1.1;
	m_vetro_verde_in_aria->ka = color(1.0, 1.0, 1.0) / 1.1;
	m_vetro_verde_in_aria->ks = color(1.0, 1.0, 1.0);
	// definiamo il coefficiente di lucentezza
	m_vetro_verde_in_aria->alpha = 180.0;
	// definiamo gli indici di rifrazione
	m_vetro_verde_in_aria->eta_in = 1.51;
	m_vetro_verde_in_aria->eta_out = 1.0;
	// definisco i colori di filtraggio
	m_vetro_verde_in_aria->c_out = color(1.0, 1.0, 1.0);
	m_vetro_verde_in_aria->c_in = color(34.0 / 255.0, 139.0 / 255.0, 34.0 / 255.0);

	//Vetro viola
	// materiale per le superfici che dentro sono in vetro e fuori sono circondate da aria
	// inoltre questo vetro è colorato di viola (vedi c_in)
	dielectric* m_vetro_viola_in_aria = new dielectric();
	// consideriamo oggetti di vetro senza texture
	m_vetro_viola_in_aria->texture = NULL;
	// colori per l'illuminazione di Phong
	m_vetro_viola_in_aria->kd = color(1.0, 1.0, 1.0) / 1.1;
	m_vetro_viola_in_aria->ka = color(1.0, 1.0, 1.0) / 1.1;
	m_vetro_viola_in_aria->ks = color(1.0, 1.0, 1.0);
	// definiamo il coefficiente di lucentezza
	m_vetro_viola_in_aria->alpha = 180.0;
	// definiamo gli indici di rifrazione
	m_vetro_viola_in_aria->eta_in = 1.51;
	m_vetro_viola_in_aria->eta_out = 1.0;
	// definisco i colori di filtraggio
	m_vetro_viola_in_aria->c_out = color(1.0, 1.0, 1.0);
	m_vetro_viola_in_aria->c_in = color(138.0 / 255.0, 43.0 / 255.0, 226.0 / 255.0);

	//Vetro celeste
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
	m_vetro_caleste_in_aria->c_in = color(153.0 / 255.0, 203.0 / 255.0, 1.0);

	//Vetro rosa
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

	//Vetro oro
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

	//Vetro grigio
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

	//Vetro normale
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

	/*mesh* stalla = new mesh("models/stalla.obj", "models/");
	auto instance_ptrStalla = make_shared<instance>(stalla, m_vetro_in_aria);
	instance_ptrStalla->scale(0.75, 0.7, 0.7);
	instance_ptrStalla->scale(1.5, 1.6, 2.0);
	instance_ptrStalla->translate(-7.0f, 5.4f, 5.0f);
	instance_ptrStalla->translate(0.5f, -10.4f, -7.7f);
	instance_ptrStalla->InOmbrabile = true;
	instance_ptrStalla->FaOmbra = true;
	world.add(instance_ptrStalla);*/

	mesh* stalla = new mesh("models/stalla.obj", "models/");
	auto instance_ptrStalla = make_shared<instance>(stalla, m_vetro_in_aria);
	instance_ptrStalla->scale(0.75, 0.7, 0.7);
	instance_ptrStalla->scale(1.5, 1.6, 2.0);
	instance_ptrStalla->translate(-7.0f, 5.4f, 5.0f);
	instance_ptrStalla->translate(5.0f, -10.4f, -9.7f);
	instance_ptrStalla->rotate_y(17.0);
	instance_ptrStalla->InOmbrabile = true;
	instance_ptrStalla->FaOmbra = true;
	world.add(instance_ptrStalla);

	mesh* palms = new mesh("models/palmsBase.obj", "models/");
	//sinistra
	auto instance_ptrPalms = make_shared<instance>(palms, m_vetro_verde_in_aria);
	instance_ptrPalms->scale(8.5, 8.5, 8.5);
	instance_ptrPalms->translate(-16.0f, -10.0f, -9.0f);
	instance_ptrPalms->InOmbrabile = true;
	instance_ptrPalms->FaOmbra = true;
	world.add(instance_ptrPalms);
	//destra
	auto instance2_ptrPalms = make_shared<instance>(palms, m_vetro_verde_in_aria);
	instance2_ptrPalms->scale(8.5, 8.5, 8.5);
	instance2_ptrPalms->rotate_y(90.0);
	instance2_ptrPalms->translate(6.5f, -10.0f, -5.0f);
	instance2_ptrPalms->InOmbrabile = true;
	instance2_ptrPalms->FaOmbra = true;
	world.add(instance2_ptrPalms);

	mesh* threeWise = new mesh("models/threeNegros.obj", "models/");
	auto instance_ptrWise = make_shared<instance>(threeWise, m_vetro_viola_in_aria);
	instance_ptrWise->scale(3.5, 3.5, 3.5);
	instance_ptrWise->translate(2.5f, -8.5f, 4.0f);
	instance_ptrWise->rotate_y(0.0);
	instance_ptrWise->InOmbrabile = true;
	instance_ptrWise->FaOmbra = true;
	world.add(instance_ptrWise);

	mesh* library = new mesh("models/library.obj", "models/");
	texture* library_tex = new image_texture("models/BaseColor.jpg");
	material* library_m = new material();
	library_m->texture = library_tex;
	library_m->ka = white;
	library_m->ks = white;
	library_m->alpha = 70;
	auto library_ptr = make_shared<instance>(library, library_m);
	library_ptr->scale(3.8f, 3.8f, 3.8f);
	library_ptr->rotate_y(-90.0f);
	library_ptr->translate(28.0f, -19.0f, -18.0f);
	library_ptr->InOmbrabile = true;
	library_ptr->FaOmbra = true;
	world.add(library_ptr);

	mesh* book = new mesh("models/book.obj", "models/");
	texture* book_tex = new image_texture("models/book_tex.jpg");
	material* book_m = new material();
	book_m->texture = book_tex;
	book_m->ka = white;
	book_m->ks = black;
	book_m->alpha = 70;
	auto book_ptr = make_shared<instance>(book, book_m);
	book_ptr->scale(15.5f, 15.5f, 15.5f);
	book_ptr->rotate_y(-270.0f);
	book_ptr->translate(38.0f, 5.75f, -21.5f);
	book_ptr->InOmbrabile = false;
	book_ptr->FaOmbra = false;
	world.add(book_ptr);
	auto book2_ptr = make_shared<instance>(book, book_m);
	book2_ptr->scale(15.5f, 15.5f, 15.5f);
	book2_ptr->rotate_y(-270.0f);
	book2_ptr->translate(37.0f, 5.75f, -21.5f);
	book2_ptr->InOmbrabile = false;
	book2_ptr->FaOmbra = false;
	world.add(book2_ptr);
	auto book3_ptr = make_shared<instance>(book, book_m);
	book3_ptr->scale(15.5f, 15.5f, 15.5f);
	book3_ptr->rotate_y(-270.0f);
	book3_ptr->translate(36.0f, 5.75f, -21.5f);
	book3_ptr->InOmbrabile = false;
	book3_ptr->FaOmbra = false;
	world.add(book3_ptr);
	auto book4_ptr = make_shared<instance>(book, book_m);
	book4_ptr->scale(15.5f, 15.5f, 15.5f);
	book4_ptr->rotate_y(-270.0f);
	book4_ptr->translate(35.0f, 5.75f, -21.5f);
	book4_ptr->InOmbrabile = false;
	book4_ptr->FaOmbra = false;
	world.add(book4_ptr);
	auto book5_ptr = make_shared<instance>(book, book_m);
	book5_ptr->scale(15.5f, 15.5f, 15.5f);
	book5_ptr->rotate_y(-270.0f);
	book5_ptr->translate(34.0f, 5.75f, -21.5f);
	book5_ptr->InOmbrabile = true;
	book5_ptr->FaOmbra = false;
	world.add(book5_ptr);
	auto book6_ptr = make_shared<instance>(book, book_m);
	book6_ptr->scale(15.5f, 15.5f, 15.5f);
	book6_ptr->rotate_y(-270.0f);
	book6_ptr->translate(33.0f, 5.75f, -21.5f);
	book6_ptr->InOmbrabile = true;
	book6_ptr->FaOmbra = false;
	world.add(book6_ptr);

	mesh* greenBook = new mesh("models/greenBook.obj", "models/");
	texture* greenBook_tex = new image_texture("models/greenBook_tex.jpg");
	material* greenBook_m = new material();
	greenBook_m->texture = greenBook_tex;
	greenBook_m->ka = white;
	greenBook_m->ks = black;
	greenBook_m->alpha = 70;
	auto greenBook_ptr = make_shared<instance>(greenBook, greenBook_m);
	greenBook_ptr->scale(1.7f, 1.5f, 1.7f);
	greenBook_ptr->rotate_y(-90.0f);
	greenBook_ptr->rotate_z(180.0f);
	greenBook_ptr->translate(39.0f, 2.8f, -22.5f);
	greenBook_ptr->InOmbrabile = false;
	greenBook_ptr->FaOmbra = false;
	world.add(greenBook_ptr);
	auto greenBook2_ptr = make_shared<instance>(greenBook, greenBook_m);
	greenBook2_ptr->scale(1.7f, 1.5f, 1.7f);
	greenBook2_ptr->rotate_y(-90.0f);
	greenBook2_ptr->rotate_z(180.0f);
	greenBook2_ptr->translate(38.0f, 2.8f, -22.5f);
	greenBook2_ptr->InOmbrabile = false;
	greenBook2_ptr->FaOmbra = false;
	world.add(greenBook2_ptr);
	auto greenBook3_ptr = make_shared<instance>(greenBook, greenBook_m);
	greenBook3_ptr->scale(1.7f, 1.5f, 1.7f);
	greenBook3_ptr->rotate_y(-90.0f);
	greenBook3_ptr->rotate_z(180.0f);
	greenBook3_ptr->translate(37.0f, 2.8f, -22.5f);
	greenBook3_ptr->InOmbrabile = false;
	greenBook3_ptr->FaOmbra = false;
	world.add(greenBook3_ptr);
	auto greenBook4_ptr = make_shared<instance>(greenBook, greenBook_m);
	greenBook4_ptr->scale(1.7f, 1.5f, 1.7f);
	greenBook4_ptr->rotate_y(-90.0f);
	greenBook4_ptr->rotate_z(180.0f);
	greenBook4_ptr->translate(36.0f, 2.8f, -22.5f);
	greenBook4_ptr->InOmbrabile = false;
	greenBook4_ptr->FaOmbra = false;
	world.add(greenBook4_ptr);
	auto greenBook5_ptr = make_shared<instance>(greenBook, greenBook_m);
	greenBook5_ptr->scale(1.7f, 1.5f, 1.7f);
	greenBook5_ptr->rotate_y(-90.0f);
	greenBook5_ptr->rotate_z(180.0f);
	greenBook5_ptr->translate(35.0f, 2.8f, -22.5f);
	greenBook5_ptr->InOmbrabile = true;
	greenBook5_ptr->FaOmbra = false;
	world.add(greenBook5_ptr);

	mesh* book2 = new mesh("models/book2.obj", "models/");
	texture* book_tex2 = new image_texture("models/book_tex2.jpg");
	material* book_m2 = new material();
	book_m2->texture = book_tex2;
	book_m2->ka = white;
	book_m2->ks = black;
	book_m2->alpha = 70;
	auto book_ptr2 = make_shared<instance>(book2, book_m2);
	book_ptr2->scale(2.5f, 2.5f, 2.5f);
	book_ptr2->rotate_y(-270.0f);
	book_ptr2->translate(38.0f, -9.0f, -22.0f);
	book_ptr2->InOmbrabile = false;
	book_ptr2->FaOmbra = false;
	world.add(book_ptr2);
	auto book2_ptr2 = make_shared<instance>(book2, book_m2);
	book2_ptr2->scale(2.5f, 2.5f, 2.5f);
	book2_ptr2->rotate_y(-270.0f);
	book2_ptr2->translate(37.0f, -9.0f, -22.0f);
	book2_ptr2->InOmbrabile = false;
	book2_ptr2->FaOmbra = false;
	world.add(book2_ptr2);
	auto book3_ptr2 = make_shared<instance>(book2, book_m2);
	book3_ptr2->scale(2.5f, 2.5f, 2.5f);
	book3_ptr2->rotate_y(-270.0f);
	book3_ptr2->translate(36.0f, -9.0f, -22.0f);
	book3_ptr2->InOmbrabile = false;
	book3_ptr2->FaOmbra = false;
	world.add(book3_ptr2);
	auto book4_ptr2 = make_shared<instance>(book2, book_m2);
	book4_ptr2->scale(2.5f, 2.5f, 2.5f);
	book4_ptr2->rotate_y(-270.0f);
	book4_ptr2->translate(35.0f, -9.0f, -22.0f);
	book4_ptr2->InOmbrabile = false;
	book4_ptr2->FaOmbra = false;
	world.add(book4_ptr2);
	auto book5_ptr2 = make_shared<instance>(book2, book_m2);
	book5_ptr2->scale(2.5f, 2.5f, 2.5f);
	book5_ptr2->rotate_y(-270.0f);
	book5_ptr2->translate(34.0f, -9.0f, -22.0f);
	book5_ptr2->InOmbrabile = true;
	book5_ptr2->FaOmbra = false;
	world.add(book5_ptr2);

	mesh* bookend = new mesh("models/bookend.obj", "models/");
	texture* bookend_tex = new image_texture("models/marmoGiallo.jpg");
	material* bookend_m = new material();
	bookend_m->texture = bookend_tex;
	bookend_m->ka = white;
	bookend_m->ks = black;
	bookend_m->alpha = 70;
	auto bookend_ptr = make_shared<instance>(bookend, bookend_m);
	bookend_ptr->scale(0.215f, 0.215f, 0.215f);
	bookend_ptr->rotate_y(-90.0f);
	bookend_ptr->translate(32.3f, -8.7f, -22.0f);
	bookend_ptr->InOmbrabile = true;
	bookend_ptr->FaOmbra = false;
	world.add(bookend_ptr);

	mesh* vase = new mesh("models/vase.obj", "models/");
	texture* vase_tex = new image_texture("models/vase_tex.jpg");
	material* vase_m = new material();
	vase_m->texture = vase_tex;
	vase_m->ka = white;
	vase_m->ks = white;
	vase_m->alpha = 70;
	auto vase_ptr = make_shared<instance>(vase, vase_m);
	vase_ptr->scale(0.8f, 0.8f, 0.8f);
	vase_ptr->translate(28.0f, 6.0f, -22.0f);
	vase_ptr->InOmbrabile = false;
	vase_ptr->FaOmbra = false;
	world.add(vase_ptr);

	mesh* chair = new mesh("models/chair.obj", "models/");
	texture* chair_tex = new image_texture("models/chairTexture.jpg");
	material* chair_m = new material();
	chair_m->texture = chair_tex;
	chair_m->ka = white;
	chair_m->ks = white;
	chair_m->alpha = 70;
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
	tv_m->alpha = 150;
	auto tv_ptr = make_shared<instance>(tv, tv_m);
	tv_ptr->rotate_y(-20.0f);
	tv_ptr->scale(12.0, 12.0, 12.0);
	tv_ptr->translate(-49.0f, -8.7f, -34.0f);
	tv_ptr->rotate_y(20.0f);
	tv_ptr->translate(80.0f, 0.0f, -5.0f);
	tv_ptr->InOmbrabile = true;
	tv_ptr->FaOmbra = true;
	world.add(tv_ptr);


	mesh* sideTableNativity = new mesh("models/tavolinoPresepe.obj", "models/");
	texture* sideTableNativity_tex = new image_texture("models/BaseColor.jpg");
	material* sideTableNativity_m = new material();
	sideTableNativity_m->texture = sideTableNativity_tex;
	sideTableNativity_m->ka = white;
	sideTableNativity_m->ks = lightgray;
	sideTableNativity_m->alpha = 150;
	auto sideTableNativity_ptr = make_shared<instance>(sideTableNativity, sideTableNativity_m);
	sideTableNativity_ptr->scale(1.5, 1.6, 2.0);
	sideTableNativity_ptr->scale(2.2, 2.2, 2.2);
	sideTableNativity_ptr->translate(-6.0f, -16.1f, 3.0f);
	sideTableNativity_ptr->translate(-7.0f, 1.88f, -2.0f);
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
	room_ptr->scale(1.9, 3.0, 3.0);
	room_ptr->translate(20.0f, 0.0f, 0.0f);
	room_ptr->translate(-6.0f, -14.5f, 15.0f);
	room_ptr->InOmbrabile = true;
	room_ptr->FaOmbra = true;
	world.add(room_ptr);

	mesh* chandelier = new mesh("models/chandelier.obj", "models/");
	auto instance_ptrChandelier = make_shared<instance>(chandelier, m_vetro_in_aria);
	instance_ptrChandelier->scale(29.0, 29.0, 29.0);
	/*instance_ptrChandelier->translate(0.5f, 0.0f, 0.7f);*/
	instance_ptrChandelier->translate(0.5f, 10.0f, -14.5f);
	instance_ptrChandelier->InOmbrabile = true;
	instance_ptrChandelier->FaOmbra = false;
	world.add(instance_ptrChandelier);

	// luci del candelabro
	// posizione nel sistema relativo
	/*point3 light_position_chandelier(0.0, 0.2, 0.0);
	light_position_chandelier = multiply(instance_ptrChandelier->getCMat(), light_position_chandelier);
	shared_ptr<point_light> punto_chandelier = make_shared<point_light>(light_position_chandelier, darkyellow, darkyellow, darkyellow);
	worldlight.add(punto_chandelier);*/

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
	//libreria
	/*cam.lookfrom = point3(20.0, 5.0, 20.0);
	cam.lookat = point3(20.0f, 0.0f, -20.f);*/

	//lontano di fronte, guarda centro
	/*cam.lookfrom = point3(0.0, 0.0, 55.0);
	cam.lookat = point3(10.0f, 0.0f, -0.15f);*/

	//guarda nativita
	cam.lookfrom = point3(0.0, -5.0, 30.0);
	cam.lookat = point3(0.0f, -5.0f, -0.15f);

	//guarda lampada
	/*cam.lookfrom = point3(47.0, 0.0, 40.0);
	cam.lookat = point3(47.0f, 0.0f, -0.15f);*/

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
