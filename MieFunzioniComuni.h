#ifndef MIEFUNZIONICOMUNI_H
#define MIEFUNZIONICOMUNI_H

// Common Headers
#include "camera.h"
#include "hittable_list.h"
#include "interval.h"


inline vec3 RimuoviComponenteParallela(vec3 partenza, vec3 ortogonale) {
	// prende il vettore "partenza" e ci sottrae la componente parallela
	// al vettore "ortogonale", così rimane la sola componente ortogonale
	// risultato utilizzato:
	// < v, w - v * <v,w>/|v|^2 > = <v,w> - <v,v> * <v,w>/|v|^2 =
	//                            = <v,w> - <v,w> = 0
	// così w = (w - v * <v,w>/|v|^2) + v * <v,w>/|v|^2
	// dove (w - v * <v,w>/|v|^2) è ortogonale a v mentre v * <v,w>/|v|^2
	// è parallelo a v
	return partenza - ortogonale * (dot(partenza, ortogonale) / ortogonale.length_squared());
}


inline bool InterRagLato(const ray& r, interval ray_t, point3 vertice1, vec3 latoA, vec3 nA, vec3 n, hit_record& rec) {
	// immaginiamo che il raggio ed il lato giacciano in un piano
	// r è il raggio
	// il lato è un segmento che parte da vertice1 ed arriva al vertice2
	// dove vertice2 non è esplicitamente dato ma indirettamente dato che
	// latoA = vertice2-vertice1
	// nA è un versore ortogonale al latoA (ma giace anch'esso sul piano)
	// n è un vettore ortogonale al piano
	// 
	// Verifichiamo se c'è intersezione tra i punti del raggio (origine escluso)
	// ed il segmento (vertice1,latoA); se c'è salviamo l'intersezione in rec
	// 
	// Supponiamo che r.o non sia sul lato

	// raggio interseca la retta r:(vertice1 + s latoA) all'istante
	// sviluppando i calcoli p_t appartiene ad r <=>
	// <=> t * dot(r.d,nA) = dot( vertice1 - r.o, nA)

	float scalare1 = dot(r.d, nA);
	float scalare2 = dot(vertice1 - r.o, nA);

	if (scalare1 == 0) {
		// r è parallelo al latoA

		if (scalare2 == 0) {
			// r e latoA giacciono su rette parallele ma distinte e quinti disgiunte 
			return false;
		}

		// r e latoA giacciono sulla stessa retta r
		// dato che r.o non giace sul lato le uniche intersezioni possono
		// essere i vertici, a seconda della direzione di r
		// istanti in cui r interseca i vertici
		// istante vertice1 = dot(vertice1 - r.o, r.d)
		float istante = dot(vertice1 - r.o, r.d);

		if (dot(r.d, latoA)>0) {
			// dato che r.o non giace sul segmento può essere a destra o a sinistra 
			// dello stesso.
			// Poichè r è diretto come latoA si ha:
			//   1)  r.o a detra del segmento <=> vertice1 ha istante negativo
			//       inoltre in questo caso non ci sono intersezioni
			//   2)  r.o a sinistra del segmento <=> vertice1 ha istante positivo
			//       inoltre in questo caso l'intersezione è proprio vertice1
			
			// verifica contemporanea che istante>o ed istante appartiene all'intervallo:
			if (ray_t.surrounds(istante)) {
				rec.t = istante;
				rec.p = vertice1;
				rec.set_face_normal(r, n);
			}
			return false;
		}
		
		// qui dot(r.d, latoA) < 0)
		// dato che r.o non giace sul segmento può essere a destra o a sinistra 
		// dello stesso.
		// Poichè r è diretto con verso opposto a latoA si ha:
		//   1)  r.o a sinistra del segmento <=> vertice1 ha istante negativo
		//       inoltre in questo caso non ci sono intersezioni
		//   2)  r.o a destra del segmento <=> vertice1 ha istante positivo
		//       inoltre in questo caso l'intersezione è proprio vertice2

		// verifica contemporanea che istante>o ed istante appartiene all'intervallo:
		if (ray_t.surrounds(istante)) {
			// istante vertice2 = dot(vertice2 - r.o, r.d) =
			//                  = dot(vertice1 + latoA - r.o, r.d) =
			//                  = dot(vertice1 - r.o, r.d) + dot(latoA, r.d) =
			//                  = istante + dot(latoA, r.d)
			istante += dot(latoA, r.d);
			rec.t = istante;
			rec.p = vertice1 + latoA;
			rec.set_face_normal(r, n);
		}
		return false;
	}

	// qui raggio e lato non sono paralleli, quindi tra le rette su cui giaciono
	// vi è un'unica intersezione ed avviene all'istante
	float istante = scalare2 / scalare1;
	if (!ray_t.surrounds(istante))
		return false;

	point3 puntoIntersezione = r.at(istante);
	vec3 posizioneNellaRetta = puntoIntersezione - vertice1;
	float coordinataIntersezione = dot(posizioneNellaRetta, latoA);
	if (coordinataIntersezione >= 0 && coordinataIntersezione <= latoA.length_squared()) {
		rec.p = puntoIntersezione;
		rec.t = istante;
		rec.set_face_normal(r, n);
		return true;
	}
	return false;
}


#endif