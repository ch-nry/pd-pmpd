/*
--------------------------  pmpd2d  ---------------------------------------- 
     
	  
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
 along with this program; if not, write to the Free Software                  
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.  
                                                                              
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
	
static t_class *pmpd2d_class;

typedef struct _mass {
    t_symbol *Id;
    int mobile;
	t_float invM;
	t_float speedX;
	t_float speedY;
	t_float posX;
	t_float posY;
	t_float forceX;
	t_float forceY;
	t_float D2;
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
    t_symbol *arrayK;
    t_symbol *arrayD;
} foo1 ;

typedef struct _pmpd2d2d {
 	t_object  x_obj;
	struct _link link[nb_max_link];
	struct _mass mass[nb_max_mass];
	t_outlet *main_outlet;
    t_outlet *info_outlet;
    int nb_link;
	int nb_mass;
    t_float minX, maxX, minY, maxY;
    t_int grab; // si on grab une mass ou pas
	t_int grab_nb; // la masse grabé
} t_pmpd2d;

t_float tabread2(t_pmpd2d *x, t_float pos, t_symbol *array)
{
    t_garray *a;
	int npoints;
	t_word *vec;
	t_float posx;
	
    if (!(a = (t_garray *)pd_findbyclass(array, garray_class)))
        pd_error(x, "%s: no such array", array);
    else if (!garray_getfloatwords(a, &npoints, &vec))
        pd_error(x, "%s: bad template for tabLink", array);
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

void pmpd2d_reset(t_pmpd2d *x)
{
	x->nb_link = 0;
	x->nb_mass = 0;
    x->minX = -1000000;
    x->maxX = 1000000;
    x->minY = -1000000;
    x->maxY = 1000000;
    x->grab = 0;
}

void pmpd2d_infosL(t_pmpd2d *x)
{
    int i;
    post("list of mass");
    post("number, Id, mobile, mass, positionX Y, speedX Y, forcesX Y");
	for(i=0; i < x->nb_mass; i++)
    {
        post("masse %i: %s, %d, %f, %f, %f, %f, %f, %f, %f",i, x->mass[i].Id->s_name, x->mass[i].mobile, 1/x->mass[i].invM, x->mass[i].posX, x->mass[i].posY, x->mass[i].speedX, x->mass[i].speedY, x->mass[i].forceX, x->mass[i].forceY );
    }

    post("list of link");
    post("number, Id, mass1, mass2, K, D, Pow, L, Lmin, Lmax");
	for(i=0; i < x->nb_link; i++)
    {
        post("link %i: %s, %i, %i, %f, %f, %f, %f, %f, %f", i, x->link[i].Id->s_name, x->link[i].mass1->num, x->link[i].mass2->num, x->link[i].K, x->link[i].D, x->link[i].Pow, x->link[i].L, x->link[i].Lmin, x->link[i].Lmax);
    }
}

void pmpd2d_bang(t_pmpd2d *x)
{
// this part is doing all the PM
	t_float F, L, Lx,Ly, Fx, Fy, tmpX, tmpY;
	t_int i;
    // post("bang");

	for (i=1; i<x->nb_mass; i++)
	// compute new masses position
		if (x->mass[i].mobile > 0) // only if mobile
		{
			x->mass[i].speedX += x->mass[i].forceX * x->mass[i].invM;
			x->mass[i].speedY += x->mass[i].forceY * x->mass[i].invM;
			// x->mass[i].forceX = 0;
			// x->mass[i].forceY = 0;		
			x->mass[i].posX += x->mass[i].speedX ;
			x->mass[i].posY += x->mass[i].speedY ;
            if ( (x->mass[i].posX < x->minX) || (x->mass[i].posX > x->maxX) || (x->mass[i].posY < x->minY) || (x->mass[i].posY > x->maxY) ) 
            {
				tmpX = min(x->maxX,max(x->minX,x->mass[i].posX));
				tmpY = min(x->maxY,max(x->minY,x->mass[i].posY));
				x->mass[i].speedX -= x->mass[i].posX - tmpX;
				x->mass[i].speedY -= x->mass[i].posY - tmpY;
				x->mass[i].posX = tmpX;
				x->mass[i].posY = tmpY;
			}
			x->mass[i].forceX = -x->mass[i].D2 * x->mass[i].speedX;
			x->mass[i].forceY = -x->mass[i].D2 * x->mass[i].speedY;
		}

	for (i=0; i<x->nb_link; i++)
	// compute link forces
	{
		Lx = x->link[i].mass1->posX - x->link[i].mass2->posX;
		Ly = x->link[i].mass1->posY - x->link[i].mass2->posY;
		L = sqrt( sqr(Lx) + sqr(Ly) );
		
		if ( (L >= x->link[i].Lmin) & (L < x->link[i].Lmax)  & (L != 0))
		{
			if (x->link[i].lType == 2)
			{ // K et D viennent d'une table
				if (x->link[i].D == 0)
					F = 0;
				else
					F  = tabread2(x, (L - x->link[i].distance) / x->link[i].D, x->link[i].arrayD);
				if (x->link[i].K != 0)
					F += tabread2(x, L / x->link[i].K, x->link[i].arrayK);
			}
			else
			{			
				F  = x->link[i].D * (L - x->link[i].distance) ;
				F += x->link[i].K *  pow_ch( L - x->link[i].L, x->link[i].Pow);
			}

			Fx = F * Lx/L;
			Fy = F * Ly/L;	
				
			if (x->link[i].lType == 1)
			{ // on projette selon 1 axe
				F = Fx*x->link[i].VX + Fy*x->link[i].VY; // produit scalaire de la force sur le vecteur qui la porte
				Fx = F*x->link[i].VX; // V est unitaire, dc on projete sans pb
				Fy = F*x->link[i].VY;				
			}
			
			x->link[i].mass1->forceX -= Fx;
			x->link[i].mass1->forceY -= Fy;
			x->link[i].mass2->forceX += Fx;
			x->link[i].mass2->forceY += Fy;
		}
		x->link[i].distance=L;
	}
}

void pmpd2d_mass(t_pmpd2d *x, t_symbol *Id, t_float mobile, t_float M, t_float posX, t_float posY )
{ // add a mass : Id, invM speedX posX

	if (M<=0) M=1;
	x->mass[x->nb_mass].Id = Id;
	x->mass[x->nb_mass].mobile = (int)mobile;
	x->mass[x->nb_mass].invM = 1/M;
	x->mass[x->nb_mass].speedX = 0;
	x->mass[x->nb_mass].speedY = 0;
	x->mass[x->nb_mass].posX = posX;
	x->mass[x->nb_mass].posY = posY;
	x->mass[x->nb_mass].forceX = 0;
	x->mass[x->nb_mass].forceY = 0;
	x->mass[x->nb_mass].num = x->nb_mass;

	x->nb_mass++ ;
	x->nb_mass = min ( nb_max_mass -1, x->nb_mass );
}

void pmpd2d_create_link(t_pmpd2d *x, t_symbol *Id, int mass1, int mass2, t_float K, t_float D, t_float Pow, t_float Lmin, t_float Lmax, t_int type)
{ // create a link based on mass number

    if ((x->nb_mass>1) & (mass1 != mass2) & (mass1 >= 0) & (mass2 >= 0) & (mass1 < x->nb_mass) & (mass2 < x->nb_mass) )
    {
	    x->link[x->nb_link].lType = type;
	    x->link[x->nb_link].Id = Id;
	    x->link[x->nb_link].mass1 = &x->mass[mass1]; 
	    x->link[x->nb_link].mass2 = &x->mass[mass2];
	    x->link[x->nb_link].K = K;
	    x->link[x->nb_link].D = D;
	    x->link[x->nb_link].L = sqrt(sqr(x->mass[mass1].posX - x->mass[mass2].posX) + sqr(x->mass[mass1].posY - x->mass[mass2].posY));
	    x->link[x->nb_link].Pow = Pow;
	    x->link[x->nb_link].Lmin = Lmin;
	    x->link[x->nb_link].Lmax = Lmax;
	    x->link[x->nb_link].distance = x->link[x->nb_link].L ;

	    x->nb_link++ ;
	    x->nb_link = min ( nb_max_link -1, x->nb_link );
    }
}

void pmpd2d_link(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
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

    if ( ( argv[1].a_type == A_FLOAT ) & ( argv[2].a_type == A_FLOAT ) )
    {
        pmpd2d_create_link(x, Id, mass1, mass2, K, D, Pow, Lmin, Lmax, 0);
    }
    else
    if ( ( argv[1].a_type == A_SYMBOL ) & ( argv[2].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                pmpd2d_create_link(x, Id, i, mass2, K, D, Pow, Lmin, Lmax, 0);
            }
        }
    }
    else
    if ( ( argv[1].a_type == A_FLOAT ) & ( argv[2].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(2,argc,argv) == x->mass[i].Id)
            {
                pmpd2d_create_link(x, Id, mass1, i, K, D, Pow, Lmin, Lmax, 0);
            }
        }
    }
    else
    if ( ( argv[1].a_type == A_SYMBOL ) & ( argv[2].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            for (j=0; j< x->nb_mass; j++)
            {
                if ( (atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)&(atom_getsymbolarg(2,argc,argv) == x->mass[j].Id))
                {
                    pmpd2d_create_link(x, Id, i, j, K, D, Pow, Lmin, Lmax, 0);
                }
            }   
        }
    }
}

void pmpd2d_tLink(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{ // add a link : Id, *mass1, *mass2, K, D, Vx, Vy, Pow, Lmin, Lmax;

    int i, j;
	
    t_symbol *Id = atom_getsymbolarg(0,argc,argv);
    int mass1 = atom_getfloatarg(1, argc, argv);
    int mass2 = atom_getfloatarg(2, argc, argv);
    t_float K = atom_getfloatarg(3, argc, argv);
    t_float D = atom_getfloatarg(4, argc, argv);
    t_float vecteurX = atom_getfloatarg(5, argc, argv);
    t_float vecteurY = atom_getfloatarg(6, argc, argv);
    t_float vecteur = sqrt( sqr(vecteurX) + sqr(vecteurY) );
    vecteurX /= vecteur;
    vecteurY /= vecteur;
    t_float Pow = 1; 
    if (argc > 7) Pow = atom_getfloatarg(5, argc, argv);
    t_float Lmin = 0;
    if (argc > 8) Lmin = atom_getfloatarg(6, argc, argv);
    t_float Lmax =  1000000;
    if (argc > 9) Lmax = atom_getfloatarg(7, argc, argv);

    if ( ( argv[1].a_type == A_FLOAT ) & ( argv[2].a_type == A_FLOAT ) )
    {
        pmpd2d_create_link(x, Id, mass1, mass2, K, D, Pow, Lmin, Lmax, 0);
		x->link[x->nb_link-1].VX = vecteurX;
		x->link[x->nb_link-1].VY = vecteurY;
    }
    else
    if ( ( argv[1].a_type == A_SYMBOL ) & ( argv[2].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                pmpd2d_create_link(x, Id, i, mass2, K, D, Pow, Lmin, Lmax, 0);
            	x->link[x->nb_link-1].VX = vecteurX;
				x->link[x->nb_link-1].VY = vecteurY;
            }
        }
    }
    else
    if ( ( argv[1].a_type == A_FLOAT ) & ( argv[2].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(2,argc,argv) == x->mass[i].Id)
            {
                pmpd2d_create_link(x, Id, mass1, i, K, D, Pow, Lmin, Lmax, 0);
            	x->link[x->nb_link-1].VX = vecteurX;
				x->link[x->nb_link-1].VY = vecteurY;
			}
        }
    }
    else
    if ( ( argv[1].a_type == A_SYMBOL ) & ( argv[2].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            for (j=0; j< x->nb_mass; j++)
            {
                if ( (atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)&(atom_getsymbolarg(2,argc,argv) == x->mass[j].Id))
                {
                    pmpd2d_create_link(x, Id, i, j, K, D, Pow, Lmin, Lmax, 0);
                	x->link[x->nb_link-1].VX = vecteurX;
					x->link[x->nb_link-1].VY = vecteurY;
				}
            }   
        }
    }
}

void pmpd2d_tabLink(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j;
	
    t_symbol *Id = atom_getsymbolarg(0,argc,argv);
    int mass1 = atom_getfloatarg(1, argc, argv);
    int mass2 = atom_getfloatarg(2, argc, argv);
    t_symbol *arrayK = atom_getsymbolarg(3,argc,argv);
    t_float K = atom_getfloatarg(4, argc, argv);
    t_symbol *arrayD = atom_getsymbolarg(5,argc,argv);    
    t_float D = atom_getfloatarg(6, argc, argv);

    if ( ( argv[1].a_type == A_FLOAT ) & ( argv[2].a_type == A_FLOAT ) )
    {
        pmpd2d_create_link(x, Id, mass1, mass2, K, D, 1, 0, 1000000, 2);
		x->link[x->nb_link-1].arrayK = arrayK;
		x->link[x->nb_link-1].arrayD = arrayD;
    }
    else
    if ( ( argv[1].a_type == A_SYMBOL ) & ( argv[2].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                pmpd2d_create_link(x, Id, i, mass2, K, D, 1, 0, 1000000, 2);
				x->link[x->nb_link-1].arrayK = arrayK;
				x->link[x->nb_link-1].arrayD = arrayD;
            }
        }
    }
    else
    if ( ( argv[1].a_type == A_FLOAT ) & ( argv[2].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(2,argc,argv) == x->mass[i].Id)
            {
                pmpd2d_create_link(x, Id, mass1, i, K, D, 1, 0, 1000000, 2);
				x->link[x->nb_link-1].arrayK = arrayK;
				x->link[x->nb_link-1].arrayD = arrayD;
			}
        }
    }
    else
    if ( ( argv[1].a_type == A_SYMBOL ) & ( argv[2].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            for (j=0; j< x->nb_mass; j++)
            {
                if ( (atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)&(atom_getsymbolarg(2,argc,argv) == x->mass[j].Id))
                {
                    pmpd2d_create_link(x, Id, i, j, K, D, 1, 0, 1000000, 2);
					x->link[x->nb_link-1].arrayK = arrayK;
					x->link[x->nb_link-1].arrayD = arrayD;
				}
            }   
        }
    }

}

void pmpd2d_setK(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
	    x->link[tmp].K = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) )
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

void pmpd2d_setD(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
	    x->link[tmp].D = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) )
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

void pmpd2d_setL(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
	    x->link[tmp].L = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
	            x->link[i].L = atom_getfloatarg(1, argc, argv);
            }
        }
    }
	if ( ( argv[0].a_type == A_FLOAT ) & ( argc == 1 ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].L = x->link[tmp].mass2->posX - x->link[tmp].mass1->posX;
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argc == 1 ) )
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

void pmpd2d_setLinkId(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_SYMBOL ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
	    x->link[tmp].Id = atom_getsymbolarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_SYMBOL ) )
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

void pmpd2d_setMassId(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
// add a force to a specific mass
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_SYMBOL ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].Id = atom_getsymbolarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_SYMBOL ) )
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

void pmpd2d_forceX(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
// add a force to a specific mass
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].forceX += atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) )
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

void pmpd2d_forceY(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
// add a force to a specific mass
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].forceY += atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) )
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

void pmpd2d_force(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
// add a force to a specific mass
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) & ( argv[2].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].forceX += atom_getfloatarg(1, argc, argv);
	    x->mass[tmp].forceY += atom_getfloatarg(2, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) & ( argv[2].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].forceX += atom_getfloatarg(1, argc, argv);
	            x->mass[i].forceY += atom_getfloatarg(2, argc, argv);
            }
        }
    }
}

void pmpd2d_posX(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
// displace a mass to a certain position
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].posX = atom_getfloatarg(1, argc, argv);
   	    x->mass[tmp].speedX = 0; 
        x->mass[tmp].forceX = 0; 
        
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) )
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

void pmpd2d_posY(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
// displace a mass to a certain position
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].posY = atom_getfloatarg(1, argc, argv);
   	    x->mass[tmp].speedY = 0; 
        x->mass[tmp].forceY = 0; 
        
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) )
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

void pmpd2d_pos(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
// displace a mass to a certain position
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) & ( argv[2].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].posX = atom_getfloatarg(1, argc, argv);
   	    x->mass[tmp].speedX = 0; 
        x->mass[tmp].forceX = 0; 
   	    x->mass[tmp].posY = atom_getfloatarg(2, argc, argv);
   	    x->mass[tmp].speedY = 0; 
        x->mass[tmp].forceY = 0; 
        
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) & ( argv[2].a_type == A_FLOAT ) )
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

            }
        }
    }
}

void pmpd2d_minX(t_pmpd2d *x, t_float min)
{
	x->minX = min;
}

void pmpd2d_maxX(t_pmpd2d *x, t_float max)
{
	x->maxX = max;
}

void pmpd2d_minY(t_pmpd2d *x, t_float min)
{
	x->minY = min;
}

void pmpd2d_maxY(t_pmpd2d *x, t_float max)
{
	x->maxY = max;
}

void pmpd2d_setFixed(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd2d_setMobile(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd2d_setD2(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].D2 = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].D2 = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd2d_setSpeedX(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].speedX = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) )
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

void pmpd2d_setSpeedY(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].speedY = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) )
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

void pmpd2d_setSpeed(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) & ( argv[2].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].speedX = atom_getfloatarg(1, argc, argv);
	    x->mass[tmp].speedY = atom_getfloatarg(2, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) & ( argv[2].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].speedX = atom_getfloatarg(1, argc, argv);
	            x->mass[i].speedY = atom_getfloatarg(2, argc, argv);
            }
        }
    }
}

void pmpd2d_addPosX(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].posX += atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) )
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

void pmpd2d_addPosY(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].posY += atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) )
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

void pmpd2d_addPos(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) & ( argv[2].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].posX += atom_getfloatarg(1, argc, argv);
	    x->mass[tmp].posY += atom_getfloatarg(2, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) & ( argv[2].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].posX += atom_getfloatarg(1, argc, argv);
	            x->mass[i].posY += atom_getfloatarg(2, argc, argv);
            }
        }
    }
}

void pmpd2d_setForceX(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].forceX = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) )
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

void pmpd2d_setForceY(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].forceY = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) )
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

void pmpd2d_setForce(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) & ( argv[2].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].forceX = atom_getfloatarg(1, argc, argv);
	    x->mass[tmp].forceY = atom_getfloatarg(2, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) & ( argv[2].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].forceX = atom_getfloatarg(1, argc, argv);
	            x->mass[i].forceY = atom_getfloatarg(2, argc, argv);
            }
        }
    }
}

void pmpd2d_get(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_symbol *toget; 
    t_atom  toout[3];
    toget = atom_getsymbolarg(0, argc, argv);

    if ( (toget == gensym("massesPos")) & (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) & (i<x->nb_mass) )
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->mass[i].posX);
            SETFLOAT(&(toout[2]), x->mass[i].posY);
            outlet_anything(x->main_outlet, gensym("massesPosNo"), 3, toout);
        }  
    }
    else
    if ( (toget == gensym("massesPos")) & (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                SETFLOAT(&(toout[0]), i);
                SETFLOAT(&(toout[1]), x->mass[i].posX);
                SETFLOAT(&(toout[2]), x->mass[i].posY);
                outlet_anything(x->main_outlet, gensym("massesPosId"), 3, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("massesPos")) & (argc == 1) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->mass[i].posX);
            SETFLOAT(&(toout[2]), x->mass[i].posY);
            outlet_anything(x->main_outlet, gensym("massesPos"), 3, toout);
        } 
    }
    else
    if ( (toget == gensym("massesPosName")) & (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) & (i<x->nb_mass) )
        {
            SETSYMBOL(&(toout[0]), x->mass[i].Id);
            SETFLOAT(&(toout[1]), x->mass[i].posX);
            SETFLOAT(&(toout[2]), x->mass[i].posY);
            outlet_anything(x->main_outlet, gensym("massesPosNameNo"), 3, toout);
        }  
    }
    else
    if ( (toget == gensym("massesPosName")) & (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                SETSYMBOL(&(toout[0]), x->mass[i].Id);
                SETFLOAT(&(toout[1]), x->mass[i].posX);
				SETFLOAT(&(toout[2]), x->mass[i].posY);
                outlet_anything(x->main_outlet, gensym("massesPosNameId"), 3, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("massesPosName")) & (argc == 1) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            SETSYMBOL(&(toout[0]), x->mass[i].Id);
            SETFLOAT(&(toout[1]), x->mass[i].posX);
            SETFLOAT(&(toout[2]), x->mass[i].posY);
            outlet_anything(x->main_outlet, gensym("massesPosName"), 3, toout);
        } 
    }
    else
    if ( (toget == gensym("massesSpeeds")) & (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) & (i<x->nb_mass) )
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->mass[i].speedX);
            SETFLOAT(&(toout[2]), x->mass[i].speedY);
            outlet_anything(x->main_outlet, gensym("massesSpeedsNo"), 3, toout);
        }  
    }
    else
    if ( (toget == gensym("massesSpeeds")) & (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                SETFLOAT(&(toout[0]), i);
                SETFLOAT(&(toout[1]), x->mass[i].speedX);
				SETFLOAT(&(toout[2]), x->mass[i].speedY);
                outlet_anything(x->main_outlet, gensym("massesSpeedsId"), 3, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("massesSpeeds")) & (argc == 1) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->mass[i].speedX);
            SETFLOAT(&(toout[2]), x->mass[i].speedY);
            outlet_anything(x->main_outlet, gensym("massesSpeeds"), 3, toout);
        } 
    }
    else
    if ( (toget == gensym("massesSpeedsName")) & (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) & (i<x->nb_mass) )
        {
            SETSYMBOL(&(toout[0]), x->mass[i].Id);
            SETFLOAT(&(toout[1]), x->mass[i].speedX);
            SETFLOAT(&(toout[2]), x->mass[i].speedY);
            outlet_anything(x->main_outlet, gensym("massesSpeedsNameNo"), 3, toout);
        }  
    }
    else
    if ( (toget == gensym("massesSpeedsName")) & (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                SETSYMBOL(&(toout[0]), x->mass[i].Id);
                SETFLOAT(&(toout[1]), x->mass[i].speedX);
				SETFLOAT(&(toout[2]), x->mass[i].speedY);
                outlet_anything(x->main_outlet, gensym("massesSpeedsNameId"), 3, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("massesSpeedsName")) & (argc == 1) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            SETSYMBOL(&(toout[0]), x->mass[i].Id);
            SETFLOAT(&(toout[1]), x->mass[i].speedX);
            SETFLOAT(&(toout[2]), x->mass[i].speedY);
            outlet_anything(x->main_outlet, gensym("massesSpeedsName"), 3, toout);
        } 
    }
    else
    if ( (toget == gensym("massesForces")) & (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) & (i<x->nb_mass) )
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->mass[i].forceX);
            SETFLOAT(&(toout[2]), x->mass[i].forceY);
            outlet_anything(x->main_outlet, gensym("massesForcesNo"), 3, toout);
        }  
    }
    else
    if ( (toget == gensym("massesForces")) & (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                SETFLOAT(&(toout[0]), i);
                SETFLOAT(&(toout[1]), x->mass[i].forceX);
				SETFLOAT(&(toout[2]), x->mass[i].forceY);
                outlet_anything(x->main_outlet, gensym("massesForcesId"), 3, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("massesForces")) & (argc == 1) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->mass[i].forceX);
            SETFLOAT(&(toout[2]), x->mass[i].forceY);
            outlet_anything(x->main_outlet, gensym("massesForces"), 3, toout);
        } 
    }
    else
    if ( (toget == gensym("massesForcesName")) & (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) & (i<x->nb_mass) )
        {
            SETSYMBOL(&(toout[0]), x->mass[i].Id);
            SETFLOAT(&(toout[1]), x->mass[i].forceX);
            SETFLOAT(&(toout[2]), x->mass[i].forceY);
            outlet_anything(x->main_outlet, gensym("massesForcesNameNo"), 3, toout);
        }
    }
    else
    if ( (toget == gensym("massesForcesName")) & (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                SETSYMBOL(&(toout[0]), x->mass[i].Id);
                SETFLOAT(&(toout[1]), x->mass[i].forceX);
				SETFLOAT(&(toout[2]), x->mass[i].forceY);
                outlet_anything(x->main_outlet, gensym("massesForcesNameId"), 3, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("massesForcesName")) & (argc == 1) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            SETSYMBOL(&(toout[0]), x->mass[i].Id);
            SETFLOAT(&(toout[1]), x->mass[i].forceX);
            SETFLOAT(&(toout[2]), x->mass[i].forceY);
            outlet_anything(x->main_outlet, gensym("massesForcesName"), 3, toout);
        } 
    }
    else
    if ( (toget == gensym("linksPos")) & (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) & (i<x->nb_mass) )
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
            SETFLOAT(&(toout[2]), x->link[i].mass1->posY);
            SETFLOAT(&(toout[3]), x->link[i].mass2->posX);
            SETFLOAT(&(toout[4]), x->link[i].mass2->posY);
            outlet_anything(x->main_outlet, gensym("linksPosNo"), 5, toout);
        }
    }
    else
    if ( (toget == gensym("linksPos")) & (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(toout[0]), i);
				SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
				SETFLOAT(&(toout[2]), x->link[i].mass1->posY);
				SETFLOAT(&(toout[3]), x->link[i].mass2->posX);
				SETFLOAT(&(toout[4]), x->link[i].mass2->posY);
				outlet_anything(x->main_outlet, gensym("linksPosNo"), 5, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("linksPos")) & (argc == 1) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
            SETFLOAT(&(toout[2]), x->link[i].mass1->posY);
            SETFLOAT(&(toout[3]), x->link[i].mass2->posX);
            SETFLOAT(&(toout[4]), x->link[i].mass2->posY);
            outlet_anything(x->main_outlet, gensym("linksPosNo"), 5, toout);
        } 
    }
    else
    if ( (toget == gensym("linksPosName")) & (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) & (i<x->nb_mass) )
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
            SETFLOAT(&(toout[2]), x->link[i].mass1->posY);
            SETFLOAT(&(toout[3]), x->link[i].mass2->posX);
            SETFLOAT(&(toout[4]), x->link[i].mass2->posY);
            outlet_anything(x->main_outlet, gensym("linksPosNo"), 5, toout);
        }
    }
    else
    if ( (toget == gensym("linksPosName")) & (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->link[i].Id)
            {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
            SETFLOAT(&(toout[2]), x->link[i].mass1->posY);
            SETFLOAT(&(toout[3]), x->link[i].mass2->posX);
            SETFLOAT(&(toout[4]), x->link[i].mass2->posY);
            outlet_anything(x->main_outlet, gensym("linksPosNo"), 5, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("linksPosName")) & (argc == 1) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
            SETFLOAT(&(toout[2]), x->link[i].mass1->posY);
            SETFLOAT(&(toout[3]), x->link[i].mass2->posX);
            SETFLOAT(&(toout[4]), x->link[i].mass2->posY);
            outlet_anything(x->main_outlet, gensym("linksPosNo"), 5, toout);
        } 
    }
    else
        error("not get attribute");
}

void pmpd2d_massesPosXL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[nb_max_link];

    for (i=0; i < x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].posX);
    }
    outlet_anything(x->main_outlet, gensym("massesPosXL"),x->nb_mass , pos_list);
}

void pmpd2d_massesForcesXL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[nb_max_link];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].forceX);
    }
    outlet_anything(x->main_outlet, gensym("massesForcesXL"),x->nb_mass , pos_list);
}

void pmpd2d_massesSpeedsXL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[nb_max_link];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].speedX);
    }
    outlet_anything(x->main_outlet, gensym("massesSpeedsXL"),x->nb_mass , pos_list);
}

void pmpd2d_massesPosXT(t_pmpd2d *x, t_symbol *tab_name)
{
    int i, vecsize;
    t_garray *a;
     t_word *vec;
     
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

void pmpd2d_massesSpeedsXT(t_pmpd2d *x, t_symbol *tab_name)
{
    int i, vecsize;
    t_garray *a;
     t_word *vec;
     
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

void pmpd2d_massesForcesXT(t_pmpd2d *x, t_symbol *tab_name)
{
    int i, vecsize;
    t_garray *a;
     t_word *vec;
     
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

void pmpd2d_massesPosYL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[nb_max_link];

    for (i=0; i < x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].posY);
    }
    outlet_anything(x->main_outlet, gensym("massesPosYL"),x->nb_mass , pos_list);
}

void pmpd2d_massesForcesYL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[nb_max_link];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].forceY);
    }
    outlet_anything(x->main_outlet, gensym("massesForcesYL"),x->nb_mass , pos_list);
}

void pmpd2d_massesSpeedsYL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[nb_max_link];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].speedY);
    }
    outlet_anything(x->main_outlet, gensym("massesSpeedsYL"),x->nb_mass , pos_list);
}

void pmpd2d_massesPosYT(t_pmpd2d *x, t_symbol *tab_name)
{
    int i, vecsize;
    t_garray *a;
     t_word *vec;
     
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

void pmpd2d_massesSpeedsYT(t_pmpd2d *x, t_symbol *tab_name)
{
    int i, vecsize;
    t_garray *a;
     t_word *vec;
     
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

void pmpd2d_massesForcesYT(t_pmpd2d *x, t_symbol *tab_name)
{
    int i, vecsize;
    t_garray *a;
     t_word *vec;
     
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

void pmpd2d_massesPosL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[2*nb_max_link];

    for (i=0; i < x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[2*i]  ),x->mass[i].posX);
        SETFLOAT(&(pos_list[2*i+1]),x->mass[i].posY);
    }
    outlet_anything(x->main_outlet, gensym("massesPosL"),2*x->nb_mass , pos_list);
}

void pmpd2d_massesForcesL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[2*nb_max_link];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[2*i]  ),x->mass[i].forceX);
        SETFLOAT(&(pos_list[2*i+1]),x->mass[i].forceY);
    }
    outlet_anything(x->main_outlet, gensym("massesForcesL"),2*x->nb_mass , pos_list);
}

void pmpd2d_massesSpeedsL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[2*nb_max_link];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[2*i]  ),x->mass[i].speedX);
        SETFLOAT(&(pos_list[2*i+1]),x->mass[i].speedY);
    }
    outlet_anything(x->main_outlet, gensym("massesSpeedsL"),2*x->nb_mass , pos_list);
}

void pmpd2d_massesPosT(t_pmpd2d *x, t_symbol *tab_name)
{
    int i, vecsize;
    t_garray *a;
     t_word *vec;
     
     if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
        pd_error(x, "%s: no such array", tab_name->s_name);
    else if (!garray_getfloatwords(a, &vecsize, &vec))
        pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
    else
    {
		int taille_max = x->nb_mass;
		taille_max = min(taille_max, vecsize/2);
		for (i=0; i < taille_max ; i++)
		{
			        vec[2*i  ].w_float = x->mass[i].posX;
			        vec[2*i+1].w_float = x->mass[i].posY;
		}
        garray_redraw(a);
    }
}

void pmpd2d_massesSpeedsT(t_pmpd2d *x, t_symbol *tab_name)
{
    int i, vecsize;
    t_garray *a;
     t_word *vec;
     
     if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
        pd_error(x, "%s: no such array", tab_name->s_name);
    else if (!garray_getfloatwords(a, &vecsize, &vec))
        pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
    else
    {
		int taille_max = x->nb_mass;
		taille_max = min(taille_max, vecsize/2);
		for (i=0; i < taille_max ; i++)
		{
			        vec[2*i  ].w_float = x->mass[i].speedX;
			        vec[2*i+1].w_float = x->mass[i].speedY;
		}
        garray_redraw(a);
    }
}

void pmpd2d_massesForcesT(t_pmpd2d *x, t_symbol *tab_name)
{
    int i, vecsize;
    t_garray *a;
     t_word *vec;
     
     if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
        pd_error(x, "%s: no such array", tab_name->s_name);
    else if (!garray_getfloatwords(a, &vecsize, &vec))
        pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
    else
    {
		int taille_max = x->nb_mass;
		taille_max = min(taille_max, vecsize/2);
		for (i=0; i < taille_max ; i++)
		{
			    vec[2*i  ].w_float = x->mass[i].forceX;
			    vec[2*i+1].w_float = x->mass[i].forceY;
		}
        garray_redraw(a);
    }
}

void pmpd2d_massesPosMean(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, somme;
    t_int i,j;
    t_atom mean[3];

	sommeX = 0;
	sommeY = 0;
	somme = 0;
	j = 0;
	
    if ( (argc >= 1) & (argv[0].a_type == A_SYMBOL) ) 
    {
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				sommeX += x->mass[i].posX;
				sommeY += x->mass[i].posY;
				somme +=  sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY)); // distance au centre
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
				somme +=  sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY)); // distance au centre
				j++;
		}
	}	
	
	sommeX /= j;
	sommeY /= j;
	somme  /= j;	
	
    SETFLOAT(&(mean[0]),sommeX);
    SETFLOAT(&(mean[1]),sommeY);
    SETFLOAT(&(mean[2]),somme);
    
    outlet_anything(x->main_outlet, gensym("massesPosMean"),3 , mean);
}

void pmpd2d_massesPosStd(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, somme;
    t_int i,j;
    t_float stdX, stdY,std;
    t_atom std_out[3];

	sommeX = 0;
	sommeY = 0;
	somme  = 0;
	stdX = 0;
	stdY = 0;
	std  = 0;
	j = 0;
	
    if ( (argc >= 1) & (argv[0].a_type == A_SYMBOL) ) 
    {
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				sommeX += x->mass[i].posX;
				sommeY += x->mass[i].posY;
				somme +=  sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY)); // distance au centre
				j++;
			}
		}
		sommeX /= j;
		sommeY /= j;
		somme /= j;
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				stdX += sqr(x->mass[i].posX-sommeX);
				stdY += sqr(x->mass[i].posY-sommeY);
				std  +=  sqr(sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY))-somme);
			}
		}		
    }
	else
	{
		for (i=0; i< x->nb_mass; i++)
        {
			sommeX += x->mass[i].posX;
			sommeY += x->mass[i].posY;
			somme +=  sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY)); // distance au centre
			j++;
		}
		sommeX /= j;
		sommeY /= j;
		somme /= j;
		for (i=0; i< x->nb_mass; i++)
        {
			stdX += sqr(x->mass[i].posX-sommeX);
			stdY += sqr(x->mass[i].posY-sommeY);
			std  += sqr(sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY))-somme);
		}
	}	
	
	stdX = sqrt(stdX/j);
	stdY = sqrt(stdY/j);
	std  = sqrt(std /j);	

    SETFLOAT(&(std_out[0]),stdX);
    SETFLOAT(&(std_out[1]),stdY);
    SETFLOAT(&(std_out[2]),std);
    
    outlet_anything(x->main_outlet, gensym("massesPosStd"),3 , std_out);
}

void pmpd2d_grabMass(t_pmpd2d *x, t_float posX, t_float posY, t_float grab)
{
	t_float dist, tmp;
	t_int i;
	
	if (grab == 0)
		x->grab=0;
	if ((x->grab == 0)&(grab == 1)&(x->nb_mass > 0))
	{
		x->grab=1;
		x->grab_nb= 0;
		dist = sqr(x->mass[0].posX - posX) + sqr(x->mass[0].posY - posY);
		for (i=1; i<x->nb_mass; i++)
		{
			tmp = sqr(x->mass[i].posX - posX) + sqr(x->mass[i].posY - posY);
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
	}
}

void *pmpd2d_new()
{
	t_pmpd2d *x = (t_pmpd2d *)pd_new(pmpd2d_class);

	pmpd2d_reset(x);
	
	x->main_outlet=outlet_new(&x->x_obj, 0);
	// x->info_outlet=outlet_new(&x->x_obj, 0); // TODO

	return (void *)x;
}

void pmpd2d_setup(void) 
{
 pmpd2d_class = class_new(gensym("pmpd2d"),
        (t_newmethod)pmpd2d_new,
        0, sizeof(t_pmpd2d),CLASS_DEFAULT, 0);

	class_addbang(pmpd2d_class, pmpd2d_bang);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_reset,           gensym("reset"), 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_infosL,          gensym("infosL"), 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_mass,            gensym("mass"), A_DEFSYMBOL, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_link,            gensym("link"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_tabLink,         gensym("tabLink"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_tLink,           gensym("tLink"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_setK,            gensym("setK"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_setD,            gensym("setD"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_setL,            gensym("setL"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_setLinkId,       gensym("setLinkId"), A_GIMME, 0);

	class_addmethod(pmpd2d_class, (t_method)pmpd2d_setMassId,       gensym("setMassId"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_pos,             gensym("pos"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_posX,            gensym("posX"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_posY,            gensym("posY"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_force,           gensym("force"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_forceX,          gensym("forceX"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_forceY,          gensym("forceY"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_minX,            gensym("Xmin"), A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_maxX,            gensym("Xmax"), A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_minY,            gensym("Ymin"), A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_maxY,            gensym("Ymax"), A_DEFFLOAT, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_setFixed,        gensym("setFixed"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_setMobile,       gensym("setMobile"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_setD2,           gensym("setD2"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_setSpeed,        gensym("setSpeed"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_setSpeedX,       gensym("setSpeedX"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_setSpeedY,       gensym("setSpeedY"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_setForce,        gensym("setForce"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_setForceX,       gensym("setForceX"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_setForceY,       gensym("setForceY"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_addPos,          gensym("addPos"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_addPosX,         gensym("addPosX"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_addPosY,         gensym("addPosY"), A_GIMME, 0);

	class_addmethod(pmpd2d_class, (t_method)pmpd2d_get,             gensym("get"), A_GIMME, 0);
	
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesPosL,      gensym("massesPosL"), 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesSpeedsL,   gensym("massesSpeedsL"), 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesForcesL,   gensym("massesForcesL"), 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesPosT,      gensym("massesPosT"), A_DEFSYMBOL, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesSpeedsT,   gensym("massesSpeedsT"),A_DEFSYMBOL, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesForcesT,   gensym("massesForcesT"),A_DEFSYMBOL, 0);
	
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesPosXL,     gensym("massesPosXL"), 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesSpeedsXL,  gensym("massesSpeedsXL"), 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesForcesXL,  gensym("massesForcesXL"), 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesPosXT,     gensym("massesPosXT"), A_DEFSYMBOL, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesSpeedsXT,  gensym("massesSpeedsXT"),A_DEFSYMBOL, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesForcesXT,  gensym("massesForcesXT"),A_DEFSYMBOL, 0);
	
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesPosYL,     gensym("massesPosYL"), 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesSpeedsYL,  gensym("massesSpeedsYL"), 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesForcesYL,  gensym("massesForcesYL"), 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesPosYT,     gensym("massesPosYT"), A_DEFSYMBOL, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesSpeedsYT,  gensym("massesSpeedsYT"),A_DEFSYMBOL, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesForcesYT,  gensym("massesForcesYT"),A_DEFSYMBOL, 0);
	
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesPosMean,   gensym("massesPosMean"), A_GIMME, 0);
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_massesPosStd,    gensym("massesPosStd"),A_GIMME, 0);
	// class_addmethod(pmpd2d_class, (t_method)pmpd2d_linkLMean,       gensym("linkLMean"),A_GIMME, 0);
	// class_addmethod(pmpd2d_class, (t_method)pmpd2d_linkLStd,        gensym("linkLStd"),A_GIMME, 0);
	
	class_addmethod(pmpd2d_class, (t_method)pmpd2d_grabMass,        gensym("grabMass"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);

}

