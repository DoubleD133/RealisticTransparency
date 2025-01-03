#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "interval.h"
#include "hittable.h"
#include "vec3.h"
#include "MieFunzioniComuni.h"

class quadrilatero : public hittable {
public:
    quadrilatero(point3 _vertice1, point3 _vertice2, point3 _vertice3, point3 _vertice4) : vertice1(_vertice1), vertice2(_vertice2), vertice3(_vertice3), vertice4(_vertice4) {
        latoA = vertice2 - vertice1;
        latoB = vertice4 - vertice1;
        n = unit_vector(cross(latoA, latoB));
        vec3 raggio3 = vertice3 - vertice1;
        float ComponenteNormale3 = dot(n, raggio3);
        float ComponenteNormale3Normalizzata = ComponenteNormale3 / raggio3.length();
        if (ComponenteNormale3Normalizzata >= 0.0001 || ComponenteNormale3Normalizzata <= -0.0001) {
            std::cout << "Il quadrilatero è sghembo (non piano), ne modifico il vertice3\n";
            vec3 NuovoRaggio3 = raggio3 - ComponenteNormale3 * n;
            vertice3 = vertice1 + NuovoRaggio3;
        }

        latoOppostoA = vertice3 - vertice4;
        latoOppostoB = vertice3 - vertice2;

        // in vec3 c'è la funzione RimuoviComponenteParallela(vec3 partenza, vec3 ortogonale)
        nA = unit_vector(RimuoviComponenteParallela(latoB,latoA)); //tolgo a latoB la componente parallela a latoA
        nB = unit_vector(RimuoviComponenteParallela(latoA,latoB));
        nOppostoA = unit_vector(RimuoviComponenteParallela(-latoB, latoOppostoA));
        nOppostoB = unit_vector(RimuoviComponenteParallela(-latoA, latoOppostoB));
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // verifico per quali t p_t sta nel piano passante per vertice1 ed ortogonale ad n
        // equivale a verificare per quali t i vettori p_t-vertice1 ed n sono ortogonali
        // <=> dot(p_t-vertice1, n)=0 <=> t dot(d,n) = dot(vertice1-p_0,n)
        float scalare1 = dot(r.direction(), n);
        float scalare2 = dot(vertice1 - r.origin(), n);

        // il raggio interseca il piano in un punto
        if (scalare1 != 0) {
            float istante = scalare2 / scalare1;
            if (!ray_t.surrounds(istante))
                return false;

            point3 punto = r.at(istante);
            vec3 Raggio1 = punto - vertice1;
            vec3 Raggio3 = punto - vertice3;
            if (dot(Raggio1,nA) >= 0 && dot(Raggio1, nB) >= 0 && dot(Raggio3, nOppostoA) >= 0 && dot(Raggio3, nOppostoB) >= 0) {
                rec.p = punto;
                rec.t = istante;
                rec.set_face_normal(r, n);
                return true;
            }
            else {
                return false;
            }
        }

        // se arriviamo qui il raggio è parallelo al piano
        // verifivhiamo se giace o meno nel piano
        if (scalare2 != 0)
            return false; // non giace nel piano

        // il raggio giace sul piano, statisticamente impossibile
        return false;
    }

private:
    // vertici consecutivi (prefibilmente orari)
    point3 vertice1;
    point3 vertice2;
    point3 vertice3;
    point3 vertice4;
    // lati:
    vec3 latoA; // da v1 a v2
    vec3 latoOppostoA; // da v4 a v3
    vec3 latoB; // da v1 a v4
    vec3 latoOppostoB; // da v2 a v3
    // normali ai lati nel piano:
    vec3 nA;
    vec3 nOppostoA;
    vec3 nB;
    vec3 nOppostoB;
    // normale al piano:
    vec3 n;
    // il piano ha un orientamento locale con origine vertice1 ed assi diretti come latoA e latoB
};

class cubo : public hittable_list {
public:

    // costruttore con tre punti orari su una faccia
    cubo(point3 v1, point3 v2, point3 v3) {
        objects.clear();

        //faccia inferiore
        vec3 lato23 = v3 - v2;
        point3 v4 = v1 + lato23;
        objects.push_back(make_shared<quadrilatero>(v1, v2, v3, v4));
        vec3 n = cross(lato23, v1 - v2) * (1.0 / lato23.length());
        vec3 v5 = v1 + n;
        vec3 v6 = v2 + n;
        vec3 v7 = v3 + n;
        vec3 v8 = v4 + n;

        // faccia frontale ~
        objects.push_back(make_shared<quadrilatero>(v1, v2, v6, v5));
        // faccia destra
        objects.push_back(make_shared<quadrilatero>(v6, v2, v3, v7));
        // faccia posteriore ~
        objects.push_back(make_shared<quadrilatero>(v7, v3, v4, v8));
        // faccia sinistra
        objects.push_back(make_shared<quadrilatero>(v8, v4, v1, v5));
        // faccia superiore
        objects.push_back(make_shared<quadrilatero>(v6, v7, v8, v5));
    };

    // costruttore con tre punti orari su una faccia
    cubo(point3 vmin, point3 vmax) {
        objects.clear();

        point3 v1 = vmin;
        point3 v2(vmax.x(), vmin.y(), vmin.z());
        point3 v3(vmax.x(), vmin.y(), vmax.z());
        point3 v4(vmin.x(), vmin.y(), vmax.z());

        objects.push_back(make_shared<quadrilatero>(v1, v2, v3, v4));

        vec3 v5(vmin.x(), vmax.y(), vmin.z());
        vec3 v6(vmax.x(), vmax.y(), vmin.z());
        vec3 v7 = vmax;
        vec3 v8(vmin.x(), vmax.y(), vmax.z());

        // faccia frontale ~
        objects.push_back(make_shared<quadrilatero>(v1, v2, v6, v5));
        // faccia destra
        objects.push_back(make_shared<quadrilatero>(v6, v2, v3, v7));
        // faccia posteriore ~
        objects.push_back(make_shared<quadrilatero>(v7, v3, v4, v8));
        // faccia sinistra
        objects.push_back(make_shared<quadrilatero>(v8, v4, v1, v5));
        // faccia superiore
        objects.push_back(make_shared<quadrilatero>(v6, v7, v8, v5));
    };

};

