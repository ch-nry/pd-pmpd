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
	t_float D; // damping de vitesse
	t_float Doffset;
	t_int Id;	
};

struct _link {
	struct _mass *mass1;
	struct _mass *mass2;
	t_float K1, D1, L0, L; 
};

struct _NLlink {
	struct _mass *mass1;
	struct _mass *mass2;
	t_float K1, D1,L0,Lmin, Lmax, Pow; 
};

struct _inPos {
	// in en position
	t_int nbr_inlet;
	struct _mass *mass1;
	t_float influence;
};

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
	t_sample *inlet_vector[nb_max_inlet];
	t_sample *outlet_vector[nb_max_outlet];
	int nb_link, nb_NLlink, nb_mass, nb_inlet, nb_outlet, nb_inPosX, nb_inPosY, nb_inForceX, nb_inForceY, nb_outPosX, nb_outPosY, nb_outSpeedX, nb_outSpeedY, nb_outSpeed;
	t_sample f; // used for signal inlet
	t_int loop, nb_loop; // to be able not to compute everything a each iteration
} t_pmpd2d_tilde;


t_int *pmpd2d_tilde_perform(t_int *w) {
///////////////////////////////////////////////////////////////////////////////////
	t_pmpd2d_tilde *x = (t_pmpd2d_tilde *)(w[1]);
	int n = (int)(w[2]);

	t_float F,FX,FY,L, LX, LY, tmp;
	t_int i;

	t_sample *out[nb_max_outlet]; 
	t_sample *in[nb_max_inlet];

	for (i=0; i<x->nb_inlet; i++) in[i]= x->inlet_vector[i];
	for (i=0; i<x->nb_outlet; i++) out[i]= x->outlet_vector[i];
	
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
				LX = x->link[i].mass2->posX - x->link[i].mass1->posX;
				LY = x->link[i].mass2->posY - x->link[i].mass1->posY;
				L = sqrt(LY*LY + LX*LX);
				F  = x->link[i].K1 * (L - x->link[i].L0);
				// spring
				
				tmp = (L - x->link[i].L);
				F  += x->link[i].D1 * (L - x->link[i].L); // on derive la longeur L calculé précedement
				x->link[i].L = L; // on la sauve pour la prochaine itération				
				// dashpot	

				if(L !=0 ) { // si L = 0 : on ne sais pas dans quel direction apliquer la force : c'est un point d'equilibre instable
					FX = F * LX/L;
    	            FY = F * LY/L;  
				} else {
					FX = 0;
					FY = 0;
				}

				x->link[i].mass1->forceX += FX;
				x->link[i].mass2->forceX -= FX;
				x->link[i].mass1->forceY += FY;
				x->link[i].mass2->forceY -= FY;		
			}

			for (i=0; i<x->nb_NLlink; i++)
			// compute forces generated by NLlinks (spring / dashpot)
			{
				LX = x->link[i].mass2->posX - x->link[i].mass1->posX;
				LY = x->link[i].mass2->posY - x->link[i].mass1->posY;
				L = sqrt(LY*LY + LX*LX);
				if ((L < x->NLlink[i].Lmax) && (L > x->NLlink[i].Lmin)) {
					F  = x->link[i].K1 * pow(fabs(L- x->link[i].L0) ,x->NLlink[i].Pow); 
					if (L < 0) F *= -1;
					// spring
					
					tmp = (L - x->link[i].L);
					F  += x->link[i].D1 * (L - x->link[i].L); // on derive la longeur L calculé précedement
					x->link[i].L = L; // on la sauve pour la prochaine itération				
					// dashpot	

					if(L !=0 ) { // si L = 0 : on ne sais pas dans quel direction apliquer la force : c'est un point d'equilibre instable
						FX = F * LX/L;
						FY = F * LY/L;  
					} else {
						FX = 0;
						FY = 0;
					}
					x->link[i].mass1->forceX += FX;
					x->link[i].mass2->forceX -= FX;
					x->link[i].mass1->forceY += FY;
					x->link[i].mass2->forceY -= FY;	
				}
			}
			for (i=0; i<x->nb_mass; i++)
			{
			// compute new masses position
			// a mass does not move if M=0 (i.e : invM = 0)
				if ( (x->mass[i].D != 0) && (x->mass[i].speedX != 0) && (x->mass[i].speedY != 0) ) { // amortissement en fct de la vitesse
					L = sqrt(x->mass[i].speedX * x->mass[i].speedX + x->mass[i].speedY * x->mass[i].speedY);
					F = (L - x->mass[i].Doffset) * x->mass[i].D;
				 	FX += F * x->mass[i].speedX / L;
				 	FY += F * x->mass[i].speedY / L;				 	
				}
				x->mass[i].speedX += x->mass[i].forceX * x->mass[i].invM;
				x->mass[i].speedY += x->mass[i].forceY * x->mass[i].invM;
				x->mass[i].forceX = 0;
				x->mass[i].forceY = 0;
				x->mass[i].posX += x->mass[i].speedX ;
				x->mass[i].posY += x->mass[i].speedY ;
			}
		}	

		for (i=0; i<x->nb_inlet; i++) in[i]++; // increase pointer to inlet vectors value

		// compute output vector value		
		for (i=0; i<x->nb_outlet; i++) x->outlet[i] = 0;
		for (i=0; i<x->nb_outPosX; i++) x->outlet[x->outPosX[i].nbr_outlet] += x->outPosX[i].mass1->posX * x->outPosX[i].influence;
		for (i=0; i<x->nb_outPosY; i++) x->outlet[x->outPosY[i].nbr_outlet] += x->outPosY[i].mass1->posY * x->outPosY[i].influence;
		for (i=0; i<x->nb_outSpeedX; i++) x->outlet[x->outSpeedX[i].nbr_outlet] += x->outSpeedX[i].mass1->speedX * x->outSpeedX[i].influence;
		for (i=0; i<x->nb_outSpeedY; i++) x->outlet[x->outSpeedY[i].nbr_outlet] += x->outSpeedY[i].mass1->speedY * x->outSpeedY[i].influence;
		for (i=0; i<x->nb_outSpeed; i++) x->outlet[x->outSpeed[i].nbr_outlet] += sqrt(x->outSpeed[i].mass1->speedX*x->outSpeed[i].mass1->speedX + x->outSpeed[i].mass1->speedY * x->outSpeed[i].mass1->speedY) * x->outSpeed[i].influence;

		for (i=0; i<x->nb_outlet; i++) *out[i]++ = x->outlet[i]; // send vector value to the vector pointer
	}
	return(w+3);
}

