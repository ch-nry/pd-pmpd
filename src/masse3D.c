#include "m_pd.h"
#include "math.h"

static t_class *masse3D_class;

extern t_float max(t_float, t_float);
extern t_float min(t_float, t_float);


typedef struct _masse3D {
  t_object  x_obj;
  t_float posX_old_1, posX_old_2, posY_old_1, posY_old_2, posZ_old_1, posZ_old_2;
  t_float Xinit, Yinit, Zinit, forceX, forceY, forceZ, VX, VY, VZ, dX, dY, dZ;
  t_float masse3D, seuil, onoff, damp;
  t_atom  pos_new[3], vitesse[4], force[4];
  t_float minX, maxX, minY, maxY, minZ, maxZ;
  t_outlet *position3D_new, *vitesse_out, *force_out;
  t_symbol *x_sym; // receive
  unsigned int x_state; // random
  t_float x_f; // random
} t_masse3D;

void masse3D_on(t_masse3D *x)
{
  x->onoff = 1;
}

void masse3D_off(t_masse3D *x)
{
  x->onoff = 0;
}

void masse3D_minX(t_masse3D *x, t_floatarg f1)
{
  x->minX = f1;
}

void masse3D_maxX(t_masse3D *x, t_floatarg f1)
{
  x->maxX = f1;
}

void masse3D_minY(t_masse3D *x, t_floatarg f1)
{
  x->minY = f1;
}

void masse3D_maxY(t_masse3D *x, t_floatarg f1)
{
  x->maxY = f1;
}

void masse3D_minZ(t_masse3D *x, t_floatarg f1)
{
  x->minZ = f1;
}

void masse3D_maxZ(t_masse3D *x, t_floatarg f1)
{
  x->maxZ = f1;
}

void masse3D_seuil(t_masse3D *x, t_floatarg f1)
{
  x->seuil = f1;
}

void masse3D_damp(t_masse3D *x, t_floatarg f1)
{
  x->damp = f1;
}

void masse3D_loadbang(t_masse3D *x, t_float posZ)
{
  outlet_anything(x->position3D_new, gensym("position3D"), 3, x->pos_new);
}

void masse3D_setX(t_masse3D *x, t_float posX)
{
  
  x->posX_old_2 = posX;
  x->posX_old_1 = posX;
  x->forceX=0;

  SETFLOAT(&(x->pos_new[0]), posX);

  outlet_anything(x->position3D_new, gensym("position3D"), 3, x->pos_new);

}

void masse3D_setY(t_masse3D *x, t_float posY)
{
  x->posY_old_2 = posY;
  x->posY_old_1 = posY;
  x->forceY=0;

  SETFLOAT(&(x->pos_new[1]), posY);

  outlet_anything(x->position3D_new, gensym("position3D"), 3, x->pos_new);

}

void masse3D_setZ(t_masse3D *x, t_float posZ)
{
  x->posZ_old_2 = posZ;
  x->posZ_old_1 = posZ;
  x->forceZ=0;

  SETFLOAT(&(x->pos_new[2]), posZ);

  outlet_anything(x->position3D_new, gensym("position3D"), 3, x->pos_new);

}

void masse3D_setXYZ(t_masse3D *x, t_float posX, t_float posY, t_float posZ)
{
  
  x->posX_old_2 = posX;
  x->posX_old_1 = posX;
  x->forceX=0;

  x->posY_old_2 = posY;
  x->posY_old_1 = posY;
  x->forceY=0;

  x->posZ_old_2 = posZ;
  x->posZ_old_1 = posZ;
  x->forceZ=0;

  SETFLOAT(&(x->pos_new[0]), posX);
  SETFLOAT(&(x->pos_new[1]), posY);
  SETFLOAT(&(x->pos_new[2]), posZ);

  outlet_anything(x->position3D_new, gensym("position3D"), 3, x->pos_new);
}

void masse3D_set_masse3D(t_masse3D *x, t_float mass)
{
  x->masse3D=mass;
}


void masse3D_force(t_masse3D *x, t_floatarg f1, t_floatarg f2, t_floatarg f3)
{
  x->forceX = x->forceX+f1;
  x->forceY = x->forceY+f2;
  x->forceZ = x->forceZ+f3;
}

void masse3D_dXYZ(t_masse3D *x, t_floatarg f1, t_floatarg f2, t_floatarg f3)
{
  x->dX += f1;
  x->dY += f2;
  x->dZ += f3;
}

void masse3D_dX(t_masse3D *x, t_floatarg f1 )
{
  x->dX += f1;
}

void masse3D_dY(t_masse3D *x, t_floatarg f1 )
{
  x->dY += f1;
}

void masse3D_dZ(t_masse3D *x, t_floatarg f1 )
{
  x->dZ += f1;
}

