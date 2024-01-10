// --------------------------------------------------------------------------
// This file is part of the pmpd software.
//
//    pmpd software is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    pmpd firmware is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with pmpd software. If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------------- 
//
// pmpd = physical modeling for pure data
// ch@chnry.net
// pmpd2d~
// The same than pmpd, but with audio data for input and output
// It can be used for particular physical modeling and for sound synthesis


#include <stdio.h>
#include <math.h>

#include "pmpd_export.h"
#include "pmpd_version.h"

#include "m_pd.h"

#define max(a,b) ( ((a) > (b)) ? (a) : (b) ) 
#define min(a,b) ( ((a) < (b)) ? (a) : (b) ) 

#define nb_max_link     10000
#define nb_max_mass     10000
#define nb_max_in       1000
#define nb_max_out      1000
#define nb_max_outlet   20
#define nb_max_inlet    20 // hard-coded on the methods definition

static t_class *pmpd2d_tilde_class;

struct _mass {
	t_float invM;
	t_float speedX;
	t_float speedY;
	t_float posX;
	t_float posY;
	t_float forceX;
	t_float forceY;
};

struct _link {
	struct _mass *mass1;
	struct _mass *mass2;
	t_float K1, D1; 
} ;

struct _NLlink {
	struct _mass *mass1;
	struct _mass *mass2;
	t_float K1, D1,L0,Lmin, Lmax, Pow; 
} ;

struct _inPos {
	// in en position
	t_int nbr_inlet;
	struct _mass *mass1;
	t_float influence;
} ;

struct _inForce {
	// in en force
	t_int nbr_inlet;
	struct _mass *mass1;
	t_float influence;
} ;

struct _outPos {
	// out en position
	t_int nbr_outlet;
	struct _mass *mass1;
	t_float influence;
} ;

struct _outSpeed {
	// out en vitesse
	t_int nbr_outlet;
	struct _mass *mass1;
	t_float influence;
} ;


typedef struct _pmpd2d_tilde {
	t_object  x_obj;
	struct _link link[nb_max_link];
	struct _NLlink NLlink[nb_max_link];
	struct _mass mass[nb_max_mass];
	struct _inPos inPosX[nb_max_in];
	struct _inPos inPosY[nb_max_in];
	struct _inForce inForceX[nb_max_in];
	struct _inForce inForceY[nb_max_in];
	struct _outPos outPosX[nb_max_out];
	struct _outPos outPosY[nb_max_out];
	struct _outSpeed outSpeed[nb_max_out];
	struct _outSpeed outSpeedX[nb_max_out];
	struct _outSpeed outSpeedY[nb_max_out];
	t_float outlet[nb_max_outlet];
	t_sample *outlet_vector[nb_max_outlet];
	t_sample *inlet_vector[nb_max_inlet];
	int nb_link, nb_NLlink, nb_mass, nb_inlet, nb_outlet, nb_inPosX, nb_inPosY, nb_inForceX, nb_inForceY, nb_outPosX, nb_outPosY, nb_outSpeedX, nb_outSpeedY, nb_outSpeed;
	t_sample f; // used for signal inlet
	t_int loop, nb_loop; // to be able not to compute everything a each iteration
	unsigned int x_state; // random
    t_float x_f; // random
} t_pmpd2d_tilde;

t_float pmpd2d_tilde_distance(struct _mass M1, struct _mass M2) {
	t_float distance, tmp;
	distance = (M2.posX - M1.posX);
	distance *= distance;
	tmp = (M2.posY - M1.posY);
	tmp *= tmp;
	distance += tmp;
	distance = sqrt(distance);
	return distance;
}

t_float pmpd2d_tilde_Ddt_distance(struct _mass M1, struct _mass M2) { // D/Dt(distance) = derivé de la distance = vitesse d'elongation d'un lien
	t_float distance, tmp;
	distance = (M2.speedX - M1.speedX);
	distance *= distance;
	tmp = (M2.speedY - M1.speedY);
	tmp *= tmp;
	distance += tmp;
	distance = sqrt(distance);
	return distance;
}

