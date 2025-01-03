#pragma once
#include <cmath>
#include "SDL.h"
#include "SDL_image.h"
#include "vec3.h"
#include "raster.h"
#include "common.h"

using namespace std;

class texture {
public:
	virtual color value(float u, float v, const point3& p) const = 0;
};

class constant_texture : public texture {
public:
	constant_texture() { }

	constant_texture(color c) : color(c) { }

	virtual color value(float u, float v, const point3& p) const {
		return color;
	}

	color color;
};

class checker_texture : public texture {
public:
	checker_texture() { }

	checker_texture(texture* t0, texture* t1) : even(t0), odd(t1) { }

	virtual color value(float u, float v, const point3& p) const {
		float sines = sin(2 * p[0]) * sin(2 * p[1]) * sin(2 * p[2]);
		if (sines < 0)
			return odd->value(u, v, p);
		else
			return even->value(u, v, p);
	}

	texture* odd;
	texture* even;
};

class image_texture : public texture {
public:
	image_texture(const char* filename) {
		surface = loadTexture(filename, imageWidth, imageHeight);
	}

	virtual color value(float u, float v, const point3& p) const;

	color* pixels;
	SDL_Surface* surface;
	int imageWidth, imageHeight;
};

color image_texture::value(float u, float v, const point3& p) const {
	int i = (u) * float(imageWidth);
	int j = (1.0f - v) * imageHeight - 0.001f;
	if (i < 0) i = 0;
	if (j < 0) j = 0;
	if (i > imageWidth - 1) i = imageWidth - 1;
	if (j > imageHeight - 1) j = imageHeight - 1;

	Uint32 value = getpixel(surface, i, j);

	float red = float((value >> 16) & 0xff) / 255.0f;
	float green = float((value >> 8) & 0xff) / 255.0f;
	float blue = float(value & 0xff) / 255.0f;

	return color(blue, green, red);
}

class image_texture_Cylindrical : public texture {
public:
	image_texture_Cylindrical(const char* filename) {
		surface = loadTexture(filename, imageWidth, imageHeight);
		hmin = -1.0;
		hmax = 1.0;
		h = 2.0;
	}
	image_texture_Cylindrical(const char* filename, float ymin, float ymax) {
		surface = loadTexture(filename, imageWidth, imageHeight);
		hmin = ymin;
		hmax = ymax;
		h = hmax - hmin;
		coordinatePreCalc = false;
	}
	image_texture_Cylindrical(const char* filename, point3 _vertice1, point3 _vertice2) : vertice1(_vertice1), vertice2(_vertice2) {
		surface = loadTexture(filename, imageWidth, imageHeight);

		v0 = vertice2 - vertice1;
		h = v0.length();
		v0 = unit_vector(v0);

		// costruisco un vettore non nullo ed ortogonale a v
		if (abs(v0.x()) >= abs(v0.z()) || abs(v0.y()) >= abs(v0.z()))
			w1 = unit_vector(vec3(v0.y(), -v0.x(), 0.0f));
		else
			w1 = unit_vector(vec3(v0.z(), 0.0f, -v0.x()));

		// essendo v e w1 ortonormali allora w2 è unitario
		w2 = cross(v0, w1);
		hmin = 0.0;
		hmax = h;
		coordinatePreCalc = false;
		asseY = false;
	}

	virtual color value(float u, float v, const point3& p) const override {
		if (!coordinatePreCalc) {
			float x, y, z, phi;
			if (asseY) {
				// angolo, in radianti (quindi in [-pi/2, pi * 3/2[), che
				// il vettore (x,0,z)-(0,0,0) forma con l'asse z
				// anomalia (una delle coordinate polari) del punto (x,0,z) nel piano xz
				// phi;
				x = p[0]; y = p[1]; z = p[2];
			}
			else {
				// angolo, in radianti (quindi in [-pi/2, pi * 3/2[), che
				// il vettore p-vertice1 forma con l'asse w2
				// anomalia (una delle coordinate polari) del punto p nel piano w1,w2
				// phi;
				x = dot(p - vertice1, w1); y = dot(p - vertice1, v0); z = dot(p - vertice1, w2);
			}
			if (z > 0)
				phi = atan(x / z);
			else if (z == 0) {
				if (x >= 0)
					phi = pi / 2;
				else
					phi = -pi / 2;
			}
			else
				phi = pi + atan(x / z);

			if (phi < -pi / 2 - 0.00001)
				cout << "surreale1\n";
			if (phi >= pi * 3 / 2 + 0.00001)
				cout << "surreale2\n";

			// coordinate cilindriche
			u = (phi + pi / 2) / (2 * pi);

			v = (y - hmin) / h;
		}
		

		int i = (u) * float(imageWidth);
		int j = (1-v) * imageHeight - 0.001f;
		if (i < 0) i = 0;
		if (j < 0) j = 0;
		if (i > imageWidth - 1) i = imageWidth - 1;
		if (j > imageHeight - 1) j = imageHeight - 1;

		Uint32 value = getpixel(surface, i, j);

		float red = float((value >> 16) & 0xff) / 255.0f;
		float green = float((value >> 8) & 0xff) / 255.0f;
		float blue = float(value & 0xff) / 255.0f;

		return color(blue, green, red);
	}