void masse3D_bang(t_masse3D *x)
{
  t_float posX_new, posY_new, posZ_new, vX=1, vY=1, vZ=1;
 if (x->onoff != 0)
 {

  if (x->seuil > 0)
  {
  if (x->posZ_old_1 == x->minZ) // si on est en dehors de la structure -> frottement sec sur les bords
  {
	  if (sqrt(x->forceX*x->forceX + x->forceY*x->forceY)<=(x->seuil * -(x->forceZ)))
	  {
		  vX = 0;		// on est a l'interieur du cone de frotement,  
		  vY = 0;		// on est a l'interieur du cone de frotement,  
	  }
  }

  if (x->posZ_old_1 == x->maxZ) // si on est en dehors de la structure -> frottement sec sur les bords
  {
	  if (sqrt(x->forceX*x->forceX + x->forceY*x->forceY)<=(x->seuil * (x->forceZ)))
 	  {
		  vX = 0;		// on est a l'interieur du cone de frotement,  
 		  vY = 0;		// on est a l'interieur du cone de frotement,  
 	  }
  }
 
  if (x->posY_old_1 == x->minY) // si on est en dehors de la structure -> frottement sec sur les bords
  {
	  if (sqrt(x->forceX*x->forceX + x->forceZ*x->forceZ)<=(x->seuil * -(x->forceY)))
	  {
		  vX = 0;		// on est a l'interieur du cone de frotement,  
 	      vZ = 0;		// on est a l'interieur du cone de frotement,  
 	  }
  }

  if (x->posY_old_1 == x->maxY) // si on est en dehors de la structure -> frottement sec sur les bords
  {
	  if (sqrt(x->forceX*x->forceX + x->forceZ*x->forceZ)<=(x->seuil * (x->forceY)))
	  {
		  vX = 0;		// on est a l'interieur du cone de frotement,  
 		  vZ = 0;		// on est a l'interieur du cone de frotement,  
  	  }
  }

  if (x->posX_old_1 == x->minX) // si on est en dehors de la structure -> frottement sec sur les bords
  {
	  if (sqrt(x->forceY*x->forceY + x->forceZ*x->forceZ)<=(x->seuil * -(x->forceX)))
	  {
		  vY = 0;		// on est a l'interieur du cone de frotement,  
 		  vZ = 0;		// on est a l'interieur du cone de frotement,  
  	  }
  }

  if (x->posX_old_1 == x->maxX) // si on est en dehors de la structure -> frottement sec sur les bords
  {
	  if (sqrt(x->forceY*x->forceY + x->forceZ*x->forceZ)<=(x->seuil * (x->forceX)))
	  {
		  vY = 0;		// on est a l'interieur du cone de frotement,  
  		  vZ = 0;		// on est a l'interieur du cone de frotement,  
 	  }
  }
  }

	x->forceX += x->damp * ((x->posX_old_2)-(x->posX_old_1));
	x->forceY += x->damp * ((x->posY_old_2)-(x->posY_old_1)); // damping
	x->forceZ += x->damp * ((x->posZ_old_2)-(x->posZ_old_1)); // damping

  if (!(x->masse3D == 0))
  {
  posX_new = x->forceX/x->masse3D + 2*x->posX_old_1 - x->posX_old_2;
  posY_new = x->forceY/x->masse3D + 2*x->posY_old_1 - x->posY_old_2;
  posZ_new = x->forceZ/x->masse3D + 2*x->posZ_old_1 - x->posZ_old_2;
  }
  else 
  {
  posX_new = x->posX_old_1;
  posY_new = x->posY_old_1;
  posZ_new = x->posY_old_1;
  }

 
	  if (vX==0)
	    posX_new = x->posX_old_1; // on n'a pas de mv qd on est a l'interieur du cone de frotement
	  if (vY==0)
	    posY_new = x->posY_old_1;
	  if (vZ==0)
	    posZ_new = x->posZ_old_1;

	  posX_new = max(min(x->maxX, posX_new), x->minX);
	  posY_new = max(min(x->maxY, posY_new), x->minY);
	  posZ_new = max(min(x->maxZ, posZ_new), x->minZ);


  posX_new += x->dX;
  posY_new += x->dY;
  posZ_new += x->dZ;

  x->posX_old_1 += x->dX;
  x->posY_old_1 += x->dY;
  x->posZ_old_1 += x->dZ;
	
  SETFLOAT(&(x->pos_new[0]), posX_new );
  SETFLOAT(&(x->pos_new[1]), posY_new );
  SETFLOAT(&(x->pos_new[2]), posZ_new );

  x->posX_old_2 = x->posX_old_1;
  x->posX_old_1 = posX_new;

  x->posY_old_2 = x->posY_old_1;
  x->posY_old_1 = posY_new;

  x->posZ_old_2 = x->posZ_old_1;
  x->posZ_old_1 = posZ_new;

  SETFLOAT(&(x->force[0]), x->forceX );
  SETFLOAT(&(x->force[1]), x->forceY );
  SETFLOAT(&(x->force[2]), x->forceZ );
  SETFLOAT(&(x->force[3]), sqrt( (x->forceX * x->forceX) + (x->forceY * x->forceY) + (x->forceZ * x->forceZ) ));
 
  x->forceX=0;
  x->forceY=0;
  x->forceZ=0;

  x->dX=0;
  x->dY=0;
  x->dZ=0;

  x->VX =  x->posX_old_1 -  x->posX_old_2;
  x->VY =  x->posY_old_1 -  x->posY_old_2;
  x->VZ =  x->posZ_old_1 -  x->posZ_old_2;
  
  SETFLOAT(&(x->vitesse[0]), x->VX );
  SETFLOAT(&(x->vitesse[1]), x->VY );
  SETFLOAT(&(x->vitesse[2]), x->VZ );
  SETFLOAT(&(x->vitesse[3]), sqrt( (x->VX * x->VX) + (x->VY * x->VY) + (x->VZ * x->VZ) ));
 
  outlet_anything(x->vitesse_out, gensym("velocity3D"), 4, x->vitesse);
  outlet_anything(x->force_out, gensym("force3D"), 4, x->force);
  outlet_anything(x->position3D_new, gensym("position3D"), 3, x->pos_new);
 }
}