t_int *pmpd2d_tilde_perform(t_int *w)
///////////////////////////////////////////////////////////////////////////////////
{
	t_pmpd2d_tilde *x = (t_pmpd2d_tilde *)(w[1]);
	int n = (int)(w[2]);

	t_float F,L;
	t_int i;
//	struct _mass mass_1, mass_2;

	t_sample *out[nb_max_outlet]; 
	t_sample *in[nb_max_inlet];

	for (i=0; i<x->nb_inlet; i++) 
		in[i]= x->inlet_vector[i];

	for (i=0; i<x->nb_outlet; i++)
		out[i]= x->outlet_vector[i];

	while (n--) 
	{
		x->loop = 0;
		while (x->loop++ < x->nb_loop)
		{
			for (i=0; i<x->nb_inPosX; i++)
				// get inlet value and make it a position to the specified mass
				x->inPosX[i].mass1->posX = x->inPosX[i].influence * *in[x->inPosX[i].nbr_inlet];
			for (i=0; i<x->nb_inPosY; i++)
				x->inPosY[i].mass1->posY = x->inPosY[i].influence * *in[x->inPosY[i].nbr_inlet];
			for (i=0; i<x->nb_inForceX; i++)
				// get inlet value and make it a force to the specified mass
				x->inForceX[i].mass1->forceX += x->inForceX[i].influence * *in[x->inForceX[i].nbr_inlet];
			for (i=0; i<x->nb_inForceY; i++)
				x->inForceY[i].mass1->forceY += x->inForceY[i].influence * *in[x->inForceY[i].nbr_inlet];
			for (i=0; i<x->nb_link; i++)
			// compute forces generated by links (spring / dashpot)
			{
				F  = -x->link[i].K1 * pmpd2d_tilde_distance( *x->link[i].mass1, *x->link[i].mass2) ;
				// spring

				F -= x->link[i].D1 * pmpd2d_tilde_Ddt_distance( *x->link[i].mass1, *x->link[i].mass2) ;
				// dashpot
			
				x->link[i].mass1->forceX += F;
				x->link[i].mass2->forceX -= F;
			}

			for (i=0; i<x->nb_NLlink; i++)
			// compute forces generated by NLlinks (spring / dashpot)
			{
				L=pmpd2d_tilde_distance(*x->NLlink[i].mass1, *x->NLlink[i].mass2) - x->NLlink[i].L0;
				if ((L < x->NLlink[i].Lmax) && (L > x->NLlink[i].Lmin))
				{
					F  = x->NLlink[i].K1 * pow(fabs(L) ,x->NLlink[i].Pow) ;
					if (L < 0) F *= -1;
					// spring
	
					F -= x->NLlink[i].D1 * pmpd2d_tilde_Ddt_distance( *x->link[i].mass1, *x->link[i].mass2) ;
					// dashpot
				
					x->NLlink[i].mass1->forceX += F;
					x->NLlink[i].mass2->forceX -= F;
				}
			}
			for (i=1; i<x->nb_mass; i++)
			{
			// compute new masses position
			// a mass does not move if M=0 (i.e : invM = 0)
				x->mass[i].speedX += x->mass[i].forceX * x->mass[i].invM;
				x->mass[i].speedY += x->mass[i].forceY * x->mass[i].invM;
				x->mass[i].forceX = 0;
				x->mass[i].forceY = 0;
				x->mass[i].posX += x->mass[i].speedX ;
				x->mass[i].posY += x->mass[i].speedY ;
			}
		}	

		for (i=0; i<x->nb_inlet; i++)
		// increase pointer to inlet vectors value
			in[i]++;

		for (i=0; i<x->nb_outPosX; i++)
			x->outlet[x->outPosX[i].nbr_outlet] += x->outPosX[i].mass1->posX * x->outPosX[i].influence;
		for (i=0; i<x->nb_outPosY; i++)
			x->outlet[x->outPosY[i].nbr_outlet] += x->outPosY[i].mass1->posY * x->outPosY[i].influence;
			// compute output vector value		
		for (i=0; i<x->nb_outSpeedX; i++)
			x->outlet[x->outSpeedX[i].nbr_outlet] += x->outSpeedX[i].mass1->speedX * x->outSpeedX[i].influence;
		for (i=0; i<x->nb_outSpeed; i++)
			x->outlet[x->outSpeedY[i].nbr_outlet] += x->outSpeedY[i].mass1->speedY * x->outSpeedY[i].influence;
		for (i=0; i<x->nb_outSpeed; i++)
			x->outlet[x->outSpeed[i].nbr_outlet] += sqrt(x->outSpeed[i].mass1->speedX*x->outSpeed[i].mass1->speedX + x->outSpeed[i].mass1->speedY * x->outSpeed[i].mass1->speedY) * x->outSpeed[i].influence;
			// compute output vector value		

		for (i=0; i<x->nb_outlet; i++)
		// send vector value to the vector pointer
		{
			*out[i]++ = x->outlet[i];
			x->outlet[i] = 0;
		}
	}
	return(w+3);
}

