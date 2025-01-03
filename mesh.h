#pragma once
#include <vector>
#include <cstdio>
#include <iostream>
#include "point2.h"
#include "vec3.h"
#include "aabb.h"
#include "ray.h"
#include "hittable.h"
#include "interval.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define TINYBVH_IMPLEMENTATION
#include "tiny_bvh.h"

using namespace std;

class mesh : public hittable {
public:

	vector<point3> 				vertices;				// mesh vertices 
	vector<vec3> 					normals;				// average normal at each vertex;
	vector<point2>				textures;
	vector<vector<int> > 	vertex_faces;		// the triangles shared by each vertex
	int num_vertices; 										// number of vertices
	int num_normals;
	int num_textures;
	int num_shapes;
	int num_faces;
	aabb aabb_mesh;
	bool UsaBVH = true;
	tinybvh::bvhvec4* triangles;// [70576 * 3] ;
	tinybvh::BVH bvh;

	bool areNormals = false;

	mesh(const char* filename, const char* basepath = NULL, bool triangulate = true) {
		load_mesh(filename, basepath, triangulate);
	};

	bool load_mesh(const char* filename, const char* basepath, bool triangulate);

	virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const;
	virtual bool hit_shadow(const ray& r, interval ray_t) const;

	virtual bool bounding_box(aabb& box) const;
};

bool mesh::bounding_box(aabb& box) const {
	box = aabb_mesh;
	return true;
}

bool mesh::load_mesh(const char* filename, const char* basepath = NULL, bool triangulate = true)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	std::string war;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &war, &err, filename, basepath, triangulate);

	if (!err.empty()) {
		std::cerr << err << std::endl;
		return false;
	}

	if (!ret) {
		printf("Failed to load/parse .obj.\n");
		return false;
	}

	num_vertices = (attrib.vertices.size() / 3);
	num_normals = (attrib.normals.size() / 3);
	num_textures = (attrib.texcoords.size() / 2);
	num_shapes = shapes.size();

	aabb_mesh.x.min = aabb_mesh.y.min = aabb_mesh.z.min = FLT_MIN;
	aabb_mesh.x.max = aabb_mesh.y.max = aabb_mesh.z.max = FLT_MAX;;

	for (int v = 0; v < attrib.vertices.size() / 3; v++) {
		point3 p = point3(attrib.vertices[3 * v + 0], attrib.vertices[3 * v + 1], attrib.vertices[3 * v + 2]);

		aabb_mesh.x.min = ffmin(p[0], aabb_mesh.x.min);
		aabb_mesh.y.min = ffmin(p[1], aabb_mesh.y.min);
		aabb_mesh.z.min = ffmin(p[2], aabb_mesh.z.min);
		aabb_mesh.x.max = ffmax(p[0], aabb_mesh.x.max);
		aabb_mesh.y.max = ffmax(p[1], aabb_mesh.y.max);
		aabb_mesh.z.max = ffmax(p[2], aabb_mesh.z.max);

		vertices.push_back(p);
	}

	if (attrib.normals.size() > 0)
	{
		areNormals = true;
		for (int v = 0; v < attrib.normals.size() / 3; v++) {
			normals.push_back(vec3(attrib.normals[3 * v + 0], attrib.normals[3 * v + 1], attrib.normals[3 * v + 2]));
		}
	}

	if (attrib.texcoords.size() > 0)
	{
		for (int v = 0; v < attrib.texcoords.size() / 2; v++) {
			textures.push_back(point2(attrib.texcoords[2 * v + 0], attrib.texcoords[2 * v + 1]));
		}
	}

	vertex_faces.push_back(vector<int>());
	vertex_faces.push_back(vector<int>());
	vertex_faces.push_back(vector<int>());

	num_faces = 0;
	// For each shape
	for (int s = 0; s < shapes.size(); s++) {
		int index_offset = 0;

		// For each face
		for (int f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			int fv = shapes[s].mesh.num_face_vertices[f];
			num_faces++;

			// For each vertex in the face
			for (int v = 0; v < fv; v++) {
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

				vertex_faces[0].push_back(idx.vertex_index);
				vertex_faces[1].push_back(idx.normal_index);
				vertex_faces[2].push_back(idx.texcoord_index);
			}

			index_offset += fv;
		}
	}

	if (UsaBVH) {

		//creo i triangoli corrispondenti alle facce salvate nel vettore vertex_faces 
		// e li salvo nel vettore triangles
		//poi creo il bvh a partire da quella lista
		//passo ai vari triangoli anche le normali ai vertici, se presenti

		cout << "Numero di figure: " << num_shapes << "\n" << endl;
		cout << "Numero di triangoli: " << num_faces << "\n" << endl;
		cout << "Numero di vertici: " << num_vertices << "\n" << endl;
		cout << "Numero di normali: " << num_normals << "\n" << endl;

		triangles = new tinybvh::bvhvec4[num_faces*3];

		for (int i = 0; i < num_faces; i++) {
			int i0 = vertex_faces[0][3 * i + 0];
			int i1 = vertex_faces[0][3 * i + 1];
			int i2 = vertex_faces[0][3 * i + 2];

			point3 v0 = vertices[i0];
			point3 v1 = vertices[i1];
			point3 v2 = vertices[i2];

			triangles[3 * i + 0].x = v0.x();
			triangles[3 * i + 0].y = v0.y();
			triangles[3 * i + 0].z = v0.z();
			triangles[3 * i + 1].x = v1.x();
			triangles[3 * i + 1].y = v1.y();
			triangles[3 * i + 1].z = v1.z();
			triangles[3 * i + 2].x = v2.x();
			triangles[3 * i + 2].y = v2.y();
			triangles[3 * i + 2].z = v2.z();

			//cout << "Creato triangolo n. " << i << ":\n\t" << t->to_string() << "\n******\n" << endl;
		}

		bvh.Build(triangles, num_faces);



	}

	return true;
}