void masse3D_reset(t_masse3D *x)
{
  
  x->posX_old_2 = x->Xinit;
  x->posX_old_1 = x->Xinit;
  x->forceX=0;

  x->posY_old_2 = x->Yinit;
  x->posY_old_1 = x->Yinit;
  x->forceY=0;

  x->posZ_old_2 = x->Zinit;
  x->posZ_old_1 = x->Zinit;
  x->forceZ=0;

  x->VX = 0;
  x->VY = 0;
  x->VZ = 0;

  x->dX=0;
  x->dY=0;
  x->dZ=0;

  x->seuil=0;

  x->onoff = 1;

  SETFLOAT(&(x->pos_new[0]), x->Xinit );
  SETFLOAT(&(x->pos_new[1]), x->Yinit );
  SETFLOAT(&(x->pos_new[2]), x->Zinit );

  SETFLOAT(&(x->force[0]), 0 );
  SETFLOAT(&(x->force[1]), 0 );
  SETFLOAT(&(x->force[2]), 0 );
  SETFLOAT(&(x->force[3]), 0 );
 
  SETFLOAT(&(x->vitesse[0]), 0 );
  SETFLOAT(&(x->vitesse[1]), 0 );
  SETFLOAT(&(x->vitesse[2]), 0 );
  SETFLOAT(&(x->vitesse[3]), 0 );

  outlet_anything(x->vitesse_out, gensym("velocity3D"), 4, x->vitesse);
  outlet_anything(x->force_out, gensym("force3D"), 4, x->force); 
  outlet_anything(x->position3D_new, gensym("position3D"), 3, x->pos_new);

}


void masse3D_resetf(t_masse3D *x)
{
  x->forceX=0;
  x->forceY=0;
  x->forceZ=0;

  x->dX=0;
  x->dY=0;
  x->dZ=0;
}

static int makeseed3D(void)
{
    static unsigned int random_nextseed = 1489853723;
    random_nextseed = random_nextseed * 435898247 + 938284287;
    return (random_nextseed & 0x7fffffff);
}

static float random_bang3D(t_masse3D *x)
{
    int nval;
    int range = 2000000;
	float rnd;
	unsigned int randval = x->x_state;
	x->x_state = randval = randval * 472940017 + 832416023;
    nval = ((double)range) * ((double)randval)
    	* (1./4294967296.);
    if (nval >= range) nval = range-1;

	rnd=nval;

	rnd-=1000000;
	rnd=rnd/1000000.;	//pour mettre entre -1 et 1;
    return (rnd);
}


void masse3D_inter_ambient(t_masse3D *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float tmp;

	if (argc == 17) 
		// 0 : FX
		// 1 : FY
		// 2 : FZ
		// 3 : RndX
		// 4 : RndY
		// 5 : RndZ
		// 6 : D2
		// 7 : rien
		// 8 : Xmin
		// 9 : Xmax
		// 10 : Ymin
		// 11 : Ymax
		// 12 : Zmin
		// 13 : Zmax
		// 14 : dX
		// 15 : dY
		// 16 : dZ
	{
		if (x->posX_old_1 > atom_getfloatarg(8, argc, argv))
		{
			if (x->posX_old_1 < atom_getfloatarg(9, argc, argv))
			{
				if (x->posY_old_1 > atom_getfloatarg(10, argc, argv))
				{
					if (x->posY_old_1 < atom_getfloatarg(11, argc, argv))
					{
					if (x->posZ_old_1 > atom_getfloatarg(12, argc, argv))
					{
					if (x->posZ_old_1 < atom_getfloatarg(13, argc, argv))
					{
						x->forceX += atom_getfloatarg(0, argc, argv);
						x->forceY += atom_getfloatarg(1, argc, argv); // constant
						x->forceZ += atom_getfloatarg(2, argc, argv); // constant

						x->forceX += random_bang3D(x)*atom_getfloatarg(3, argc, argv);
						x->forceY += random_bang3D(x)*atom_getfloatarg(4, argc, argv); // random
						x->forceZ += random_bang3D(x)*atom_getfloatarg(5, argc, argv); // random
	
						tmp = atom_getfloatarg(6, argc, argv);
						if (tmp != 0)
						{
							x->forceX += tmp * ((x->posX_old_2)-(x->posX_old_1));
							x->forceY += tmp * ((x->posY_old_2)-(x->posY_old_1)); // damping
							x->forceZ += tmp * ((x->posZ_old_2)-(x->posZ_old_1)); // damping
						}

						x->dX += atom_getfloatarg(14, argc, argv);
						x->dY += atom_getfloatarg(15, argc, argv); // constant
						x->dZ += atom_getfloatarg(16, argc, argv); // constant
					}
					}
					}
				}
			}
		}
	}
	else
	{
		error("bad ambient interraction message");
	}
}