class piano : public hittable {
public:
    piano(point3 _p0, vec3 _n) : p0(_p0), n(_n) { };

    piano(point3 _p0, point3 p1, point3 p2) : p0(_p0) {
        n = unit_vector(cross(p1 - p0, p2 - p0));
    };

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // verifico per quali t p_t sta nel piano passante per p0 ed ortogonale ad n
        // equivale a verificare per quali t i vettori p_t-p0 ed n sono ortogonali
        // <=> dot(p_t-p0, n)=0 <=> t dot(d,n) = dot(o-p_0,n)
        float scalare1 = dot(r.direction(), n);
        float scalare2 = dot(p0 - r.origin(), n);

        // il raggio interseca il piano in un punto
        if (scalare1 != 0) {
            float istante = scalare2 / scalare1;
            if (!ray_t.surrounds(istante))
                return false;

            point3 punto = r.at(istante);
            rec.p = punto;
            rec.t = istante;
            rec.set_face_normal(r, n);
            return true;
        }

        // se arriviamo qui il raggio è parallelo al piano
        // verifivhiamo se giace o meno nel piano
        if (scalare2 != 0)
            return false;

        // se siamo qui il raggio, e quindi anche il suo origine giacciono sul piano
        rec.p = r.o;
        rec.t = 0;
        rec.set_face_normal(r, n);
        return true;
    }

private:
    // punto del piano
    point3 p0;
    // normale al piano
    vec3 n;
};

class disco : public hittable {
public:
    disco(point3 _centro, vec3 _n, float _raggio) : centro(_centro), n(_n), raggio(_raggio) {
        raggioQuadro = raggio * raggio;
    };

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // verifico per quali t p_t sta nel piano passante per centro ed ortogonale ad n
        // equivale a verificare per quali t i vettori p_t-centro ed n sono ortogonali
        // <=> dot(p_t-centro, n)=0 <=> t dot(d,n) = dot(o-centro,n)
        float scalare1 = dot(r.direction(), n);
        float scalare2 = dot(centro - r.origin(), n);

        // il raggio interseca il piano in un punto
        if (scalare1 != 0) {
            float istante = scalare2 / scalare1;
            if (!ray_t.surrounds(istante))
                return false;
            
            point3 punto = r.at(istante);

            if( (punto - centro).length_squared() <= raggioQuadro) {
                rec.p = punto;
                rec.t = istante;
                rec.set_face_normal(r, n);
                return true;
            }
            return false;
        }

        // se arriviamo qui il raggio è parallelo al piano
        // verifivhiamo se giace o meno nel piano
        if (scalare2 != 0)
            return false;

        // se siamo qui il raggio giacce sul piano, statisticamente impossibile
        return false;
    }

private:
    // centro del disco
    point3 centro;
    // normale al piano
    vec3 n;
    // raggio del disco
    float raggio;
    float raggioQuadro;
};