void pmpd2d_tilde_dsp(t_pmpd2d_tilde *x, t_signal **sp) {
	int i;
	for (i=0; i<x->nb_inlet; i++)
		x->inlet_vector[i] = sp[i]->s_vec;

	for (i=0; i<x->nb_outlet; i++)
		x->outlet_vector[i] = sp[i+x->nb_inlet]->s_vec;

	dsp_add(pmpd2d_tilde_perform, 2, x, sp[0]->s_n); // S_n : la taill du vecteur
}

void pmpd2d_tilde_bang(t_pmpd2d_tilde *x) { 
	t_int i;

	for (i=0; i<x->nb_mass; i++) logpost(x, 2, "mass:%ld, M:%f, posX:%f, posY:%f, D2:%f, D2offset:%f",i, x->mass[i].invM<=0.?0:1/x->mass[i].invM, x->mass[i].posX,x->mass[i].posY,x->mass[i].D,x->mass[i].Doffset);
	for (i=0; i<x->nb_link; i++) logpost(x, 2, "link:%ld, mass1:%ld, mass2:%ld, K:%f, D:%f, L0:%f", i, x->link[i].mass1->Id, x->link[i].mass2->Id, x->link[i].K1, x->link[i].D1, x->link[i].L0);
	for (i=0; i<x->nb_NLlink; i++) logpost(x, 2, "NLlink:%ld, mass1:%ld, mass2:%ld, K:%f, D:%f, L0:%f, Lmin:%f, Lmax:%f, Pow:%f", i, x->NLlink[i].mass1->Id, x->NLlink[i].mass2->Id, x->NLlink[i].K1, x->NLlink[i].D1, x->NLlink[i].L0, x->NLlink[i].Lmin, x->NLlink[i].Lmax, x->NLlink[i].Pow);
	for (i=0; i<x->nb_inPosX; i++) logpost(x, 2, "In_posX:%ld, Inlet:%ld, Mass:%ld, Amplitude:%f", i, x->inPosX[i].nbr_inlet, x->inPosX[i].mass1->Id, x->inPosX[i].influence);
	for (i=0; i<x->nb_inPosY; i++) logpost(x, 2, "In_posY:%ld, Inlet:%ld, Mass:%ld, Amplitude:%f", i, x->inPosY[i].nbr_inlet, x->inPosY[i].mass1->Id, x->inPosY[i].influence);
	for (i=0; i<x->nb_inForceX; i++) logpost(x, 2, "In_forceX:%ld, Inlet:%ld, Mass:%ld, Amplitude:%f", i, x->inForceX[i].nbr_inlet, x->inForceX[i].mass1->Id, x->inForceX[i].influence);
	for (i=0; i<x->nb_inForceY; i++) logpost(x, 2, "In_forceY:%ld, Inlet:%ld, Mass:%ld, Amplitude:%f", i, x->inForceY[i].nbr_inlet, x->inForceY[i].mass1->Id, x->inForceY[i].influence);
	for (i=0; i<x->nb_outPosX; i++) logpost(x, 2, "Out_posX:%ld, Inlet:%ld, Mass:%ld, Amplitude:%f", i, x->outPosX[i].nbr_outlet, x->outPosX[i].mass1->Id, x->outPosX[i].influence);
	for (i=0; i<x->nb_outPosY; i++) logpost(x, 2, "Out_posY:%ld, Inlet:%ld, Mass:%ld, Amplitude:%f", i, x->outPosY[i].nbr_outlet, x->outPosY[i].mass1->Id, x->outPosY[i].influence);
	for (i=0; i<x->nb_outSpeed; i++) logpost(x, 2, "Out_speed:%ld, Inlet:%ld, Mass:%ld, Amplitude:%f", i, x->outSpeed[i].nbr_outlet, x->outSpeed[i].mass1->Id, x->outSpeed[i].influence);
	for (i=0; i<x->nb_outSpeedX; i++) logpost(x, 2, "Out_speedX:%ld, Inlet:%ld, Mass:%ld, Amplitude:%f", i, x->outSpeedX[i].nbr_outlet, x->outSpeedX[i].mass1->Id, x->outSpeedX[i].influence);
	for (i=0; i<x->nb_outSpeedY; i++) logpost(x, 2, "Out_speedY:%ld, Inlet:%ld, Mass:%ld, Amplitude:%f", i, x->outSpeedY[i].nbr_outlet, x->outSpeedY[i].mass1->Id, x->outSpeedY[i].influence);
}