void masse3D_inter_plane(t_masse3D *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float a, b, c, d, profondeur, distance, tmp, profondeur_old;

	if (argc == 12) 
		// 0 : Xvector
		// 1 : Yvector
		// 2 : Zvector
		// 3 : Xcenter
		// 4 : Ycenter
		// 5 : Zcenter
		// 6 : FNCt
		// 7 : KN
		// 8 : damping de liaison (profondeur)
		// 9 : Profondeur maximum
		// 10 : deplacement normal X
		// 11 : deplacement proportionel a P

	{

// ax+by+cz-d=0
// a = Xvector / |V|
// b = Yvector ...
// d est tel que aXcenter +bYcenter + cYcenter = d

		a = atom_getfloatarg(0, argc, argv);
		b = atom_getfloatarg(1, argc, argv);
		c = atom_getfloatarg(2, argc, argv);

		tmp = sqrt (a*a + b*b + c*c);
	if (tmp != 0)
	{
		a /= tmp;
		b /= tmp;
		c /= tmp;

	}
	else
	{
		a=1;
		b=0;
		c=0;
	}

		d = a * atom_getfloatarg(3, argc, argv) + b * atom_getfloatarg(4, argc, argv) + c * atom_getfloatarg(5, argc, argv);
//C a optimiser : envoyer  directement les coef directeur et l'offset
//C a faire pour les autres obj aussi
		
		profondeur = a * x->posX_old_1 + b * x->posY_old_1 + c * x->posZ_old_1 - d;

		if ( (profondeur < 0) & (profondeur > -atom_getfloatarg(9, argc, argv)) )
		{

			tmp = atom_getfloatarg(6, argc, argv); // force normal constante

			x->forceX += tmp * a;
			x->forceY += tmp * b;
			x->forceZ += tmp * c;
	
			tmp = atom_getfloatarg(7, argc, argv); // force normal proportionelle a la profondeur
			tmp *= profondeur;
			x->forceX -= tmp * a;
			x->forceY -= tmp * b;
			x->forceZ -= tmp * c;

			tmp = atom_getfloatarg(8, argc, argv); // force normal proportionelle a la profondeur

			profondeur_old = a * x->posX_old_2 + b * x->posY_old_2 + c * x->posZ_old_2 - d;		
			tmp *= (profondeur - profondeur_old);
			x->forceX -= tmp * a;
			x->forceY -= tmp * b;
			x->forceZ -= tmp * c;

			
			tmp = atom_getfloatarg(10, argc, argv); // deplacement normal constant

			x->dX += tmp * a;
			x->dY += tmp * b;
			x->dZ += tmp * c;

			tmp = atom_getfloatarg(11, argc, argv); // deplacement normal proportionel
			tmp *= profondeur;

			x->dX -= tmp * a;
			x->dY -= tmp * b;
			x->dZ -= tmp * c;
		}

	}	
	else
	{
		error("bad plane interraction message");
	}
}