class triangolo : public hittable {
public:
    // tre punti orari
    triangolo(point3 _vertice1, point3 _vertice2, point3 _vertice3) : vertice1(_vertice1), vertice2(_vertice2), vertice3(_vertice3) {
        lato12 = vertice2 - vertice1;
        lato23 = vertice3 - vertice2;
        lato31 = vertice1 - vertice3;
        n = unit_vector(cross(lato31, lato12));

        // in vec3 c'è la funzione RimuoviComponenteParallela(vec3 partenza, vec3 ortogonale)
        // RimuoviComponenteParallela( v, w) è la componente di v ortogonale a w
        n12 = unit_vector(RimuoviComponenteParallela(-lato31, lato12)); //tolgo a -lato31 la componente parallela a lato12
        n23 = unit_vector(RimuoviComponenteParallela(lato31, lato23));
        n31 = unit_vector(RimuoviComponenteParallela(lato12, lato31));

        // Questi tre vettori e tre punti giaciono su un piano di origine vertice1
        // ed assi unit_vector(lato12) ed n12
        dir1 = unit_vector(lato12);
        // componenti nel piano
        vertice1_1 = 0;
        vertice1_2 = 0;
        vertice2_1 = dot(vertice2 - vertice1, dir1);
        vertice2_2 = dot(vertice2 - vertice1, n12);
        vertice3_1 = dot(vertice3 - vertice1, dir1);
        vertice3_2 = dot(vertice3 - vertice1, n12);
        lato12_1 = dot(lato12, dir1);
        lato12_2 = 0;
        lato23_1 = dot(lato23, dir1);
        lato23_2 = dot(lato23, n12);
        lato31_1 = dot(lato31, dir1);
        lato31_2 = dot(lato31, n12);
        n12_1 = 0;
        n12_2 = 1;
        n23_1 = dot(n23, dir1);
        n23_2 = dot(n23, n12);
        n31_1 = dot(n31, dir1);
        n31_2 = dot(n31, n12);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // verifico per quali t p_t sta nel piano passante per vertice1 ed ortogonale ad n
        // equivale a verificare per quali t i vettori p_t-vertice1 ed n sono ortogonali
        // <=> dot(p_t-vertice1, n)=0 <=> t dot(d,n) = dot(vertice1-p_0,n)
        
        // vecchi conti in IR^3 
        float scalare1 = dot(r.direction(), n);
        float scalare2 = dot(vertice1 - r.origin(), n);

        // il raggio interseca il piano in un punto
        if (scalare1 != 0) {
            float istante = scalare2 / scalare1;
            if (!ray_t.surrounds(istante))
                return false;

            point3 punto = r.at(istante);
            vec3 Raggio1 = punto - vertice1;
            vec3 Raggio3 = punto - vertice3;
            if (dot(Raggio1, n12) >= 0 && dot(Raggio3, n23) >= 0 && dot(Raggio3, n31) >= 0 ) {
                rec.p = punto;
                rec.t = istante;
                rec.set_face_normal(r, n);
                return true;
            }
            return false;
        }
        
        /* // conti in IR^2
        vec3 OMenoV1 = r.origin() - vertice1;
        float scalare1 = dot(r.direction(), n);
        float scalare2 = -dot(OMenoV1, n);

        // il raggio interseca il piano in un punto
        if (scalare1 != 0) {
            float istante = scalare2 / scalare1;
            if (!ray_t.surrounds(istante))
                return false;

            // punto = o + td => Raggio1 = punto - vertice1 = o - vertice1 + td
            point3 Raggio1 = OMenoV1 + istante * r.direction();
            // coordinate del punto nel piano
            float punto_1 = dot(Raggio1, dir1);
            float punto_2 = dot(Raggio1, n12);
            // Raggio3 = punto - vertice3 = o - vertice3 + td =
            //         = (o - vertice1 + td) - (vertice3 - vertice1) =
            //         = (punto_1, punto_2) - (vertice3_1, vertice3_2)
            float Raggio3_1 = punto_1 - vertice3_1;
            float Raggio3_2 = punto_2 - vertice3_2;
            float Raggio1_dot_n12 = punto_2;
            float Raggio3_dot_n23 = Raggio3_1 * n23_1 + Raggio3_2 * n23_2;
            float Raggio3_dot_n31 = Raggio3_1 * n31_1 + Raggio3_2 * n31_2;
            if (Raggio1_dot_n12 >= 0 && Raggio3_dot_n23 >= 0 && Raggio3_dot_n31 >= 0) {
                rec.p = Raggio1 + vertice1;
                rec.t = istante;
                rec.set_face_normal(r, n);
                return true;
            }
            return false;
        }*/

        // CASO RARISSIMISSIMO (soprattutto per l'errore numerico)
        // se arriviamo qui il raggio è parallelo al piano
        // verifichiamo se giace o meno nel piano
        if (scalare2 != 0)
            return false; // non giace nel piano

        // se arriviamo qui il raggio giace nel piano del triangolo
        // Statisticamente impossibile ma studiamolo comunque

        point3 centro = r.o;
        vec3 Raggio1 = centro - vertice1;
        vec3 Raggio3 = centro - vertice3;

        if (dot(Raggio1, n12) >= 0 && dot(Raggio3, n23) >= 0 && dot(Raggio3, n31) >= 0) {
            // origine nel quadrilatero
            rec.p = centro;
            rec.t = 0;
            rec.set_face_normal(r, n);
            return true;
        }

        // se arriviamo qui il raggio giace nel piano del triangolo ma l'origine è fuori dal triangolo
        // le eventuali intersezioni raggio-triangolo sono intersezioni raggio-segmento
        // le calcolo di seguito è prendo la prima

        hit_record temp_rec;
        bool hit_anything = false;
        float closest_so_far = ray_t.max;

        std::vector<point3> vertici;
        vertici.push_back(vertice1);
        vertici.push_back(vertice2);
        vertici.push_back(vertice3);

        std::vector<vec3> lati;
        lati.push_back(lato12);
        lati.push_back(lato23);
        lati.push_back(lato31);

        std::vector<vec3> normali;
        normali.push_back(n12);
        normali.push_back(n23);
        normali.push_back(n31);

        for (int indice = 0; indice < 3; indice++) {
            if (InterRagLato(r, interval(ray_t.min, closest_so_far), vertici[indice], lati[indice], normali[indice], n, temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }



private:
    // vertici consecutivi (prefibilmente orari)
    point3 vertice1;
    point3 vertice2;
    point3 vertice3;
    // lati:
    vec3 lato12; // da v1 a v2
    vec3 lato23; // da v2 a v3
    vec3 lato31; // da v3 a v1
    // normali ai lati nel piano (diretti dentro il triangolo):
    vec3 n12;
    vec3 n23;
    vec3 n31;
    // normale al piano:
    vec3 n;
    // il piano ha un orientamento locale con origine vertice1 ed assi diretti come latoA e latoB
    vec3 dir1;
    float vertice1_1, vertice1_2;
    float vertice2_1, vertice2_2;
    float vertice3_1, vertice3_2;
    float lato12_1, lato12_2; // da v1 a v2
    float lato23_1, lato23_2; // da v2 a v3
    float lato31_1, lato31_2; // da v3 a v1
    float n12_1, n12_2;
    float n23_1, n23_2;
    float n31_1, n31_2;
};

class conoSenzaBase : public hittable {
public:

    void PerDirezioneAlpha(point3 ver, vec3 dir, float al, float alt) {
        vertice = ver;
        alpha = al;
        h = alt;
        v = unit_vector(dir);
        centroBase = vertice + h * v;

        // costruisco un vettore non nullo ed ortogonale a v
        if (abs(v.x()) >= abs(v.z()) || abs(v.y()) >= abs(v.z()))
            w1 = unit_vector(vec3(v.y(), -v.x(), 0.0f));
        else
            w1 = unit_vector(vec3(v.z(), 0.0f, -v.x()));

        // essendo v e w1 ortonormali allora w2 è unitario
        w2 = cross(v, w1);

        fattore = tan(alpha);

        radiusBase = h * fattore;

        radiusBase_squared = radiusBase * radiusBase;
        fattoreQuadro = fattore * fattore;
        return;
    }

    void PerCentroAlpha(point3 ver, point3 centro, float al) {
        vertice = ver;
        alpha = al;
        h = (ver-centro).length();
        centroBase = centro;
        v = unit_vector(centroBase - vertice);

        // costruisco un vettore non nullo ed ortogonale a v
        if (abs(v.x()) >= abs(v.z()) || abs(v.y()) >= abs(v.z()))
            w1 = unit_vector(vec3(v.y(), -v.x(), 0.0f));
        else
            w1 = unit_vector(vec3(v.z(), 0.0f, -v.x()));

        // essendo v e w1 ortonormali allora w2 è unitario
        w2 = cross(v, w1);

        fattore = tan(alpha);

        radiusBase = h * fattore;

        radiusBase_squared = radiusBase * radiusBase;
        fattoreQuadro = fattore * fattore;
        return;
    }

    void PerCentroRaggio(point3 ver, point3 centro, float raggio) {
        vertice = ver;
        radiusBase = raggio;
        h = (ver - centro).length();
        centroBase = centro;
        v = unit_vector(centroBase - vertice);

        // costruisco un vettore non nullo ed ortogonale a v
        if ( abs(v.x()) >= abs(v.z()) || abs(v.y()) >= abs(v.z()) )
            w1 = unit_vector(vec3(v.y(), -v.x(), 0.0f));
        else
            w1 = unit_vector(vec3(v.z(), 0.0f, -v.x()));

        // essendo v e w1 ortonormali allora w2 è unitario
        w2 = cross(v, w1);

        fattore = radiusBase / h;
        alpha = atan(fattore);

        radiusBase_squared = radiusBase * radiusBase;
        fattoreQuadro = fattore * fattore;

        return;
    }


    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // cono: f(x,y,z)=0 con f(p)=dot(p - vertice, w1)^2 + dot(p - vertice, w2)^2 - ( dot(p - vertice,v) * fattore )^2
        // f(r.o + t d) = dot(r.o + t d - vertice, w1)^2 + dot(r.o + t d - vertice, w2)^2 - ( dot(r.o + t d - vertice,v) * fattore )^2
        // chiamando p0 = r.o - vertice
        // f(r.o + t d) = dot(p0 + t d, w1)^2 + dot(p0 + t d, w2)^2 - ( dot(p0 + t d,v) * fattore )^2 =
        //              = [dot(p0, w1) + t dot(d, w1)]^2 + [dot(p0, w2) + t dot(d, w2)]^2 - [ dot(p0,v) * fattore + t dot(d,v) * fattore ]^2 =
        //              =   dot(p0, w1)^2 + t^2 dot(d, w1)^2 + 2 * t * dot(p0, w1) * dot(d, w1)
        //                + dot(p0, w2)^2 + t^2 dot(d, w2)^2 + 2 * t * dot(p0, w2) * dot(d, w2)
        //                - dot(p0, v)^2 * fattore^2 - t^2 dot(d, v)^2 * fattore^2 - 2 * t * dot(p0, v) * dot(d, v) * fattore^2 =
        //              = t^2 [dot(d, w1)^2 + dot(d, w2)^2 - dot(d, v)^2 * fattore^2]
        //                + 2*t [dot(p0, w1) * dot(d, w1) + dot(p0, w2) * dot(d, w2) - dot(p0, v) * dot(d, v) * fattore^2 ]
        //                + [dot(p0, w1)^2 + dot(p0, w2)^2 - dot(p0, v)^2 * fattore^2]



        vec3 d = r.d;
        point3 p0 = r.o - vertice;
        float DdotW1 = dot(d, w1);
        float DdotW2 = dot(d, w2);
        float DdotV = dot(d, v);
        float P0dotW1 = dot(p0, w1);
        float P0dotW2 = dot(p0, w2);
        float P0dotV = dot(p0, v);

        float a = DdotW1 * DdotW1 + DdotW2 * DdotW2 - DdotV * DdotV * fattoreQuadro;
        float half_b = P0dotW1 * DdotW1 + P0dotW2 * DdotW2 - P0dotV * DdotV * fattoreQuadro;
        float c = P0dotW1 * P0dotW1 + P0dotW2 * P0dotW2 - P0dotV * P0dotV * fattoreQuadro;


        float discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        float sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        float root = (-half_b - sqrtd) / a;
        // punto d'intersezione
        point3 intersezione = r.at(root);
        // altezza relativa dell'intersezione
        float altRel = dot(intersezione-vertice, v);


        if (!ray_t.surrounds(root) || altRel < 0 || altRel > h) {
            root = (-half_b + sqrtd) / a;
            // punto d'intersezione
            point3 intersezione = r.at(root);
            // altezza relativa dell'intersezione
            float altRel = dot(intersezione-vertice, v);
            if (!ray_t.surrounds(root) || altRel < 0 || altRel > h)
                return false;
        }


        rec.t = root;
        rec.p = intersezione;

        // cono: f(x,y,z)=0 con f(p)=dot(p - vertice, w1)^2 + dot(p - vertice, w2)^2 - ( dot(p - vertice,v) * fattore )^2
        // calcolo le componenti 8dimezzate) del gradiente nel punto per avere la normale
        float gradx = w1.x() * dot(intersezione - vertice, w1) + w2.x() * dot(intersezione - vertice, w2) - v.x() * fattoreQuadro * dot(intersezione - vertice, v);
        float grady = w1.y() * dot(intersezione - vertice, w1) + w2.y() * dot(intersezione - vertice, w2) - v.y() * fattoreQuadro * dot(intersezione - vertice, v);
        float gradz = w1.z() * dot(intersezione - vertice, w1) + w2.z() * dot(intersezione - vertice, w2) - v.z() * fattoreQuadro * dot(intersezione - vertice, v);

        //float scalare = pow(dot(intersezione - vertice, w1),2) + pow(dot(intersezione - vertice, w2),2) - pow( dot(intersezione - vertice,v) * fattore ,2);
        //if( abs(scalare) >= 0.001 )
        //    cout << "f(p_t) = " << scalare << endl;

        vec3 outward_normal = unit_vector(vec3(gradx,grady,gradz));
        rec.set_face_normal(r, outward_normal);

        return true;
    }

private:
    point3 vertice;
    point3 centroBase;
    // direzione apertura cono, di solito (0.0f, -1.0f, 0.0f)
    vec3 v;
    // versori in modo che v,w1,w2 sia un sistema ortonormale
    vec3 w1;
    vec3 w2;
    float radiusBase;
    float radiusBase_squared;
    // semi-ampiezza del cono, angolo alpha in [0, pi[
    float alpha;
    float fattore;
    float fattoreQuadro;
    // altezza cono
    float h;
};

class conoConBase : public hittable_list {
public:

    // costruttore con vertice, direzione, alpha ed altezza
    conoConBase(point3 ver, vec3 dir, float al, float alt) {
        point3 centroBase = ver + alt * unit_vector(dir);
        float radiusBase = alt * tan(al);

        objects.clear();

        shared_ptr<conoSenzaBase> conoA;
        conoA->PerDirezioneAlpha(ver, dir, al, alt);
        objects.push_back(make_shared<disco>(centroBase, unit_vector(dir), radiusBase));
        objects.push_back(make_shared<conoSenzaBase>(conoA));
    }

    // costruttore con vertice, centro della base e raggio della base
    conoConBase(point3 ver, point3 centro, float raggio) {
        vec3 dir = centro - ver;

        objects.clear();

        shared_ptr<conoSenzaBase> conoA;
        conoA->PerCentroRaggio(ver, centro, raggio);
        objects.push_back(make_shared<disco>(centro, unit_vector(dir), raggio));
        objects.push_back(make_shared<conoSenzaBase>(conoA));
    }

    // metodo che inizializza con vertice, centro della base ed alpha
    void PerCentroRaggio(point3 ver, point3 centro, float al) {
        vec3 dir = centro - ver;
        float alt = dir.length();
        float radiusBase = alt * tan(al);

        objects.clear();

        shared_ptr<conoSenzaBase> conoA;
        conoA->PerDirezioneAlpha(ver, dir, al, alt);
        objects.push_back(make_shared<disco>(centro, unit_vector(dir), radiusBase));
        objects.push_back(make_shared<conoSenzaBase>(conoA));
        return;
    }
};

class cilindroSenzaBasi : public hittable {
public:

    // costruttore con centri delle basi e raggio
    cilindroSenzaBasi(point3 _vertice1, point3 _vertice2, float _radiusBase) : vertice1(_vertice1), vertice2(_vertice2), radiusBase(_radiusBase) {
        v = vertice2 - vertice1;
        h = v.length();
        v = unit_vector(v);

        // costruisco un vettore non nullo ed ortogonale a v
        if (abs(v.x()) >= abs(v.z()) || abs(v.y()) >= abs(v.z()))
            w1 = unit_vector(vec3(v.y(), -v.x(), 0.0f));
        else
            w1 = unit_vector(vec3(v.z(), 0.0f, -v.x()));

        // essendo v e w1 ortonormali allora w2 è unitario
        w2 = cross(v, w1);

        radiusBase_squared = radiusBase * radiusBase;
        return;
    }

    // costruttore con un centro, direzione, altezza e raggio
    cilindroSenzaBasi(point3 _vertice1, vec3 dir, float _h, float _radiusBase) : vertice1(_vertice1), h(_h), radiusBase(_radiusBase) {
        v = unit_vector(dir);
        vertice2 = vertice1 + h * v;

        // costruisco un vettore non nullo ed ortogonale a v
        if (abs(v.x()) >= abs(v.z()) || abs(v.y()) >= abs(v.z()))
            w1 = unit_vector(vec3(v.y(), -v.x(), 0.0f));
        else
            w1 = unit_vector(vec3(v.z(), 0.0f, -v.x()));

        // essendo v e w1 ortonormali allora w2 è unitario
        w2 = cross(v, w1);

        radiusBase_squared = radiusBase * radiusBase;
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
        //                + [dot(p0, w1)^2 + dot(p0, w2)^2 - radiusBase^2]


        vec3 d = r.d;
        point3 p0 = r.o - vertice1;
        float DdotW1 = dot(d, w1);
        float DdotW2 = dot(d, w2);
        float P0dotW1 = dot(p0, w1);
        float P0dotW2 = dot(p0, w2);

        float a = DdotW1 * DdotW1 + DdotW2 * DdotW2;
        float half_b = P0dotW1 * DdotW1 + P0dotW2 * DdotW2;
        float c = P0dotW1 * P0dotW1 + P0dotW2 * P0dotW2 - radiusBase_squared;

        float discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        float sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        float root = (-half_b - sqrtd) / a;
        // punto d'intersezione
        point3 intersezione = r.at(root);
        // altezza relativa dell'intersezione
        float altRel = dot(intersezione-vertice1, v);
        if (!ray_t.surrounds(root) || altRel < 0 || altRel > h) {
            root = (-half_b + sqrtd) / a;
            // punto d'intersezione
            point3 intersezione = r.at(root);
            // altezza relativa dell'intersezione
            float altRel = dot(intersezione-vertice1, v);
            if (!ray_t.surrounds(root) || altRel < 0 || altRel > h)
                return false;
        }


        rec.t = root;
        rec.p = intersezione;

        // cilindro: f(x,y,z)=0 con f(p)=dot(p - vertice, w1)^2 + dot(p - vertice, w2)^2 - radiusBase^2
        // calcolo le componenti 8dimezzate) del gradiente nel punto per avere la normale
        float gradx = w1.x() * dot(intersezione - vertice1, w1) + w2.x() * dot(intersezione - vertice1, w2);
        float grady = w1.y() * dot(intersezione - vertice1, w1) + w2.y() * dot(intersezione - vertice1, w2);
        float gradz = w1.z() * dot(intersezione - vertice1, w1) + w2.z() * dot(intersezione - vertice1, w2);

        //float scalare = pow(dot(intersezione - vertice, w1),2) + pow(dot(intersezione - vertice, w2),2) - pow( radiusBase ,2);
        //if( abs(scalare) >= 0.001 )
        //    cout << "f(p_t) = " << scalare << endl;

        vec3 outward_normal = unit_vector(vec3(gradx, grady, gradz));
        rec.set_face_normal(r, outward_normal);

        return true;
    }

private:
    point3 vertice1;
    point3 vertice2;
    // direzione apertura cono, di solito (0.0f, -1.0f, 0.0f)
    vec3 v;
    // versori in modo che v,w1,w2 sia un sistema ortonormale
    vec3 w1;
    vec3 w2;
    float radiusBase;
    float radiusBase_squared;
    // altezza cilindro
    float h;
};

class cilindroConBasi : public hittable_list {
public:

    // costruttore con centri delle basi e raggio
    cilindroConBasi(point3 vertice1, point3 vertice2, float radiusBase) {
        vec3 dir = unit_vector(vertice2 - vertice1);

        objects.clear();

        objects.push_back(make_shared<disco>(vertice1, -dir, radiusBase));
        objects.push_back(make_shared<disco>(vertice2, dir, radiusBase));
        objects.push_back(make_shared<cilindroSenzaBasi>(vertice1, vertice2, radiusBase));
    }

    // costruttore con un centro, direzione, altezza e raggio
    cilindroConBasi(point3 vertice1, vec3 dir, float h, float radiusBase) {
        dir = unit_vector(dir);

        objects.clear();

        objects.push_back(make_shared<disco>(vertice1, -dir, radiusBase));
        objects.push_back(make_shared<disco>(vertice1+h*dir, dir, radiusBase));
        objects.push_back(make_shared<cilindroSenzaBasi>(vertice1, dir, h, radiusBase));
    }
};

class paraboloide : public hittable {
public:

    // costruttore che prende due vettori non paralleli, rispettivamente asse del paraboloide e direzione del primo
    // diametro dell'ellisse che troviamo nelle sezioni (direzione del secondo diametro ottenuta come prodotto vettoriale)
    paraboloide(point3 _vertice, vec3 dir, vec3 dirx, float _h, float _a, float _b) : vertice(_vertice), h(_h), a(_a), b(_b) {
        v = unit_vector(dir);

        // tolgo a dirx la componente parallela a v e lo normalizzo
        w1 = unit_vector(dirx - dot(dirx, v) * v);
        w2 = cross(v, w1);
        // così w1, w2 e v sono ortonormali

        a_squared = a * a;
        b_squared = b * b;
        return;
    }

    // costruttore con vertice, direzione, altezza e coefficienti
    paraboloide(point3 _vertice, vec3 dir, float _h, float _a, float _b) : vertice(_vertice), h(_h), a(_a), b(_b) {
        v = unit_vector(dir);

        // costruisco un vettore non nullo ed ortogonale a v
        if (abs(v.x()) >= abs(v.z()) || abs(v.y()) >= abs(v.z()))
            w1 = unit_vector(vec3(v.y(), -v.x(), 0.0f));
        else
            w1 = unit_vector(vec3(v.z(), 0.0f, -v.x()));

        // essendo v e w1 ortonormali allora w2 è unitario
        w2 = cross(v, w1);

        a_squared = a * a;
        b_squared = b * b;
        return;
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {

        // paraboloide: f(x,y,z)=0 con f(p)=dot(p - vertice, w1)^2/a^2 + dot(p - vertice, w2)^2/b^2 - dot(p - vertice, v)
        // f(r.o + t d) = dot(r.o + t d - vertice, w1/a)^2 + dot(r.o + t d - vertice, w2/b)^2 - dot(r.o + t d - vertice,v)
        // chiamando p0 = r.o - vertice
        // chiamando tempW1 come w1/a e tempW2 come w2/b
        // f(r.o + t d) = dot(p0 + t d, w1)^2 + dot(p0 + t d, w2)^2 - dot(p0 + t d,v)  =
        //              = [dot(p0, w1) + t dot(d, w1)]^2 + [dot(p0, w2) + t dot(d, w2)]^2 - dot(p0,v) - t dot(d,v) =
        //              =   dot(p0, w1)^2 + t^2 dot(d, w1)^2 + 2 * t * dot(p0, w1) * dot(d, w1)
        //                + dot(p0, w2)^2 + t^2 dot(d, w2)^2 + 2 * t * dot(p0, w2) * dot(d, w2)
        //                - dot(p0,v) - t dot(d,v) =
        //              = t^2 [dot(d, w1)^2 + dot(d, w2)^2 ]
        //                + 2*t [dot(p0, w1) * dot(d, w1) + dot(p0, w2) * dot(d, w2) - dot(d,v)/2 ]
        //                + [dot(p0, w1)^2 + dot(p0, w2)^2 - dot(p0,v)]

        vec3 tempW1 = w1 / a;
        vec3 tempW2 = w2 / b;
        vec3 d = r.d;
        point3 p0 = r.o - vertice;
        float DdotW1 = dot(d, tempW1);
        float DdotW2 = dot(d, tempW2);
        float DdotV = dot(d, v);
        float P0dotW1 = dot(p0, tempW1);
        float P0dotW2 = dot(p0, tempW2);
        float P0dotV = dot(p0, v);

        float A = DdotW1 * DdotW1 + DdotW2 * DdotW2;
        float half_b = P0dotW1 * DdotW1 + P0dotW2 * DdotW2 - DdotV / 2;
        float c = P0dotW1 * P0dotW1 + P0dotW2 * P0dotW2 - P0dotV;

        float discriminant = half_b * half_b - A * c;
        if (discriminant < 0) return false;
        float sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        float root = (-half_b - sqrtd) / A;
        // punto d'intersezione
        point3 intersezione = r.at(root);
        // altezza relativa dell'intersezione
        float altRel = dot(intersezione - vertice, v);
        if (!ray_t.surrounds(root) || altRel < 0 || altRel > h) {
            root = (-half_b + sqrtd) / A;
            // punto d'intersezione
            point3 intersezione = r.at(root);
            // altezza relativa dell'intersezione
            float altRel = dot(intersezione - vertice, v);
            if (!ray_t.surrounds(root) || altRel < 0 || altRel > h)
                return false;
        }


        rec.t = root;
        rec.p = intersezione;

        // paraboloide: f(x,y,z)=0 con f(p)=dot(p - vertice, tempW1)^2 + dot(p - vertice, tempW2)^2 - dot(p - vertice, v)
        // calcolo le componenti (dimezzate) del gradiente nel punto per avere la normale
        float gradx = tempW1.x() * dot(intersezione - vertice, tempW1) + tempW2.x() * dot(intersezione - vertice, tempW2) - v.x() / 2;
        float grady = tempW1.y() * dot(intersezione - vertice, tempW1) + tempW2.y() * dot(intersezione - vertice, tempW2) - v.y() / 2;
        float gradz = tempW1.z() * dot(intersezione - vertice, tempW1) + tempW2.z() * dot(intersezione - vertice, tempW2) - v.z() / 2;


        vec3 outward_normal = unit_vector(vec3(gradx, grady, gradz));
        rec.set_face_normal(r, outward_normal);

        return true;
    }

private:
    point3 vertice;
    // direzione apertura paraboloide, di solito (0.0f, -1.0f, 0.0f)
    vec3 v;
    // versori in modo che v,w1,w2 sia un sistema ortonormale
    vec3 w1;
    vec3 w2;
    // coefficienti
    float a;
    float b;
    float a_squared;
    float b_squared;
    // altezza cilindro
    float h;
};

class iperb1falda : public hittable {
public:
    // iperboloide a una falda dati vertice, direzione e altezza
    iperb1falda(point3 ver, vec3 dir, float alt) {
        vertice = ver;
        h = alt;
        v = unit_vector(dir);

        // costruisco un vettore non nullo ed ortogonale a v
        if (abs(v.x()) >= abs(v.z()) || abs(v.y()) >= abs(v.z()))
            w1 = unit_vector(vec3(v.y(), -v.x(), 0.0f));
        else
            w1 = unit_vector(vec3(v.z(), 0.0f, -v.x()));

        // essendo v e w1 ortonormali allora w2 è unitario
        w2 = cross(v, w1);
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
        point3 p0 = r.o - vertice;
        float DdotW1 = dot(d, w1);
        float DdotW2 = dot(d, w2);
        float DdotV = dot(d, v);
        float P0dotW1 = dot(p0, w1);
        float P0dotW2 = dot(p0, w2);
        float P0dotV = dot(p0, v);

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
        float altRel = dot(intersezione - vertice, v);


        if (!ray_t.surrounds(root) || altRel < -h || altRel > h) {
            root = (-half_b + sqrtd) / a;
            // punto d'intersezione
            point3 intersezione = r.at(root);
            // altezza relativa dell'intersezione
            float altRel = dot(intersezione - vertice, v);
            if (!ray_t.surrounds(root) || altRel < -h || altRel > h)
                return false;
        }


        rec.t = root;
        rec.p = intersezione;

        // iperboloide: f(x,y,z)=0 con f(p)=dot(p - vertice, w1)^2 + dot(p - vertice, w2)^2 - dot(p - vertice,v)^2 - 1
        // calcolo le componenti 8dimezzate) del gradiente nel punto per avere la normale
        float gradx = w1.x() * dot(intersezione - vertice, w1) + w2.x() * dot(intersezione - vertice, w2) - v.x() * dot(intersezione - vertice, v);
        float grady = w1.y() * dot(intersezione - vertice, w1) + w2.y() * dot(intersezione - vertice, w2) - v.y() * dot(intersezione - vertice, v);
        float gradz = w1.z() * dot(intersezione - vertice, w1) + w2.z() * dot(intersezione - vertice, w2) - v.z() * dot(intersezione - vertice, v);

        //float scalare = pow(dot(intersezione - vertice, w1),2) + pow(dot(intersezione - vertice, w2),2) - pow( dot(intersezione - vertice,v) ,2) - 1;
        //if( abs(scalare) >= 0.001 )
        //    cout << "f(p_t) = " << scalare << endl;

        vec3 outward_normal = unit_vector(vec3(gradx, grady, gradz));
        rec.set_face_normal(r, outward_normal);

        return true;
    }

private:
    point3 vertice;
    // direzione apertura iperboloide, di solito (0.0f, -1.0f, 0.0f)
    vec3 v;
    // versori in modo che v,w1,w2 sia un sistema ortonormale
    vec3 w1;
    vec3 w2;
    // altezza a partire dal vertice
    float h;
};

class iperb2falde : public hittable {
public:
    // iperboloide a una falda dati vertice, direzione e altezza
    iperb2falde(point3 ver, vec3 dir, float alt) {
        vertice = ver;
        h = alt;
        v = unit_vector(dir);

        // costruisco un vettore non nullo ed ortogonale a v
        if (abs(v.x()) >= abs(v.z()) || abs(v.y()) >= abs(v.z()))
            w1 = unit_vector(vec3(v.y(), -v.x(), 0.0f));
        else
            w1 = unit_vector(vec3(v.z(), 0.0f, -v.x()));

        // essendo v e w1 ortonormali allora w2 è unitario
        w2 = cross(v, w1);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        
        // Iperb2falde: f(x,y,z)=0 con f(p)=dot(p - vertice, w1)^2 + dot(p - vertice, w2)^2 - dot(p - vertice,v)^2 + 1
        // f(r.o + t d) = dot(r.o + t d - vertice, w1)^2 + dot(r.o + t d - vertice, w2)^2 - dot(r.o + t d - vertice,v)^2 +1
        // chiamando p0 = r.o - vertice
        // f(r.o + t d) = dot(p0 + t d, w1)^2 + dot(p0 + t d, w2)^2 - dot(p0 + t d,v)^2 +1=
        //              = [dot(p0, w1) + t dot(d, w1)]^2 + [dot(p0, w2) + t dot(d, w2)]^2 - [ dot(p0,v) + t dot(d,v) ]^2 +1=
        //              =   dot(p0, w1)^2 + t^2 dot(d, w1)^2 + 2 * t * dot(p0, w1) * dot(d, w1)
        //                + dot(p0, w2)^2 + t^2 dot(d, w2)^2 + 2 * t * dot(p0, w2) * dot(d, w2)
        //                - dot(p0, v)^2 - t^2 dot(d, v)^2 - 2 * t * dot(p0, v) * dot(d, v) +1 =
        //              = t^2 [dot(d, w1)^2 + dot(d, w2)^2 - dot(d, v)^2 ]
        //                + 2*t [dot(p0, w1) * dot(d, w1) + dot(p0, w2) * dot(d, w2) - dot(p0, v) * dot(d, v)  ]
        //                + [dot(p0, w1)^2 + dot(p0, w2)^2 - dot(p0, v)^2 +1]


        vec3 d = r.d;
        point3 p0 = r.o - vertice;
        float DdotW1 = dot(d, w1);
        float DdotW2 = dot(d, w2);
        float DdotV = dot(d, v);
        float P0dotW1 = dot(p0, w1);
        float P0dotW2 = dot(p0, w2);
        float P0dotV = dot(p0, v);

        float a = DdotW1 * DdotW1 + DdotW2 * DdotW2 - DdotV * DdotV;
        float half_b = P0dotW1 * DdotW1 + P0dotW2 * DdotW2 - P0dotV * DdotV;
        float c = P0dotW1 * P0dotW1 + P0dotW2 * P0dotW2 - P0dotV * P0dotV +1;

        float discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        float sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        float root = (-half_b - sqrtd) / a;
        // punto d'intersezione
        point3 intersezione = r.at(root);
        // altezza relativa dell'intersezione
        float altRel = dot(intersezione - vertice, v);


        if (!ray_t.surrounds(root) || altRel < -h || altRel > h) {
            root = (-half_b + sqrtd) / a;
            // punto d'intersezione
            point3 intersezione = r.at(root);
            // altezza relativa dell'intersezione
            float altRel = dot(intersezione - vertice, v);
            if (!ray_t.surrounds(root) || altRel < -h || altRel > h)
                return false;
        }


        rec.t = root;
        rec.p = intersezione;

        // iperboloide: f(x,y,z)=0 con f(p)=dot(p - vertice, w1)^2 + dot(p - vertice, w2)^2 - dot(p - vertice,v)^2 + 1
        // calcolo le componenti 8dimezzate) del gradiente nel punto per avere la normale
        float gradx = w1.x() * dot(intersezione - vertice, w1) + w2.x() * dot(intersezione - vertice, w2) - v.x() * dot(intersezione - vertice, v);
        float grady = w1.y() * dot(intersezione - vertice, w1) + w2.y() * dot(intersezione - vertice, w2) - v.y() * dot(intersezione - vertice, v);
        float gradz = w1.z() * dot(intersezione - vertice, w1) + w2.z() * dot(intersezione - vertice, w2) - v.z() * dot(intersezione - vertice, v);

        //float scalare = pow(dot(intersezione - vertice, w1),2) + pow(dot(intersezione - vertice, w2),2) - pow( dot(intersezione - vertice,v) ,2) + 1;
        //if( abs(scalare) >= 0.001 )
        //    cout << "f(p_t) = " << scalare << endl;

        vec3 outward_normal = unit_vector(vec3(gradx, grady, gradz));
        rec.set_face_normal(r, outward_normal);

        return true;
    }

private:
    point3 vertice;
    // direzione apertura iperboloide, di solito (0.0f, -1.0f, 0.0f)
    vec3 v;
    // versori in modo che v,w1,w2 sia un sistema ortonormale
    vec3 w1;
    vec3 w2;
    // altezza a partire dal vertice
    float h;
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
        float altRel = intersezione.y() + 1;
        if (!ray_t.surrounds(root) || altRel < 0 || altRel > 2) {
            root = (-half_b + sqrtd) / a;
            // punto d'intersezione
            point3 intersezione = r.at(root);
            // altezza relativa dell'intersezione
            float altRel = intersezione.y() + 1;
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

        return true;
    }

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
    // normali verso l'esterno
    bool normEst = true;
};

#endif