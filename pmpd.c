/*
--------------------------  pmpd  ---------------------------------------- 
     
	  
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

t_float pow_ch(t_float x, t_float y)
{
	return x > 0 ? pow(x,y) : -pow(-x,y);
}

t_float sqr(t_float x)
{
	return x*x ;
}

static t_class *pmpd_class;

typedef struct _mass {
    t_symbol *Id;
    int mobile;
	t_float invM;
	t_float speedX;
	t_float posX;
	t_float forceX;
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
    t_symbol *arrayK;
    t_symbol *arrayD;
    t_float K_L; // longeur du tabeau K
	t_float D_L; // longeur du tabeau D
} foo1 ;

typedef struct _pmpd {
 	t_object  x_obj;
	struct _link link[nb_max_link];
	struct _mass mass[nb_max_mass];
	t_outlet *main_outlet;
    t_outlet *info_outlet;
    int nb_link;
	int nb_mass;
    t_float minX, maxX;
    t_int grab; // si on grab une mass ou pas
	t_int grab_nb; // la masse grabé
} t_pmpd;

t_float tabread2(t_pmpd *x, t_float pos, t_symbol *array)
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

void pmpd_reset(t_pmpd *x)
{
	x->nb_link = 0;
	x->nb_mass = 0;
    x->minX = -1000000;
    x->maxX = 1000000;
    x->grab = 0;
}

void pmpd_infosL(t_pmpd *x)
{
    int i;
    post("list of mass");
    post("number, Id, mobile, mass, Damping, position, speed, forces");
	for(i=0; i < x->nb_mass; i++)
    {
        post("masse %i: %s, %d, %f, %f, %f, %f, %f",i, x->mass[i].Id->s_name, x->mass[i].mobile, 1/x->mass[i].invM, x->mass[i].D2, x->mass[i].posX, x->mass[i].speedX, x->mass[i].forceX );
    }

    post("list of link");
    post("number, Id, mass1, mass2, K, D, Pow, L, Lmin, Lmax");
	for(i=0; i < x->nb_link; i++)
    {
		switch(x->link[i].lType)
		{
		case 0 :
			post("link %i: %s, %i, %i, %f, %f, %f, %f, %f, %f", i, x->link[i].Id->s_name, x->link[i].mass1->num, x->link[i].mass2->num, x->link[i].K, x->link[i].D, x->link[i].Pow, x->link[i].L, x->link[i].Lmin, x->link[i].Lmax);
			break;
		case 2 :
			post("tabLink %i: %s, %i, %i, %f, %f, %s, %f, %s, %f", i, x->link[i].Id->s_name, x->link[i].mass1->num, x->link[i].mass2->num, x->link[i].K, x->link[i].D, x->link[i].arrayK->s_name, x->link[i].K_L, x->link[i].arrayD->s_name, x->link[i].D_L);
			break;
		}
    }
}

void pmpd_bang(t_pmpd *x)
{ // this part is doing all the PM
	t_float F, L, absL, tmpX;
	t_int i;
    // post("bang");

	for (i=0; i<x->nb_mass; i++)
	// compute new masses position
		if (x->mass[i].mobile > 0) // only if mobile
		{
			x->mass[i].speedX += x->mass[i].forceX * x->mass[i].invM;
			// x->mass[i].forceX = 0;
			x->mass[i].posX += x->mass[i].speedX ;
            // x->mass[i].posX = min(x->maxX,max(x->minX,x->mass[i].posX));
            if ( (x->mass[i].posX < x->minX) || (x->mass[i].posX > x->maxX) ) 
            {
				tmpX = min(x->maxX,max(x->minX,x->mass[i].posX));
				x->mass[i].speedX -= x->mass[i].posX - tmpX;
				x->mass[i].posX = tmpX;
			}
			x->mass[i].forceX = -x->mass[i].D2 * x->mass[i].speedX;
		}

	for (i=0; i<x->nb_link; i++)
	// compute link forces
	{
		L = x->link[i].mass1->posX - x->link[i].mass2->posX;
		absL = fabs(L);
		if ( (absL >= x->link[i].Lmin) && (absL < x->link[i].Lmax)  && (L !=0))
		{
			if (x->link[i].lType == 2)
			{ // K et D viennent d'une table
				F  = x->link[i].D * tabread2(x, (L - x->link[i].distance) / x->link[i].D_L, x->link[i].arrayD);
				F += x->link[i].K * tabread2(x, L / x->link[i].K_L, x->link[i].arrayK);
			}
			else
			{			
				F  = x->link[i].D * (L - x->link[i].distance) ;
				F += x->link[i].K *  sign_ch(L) * pow_ch( absL - x->link[i].L, x->link[i].Pow);
			}
			
			x->link[i].mass1->forceX -= F;
			x->link[i].mass2->forceX += F;
		}
		x->link[i].distance=L;
	}
}

void pmpd_mass(t_pmpd *x, t_symbol *Id, t_float mobile, t_float M, t_float posX )
{ // add a mass : Id, invM speedX posX forceX

	if (M<=0) M=1;
	x->mass[x->nb_mass].Id = Id;
	x->mass[x->nb_mass].mobile = (int)mobile;
	x->mass[x->nb_mass].invM = 1/M;
	x->mass[x->nb_mass].speedX = 0;
	x->mass[x->nb_mass].posX = posX;
	x->mass[x->nb_mass].forceX = 0;
	x->mass[x->nb_mass].num = x->nb_mass;

	x->nb_mass++ ;
	x->nb_mass = min ( nb_max_mass -1, x->nb_mass );
}

void pmpd_create_link(t_pmpd *x, t_symbol *Id, int mass1, int mass2, t_float K, t_float D, t_float Pow, t_float Lmin, t_float Lmax, t_int type)
{ // create a link based on mass number

    if ((x->nb_mass>0) && (mass1 != mass2) && (mass1 >= 0) && (mass2 >= 0) && (mass1 < x->nb_mass) && (mass2 < x->nb_mass) )
    {
	    x->link[x->nb_link].lType = type;
	    x->link[x->nb_link].Id = Id;
	    x->link[x->nb_link].mass1 = &x->mass[mass1]; 
	    x->link[x->nb_link].mass2 = &x->mass[mass2];
	    x->link[x->nb_link].K = K;
	    x->link[x->nb_link].D = D;
	    x->link[x->nb_link].L = x->mass[mass1].posX - x->mass[mass2].posX;
	    x->link[x->nb_link].Pow = Pow;
	    x->link[x->nb_link].Lmin = Lmin;
	    x->link[x->nb_link].Lmax = Lmax;
	    x->link[x->nb_link].distance =  x->link[x->nb_link].L;

	    x->nb_link++ ;
	    x->nb_link = min ( nb_max_link -1, x->nb_link );
    }
}

void pmpd_link(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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
        pmpd_create_link(x, Id, mass1, mass2, K, D, Pow, Lmin, Lmax, 0);
    }
    else
    if ( ( argv[1].a_type == A_SYMBOL ) && ( argv[2].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                pmpd_create_link(x, Id, i, mass2, K, D, Pow, Lmin, Lmax, 0);
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
                pmpd_create_link(x, Id, mass1, i, K, D, Pow, Lmin, Lmax, 0);
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
						pmpd_create_link(x, Id, i, j, K, D, Pow, Lmin, Lmax, 0);
                }
            }   
        }
    }
}

void pmpd_tabLink(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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
        pmpd_create_link(x, Id, mass1, mass2, 1, 1, 1, -1000000, 1000000, 2);
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
                pmpd_create_link(x, Id, i, mass2, 1, 1, 1, -1000000, 1000000, 2);
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
                pmpd_create_link(x, Id, mass1, i, 1, 1, 1, -1000000, 1000000, 2);
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
						pmpd_create_link(x, Id, i, j, 1, 1, 1, -1000000, 1000000, 2);
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

void pmpd_setK(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_setD(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_setL(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_setLKTab(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_setLDTab(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_setLinkId(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_setMassId(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_minX(t_pmpd *x, t_float min)
{
	x->minX = min;
}

void pmpd_maxX(t_pmpd *x, t_float max)
{
	x->maxX = max;
}

void pmpd_setFixed(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_setMobile(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_forceX(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_posX(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
// displace a mass to a certain position
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].posX = atom_getfloatarg(1, argc, argv);
   	    x->mass[tmp].speedX = 0; // ??? TODO : esce la bonne chose a faire?
        x->mass[tmp].forceX = 0; // ??? TODO : esce la bonne chose a faire?
        
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].posX = atom_getfloatarg(1, argc, argv);
        	    x->mass[i].speedX = 0; // ??? TODO : esce la bonne chose a faire?
                x->mass[i].forceX = 0; // ??? TODO : esce la bonne chose a faire?

            }
        }
    }
}

void pmpd_setD2(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_setSpeedX(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_addPosX(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_setForceX(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_get(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_symbol *toget; 
    t_atom  toout[3];
    toget = atom_getsymbolarg(0, argc, argv);

    if ( (toget == gensym("massesPos")) && (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) && (i<x->nb_mass) )
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->mass[i].posX);
            outlet_anything(x->main_outlet, gensym("massesPosNo"), 2, toout);
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
                outlet_anything(x->main_outlet, gensym("massesPosId"), 2, toout);
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
            outlet_anything(x->main_outlet, gensym("massesPos"), 2, toout);
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
            outlet_anything(x->main_outlet, gensym("massesPosNameNo"), 2, toout);
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
                outlet_anything(x->main_outlet, gensym("massesPosNameId"), 2, toout);
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
            outlet_anything(x->main_outlet, gensym("massesPosName"), 2, toout);
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
            outlet_anything(x->main_outlet, gensym("massesSpeedsNo"), 2, toout);
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
                outlet_anything(x->main_outlet, gensym("massesSpeedsId"), 2, toout);
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
            outlet_anything(x->main_outlet, gensym("massesSpeeds"), 2, toout);
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
            outlet_anything(x->main_outlet, gensym("massesSpeedsNameNo"), 2, toout);
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
                outlet_anything(x->main_outlet, gensym("massesSpeedsNameId"), 2, toout);
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
            outlet_anything(x->main_outlet, gensym("massesSpeedsName"), 2, toout);
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
            outlet_anything(x->main_outlet, gensym("massesForcesNo"), 2, toout);
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
                outlet_anything(x->main_outlet, gensym("massesForcesId"), 2, toout);
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
            outlet_anything(x->main_outlet, gensym("massesForces"), 2, toout);
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
            outlet_anything(x->main_outlet, gensym("massesForcesNameNo"), 2, toout);
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
                outlet_anything(x->main_outlet, gensym("massesForcesNameId"), 2, toout);
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
            outlet_anything(x->main_outlet, gensym("massesForcesName"), 2, toout);
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
            SETFLOAT(&(toout[2]), x->link[i].mass2->posX);
            outlet_anything(x->main_outlet, gensym("linksPosNo"), 3, toout);
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
                SETFLOAT(&(toout[2]), x->link[i].mass2->posX);
                outlet_anything(x->main_outlet, gensym("linksPosId"), 3, toout);
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
            SETFLOAT(&(toout[2]), x->link[i].mass2->posX);
            outlet_anything(x->main_outlet, gensym("linksPos"), 3, toout);
        } 
    }
    else
    if ( (toget == gensym("linksPosName")) && (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) && (i<x->nb_mass) )
        {
            SETSYMBOL(&(toout[0]), x->link[i].Id);
            SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
            SETFLOAT(&(toout[2]), x->link[i].mass2->posX);
            outlet_anything(x->main_outlet, gensym("linksPosNameNo"), 3, toout);
        }
    }
    else
    if ( (toget == gensym("linksPosName")) && (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->link[i].Id)
            {
                SETSYMBOL(&(toout[0]), x->link[i].Id);
                SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
                SETFLOAT(&(toout[2]), x->link[i].mass2->posX);
                outlet_anything(x->main_outlet, gensym("linksPosNameId"), 3, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("linksPosName")) && (argc == 1) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            SETSYMBOL(&(toout[0]), x->link[i].Id);
            SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
            SETFLOAT(&(toout[2]), x->link[i].mass2->posX);
            outlet_anything(x->main_outlet, gensym("linksPosName"), 3, toout);
        } 
    }
    else
        error("not get attribute");
}

void pmpd_massesPosL(t_pmpd *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i < x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].posX);
    }
    outlet_anything(x->main_outlet, gensym("massesPosL"),x->nb_mass , pos_list);
}

void pmpd_massesForcesL(t_pmpd *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].forceX);
    }
    outlet_anything(x->main_outlet, gensym("massesForcesL"),x->nb_mass , pos_list);
}

void pmpd_massesSpeedsL(t_pmpd *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].speedX);
    }
    outlet_anything(x->main_outlet, gensym("massesSpeedsL"),x->nb_mass , pos_list);
}

void pmpd_massesPosT(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_massesSpeedsT(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_massesForcesT(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_massesPosMean(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX;
    t_int i,j;
    t_atom mean[1];

	sommeX = 0;
	j = 0;
	
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				sommeX += x->mass[i].posX;
				j++;
			}
		}
    }
	else
	{
		for (i=0; i< x->nb_mass; i++)
        {
				sommeX += x->mass[i].posX;
				j++;
		}
	}	
	
	sommeX /= j;
	
    SETFLOAT(&(mean[0]),sommeX);
    
    outlet_anything(x->main_outlet, gensym("massesPosMean"),1 , mean);
}

void pmpd_massesPosStd(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX;
    t_int i,j;
    t_float stdX;
    t_atom std_out[1];

	sommeX = 0;
	stdX = 0;
	j = 0;
	
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				sommeX += x->mass[i].posX;
				j++;
			}
		}
		sommeX /= j;
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				stdX += sqr(x->mass[i].posX-sommeX);
			}
		}		
    }
	else
	{
		for (i=0; i< x->nb_mass; i++)
        {
			sommeX += x->mass[i].posX;
			j++;
		}
		sommeX /= j;
		for (i=0; i< x->nb_mass; i++)
        {
			stdX += sqr(x->mass[i].posX-sommeX);
		}
	}	
	
	stdX = sqrt(stdX/j);

    SETFLOAT(&(std_out[0]),stdX);
    
    outlet_anything(x->main_outlet, gensym("massesPosStd"),1 , std_out);
}

void pmpd_massesForcesMean(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX;
    t_int i,j;
    t_atom mean[1];

	sommeX = 0;
	j = 0;
	
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				sommeX += x->mass[i].forceX;
				j++;
			}
		}
    }
	else
	{
		for (i=0; i< x->nb_mass; i++)
        {
				sommeX += x->mass[i].forceX;
				j++;
		}
	}	
	
	sommeX /= j;
	
    SETFLOAT(&(mean[0]),sommeX);
    
    outlet_anything(x->main_outlet, gensym("massesForcesMean"),1 , mean);
}

void pmpd_massesForcesStd(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX;
    t_int i,j;
    t_float stdX;
    t_atom std_out[1];

	sommeX = 0;
	stdX = 0;
	j = 0;
	
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				sommeX += x->mass[i].forceX;
				j++;
			}
		}
		sommeX /= j;
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				stdX += sqr(x->mass[i].forceX-sommeX);
			}
		}		
    }
	else
	{
		for (i=0; i< x->nb_mass; i++)
        {
			sommeX += x->mass[i].forceX;
			j++;
		}
		sommeX /= j;
		for (i=0; i< x->nb_mass; i++)
        {
			stdX += sqr(x->mass[i].forceX-sommeX);
		}
	}	
	
	stdX = sqrt(stdX/j);

    SETFLOAT(&(std_out[0]),stdX);
    
    outlet_anything(x->main_outlet, gensym("massesForcesStd"),1 , std_out);
}

void pmpd_massesSpeedsMean(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX;
    t_int i,j;
    t_atom mean[1];

	sommeX = 0;
	j = 0;
	
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				sommeX += x->mass[i].speedX;
				j++;
			}
		}
    }
	else
	{
		for (i=0; i< x->nb_mass; i++)
        {
				sommeX += x->mass[i].speedX;
				j++;
		}
	}	
	
	sommeX /= j;
	
    SETFLOAT(&(mean[0]),sommeX);
    
    outlet_anything(x->main_outlet, gensym("massesSpeedsMean"),1 , mean);
}

void pmpd_massesSpeedsStd(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX;
    t_int i,j;
    t_float stdX;
    t_atom std_out[1];

	sommeX = 0;
	stdX = 0;
	j = 0;
	
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				sommeX += x->mass[i].speedX;
				j++;
			}
		}
		sommeX /= j;
		for (i=0; i< x->nb_mass; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{ 
				stdX += sqr(x->mass[i].speedX-sommeX);
			}
		}		
    }
	else
	{
		for (i=0; i< x->nb_mass; i++)
        {
			sommeX += x->mass[i].speedX;
			j++;
		}
		sommeX /= j;
		for (i=0; i< x->nb_mass; i++)
        {
			stdX += sqr(x->mass[i].speedX-sommeX);
		}
	}	
	
	stdX = sqrt(stdX/j);

    SETFLOAT(&(std_out[0]),stdX);
    
    outlet_anything(x->main_outlet, gensym("massesSpeedsStd"),1 , std_out);
}

// --------------------------------------------

void pmpd_linksPosL(t_pmpd *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),(x->link[i].mass1->posX + x->link[i].mass2->posX)/2);
    }
    outlet_anything(x->main_outlet, gensym("linksPosXL"),x->nb_link , pos_list);
}

void pmpd_linksLengthL(t_pmpd *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),x->link[i].mass2->posX - x->link[i].mass1->posX);
    }
    outlet_anything(x->main_outlet, gensym("linksLengthXL"),x->nb_link , pos_list);
}

void pmpd_linksPosSpeedL(t_pmpd *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),(x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2);
    }
    outlet_anything(x->main_outlet, gensym("linksPosSpeedXL"),x->nb_link , pos_list);
}

void pmpd_linksLengthSpeedL(t_pmpd *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),x->link[i].mass2->speedX - x->link[i].mass1->speedX);
    }
    outlet_anything(x->main_outlet, gensym("linksLengthSpeedXL"),x->nb_link , pos_list);
}

void pmpd_linksPosT(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_linksLengthT(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_linksPosSpeedT(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_linksLengthSpeedT(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

//----------------------------------------------

void pmpd_grabMass(t_pmpd *x, t_float posX, t_float grab)
{
	t_float dist, tmp;
	t_int i;
	
	if (grab == 0)
		x->grab=0;
	if ((x->grab == 0)&(grab == 1)&(x->nb_mass > 0))
	{
		x->grab=1;
		x->grab_nb= 0;
		dist = x->mass[0].posX - posX;
		for (i=1; i<x->nb_mass; i++)
		{
			tmp = x->mass[i].posX - posX;
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
	}
}

void *pmpd_new()
{
	t_pmpd *x = (t_pmpd *)pd_new(pmpd_class);

	pmpd_reset(x);
	
	x->main_outlet=outlet_new(&x->x_obj, 0);
	// x->info_outlet=outlet_new(&x->x_obj, 0); // TODO

	return (void *)x;
}

void pmpd_setup(void) 
{
 pmpd_class = class_new(gensym("pmpd"),
        (t_newmethod)pmpd_new,
        0, sizeof(t_pmpd),CLASS_DEFAULT, 0);

	class_addbang(pmpd_class, pmpd_bang);
	class_addmethod(pmpd_class, (t_method)pmpd_reset,           gensym("reset"), 0);
	class_addmethod(pmpd_class, (t_method)pmpd_infosL,          gensym("infosL"), 0);
	class_addmethod(pmpd_class, (t_method)pmpd_infosL,          gensym("print"), 0);
	class_addmethod(pmpd_class, (t_method)pmpd_mass,            gensym("mass"), A_DEFSYMBOL, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_link,            gensym("link"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_tabLink,         gensym("tabLink"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_setK,            gensym("setK"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_setD,            gensym("setD"), A_GIMME, 0);	
	class_addmethod(pmpd_class, (t_method)pmpd_setL,            gensym("setL"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_setLKTab,        gensym("setLKTab"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_setLDTab,        gensym("setLDTab"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_setLinkId,       gensym("setLinkId"), A_GIMME, 0);

	class_addmethod(pmpd_class, (t_method)pmpd_setMassId,       gensym("setMassId"), A_GIMME, 0);	
	class_addmethod(pmpd_class, (t_method)pmpd_posX,            gensym("pos"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_posX,            gensym("posX"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_forceX,          gensym("force"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_forceX,          gensym("forceX"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_minX,            gensym("Xmin"), A_DEFFLOAT, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_minX,            gensym("min"), A_DEFFLOAT, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_maxX,            gensym("Xmax"), A_DEFFLOAT, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_maxX,            gensym("max"), A_DEFFLOAT, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_setFixed,        gensym("setFixed"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_setMobile,       gensym("setMobile"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_setD2,           gensym("setDEnv"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_setSpeedX,       gensym("setSpeed"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_setSpeedX,       gensym("setSpeedX"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_setForceX,       gensym("setForce"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_setForceX,       gensym("setForceX"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_addPosX,         gensym("addPos"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_addPosX,         gensym("addPosX"), A_GIMME, 0);
	
	class_addmethod(pmpd_class, (t_method)pmpd_get,             gensym("get"), A_GIMME, 0);
	
	class_addmethod(pmpd_class, (t_method)pmpd_massesPosL,      gensym("massesPosL"), 0);
	class_addmethod(pmpd_class, (t_method)pmpd_massesSpeedsL,   gensym("massesSpeedsL"), 0);
	class_addmethod(pmpd_class, (t_method)pmpd_massesForcesL,   gensym("massesForcesL"), 0);
	class_addmethod(pmpd_class, (t_method)pmpd_massesPosT,      gensym("massesPosT"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_massesSpeedsT,   gensym("massesSpeedsT"),A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_massesForcesT,   gensym("massesForcesT"),A_GIMME, 0);
	
	class_addmethod(pmpd_class, (t_method)pmpd_massesPosL,      gensym("massesPosXL"), 0);
	class_addmethod(pmpd_class, (t_method)pmpd_massesSpeedsL,   gensym("massesSpeedsXL"), 0);
	class_addmethod(pmpd_class, (t_method)pmpd_massesForcesL,   gensym("massesForcesXL"), 0);
	class_addmethod(pmpd_class, (t_method)pmpd_massesPosT,      gensym("massesPosXT"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_massesSpeedsT,   gensym("massesSpeedsXT"),A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_massesForcesT,   gensym("massesForcesXT"),A_GIMME, 0);
	
	class_addmethod(pmpd_class, (t_method)pmpd_massesPosMean,   gensym("massesPosMean"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_massesPosStd,    gensym("massesPosStd"),A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_massesForcesMean,gensym("massesForcesMean"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_massesForcesStd, gensym("massesForcesStd"),A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_massesSpeedsMean,gensym("massesSpeedsMean"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_massesSpeedsStd, gensym("massesSpeedsStd"),A_GIMME, 0);
	
	class_addmethod(pmpd_class, (t_method)pmpd_linksPosL,	   		gensym("linksPosXL"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksLengthL,	   	gensym("linksLengthXL"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksPosSpeedL,		gensym("linksPosSpeedXL"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksLengthSpeedL,	gensym("linksLengthSpeedXL"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksPosT,	   		gensym("linksPosXT"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksLengthT,	   	gensym("linksLengthXT"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksPosSpeedT,		gensym("linksPosSpeedXT"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksLengthSpeedT,	gensym("linksLengthSpeedXT"), A_GIMME, 0);

	class_addmethod(pmpd_class, (t_method)pmpd_linksPosL,	   		gensym("linksPosL"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksLengthL,	   	gensym("linksLengthL"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksPosSpeedL,		gensym("linksPosSpeedL"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksLengthSpeedL,	gensym("linksLengthSpeedL"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksPosT,	   		gensym("linksPosT"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksLengthT,	   	gensym("linksLengthT"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksPosSpeedT,		gensym("linksPosSpeedT"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksLengthSpeedT,	gensym("linksLengthSpeedT"), A_GIMME, 0);
	
/*	class_addmethod(pmpd_class, (t_method)pmpd_linksPosMean,	   		gensym("linksPosMean"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksLengthMean,	   		gensym("linksLengthMean"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksPosSpeedMean,		gensym("linksPosSpeedMean"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksLengthSpeedMean,	gensym("linksLengthSpeedMean"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksPosStd,		   		gensym("linksPosStd"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksLengthStd,	   		gensym("linksLengthStd"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksPosSpeedStd,		gensym("linksPosSpeedStd"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_linksLengthSpeedStd0,	gensym("linksLengthSpeedStd"), A_GIMME, 0);
*/
	
	class_addmethod(pmpd_class, (t_method)pmpd_grabMass,        gensym("grabMass"), A_DEFFLOAT, A_DEFFLOAT, 0);
}