void masse3D_inter_sphere(t_masse3D *x, t_symbol *s, int argc, t_atom *argv)
{
t_float posx1, posy1, posz1, Nx, Ny, Nz, dx, dy, dz, distance, Dmax, tmp;
t_float deltaX_old, deltaY_old, deltaZ_old, distance_old ;

	if (argc == 17) 
		// 0 : Xcentre
		// 1 : Ycendre
		// 2 : Zcentre
		// 3 : Rmin
		// 4 : Rmax
		// 5 : F normal
		// 6 : K normal
		// 7 : F normal proportionel a 1/R
		// 8 : Damp de liason normal 
		// 9 : deplacement N Ct
		// 10 : position ancienne de l'interacteur en X
		// 11 : position abcienne de l'interacteur en Y
		// 12 : position abcienne de l'interacteur en Z
		// 13 : d dormal proportionel a R
		// 14 : force normal proportionel a 1/R2
		// 15 : d dormal proportionel a 1/R
		// 16 : d dormal proportionel a 1/R*R

	{
		posx1 = atom_getfloatarg(0, argc, argv);
		posy1 = atom_getfloatarg(1, argc, argv);
		posz1 = atom_getfloatarg(2, argc, argv);
		Nx = (x->posX_old_1)-posx1;					// vecteur deplacement X
		Ny = (x->posY_old_1)-posy1;					// vecteur deplacement Y
		Nz = (x->posZ_old_1)-posz1;					// vecteur deplacement Y

		distance = sqrt((Nx * Nx)+(Ny * Ny)+(Nz * Nz));		// distance entre le centre de l'interaction, et le pts

		Nx = Nx/distance;							// composante X de la normal (normalisé)
		Ny = Ny/distance;							// composante Y de la normal.
		Nz = Nz/distance;							// composante Y de la normal.

		Dmax= atom_getfloatarg(4, argc, argv);		// distance max de l'interaction
		if ( (distance > atom_getfloatarg(3, argc, argv)) & (distance < Dmax) )
		{
			tmp = atom_getfloatarg(5, argc, argv); // force constante normal
			x->forceX += tmp * Nx;
			x->forceY += tmp * Ny;
			x->forceZ += tmp * Nz;

			tmp = atom_getfloatarg(6, argc, argv); // force variable (K) normal
			tmp *= ( Dmax-distance );
			x->forceX += tmp * Nx ;
			x->forceY += tmp * Ny ;
			x->forceZ += tmp * Nz ;

		    tmp = atom_getfloatarg(7, argc, argv); // force normal proportionel a 1/r
			if ( (distance != 0) & (tmp != 0) )
			{
				tmp /= distance;
				x->forceX += tmp * Nx;
				x->forceY += tmp * Ny;
				x->forceZ += tmp * Nz ;
			}

			tmp = atom_getfloatarg(8, argc, argv); // damping2 normal
			tmp *= ( x->VX * Nx + x->VY * Ny + x->VZ * Nz );
			x->forceX -= tmp * Nx ;
			x->forceY -= tmp * Ny ;
			x->forceZ -= tmp * Nz ;

			tmp = atom_getfloatarg(9, argc, argv); // d normal
			x->dX += tmp * Nx ;
			x->dY += tmp * Ny ;
			x->dZ += tmp * Nz ;

			tmp = atom_getfloatarg(13, argc, argv); // force normal proportionel a 1/r2
			if ( (distance != 0) & (tmp != 0) )
			{
				tmp /= (distance * distance);
				x->forceX += tmp * Nx ;
				x->forceY += tmp * Ny ;
				x->forceZ += tmp * Nz ;
			}

			tmp = atom_getfloatarg(14, argc, argv); // deplacement variable (K) normal
			tmp *= ( Dmax-distance );
			x->dX += tmp * Nx ;
			x->dY += tmp * Ny ;
			x->dZ += tmp * Nz ;

			tmp = atom_getfloatarg(15, argc, argv); // deplacement normal proportionel a 1/r
			if ( (distance != 0) & (tmp != 0) )
			{
				tmp /= distance;
			    x->dX += tmp * Nx ;
			    x->dY += tmp * Ny ;
			    x->dZ += tmp * Nz ;
			}

			tmp = atom_getfloatarg(16, argc, argv); // deplacement normal proportionel a 1/r2
			if ( (distance != 0) & (tmp != 0) )
			{
				tmp /= (distance * distance);
			    x->dX += tmp * Nx;
			    x->dY += tmp * Ny;
			    x->dZ += tmp * Nz;
			}

		}
	}
	else
	{
		error("bad interact_3D_sphere message");
	}
}


