#include "m_pd.h"
#include "math.h"

static t_class *liaKD_class;

typedef struct _liaKD {
  t_object  x_obj;
  t_float raideur, viscosite, D2, longueur, distance_old, position1,  position2, position_old1, position_old2;
  t_outlet *force1;
  t_outlet *force2;
  t_float Lmin, Lmax;
  t_symbol *x_sym;  // receive
} t_liaKD;

void liaKD_float(t_liaKD *x, t_floatarg f1)
{
  x->position1 = f1;
}

void liaKD_bang(t_liaKD *x)
{
  t_float force1, force2, distance;

  distance = (x->position2 - x->position1);
//distance = abs(x->position2 - x->position1);
  if (distance<0) distance = -distance;

  force1 =  x->raideur*(distance-(x->longueur)) + x->viscosite*(distance - x->distance_old) ;

  x->distance_old = distance;  

  if (distance > x->Lmax) force1=0;
  if (distance < x->Lmin) force1=0;

   if (distance != 0)
  {
    force1 = force1 * (x->position2 - x->position1) / distance;
  }

  force2 = -force1 + (x->position_old2 - x->position2)*x->D2;
  force1 += (x->position_old1 - x->position1)*x->D2;
  // masse damping

  outlet_float(x->force1, force1);
  outlet_float(x->force2, force2);
  
 
  x->position_old1 = x->position1;
  x->position_old2 = x->position2;

}

void liaKD_reset(t_liaKD *x)
{
  x->position1 = 0;
  x->position2 = 0;

  x->position_old1 = 0;
  x->position_old2 = 0;

  x->distance_old = x->longueur;
}

void liaKD_resetF(t_liaKD *x)
{
  x->position_old1 = x->position1;
  x->position_old2 = x->position2;

  x->distance_old = x->longueur;
}

void liaKD_resetl(t_liaKD *x)
{
  x->longueur = (x->position1 - x->position2);
}

void liaKD_setL(t_liaKD *x, t_float L)
{
  x->longueur = L;
}

void liaKD_setK(t_liaKD *x, t_float K)
{
  x->raideur = K;
}

void liaKD_setD(t_liaKD *x, t_float D)
{
  x->viscosite = D;
}

void liaKD_setD2(t_liaKD *x, t_float D2)
{
  x->D2 = D2;
}

void liaKD_Lmin(t_liaKD *x, t_float Lmin)
{
  x->Lmin = Lmin;
}

void liaKD_Lmax(t_liaKD *x, t_float Lmax)
{
  x->Lmax = Lmax;
}

static void liaKD_free(t_liaKD *x)
{
    pd_unbind(&x->x_obj.ob_pd, x->x_sym);
}

void *liaKD_new(t_symbol *s, t_floatarg L, t_floatarg K, t_floatarg D, t_floatarg D2 )
{
  
  t_liaKD *x = (t_liaKD *)pd_new(liaKD_class);

  x->x_sym = s;
  pd_bind(&x->x_obj.ob_pd, s);

  floatinlet_new(&x->x_obj, &x->position2);

  x->force1=outlet_new(&x->x_obj, 0);
  x->force2=outlet_new(&x->x_obj, 0);

  x->position1 = 0;
  x->position2 = 0;
 
  x->raideur=K;
  x->viscosite=D;
  x->D2=D2;

  x->Lmin= 0;
  x->Lmax= 10000;

  x->longueur=L;

  return (void *)x;
}

void lia_setup(void) 
{
  liaKD_class = class_new(gensym("lia"),
        (t_newmethod)liaKD_new,
        (t_method)liaKD_free,
		sizeof(t_liaKD),
        CLASS_DEFAULT, A_DEFSYM, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);

  class_addcreator((t_newmethod)liaKD_new, gensym("link"), A_DEFSYM, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
  class_addcreator((t_newmethod)liaKD_new, gensym("pmpd.link"), A_DEFSYM, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);

  class_addfloat(liaKD_class, liaKD_float);
  class_addbang(liaKD_class, liaKD_bang);
  class_addmethod(liaKD_class, (t_method)liaKD_reset, gensym("reset"), 0);
  class_addmethod(liaKD_class, (t_method)liaKD_resetl, gensym("resetL"), 0);
  class_addmethod(liaKD_class, (t_method)liaKD_resetF, gensym("resetF"), 0);
  class_addmethod(liaKD_class, (t_method)liaKD_setD, gensym("setD"), A_DEFFLOAT, 0);
  class_addmethod(liaKD_class, (t_method)liaKD_setD2, gensym("setD2"), A_DEFFLOAT, 0);
  class_addmethod(liaKD_class, (t_method)liaKD_setK, gensym("setK"), A_DEFFLOAT, 0);
  class_addmethod(liaKD_class, (t_method)liaKD_setL, gensym("setL"), A_DEFFLOAT, 0);
  class_addmethod(liaKD_class, (t_method)liaKD_Lmin, gensym("setLmin"), A_DEFFLOAT, 0);
  class_addmethod(liaKD_class, (t_method)liaKD_Lmax, gensym("setLmax"), A_DEFFLOAT, 0);
}
