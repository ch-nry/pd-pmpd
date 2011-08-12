/*
--------------------------  pmpd3d  ---------------------------------------- 
     
	  
 pmpd = physical modeling for pure data                                     
 Written by Cyrille Henry (ch@chnry.net)
 
 Get sources on pd svn on sourceforge.

 This program is free software; you can redistribute it and/or                
 modify it under the terms of the GNU General Public License                  
 as published by the Free Software Foundation; either version 2               
 of the License, or (at your option) any later version.                       
                                                                             
 This program is distributed in the hope that it will be useful,              
 but WITHOUT ANY WARRANTY; without even the implied warranty of               
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                
 GNU General Public License for more details.                           
                                                                              
 You should have received a copy of the GNU General Public License           
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
                                                                              
 Based on PureData by Miller Puckette and others.                             
                                                                             

----------------------------------------------------------------------------
*/

#include "m_pd.h"
#include "stdio.h"
#include "math.h"

#define nb_max_link   100000
#define nb_max_mass   100000

#define max(a,b) ( ((a) > (b)) ? (a) : (b) ) 
#define min(a,b) ( ((a) < (b)) ? (a) : (b) ) 

t_float sign_ch(t_float v)
{
    return v > 0 ? 1 : -1;
}

t_float sqr(t_float x)
{
	return x*x ;
}

t_float pow_ch(t_float x, t_float y)
{
	return x > 0 ? pow(x,y) : -pow(-x,y);
}
	
static t_class *pmpd3d_class;

typedef struct _mass {
    t_symbol *Id;
    int mobile;
	t_float invM;
	t_float speedX;
	t_float speedY;
	t_float speedZ;
	t_float posX;
	t_float posY;
	t_float posZ;
	t_float forceX;
	t_float forceY;
	t_float forceZ;
	t_float D2;
	t_float D2offset;
    int num;
} foo;

typedef struct _link {
    t_symbol *Id;
    int lType;
	struct _mass *mass1;
	struct _mass *mass2;
	t_float K;
    t_float D;
    t_float L;
    t_float Pow;
    t_float Lmin;
    t_float Lmax;
    t_float distance;
    t_float VX; // vecteur portant la liaison, si c'est le cas
    t_float VY;
    t_float VZ;
    t_symbol *arrayK;
    t_symbol *arrayD;
	t_float K_L; // longeur du tabeau K
	t_float D_L; // longeur du tabeau D
} foo1 ;

typedef struct _pmpd3d {
 	t_object  x_obj;
	struct _link link[nb_max_link];
	struct _mass mass[nb_max_mass];
	t_outlet *main_outlet;
    t_outlet *info_outlet;
    int nb_link;
	int nb_mass;
    t_float minX, maxX, minY, maxY, minZ, maxZ;
    t_int grab; // si on grab une mass ou pas
	t_int grab_nb; // la masse grabé
} t_pmpd3d;

t_float tabread2(t_pmpd3d *x, t_float pos, t_symbol *array)
{
    t_garray *a;
	int npoints;
	t_word *vec;
	t_float posx;
	
    if (!(a = (t_garray *)pd_findbyclass(array, garray_class)))
        pd_error(x, "%s: no such array", array->s_name);
    else if (!garray_getfloatwords(a, &npoints, &vec))
        pd_error(x, "%s: bad template for tabLink", array->s_name);
	else
    {
		posx = fabs(pos)*npoints;
		int n=posx;
		if (n >= npoints - 1) 
			return (sign_ch(pos)*vec[npoints-1].w_float);
		float fract = posx-n;
		return (sign_ch(pos) * ( fract*vec[n+1].w_float+(1-fract)*vec[n].w_float));
	}
	return( pos); // si il y a un pb sur le tableau, on renvoie l'identité
}

void pmpd3d_reset(t_pmpd3d *x)
{
	x->nb_link = 0;
	x->nb_mass = 0;
    x->minX = -1000000;
    x->maxX = 1000000;
    x->minY = -1000000;
    x->maxY = 1000000;
    x->minZ = -1000000;
    x->maxZ = 1000000;
    x->grab = 0;
}

void pmpd3d_infosL(t_pmpd3d *x)
{
    int i;
    post("list of mass");
    post("number, Id, mobile, mass, Damping, positionX Y Z, speedX Y Z, forcesX Y Z");
	for(i=0; i < x->nb_mass; i++)
    {
        post("masse %i: %s, %d, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f",i, x->mass[i].Id->s_name, \
			x->mass[i].mobile, 1/x->mass[i].invM, x->mass[i].D2, x->mass[i].posX, x->mass[i].posY, \
			x->mass[i].posZ, x->mass[i].speedX, x->mass[i].speedY, x->mass[i].speedZ, \
			x->mass[i].forceX, x->mass[i].forceY, x->mass[i].forceZ );
    }

    post("list of link");
    post("number, Id, mass1, mass2, K, D, Pow, L, Lmin, Lmax");
	for(i=0; i < x->nb_link; i++)
    {	
		switch (x->link[i].lType)
		{
		case 0:
			post("link %i: %s, %i, %i, %f, %f, %f, %f, %f, %f", i, x->link[i].Id->s_name, \
				x->link[i].mass1->num, x->link[i].mass2->num, x->link[i].K, x->link[i].D, \
				x->link[i].Pow, x->link[i].L, x->link[i].Lmin, x->link[i].Lmax);
			break;
		case 1:
			post("tLink %i: %s, %i, %i, %f, %f, %f, %f, %f, %f, %f, %f, %f", i, x->link[i].Id->s_name, \
				x->link[i].mass1->num, x->link[i].mass2->num, x->link[i].K, x->link[i].D, \
				x->link[i].Pow, x->link[i].L, x->link[i].Lmin, x->link[i].Lmax, \
				x->link[i].VX, x->link[i].VY, x->link[i].VZ);
			break;
		case 2:
			post("tabLink %i: %s, %i, %i, %f, %f, %s, %f, %s, %f", i, x->link[i].Id->s_name, \
				x->link[i].mass1->num, x->link[i].mass2->num, x->link[i].K, x->link[i].D, \
				x->link[i].arrayK->s_name, x->link[i].K_L, x->link[i].arrayD->s_name, x->link[i].D_L);
			break;		
		}
    }
}

void pmpd3d_bang(t_pmpd3d *x)
{
// this part is doing all the PM
	t_float F, L, Lx,Ly, Lz, Fx, Fy, Fz, tmpX, tmpY, tmpZ, speed;
	t_int i;
    // post("bang");

	for (i=0; i<x->nb_mass; i++)
	// compute new masses position
		if (x->mass[i].mobile > 0) // only if mobile
		{
			x->mass[i].speedX += x->mass[i].forceX * x->mass[i].invM;
			x->mass[i].speedY += x->mass[i].forceY * x->mass[i].invM;
			x->mass[i].speedZ += x->mass[i].forceZ * x->mass[i].invM;
			// x->mass[i].forceX = 0;
			// x->mass[i].forceY = 0;		
			// x->mass[i].forceZ = 0;		
			x->mass[i].posX += x->mass[i].speedX ;
			x->mass[i].posY += x->mass[i].speedY ;
			x->mass[i].posZ += x->mass[i].speedZ ;
            if ( (x->mass[i].posX < x->minX) || (x->mass[i].posX > x->maxX) || (x->mass[i].posY < x->minY) 
				|| (x->mass[i].posY > x->maxY) || (x->mass[i].posZ < x->minZ) || (x->mass[i].posZ > x->maxZ) ) 
            {
				tmpX = min(x->maxX,max(x->minX,x->mass[i].posX));
				tmpY = min(x->maxY,max(x->minY,x->mass[i].posY));
				tmpZ = min(x->maxZ,max(x->minZ,x->mass[i].posZ));
				x->mass[i].speedX -= x->mass[i].posX - tmpX;
				x->mass[i].speedY -= x->mass[i].posY - tmpY;
				x->mass[i].speedZ -= x->mass[i].posZ - tmpZ;
				x->mass[i].posX = tmpX;
				x->mass[i].posY = tmpY;
				x->mass[i].posZ = tmpZ;
			}
			x->mass[i].forceX = -x->mass[i].D2 * x->mass[i].speedX;
			x->mass[i].forceY = -x->mass[i].D2 * x->mass[i].speedY;
			x->mass[i].forceZ = -x->mass[i].D2 * x->mass[i].speedZ;
			speed = sqrt(x->mass[i].speedX * x->mass[i].speedX + x->mass[i].speedY * x->mass[i].speedY + x->mass[i].speedZ * x->mass[i].speedZ);
            if (speed != 0) {
                x->mass[i].forceX += x->mass[i].D2offset * (x->mass[i].speedX/speed);
                x->mass[i].forceY += x->mass[i].D2offset * (x->mass[i].speedY/speed);
                x->mass[i].forceZ += x->mass[i].D2offset * (x->mass[i].speedZ/speed);
			}
		}

	for (i=0; i<x->nb_link; i++)
	// compute link forces
	{
		Lx = x->link[i].mass1->posX - x->link[i].mass2->posX;
		Ly = x->link[i].mass1->posY - x->link[i].mass2->posY;
		Lz = x->link[i].mass1->posZ - x->link[i].mass2->posZ;
		L = sqrt( sqr(Lx) + sqr(Ly) + sqr(Lz) );
		
		if ( (L >= x->link[i].Lmin) && (L < x->link[i].Lmax)  && (L != 0))
		{
			if (x->link[i].lType == 2)
			{ // K et D viennent d'une table
				F  = x->link[i].D * tabread2(x, (L - x->link[i].distance) / x->link[i].D_L, x->link[i].arrayD);
				F += x->link[i].K * tabread2(x, L / x->link[i].K_L, x->link[i].arrayK);
			}
			else
			{			
				F  = x->link[i].D * (L - x->link[i].distance) ;
				F += x->link[i].K *  pow_ch( L - x->link[i].L, x->link[i].Pow);
			}

			Fx = F * Lx/L;
			Fy = F * Ly/L;	
			Fz = F * Lz/L;	
				
			if (x->link[i].lType == 1)
			{ // on projette selon 1 axe
				Fx = Fx*x->link[i].VX; // V est unitaire, dc on projete sans pb
				Fy = Fy*x->link[i].VY;				
				Fz = Fz*x->link[i].VZ;				
			}
			
			x->link[i].mass1->forceX -= Fx;
			x->link[i].mass1->forceY -= Fy;
			x->link[i].mass1->forceZ -= Fz;
			x->link[i].mass2->forceX += Fx;
			x->link[i].mass2->forceY += Fy;
			x->link[i].mass2->forceZ += Fz;
		}
		x->link[i].distance=L;
	}
}