void masse3D_inter_circle(t_masse3D *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float a, b, c, d, profondeur, distance, tmp, profondeur_old, rayon, rayon_old;

	if (argc == 14) 
		// 0 : Xvector
		// 1 : Yvector
		// 2 : Zvector
		// 3 : Xcenter
		// 4 : Ycenter
		// 5 : Zcenter
		// 6 : Rmin
		// 7 : RMax
		// 8 : FNCt
		// 9 : KN
		// 10 : damping de liaison (profondeur)
		// 11 : Profondeur maximum
		// 12 : dN
		// 13 : dKN

	{
// ax+by+cz-d=0
// a = Xvector / |V|
// b = Yvector ...
// d est tel que aXcenter +bYcenter + cYcenter = d

		a = atom_getfloatarg(0, argc, argv);
		b = atom_getfloatarg(1, argc, argv);
		c = atom_getfloatarg(2, argc, argv);

		tmp = sqrt (a*a + b*b + c*c);
	if (tmp != 0)
	{
		a /= tmp;
		b /= tmp;
		c /= tmp;
	}
	else
	{
		a=1;
		b=0;
		c=0;
	}

		d = a * atom_getfloatarg(3, argc, argv) + b * atom_getfloatarg(4, argc, argv) + c * atom_getfloatarg(5, argc, argv);

		profondeur = a * x->posX_old_1 + b * x->posY_old_1 + c * x->posZ_old_1 - d;

		rayon = sqrt ( pow(x->posX_old_1-atom_getfloatarg(3, argc, argv), 2) +pow(x->posY_old_1-atom_getfloatarg(4, argc, argv) , 2)  + pow(x->posZ_old_1 - atom_getfloatarg(5, argc, argv) , 2) - profondeur*profondeur );

		if ( (profondeur < 0) & (profondeur > - atom_getfloatarg(11, argc, argv)) & (rayon > atom_getfloatarg(6, argc, argv)) & (rayon < atom_getfloatarg(7, argc, argv)))
		{

			tmp = atom_getfloatarg(8, argc, argv); // force normal constante

			x->forceX += tmp * a;
			x->forceY += tmp * b;
			x->forceZ += tmp * c;
	
			tmp = atom_getfloatarg(9, argc, argv); // force normal proportionelle a la profondeur
			tmp *= profondeur;
			x->forceX -= tmp * a;
			x->forceY -= tmp * b;
			x->forceZ -= tmp * c;

			tmp = atom_getfloatarg(10, argc, argv); // force normal proportionelle a la profondeur

			profondeur_old = a * x->posX_old_2 + b * x->posY_old_2 + c * x->posZ_old_2 - d;		
			tmp *= (profondeur - profondeur_old);

			x->forceX -= tmp * a;
			x->forceY -= tmp * b;
			x->forceZ -= tmp * c;

			tmp = atom_getfloatarg(12, argc, argv); // deplacement normal constante
			x->dX += tmp * a;
			x->dY += tmp * b;
			x->dZ += tmp * c;
	
			tmp = atom_getfloatarg(13, argc, argv); // deplacement normal proportionelle a la profondeur
			tmp *= profondeur;
			x->dX -= tmp * a;
			x->dY -= tmp * b;
			x->dZ -= tmp * c;
		}
	}	
	else
	{
		error("bad circle interraction message");
	}
}


