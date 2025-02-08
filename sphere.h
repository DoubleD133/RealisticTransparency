#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "interval.h"
#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
  sphere() {
    center = point3(0.0f, 0.0f, 0.0f);
    radius = 1.0f;
    SquareRadius = 1.0f;
  }
  sphere(point3 _center, float _radius) : center(_center), radius(_radius) {
      SquareRadius = radius * radius;
  }

  bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
    vec3 oc = r.origin() - center;
    float a = r.direction().length_squared();
    float half_b = dot(oc, r.direction());
    float c = oc.length_squared() - SquareRadius;

    float discriminant = half_b * half_b - a * c;
    if (discriminant < 0) return false;
    float sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    float root = (-half_b - sqrtd) / a;
    if (!ray_t.surrounds(root)) {
      root = (-half_b + sqrtd) / a;
      if (!ray_t.surrounds(root))
        return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);

    point3 puntoSistemaRel = (rec.p - center) / radius;
    float x = puntoSistemaRel.x(), y = puntoSistemaRel.y(), z = puntoSistemaRel.z();
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

    rec.u = (phi + pi / 2) / (2 * pi);
    rec.v = 1 - theta / pi;

    vec3 outward_normal = puntoSistemaRel;
    if (!normEst)
        outward_normal = -outward_normal;
    rec.set_face_normal(r, outward_normal);

    return true;
  }

  bool hit_shadow(const ray& r, interval ray_t) const override {
    vec3 oc = r.origin() - center;
    float a = r.direction().length_squared();
    float half_b = dot(oc, r.direction());
    float c = oc.length_squared() - radius * radius;

    float discriminant = half_b * half_b - a * c;
    if (discriminant < 0) return false;
    float sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    float root = (-half_b - sqrtd) / a;
    if (!ray_t.surrounds(root)) {
      root = (-half_b + sqrtd) / a;
      if (!ray_t.surrounds(root))
        return false;
    }

    return true;
  }

  // raggio al quadrato
  float SquareRadius;
  // normali verso l'esterno
  bool normEst = true;
private:
  point3 center;
  float radius;
};

class cilindroStandard : public hittable {
public:

    //cilindro standard è un cilindro senza basi di raggio unitario,
    //altezza 2 e centro l'origine. Tramite istanze crea tutti gli altri cilindri

