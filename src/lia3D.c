#include "m_pd.h"
#include "math.h"

static t_class *lia3D_class;

typedef struct _lia3D {
  t_object  x_obj;
  t_float raideur, viscosite, D2, longueur, distance_old;
  t_float position3Dx1, position3Dx2, posx_old1, posx_old2;
  t_float position3Dy1, position3Dy2, posy_old1, posy_old2;
  t_float position3Dz1, position3Dz2, posz_old1, posz_old2;
  t_float Lmin, Lmax, muscle;
  t_outlet *force1;
  t_outlet *force2;
  t_symbol *x_sym;  // receive
} t_lia3D;

void lia3D_position3D(t_lia3D *x, t_floatarg f1, t_floatarg f2, t_floatarg f3)
{
  x->position3Dx1 = f1;
  x->position3Dy1 = f2;
  x->position3Dz1 = f3;

}

void lia3D_position3D2(t_lia3D *x, t_floatarg f1, t_floatarg f2, t_floatarg f3)
{
  x->position3Dx2 = f1;
  x->position3Dy2 = f2;
  x->position3Dz2 = f3;
}

void lia3D_bang(t_lia3D *x)
{
  t_float force, force2, forcex1, forcey1, forcez1, forcex2, forcey2, forcez2, distance;
  t_atom force1[3];

  distance = sqrt ( pow((x->position3Dx2-x->position3Dx1), 2) + pow((x->position3Dy2-x->position3Dy1),2) + pow((x->position3Dz2-x->position3Dz1), 2) );

  force = ( x->raideur*(distance-(x->longueur * x->muscle)) ) + (  x->viscosite*(distance-x->distance_old) );

  if (distance > x->Lmax)  force=0;
  if (distance < x->Lmin)  force=0;

  if (distance != 0)
  {
    forcex1 = force * (x->position3Dx2 - x->position3Dx1) / distance;
    forcey1 = force * (x->position3Dy2 - x->position3Dy1) / distance;
    forcez1 = force * (x->position3Dz2 - x->position3Dz1) / distance;
  }
  else
  {
   forcex1 = 0;
   forcey1 = 0;
   forcez1 = 0;
  }

  forcex2 = -forcex1;
  forcey2 = -forcey1;
  forcez2 = -forcez1;

  forcex1 += (x->posx_old1 - x->position3Dx1)*x->D2;
  forcey1 += (x->posy_old1 - x->position3Dy1)*x->D2;
  forcez1 += (x->posz_old1 - x->position3Dz1)*x->D2;
 
  forcex2 += (x->posx_old2 - x->position3Dx2)*x->D2;
  forcey2 += (x->posy_old2 - x->position3Dy2)*x->D2;
  forcez2 += (x->posz_old2 - x->position3Dz2)*x->D2;


  SETFLOAT(&(force1[0]), forcex1 );
  SETFLOAT(&(force1[1]), forcey1 );
  SETFLOAT(&(force1[2]), forcez1 );
  outlet_anything(x->force1, gensym("force3D"), 3, force1);
 
  SETFLOAT(&(force1[0]), forcex2 );
  SETFLOAT(&(force1[1]), forcey2 );
  SETFLOAT(&(force1[2]), forcez2 );

  outlet_anything(x->force2, gensym("force3D"), 3, force1);
 
  x->posx_old2 = x->position3Dx2;
  x->posx_old1 = x->position3Dx1;

  x->posy_old2 = x->position3Dy2;
  x->posy_old1 = x->position3Dy1;

  x->posz_old2 = x->position3Dz2;
  x->posz_old1 = x->position3Dz1;

  x->distance_old = distance;
}

void lia3D_reset(t_lia3D *x)
{
  x->position3Dx1 = 0;
  x->position3Dx2 = 0;
  x->posx_old1 = 0;
  x->posx_old2 = 0;

  x->position3Dy1 = 0;
  x->position3Dy2 = 0;
  x->posy_old1 = 0;
  x->posy_old2 = 0;

  x->position3Dz1 = 0;
  x->position3Dz2 = 0;
  x->posz_old1 = 0;
  x->posz_old2 = 0;

  x->distance_old = x->longueur;

}

void lia3D_resetF(t_lia3D *x)
{

  x->posx_old1 = x->position3Dx1;
  x->posx_old2 = x->position3Dx2;
 
  x->posy_old1 = x->position3Dy1;
  x->posy_old2 = x->position3Dy2;
 
  x->posz_old1 = x->position3Dz1;
  x->posz_old2 = x->position3Dz2;
 
  x->distance_old = x->longueur;

}