void masse3D_inter_cylinder(t_masse3D *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float a, b, c, d, profondeur, profondeur_old, distance, tmp, rayon_old, rayon;
	t_float Xb, Yb, Zb, Ta, Tb, Tc, Xb_old, Yb_old, Zb_old;

	if (argc == 21) 
		// 0 : Xvector
		// 1 : Yvector
		// 2 : Zvector
		// 3 : Xcenter
		// 4 : Ycenter
		// 5 : Zcenter
		// 6 : Rmin
		// 7 : Rmax
		// 8 : FNCt
		// 9 : KN
		// 10 : damping de liaison (rayon)
		// 11 : FN 1/R
		// 12 : FN 1/R2
		// 13 : Pmin
		// 14 : Pmax
		// 15 : FTct
		// 16 : KT
		// 17 : dNct
		// 18 : dTct
		// 19 : dKN
		// 20 : dKT

	{

// ax+by+cz-d=0
// a = Xvector / |V|
// b = Yvector ...
// d est tel que aXcenter +bYcenter + cYcenter = d

		a = atom_getfloatarg(0, argc, argv);
		b = atom_getfloatarg(1, argc, argv);
		c = atom_getfloatarg(2, argc, argv);

		tmp = sqrt (a*a + b*b + c*c);
	if (tmp != 0)
	{
		a /= tmp;
		b /= tmp;
		c /= tmp;
	}
	else
	{
		a=1;
		b=0;
		c=0;
	}

		d = a * atom_getfloatarg(3, argc, argv) + b * atom_getfloatarg(4, argc, argv) + c * atom_getfloatarg(5, argc, argv);

		profondeur = a * x->posX_old_1 + b * x->posY_old_1 + c * x->posZ_old_1 - d;

		Xb = x->posX_old_1 - atom_getfloatarg(3, argc, argv) - profondeur * a;
		Yb = x->posY_old_1 - atom_getfloatarg(4, argc, argv) - profondeur * b;
		Zb = x->posZ_old_1 - atom_getfloatarg(5, argc, argv) - profondeur * c;

		rayon = sqrt ( pow(Xb, 2) + pow(Yb, 2)  + pow(Zb, 2) );

		if (rayon != 0)
		{
			Xb /= rayon;  // normalisation
			Yb /= rayon;
			Zb /= rayon;
		}
		else
		{
			Xb = 0;  // normalisation
			Yb = 0;
			Zb = 0;
		}


		Ta = b*Zb - c*Yb; // vecteur tengentiel = vecteur vectoriel rayon
		Tb = c*Xb - a*Zb;
		Tc = a*Yb - b*Xb;
		
		if ( (profondeur < atom_getfloatarg(14, argc, argv)) & (profondeur > atom_getfloatarg(13, argc, argv)) & (rayon < atom_getfloatarg(7, argc, argv)) & (rayon > atom_getfloatarg(6, argc, argv)) )
		{

			tmp = atom_getfloatarg(8, argc, argv); // force normal constante

			x->forceX += tmp * Xb;
			x->forceY += tmp * Yb;
			x->forceZ += tmp * Zb;
	
			tmp = atom_getfloatarg(9, argc, argv); // rigidité normal proportionelle
			tmp *= ( atom_getfloatarg(7, argc, argv) - rayon ) ;
			x->forceX += tmp * Xb;
			x->forceY += tmp * Yb;
			x->forceZ += tmp * Zb;
						
			tmp = atom_getfloatarg(10, argc, argv); // damping normal proportionelle a la profondeur

			profondeur_old = a * x->posX_old_2 + b * x->posY_old_2 + c * x->posZ_old_2 - d;

			Xb_old = x->posX_old_2 - atom_getfloatarg(3, argc, argv) - profondeur_old * a;
			Yb_old = x->posY_old_2 - atom_getfloatarg(4, argc, argv) - profondeur_old * b;
			Zb_old = x->posZ_old_2 - atom_getfloatarg(5, argc, argv) - profondeur_old * c;

			rayon_old = sqrt ( pow(Xb_old, 2) + pow(Yb_old, 2)  + pow(Zb_old, 2) );		

			tmp *= (rayon - rayon_old);

			x->forceX -= tmp * Xb;
			x->forceY -= tmp * Yb;
			x->forceZ -= tmp * Zb;

			tmp = atom_getfloatarg(11, argc, argv); // force normal proportionne a 1/R
			if (rayon != 0)
			{	
				tmp /= rayon;
			    x->forceX += tmp * Xb;
				x->forceY += tmp * Yb;
				x->forceZ += tmp * Zb;
			}

			tmp = atom_getfloatarg(12, argc, argv); // force normal proportionne a 1/R*R
			if (rayon != 0)
			{	
				tmp /= (rayon*rayon);
			    x->forceX += tmp * Xb;
				x->forceY += tmp * Yb;
				x->forceZ += tmp * Zb;
			}

			tmp = atom_getfloatarg(15, argc, argv); // force tengente constante
			x->forceX -= tmp * Ta;
			x->forceY -= tmp * Tb;
			x->forceZ -= tmp * Tc;

			tmp = atom_getfloatarg(16, argc, argv); // rigidité tengentiel proportionelle
			tmp *= ( atom_getfloatarg(7, argc, argv) - rayon ) ;
			x->forceX += tmp * Ta;
			x->forceY += tmp * Tb;
			x->forceZ += tmp * Tc;

			tmp = atom_getfloatarg(17, argc, argv); // deplacement normal constante

			x->dX += tmp * Xb;
			x->dY += tmp * Yb;
			x->dZ += tmp * Zb;
	
			tmp = atom_getfloatarg(19, argc, argv); // deplacement normal proportionelle
			tmp *= ( atom_getfloatarg(7, argc, argv) - rayon ) ;
			x->dX += tmp * Xb;
			x->dY += tmp * Yb;
			x->dZ += tmp * Zb;

			tmp = atom_getfloatarg(18, argc, argv); // deplacement tengente constante
			x->dX += tmp * Ta;
			x->dY += tmp * Tb;
			x->dZ += tmp * Tc;

			tmp = atom_getfloatarg(20, argc, argv); // deplacement tengentiel proportionelle
			tmp *= ( atom_getfloatarg(7, argc, argv) - rayon ) ;
			x->dX += tmp * Ta;
			x->dY += tmp * Tb;
			x->dZ += tmp * Tc;
		}
	}	
	else
	{
		error("bad cylinder interraction message");
	}
}