void pmpd2d_tilde_float(t_pmpd2d_tilde *x, t_float force) {}

void pmpd2d_tilde_forceX(t_pmpd2d_tilde *x, t_float nbr_mass, t_float force) {
// add a force to a specific mass
	if( (nbr_mass >= 0) && (nbr_mass < x->nb_mass) ) x->mass[(int)nbr_mass].forceX += force;
}

void pmpd2d_tilde_forceY(t_pmpd2d_tilde *x, t_float nbr_mass, t_float force) { // add a force to a specific mass
	if( (nbr_mass >= 0) && (nbr_mass < x->nb_mass) ) x->mass[(int)nbr_mass].forceY += force;
}

void pmpd2d_tilde_posX(t_pmpd2d_tilde *x, t_float nbr_mass, t_float posX) { // move a mass to a certain position
	if( (nbr_mass >= 0) && (nbr_mass < x->nb_mass) )  x->mass[(int)nbr_mass].posX = posX;
}

void pmpd2d_tilde_posY(t_pmpd2d_tilde *x, t_float nbr_mass, t_float posY) { // move a mass to a certain position
	if( (nbr_mass >= 0) && (nbr_mass < x->nb_mass) )  x->mass[(int)nbr_mass].posY = posY;
}

void pmpd2d_tilde_setD2(t_pmpd2d_tilde *x, t_float nbr_mass, t_float D) {
	if( (nbr_mass >= 0) && (nbr_mass < x->nb_mass) )  x->mass[(int)nbr_mass].D = D;
}

void pmpd2d_tilde_setD2offset(t_pmpd2d_tilde *x, t_float nbr_mass, t_float D) {
	if( (nbr_mass >= 0) && (nbr_mass < x->nb_mass) )  x->mass[(int)nbr_mass].Doffset = D;
}

void pmpd2d_tilde_setM(t_pmpd2d_tilde *x, t_float nbr_mass, t_float M) {
	if( (nbr_mass >= 0) && (nbr_mass < x->nb_mass)) {
		if (M> 0) x->mass[(int)nbr_mass].invM = 1./M;
		else x->mass[(int)nbr_mass].invM = 0;
	}
}

void pmpd2d_tilde_setK(t_pmpd2d_tilde *x, t_float nbr_link, t_float K) {
	if( (nbr_link >= 0) && (nbr_link < x->nb_link) ) x->link[(int)nbr_link].K1 = K;
}

void pmpd2d_tilde_setD(t_pmpd2d_tilde *x, t_float nbr_link, t_float D) {
	if( (nbr_link >= 0) && (nbr_link < x->nb_link) ) x->link[(int)nbr_link].D1 = D;
}