void pmpd2d_tilde_dsp(t_pmpd2d_tilde *x, t_signal **sp)
{
	int i;
	for (i=0; i<x->nb_inlet; i++)
		x->inlet_vector[i] = sp[i]->s_vec;

	for (i=0; i<x->nb_outlet; i++)
		x->outlet_vector[i] = sp[i+x->nb_inlet]->s_vec;

	dsp_add(pmpd2d_tilde_perform, 2, x, sp[0]->s_n);
}

void pmpd2d_tilde_bang(t_pmpd2d_tilde *x)
{
}

void pmpd2d_tilde_float(t_pmpd2d_tilde *x, t_float force)
{
// add a force to all masses
	int i;
	for (i=0;i < x->nb_mass; i++)
		x->mass[i].forceX += force;
}

void pmpd2d_tilde_forceX(t_pmpd2d_tilde *x, t_float nbr_mass, t_float force)
{
// add a force to a specific mass
	if( (nbr_mass >= 0) && (nbr_mass < x->nb_mass) ) x->mass[(int)nbr_mass].forceX += force;
}
void pmpd2d_tilde_forceY(t_pmpd2d_tilde *x, t_float nbr_mass, t_float force)
{
// add a force to a specific mass
	if( (nbr_mass >= 0) && (nbr_mass < x->nb_mass) ) x->mass[(int)nbr_mass].forceY += force;
}

void pmpd2d_tilde_posX(t_pmpd2d_tilde *x, t_float nbr_mass, t_float posX)
{
// move a mass to a certain position
	if( (nbr_mass >= 0) && (nbr_mass < x->nb_mass) )  x->mass[(int)nbr_mass].posX = posX;
}
void pmpd2d_tilde_posY(t_pmpd2d_tilde *x, t_float nbr_mass, t_float posY)
{
// move a mass to a certain position
	if( (nbr_mass >= 0) && (nbr_mass < x->nb_mass) )  x->mass[(int)nbr_mass].posY = posY;
}

void pmpd2d_tilde_setM(t_pmpd2d_tilde *x, t_float nbr_mass, t_float M)
{
	if( (nbr_mass >= 0) && (nbr_mass < x->nb_mass) && (M>0)) x->mass[(int)nbr_mass].invM = 1./M;
}

void pmpd2d_tilde_setK(t_pmpd2d_tilde *x, t_float nbr_link, t_float K)
{
	if( (nbr_link >= 0) && (nbr_link < x->nb_link) ) x->link[(int)nbr_link].K1 = K;
}

void pmpd2d_tilde_setD(t_pmpd2d_tilde *x, t_float nbr_link, t_float D)
{
	if( (nbr_link >= 0) && (nbr_link < x->nb_link) ) x->link[(int)nbr_link].D1 = D;
}