void *masse3D_new(t_symbol *s, int argc, t_atom *argv)
{
  t_masse3D *x = (t_masse3D *)pd_new(masse3D_class);

  x->x_sym = atom_getsymbolarg(0, argc, argv);
  x->x_state = makeseed();

  pd_bind(&x->x_obj.ob_pd, atom_getsymbolarg(0, argc, argv));

  x->position3D_new=outlet_new(&x->x_obj, 0);
  x->force_out=outlet_new(&x->x_obj, 0);
  x->vitesse_out=outlet_new(&x->x_obj, 0);

  x->forceX=0;
  x->forceY=0;
  x->forceZ=0;

  if (argc >= 2)
    x->masse3D = atom_getfloatarg(1, argc, argv) ;
  else
    x->masse3D = 1;

  x->onoff = 1;

  x->VX = 0;
  x->VY = 0;
  x->VZ = 0;

  x->dX=0;
  x->dY=0;
  x->dZ=0;

   if (argc >= 3)
		x->Xinit = atom_getfloatarg(2, argc, argv);
	else
		x->Xinit = 0 ;

	x->posX_old_1 = x->Xinit ;
	x->posX_old_2 = x->Xinit;
	SETFLOAT(&(x->pos_new[0]),  x->Xinit);

	if (argc >= 4)
		x->Yinit = atom_getfloatarg(3, argc, argv);
	else
		x->Yinit = 0 ;

	x->posY_old_1 = x->Yinit ;
	x->posY_old_2 = x->Yinit;
	SETFLOAT(&(x->pos_new[1]),  x->Yinit);

	if (argc >= 5)
		x->Zinit = atom_getfloatarg(4, argc, argv);
	else
		x->Zinit = 0 ;

	x->posZ_old_1 = x->Zinit ;
	x->posZ_old_2 = x->Zinit;
	SETFLOAT(&(x->pos_new[2]),  x->Zinit);


	  if (argc >= 6)
		x->minX = atom_getfloatarg(5, argc, argv) ;
	  else 
		x->minX = -100000;

	  if (argc >= 7)
		x->maxX = atom_getfloatarg(6, argc, argv) ;
	  else 
		x->maxX = 100000;
	
	  if (argc >= 8)
		x->minY = atom_getfloatarg(7, argc, argv) ;
	  else 
		x->minY = -100000;

	  if (argc >= 9)
		x->maxY = atom_getfloatarg(8, argc, argv) ;
	  else 
		x->maxY = 100000;

	  if (argc >= 10)
		x->minZ = atom_getfloatarg(9, argc, argv) ;
	  else 
		x->minZ = -100000;

	  if (argc >= 11)
		x->maxZ = atom_getfloatarg(10, argc, argv) ;
	  else 
		x->maxZ = 100000;

	  if (argc >= 12)
		x->seuil = atom_getfloatarg(11, argc, argv) ;
	  else 
		x->seuil = 0;

	  if (argc >= 13)
		x->damp = atom_getfloatarg(12, argc, argv) ;
	  else 
		x->damp = 0;

  return (void *)x;
}

static void masse3D_free(t_masse3D *x)
{
    pd_unbind(&x->x_obj.ob_pd, x->x_sym);
}


void masse3D_setup(void) 
{

  masse3D_class = class_new(gensym("masse3D"),
        (t_newmethod)masse3D_new,
        (t_method)masse3D_free,
		sizeof(t_masse3D),
        CLASS_DEFAULT, A_GIMME, 0);

  class_addcreator((t_newmethod)masse3D_new, gensym("mass3D"), A_GIMME, 0);
  class_addcreator((t_newmethod)masse3D_new, gensym("pmpd.mass3D"), A_GIMME, 0);

  class_addmethod(masse3D_class, (t_method)masse3D_force, gensym("force3D"),A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
  class_addbang(masse3D_class, masse3D_bang);

  class_addmethod(masse3D_class, (t_method)masse3D_dX, gensym("dX"), A_DEFFLOAT, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_dY, gensym("dY"), A_DEFFLOAT, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_dZ, gensym("dZ"), A_DEFFLOAT, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_dXYZ, gensym("dXYZ"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_setX, gensym("setX"), A_DEFFLOAT, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_setY, gensym("setY"), A_DEFFLOAT, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_setZ, gensym("setZ"), A_DEFFLOAT, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_setXYZ, gensym("setXYZ"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_minX, gensym("setXmin"), A_DEFFLOAT, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_minY, gensym("setYmin"), A_DEFFLOAT, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_maxX, gensym("setXmax"), A_DEFFLOAT, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_maxY, gensym("setYmax"), A_DEFFLOAT, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_minZ, gensym("setZmin"), A_DEFFLOAT, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_maxZ, gensym("setZmax"), A_DEFFLOAT, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_set_masse3D, gensym("setM"), A_DEFFLOAT, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_reset, gensym("reset"), 0);
  class_addmethod(masse3D_class, (t_method)masse3D_resetf, gensym("resetF"), 0);
  class_addmethod(masse3D_class, (t_method)masse3D_reset, gensym("loadbang"), 0);
  class_addmethod(masse3D_class, (t_method)masse3D_on, gensym("on"), 0);
  class_addmethod(masse3D_class, (t_method)masse3D_off, gensym("off"), 0);
  class_addmethod(masse3D_class, (t_method)masse3D_seuil, gensym("setT"), A_DEFFLOAT, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_damp, gensym("setD"), A_DEFFLOAT, 0);

  class_addmethod(masse3D_class, (t_method)masse3D_inter_ambient, gensym("interactor_ambient_3D"), A_GIMME, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_inter_sphere, gensym("interactor_sphere_3D"), A_GIMME, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_inter_plane, gensym("interactor_plane_3D"), A_GIMME, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_inter_circle, gensym("interactor_circle_3D"), A_GIMME, 0);
  class_addmethod(masse3D_class, (t_method)masse3D_inter_cylinder, gensym("interactor_cylinder_3D"), A_GIMME, 0);

}