void pmpd3d_mass(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
//, t_symbol *Id, t_float mobile, t_float M, t_float posX, t_float posY, t_float posZ )
{ // add a mass : Id, invM speedX posX
	if(argv[0].a_type == A_SYMBOL)
	{
		x->mass[x->nb_mass].Id = atom_getsymbolarg(0,argc,argv);
		x->mass[x->nb_mass].mobile = (int) atom_getfloatarg(1, argc, argv);
		t_float M = atom_getfloatarg(2, argc, argv);
		if (M<=0) M=1;
		x->mass[x->nb_mass].invM = 1/M;
		x->mass[x->nb_mass].speedX = 0;
		x->mass[x->nb_mass].speedY = 0;
		x->mass[x->nb_mass].speedZ = 0;
		x->mass[x->nb_mass].posX = atom_getfloatarg(3, argc, argv);
		x->mass[x->nb_mass].posY = atom_getfloatarg(4, argc, argv);
		x->mass[x->nb_mass].posZ = atom_getfloatarg(5, argc, argv);
		x->mass[x->nb_mass].forceX = 0;
		x->mass[x->nb_mass].forceY = 0;
		x->mass[x->nb_mass].forceZ = 0;
		x->mass[x->nb_mass].num = x->nb_mass;
		x->mass[x->nb_mass].D2 = 0;
		x->mass[x->nb_mass].D2offset = 0;


		x->nb_mass++ ;
		x->nb_mass = min ( nb_max_mass -1, x->nb_mass );
	}
}

void pmpd3d_create_link(t_pmpd3d *x, t_symbol *Id, int mass1, int mass2, t_float K, t_float D, t_float Pow, t_float Lmin, t_float Lmax, t_int type)
{ // create a link based on mass number

    if ((x->nb_mass>1) && (mass1 != mass2) && (mass1 >= 0) && (mass2 >= 0) && (mass1 < x->nb_mass) && (mass2 < x->nb_mass) )
    {
	    x->link[x->nb_link].lType = type;
	    x->link[x->nb_link].Id = Id;
	    x->link[x->nb_link].mass1 = &x->mass[mass1]; 
	    x->link[x->nb_link].mass2 = &x->mass[mass2];
	    x->link[x->nb_link].K = K;
	    x->link[x->nb_link].D = D;
	    x->link[x->nb_link].L = sqrt(sqr(x->mass[mass1].posX - x->mass[mass2].posX) + 
			sqr(x->mass[mass1].posY - x->mass[mass2].posY) + sqr(x->mass[mass1].posZ - x->mass[mass2].posZ));
	    x->link[x->nb_link].Pow = Pow;
	    x->link[x->nb_link].Lmin = Lmin;
	    x->link[x->nb_link].Lmax = Lmax;
	    x->link[x->nb_link].distance = x->link[x->nb_link].L ;

	    x->nb_link++ ;
	    x->nb_link = min ( nb_max_link -1, x->nb_link );
    }
}

void pmpd3d_link(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{ // add a link : Id, *mass1, *mass2, K, D, Pow, Lmin, Lmax;

    int i, j;

    t_symbol *Id = atom_getsymbolarg(0,argc,argv);
    int mass1 = atom_getfloatarg(1, argc, argv);
    int mass2 = atom_getfloatarg(2, argc, argv);
    t_float K = atom_getfloatarg(3, argc, argv);
    t_float D = atom_getfloatarg(4, argc, argv);
    t_float Pow = 1; 
    if (argc > 5) Pow = atom_getfloatarg(5, argc, argv);
    t_float Lmin = -1000000;
    if (argc > 6) Lmin = atom_getfloatarg(6, argc, argv);
    t_float Lmax =  1000000;
    if (argc > 7) Lmax = atom_getfloatarg(7, argc, argv);
//    post("%d,%d, %f,%f", mass1, mass2, K, D);

    if ( ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) )
    {
        pmpd3d_create_link(x, Id, mass1, mass2, K, D, Pow, Lmin, Lmax, 0);
    }
    else
    if ( ( argv[1].a_type == A_SYMBOL ) && ( argv[2].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                pmpd3d_create_link(x, Id, i, mass2, K, D, Pow, Lmin, Lmax, 0);
            }
        }
    }
    else
    if ( ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(2,argc,argv) == x->mass[i].Id)
            {
                pmpd3d_create_link(x, Id, mass1, i, K, D, Pow, Lmin, Lmax, 0);
            }
        }
    }
    else
    if ( ( argv[1].a_type == A_SYMBOL ) && ( argv[2].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            for (j=0; j< x->nb_mass; j++)
            {
                if ( (atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)&(atom_getsymbolarg(2,argc,argv) == x->mass[j].Id))
                {
					if (!( (x->mass[i].Id == x->mass[j].Id) && (i>j) )) 
					// si lien entre 2 serie de masses identique entres elle, alors on ne creer qu'un lien sur 2, pour evider les redondances
						pmpd3d_create_link(x, Id, i, j, K, D, Pow, Lmin, Lmax, 0);
                }
            }   
        }
    }
}

void pmpd3d_tLink(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{ // add a link : Id, *mass1, *mass2, K, D, Vx, Vy, Pow, Lmin, Lmax;

    int i, j;
	
    t_symbol *Id = atom_getsymbolarg(0,argc,argv);
    int mass1 = atom_getfloatarg(1, argc, argv);
    int mass2 = atom_getfloatarg(2, argc, argv);
    t_float K = atom_getfloatarg(3, argc, argv);
    t_float D = atom_getfloatarg(4, argc, argv);
    t_float vecteurX = atom_getfloatarg(5, argc, argv);
    t_float vecteurY = atom_getfloatarg(6, argc, argv);
    t_float vecteurZ = atom_getfloatarg(7, argc, argv);
    t_float vecteur = sqrt( sqr(vecteurX) + sqr(vecteurY) + sqr(vecteurZ) );
    vecteurX /= vecteur;
    vecteurY /= vecteur;
    vecteurZ /= vecteur;
    t_float Pow = 1; 
    if (argc > 8) Pow = atom_getfloatarg(8, argc, argv);
    t_float Lmin = 0;
    if (argc > 9) Lmin = atom_getfloatarg(9, argc, argv);
    t_float Lmax =  1000000;
    if (argc > 10) Lmax = atom_getfloatarg(10, argc, argv);

    if ( ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) )
    {
        pmpd3d_create_link(x, Id, mass1, mass2, K, D, Pow, Lmin, Lmax, 1);
		x->link[x->nb_link-1].VX = vecteurX;
		x->link[x->nb_link-1].VY = vecteurY;
		x->link[x->nb_link-1].VZ = vecteurZ;
    }
    else
    if ( ( argv[1].a_type == A_SYMBOL ) && ( argv[2].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                pmpd3d_create_link(x, Id, i, mass2, K, D, Pow, Lmin, Lmax, 1);
            	x->link[x->nb_link-1].VX = vecteurX;
		x->link[x->nb_link-1].VY = vecteurY;
		x->link[x->nb_link-1].VZ = vecteurZ;
            }
        }
    }
    else
    if ( ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(2,argc,argv) == x->mass[i].Id)
            {
                pmpd3d_create_link(x, Id, mass1, i, K, D, Pow, Lmin, Lmax, 1);
            	x->link[x->nb_link-1].VX = vecteurX;
		x->link[x->nb_link-1].VY = vecteurY;
		x->link[x->nb_link-1].VZ = vecteurZ;
	    }
        }
    }
    else
    if ( ( argv[1].a_type == A_SYMBOL ) && ( argv[2].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            for (j=0; j< x->nb_mass; j++)
            {
                if ( (atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)&(atom_getsymbolarg(2,argc,argv) == x->mass[j].Id))
                {
			if (!( (x->mass[i].Id == x->mass[j].Id) && (i>j) )) 
			// si lien entre 2 serie de masses identique entres elle, alors on ne creer qu'un lien sur 2, pour evider les redondances
			{
				pmpd3d_create_link(x, Id, i, j, K, D, Pow, Lmin, Lmax, 1);
				x->link[x->nb_link-1].VX = vecteurX;
				x->link[x->nb_link-1].VY = vecteurY;
				x->link[x->nb_link-1].VZ = vecteurZ;
			}
		}
            }   
        }
    }
}

void pmpd3d_tabLink(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j;
	
    t_symbol *Id = atom_getsymbolarg(0,argc,argv);
    int mass1 = atom_getfloatarg(1, argc, argv);
    int mass2 = atom_getfloatarg(2, argc, argv);
    t_symbol *arrayK = atom_getsymbolarg(3,argc,argv);
    t_float Kl = atom_getfloatarg(4, argc, argv);
	if (Kl <= 0) Kl = 1;
    t_symbol *arrayD = atom_getsymbolarg(5,argc,argv);    
    t_float Dl = atom_getfloatarg(6, argc, argv);
	if (Dl <= 0) Dl = 1;

    if ( ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) )
    {
        pmpd3d_create_link(x, Id, mass1, mass2, 1, 1, 1, 0, 1000000, 2);
		x->link[x->nb_link-1].arrayK = arrayK;
		x->link[x->nb_link-1].arrayD = arrayD;
		x->link[x->nb_link-1].K_L = Kl;
		x->link[x->nb_link-1].D_L = Dl;		
    }
    else
    if ( ( argv[1].a_type == A_SYMBOL ) && ( argv[2].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                pmpd3d_create_link(x, Id, i, mass2, 1, 1, 1, 0, 1000000, 2);
				x->link[x->nb_link-1].arrayK = arrayK;
				x->link[x->nb_link-1].arrayD = arrayD;
				x->link[x->nb_link-1].K_L = Kl;
				x->link[x->nb_link-1].D_L = Dl;	
            }
        }
    }
    else
    if ( ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(2,argc,argv) == x->mass[i].Id)
            {
                pmpd3d_create_link(x, Id, mass1, i, 1, 1, 1, 0, 1000000, 2);
				x->link[x->nb_link-1].arrayK = arrayK;
				x->link[x->nb_link-1].arrayD = arrayD;
				x->link[x->nb_link-1].K_L = Kl;
				x->link[x->nb_link-1].D_L = Dl;	
			}
        }
    }
    else
    if ( ( argv[1].a_type == A_SYMBOL ) && ( argv[2].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            for (j=0; j< x->nb_mass; j++)
            {
                if ( (atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)&(atom_getsymbolarg(2,argc,argv) == x->mass[j].Id))
                {
					if (!( (x->mass[i].Id == x->mass[j].Id) && (i>j) )) 
					// si lien entre 2 serie de masses identique entres elle, alors on ne creer qu'un lien sur 2, pour evider les redondances
					{
						pmpd3d_create_link(x, Id, i, j, 1, 1, 1, 0, 1000000, 2);
						x->link[x->nb_link-1].arrayK = arrayK;
						x->link[x->nb_link-1].arrayD = arrayD;
						x->link[x->nb_link-1].K_L = Kl;
						x->link[x->nb_link-1].D_L = Dl;	
					}
				}
            }   
        }
    }

}