void pmpd2d_tilde_mass(t_pmpd2d_tilde *x, t_float M, t_float posX, t_float posY)
// add a mass
//invM speedX posX force
{
	if (M<=0)
	{
		M = 0;
		x->mass[x->nb_mass].invM = 0;
	}
	else
		x->mass[x->nb_mass].invM = 1/M;

	x->mass[x->nb_mass].speedX = 0;
	x->mass[x->nb_mass].posX = posX;
	x->mass[x->nb_mass].posY = posY;
	x->mass[x->nb_mass].forceX = 0;

	x->nb_mass++ ;
	if (x->nb_mass == nb_max_mass) pd_error((t_object*)x, "to many mass");
	x->nb_mass = min ( nb_max_mass -1, x->nb_mass );
}

void pmpd2d_tilde_link(t_pmpd2d_tilde *x, t_float mass_1, t_float mass_2, t_float K1, t_float D1)
// add a link
// *mass1, *mass2, K1, D1;
{
	x->link[x->nb_link].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->link[x->nb_link].mass2 = &x->mass[max(0, min ( x->nb_mass, (int)mass_2))];
	x->link[x->nb_link].K1 = K1;
	x->link[x->nb_link].D1 = D1;

	x->nb_link++ ;
	if (x->nb_link == nb_max_link) pd_error((t_object*)x,"to many link");
	x->nb_link = min ( nb_max_link -1, x->nb_link );
}

void pmpd2d_tilde_NLlink(t_pmpd2d_tilde *x, t_symbol *s, int argc, t_atom *argv)
// t_float mass_1, t_float mass_2, t_float K1, t_float D1, t_float Pow, t_float Lmin, t_float Lmax, t_float L0
// add a NLlink
{
	if  (argc == 8) 
	{
		x->NLlink[x->nb_NLlink].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)atom_getfloatarg(0, argc, argv)))];
		x->NLlink[x->nb_NLlink].mass2 = &x->mass[max(0, min ( x->nb_mass, (int)atom_getfloatarg(1, argc, argv)))];
		x->NLlink[x->nb_NLlink].K1 = atom_getfloatarg(2, argc, argv);
		x->NLlink[x->nb_NLlink].D1 = atom_getfloatarg(3, argc, argv);
		x->NLlink[x->nb_NLlink].Pow = atom_getfloatarg(4, argc, argv);
		x->NLlink[x->nb_NLlink].L0 = atom_getfloatarg(5, argc, argv);
		x->NLlink[x->nb_NLlink].Lmin = atom_getfloatarg(6, argc, argv);
		x->NLlink[x->nb_NLlink].Lmax = atom_getfloatarg(7, argc, argv);

		x->nb_NLlink++ ;
		if (x->nb_NLlink == nb_max_link) pd_error((t_object*)x,"to many NLlink");
		x->nb_NLlink = min ( nb_max_link -1, x->nb_NLlink );
	}
	else
	pd_error((t_object*)x,"wrong argument number for NLlink");
}

