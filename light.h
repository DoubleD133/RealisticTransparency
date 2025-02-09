#ifndef LIST_LIGHT_H
#define LIST_LIGHT_H

#include "vec3.h"
#include "color.h"
#include "common.h"
#include "hittable_list.h"

#include "material.h"
#include "dielectric.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

const int DEEP_MAX = 5;

inline vec3 reflect(const vec3& i, const vec3& n)
{
	//return i - 2.0f * dot(i, n) * n;
	return  2.0f * dot(i, n) * n - i; // i vettore uscente dalla normale
}

class light {
public:

	color ambient;
	color diffuse;
	color specular;

	bool ambTex = false;

	float att_a = 1.0f;
	float att_b = 0.0f;
	float att_c = 0.0f;

	virtual color p_shading(hit_record& hr, point3& camera_pos, hittable_list& world) = 0;
	virtual color amb_shading(hit_record& hr, hittable_list& world) = 0;
};

class point_light : public light {
public:
	point3 position;

	point_light() {}
	point_light(point3 p, color a, color d, color s) {
		position = p;
		ambient = a;
		diffuse = d;
		specular = s;
	}

	virtual color p_shading(hit_record& hr, point3& camera_pos, hittable_list& world) override {

		vec3 L = position - hr.p;
		float lun = L.length();
		if (lun > 0.00001)
			L = unit_vector(L);

		ray shadow_ray(hr.p, L);
		float closest_light = lun;
		
		if (hr.InOmbrabile)
			if (world.hit_shadow(shadow_ray, interval(0.01f, closest_light)))
				return amb_shading(hr, world);
			

		color OutAmbient(0.0f, 0.0f, 0.0f);
		color OutDiffuse(0.0f, 0.0f, 0.0f);
		color OutSpecular(0.0f, 0.0f, 0.0f);

		if(ambTex && hr.m->texture != NULL)
			OutAmbient = hr.m->texture->value(hr.u, hr.v, hr.p) * ambient;
		else
			OutAmbient = hr.m->ka * ambient;


		float LDotN = max(dot(L, hr.normal), 0.0f);

		if (LDotN > 0.0f) {

			if (hr.m->texture == NULL)
				OutDiffuse = hr.m->kd * diffuse * LDotN;
			else
				OutDiffuse = hr.m->texture->value(hr.u, hr.v, hr.p) * diffuse * LDotN;

			vec3 R = reflect(L, hr.normal);

			vec3 V = camera_pos - hr.p;
			lun = V.length();
			if (lun > 0.00001)
				V = unit_vector(V);

			float VDotR = std::pow(max(dot(V, R), 0.0f), hr.m->alpha);

			OutSpecular = hr.m->ks * specular * VDotR;

			/*if (OutAmbient.length_squared() >= 0.1)
				cout << OutAmbient.length_squared() << " significa problemino1\n";
			if (OutDiffuse.length_squared() >= 0.1)
				cout << OutDiffuse.length_squared() << " significa problemino2\n";
			if (OutSpecular.length_squared() >= 0.1)
				cout << OutSpecular.length_squared() << " significa problemino3\n";*/

			float att = 1 / (att_a + att_b * closest_light + att_c * closest_light * closest_light);

			return OutAmbient + att * (OutDiffuse + OutSpecular);
		}
		else
			return OutAmbient;
	}

	virtual color amb_shading(hit_record& hr, hittable_list& world) override {
		if (ambTex && hr.m->texture != NULL)
			return hr.m->texture->value(hr.u, hr.v, hr.p) * ambient;
		else
			return hr.m->ka * ambient;
	}
};

class diffuse_light : public light {
public:
	vec3 direction;

	diffuse_light() {}
	diffuse_light(vec3 dir, color a, color d, color s) {
		direction = unit_vector(dir);
		ambient = a;
		diffuse = d;
		specular = s;
	}