void pmpd3d_setK(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
	    x->link[tmp].K = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
	            x->link[i].K = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd3d_setD(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
	    x->link[tmp].D = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
	            x->link[i].D = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd3d_setL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
	    x->link[tmp].L = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
	            x->link[i].L = atom_getfloatarg(1, argc, argv);
            }
        }
    }
	if ( ( argv[0].a_type == A_FLOAT ) && ( argc == 1 ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].L = x->link[tmp].mass2->posX - x->link[tmp].mass1->posX;
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argc == 1 ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
                x->link[i].L = x->link[i].mass2->posX - x->link[i].mass1->posX;
            }
        }
    }
}

void pmpd3d_setLKTab(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;
	t_float K_l = atom_getfloatarg(1, argc, argv);
	if (K_l <=  0) K_l = 1;
    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
	    x->link[tmp].K_L = K_l;
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
	            x->link[i].K_L = K_l;
            }
        }
    }
}

void pmpd3d_setLDTab(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;
	t_float D_l = atom_getfloatarg(1, argc, argv);
	if (D_l <=  0) D_l = 1;
    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
	    x->link[tmp].D_L = D_l;
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
	            x->link[i].D_L = D_l;
            }
        }
    }
}

void pmpd3d_setLinkId(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_SYMBOL ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
	    x->link[tmp].Id = atom_getsymbolarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
	            x->link[i].Id = atom_getsymbolarg(1, argc, argv);
            }
        }
    }
}

void pmpd3d_setMassId(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_SYMBOL ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].Id = atom_getsymbolarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].Id = atom_getsymbolarg(1, argc, argv);
            }
        }
    }
}

void pmpd3d_forceX(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// add a force to a specific mass
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].forceX += atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].forceX += atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd3d_forceY(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// add a force to a specific mass
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].forceY += atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].forceY += atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd3d_forceZ(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// add a force to a specific mass
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].forceZ += atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].forceZ += atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd3d_force(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// add a force to a specific mass
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].forceX += atom_getfloatarg(1, argc, argv);
	    x->mass[tmp].forceY += atom_getfloatarg(2, argc, argv);
	    x->mass[tmp].forceZ += atom_getfloatarg(3, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].forceX += atom_getfloatarg(1, argc, argv);
	            x->mass[i].forceY += atom_getfloatarg(2, argc, argv);
				x->mass[i].forceZ += atom_getfloatarg(3, argc, argv);
            }
        }
    }
}

void pmpd3d_posX(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// displace a mass to a certain position
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].posX = atom_getfloatarg(1, argc, argv);
   	    x->mass[tmp].speedX = 0; 
        x->mass[tmp].forceX = 0; 
        
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].posX = atom_getfloatarg(1, argc, argv);
        	    x->mass[i].speedX = 0; 
                x->mass[i].forceX = 0;

            }
        }
    }
}

void pmpd3d_posY(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// displace a mass to a certain position
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].posY = atom_getfloatarg(1, argc, argv);
   	    x->mass[tmp].speedY = 0; 
        x->mass[tmp].forceY = 0; 
        
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].posY = atom_getfloatarg(1, argc, argv);
        	    x->mass[i].speedY = 0; 
                x->mass[i].forceY = 0;
            }
        }
    }
}

void pmpd3d_posZ(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// displace a mass to a certain position
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].posZ = atom_getfloatarg(1, argc, argv);
   	    x->mass[tmp].speedZ = 0; 
        x->mass[tmp].forceZ = 0; 
        
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].posZ = atom_getfloatarg(1, argc, argv);
        	    x->mass[i].speedZ = 0; 
                x->mass[i].forceZ = 0;
            }
        }
    }
}

void pmpd3d_pos(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// displace a mass to a certain position
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].posX = atom_getfloatarg(1, argc, argv);
   	    x->mass[tmp].speedX = 0; 
        x->mass[tmp].forceX = 0; 
   	    x->mass[tmp].posY = atom_getfloatarg(2, argc, argv);
   	    x->mass[tmp].speedY = 0; 
        x->mass[tmp].forceY = 0;
   	    x->mass[tmp].posZ = atom_getfloatarg(3, argc, argv);
   	    x->mass[tmp].speedZ = 0; 
        x->mass[tmp].forceZ = 0; 
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].posX = atom_getfloatarg(1, argc, argv);
        	    x->mass[i].speedX = 0; 
                x->mass[i].forceX = 0;
                x->mass[i].posY = atom_getfloatarg(2, argc, argv);
        	    x->mass[i].speedY = 0; 
                x->mass[i].forceY = 0;
                x->mass[i].posZ = atom_getfloatarg(3, argc, argv);
        	    x->mass[i].speedZ = 0; 
                x->mass[i].forceZ = 0;
            }
        }
    }
}

void pmpd3d_minX(t_pmpd3d *x, t_float min)
{
	x->minX = min;
}

void pmpd3d_maxX(t_pmpd3d *x, t_float max)
{
	x->maxX = max;
}

void pmpd3d_minY(t_pmpd3d *x, t_float min)
{
	x->minY = min;
}

void pmpd3d_maxY(t_pmpd3d *x, t_float max)
{
	x->maxY = max;
}

void pmpd3d_minZ(t_pmpd3d *x, t_float min)
{
	x->minZ = min;
}

void pmpd3d_maxZ(t_pmpd3d *x, t_float max)
{
	x->maxZ = max;
}

void pmpd3d_min(t_pmpd3d *x, t_float minX, t_float minY, t_float minZ)
{
	x->minX = minX;
	x->minY = minY;
	x->minZ = minZ;
}

void pmpd3d_max(t_pmpd3d *x, t_float maxX, t_float maxY, t_float maxZ)
{
	x->maxX = maxX;
	x->maxY = maxY;
	x->maxZ = maxZ;
}

void pmpd3d_setFixed(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( argv[0].a_type == A_FLOAT ) 
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].mobile = 0;
    }
    if ( argv[0].a_type == A_SYMBOL )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].mobile = 0;
            }
        }
    }
}

void pmpd3d_setMobile(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( argv[0].a_type == A_FLOAT )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].mobile = 1;
    }
    if ( argv[0].a_type == A_SYMBOL ) 
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].mobile = 1;
            }
        }
    }
}

void pmpd3d_setD2(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].D2 = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].D2 = atom_getfloatarg(1, argc, argv);
            }
        }
    }
    if ( ( argv[0].a_type == A_FLOAT ) && ( argc == 1 ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
			x->mass[i].D2 = atom_getfloatarg(0, argc, argv);
        }
    }
}

void pmpd3d_setD2offset(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].D2offset = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].D2offset = atom_getfloatarg(1, argc, argv);
            }
        }
    }
    if ( ( argv[0].a_type == A_FLOAT ) && ( argc == 1 ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
			x->mass[i].D2offset = atom_getfloatarg(0, argc, argv);
        }
    }
}

void pmpd3d_setSpeedX(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].speedX = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].speedX = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd3d_setSpeedY(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].speedY = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].speedY = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd3d_setSpeedZ(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].speedZ = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].speedZ = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd3d_setSpeed(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].speedX = atom_getfloatarg(1, argc, argv);
	    x->mass[tmp].speedY = atom_getfloatarg(2, argc, argv);
	    x->mass[tmp].speedZ = atom_getfloatarg(3, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].speedX = atom_getfloatarg(1, argc, argv);
	            x->mass[i].speedY = atom_getfloatarg(2, argc, argv);
	            x->mass[i].speedZ = atom_getfloatarg(3, argc, argv);
            }
        }
    }
}

void pmpd3d_addPosX(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].posX += atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].posX += atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd3d_addPosY(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].posY += atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].posY += atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd3d_addPosZ(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].posZ += atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].posZ += atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd3d_addPos(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].posX += atom_getfloatarg(1, argc, argv);
	    x->mass[tmp].posY += atom_getfloatarg(2, argc, argv);
	    x->mass[tmp].posZ += atom_getfloatarg(3, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].posX += atom_getfloatarg(1, argc, argv);
	            x->mass[i].posY += atom_getfloatarg(2, argc, argv);
	            x->mass[i].posZ += atom_getfloatarg(3, argc, argv);
            }
        }
    }
}

void pmpd3d_setForceX(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].forceX = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].forceX = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd3d_setForceY(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].forceY = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].forceY = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd3d_setForceZ(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].forceZ = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].forceZ = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd3d_setForce(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].forceX = atom_getfloatarg(1, argc, argv);
	    x->mass[tmp].forceY = atom_getfloatarg(2, argc, argv);
	    x->mass[tmp].forceZ = atom_getfloatarg(3, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].forceX = atom_getfloatarg(1, argc, argv);
	            x->mass[i].forceY = atom_getfloatarg(2, argc, argv);
	            x->mass[i].forceZ = atom_getfloatarg(3, argc, argv);
            }
        }
    }
}