void pmpd2d_tilde_setL(t_pmpd2d_tilde *x, t_float nbr_link, t_float L) {
	if( (nbr_link >= 0) && (nbr_link < x->nb_link) ) x->link[(int)nbr_link].L0 = L;
}

void pmpd2d_tilde_mass(t_pmpd2d_tilde *x, t_float M, t_float posX, t_float posY, t_float D) {
// add a mass
//invM speedX posX force
	if (M<=0)
	{
		M = 0;
		x->mass[x->nb_mass].invM = 0; 
	}
	else
		x->mass[x->nb_mass].invM = 1/M;

	x->mass[x->nb_mass].speedX = 0;
	x->mass[x->nb_mass].speedY = 0;
	x->mass[x->nb_mass].posX = posX;
	x->mass[x->nb_mass].posY = posY;
	x->mass[x->nb_mass].forceX = 0;
	x->mass[x->nb_mass].forceY = 0;
	x->mass[x->nb_mass].D = D;
	x->mass[x->nb_mass].Doffset = 0;
	x->mass[x->nb_mass].Id = x->nb_mass;	

	x->nb_mass++ ;
	if (x->nb_mass >= nb_max_mass) {
		logpost(x,1, "to many mass");
		x->nb_mass--;
	}
}

void pmpd2d_tilde_link(t_pmpd2d_tilde *x, t_float mass_1, t_float mass_2, t_float K1, t_float D1, t_float L0) {
// add a link
// *mass1, *mass2, K1, D1;
	x->link[x->nb_link].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->link[x->nb_link].mass2 = &x->mass[max(0, min ( x->nb_mass, (int)mass_2))];
	x->link[x->nb_link].K1 = K1;
	x->link[x->nb_link].D1 = D1;
	x->link[x->nb_link].L0 = L0;
	x->link[x->nb_link].L = L0;

	x->nb_link++ ;
	if (x->nb_link >= nb_max_link) {
		logpost(x,1,"to many link");
		x->nb_link--;
	}
}

void pmpd2d_tilde_NLlink(t_pmpd2d_tilde *x, t_symbol *s, int argc, t_atom *argv) {
// t_float mass_1, t_float mass_2, t_float K1, t_float D1, t_float Pow, t_float Lmin, t_float Lmax, t_float L0
// add a NLlink
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
		if (x->nb_NLlink == nb_max_link) {
			logpost(x,1,"to many NLlink");
			x->nb_NLlink--;
		}
	}
	else
	logpost(x,1,"wrong argument number for NLlink");
}

void pmpd2d_tilde_inPosX(t_pmpd2d_tilde *x, t_float nb_inlet, t_float mass_1, t_float influence) {
//add an input point
// nbr_inlet, *mass1, influence;
	x->inPosX[x->nb_inPosX].nbr_inlet = max(0, min( x->nb_inlet,(int)nb_inlet));
	x->inPosX[x->nb_inPosX].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->inPosX[x->nb_inPosX].influence = influence;

	x->nb_inPosX++;
	if (x->nb_inPosX == nb_max_in) {
		logpost(x,1,"to many inPosX");
		x->nb_inPosX--;
	}
}

void pmpd2d_tilde_inPosY(t_pmpd2d_tilde *x, t_float nb_inlet, t_float mass_1, t_float influence) {
//add an input point
// nbr_inlet, *mass1, influence;
	x->inPosY[x->nb_inPosY].nbr_inlet = max(0, min( x->nb_inlet,(int)nb_inlet));
	x->inPosY[x->nb_inPosY].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->inPosY[x->nb_inPosY].influence = influence;

	x->nb_inPosY++;
	if (x->nb_inPosY == nb_max_in) {
		logpost(x,1,"to many inPosY");
		x->nb_inPosY--;
	}
}

void pmpd2d_tilde_inForceX(t_pmpd2d_tilde *x, t_float nb_inlet, t_float mass_1, t_float influence) {
	x->inForceX[x->nb_inForceX].nbr_inlet = max(0, min( x->nb_inlet,(int)nb_inlet));
	x->inForceX[x->nb_inForceX].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->inForceX[x->nb_inForceX].influence = influence;

	x->nb_inForceX++;
	if (x->nb_inForceX == nb_max_in) {
		logpost(x,1,"to many inForceX");
		x->nb_inForceX--;
	}
}

void pmpd2d_tilde_inForceY(t_pmpd2d_tilde *x, t_float nb_inlet, t_float mass_1, t_float influence) {
	x->inForceY[x->nb_inForceY].nbr_inlet = max(0, min( x->nb_inlet,(int)nb_inlet));
	x->inForceY[x->nb_inForceY].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->inForceY[x->nb_inForceY].influence = influence;

	x->nb_inForceY++;
	if (x->nb_inForceY == nb_max_in) {
		logpost(x,1,"to many inForceY");
		x->nb_inForceY--;
	}
}