	virtual color p_shading(hit_record& hr, point3& camera_pos, hittable_list& world) override {

		vec3 L = - direction;
		float lun = 1;

		ray shadow_ray(hr.p, L);
		float closest_light = lun;

		if (hr.InOmbrabile)
			if (world.hit_shadow(shadow_ray, interval(0.01f, infinity)))
				return amb_shading(hr, world);


		color OutAmbient(0.0f, 0.0f, 0.0f);
		color OutDiffuse(0.0f, 0.0f, 0.0f);
		color OutSpecular(0.0f, 0.0f, 0.0f);

		if (ambTex && hr.m->texture != NULL)
			OutAmbient = hr.m->texture->value(hr.u, hr.v, hr.p) * ambient;
		else
			OutAmbient = hr.m->ka * ambient;


		float LDotN = max(dot(L, hr.normal), 0.0f);

		if (LDotN > 0.0f) {

			if (hr.m->texture == NULL)
				OutDiffuse = hr.m->kd * diffuse * LDotN;
			else
				OutDiffuse = hr.m->texture->value(hr.u, hr.v, hr.p) * diffuse * LDotN;

			vec3 R = reflect(L, hr.normal);

			vec3 V = camera_pos - hr.p;
			lun = V.length();
			if (lun > 0.00001)
				V = unit_vector(V);

			float VDotR = std::pow(max(dot(V, R), 0.0f), hr.m->alpha);

			OutSpecular = hr.m->ks * specular * VDotR;

			/*if (OutAmbient.length_squared() >= 0.1)
				cout << OutAmbient.length_squared() << " significa problemino1\n";
			if (OutDiffuse.length_squared() >= 0.1)
				cout << OutDiffuse.length_squared() << " significa problemino2\n";
			if (OutSpecular.length_squared() >= 0.1)
				cout << OutSpecular.length_squared() << " significa problemino3\n";*/

			float att = 1 / (att_a + att_b * closest_light + att_c * closest_light * closest_light);

			return OutAmbient + att * (OutDiffuse + OutSpecular);
		}
		else
			return OutAmbient;
	}

	virtual color amb_shading(hit_record& hr, hittable_list& world) override {
		if (ambTex && hr.m->texture != NULL)
			return hr.m->texture->value(hr.u, hr.v, hr.p) * ambient;
		else
			return hr.m->ka * ambient;
	}
};

class spot_light : public light {
public:

	spot_light() {}
	spot_light(point3 p, color a, color d, color s) {
		position = p;
		ambient = a;
		diffuse = d;
		specular = s;

		theta = 180;
		esp = 0;
		dir = point3(0.0f, 0.0f, -1.0f);
		cosTheta = cos(degrees_to_radians(theta));
	}

	spot_light(point3 p, vec3 Dir, float angolo, color a, color d, color s) {
		position = p;
		ambient = a;
		diffuse = d;
		specular = s;

		if (0 > angolo)
			theta = 0;
		else if (angolo > 180)
			theta = 180;
		else
			theta = angolo;

		dir = unit_vector(Dir);
		esp = 1;
		cosTheta = cos(degrees_to_radians(theta));
	}

	spot_light(point3 p, vec3 Dir, float angolo, float e, color a, color d, color s) {
		position = p;
		ambient = a;
		diffuse = d;
		specular = s;

		if (0 > angolo)
			theta = 0;
		else if (angolo > 180)
			theta = 180;
		else
			theta = angolo;

		dir = unit_vector(Dir);
		esp = e;
		cosTheta = cos(degrees_to_radians(theta));
	}

	color p_shading(hit_record& hr, point3& camera_pos, hittable_list& world) override {

		vec3 L = position - hr.p;
		float lun = L.length();
		if (lun > 0.00001)
			L = unit_vector(L);

		ray shadow_ray(hr.p, L);
		float closest_light = lun;


		if (hr.InOmbrabile)
			if (world.hit_shadow(shadow_ray, interval(0.01f, closest_light)))
				return amb_shading(hr, world);

		color OutAmbient(0.0f, 0.0f, 0.0f);
		color OutDiffuse(0.0f, 0.0f, 0.0f);
		color OutSpecular(0.0f, 0.0f, 0.0f);

		float attenuazione;
		float DirPosDotDir;
		vec3 DirPos = -L;

		DirPosDotDir = dot(DirPos, dir);
		if (cosTheta <= DirPosDotDir)
			attenuazione = pow(DirPosDotDir, esp);
		else
			attenuazione = 0;

		color InAmbient = ambient;
		color InDiffuse = diffuse * attenuazione;
		color InSpecular = specular * attenuazione;

		if(ambTex && hr.m->texture != NULL)
			OutAmbient = hr.m->texture->value(hr.u, hr.v, hr.p) * InAmbient;
		else
			OutAmbient = hr.m->ka * InAmbient;

		float LDotN = max(dot(L, hr.normal), 0.0f);

		if (LDotN > 0.0f) {
			if (hr.m->texture == NULL)
				OutDiffuse = hr.m->kd * InDiffuse * LDotN;
			else
				OutDiffuse = hr.m->texture->value(hr.u, hr.v, hr.p) * InDiffuse * LDotN;

			vec3 R = reflect(L, hr.normal);

			vec3 V = camera_pos - hr.p;
			lun = V.length();
			if (lun > 0.00001)
				V = unit_vector(V);

			float VDotR = std::pow(max(dot(V, R), 0.0f), hr.m->alpha);

			OutSpecular = hr.m->ks * InSpecular * VDotR;

			/*if (OutAmbient.length_squared() >= 0.1)
				cout << OutAmbient.length_squared() << " significa problemino1\n";
			if (OutDiffuse.length_squared() >= 0.1)
				cout << OutDiffuse.length_squared() << " significa problemino2\n";
			if (OutSpecular.length_squared() >= 0.1)
				cout << OutSpecular.length_squared() << " significa problemino3\n";*/

			float att = 1 / (att_a + att_b * closest_light + att_c * closest_light * closest_light);

			return OutAmbient + att * (OutDiffuse + OutSpecular);
		}
		else
			return OutAmbient;
	}