void pmpd3d_get(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_symbol *toget; 
    t_atom  toout[7];
    toget = atom_getsymbolarg(0, argc, argv);

    if ( (toget == gensym("massesPos")) && (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) && (i<x->nb_mass) )
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->mass[i].posX);
            SETFLOAT(&(toout[2]), x->mass[i].posY);
            SETFLOAT(&(toout[3]), x->mass[i].posZ);
            outlet_anything(x->main_outlet, gensym("massesPosNo"), 4, toout);
        }  
    }
    else
    if ( (toget == gensym("massesPos")) && (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                SETFLOAT(&(toout[0]), i);
                SETFLOAT(&(toout[1]), x->mass[i].posX);
                SETFLOAT(&(toout[2]), x->mass[i].posY);
                SETFLOAT(&(toout[3]), x->mass[i].posZ);
                outlet_anything(x->main_outlet, gensym("massesPosId"), 4, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("massesPos")) && (argc == 1) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->mass[i].posX);
            SETFLOAT(&(toout[2]), x->mass[i].posY);
            SETFLOAT(&(toout[3]), x->mass[i].posZ);
            outlet_anything(x->main_outlet, gensym("massesPos"), 4, toout);
        } 
    }
    else
    if ( (toget == gensym("massesPosName")) && (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) && (i<x->nb_mass) )
        {
            SETSYMBOL(&(toout[0]), x->mass[i].Id);
            SETFLOAT(&(toout[1]), x->mass[i].posX);
            SETFLOAT(&(toout[2]), x->mass[i].posY);
            SETFLOAT(&(toout[3]), x->mass[i].posZ);
            outlet_anything(x->main_outlet, gensym("massesPosNameNo"), 4, toout);
        }  
    }
    else
    if ( (toget == gensym("massesPosName")) && (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                SETSYMBOL(&(toout[0]), x->mass[i].Id);
                SETFLOAT(&(toout[1]), x->mass[i].posX);
				SETFLOAT(&(toout[2]), x->mass[i].posY);
				SETFLOAT(&(toout[3]), x->mass[i].posZ);
                outlet_anything(x->main_outlet, gensym("massesPosNameId"), 4, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("massesPosName")) && (argc == 1) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            SETSYMBOL(&(toout[0]), x->mass[i].Id);
            SETFLOAT(&(toout[1]), x->mass[i].posX);
            SETFLOAT(&(toout[2]), x->mass[i].posY);
            SETFLOAT(&(toout[3]), x->mass[i].posZ);
            outlet_anything(x->main_outlet, gensym("massesPosName"), 4, toout);
        } 
    }
    else
    if ( (toget == gensym("massesSpeeds")) && (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) && (i<x->nb_mass) )
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->mass[i].speedX);
            SETFLOAT(&(toout[2]), x->mass[i].speedY);
            SETFLOAT(&(toout[3]), x->mass[i].speedZ);
            outlet_anything(x->main_outlet, gensym("massesSpeedsNo"), 4, toout);
        }  
    }
    else
    if ( (toget == gensym("massesSpeeds")) && (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                SETFLOAT(&(toout[0]), i);
                SETFLOAT(&(toout[1]), x->mass[i].speedX);
				SETFLOAT(&(toout[2]), x->mass[i].speedY);
				SETFLOAT(&(toout[3]), x->mass[i].speedZ);
                outlet_anything(x->main_outlet, gensym("massesSpeedsId"), 4, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("massesSpeeds")) && (argc == 1) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->mass[i].speedX);
            SETFLOAT(&(toout[2]), x->mass[i].speedY);
            SETFLOAT(&(toout[3]), x->mass[i].speedZ);
            outlet_anything(x->main_outlet, gensym("massesSpeeds"), 4, toout);
        } 
    }
    else
    if ( (toget == gensym("massesSpeedsName")) && (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) && (i<x->nb_mass) )
        {
            SETSYMBOL(&(toout[0]), x->mass[i].Id);
            SETFLOAT(&(toout[1]), x->mass[i].speedX);
            SETFLOAT(&(toout[2]), x->mass[i].speedY);
            SETFLOAT(&(toout[3]), x->mass[i].speedZ);
            outlet_anything(x->main_outlet, gensym("massesSpeedsNameNo"), 4, toout);
        }  
    }
    else
    if ( (toget == gensym("massesSpeedsName")) && (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                SETSYMBOL(&(toout[0]), x->mass[i].Id);
                SETFLOAT(&(toout[1]), x->mass[i].speedX);
				SETFLOAT(&(toout[2]), x->mass[i].speedY);
				SETFLOAT(&(toout[3]), x->mass[i].speedZ);
                outlet_anything(x->main_outlet, gensym("massesSpeedsNameId"), 4, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("massesSpeedsName")) && (argc == 1) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            SETSYMBOL(&(toout[0]), x->mass[i].Id);
            SETFLOAT(&(toout[1]), x->mass[i].speedX);
            SETFLOAT(&(toout[2]), x->mass[i].speedY);
            SETFLOAT(&(toout[3]), x->mass[i].speedZ);
            outlet_anything(x->main_outlet, gensym("massesSpeedsName"), 4, toout);
        } 
    }
    else
    if ( (toget == gensym("massesForces")) && (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) && (i<x->nb_mass) )
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->mass[i].forceX);
            SETFLOAT(&(toout[2]), x->mass[i].forceY);
            SETFLOAT(&(toout[3]), x->mass[i].forceZ);
            outlet_anything(x->main_outlet, gensym("massesForcesNo"), 4, toout);
        }  
    }
    else
    if ( (toget == gensym("massesForces")) && (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                SETFLOAT(&(toout[0]), i);
                SETFLOAT(&(toout[1]), x->mass[i].forceX);
				SETFLOAT(&(toout[2]), x->mass[i].forceY);
				SETFLOAT(&(toout[3]), x->mass[i].forceZ);
                outlet_anything(x->main_outlet, gensym("massesForcesId"), 4, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("massesForces")) && (argc == 1) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->mass[i].forceX);
            SETFLOAT(&(toout[2]), x->mass[i].forceY);
            SETFLOAT(&(toout[3]), x->mass[i].forceZ);
            outlet_anything(x->main_outlet, gensym("massesForces"), 4, toout);
        } 
    }
    else
    if ( (toget == gensym("massesForcesName")) && (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) && (i<x->nb_mass) )
        {
            SETSYMBOL(&(toout[0]), x->mass[i].Id);
            SETFLOAT(&(toout[1]), x->mass[i].forceX);
            SETFLOAT(&(toout[2]), x->mass[i].forceY);
            SETFLOAT(&(toout[3]), x->mass[i].forceZ);
            outlet_anything(x->main_outlet, gensym("massesForcesNameNo"), 4, toout);
        }
    }
    else
    if ( (toget == gensym("massesForcesName")) && (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                SETSYMBOL(&(toout[0]), x->mass[i].Id);
                SETFLOAT(&(toout[1]), x->mass[i].forceX);
				SETFLOAT(&(toout[2]), x->mass[i].forceY);
				SETFLOAT(&(toout[3]), x->mass[i].forceZ);
                outlet_anything(x->main_outlet, gensym("massesForcesNameId"), 4, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("massesForcesName")) && (argc == 1) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            SETSYMBOL(&(toout[0]), x->mass[i].Id);
            SETFLOAT(&(toout[1]), x->mass[i].forceX);
            SETFLOAT(&(toout[2]), x->mass[i].forceY);
            SETFLOAT(&(toout[3]), x->mass[i].forceZ);
            outlet_anything(x->main_outlet, gensym("massesForcesName"), 4, toout);
        } 
    }
    else
    if ( (toget == gensym("linksPos")) && (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) && (i<x->nb_mass) )
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
            SETFLOAT(&(toout[2]), x->link[i].mass1->posY);
            SETFLOAT(&(toout[3]), x->link[i].mass1->posZ);
            SETFLOAT(&(toout[4]), x->link[i].mass2->posX);
            SETFLOAT(&(toout[5]), x->link[i].mass2->posY);
            SETFLOAT(&(toout[6]), x->link[i].mass2->posZ);
            outlet_anything(x->main_outlet, gensym("linksPosNo"), 7, toout);
        }
    }
    else
    if ( (toget == gensym("linksPos")) && (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(toout[0]), i);
				SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
				SETFLOAT(&(toout[2]), x->link[i].mass1->posY);
				SETFLOAT(&(toout[3]), x->link[i].mass1->posZ);
				SETFLOAT(&(toout[4]), x->link[i].mass2->posX);
				SETFLOAT(&(toout[5]), x->link[i].mass2->posY);
				SETFLOAT(&(toout[6]), x->link[i].mass2->posZ);
				outlet_anything(x->main_outlet, gensym("linksPosNo"), 7, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("linksPos")) && (argc == 1) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
            SETFLOAT(&(toout[2]), x->link[i].mass1->posY);
            SETFLOAT(&(toout[3]), x->link[i].mass1->posZ);
            SETFLOAT(&(toout[4]), x->link[i].mass2->posX);
            SETFLOAT(&(toout[5]), x->link[i].mass2->posY);
            SETFLOAT(&(toout[6]), x->link[i].mass2->posZ);
            outlet_anything(x->main_outlet, gensym("linksPosNo"), 7, toout);
        } 
    }
    else
    if ( (toget == gensym("linksPosName")) && (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) && (i<x->nb_mass) )
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
            SETFLOAT(&(toout[2]), x->link[i].mass1->posY);
            SETFLOAT(&(toout[3]), x->link[i].mass1->posZ);
            SETFLOAT(&(toout[3]), x->link[i].mass2->posX);
            SETFLOAT(&(toout[5]), x->link[i].mass2->posY);
            SETFLOAT(&(toout[6]), x->link[i].mass2->posZ);
            outlet_anything(x->main_outlet, gensym("linksPosNo"), 7, toout);
        }
    }
    else
    if ( (toget == gensym("linksPosName")) && (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->link[i].Id)
            {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
            SETFLOAT(&(toout[2]), x->link[i].mass1->posY);
            SETFLOAT(&(toout[3]), x->link[i].mass1->posZ);
            SETFLOAT(&(toout[4]), x->link[i].mass2->posX);
            SETFLOAT(&(toout[5]), x->link[i].mass2->posY);
            SETFLOAT(&(toout[6]), x->link[i].mass2->posZ);
            outlet_anything(x->main_outlet, gensym("linksPosNo"), 7, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("linksPosName")) && (argc == 1) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
            SETFLOAT(&(toout[2]), x->link[i].mass1->posY);
            SETFLOAT(&(toout[3]), x->link[i].mass1->posZ);
            SETFLOAT(&(toout[4]), x->link[i].mass2->posX);
            SETFLOAT(&(toout[5]), x->link[i].mass2->posY);
            SETFLOAT(&(toout[6]), x->link[i].mass2->posZ);
            outlet_anything(x->main_outlet, gensym("linksPosNo"), 7, toout);
        } 
    }
    else
        error("not get attribute");
}

void pmpd3d_massesPosXL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i < x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].posX);
    }
    outlet_anything(x->main_outlet, gensym("massesPosXL"),x->nb_mass , pos_list);
}

void pmpd3d_massesForcesXL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].forceX);
    }
    outlet_anything(x->main_outlet, gensym("massesForcesXL"),x->nb_mass , pos_list);
}

void pmpd3d_massesSpeedsXL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].speedX);
    }
    outlet_anything(x->main_outlet, gensym("massesSpeedsXL"),x->nb_mass , pos_list);
}

void pmpd3d_massesPosXT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_mass;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = x->mass[i].posX;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_mass))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)

				{
					vec[i].w_float = x->mass[j].posX;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_massesSpeedsXT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {		
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_mass;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = x->mass[i].speedX;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_mass))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
				{
					vec[i].w_float = x->mass[j].speedX;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_massesForcesXT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_mass;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = x->mass[i].forceX;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_mass))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
				{
					vec[i].w_float = x->mass[j].forceX;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_massesPosYL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i < x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].posY);
    }
    outlet_anything(x->main_outlet, gensym("massesPosYL"),x->nb_mass , pos_list);
}

