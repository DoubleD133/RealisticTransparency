#pragma once
#include <iostream>
#include <time.h>
#include <algorithm>
#include "texture.h"
#include "material.h"

class dielectric: public material {
public:

	float eta_in = 1.0, eta_out = 1.0, cos_angolo_critico_in = 0.0, cos_angolo_critico_out = 0.0;
	color c_in = color(1.0, 1.0, 1.0), c_out = color(1.0, 1.0, 1.0);

	void calcola_angolo_critico(void) {
		// eta = ni_over_nt
		// entro nella superfice => i=out, t=in
		float eta = eta_out / eta_in;
		float A = 1 - eta * eta;
		if (A < 0)
			cos_angolo_critico_in = 0.0;
		else
			cos_angolo_critico_in = sqrt(A);
		// esco dalla superfice => i=in, t=out
		eta = eta_in / eta_out;
		A = 1 - eta * eta;
		if (A < 0)
			cos_angolo_critico_out = 0.0;
		else
			cos_angolo_critico_out = sqrt(A);
	}



	color shading(const ray& r, light& light, hit_record& hr, point3& camera_pos, hittable_list& world, int depth) override;
};