void pmpd2d_tilde_inPosX(t_pmpd2d_tilde *x, t_float nb_inlet, t_float mass_1, t_float influence)
//add an input point
// nbr_inlet, *mass1, influence;
{
	x->inPosX[x->nb_inPosX].nbr_inlet = max(0, min( x->nb_inlet,(int)nb_inlet));
	x->inPosX[x->nb_inPosX].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->inPosX[x->nb_inPosX].influence = influence;

	x->nb_inPosX++;
	if (x->nb_inPosX == nb_max_in) {
		pd_error((t_object*)x,"to many inPosX");
		x->nb_inPosX--;
	}
}
void pmpd2d_tilde_inPosY(t_pmpd2d_tilde *x, t_float nb_inlet, t_float mass_1, t_float influence)
//add an input point
// nbr_inlet, *mass1, influence;
{
	x->inPosY[x->nb_inPosY].nbr_inlet = max(0, min( x->nb_inlet,(int)nb_inlet));
	x->inPosY[x->nb_inPosY].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->inPosY[x->nb_inPosY].influence = influence;

	x->nb_inPosY++;
	if (x->nb_inPosY == nb_max_in) {
		pd_error((t_object*)x,"to many inPosY");
		x->nb_inPosY--;
	}
}
void pmpd2d_tilde_inForceX(t_pmpd2d_tilde *x, t_float nb_inlet, t_float mass_1, t_float influence)
//add an input point
// nbr_inlet, *mass1, influence;
{
	x->inForceX[x->nb_inForceX].nbr_inlet = max(0, min( x->nb_inlet,(int)nb_inlet));
	x->inForceX[x->nb_inForceX].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->inForceX[x->nb_inForceX].influence = influence;

	x->nb_inForceX++;
	if (x->nb_inForceX == nb_max_in) {
		pd_error((t_object*)x,"to many inForceX");
		x->nb_inForceX--;
	}
}
void pmpd2d_tilde_inForceY(t_pmpd2d_tilde *x, t_float nb_inlet, t_float mass_1, t_float influence)
//add an input point
// nbr_inlet, *mass1, influence;
{
	x->inForceY[x->nb_inForceY].nbr_inlet = max(0, min( x->nb_inlet,(int)nb_inlet));
	x->inForceY[x->nb_inForceY].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->inForceY[x->nb_inForceY].influence = influence;

	x->nb_inForceY++;
	if (x->nb_inForceY == nb_max_in) {
		pd_error((t_object*)x,"to many inForceY");
		x->nb_inForceY--;
	}
}

void pmpd2d_tilde_outPosX(t_pmpd2d_tilde *x, t_float nb_outlet, t_float mass_1, t_float influence)
// add an output point
// nbr_outlet, *mass1, influence;
{
	x->outPosX[x->nb_outPosX].nbr_outlet = max(0, min( x->nb_outlet,(int)nb_outlet));
	x->outPosX[x->nb_outPosX].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->outPosX[x->nb_outPosX].influence = influence;

	x->nb_outPosX++ ;
	if (x->nb_outPosX == nb_max_out) {
		pd_error((t_object*)x,"to many outPosX");
		x->nb_outPosX--;
	}
}
void pmpd2d_tilde_outPosY(t_pmpd2d_tilde *x, t_float nb_outlet, t_float mass_1, t_float influence)
// add an output point
// nbr_outlet, *mass1, influence;
{
	x->outPosY[x->nb_outPosY].nbr_outlet = max(0, min( x->nb_outlet,(int)nb_outlet));
	x->outPosY[x->nb_outPosY].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->outPosY[x->nb_outPosY].influence = influence;

	x->nb_outPosY++ ;
	if (x->nb_outPosY == nb_max_out) {
		pd_error((t_object*)x,"to many outPosY");
		x->nb_outPosY--;
	}
}

void pmpd2d_tilde_outSpeedX(t_pmpd2d_tilde *x, t_float nb_outlet, t_float mass_1, t_float influence)
// add an output point
// nbr_outlet, *mass1, influence;
{
	x->outSpeedX[x->nb_outSpeedX].nbr_outlet = max(0, min( x->nb_outlet,(int)nb_outlet));
	x->outSpeedX[x->nb_outSpeedX].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->outSpeedX[x->nb_outSpeedX].influence = influence;

	x->nb_outSpeedX++ ;
	if (x->nb_outSpeedX == nb_max_out) {
		pd_error((t_object*)x,"to many outSpeedX");
		x->nb_outSpeedX--;
	}
}
void pmpd2d_tilde_outSpeedY(t_pmpd2d_tilde *x, t_float nb_outlet, t_float mass_1, t_float influence)
// add an output point
// nbr_outlet, *mass1, influence;
{
	x->outSpeedY[x->nb_outSpeedY].nbr_outlet = max(0, min( x->nb_outlet,(int)nb_outlet));
	x->outSpeedY[x->nb_outSpeedY].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->outSpeedY[x->nb_outSpeedY].influence = influence;

	x->nb_outSpeedY++ ;
	if (x->nb_outSpeedY == nb_max_out) {
		pd_error((t_object*)x,"to many outSpeedY");
		x->nb_outSpeedY--;
	}
}