void pmpd3d_massesForcesYL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].forceY);
    }
    outlet_anything(x->main_outlet, gensym("massesForcesYL"),x->nb_mass , pos_list);
}

void pmpd3d_massesSpeedsYL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].speedY);
    }
    outlet_anything(x->main_outlet, gensym("massesSpeedsYL"),x->nb_mass , pos_list);
}

void pmpd3d_massesPosYT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_mass;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = x->mass[i].posY;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_mass))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
				{
					vec[i].w_float = x->mass[j].posY;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_massesSpeedsYT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_mass;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = x->mass[i].speedY;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_mass))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
				{
					vec[i].w_float = x->mass[j].speedY;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_massesForcesYT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_mass;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = x->mass[i].forceY;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_mass))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
				{
					vec[i].w_float = x->mass[j].forceY;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_massesPosZL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i < x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].posZ);
    }
    outlet_anything(x->main_outlet, gensym("massesPosZL"),x->nb_mass , pos_list);
}

void pmpd3d_massesForcesZL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].forceZ);
    }
    outlet_anything(x->main_outlet, gensym("massesForcesZL"),x->nb_mass , pos_list);
}

void pmpd3d_massesSpeedsZL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].speedZ);
    }
    outlet_anything(x->main_outlet, gensym("massesSpeedsZL"),x->nb_mass , pos_list);
}

void pmpd3d_massesPosZT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_mass;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = x->mass[i].posZ;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_mass))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
				{
					vec[i].w_float = x->mass[j].posZ;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_massesSpeedsZT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_mass;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = x->mass[i].speedZ;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_mass))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
				{
					vec[i].w_float = x->mass[j].speedZ;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_massesForcesZT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_mass;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = x->mass[i].forceZ;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_mass))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
				{
					vec[i].w_float = x->mass[j].forceZ;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_massesPosL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[3*x->nb_mass];

    for (i=0; i < x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[3*i]  ),x->mass[i].posX);
        SETFLOAT(&(pos_list[3*i+1]),x->mass[i].posY);
        SETFLOAT(&(pos_list[3*i+2]),x->mass[i].posZ);
    }
    outlet_anything(x->main_outlet, gensym("massesPosL"),3*x->nb_mass , pos_list);
}

void pmpd3d_massesForcesL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[3*x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[3*i]  ),x->mass[i].forceX);
        SETFLOAT(&(pos_list[3*i+1]),x->mass[i].forceY);
        SETFLOAT(&(pos_list[3*i+2]),x->mass[i].forceZ);
    }
    outlet_anything(x->main_outlet, gensym("massesForcesL"),3*x->nb_mass , pos_list);
}

void pmpd3d_massesSpeedsL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[3*x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[3*i]  ),x->mass[i].speedX);
        SETFLOAT(&(pos_list[3*i+1]),x->mass[i].speedY);
        SETFLOAT(&(pos_list[3*i+2]),x->mass[i].speedZ);
    }
    outlet_anything(x->main_outlet, gensym("massesSpeedsL"),3*x->nb_mass , pos_list);
}

void pmpd3d_massesPosT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_mass;
			taille_max = min(taille_max, (vecsize-2)/3);
			for (i=0; i < taille_max ; i++)
			{
				vec[3*i  ].w_float = x->mass[i].posX;
				vec[3*i+1].w_float = x->mass[i].posY;
				vec[3*i+2].w_float = x->mass[i].posZ;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize-2) && (j < x->nb_mass))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
				{
					vec[i].w_float = x->mass[j].posX;
					i++;
					vec[i].w_float = x->mass[j].posY;
					i++;
					vec[i].w_float = x->mass[j].posZ;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_massesSpeedsT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_mass;
			taille_max = min(taille_max, (vecsize-2)/3);
			for (i=0; i < taille_max ; i++)
			{
				vec[3*i  ].w_float = x->mass[i].speedX;
				vec[3*i+1].w_float = x->mass[i].speedY;
				vec[3*i+2].w_float = x->mass[i].speedZ;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize-2) && (j < x->nb_mass))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
				{
					vec[i].w_float = x->mass[j].speedX;
					i++;
					vec[i].w_float = x->mass[j].speedY;
					i++;
					vec[i].w_float = x->mass[j].speedZ;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_massesForcesT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_mass;
			taille_max = min(taille_max, (vecsize-2)/3);
			for (i=0; i < taille_max ; i++)
			{
				vec[3*i  ].w_float = x->mass[i].forceX;
				vec[3*i+1].w_float = x->mass[i].forceY;
				vec[3*i+2].w_float = x->mass[i].forceZ;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize-2) && (j < x->nb_mass))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
				{
					vec[i].w_float = x->mass[j].forceX;
					i++;
					vec[i].w_float = x->mass[j].forceY;
					i++;
					vec[i].w_float = x->mass[j].forceZ;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_massesPosNormL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i < x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),sqrt(sqr(x->mass[i].posX)+sqr(x->mass[i].posY)+sqr(x->mass[i].posZ)));
    }
    outlet_anything(x->main_outlet, gensym("massesPosNormL"),x->nb_mass , pos_list);
}

void pmpd3d_massesForcesNormL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),sqrt(sqr(x->mass[i].forceX)+sqr(x->mass[i].forceY)+sqr(x->mass[i].forceZ)));
    }
    outlet_anything(x->main_outlet, gensym("massesForcesNormL"),x->nb_mass , pos_list);
}

void pmpd3d_massesSpeedsNormL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),sqrt(sqr(x->mass[i].speedX)+sqr(x->mass[i].speedY)+sqr(x->mass[i].speedZ)));
    }
    outlet_anything(x->main_outlet, gensym("massesSpeedsNormL"),x->nb_mass , pos_list);
}

void pmpd3d_massesPosNormT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_mass;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = sqrt(sqr(x->mass[i].posX)+sqr(x->mass[i].posY)+sqr(x->mass[i].posZ));
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_mass))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
				{
					vec[i].w_float = sqrt(sqr(x->mass[j].posX)+sqr(x->mass[j].posY)+sqr(x->mass[i].posZ));
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_massesSpeedsNormT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_mass;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = sqrt(sqr(x->mass[i].speedX)+sqr(x->mass[i].speedY)+sqr(x->mass[i].speedZ));
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_mass))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
				{
					vec[i].w_float = sqrt(sqr(x->mass[j].speedX)+sqr(x->mass[j].speedY)+sqr(x->mass[i].speedZ));
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_massesForcesNormT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_mass;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = sqrt(sqr(x->mass[i].forceX)+sqr(x->mass[i].forceY)+sqr(x->mass[i].forceZ));
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_mass))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
				{
					vec[i].w_float = sqrt(sqr(x->mass[j].forceX)+sqr(x->mass[j].forceY)+sqr(x->mass[i].forceZ));
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_massesPosMean(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, sommeZ, somme;
    t_int i,j;
    t_atom mean[4];

	sommeX = 0;
	sommeY = 0;
	sommeZ = 0;
	somme = 0;
	j = 0;
	
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				sommeX += x->mass[i].posX;
				sommeY += x->mass[i].posY;
				sommeZ += x->mass[i].posZ;
				somme +=  sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY) + sqr(x->mass[i].posZ)); // distance au centre
				j++;
			}
		}
    }
	else
	{
		for (i=0; i< x->nb_mass; i++)
        {
				sommeX += x->mass[i].posX;
				sommeY += x->mass[i].posY;
				sommeZ += x->mass[i].posZ;
				somme +=  sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY) + sqr(x->mass[i].posZ)); // distance au centre
				j++;
		}
	}	
	
	sommeX /= j;
	sommeY /= j;
	sommeZ /= j;
	somme  /= j;	
	
    SETFLOAT(&(mean[0]),sommeX);
    SETFLOAT(&(mean[1]),sommeY);
    SETFLOAT(&(mean[2]),sommeZ);
    SETFLOAT(&(mean[3]),somme);
    
    outlet_anything(x->main_outlet, gensym("massesPosMean"),4 , mean);
}

void pmpd3d_massesPosStd(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, sommeZ, somme;
    t_int i,j;
    t_float stdX, stdY, stdZ, std;
    t_atom std_out[4];

	sommeX = 0;
	sommeY = 0;
	sommeZ = 0;
	somme  = 0;
	stdX = 0;
	stdY = 0;
	stdZ = 0;
	std  = 0;
	j = 0;
	
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				sommeX += x->mass[i].posX;
				sommeY += x->mass[i].posY;
				sommeZ += x->mass[i].posZ;
				somme +=  sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY) + sqr(x->mass[i].posZ)); // distance au centre
				j++;
			}
		}
		sommeX /= j;
		sommeY /= j;
		sommeZ /= j;
		somme /= j;
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				stdX += sqr(x->mass[i].posX-sommeX);
				stdY += sqr(x->mass[i].posY-sommeY);
				stdZ += sqr(x->mass[i].posZ-sommeZ);
				std  +=  sqr(sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY) + sqr(x->mass[i].posZ))-somme);
			}
		}		
    }
	else
	{
		for (i=0; i< x->nb_mass; i++)
        {
			sommeX += x->mass[i].posX;
			sommeY += x->mass[i].posY;
			sommeZ += x->mass[i].posZ;
			somme +=  sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY) + sqr(x->mass[i].posZ)); // distance au centre
			j++;
		}
		sommeX /= j;
		sommeY /= j;
		sommeZ /= j;
		somme /= j;
		for (i=0; i< x->nb_mass; i++)
        {
			stdX += sqr(x->mass[i].posX-sommeX);
			stdY += sqr(x->mass[i].posY-sommeY);
			stdZ += sqr(x->mass[i].posZ-sommeZ);
			std  += sqr(sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY) + sqr(x->mass[i].posZ))-somme);
		}
	}	
	
	stdX = sqrt(stdX/j);
	stdY = sqrt(stdY/j);
	stdZ = sqrt(stdZ/j);
	std  = sqrt(std /j);	

    SETFLOAT(&(std_out[0]),stdX);
    SETFLOAT(&(std_out[1]),stdY);
    SETFLOAT(&(std_out[2]),stdZ);
    SETFLOAT(&(std_out[3]),std);
    
    outlet_anything(x->main_outlet, gensym("massesPosStd"),4 , std_out);
}