bool triangle_intersection(const ray& r, float tmin, float tmax, hit_record& rec, const point3& v0, const point3& v1, const point3& v2, float& u, float& v)
{
	const float EPSILON = 0.0000001f;
	vec3 edge1, edge2, h, s, q;
	float a, f;	// , u, v;
	edge1 = v1 - v0;
	edge2 = v2 - v0;
	h = cross(r.d, edge2);
	a = dot(edge1, h);
	if (a > -EPSILON && a < EPSILON)
		return false;
	f = 1 / a;
	s = r.o - v0;
	u = f * (dot(s, h));
	if (u < 0.0f || u > 1.0f)
		return false;
	q = cross(s, edge1);
	v = f * dot(r.d, q);
	if (v < 0.0f || u + v > 1.0f)
		return false;
	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = f * dot(edge2, q);
	if (t > tmin && t < tmax) {
		if (t > EPSILON) // ray intersection
		{
			rec.normal = unit_vector(cross(v1 - v0, v2 - v0));
			rec.t = dot((v0 - r.o), rec.normal) / dot(r.direction(), rec.normal);
			rec.p = r.at(rec.t);
			return true;
		}
	}
	// This means that there is a line intersection but not a ray intersection.
	return false;
}


bool mesh::hit(const ray& ray, interval ray_t, hit_record& rec) const
{
	bool hit_anything = false;
	hit_record temp_rec;
	float closest_so_far = ray_t.max;
	float u, v;

	// Check intersection with bound boux
	if (aabb_mesh.hit(ray, ray_t) == false)
		return false;

	if (UsaBVH) {
		// from here: play with the BVH!

		float lungDir = ray.d.length();
		point3 dir = ray.d / lungDir;
		tinybvh::bvhvec3 O(ray.o[0], ray.o[1], ray.o[2]);
		tinybvh::bvhvec3 D(dir[0], dir[1], dir[2]);
		tinybvh::Ray r(O, D);
		bvh.Intersect(r);
		int primIdx = r.hit.prim;
		if (r.hit.t > 10000)
			return false;


		tinybvh::bvhvec4 vet0 = triangles[3 * primIdx + 0];
		tinybvh::bvhvec4 vet1 = triangles[3 * primIdx + 1];
		tinybvh::bvhvec4 vet2 = triangles[3 * primIdx + 2];

		int i0, i1, i2;

		/*int i0 = vertex_faces[0][3 * primIdx + 0];
		int i1 = vertex_faces[0][3 * primIdx + 1];
		int i2 = vertex_faces[0][3 * primIdx + 2];*/

		point3 v0(vet0.x, vet0.y, vet0.z);
		point3 v1(vet1.x, vet1.y, vet1.z);
		point3 v2(vet2.x, vet2.y, vet2.z);

		if ((v2 - vertices[vertex_faces[0][3 * primIdx + 2]]).length_squared() >= 0.00000001)
			cout << "problema vertici\n";

		float t = r.hit.t / lungDir;
		if (t <= ray_t.min || t >= ray_t.max)
			return false;

		vec3 bary(1.0f - r.hit.u - r.hit.v, r.hit.u, r.hit.v);
		//vec3 bary(r.hit.u, r.hit.v, 1.0f - r.hit.u - r.hit.v);
		if (areNormals)
			rec.normal = unit_vector(cross(v1 - v0, v2 - v0));
		else {
			i0 = vertex_faces[1][3 * primIdx + 0];
			i1 = vertex_faces[1][3 * primIdx + 1];
			i2 = vertex_faces[1][3 * primIdx + 2];

			rec.normal = unit_vector(normals[i0] * bary[0] + normals[i1] * bary[1] + normals[i2] * bary[2]);
		}
		rec.t = t;
		rec.p = ray.at(rec.t);

		
		i0 = vertex_faces[2][3 * primIdx + 0];
		i1 = vertex_faces[2][3 * primIdx + 1];
		i2 = vertex_faces[2][3 * primIdx + 2];

		point2 uv0 = textures[i0];
		point2 uv1 = textures[i1];
		point2 uv2 = textures[i2];

		point2 uv = uv0 * bary[0] + uv1 * bary[1] + uv2 * bary[2];
		rec.u = uv.x;
		rec.v = uv.y;


		/*if (triangle_intersection(ray, ray_t.min, closest_so_far, temp_rec, v0, v1, v2, u, v)) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;

			if (abs(r.hit.t / lungDir - rec.t) >= 0.001)
				cout << "caso ancora più strano, r.hit.t / lungDir = " << r.hit.t / lungDir << ", rec.t = " << rec.t << endl;

			vec3 bary(1.0f - u - v, u, v);

			if (areNormals)
			{
				i0 = vertex_faces[1][3 * primIdx + 0];
				i1 = vertex_faces[1][3 * primIdx + 1];
				i2 = vertex_faces[1][3 * primIdx + 2];

				rec.normal = unit_vector(normals[i0] * bary[0] + normals[i1] * bary[1] + normals[i2] * bary[2]);
			}

			i0 = vertex_faces[2][3 * primIdx + 0];
			i1 = vertex_faces[2][3 * primIdx + 1];
			i2 = vertex_faces[2][3 * primIdx + 2];

			point2 uv0 = textures[i0];
			point2 uv1 = textures[i1];
			point2 uv2 = textures[i2];

			point2 uv = uv0 * bary[0] + uv1 * bary[1] + uv2 * bary[2];
			rec.u = uv.x;
			rec.v = uv.y;
		}
		else {
			cout << "caso strano" << endl;
		}*/

		return true;
	}

	for (int i = 0; i < num_faces; i++)
	{
		int i0 = vertex_faces[0][3 * i + 0];
		int i1 = vertex_faces[0][3 * i + 1];
		int i2 = vertex_faces[0][3 * i + 2];

		point3 v0 = vertices[i0];
		point3 v1 = vertices[i1];
		point3 v2 = vertices[i2];

		if (triangle_intersection(ray, ray_t.min, closest_so_far, temp_rec, v0, v1, v2, u, v)) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;

			vec3 bary(1.0f - u - v, u, v);

			if (areNormals)
			{
				i0 = vertex_faces[1][3 * i + 0];
				i1 = vertex_faces[1][3 * i + 1];
				i2 = vertex_faces[1][3 * i + 2];

				rec.normal = unit_vector(normals[i0] * bary[0] + normals[i1] * bary[1] + normals[i2] * bary[2]);
			}

			i0 = vertex_faces[2][3 * i + 0];
			i1 = vertex_faces[2][3 * i + 1];
			i2 = vertex_faces[2][3 * i + 2];

			point2 uv0 = textures[i0];
			point2 uv1 = textures[i1];
			point2 uv2 = textures[i2];

			point2 uv = uv0 * bary[0] + uv1 * bary[1] + uv2 * bary[2];
			rec.u = uv.x;
			rec.v = uv.y;
		}
	}
	return hit_anything;
}