	color amb_shading(hit_record& hr, hittable_list& world) override {
		if(ambTex && hr.m->texture != NULL)
			return hr.m->texture->value(hr.u, hr.v, hr.p) * ambient;
		else
			return hr.m->ka * ambient;
	}

	point3 position; // apice cono
	float esp;
	vec3 dir; // direzione cono
	float theta; // semi-ampiezza cono
	float cosTheta;
};

class list_light : public light {
public:

	std::vector<shared_ptr<light>> lights;
	bool isList = true;

	list_light() {}

	list_light(shared_ptr<light> object) { add(object); }

	void clear() {
		size = 0;
		lights.clear();
	}

	void add(shared_ptr<light> object) {
		size++;
		lights.push_back(object);
	}

	color p_shading(hit_record& hr, point3& camera_pos, hittable_list& world) override {
		int i;
		color OutAmbient(0.0f, 0.0f, 0.0f);
		color ColorSummer(0.0f, 0.0f, 0.0f);

		for (i = 0; i < size; i++) {
			OutAmbient += lights[i]->amb_shading(hr, world);
			ColorSummer += lights[i]->p_shading(hr, camera_pos, world);
		}
		ColorSummer = ColorSummer - OutAmbient;
		OutAmbient = OutAmbient / size;
		return ColorSummer + OutAmbient;
	}

	color amb_shading(hit_record& hr, hittable_list& world) override {
		int i;
		color OutAmbient(0.0f, 0.0f, 0.0f);

		for (i = 0; i < size; i++) {
			OutAmbient += lights[i]->amb_shading(hr, world);
		}
		return OutAmbient / size;
	}

	int size = 0;
};


color material::shading(const ray& r, light& light, hit_record& hr, point3& camera_pos, hittable_list& world, int depth) {
	return light.p_shading(hr, camera_pos, world);
}

float fresnel(hit_record& hr, float eta_in, float eta_out) {
	float eta;
	if (hr.front_face)
		eta = eta_in / eta_out;
	else
		eta = eta_out / eta_in;

	float cos_theta_i = hr.d_dot_n;
	float temp = 1.0 - (1.0 - cos_theta_i * cos_theta_i) / (eta * eta);
	if (temp <= 0)
		return 1.0;
	float cos_theta_t = sqrt(temp);
	float r_parallel = (eta * cos_theta_i - cos_theta_t) / (eta * cos_theta_i + cos_theta_t);
	float r_perpendicular = (cos_theta_i - eta * cos_theta_t) / (cos_theta_i + eta * cos_theta_t);
	return 0.5 * (r_parallel * r_parallel + r_perpendicular * r_perpendicular);
}

color ray_color(const ray& r, hittable_list& world, light& worldlight, point3& camera_pos, float &t, int depth) {
	if (depth > DEEP_MAX) {
		return color(0.0, 0.0, 0.0);
	}

	hit_record rec;

	// 0.0001 può essere ulteriormente aumentato se si hanno problemi con gli obj
	if (world.hit(r, interval(0.0001, infinity), rec)) {
		t = rec.t;
		return rec.m->shading(r, worldlight, rec, camera_pos, world, depth);
	}

	return color(1.0, 1.0, 1.0);
	vec3 unit_direction = unit_vector(r.direction());
	float tt = 0.5f * (unit_direction.y() + 1.0f);
	//return lerp(color(1.0f, 1.0f, 1.0f), color(0.5f, 0.7f, 1.0f), t);
	return (1.0f - tt) * color(1.0f, 1.0f, 1.0f) + tt * color(0.5f, 0.7f, 1.0f);
}

inline vec3 refract(const vec3& v, const vec3& n, float ni_over_nt) {
	vec3 d = unit_vector(v);
	float cos_thetai = dot(d, n);
	float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1.0f - cos_thetai * cos_thetai);
	return ni_over_nt * (n * cos_thetai - d) - n * sqrt(discriminant);
}