void pmpd3d_massesForcesMean(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, sommeZ, somme;
    t_int i,j;
    t_atom mean[4];

	sommeX = 0;
	sommeY = 0;
	sommeZ = 0;
	somme = 0;
	j = 0;
	
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				sommeX += x->mass[i].forceX;
				sommeY += x->mass[i].forceY;
				sommeZ += x->mass[i].forceZ;
				somme +=  sqrt(sqr(x->mass[i].forceX) + sqr(x->mass[i].forceY) + sqr(x->mass[i].forceZ)); // force total
				j++;
			}
		}
    }
	else
	{
		for (i=0; i< x->nb_mass; i++)
        {
				sommeX += x->mass[i].forceX;
				sommeY += x->mass[i].forceY;
				sommeZ += x->mass[i].forceZ;
				somme +=  sqrt(sqr(x->mass[i].forceX) + sqr(x->mass[i].forceY) + sqr(x->mass[i].forceZ)); // force
				j++;
		}
	}	
	
	sommeX /= j;
	sommeY /= j;
	sommeZ /= j;
	somme  /= j;

    SETFLOAT(&(mean[0]),sommeX);
    SETFLOAT(&(mean[1]),sommeY);
    SETFLOAT(&(mean[2]),sommeZ);
    SETFLOAT(&(mean[3]),somme);
    
    outlet_anything(x->main_outlet, gensym("massesForcesMean"),4 , mean);
}

void pmpd3d_massesForcesStd(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, sommeZ, somme;
    t_int i,j;
    t_float stdX, stdY, stdZ, std;
    t_atom std_out[4];

	sommeX = 0;
	sommeY = 0;
	sommeZ = 0;
	somme  = 0;
	stdX = 0;
	stdY = 0;
	stdZ = 0;
	std  = 0;
	j = 0;
	
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				sommeX += x->mass[i].forceX;
				sommeY += x->mass[i].forceY;
				sommeZ += x->mass[i].forceZ;
				somme +=  sqrt(sqr(x->mass[i].forceX) + sqr(x->mass[i].forceY) + sqr(x->mass[i].forceZ)); // force
				j++;
			}
		}
		sommeX /= j;
		sommeY /= j;
		sommeZ /= j;
		somme /= j;
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				stdX += sqr(x->mass[i].forceX-sommeX);
				stdY += sqr(x->mass[i].forceY-sommeY);
				stdZ += sqr(x->mass[i].forceZ-sommeZ);
				std  +=  sqr(sqrt(sqr(x->mass[i].forceX) + sqr(x->mass[i].forceY) + sqr(x->mass[i].forceZ))-somme);
			}
		}		
    }
	else
	{
		for (i=0; i< x->nb_mass; i++)
        {
			sommeX += x->mass[i].forceX;
			sommeY += x->mass[i].forceY;
			sommeZ += x->mass[i].forceZ;
			somme +=  sqrt(sqr(x->mass[i].forceX) + sqr(x->mass[i].forceY) + sqr(x->mass[i].forceZ)); 
			j++;
		}
		sommeX /= j;
		sommeY /= j;
		sommeZ /= j;
		somme /= j;
		for (i=0; i< x->nb_mass; i++)
        {
			stdX += sqr(x->mass[i].forceX-sommeX);
			stdY += sqr(x->mass[i].forceY-sommeY);
			stdZ += sqr(x->mass[i].forceZ-sommeZ);
			std  += sqr(sqrt(sqr(x->mass[i].forceX) + sqr(x->mass[i].forceY) + sqr(x->mass[i].forceZ))-somme);
		}
	}	
	
	stdX = sqrt(stdX/j);
	stdY = sqrt(stdY/j);
	stdZ = sqrt(stdZ/j);
	std  = sqrt(std /j);	

    SETFLOAT(&(std_out[0]),stdX);
    SETFLOAT(&(std_out[1]),stdY);
    SETFLOAT(&(std_out[2]),stdZ);
    SETFLOAT(&(std_out[3]),std);
    
    outlet_anything(x->main_outlet, gensym("massesForcesStd"),4 , std_out);
}

void pmpd3d_massesSpeedsMean(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, sommeZ, somme;
    t_int i,j;
    t_atom mean[4];

	sommeX = 0;
	sommeY = 0;
	sommeZ = 0;
	somme = 0;
	j = 0;
	
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				sommeX += x->mass[i].speedX;
				sommeY += x->mass[i].speedY;
				sommeZ += x->mass[i].speedZ;
				somme +=  sqrt(sqr(x->mass[i].speedX) + sqr(x->mass[i].speedY) + sqr(x->mass[i].speedZ)); // speed total
				j++;
			}
		}
    }
	else
	{
		for (i=0; i< x->nb_mass; i++)
        {
				sommeX += x->mass[i].speedX;
				sommeY += x->mass[i].speedY;
				sommeZ += x->mass[i].speedZ;
				somme +=  sqrt(sqr(x->mass[i].speedX) + sqr(x->mass[i].speedY) + sqr(x->mass[i].speedZ)); // speed
				j++;
		}
	}	
	
	sommeX /= j;
	sommeY /= j;
	sommeZ /= j;
	somme  /= j;

    SETFLOAT(&(mean[0]),sommeX);
    SETFLOAT(&(mean[1]),sommeY);
    SETFLOAT(&(mean[2]),sommeZ);
    SETFLOAT(&(mean[3]),somme);
    
    outlet_anything(x->main_outlet, gensym("massesFpeedsMean"),4 , mean);
}

void pmpd3d_massesSpeedsStd(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, sommeZ, somme;
    t_int i,j;
    t_float stdX, stdY, stdZ, std;
    t_atom std_out[4];

	sommeX = 0;
	sommeY = 0;
	sommeZ = 0;
	somme  = 0;
	stdX = 0;
	stdY = 0;
	stdZ = 0;
	std  = 0;
	j = 0;
	
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				sommeX += x->mass[i].speedX;
				sommeY += x->mass[i].speedY;
				sommeZ += x->mass[i].speedZ;
				somme +=  sqrt(sqr(x->mass[i].speedX) + sqr(x->mass[i].speedY) + sqr(x->mass[i].speedZ)); // speed
				j++;
			}
		}
		sommeX /= j;
		sommeY /= j;
		sommeZ /= j;
		somme /= j;
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				stdX += sqr(x->mass[i].speedX-sommeX);
				stdY += sqr(x->mass[i].speedY-sommeY);
				stdZ += sqr(x->mass[i].speedZ-sommeZ);
				std  +=  sqr(sqrt(sqr(x->mass[i].speedX) + sqr(x->mass[i].speedY) + sqr(x->mass[i].speedZ))-somme);
			}
		}		
    }
	else
	{
		for (i=0; i< x->nb_mass; i++)
        {
			sommeX += x->mass[i].speedX;
			sommeY += x->mass[i].speedY;
			sommeZ += x->mass[i].speedZ;
			somme +=  sqrt(sqr(x->mass[i].speedX) + sqr(x->mass[i].speedY) + sqr(x->mass[i].speedZ)); 
			j++;
		}
		sommeX /= j;
		sommeY /= j;
		sommeZ /= j;
		somme /= j;
		for (i=0; i< x->nb_mass; i++)
        {
			stdX += sqr(x->mass[i].speedX-sommeX);
			stdY += sqr(x->mass[i].speedY-sommeY);
			stdZ += sqr(x->mass[i].speedZ-sommeZ);
			std  += sqr(sqrt(sqr(x->mass[i].speedX) + sqr(x->mass[i].speedY) + sqr(x->mass[i].speedZ))-somme);
		}
	}	
	
	stdX = sqrt(stdX/j);
	stdY = sqrt(stdY/j);
	stdZ = sqrt(stdZ/j);
	std  = sqrt(std /j);	

    SETFLOAT(&(std_out[0]),stdX);
    SETFLOAT(&(std_out[1]),stdY);
    SETFLOAT(&(std_out[2]),stdZ);
    SETFLOAT(&(std_out[3]),std);
    
    outlet_anything(x->main_outlet, gensym("massesSpeedsStd"),4 , std_out);
}

// --------------------------------------------

void pmpd3d_linksPosXL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),(x->link[i].mass1->posX + x->link[i].mass2->posX)/2);
    }
    outlet_anything(x->main_outlet, gensym("linksPosXL"),x->nb_link , pos_list);
}

void pmpd3d_linksLengthXL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),x->link[i].mass2->posX - x->link[i].mass1->posX);
    }
    outlet_anything(x->main_outlet, gensym("linksLengthXL"),x->nb_link , pos_list);
}

void pmpd3d_linksPosSpeedXL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),(x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2);
    }
    outlet_anything(x->main_outlet, gensym("linksPosSpeedXL"),x->nb_link , pos_list);
}

void pmpd3d_linksLengthSpeedXL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),x->link[i].mass2->speedX - x->link[i].mass1->speedX);
    }
    outlet_anything(x->main_outlet, gensym("linksLengthSpeedXL"),x->nb_link , pos_list);
}

void pmpd3d_linksPosXT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = (x->link[i].mass1->posX + x->link[i].mass2->posX)/2;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
				{
					vec[i].w_float = (x->link[j].mass1->posX + x->link[j].mass2->posX)/2;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_linksLengthXT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = x->link[i].mass2->posX - x->link[i].mass1->posX;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
				{
					vec[i].w_float = x->link[j].mass2->posX - x->link[j].mass1->posX;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_linksPosSpeedXT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = (x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
				{
					vec[i].w_float = (x->link[j].mass1->speedX + x->link[j].mass2->speedX)/2;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_linksLengthSpeedXT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = x->link[i].mass2->speedX - x->link[i].mass1->speedX;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
				{
					vec[i].w_float = x->link[j].mass2->speedX - x->link[j].mass1->speedX;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_linksPosYL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),(x->link[i].mass1->posY + x->link[i].mass2->posY)/2);
    }
    outlet_anything(x->main_outlet, gensym("linksPosYL"),x->nb_link , pos_list);
}

void pmpd3d_linksLengthYL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),x->link[i].mass2->posY - x->link[i].mass1->posY);
    }
    outlet_anything(x->main_outlet, gensym("linksLengthYL"),x->nb_link , pos_list);
}

void pmpd3d_linksPosSpeedYL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),(x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2);
    }
    outlet_anything(x->main_outlet, gensym("linksPosSpeedYL"),x->nb_link , pos_list);
}

void pmpd3d_linksLengthSpeedYL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),x->link[i].mass2->speedY - x->link[i].mass1->speedY);
    }
    outlet_anything(x->main_outlet, gensym("linksLengthSpeedYL"),x->nb_link , pos_list);
}