	color* pixels;
	SDL_Surface* surface;
	int imageWidth, imageHeight;
	float hmin, hmax, h;
	// booleano che ci dice se le coordinate cilindriche vengono calcolate in hit
	bool coordinatePreCalc = true;
	// booleano che ci dice se le coordinate cilindriche siano da calcolate per un
	// cilindro che ha per asse l'asse Y
	bool asseY = true;

	// centri basi
	point3 vertice1;
	point3 vertice2;
	// direzione apertura cono, di solito (0.0f, -1.0f, 0.0f)
	vec3 v0;
	// versori in modo che v,w1,w2 sia un sistema ortonormale
	vec3 w1;
	vec3 w2;
};

class image_texture_Spherical : public texture {
public:
	image_texture_Spherical(const char* filename) {
		surface = loadTexture(filename, imageWidth, imageHeight);
		center = point3(0.0, 0.0, 0.0);
	}
	image_texture_Spherical(const char* filename, bool flag) {
		surface = loadTexture(filename, imageWidth, imageHeight);
		center = point3(0.0, 0.0, 0.0);
		coordinatePreCalc = flag;
	}
	image_texture_Spherical(const char* filename, point3 c) {
		surface = loadTexture(filename, imageWidth, imageHeight);
		center = c;
		coordinatePreCalc = false;
		centro0 = false;
	}

	virtual color value(float u, float v, const point3& p) const override {
		if (!coordinatePreCalc) {
			float x, y, z;
			if (centro0) {
				// angolo, in radianti (quindi in [-pi/2, pi * 3/2[), che
				// il vettore (x,0,z)-(0,0,0) forma con l'asse z
				// anomalia (una delle coordinate polari) del punto (x,0,z) nel piano xz
				// phi;
				point3 q = unit_vector(p);
				x = q[0]; y = q[1]; z = q[2];
			}
			else {
				// angolo, in radianti (quindi in [-pi/2, pi * 3/2[), che
				// il vettore p-vertice1 forma con l'asse w2
				// anomalia (una delle coordinate polari) del punto p nel piano w1,w2
				// phi;
				point3 q = unit_vector(p - center);
				x = q[0]; y = q[1]; z = q[2];
			}
			float theta = acos(y), phi;

			if (theta <= 0.0001 || theta >= pi - 0.0001)
				phi = 0;
			else {
				float s = sin(theta);
				x = x / s;
				z = z / s;
				if (z > 0)
					phi = atan(x / z);
				else if (z == 0) {
					if (x >= 0)
						phi = pi / 2;
					else
						phi = -pi / 2;
				}
				else
					phi = pi + atan(x / z);
			}

			u = (phi + pi / 2) / (2 * pi);
			v = 1 - theta / pi;
		}


		int i = (u) * float(imageWidth);
		int j = (1 - v) * imageHeight - 0.001f;
		if (i < 0) i = 0;
		if (j < 0) j = 0;
		if (i > imageWidth - 1) i = imageWidth - 1;
		if (j > imageHeight - 1) j = imageHeight - 1;

		Uint32 value = getpixel(surface, i, j);

		float red = float((value >> 16) & 0xff) / 255.0f;
		float green = float((value >> 8) & 0xff) / 255.0f;
		float blue = float(value & 0xff) / 255.0f;

		return color(blue, green, red);
	}

	color* pixels;
	SDL_Surface* surface;
	int imageWidth, imageHeight;
	point3 center;
	// booleano che ci dice se le coordinate sferiche vengono calcolate in hit
	bool coordinatePreCalc = true;
	// booleano che ci dice se le coordinate sferiche siano da calcolate per una
	// sfera che ha per centro l'origine
	bool centro0 = true;
};