color dielectric::shading(const ray& r, light& light, hit_record& hr, point3& camera_pos, hittable_list& world, int depth) {
	color L= light.p_shading(hr, camera_pos, world);
	if (isnan(L.x()) || isnan(L.y()) || isnan(L.z()))
		L = color(0.0, 0.0, 0.0);
	vec3 wi, wo, wt;
	float len = r.d.length();
	wi = -r.d / len;
	hr.d_dot_n = dot(wi, hr.normal);
	wo = reflect(wi, hr.normal);

	float kr = fresnel(hr, eta_in, eta_out);
	color fr = ks * kr;
	ray reflected_ray(hr.p, wo);
	float t;
	color Lr, Lt;

	if (kr == 1.0) { // riflessione totale
		if (hr.front_face) {
			Lr = ray_color(reflected_ray, world, light, camera_pos, t, depth + 1);
			L += color(pow(c_out[0], t) * Lr[0], pow(c_out[1], t) * Lr[1], pow(c_out[2], t) * Lr[2]);
			//L = L / 2.0;
		}
		else {
			Lr = ray_color(reflected_ray, world, light, camera_pos, t, depth + 1);
			L += color(pow(c_in[0], t) * Lr[0], pow(c_in[1], t) * Lr[1], pow(c_in[2], t) * Lr[2]);
			//L = L / 2.0;
		}
	}
	else {
		float ni_over_nt;
		if (hr.front_face) {
			// colpiano la superfice dall'esterno passando da out a in
			// dunque l'eta di partenza è eta_out e l'eta di arrivo è eta_in
			// ni = eta_out, nt = eta_in => ni_over_nt = eta_out / eta_in
			ni_over_nt = eta_out / eta_in;
		} else 
			ni_over_nt = eta_in / eta_out;
		wt = refract(wi, hr.normal, ni_over_nt);
		float kt = 1-kr;
		color ft(1.0, 1.0, 1.0);
		float ndotwt = dot(hr.normal, wt);
		ft = ft * kt * ni_over_nt * ni_over_nt;
		ray trasmitted_ray(hr.p, wt);

		if (hr.front_face) {
			Lr = ray_color(reflected_ray, world, light, camera_pos, t, depth + 1);
			L += color(pow(c_out[0], t) * Lr[0] * fr[0], pow(c_out[1], t) * Lr[1] * fr[1], pow(c_out[2], t) * Lr[2] * fr[2]);
			Lt = ray_color(trasmitted_ray, world, light, camera_pos, t, depth + 1);
			L += color(pow(c_in[0], t) * Lt[0] * ft[0], pow(c_in[1], t) * Lt[1] * ft[1], pow(c_in[2], t) * Lt[2] * ft[2]);
		}
		else {
			Lr = ray_color(reflected_ray, world, light, camera_pos, t, depth + 1);
			L += color(pow(c_in[0], t) * Lr[0] * fr[0], pow(c_in[1], t) * Lr[1] * fr[1], pow(c_in[2], t) * Lr[2] * fr[2]);
			Lt = ray_color(trasmitted_ray, world, light, camera_pos, t, depth + 1);
			L += color(pow(c_out[0], t) * Lt[0] * ft[0], pow(c_out[1], t) * Lt[1] * ft[1], pow(c_out[2], t) * Lt[2] * ft[2]);
		}
	}


	return L;
}

class specchio : public material {
public:
	color c_in = color(1.0, 1.0, 1.0), c_out = color(1.0, 1.0, 1.0);

	color shading(const ray& r, light& light, hit_record& hr, point3& camera_pos, hittable_list& world, int depth) override {
		color L = light.p_shading(hr, camera_pos, world);
		vec3 wi, wo;
		float len = r.d.length();
		wi = -r.d / len;
		hr.d_dot_n = dot(wi, hr.normal);
		wo = reflect(wi, hr.normal);

		float t;
		color fr = ks;
		ray reflected_ray(hr.p, wo);
		color Lr;

		if (hr.front_face) {
			Lr = ray_color(reflected_ray, world, light, camera_pos, t, depth + 1);
			L += color(pow(c_out[0], t) * Lr[0], pow(c_out[1], t) * Lr[1], pow(c_out[2], t) * Lr[2]);
		}
		else {
			Lr = ray_color(reflected_ray, world, light, camera_pos, t, depth + 1);
			L += color(pow(c_in[0], t) * Lr[0], pow(c_in[1], t) * Lr[1], pow(c_in[2], t) * Lr[2]);
		}
		return L;
	}
};

#endif