void pmpd2d_tilde_outPosX(t_pmpd2d_tilde *x, t_float nb_outlet, t_float mass_1, t_float influence) {
	x->outPosX[x->nb_outPosX].nbr_outlet = max(0, min( x->nb_outlet,(int)nb_outlet));
	x->outPosX[x->nb_outPosX].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->outPosX[x->nb_outPosX].influence = influence;

	x->nb_outPosX++ ;
	if (x->nb_outPosX == nb_max_out) {
		logpost(x,1,"to many outPosX");
		x->nb_outPosX--;
	}
}

void pmpd2d_tilde_outPosY(t_pmpd2d_tilde *x, t_float nb_outlet, t_float mass_1, t_float influence) {
	x->outPosY[x->nb_outPosY].nbr_outlet = max(0, min( x->nb_outlet,(int)nb_outlet));
	x->outPosY[x->nb_outPosY].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->outPosY[x->nb_outPosY].influence = influence;

	x->nb_outPosY++ ;
	if (x->nb_outPosY == nb_max_out) {
		logpost(x,1,"to many outPosY");
		x->nb_outPosY--;
	}
}

void pmpd2d_tilde_outSpeedX(t_pmpd2d_tilde *x, t_float nb_outlet, t_float mass_1, t_float influence) {
	x->outSpeedX[x->nb_outSpeedX].nbr_outlet = max(0, min( x->nb_outlet,(int)nb_outlet));
	x->outSpeedX[x->nb_outSpeedX].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->outSpeedX[x->nb_outSpeedX].influence = influence;

	x->nb_outSpeedX++ ;
	if (x->nb_outSpeedX == nb_max_out) {
		logpost(x,1,"to many outSpeedX");
		x->nb_outSpeedX--;
	}
}

void pmpd2d_tilde_outSpeedY(t_pmpd2d_tilde *x, t_float nb_outlet, t_float mass_1, t_float influence) {
	x->outSpeedY[x->nb_outSpeedY].nbr_outlet = max(0, min( x->nb_outlet,(int)nb_outlet));
	x->outSpeedY[x->nb_outSpeedY].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->outSpeedY[x->nb_outSpeedY].influence = influence;

	x->nb_outSpeedY++ ;
	if (x->nb_outSpeedY == nb_max_out) {
		logpost(x,1,"to many outSpeedY");
		x->nb_outSpeedY--;
	}
}

void pmpd2d_tilde_outSpeed(t_pmpd2d_tilde *x, t_float nb_outlet, t_float mass_1, t_float influence) {
	x->outSpeed[x->nb_outSpeed].nbr_outlet = max(0, min( x->nb_outlet,(int)nb_outlet));
	x->outSpeed[x->nb_outSpeed].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->outSpeed[x->nb_outSpeed].influence = influence;

	x->nb_outSpeed++ ;
	if (x->nb_outSpeed == nb_max_out) {
		logpost(x,1,"to many outSpeed");
		x->nb_outSpeed--;
	}
}

void pmpd2d_tilde_reset(t_pmpd2d_tilde *x) {
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

void *pmpd2d_tilde_new(t_symbol *s, int argc, t_atom *argv) {
	int i;
	t_pmpd2d_tilde *x = (t_pmpd2d_tilde *)pd_new(pmpd2d_tilde_class);

	pmpd2d_tilde_reset(x);
	
	x->nb_inlet = (int)atom_getfloatarg(0, argc, argv);
	x->nb_inlet= max(1, min(nb_max_inlet, x->nb_inlet) );
	for(i=0; i<x->nb_inlet-1; i++)
		inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);

	x->nb_outlet= (int)atom_getfloatarg(1, argc, argv);
	x->nb_outlet= max(1, min(nb_max_outlet, x->nb_outlet) );
	for(i=0; i<x->nb_outlet; i++)
		outlet_new(&x->x_obj, &s_signal);
		
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
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_mass, gensym("mass"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
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
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_setD2, gensym("setD2"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_setD2offset, gensym("setD2offset"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_setK, gensym("setK"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_setD, gensym("setD"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_setL, gensym("setL"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_setM, gensym("setM"), A_DEFFLOAT, A_DEFFLOAT, 0);	
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_reset, gensym("reset"), 0);
	class_addmethod(pmpd2d_tilde_class, (t_method)pmpd2d_tilde_dsp, gensym("dsp"),  A_CANT, 0);
}