#include "m_pd.h"
#include "math.h"

static t_class *lia2D_class;

typedef struct _lia2D {
  t_object  x_obj;
  t_float raideur, viscosite, D2, longueur, distance_old;
  t_float position2Dx1, position2Dx2, posx_old1, posx_old2;
  t_float position2Dy1, position2Dy2, posy_old1, posy_old2;
  t_float Lmin, Lmax, muscle;
  t_outlet *force1;
  t_outlet *force2;
  t_symbol *x_sym;  // receive
} t_lia2D;

void lia2D_position2D(t_lia2D *x, t_floatarg f1, t_floatarg f2)
{
  x->position2Dx1 = f1;
  x->position2Dy1 = f2;
}

void lia2D_position2D2(t_lia2D *x, t_floatarg f1, t_floatarg f2)
{
  x->position2Dx2 = f1;
  x->position2Dy2 = f2;
}

void lia2D_bang(t_lia2D *x)
{
  t_float force, force2, forcex1, forcey1, forcex2, forcey2, distance;
  t_atom force1[2];

  distance = sqrt ( pow((x->position2Dx2-x->position2Dx1), 2) + pow((x->position2Dy2-x->position2Dy1), 2) );

  force = ( x->raideur*(distance-(x->longueur * x->muscle)) ) + (  x->viscosite*(distance-x->distance_old) );

  if (distance > x->Lmax) force=0;
  if (distance < x->Lmin) force=0;

  if (distance != 0)
  {
    forcex1 = force * (x->position2Dx2 - x->position2Dx1) / distance;
    forcey1 = force * (x->position2Dy2 - x->position2Dy1) / distance;
  }
  else
  {
   forcex1 = 0;
   forcey1 = 0 ;
  }

  forcex2 = -forcex1;
  forcey2 = -forcey1;

  forcex1 += (x->posx_old1 - x->position2Dx1)*x->D2;
  forcey1 += (x->posy_old1 - x->position2Dy1)*x->D2;
 
  forcex2 += (x->posx_old2 - x->position2Dx2)*x->D2;
  forcey2 += (x->posy_old2 - x->position2Dy2)*x->D2;

  SETFLOAT(&(force1[0]), forcex2 );
  SETFLOAT(&(force1[1]), forcey2 );

  outlet_anything(x->force2, gensym("force2D"), 2, force1);

  SETFLOAT(&(force1[0]), forcex1 );
  SETFLOAT(&(force1[1]), forcey1 );

  outlet_anything(x->force1, gensym("force2D"), 2, force1);
 
  x->posx_old2 = x->position2Dx2;
  x->posx_old1 = x->position2Dx1;

  x->posy_old2 = x->position2Dy2;
  x->posy_old1 = x->position2Dy1;

  x->distance_old = distance;
}

void lia2D_reset(t_lia2D *x)
{
  x->position2Dx1 = 0;
  x->position2Dx2 = 0;
  x->posx_old1 = 0;
  x->posx_old2 = 0;

  x->position2Dy1 = 0;
  x->position2Dy2 = 0;
  x->posy_old1 = 0;
  x->posy_old2 = 0;

  x->distance_old = x->longueur;
}

void lia2D_resetF(t_lia2D *x)
{

  x->posx_old1 = x->position2Dx1;
  x->posx_old2 = x->position2Dx2;

  x->posy_old1 = x->position2Dy1;
  x->posy_old2 = x->position2Dy2;

  x->distance_old = x->longueur;

}

void lia2D_resetL(t_lia2D *x)
{
  x->longueur = sqrt ( pow((x->position2Dx2-x->position2Dx1), 2) + pow((x->position2Dy2-x->position2Dy1), 2) );
}


void lia2D_setK(t_lia2D *x, t_float K)
{
  x->raideur = K;
}

void lia2D_setL(t_lia2D *x, t_float L)
{
  x->longueur = L;
}

void lia2D_setD(t_lia2D *x, t_float D)
{
  x->viscosite = D;
}

void lia2D_setD2(t_lia2D *x, t_float D)
{
  x->D2 = D;
}

void lia2D_Lmin(t_lia2D *x, t_float Lmin)
{
  x->Lmin = Lmin;
}

void lia2D_Lmax(t_lia2D *x, t_float Lmax)
{
  x->Lmax = Lmax;
}

void lia2D_muscle(t_lia2D *x, t_float muscle)
{
  x->muscle = muscle;
}

static void lia2D_free(t_lia2D *x)
{
    pd_unbind(&x->x_obj.ob_pd, x->x_sym);
}

void *lia2D_new(t_symbol *s, t_floatarg l, t_floatarg K, t_floatarg D, t_floatarg D2)
{
  
  t_lia2D *x = (t_lia2D *)pd_new(lia2D_class);

  x->x_sym = s;
  pd_bind(&x->x_obj.ob_pd, s);

  inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("position2D"), gensym("position2D2"));
 
  x->force1=outlet_new(&x->x_obj, 0);
  x->force2=outlet_new(&x->x_obj, 0);

  x->position2Dx1 = 0;
  x->position2Dx2 = 0;
  x->position2Dy1 = 0;
  x->position2Dy2 = 0;

  x->raideur=K;
  x->viscosite=D;
  x->longueur = l;

  x->D2=D2;

  x->Lmin= 0;
  x->Lmax= 10000;
  x->muscle= 1;

  x->distance_old = x->longueur;

  return (x);
}

void lia2D_setup(void) 
{

  lia2D_class = class_new(gensym("lia2D"),
        (t_newmethod)lia2D_new,
        (t_method)lia2D_free, 
		sizeof(t_lia2D),
        CLASS_DEFAULT, A_DEFSYM, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);

  class_addcreator((t_newmethod)lia2D_new, gensym("link2D"), A_DEFSYM, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
  class_addcreator((t_newmethod)lia2D_new, gensym("pmpd.link2D"), A_DEFSYM, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);

  class_addbang(lia2D_class, lia2D_bang);
  class_addmethod(lia2D_class, (t_method)lia2D_reset, gensym("reset"), 0);
  class_addmethod(lia2D_class, (t_method)lia2D_resetL, gensym("resetL"), 0);
  class_addmethod(lia2D_class, (t_method)lia2D_resetF, gensym("resetF"), 0);
  class_addmethod(lia2D_class, (t_method)lia2D_setD, gensym("setD"), A_DEFFLOAT, 0);
  class_addmethod(lia2D_class, (t_method)lia2D_setD2, gensym("setD2"), A_DEFFLOAT, 0);
  class_addmethod(lia2D_class, (t_method)lia2D_setK, gensym("setK"), A_DEFFLOAT, 0);
  class_addmethod(lia2D_class, (t_method)lia2D_setL, gensym("setL"), A_DEFFLOAT, 0);
  class_addmethod(lia2D_class, (t_method)lia2D_Lmin, gensym("setLmin"), A_DEFFLOAT, 0);
  class_addmethod(lia2D_class, (t_method)lia2D_Lmax, gensym("setLmax"), A_DEFFLOAT, 0);
  class_addmethod(lia2D_class, (t_method)lia2D_muscle, gensym("setM"), A_DEFFLOAT, 0);
  class_addmethod(lia2D_class, (t_method)lia2D_position2D, gensym("position2D"), A_DEFFLOAT, A_DEFFLOAT, 0);
  class_addmethod(lia2D_class, (t_method)lia2D_position2D2, gensym("position2D2"), A_DEFFLOAT, A_DEFFLOAT, 0);

}
