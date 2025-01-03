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
	// parametri grande cilindro
	float abbassamento = 4.5;
	float h = 60.0;
	float hmin = - h / 2.0 - abbassamento;
	float raggio = 30.0;
	// parametri sfera
	float raggioS = 3.0;
	point3 centroS(0.0f, hmin + 33.0f / 60.0f * h, 0.0f);

	// World
	hittable_list world;
	list_light worldlight;

	// Texture Mapping preliminare
	color lightgray = color(0.75f, 0.75f, 0.75f);
	color lightHardGray = color(0.35f, 0.35f, 0.35f);
	color black = color(0.0f, 0.0f, 0.0f);
	//point3 light_position(0.0f, 25.0f, 0.0f);
	point3 light_position(0.0, h / 2.0 - abbassamento - 15.0, 0.0);
	shared_ptr<point_light> punto = make_shared<point_light>(light_position, lightHardGray, lightgray, lightgray);
	worldlight.add(punto);

	// nell'immagine ho un raggio luminoso che proviene dall'immagine di sfondo
	// l'immagine è 2060*1030. Il punto è nel pixel 1772,498
	// il passaggio da pixel a coordinate di texsture è
	// int i = (u) * float(imageWidth);
	// int j = (1 - v) * imageHeight - 0.001f;
	// passaggio inverso
	float u = 1772.0f / 2060.0f;
	float v = 1 - (498.0f + 0.001f) / 1030.0;
	// il passaggio da coordinate di texsture a coordinate cilindriche è
	// u = (phi + pi / 2) / (2 * pi);
	// v = (y - hmin) / h;
	// passaggio inverso
	float phi = u * 2 * pi - pi / 2;
	float y = h * v + hmin;
	// il passaggio da coordinate cilindriche a coordinate cartesiane è
	float r = raggio - 0.1;
	float x = r * sin(phi);
	float z = r * cos(phi);
	color lightYellow = color(0.75f, 0.75f, 0.0f);
	shared_ptr<spot_light> faretto = make_shared<spot_light>(point3(x, y, z),  point3(-x, -y, -z), 35.0f, 15, lightHardGray, lightYellow, lightYellow);
	faretto->att_b = 0.0001f;
	faretto->att_c = 0.0001f;
	worldlight.add(faretto);
	/*point3 CentroSfera = point3(0.0, 0.0, 0.0) + point3(0.0f, -1.5f, 0.0f);
	point3 PosFar2 = CentroSfera - 3.1 / r * (point3(x, y, z) - CentroSfera);
	shared_ptr<spot_light> faretto2 = make_shared<spot_light>(PosFar2, - point3(x + 0.005, y + 0.07, z - 0.005) + CentroSfera, 12.699f, 10, lightHardGray, lightYellow, lightYellow);
	faretto2->att_b = 0.0002f;
	faretto2->att_c = 0.00015f;
	worldlight.add(faretto2);*/

	// nell'immagine ho una luce diffusa che colpisce l'origine proveniendo dall'immagine di sfondo
	// l'immagine è 2060*1030. Il punto è nel pixel 1337,367
	// il passaggio da pixel a coordinate di texsture è
	// int i = (u) * float(imageWidth);
	// int j = (1 - v) * imageHeight - 0.001f;
	// passaggio inverso
	u = 1337.0f / 2060.0f;
	v = 1 - (367.0f + 0.001f) / 1030.0;
	float v2 = 1 - (538.0f + 0.001f) / 1030.0;
	// il passaggio da coordinate di texsture a coordinate cilindriche è
	// u = (phi + pi / 2) / (2 * pi);
	// v = (y - hmin) / h;
	// passaggio inverso
	phi = u * 2 * pi - pi / 2;
	y = h * v + hmin;
	float y2 = h * v2 + hmin;
	// il passaggio da coordinate cilindriche a coordinate cartesiane è
	r = raggio - 0.1;
	x = r * sin(phi);
	z = r * cos(phi);
	color lightWaterGreen = color(169.0f / 255.0f, 205.0f / 255.0f, 137.0f / 255.0f);
	shared_ptr<diffuse_light> bagliore = make_shared<diffuse_light>(centroS + vec3(-x, - y, -z), lightWaterGreen, lightWaterGreen, lightWaterGreen);
	worldlight.add(bagliore);


	// punto sopra la macchina
	point3 SuMacchina(-120.0, 400.0, 0.0);
	vec3 DirSuMacchina(1.00, -1.50, 0.00);
	vec3 DestraMacchina(0.0, 0.0, 1.0);
	point3 SuMacchinaRel;
	vec3 DirSuMacchinaRel;
	vec3 DestraMacchinaRel;

	//point3 posFaroAuto1 = point3(107.00, 30.50, 25.00);
	point3 posFaroAuto1 = point3(112.00, 32.50, 30.00);
	vec3 dirFaroAuto1 = point3(1.00, -2.00, 0.00);
	shared_ptr<spot_light> FaroAuto1 = make_shared<spot_light>(posFaroAuto1, dirFaroAuto1, 30.0f, 1.5, lightHardGray, lightYellow, lightYellow);
	FaroAuto1->att_a = 0.75f;
	FaroAuto1->att_b = 0.001f;
	FaroAuto1->att_c = 0.11f;
	worldlight.add(FaroAuto1);

	point3 posFaroAuto2 = point3(112.00, 32.50, -30.00);
	vec3 dirFaroAuto2 = point3(1.00, -2.00, 0.00);
	shared_ptr<spot_light> FaroAuto2 = make_shared<spot_light>(posFaroAuto2, dirFaroAuto2, 30.0f, 1.5, lightHardGray, lightYellow, lightYellow);
	FaroAuto1->att_a = 0.75f;
	FaroAuto2->att_b = 0.001f;
	FaroAuto2->att_c = 0.11f;
	worldlight.add(FaroAuto2);


	shared_ptr<spot_light> FaroAuto3 = make_shared<spot_light>(posFaroAuto1, dirFaroAuto1, 30.0f, 1.5, lightHardGray, lightYellow, lightYellow);
	FaroAuto3->att_a = 0.75f;
	FaroAuto3->att_b = 0.001f;
	FaroAuto3->att_c = 0.11f;
	worldlight.add(FaroAuto3);

	shared_ptr<spot_light> FaroAuto4 = make_shared<spot_light>(posFaroAuto2, dirFaroAuto2, 30.0f, 1.5, lightHardGray, lightYellow, lightYellow);
	FaroAuto4->att_a = 0.75f;
	FaroAuto4->att_b = 0.001f;
	FaroAuto4->att_c = 0.11f;
	worldlight.add(FaroAuto4);

	

	for (int i = 0; i < worldlight.size; i++) {
		worldlight.lights[i]->ambTex = true;
	}
	

	cilindroStandard* paesaggio = new cilindroStandard();
	paesaggio->normEst = false;
	image_texture_Cylindrical* Polo = new image_texture_Cylindrical("models/photo360.jpg");
	//image_texture_Cylindrical* Polo = new image_texture_Cylindrical("models/photo360.jpg", hmin, hmin + h);
	//image_texture_Cylindrical* Polo = new image_texture_Cylindrical("models/photo360.jpg", point3(0.0f, -30.0f - 4.5f, 0.0f), point3(0.0f, 30.0f - 4.5f, 0.0f));

	image_texture_Spherical* PoloSp = new image_texture_Spherical("models/photo360.jpg");
	//image_texture_Spherical* PoloSp = new image_texture_Spherical("models/photo360.jpg", point3(0.0f, -1.5f, 0.0f));

	sphere* sfera = new sphere();
	sfera->normEst = true;

	material* mPolo = new material();
	mPolo->texture = Polo;
	mPolo->kd = lightgray;
	mPolo->ka = lightgray;
	mPolo->ks = black;

	// materiale da aria a vetro
	dielectric* mPoloSp = new dielectric();
	mPoloSp->texture = NULL;
	color cf( pow(223.0f / 255.0f, 0.5), pow(241.0f / 255.0f, 0.5), pow(236.0f / 255.0f, 0.5));
	mPoloSp->kd = cf / 8.0; // = color(1.0, 1.0, 1.0);
	mPoloSp->ka = cf / 8.0; // = color(1.0, 1.0, 1.0);
	mPoloSp->ks = cf; // = color(1.0, 1.0, 1.0);
	mPoloSp->alpha = 180.0;
	mPoloSp->eta_in = 1.51; // vetro
	mPoloSp->eta_out = 1.0; // vuoto
	mPoloSp->c_out = color(1.0, 1.0, 1.0);
	mPoloSp->c_in = cf; // = color(1.0, 1.0, 1.0);
	mPoloSp->calcola_angolo_critico();
	
	// materiale da vetro a aria
	dielectric* mPoloSp2 = new dielectric();
	mPoloSp2->texture = NULL;
	mPoloSp2->kd = color(1.0, 1.0, 1.0) / 8.0;
	mPoloSp2->ka = color(1.0, 1.0, 1.0) / 8.0;
	mPoloSp2->ks = color(1.0, 1.0, 1.0);
	mPoloSp2->alpha = 180.0;
	mPoloSp2->eta_in = 1.0; // vuoto
	mPoloSp2->eta_out = 1.51; // vetro
	mPoloSp2->c_out = cf; // = color(1.0, 1.0, 1.0);
	mPoloSp2->c_in = color(1.0, 1.0, 1.0);
	mPoloSp2->calcola_angolo_critico();

	auto instance_ptr = make_shared<instance>(paesaggio, mPolo);
	instance_ptr->scale(raggio, h / 2.0, raggio);
	instance_ptr->translate(0.0f, - abbassamento, 0.0f);
	instance_ptr->InOmbrabile = false;
	instance_ptr->FaOmbra = false;
	world.add(instance_ptr);
	
	auto instance_ptr1 = make_shared<instance>(sfera, mPoloSp);
	instance_ptr1->scale(raggioS, raggioS, raggioS);
	instance_ptr1->translate(centroS);
	instance_ptr1->InOmbrabile = true;
	instance_ptr1->FaOmbra = true;
	world.add(instance_ptr1);

	float perc_aria = 80.0 / 100.0;
	auto instance_ptr1_dentro = make_shared<instance>(sfera, mPoloSp2);
	instance_ptr1_dentro->scale(raggioS * perc_aria, raggioS * perc_aria, raggioS * perc_aria);
	instance_ptr1_dentro->translate(centroS);
	instance_ptr1_dentro->InOmbrabile = true;
	instance_ptr1_dentro->FaOmbra = true;
	world.add(instance_ptr1_dentro);



	discoStandard* paesaggioSu = new discoStandard( - 1.0);
	paesaggioSu->normAlto = false;
	paesaggioSu->normSp = true;
	image_texture_Spherical* paesaggioSuTex = new image_texture_Spherical("models/photo360.jpg", point3(0.0f, -abbassamento -27.0, 0.0f));
	material* mDiscoSu = new material();
	mDiscoSu->texture = paesaggioSuTex;
	mDiscoSu->kd = lightgray;
	mDiscoSu->ka = lightgray;
	mDiscoSu->ks = black;
	auto instance_ptrSu = make_shared<instance>(paesaggioSu, mDiscoSu);
	instance_ptrSu->scale(raggio, 1.0 * raggio, raggio);
	instance_ptrSu->translate(0.0f, h / 2.0 -abbassamento - 10.0, 0.0f);
	instance_ptrSu->InOmbrabile = false;
	instance_ptrSu->FaOmbra = false;
	world.add(instance_ptrSu);



	discoStandard* paesaggioGiu = new discoStandard(1.0);
	paesaggioGiu->normAlto = true;
	paesaggioGiu->normSp = true;
	image_texture_Spherical* paesaggioGiuTex = new image_texture_Spherical("models/photo360.jpg", point3(0.0f, -abbassamento + 27.0, 0.0f));
	material* mDiscoGiu = new material();
	mDiscoGiu->texture = paesaggioGiuTex;
	mDiscoGiu->kd = lightgray;
	mDiscoGiu->ka = lightgray;
	mDiscoGiu->ks = black;
	auto instance_ptrGiu = make_shared<instance>(paesaggioGiu, mDiscoGiu);
	instance_ptrGiu->scale(raggio, 5.0*raggio, raggio);
	instance_ptrGiu->translate(0.0f, - h / 2.0 - abbassamento + 10.0, 0.0f);
	instance_ptrGiu->InOmbrabile = false;
	instance_ptrGiu->FaOmbra = false;
	world.add(instance_ptrGiu);



	mesh* model3d = new mesh("models/TeslaModelS.obj", "models/");
	texture* tesla_texture = new image_texture("models/TeslaModelS_Base_Color.jpg");

	material* m = new material();
	m->texture = tesla_texture;
	m->ka = lightgray;
	m->ks = lightgray;
	m->alpha = 150.0;

	instance* instance_ptr2 = new instance(model3d, m);
	instance_ptr2->scale(raggioS / 300.0f, raggioS / 300.0f, raggioS / 300.0f);
	instance_ptr2->rotate_y(-90.0f);
	instance_ptr2->translate(0.0f, raggioS * 2.9f / 3.0f, 0.0f);
	instance_ptr2->InOmbrabile = true;
	instance_ptr2->FaOmbra = true;
	// instance_ptr2 è una macchina che ruota intorno all'origiine
	// nel piano yz all'origine

	// instance_ptr3 è la macchina in rotazione intorno alla sfera
	auto instance_ptr3 = make_shared<instance>(instance_ptr2, m);
	instance_ptr3->rotate_x(-120.0f);
	instance_ptr3->rotate_y(90.0f);
	instance_ptr3->rotate_z(-45.0f);
	instance_ptr3->rotate_x(45.0f);
	instance_ptr3->translate(0.0f, centroS.y(), 0.0f);
	instance_ptr3->InOmbrabile = true;
	instance_ptr3->FaOmbra = true;


	// instance_ptr4 è la macchina in rotazione intorno alla sfera
	auto instance_ptr4 = make_shared<instance>(instance_ptr2, m);
	// indietreggio in modo che sia a favore di camera
	instance_ptr4->rotate_x(-120.0f);
	// voglio che sia più dietro rispetto all'altra istanza
	// per evitare lo scontro
	instance_ptr4->rotate_x(-60.0f);
	// perchè salga invece di scendere
	instance_ptr4->rotate_y(180.0f);
	// in modo che il moto sia obliquo
	instance_ptr4->rotate_z(-45.0f);
	instance_ptr4->rotate_x(45.0f);
	// in modo che giri intorno alla sfera S
	instance_ptr4->translate(0.0f, centroS.y(), 0.0f);
	instance_ptr4->InOmbrabile = true;
	instance_ptr4->FaOmbra = true;

	auto instance_ptr5 = make_shared<instance>(model3d, m);
	auto instance_ptr6 = make_shared<instance>(model3d, m);
	world.add(instance_ptr5);
	world.add(instance_ptr6);

	////// sferetta
	sphere* Death = new sphere();
	Death->normEst = true;

	image_texture_Spherical* Sp = new image_texture_Spherical("models/Death Star_Tex.jpg");
	material* mSp = new material();
	mSp->texture = Sp;
	mSp->kd = lightgray;
	mSp->ka = lightgray;
	mSp->ks = lightgray;
	mSp->alpha = 80.0;
	
	instance* instance_ptr7 = new instance(Death, mSp);
	instance_ptr7->scale(0.5f, 0.5f, 0.5f);
	instance_ptr7->translate(0.0f, raggioS * 3.6f / 3.0f, 0.0f);
	instance_ptr7->InOmbrabile = true;
	instance_ptr7->FaOmbra = true;
	// instance_ptr7 è una sfera piccola che ruota intorno all'origiine
	// nel piano yz all'origine

	// instance_ptr8 è la sfera piccola in rotazione intorno alla sfera
	// insegue la macchina
	auto instance_ptr8 = make_shared<instance>(instance_ptr7, mSp);
	instance_ptr8->rotate_x(-240.0f);
	instance_ptr8->rotate_y(90.0f);
	instance_ptr8->rotate_z(-45.0f);
	instance_ptr8->rotate_x(45.0f);
	instance_ptr8->translate(0.0f, centroS.y(), 0.0f);
	instance_ptr8->InOmbrabile = true;
	instance_ptr8->FaOmbra = true;

	auto instance_ptr9 = make_shared<instance>(Death, mSp);
	world.add(instance_ptr9);

	image_texture_Cylindrical* Oro = new image_texture_Cylindrical("models/lamina-oro.jpg");
	material* mOro = new material();
	mOro->texture = Oro;
	mOro->kd = lightgray;
	mOro->ka = lightgray;
	mOro->ks = lightgray;
	mOro->alpha = 250.0;

	cilindroStandard* Cil = new cilindroStandard();

	auto instance_ptr10 = make_shared<instance>(Cil, mOro);
	instance_ptr10->scale(0.7, 0.4, 0.7);
	instance_ptr10->translate(centroS);
	instance_ptr10->translate(0.0,0.15 + raggioS,0.0);
	instance_ptr10->InOmbrabile = true;
	instance_ptr10->FaOmbra = true;
	world.add(instance_ptr10);

	image_texture_Spherical* OroS = new image_texture_Spherical("models/lamina-oro-Sfera.jpg");
	material* mOroS = new material();
	mOroS->texture = OroS;
	mOroS->kd = lightgray;
	mOroS->ka = lightgray;
	mOroS->ks = lightgray;
	mOroS->alpha = 250.0;

	auto instance_ptr11 = make_shared<instance>(sfera, mOroS);
	instance_ptr11->scale(0.7, 0.4, 0.7);
	instance_ptr11->translate(centroS);
	instance_ptr11->translate(0.0, 0.15 + 0.4 + raggioS, 0.0);
	instance_ptr11->InOmbrabile = true;
	instance_ptr11->FaOmbra = true;
	world.add(instance_ptr11);

	auto instance_ptr12 = make_shared<instance>(Cil, mOro);
	instance_ptr12->scale(0.05, 1.0, 0.05);
	instance_ptr12->translate(centroS);
	instance_ptr12->translate(0.0, 0.4 + 0.15 + raggioS, 0.0);
	instance_ptr12->InOmbrabile = true;
	instance_ptr12->FaOmbra = true;
	world.add(instance_ptr12);

	auto instance_ptr13 = make_shared<instance>(sfera, mOroS);
	instance_ptr13->scale(0.05, 0.3, 0.05);
	instance_ptr13->translate(centroS);
	instance_ptr13->translate(0.0, + 1.0 + 0.4 + 0.15 + raggioS, 0.0);
	instance_ptr13->InOmbrabile = true;
	instance_ptr13->FaOmbra = true;
	world.add(instance_ptr13);

	camera cam;
	cam.lookfrom = point3(0.0, 0.0, - raggio * -14.5f / 30.0f);
	cam.lookat = point3(0.0f, 0.0f, 0.0f);

	cam.aspect_ratio = 16.0f / 9.0f;
	cam.image_width = 1500; // 1280;
	cam.samples_per_pixel = 64;
	cam.vfov = 60;

	cam.initialize();

	//if (init(cam.image_width, cam.image_height) == 1) {
	//	cout << "App Error! " << std::endl;
	//	return 1;
	//}

	//cout << "Image Resolution: " << cam.image_width << "x" << cam.image_height << "\n255\n";

	//time_t start, end;
	//time(&start);

	//cam.parallel_render(world, worldlight);
	////cam.render(world, *worldlight);

	//time(&end);
	//double dif = difftime(end, start);
	//cout << "\n" << "Rendering time: " << dif << "\n";

	//SDL_Event event;
	//bool quit = false;

	///* Poll for events */
	//while (SDL_PollEvent(&event) || (!quit)) {

	//	switch (event.type) {

	//	case SDL_QUIT:
	//		quit = true;
	//		break;

	//	case SDL_KEYDOWN:
	//		switch (event.key.keysym.sym) {
	//		case SDLK_ESCAPE:
	//			quit = true;
	//			break;
	//			// cases for other keypresses

	//		case SDLK_s:
	//			saveScreenshotBMP("screenshot.bmp");
	//			cout << "Screenshot saved!" << endl;
	//			break;
	//		}
	//	}
	//}

	if (init(cam.image_width, cam.image_height) == 1) {
		cout << "App Error! " << std::endl;
		return 1;
	}

	SDL_Event event;
	bool quit = false;



	int Nframe = 50;


	mat4 A = instance_ptr2->getCMat(), B = instance_ptr3->getCMat();
	mat4 E = B * A;
	SuMacchinaRel = multiply(E, SuMacchina);
	DirSuMacchinaRel = unit_vector(E * DirSuMacchina);
	vec3 lookatRel = SuMacchinaRel + DirSuMacchinaRel;
	DestraMacchinaRel = unit_vector(E * DestraMacchina);
	vec3 UPMacchinaRel = cross(DestraMacchinaRel, DirSuMacchinaRel);
	float aperturaSuMacchina = 45;

	std::vector<double> T1 = {6.0 / 8.0, 1.0 }; // must be increasing
	std::vector<double> X1 = { cam.vup.x(), UPMacchinaRel.x() };
	std::vector<double> Y1 = { cam.vup.y(), UPMacchinaRel.y() };
	std::vector<double> Z1 = { cam.vup.z(), UPMacchinaRel.z() };
	std::vector<double> X2 = { cam.lookat.x(), lookatRel.x() };
	std::vector<double> Y2 = { cam.lookat.y(), lookatRel.y() };
	std::vector<double> Z2 = { cam.lookat.z(), lookatRel.z() };
	std::vector<double> F1 = { cam.vfov, 45 };


	tk::spline sx1(T1, X1);
	tk::spline sy1(T1, Y1);
	tk::spline sz1(T1, Z1);
	tk::spline sx2(T1, X2);
	tk::spline sy2(T1, Y2);
	tk::spline sz2(T1, Z2);
	tk::spline sf1(T1, F1);

	int indiceRel = ceil(6.0 / 8.0 * double(Nframe - 1));
	std::vector<float> xx1, yy1, zz1, ff1, xx2, yy2, zz2;
	for (int frame = indiceRel; frame < Nframe; frame++) {
		double fattore = double(frame) / double(Nframe - 1);
		if (fattore >= 6.0 / 8.0) {
			xx1.push_back((float)(sx1(fattore)));
			yy1.push_back((float)(sy1(fattore)));
			zz1.push_back((float)(sz1(fattore)));
			ff1.push_back((float)(sf1(fattore)));
			xx2.push_back((float)(sx2(fattore)));
			yy2.push_back((float)(sy2(fattore)));
			zz2.push_back((float)(sz2(fattore)));
		}
	}


	std::vector<double> T = {   0.0, 1.0/8.0, 2.0/8.0, 3.0/8.0, 4.0/8.0, 5.0/8.0, 6.0/8.0, 7.0/8.0, 1.0}; // must be increasing
	std::vector<double> X = { -20.0,   -7.07,     0.0,    7.07,    20.0,    7.07,     0.0,   -7.07, SuMacchinaRel.x()};

	tk::spline sx(T, X);

	std::vector<double> Z = {   0.0,    7.07,    20.0,    7.07,     0.0,   -7.07,    -20.0,  -7.07, SuMacchinaRel.z()};

	tk::spline sz(T, Z);

	std::vector<double> H = { 1.5, 0.8, 1.5, 0.7, 1.5, 0.8, 1.5, 0.7, SuMacchinaRel.y() - cam.lookat.y()};

	tk::spline sh(T, H);

	std::vector<float> xx, hh, zz;
	for (int frame = 0; frame < Nframe; frame++) {
		double fattore = double(frame) / double(Nframe - 1);
		xx.push_back( (float)(sx(fattore)) );
		hh.push_back( (float)(sh(fattore)) );
		zz.push_back( (float)(sz(fattore)) );
	}

	point3 centroPercorso = cam.lookat;

	

	while (SDL_PollEvent(&event) || (!quit)) {
		cam.lookfrom = point3(0.0, 0.0, -raggio * -14.5f / 30.0f);
		cam.lookat = point3(0.0f, 0.0f, 0.0f);
		cam.vfov = 60;
		cam.vup = vec3(0.0, 1.0, 0.0);
		cam.initialize();

		float fattore;
		mat4 C, F;

		float angolo = 360.0 * 1.0 / float(Nframe - 1);
		instance_ptr2->rotate_x(-angolo);

		for (int frame = 0; frame < 2*Nframe; frame++) {
			double fattore = double(frame) / double(Nframe - 1);

			if (frame < Nframe) {
				cam.lookfrom = point3(xx[frame], centroPercorso.y() + hh[frame], zz[frame]);
				if (frame >= indiceRel) {
					cam.lookat = vec3(xx2[frame - indiceRel], yy2[frame - indiceRel], zz2[frame - indiceRel]);
					cam.vup = vec3(xx1[frame - indiceRel], yy1[frame - indiceRel], zz1[frame - indiceRel]);
					cam.vfov = ff1[frame - indiceRel];
				}

			}
			//cam.lookat = centroS + vec3(0.0, 15.5, 0.0);
			//cam.lookat = point3(0.5, 0, -1);

			/*
			cout << "frame " << frame + 1 << " di " << Nframe << endl;
			cout << "angolo = " << angolo * 180.0f / pi << " gradi\n";
			cout << "x = " << cam.lookfrom.x() << ", y = " << cam.lookfrom.y() << ", z = " << cam.lookfrom.z() << endl;
			*/

			instance_ptr2->rotate_x(angolo);

			A = instance_ptr2->getCMat(); B = instance_ptr3->getCMat(); C = instance_ptr4->getCMat();
			E = B * A; F = C * A;
			instance_ptr5->setCMat(E);
			instance_ptr6->setCMat(F);

			FaroAuto1->position = multiply(E, posFaroAuto1);
			FaroAuto1->dir = unit_vector(E * dirFaroAuto1);
			FaroAuto2->position = multiply(E, posFaroAuto2);
			FaroAuto2->dir = unit_vector(E * dirFaroAuto2);

			FaroAuto3->position = multiply(F, posFaroAuto1);
			FaroAuto3->dir = unit_vector(F * dirFaroAuto1);
			FaroAuto4->position = multiply(F, posFaroAuto2);
			FaroAuto4->dir = unit_vector(F * dirFaroAuto2);

			if (frame >= Nframe) {
				cam.lookfrom = multiply(E, SuMacchina);
				DirSuMacchinaRel = unit_vector(E * DirSuMacchina);
				cam.lookat = cam.lookfrom + DirSuMacchinaRel;
				DestraMacchinaRel = unit_vector(E * DestraMacchina);
				cam.vup = cross(DestraMacchinaRel, DirSuMacchinaRel);
				cam.vfov = 45;
			}

			A = instance_ptr2->getIMat(); B = instance_ptr3->getIMat(); C = instance_ptr4->getIMat();
			E = A * B; F = A * C;
			instance_ptr5->setIMat(E);
			instance_ptr6->setIMat(F);


			instance_ptr7->rotate_x(angolo);
			A = instance_ptr7->getCMat(); B = instance_ptr8->getCMat();
			E = B * A;
			instance_ptr9->setCMat(E);
			A = instance_ptr7->getIMat(); B = instance_ptr8->getIMat();
			E = A * B;
			instance_ptr9->setIMat(E);
			cam.initialize();
			//cam.render(world);
			cam.parallel_render(world, worldlight);
			SDL_RenderPresent(renderer);
			//saveScreenshotBMP("screenshotPallaVetro.bmp");
			//saveScreenshotBMP("screenshotSferaVetro.bmp");

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
					break;
				}
			}
		}
		close();
		return 0;
	}

	close();
	return 0;
}