void pmpd3d_linksPosYT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = (x->link[i].mass1->posY + x->link[i].mass2->posY)/2;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
				{
					vec[i].w_float = (x->link[j].mass1->posY + x->link[j].mass2->posY)/2;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_linksLengthYT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = x->link[i].mass2->posY - x->link[i].mass1->posY;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
				{
					vec[i].w_float = x->link[j].mass2->posY - x->link[j].mass1->posY;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_linksPosSpeedYT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = (x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
				{
					vec[i].w_float = (x->link[j].mass1->speedY + x->link[j].mass2->speedY)/2;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_linksLengthSpeedYT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = x->link[i].mass2->speedY - x->link[i].mass1->speedY;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
				{
					vec[i].w_float = x->link[j].mass2->speedY - x->link[j].mass1->speedY;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_linksPosZL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),(x->link[i].mass1->posZ + x->link[i].mass2->posZ)/2);
    }
    outlet_anything(x->main_outlet, gensym("linksPosZL"),x->nb_link , pos_list);
}

void pmpd3d_linksLengthZL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),x->link[i].mass2->posZ - x->link[i].mass1->posZ);
    }
    outlet_anything(x->main_outlet, gensym("linksLengthZL"),x->nb_link , pos_list);
}

void pmpd3d_linksPosSpeedZL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),(x->link[i].mass1->speedZ + x->link[i].mass2->speedZ)/2);
    }
    outlet_anything(x->main_outlet, gensym("linksPosSpeedZL"),x->nb_link , pos_list);
}

void pmpd3d_linksLengthSpeedZL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),x->link[i].mass2->speedZ - x->link[i].mass1->speedZ);
    }
    outlet_anything(x->main_outlet, gensym("linksLengthSpeedZL"),x->nb_link , pos_list);
}

void pmpd3d_linksPosZT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = (x->link[i].mass1->posZ + x->link[i].mass2->posZ)/2;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
				{
					vec[i].w_float = (x->link[j].mass1->posZ + x->link[j].mass2->posZ)/2;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_linksLengthZT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = x->link[i].mass2->posZ - x->link[i].mass1->posZ;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
				{
					vec[i].w_float = x->link[j].mass2->posZ - x->link[j].mass1->posZ;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_linksPosSpeedZT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = (x->link[i].mass1->speedZ + x->link[i].mass2->speedZ)/2;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
				{
					vec[i].w_float = (x->link[j].mass1->speedZ + x->link[j].mass2->speedZ)/2;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_linksLengthSpeedZT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = x->link[i].mass2->speedZ - x->link[i].mass1->speedZ;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
				{
					vec[i].w_float = x->link[j].mass2->speedZ - x->link[j].mass1->speedZ;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_linksPosL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[3*x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[3*i]  ),(x->link[i].mass2->posX + x->link[i].mass1->posX)/2);
        SETFLOAT(&(pos_list[3*i+1]),(x->link[i].mass2->posY + x->link[i].mass1->posY)/2);
        SETFLOAT(&(pos_list[3*i+2]),(x->link[i].mass2->posZ + x->link[i].mass1->posZ)/2);
    }
    outlet_anything(x->main_outlet, gensym("linksPosL"),3*x->nb_link , pos_list);
}

void pmpd3d_linksLengthL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[3*x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[3*i]  ),x->link[i].mass2->posX - x->link[i].mass1->posX);
        SETFLOAT(&(pos_list[3*i+1]),x->link[i].mass2->posY - x->link[i].mass1->posY);
        SETFLOAT(&(pos_list[3*i+2]),x->link[i].mass2->posZ - x->link[i].mass1->posZ);
    }
    outlet_anything(x->main_outlet, gensym("linksLengthL"),3*x->nb_link , pos_list);
}

void pmpd3d_linksPosSpeedL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[3*x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[3*i]  ),(x->link[i].mass2->speedX + x->link[i].mass1->speedX)/2);
        SETFLOAT(&(pos_list[3*i+1]),(x->link[i].mass2->speedY + x->link[i].mass1->speedY)/2);
        SETFLOAT(&(pos_list[3*i+2]),(x->link[i].mass2->speedZ + x->link[i].mass1->speedZ)/2);
    }
    outlet_anything(x->main_outlet, gensym("linksPosSpeedL"),3*x->nb_link , pos_list);
}

void pmpd3d_linksLengthSpeedL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[3*x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[3*i]  ),x->link[i].mass2->speedX - x->link[i].mass1->speedX);
        SETFLOAT(&(pos_list[3*i+1]),x->link[i].mass2->speedY - x->link[i].mass1->speedY);
        SETFLOAT(&(pos_list[3*i+2]),x->link[i].mass2->speedZ - x->link[i].mass1->speedZ);
    }
    outlet_anything(x->main_outlet, gensym("linksLengthSpeedL"),3*x->nb_link , pos_list);
}

void pmpd3d_linksPosT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, (vecsize-2)/3);
			for (i=0; i < taille_max ; i++)
			{
				vec[3*i  ].w_float = (x->link[i].mass2->posX + x->link[i].mass1->posX)/2;
				vec[3*i+1].w_float = (x->link[i].mass2->posY + x->link[i].mass1->posY)/2;
				vec[3*i+2].w_float = (x->link[i].mass2->posZ + x->link[i].mass1->posZ)/2;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize-2) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
				{
					vec[i].w_float = (x->link[j].mass2->posX + x->link[j].mass1->posX)/2;
					i++;
					vec[i].w_float = (x->link[j].mass2->posY + x->link[j].mass1->posY)/2;
					i++;
					vec[i].w_float = (x->link[j].mass2->posZ + x->link[j].mass1->posZ)/2;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_linksLengthT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, (vecsize-2)/3);
			for (i=0; i < taille_max ; i++)
			{
				vec[3*i  ].w_float = x->link[i].mass2->posX - x->link[i].mass1->posX;
				vec[3*i+1].w_float = x->link[i].mass2->posY - x->link[i].mass1->posY;
				vec[3*i+2].w_float = x->link[i].mass2->posZ - x->link[i].mass1->posZ;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize-2) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
				{
					vec[i].w_float = x->link[j].mass2->posX + x->link[j].mass1->posX;
					i++;
					vec[i].w_float = x->link[j].mass2->posY + x->link[j].mass1->posY;
					i++;
					vec[i].w_float = x->link[j].mass2->posZ + x->link[j].mass1->posZ;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_linksPosSpeedT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, (vecsize-2)/3);
			for (i=0; i < taille_max ; i++)
			{
				vec[3*i  ].w_float = (x->link[i].mass2->speedX + x->link[i].mass1->speedX)/2;
				vec[3*i+1].w_float = (x->link[i].mass2->speedY + x->link[i].mass1->speedY)/2;
				vec[3*i+2].w_float = (x->link[i].mass2->speedZ + x->link[i].mass1->speedZ)/2;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize-2) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
				{
					vec[i].w_float = (x->link[j].mass2->speedX + x->link[j].mass1->speedX)/2;
					i++;
					vec[i].w_float = (x->link[j].mass2->speedY + x->link[j].mass1->speedY)/2;
					i++;
					vec[i].w_float = (x->link[j].mass2->speedZ + x->link[j].mass1->speedZ)/2;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_linksLengthSpeedT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, (vecsize-2)/3);
			for (i=0; i < taille_max ; i++)
			{
				vec[3*i  ].w_float = x->link[i].mass2->speedX - x->link[i].mass1->speedX;
				vec[3*i+1].w_float = x->link[i].mass2->speedY - x->link[i].mass1->speedY;
				vec[3*i+2].w_float = x->link[i].mass2->speedZ - x->link[i].mass1->speedZ;
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize-2) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
				{
					vec[i].w_float = x->link[j].mass2->speedX + x->link[j].mass1->speedX;
					i++;
					vec[i].w_float = x->link[j].mass2->speedY + x->link[j].mass1->speedY;
					i++;
					vec[i].w_float = x->link[j].mass2->speedZ + x->link[j].mass1->speedZ;
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_linksPosNormL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),sqrt( \
							sqr((x->link[i].mass1->posX + x->link[i].mass2->posX)/2) + \
							sqr((x->link[i].mass1->posY + x->link[i].mass2->posY)/2) + \
							sqr((x->link[i].mass1->posZ + x->link[i].mass2->posZ)/2) ));
    }
    outlet_anything(x->main_outlet, gensym("linksPosNormL"),x->nb_link , pos_list);
}

void pmpd3d_linksLengthNormL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),sqrt( \
							sqr(x->link[i].mass2->posX - x->link[i].mass1->posX) + \
							sqr(x->link[i].mass2->posY - x->link[i].mass1->posY) + \
							sqr(x->link[i].mass2->posZ - x->link[i].mass1->posZ) ));
    }
    outlet_anything(x->main_outlet, gensym("linksLengthNormL"),x->nb_link , pos_list);
}

void pmpd3d_linksPosSpeedNormL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),sqrt( \
							sqr((x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2) + \
							sqr((x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2) + \
							sqr((x->link[i].mass1->speedZ + x->link[i].mass2->speedZ)/2) ));
    }
    outlet_anything(x->main_outlet, gensym("linksPosSpeedNormL"),x->nb_link , pos_list);
}

void pmpd3d_linksLengthSpeedNormL(t_pmpd3d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),sqrt( \
							sqr(x->link[i].mass2->speedX - x->link[i].mass1->speedX) + \
							sqr(x->link[i].mass2->speedY - x->link[i].mass1->speedY) + \
							sqr(x->link[i].mass2->speedZ - x->link[i].mass1->speedZ) ));
    }
    outlet_anything(x->main_outlet, gensym("linksLengthSpeedNormL"),x->nb_link , pos_list);
}