void lia3D_resetL(t_lia3D *x)
{
  x->longueur = sqrt ( pow((x->position3Dx2-x->position3Dx1), 2) + pow((x->position3Dy2-x->position3Dy1),2) + pow((x->position3Dz2-x->position3Dz1), 2) );
}

void lia3D_setK(t_lia3D *x, t_float K)
{
  x->raideur = K;
}

void lia3D_setL(t_lia3D *x, t_float L)
{
  x->longueur = L;
}

void lia3D_setD(t_lia3D *x, t_float D)
{
  x->viscosite = D;
}

void lia3D_setD2(t_lia3D *x, t_float D2)
{
  x->D2 = D2;
}

void lia3D_Lmin(t_lia3D *x, t_float Lmin)
{
  x->Lmin = Lmin;
}

void lia3D_Lmax(t_lia3D *x, t_float Lmax)
{
  x->Lmax = Lmax;
}

void lia3D_muscle(t_lia3D *x, t_float muscle)
{
  x->muscle = muscle;
}

static void lia3D_free(t_lia3D *x)
{
    pd_unbind(&x->x_obj.ob_pd, x->x_sym);
}

void *lia3D_new(t_symbol *s, t_floatarg l, t_floatarg K, t_floatarg D, t_floatarg D2)
{
  
  t_lia3D *x = (t_lia3D *)pd_new(lia3D_class);

  x->x_sym = s;
  pd_bind(&x->x_obj.ob_pd, s);

  inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("position3D"), gensym("position3D2"));
 
  x->force1=outlet_new(&x->x_obj, 0);
  x->force2=outlet_new(&x->x_obj, 0);

  x->position3Dx1 = 0;
  x->position3Dx2 = 0;
  x->position3Dy1 = 0;
  x->position3Dy2 = 0;
  x->position3Dz1 = 0;
  x->position3Dz2 = 0;

  x->raideur = K;
  x->viscosite = D;
  x->longueur = l;
 
  x->D2 = D2;

  x->Lmin= 0;
  x->Lmax= 10000;

  x->distance_old = x->longueur;

  x->muscle = 1;

  return (void *)x;
}

void lia3D_setup(void) 
{

  lia3D_class = class_new(gensym("lia3D"),
        (t_newmethod)lia3D_new,
        (t_method)lia3D_free,
		sizeof(t_lia3D),
        CLASS_DEFAULT, A_DEFSYM, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT,  A_DEFFLOAT, 0);

  class_addcreator((t_newmethod)lia3D_new, gensym("link3D"), A_DEFSYM, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
  class_addcreator((t_newmethod)lia3D_new, gensym("pmpd.link3D"), A_DEFSYM, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);

  class_addbang(lia3D_class, lia3D_bang);
  class_addmethod(lia3D_class, (t_method)lia3D_reset, gensym("reset"), 0);
  class_addmethod(lia3D_class, (t_method)lia3D_resetL, gensym("resetL"), 0);
  class_addmethod(lia3D_class, (t_method)lia3D_resetF, gensym("resetF"), 0);
  class_addmethod(lia3D_class, (t_method)lia3D_setD, gensym("setD"), A_DEFFLOAT, 0);
  class_addmethod(lia3D_class, (t_method)lia3D_setD2, gensym("setD2"), A_DEFFLOAT, 0);
  class_addmethod(lia3D_class, (t_method)lia3D_setK, gensym("setK"), A_DEFFLOAT, 0);
  class_addmethod(lia3D_class, (t_method)lia3D_setL, gensym("setL"), A_DEFFLOAT, 0);
  class_addmethod(lia3D_class, (t_method)lia3D_Lmin, gensym("setLmin"), A_DEFFLOAT, 0);
  class_addmethod(lia3D_class, (t_method)lia3D_Lmax, gensym("setLmax"), A_DEFFLOAT, 0);
  class_addmethod(lia3D_class, (t_method)lia3D_muscle, gensym("setM"), A_DEFFLOAT, 0);
  class_addmethod(lia3D_class, (t_method)lia3D_position3D, gensym("position3D"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
  class_addmethod(lia3D_class, (t_method)lia3D_position3D2, gensym("position3D2"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);

}