void pmpd2d_tilde_outSpeed(t_pmpd2d_tilde *x, t_float nb_outlet, t_float mass_1, t_float influence)
// add an output point
// nbr_outlet, *mass1, influence;
{
	x->outSpeed[x->nb_outSpeed].nbr_outlet = max(0, min( x->nb_outlet,(int)nb_outlet));
	x->outSpeed[x->nb_outSpeed].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->outSpeed[x->nb_outSpeed].influence = influence;

	x->nb_outSpeed++ ;
	if (x->nb_outSpeed == nb_max_out) {
		pd_error((t_object*)x,"to many outSpeed");
		x->nb_outSpeed--;
	}
}


void pmpd2d_tilde_reset(t_pmpd2d_tilde *x)
{
	x->nb_link = 0;
	x->nb_NLlink = 0;
	x->nb_mass = 0;
	x->nb_inPosX = 0;
	x->nb_inPosY = 0;
	x->nb_inForceX = 0;
	x->nb_inForceY = 0;
	x->nb_outSpeedX = 0;
	x->nb_outSpeedY = 0;
	x->nb_outSpeed = 0;
	x->nb_outPosX = 0;
	x->nb_outPosY = 0;
}

void *pmpd2d_tilde_new(t_symbol *s, int argc, t_atom *argv)
{
	int i;
	t_pmpd2d_tilde *x = (t_pmpd2d_tilde *)pd_new(pmpd2d_tilde_class);

	pmpd2d_tilde_reset(x);
	
	x->nb_outlet= (int)atom_getfloatarg(1, argc, argv);
	x->nb_outlet= max(1, min(nb_max_outlet, x->nb_outlet) );
	for(i=0; i<x->nb_outlet; i++)
		outlet_new(&x->x_obj, &s_signal);

	x->nb_inlet = (int)atom_getfloatarg(0, argc, argv);
	x->nb_inlet= max(1, min(nb_max_inlet, x->nb_inlet) );
	for(i=0; i<x->nb_inlet-1; i++)
		inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);

	x->nb_loop = max (1, (int)atom_getfloatarg(2, argc, argv) );
	
	return (void *)x;
}

PMPD_EXPORT void pmpd2d_tilde_setup(void) {
	pmpd2d_tilde_class = class_new(gensym("pmpd2d~"), (t_newmethod)pmpd2d_tilde_new, 0, sizeof(t_pmpd2d_tilde), CLASS_DEFAULT, A_GIMME, 0);

    if(!pmpd2d_tilde_class)
        return;

    verbose(4, "pmpd2d~ version %s (%s)", pmpd_tag(), pmpd_sha());

	CLASS_MAINSIGNALIN(pmpd2d_tilde_class, t_pmpd2d_tilde, f);

	class_addbang(pmpd2d_tilde_class, pmpd2d_tilde_bang);
	class_addfloat(pmpd2d_tilde_class,  (t_method)pmpd2d_tilde_float);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_mass, gensym("mass"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_link, gensym("link"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_NLlink, gensym("NLlink"), A_GIMME, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_inPosX, gensym("inPosX"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_inPosY, gensym("inPosY"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_inForceX, gensym("inForceX"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_inForceY, gensym("inForceY"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_outPosX, gensym("outPosX"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_outPosY, gensym("outPosY"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_outSpeed, gensym("outSpeed"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_outSpeedX, gensym("outSpeedX"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_outSpeedY, gensym("outSpeedY"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_posX, gensym("posX"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_posY, gensym("posY"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_forceX, gensym("forceX"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_forceY, gensym("forceY"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_setK, gensym("setK"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_setD, gensym("setD"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_setM, gensym("setM"), A_DEFFLOAT, A_DEFFLOAT, 0);	
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_reset, gensym("reset"), 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_dsp, gensym("dsp"),  A_CANT, 0);
}