    cilindroStandard() {
        vertice1 = point3(0.0, -1.0, 0.0);

        //per considerarlo come i coni precedenti:
        //vertice2 = point3(0.0, 1.0, 0.0);
        //radiusBase = 1.0;
        //v = vec3(0.0, 1.0, 0.0);
        //h = 2.0;

        //// costruisco un vettore non nullo ed ortogonale a v
        //w1 = vec3(1.0, 0.0, 0.0);

        //// essendo v e w1 ortonormali allora w2 è unitario
        //w2 = vec3(0.0, 0.0, 1.0);

        //radiusBase_squared = 1.0;
        return;
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {

        //Cilindro: f(x,y,z)=0 con f(p)=dot(p - vertice1, w1)^2 + dot(p - vertice1, w2)^2 - radiusBase^2
        // f(r.o + t d) = dot(r.o + t d - vertice1, w1)^2 + dot(r.o + t d - vertice1, w2)^2 - radiusBase^2
        // chiamando p0 = r.o - vertice1
        // f(r.o + t d) = dot(p0 + t d, w1)^2 + dot(p0 + t d, w2)^2 - radiusBase^2 =
        //              = [dot(p0, w1) + t dot(d, w1)]^2 + [dot(p0, w2) + t dot(d, w2)]^2 - radiusBase^2 =
        //              =   dot(p0, w1)^2 + t^2 dot(d, w1)^2 + 2 * t * dot(p0, w1) * dot(d, w1)
        //                + dot(p0, w2)^2 + t^2 dot(d, w2)^2 + 2 * t * dot(p0, w2) * dot(d, w2)
        //                - radiusBase^2 =
        //              = t^2 [dot(d, w1)^2 + dot(d, w2)^2]
        //                + 2*t [dot(p0, w1) * dot(d, w1) + dot(p0, w2) * dot(d, w2) ]
        //                + [dot(p0, w1)^2 + dot(p0, w2)^2 - radiusBase^2] =
        //              = t^2 [ (d.x())^2 + (d.z())^2]
        //                + 2*t [p0.x() * d.x() + p0.z() * d.z() ]
        //                + [(p0.x())^2 + (p0.z())^2 - radiusBase^2]


        vec3 d = r.d;
        point3 p0 = r.o - vertice1;
        float DdotW1 = d.x();
        float DdotW2 = d.z();
        float P0dotW1 = p0.x();
        float P0dotW2 = p0.z();

        float a = DdotW1 * DdotW1 + DdotW2 * DdotW2;
        float half_b = P0dotW1 * DdotW1 + P0dotW2 * DdotW2;
        float c = P0dotW1 * P0dotW1 + P0dotW2 * P0dotW2 - 1.0;

        float discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        float sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        float root = (-half_b - sqrtd) / a;
        // punto d'intersezione
        point3 intersezione = r.at(root);
        // altezza relativa dell'intersezione
        float altRel = intersezione.y() + 1.0;
        if (!ray_t.surrounds(root) || altRel < 0 || altRel > 2) {
            root = (-half_b + sqrtd) / a;
            // punto d'intersezione
            intersezione = r.at(root);
            // altezza relativa dell'intersezione
            altRel = intersezione.y() + 1;
            if (!ray_t.surrounds(root) || altRel < 0 || altRel > 2)
                return false;
        }

        rec.t = root;
        rec.p = intersezione;

        // cilindro: f(x,y,z)=0 con f(p)=dot(p - vertice, w1)^2 + dot(p - vertice, w2)^2 - radiusBase^2
        // calcolo le componenti (dimezzate) del gradiente nel punto per avere la normale
        float gradx = intersezione.x();
        float gradz = intersezione.z();

        //float scalare = pow(dot(intersezione - vertice, w1),2) + pow(dot(intersezione - vertice, w2),2) - pow( radiusBase ,2);
        //if( abs(scalare) >= 0.001 )
        //    cout << "f(p_t) = " << scalare << endl;

        vec3 outward_normal = vec3(gradx, 0.0f, gradz);

        if (!normEst)
            outward_normal = -outward_normal;

        rec.set_face_normal(r, outward_normal);


        // angolo, in radianti (quindi in [-pi/2, pi * 3/2[), che
        // il vettore (x,0,z)-(0,0,0) forma con l'asse z
        // anomalia (una delle coordinate polari) del punto (x,0,z) nel piano xz
        float phi;
        float x = intersezione[0], y = intersezione[1], z = intersezione[2];
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
        rec.u = (phi + pi / 2) / (2 * pi);

        rec.v = altRel / 2.0;

        return true;
    }

    bool hit_shadow(const ray& r, interval ray_t) const override {
        //Cilindro: f(x,y,z)=0 con f(p)=dot(p - vertice1, w1)^2 + dot(p - vertice1, w2)^2 - radiusBase^2
        // f(r.o + t d) = dot(r.o + t d - vertice1, w1)^2 + dot(r.o + t d - vertice1, w2)^2 - radiusBase^2
        // chiamando p0 = r.o - vertice1
        // f(r.o + t d) = dot(p0 + t d, w1)^2 + dot(p0 + t d, w2)^2 - radiusBase^2 =
        //              = [dot(p0, w1) + t dot(d, w1)]^2 + [dot(p0, w2) + t dot(d, w2)]^2 - radiusBase^2 =
        //              =   dot(p0, w1)^2 + t^2 dot(d, w1)^2 + 2 * t * dot(p0, w1) * dot(d, w1)
        //                + dot(p0, w2)^2 + t^2 dot(d, w2)^2 + 2 * t * dot(p0, w2) * dot(d, w2)
        //                - radiusBase^2 =
        //              = t^2 [dot(d, w1)^2 + dot(d, w2)^2]
        //                + 2*t [dot(p0, w1) * dot(d, w1) + dot(p0, w2) * dot(d, w2) ]
        //                + [dot(p0, w1)^2 + dot(p0, w2)^2 - radiusBase^2] =
        //              = t^2 [ (d.x())^2 + (d.z())^2]
        //                + 2*t [p0.x() * d.x() + p0.z() * d.z() ]
        //                + [(p0.x())^2 + (p0.z())^2 - radiusBase^2]


        vec3 d = r.d;
        point3 p0 = r.o - vertice1;
        float DdotW1 = d.x();
        float DdotW2 = d.z();
        float P0dotW1 = p0.x();
        float P0dotW2 = p0.z();

        float a = DdotW1 * DdotW1 + DdotW2 * DdotW2;
        float half_b = P0dotW1 * DdotW1 + P0dotW2 * DdotW2;
        float c = P0dotW1 * P0dotW1 + P0dotW2 * P0dotW2 - 1.0;

        float discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        float sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        float root = (-half_b - sqrtd) / a;
        // punto d'intersezione
        point3 intersezione = r.at(root);
        // altezza relativa dell'intersezione
        float altRel = intersezione.y() + 1;
        if (!ray_t.surrounds(root) || altRel < 0 || altRel > 2) {
            root = (-half_b + sqrtd) / a;
            // punto d'intersezione
            point3 intersezione = r.at(root);
            // altezza relativa dell'intersezione
            altRel = intersezione.y() + 1;
            if (!ray_t.surrounds(root) || altRel < 0 || altRel > 2)
                return false;
        }

        return true;
    }

    // normali verso l'esterno
    bool normEst = true;
private:
    point3 vertice1;
    //point3 vertice2;
    //// direzione apertura cono, di solito (0.0f, -1.0f, 0.0f)
    //vec3 v;
    //// versori in modo che v,w1,w2 sia un sistema ortonormale
    //vec3 w1;
    //vec3 w2;
    //float radiusBase;
    //float radiusBase_squared;
    //// altezza cilindro
    //float h;
};

class discoStandard : public hittable {
public:

    //Disco standard è un disco di raggio unitario e centro l'origine. 
    //Tramite istanze crea tutti gli altri dischi

    discoStandard() {
        center = point3(0.0f, 0.0f, 0.0f);
        radius = 1.0f;
        SquareRadius = 1.0f;
        n = vec3(0.0, 1.0, 0.0);
    }

    discoStandard(float alt) {
        h = alt;
        normSp = true;
        center = point3(0.0f, 0.0f, 0.0f);
        radius = 1.0f;
        SquareRadius = 1.0f;
        n = vec3(0.0, 1.0, 0.0);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {

        // verifico per quali t p_t sta nel piano passante per centro ed ortogonale ad n
        // equivale a verificare per quali t i vettori p_t-centro ed n sono ortogonali
        // <=> dot(p_t-centro, n)=0
        // <=> dot(p0 + t d - centro, n)=0 <=> dot(p0 - centro, n) + t dot(d, n)=0
        // <=> t dot(d, n) = dot(centro - p0, n)
        float scalare1 = r.direction().y();
        float scalare2 = - r.origin().y();

        float istante = scalare2 / scalare1;
        if (!ray_t.surrounds(istante))
            return false;

        point3 punto = r.at(istante);

        if (punto.x()* punto.x() + punto.z() * punto.z() <= SquareRadius) {
            rec.p = punto;
            rec.t = istante;

            if (!normSp) {
                if(normAlto)
                    rec.set_face_normal(r, n);
                else
                    rec.set_face_normal(r, -n);
            }
            else {

                int normInt = ((int)normAlto * 2 - 1) * ((int)(h > 0) * 2 - 1);
                point3 pSfera(punto[0], ((float)(h < 0) * 2.0 - 1.0) * sqrt(1.0 - punto[0] * punto[0] - punto[2] * punto[2]), punto[2]);
                // p - CentroSfera (CentroSfera = (0,0,0) )
                vec3 m = pSfera;
                if (normInt == 1)
                    m = -m;
                rec.set_face_normal(r, m);
            }
            return true;
        }
        return false;
    }

    bool hit_shadow(const ray& r, interval ray_t) const override {
        float scalare1 = r.direction().y();
        float scalare2 = -r.origin().y();

        float istante = scalare2 / scalare1;
        if (!ray_t.surrounds(istante))
            return false;

        point3 punto = r.at(istante);

        if (punto.x() * punto.x() + punto.z() * punto.z() <= SquareRadius)
            return true;
        else
            return false;
    }

    // raggio al quadrato
    float SquareRadius;
    // normali verso l'alto
    bool normAlto = true;
    // normali sferiche
    bool normSp = false;
    // altezza centro sfera per normali sferiche
    float h;
private:
    point3 center;
    float radius;
    // normale al piano
    vec3 n;
};

class iperb1faldaStandard : public hittable {
public:

    //Iperboloide a 1 falda standard è un iperboloide a 1 falda di raggio unitario e centro l'origine.
    // f(p) = p.x() ^ 2 + p.z() ^ 2 - p.y() ^ 2 - 1
    //Tramite istanze crea tutti gli altri dischi
    
    // data altezza minima e massima
    iperb1faldaStandard(float alt_min, float alt_max) {
        vertice = point3(0.0, 0.0, 0.0);
        h_min = alt_min;
        h_max = alt_max;
        v = vec3(0.0, 1.0, 0.0);

        // costruisco un vettore non nullo ed ortogonale a v
        w1 = vec3(1.0, 0.0, 0.0f);

        // essendo v e w1 ortonormali allora w2 è unitario
        w2 = vec3(0.0, 0.0, 1.0f);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {

        // iperboloide: f(x, y, z) = 0 con f(p) = dot(p - vertice, w1) ^ 2 + dot(p - vertice, w2) ^ 2 - dot(p - vertice, v) ^ 2 - 1
        // f(r.o + t d) = dot(r.o + t d - vertice, w1)^2 + dot(r.o + t d - vertice, w2)^2 - dot(r.o + t d - vertice,v)^2 -1
        // chiamando p0 = r.o - vertice
        // f(r.o + t d) = dot(p0 + t d, w1)^2 + dot(p0 + t d, w2)^2 - dot(p0 + t d,v)^2 -1=
        //              = [dot(p0, w1) + t dot(d, w1)]^2 + [dot(p0, w2) + t dot(d, w2)]^2 - [ dot(p0,v) + t dot(d,v) ]^2 -1=
        //              =   dot(p0, w1)^2 + t^2 dot(d, w1)^2 + 2 * t * dot(p0, w1) * dot(d, w1)
        //                + dot(p0, w2)^2 + t^2 dot(d, w2)^2 + 2 * t * dot(p0, w2) * dot(d, w2)
        //                - dot(p0, v)^2 - t^2 dot(d, v)^2 - 2 * t * dot(p0, v) * dot(d, v) -1 =
        //              = t^2 [dot(d, w1)^2 + dot(d, w2)^2 - dot(d, v)^2 ]
        //                + 2*t [dot(p0, w1) * dot(d, w1) + dot(p0, w2) * dot(d, w2) - dot(p0, v) * dot(d, v)  ]
        //                + [dot(p0, w1)^2 + dot(p0, w2)^2 - dot(p0, v)^2 -1]

        vec3 d = r.d;
        point3 p0 = r.o;
        float DdotW1 = d.x();
        float DdotW2 = d.z();
        float DdotV = d.y();
        float P0dotW1 = p0.x();
        float P0dotW2 = p0.z();
        float P0dotV = p0.y();

        float a = DdotW1 * DdotW1 + DdotW2 * DdotW2 - DdotV * DdotV;
        float half_b = P0dotW1 * DdotW1 + P0dotW2 * DdotW2 - P0dotV * DdotV;
        float c = P0dotW1 * P0dotW1 + P0dotW2 * P0dotW2 - P0dotV * P0dotV - 1;

        float discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        float sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        float root = (-half_b - sqrtd) / a;
        // punto d'intersezione
        point3 intersezione = r.at(root);
        // altezza relativa dell'intersezione
        float altRel = intersezione.y();


        if (!ray_t.surrounds(root) || altRel < h_min || altRel > h_max) {
            root = (-half_b + sqrtd) / a;
            // punto d'intersezione
            point3 intersezione = r.at(root);
            // altezza relativa dell'intersezione
            float altRel = intersezione.y();
            if (!ray_t.surrounds(root) || altRel < h_min || altRel > h_max)
                return false;
        }


        rec.t = root;
        rec.p = intersezione;

        // iperboloide: f(x,y,z)=0 con f(p)=dot(p - vertice, w1)^2 + dot(p - vertice, w2)^2 - dot(p - vertice,v)^2 - 1
        // calcolo le componenti 8dimezzate) del gradiente nel punto per avere la normale
        float gradx = intersezione.x();
        float grady = - intersezione.y();
        float gradz = intersezione.z();

        vec3 outward_normal = unit_vector(vec3(gradx, grady, gradz));
        if (!normEst)
            outward_normal = -outward_normal;
        rec.set_face_normal(r, outward_normal);

        // angolo, in radianti (quindi in [-pi/2, pi * 3/2[), che
        // il vettore (x,0,z)-(0,0,0) forma con l'asse z
        // anomalia (una delle coordinate polari) del punto (x,0,z) nel piano xz
        float phi;
        float x = intersezione[0], y = intersezione[1], z = intersezione[2];
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
        rec.u = (phi + pi / 2) / (2 * pi);

        rec.v = (altRel - h_min) / (h_max - h_min);

        return true;
    }

    bool hit_shadow(const ray& r, interval ray_t) const override {

        // iperboloide: f(x, y, z) = 0 con f(p) = dot(p - vertice, w1) ^ 2 + dot(p - vertice, w2) ^ 2 - dot(p - vertice, v) ^ 2 - 1
        // f(r.o + t d) = dot(r.o + t d - vertice, w1)^2 + dot(r.o + t d - vertice, w2)^2 - dot(r.o + t d - vertice,v)^2 -1
        // chiamando p0 = r.o - vertice
        // f(r.o + t d) = dot(p0 + t d, w1)^2 + dot(p0 + t d, w2)^2 - dot(p0 + t d,v)^2 -1=
        //              = [dot(p0, w1) + t dot(d, w1)]^2 + [dot(p0, w2) + t dot(d, w2)]^2 - [ dot(p0,v) + t dot(d,v) ]^2 -1=
        //              =   dot(p0, w1)^2 + t^2 dot(d, w1)^2 + 2 * t * dot(p0, w1) * dot(d, w1)
        //                + dot(p0, w2)^2 + t^2 dot(d, w2)^2 + 2 * t * dot(p0, w2) * dot(d, w2)
        //                - dot(p0, v)^2 - t^2 dot(d, v)^2 - 2 * t * dot(p0, v) * dot(d, v) -1 =
        //              = t^2 [dot(d, w1)^2 + dot(d, w2)^2 - dot(d, v)^2 ]
        //                + 2*t [dot(p0, w1) * dot(d, w1) + dot(p0, w2) * dot(d, w2) - dot(p0, v) * dot(d, v)  ]
        //                + [dot(p0, w1)^2 + dot(p0, w2)^2 - dot(p0, v)^2 -1]

        vec3 d = r.d;
        point3 p0 = r.o;
        float DdotW1 = d.x();
        float DdotW2 = d.z();
        float DdotV = d.y();
        float P0dotW1 = p0.x();
        float P0dotW2 = p0.z();
        float P0dotV = p0.y();

        float a = DdotW1 * DdotW1 + DdotW2 * DdotW2 - DdotV * DdotV;
        float half_b = P0dotW1 * DdotW1 + P0dotW2 * DdotW2 - P0dotV * DdotV;
        float c = P0dotW1 * P0dotW1 + P0dotW2 * P0dotW2 - P0dotV * P0dotV - 1;

        float discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        float sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        float root = (-half_b - sqrtd) / a;
        // punto d'intersezione
        point3 intersezione = r.at(root);
        // altezza relativa dell'intersezione
        float altRel = intersezione.y();


        if (!ray_t.surrounds(root) || altRel < h_min || altRel > h_max) {
            root = (-half_b + sqrtd) / a;
            // punto d'intersezione
            point3 intersezione = r.at(root);
            // altezza relativa dell'intersezione
            float altRel = intersezione.y();
            if (!ray_t.surrounds(root) || altRel < h_min || altRel > h_max)
                return false;
        }

        return true;
    }

    // per orientare le normali verso l'esterno
    bool normEst = true;
private:
    point3 vertice;
    // direzione apertura iperboloide, di solito (0.0f, -1.0f, 0.0f)
    vec3 v;
    // versori in modo che v,w1,w2 sia un sistema ortonormale
    vec3 w1;
    vec3 w2;
    // altezza a partire dal vertice
    float h_min, h_max;
};

class coronaStandard : public hittable {
public:

    //Corona standard è una corona circolare di raggio esterno unitario, raggio interno r centro l'origine. 
    //Tramite istanze crea tutte le altre corone circolari

    coronaStandard(float p) {
        center = point3(0.0f, 0.0f, 0.0f);
        radius = 1.0f;
        SquareRadius = 1.0f;
        if (p >= 1.0)
            cout << "errore raggio interno\n";
        r_int = p;
        SquareR = r_int * r_int;
        n = vec3(0.0, 1.0, 0.0);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {

        // verifico per quali t p_t sta nel piano passante per centro ed ortogonale ad n
        // equivale a verificare per quali t i vettori p_t-centro ed n sono ortogonali
        // <=> dot(p_t-centro, n)=0
        // <=> dot(p0 + t d - centro, n)=0 <=> dot(p0 - centro, n) + t dot(d, n)=0
        // <=> t dot(d, n) = dot(centro - p0, n)
        float scalare1 = r.direction().y();
        float scalare2 = -r.origin().y();

        float istante = scalare2 / scalare1;
        if (!ray_t.surrounds(istante))
            return false;

        point3 punto = r.at(istante);

        float ro = punto.x() * punto.x() + punto.z() * punto.z();
        if ( ro <= SquareRadius && ro >= SquareR) {
            rec.p = punto;
            rec.t = istante;

            if (normAlto)
                rec.set_face_normal(r, n);
            else
                rec.set_face_normal(r, -n);
            return true;
        }
        return false;
    }

    bool hit_shadow(const ray& r, interval ray_t) const override {
        float scalare1 = r.direction().y();
        float scalare2 = -r.origin().y();

        float istante = scalare2 / scalare1;
        if (!ray_t.surrounds(istante))
            return false;

        point3 punto = r.at(istante);

        float ro = punto.x() * punto.x() + punto.z() * punto.z();
        if (ro <= SquareRadius && ro >= SquareR)
            return true;
        else
            return false;
    }

    // raggio esterno al quadrato
    float SquareRadius;
    // raggio interno al quadrato
    float SquareR;
    // normali verso l'alto
    bool normAlto = true;
private:
    point3 center;
    float radius;
    float r_int;
    // normale al piano
    vec3 n;
};



#endif