bool mesh::hit_shadow(const ray& ray, interval ray_t) const
{
	hit_record temp_rec;
	float closest_so_far = ray_t.max;
	float u, v;

	if (aabb_mesh.hit(ray, ray_t) == false)
		return false;

	if (UsaBVH) {
		const float EPSILON = 0.0000001f;
		// from here: play with the BVH!

		float lungDir = ray.d.length();
		point3 dir = ray.d / lungDir;
		tinybvh::bvhvec3 O(ray.o[0], ray.o[1], ray.o[2]);
		tinybvh::bvhvec3 D(dir[0], dir[1], dir[2]);
		tinybvh::Ray r(O, D);
		bvh.Intersect(r);
		if (r.hit.t > 10000)
			return false;

		float t = r.hit.t / lungDir;
		if (t <= ray_t.min || t >= ray_t.max || t<= EPSILON)
			return false;

		return true;
	}

	for (int i = 0; i < num_faces; i++)
	{
		int i0 = vertex_faces[0][3 * i + 0];
		int i1 = vertex_faces[0][3 * i + 1];
		int i2 = vertex_faces[0][3 * i + 2];

		point3 v0 = vertices[i0];
		point3 v1 = vertices[i1];
		point3 v2 = vertices[i2];

		if (triangle_intersection(ray, ray_t.min, closest_so_far, temp_rec, v0, v1, v2, u, v))
			return true;
	}
	return false;
}