void pmpd3d_linksPosNormT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = sqrt( \
							sqr((x->link[i].mass1->posX + x->link[i].mass2->posX)/2) + \
							sqr((x->link[i].mass1->posY + x->link[i].mass2->posY)/2) + \
							sqr((x->link[i].mass1->posZ + x->link[i].mass2->posZ)/2) );
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
				{
					vec[j].w_float = sqrt( \
							sqr((x->link[j].mass1->posX + x->link[j].mass2->posX)/2) + \
							sqr((x->link[j].mass1->posY + x->link[j].mass2->posY)/2) + \
							sqr((x->link[j].mass1->posZ + x->link[j].mass2->posZ)/2) );
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_linksLengthNormT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = sqrt( \
							sqr(x->link[i].mass2->posX - x->link[i].mass1->posX) + \
							sqr(x->link[i].mass2->posY - x->link[i].mass1->posY) + \
							sqr(x->link[i].mass2->posZ - x->link[i].mass1->posZ) );
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
				{
					vec[i].w_float = sqrt( \
							sqr(x->link[j].mass2->posX - x->link[j].mass1->posX) + \
							sqr(x->link[j].mass2->posY - x->link[j].mass1->posY) + \
							sqr(x->link[j].mass2->posZ - x->link[j].mass1->posZ) );
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_linksPosSpeedNormT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = sqrt( \
							sqr((x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2) + \
							sqr((x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2) + \
							sqr((x->link[i].mass1->speedZ + x->link[i].mass2->speedZ)/2) );
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
				{
					vec[i].w_float = sqrt( \
							sqr((x->link[j].mass1->speedX + x->link[j].mass2->speedX)/2) + \
							sqr((x->link[j].mass1->speedY + x->link[j].mass2->speedY)/2) + \
							sqr((x->link[j].mass1->speedZ + x->link[j].mass2->speedZ)/2) );
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

void pmpd3d_linksLengthSpeedNormT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{
			int taille_max = x->nb_link;
			taille_max = min(taille_max, vecsize);
			for (i=0; i < taille_max ; i++)
			{
				vec[i].w_float = sqrt( \
							sqr(x->link[i].mass2->speedX - x->link[i].mass1->speedX) + \
							sqr(x->link[i].mass2->speedY - x->link[i].mass1->speedY) + \
							sqr(x->link[i].mass2->speedZ - x->link[i].mass1->speedZ) );
			}
			garray_redraw(a);
		}
	}
	else 
	if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
		if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
			pd_error(x, "%s: no such array", tab_name->s_name);
		else if (!garray_getfloatwords(a, &vecsize, &vec))
			pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
		else
		{	
			i = 0;
			j = 0;
			while ((i < vecsize) && (j < x->nb_link))
			{
				if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
				{
					vec[i].w_float = sqrt( \
							sqr(x->link[j].mass2->speedX - x->link[j].mass1->speedX) + \
							sqr(x->link[j].mass2->speedY - x->link[j].mass1->speedY) + \
							sqr(x->link[j].mass2->speedZ - x->link[j].mass1->speedZ) );
					i++;
				}
				j++;
			}
			garray_redraw(a);
		}
	}
}

//----------------------------------------------

void pmpd3d_grabMass(t_pmpd3d *x, t_float posX, t_float posY, t_float posZ, t_float grab)
{
	t_float dist, tmp;
	t_int i;
	
	if (grab == 0)
		x->grab=0;
	if ((x->grab == 0)&(grab == 1)&(x->nb_mass > 0))
	{
		x->grab=1;
		x->grab_nb= 0;
		dist = sqr(x->mass[0].posX - posX) + sqr(x->mass[0].posY - posY) + sqr(x->mass[0].posZ - posZ);
		for (i=1; i<x->nb_mass; i++)
		{
			tmp = sqr(x->mass[i].posX - posX) + sqr(x->mass[i].posY - posY) + sqr(x->mass[i].posZ - posZ);
			if (tmp < dist)
			{
				dist = tmp;
				x->grab_nb= i;
			}
		}
	}
	if (x->grab == 1)
	{
		x->mass[x->grab_nb].posX = posX;
		x->mass[x->grab_nb].posY = posY;		
		x->mass[x->grab_nb].posZ = posZ;		
	}
}

void *pmpd3d_new()
{
	t_pmpd3d *x = (t_pmpd3d *)pd_new(pmpd3d_class);

	pmpd3d_reset(x);
	
	x->main_outlet=outlet_new(&x->x_obj, 0);
	// x->info_outlet=outlet_new(&x->x_obj, 0); // TODO

	return (void *)x;
}

void pmpd3d_setup(void) 
{
 pmpd3d_class = class_new(gensym("pmpd3d"),
        (t_newmethod)pmpd3d_new,
        0, sizeof(t_pmpd3d),CLASS_DEFAULT, 0);

	class_addbang(pmpd3d_class, pmpd3d_bang);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_reset,           gensym("reset"), 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_infosL,          gensym("infosL"), 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_infosL,          gensym("print"), 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_mass,            gensym("mass"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_link,            gensym("link"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_tabLink,         gensym("tabLink"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_tLink,           gensym("tLink"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_setK,            gensym("setK"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_setD,            gensym("setD"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_setL,            gensym("setL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_setLKTab,        gensym("setLKTab"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_setLDTab,        gensym("setLDTab"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_setLinkId,       gensym("setLinkId"), A_GIMME, 0);

	class_addmethod(pmpd3d_class, (t_method)pmpd3d_setMassId,       gensym("setMassId"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_pos,             gensym("pos"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_posX,            gensym("posX"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_posY,            gensym("posY"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_posZ,            gensym("posZ"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_force,           gensym("force"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_forceX,          gensym("forceX"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_forceY,          gensym("forceY"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_forceZ,          gensym("forceZ"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_minX,            gensym("Xmin"), A_DEFFLOAT, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_maxX,            gensym("Xmax"), A_DEFFLOAT, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_minY,            gensym("Ymin"), A_DEFFLOAT, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_maxY,            gensym("Ymax"), A_DEFFLOAT, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_minZ,            gensym("Zmin"), A_DEFFLOAT, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_maxZ,            gensym("Zmax"), A_DEFFLOAT, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_min,             gensym("min"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_max,             gensym("max"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_setFixed,        gensym("setFixed"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_setMobile,       gensym("setMobile"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_setD2,           gensym("setDEnv"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_setD2offset,     gensym("setDEnvOffset"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_setSpeed,        gensym("setSpeed"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_setSpeedX,       gensym("setSpeedX"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_setSpeedY,       gensym("setSpeedY"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_setSpeedZ,       gensym("setSpeedZ"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_setForce,        gensym("setForce"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_setForceX,       gensym("setForceX"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_setForceY,       gensym("setForceY"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_setForceZ,       gensym("setForceZ"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_addPos,          gensym("addPos"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_addPosX,         gensym("addPosX"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_addPosY,         gensym("addPosY"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_addPosZ,         gensym("addPosZ"), A_GIMME, 0);

	class_addmethod(pmpd3d_class, (t_method)pmpd3d_get,             gensym("get"), A_GIMME, 0);
	
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosL,      gensym("massesPosL"), 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsL,   gensym("massesSpeedsL"), 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesL,   gensym("massesForcesL"), 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosT,      gensym("massesPosT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsT,   gensym("massesSpeedsT"),A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesT,   gensym("massesForcesT"),A_GIMME, 0);
	
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosXL,     gensym("massesPosXL"), 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsXL,  gensym("massesSpeedsXL"), 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesXL,  gensym("massesForcesXL"), 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosXT,     gensym("massesPosXT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsXT,  gensym("massesSpeedsXT"),A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesXT,  gensym("massesForcesXT"),A_GIMME, 0);
	
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosYL,     gensym("massesPosYL"), 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsYL,  gensym("massesSpeedsYL"), 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesYL,  gensym("massesForcesYL"), 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosYT,     gensym("massesPosYT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsYT,  gensym("massesSpeedsYT"),A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesYT,  gensym("massesForcesYT"),A_GIMME, 0);
		
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosZL,     gensym("massesPosZL"), 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsZL,  gensym("massesSpeedsZL"), 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesZL,  gensym("massesForcesZL"), 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosZT,     gensym("massesPosZT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsZT,  gensym("massesSpeedsZT"),A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesZT,  gensym("massesForcesZT"),A_GIMME, 0);

	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosNormL,      gensym("massesPosNormL"), 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsNormL,   gensym("massesSpeedsNormL"), 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesNormL,   gensym("massesForcesNormL"), 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosNormT,      gensym("massesPosNormT"),  A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsNormT,   gensym("massesSpeedsNormT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesNormT,   gensym("massesForcesNormT"), A_GIMME, 0);
	
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosMean,   	gensym("massesPosMean"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosStd,    	gensym("massesPosStd"),A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesMean,	gensym("massesForecesMean"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesStd, 	gensym("massesForcesStd"),A_GIMME, 0);	
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsMean,	gensym("massesSpeedsMean"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsStd, 	gensym("massesSpeedsStd"),A_GIMME, 0);
	
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosXL,	   		gensym("linksPosXL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthXL,	   	gensym("linksLengthXL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedXL,		gensym("linksPosSpeedXL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedXL,	gensym("linksLengthSpeedXL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosXT,	   		gensym("linksPosXT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthXT,	   	gensym("linksLengthXT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedXT,		gensym("linksPosSpeedXT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedXT,	gensym("linksLengthSpeedXT"), A_GIMME, 0);
	
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosYL,	   		gensym("linksPosYL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthYL,	   	gensym("linksLengthYL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedYL,		gensym("linksPosSpeedYL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedYL,	gensym("linksLengthSpeedYL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosYT,	   		gensym("linksPosYT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthYT,	   	gensym("linksLengthYT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedYT,		gensym("linksPosSpeedYT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedYT,	gensym("linksLengthSpeedYT"), A_GIMME, 0);

	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosZL,	   		gensym("linksPosZL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthZL,	   	gensym("linksLengthZL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedZL,		gensym("linksPosSpeedZL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedZL,	gensym("linksLengthSpeedZL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosZT,	   		gensym("linksPosZT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthZT,	   	gensym("linksLengthZT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedZT,		gensym("linksPosSpeedZT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedZT,	gensym("linksLengthSpeedZT"), A_GIMME, 0);
	
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosL,	   		gensym("linksPosL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthL,	   	gensym("linksLengthL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedL,		gensym("linksPosSpeedL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedL,	gensym("linksLengthSpeedL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosT,	   		gensym("linksPosT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthT,	   	gensym("linksLengthT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedT,		gensym("linksPosSpeedT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedT,	gensym("linksLengthSpeedT"), A_GIMME, 0);

	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosNormL,	   		gensym("linksPosNormL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthNormL,	   	gensym("linksLengthNormL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedNormL,		gensym("linksPosSpeedNormL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedNormL,	gensym("linksLengthSpeedNormL"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosNormT,	   		gensym("linksPosNormT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthNormT,	   	gensym("linksLengthNormT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedNormT,		gensym("linksPosSpeedNormT"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedNormT,	gensym("linksLengthSpeedNormT"), A_GIMME, 0);

/*	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosMean,	   		gensym("linksPosMean"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthMean,	   		gensym("linksLengthMean"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedMean,		gensym("linksPosSpeedMean"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedMean,	gensym("linksLengthSpeedMean"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosStd,		   		gensym("linksPosStd"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthStd,	   		gensym("linksLengthStd"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedStd,		gensym("linksPosSpeedStd"), A_GIMME, 0);
	class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedStd0,	gensym("linksLengthSpeedStd"), A_GIMME, 0);
*/

	class_addmethod(pmpd3d_class, (t_method)pmpd3d_grabMass,        gensym("grabMass"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);

}

