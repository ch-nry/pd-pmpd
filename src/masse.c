#include "m_pd.h"
#include "math.h"

static t_class *masse_class;

typedef struct _masse {
  t_object  x_obj;
  t_float pos_old_1, pos_old_2, Xinit;
  t_float force, masse, dX;
  t_float minX, maxX;
  t_outlet *position_new, *vitesse_out, *force_out;
  t_symbol *x_sym; // receive
  unsigned int x_state; // random
  t_float x_f; // random

} t_masse;

static int makeseed(void)
{
    static unsigned int random_nextseed = 1489853723;
    random_nextseed = random_nextseed * 435898247 + 938284287;
    return (random_nextseed & 0x7fffffff);
}

static float random_bang(t_masse *x)
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

void masse_minX(t_masse *x, t_floatarg f1)
{
  x->minX = f1;
}

void masse_maxX(t_masse *x, t_floatarg f1)
{
  x->maxX = f1;
}

void masse_float(t_masse *x, t_floatarg f1)
{
  x->force += f1;
}

void masse_bang(t_masse *x)
{
  t_float pos_new;

	if (x->masse > 0)
  pos_new = x->force/x->masse + 2*x->pos_old_1 - x->pos_old_2;
	else pos_new = x->pos_old_1;

  pos_new = max(min(x->maxX, pos_new), x->minX);

  pos_new += x->dX;

  x->pos_old_1 += x->dX; // pour ne pas avoir d'inertie suplementaire du a ce deplacement
 
  outlet_float(x->vitesse_out, x->pos_old_1 - x->pos_old_2);
  outlet_float(x->force_out, x->force);
  outlet_float(x->position_new, pos_new);

  x->pos_old_2 = x->pos_old_1;
  x->pos_old_1 = pos_new;

//  x->force = 0;

  x->force = random_bang(x)*1e-25; // avoiding denormal problem by adding low amplitude noise

  x->dX = 0;

}

void masse_reset(t_masse *x)
{
  x->pos_old_2 = x->Xinit;
  x->pos_old_1 = x->Xinit;

  x->force=0;

  outlet_float(x->position_new, x->Xinit);
}

void masse_resetF(t_masse *x)
{
  x->force=0;

}

void masse_dX(t_masse *x, t_float posX)
{
  x->dX += posX;
}

void masse_setX(t_masse *x, t_float posX)
{
  x->pos_old_2 = posX;			// clear history for stability (instability) problem
  x->pos_old_1 = posX;

  x->force=0;

  outlet_float(x->position_new, posX);
}

void masse_loadbang(t_masse *x)
{
  outlet_float(x->position_new, x->Xinit);
}

void masse_set_masse(t_masse *x, t_float mass)
{
  x->masse=mass;
}

static void masse_free(t_masse *x)
{
    pd_unbind(&x->x_obj.ob_pd, x->x_sym);
}

void *masse_new(t_symbol *s, t_floatarg M, t_floatarg X)
{
  
  t_masse *x = (t_masse *)pd_new(masse_class);

  x->x_sym = s;
  pd_bind(&x->x_obj.ob_pd, s);

  x->position_new=outlet_new(&x->x_obj, 0);
  x->force_out=outlet_new(&x->x_obj, 0);
  x->vitesse_out=outlet_new(&x->x_obj, 0); 

  x->Xinit=X;

  x->pos_old_1 = X;
  x->pos_old_2 = X;
  x->force=0;
  x->masse=M;

  x->minX = -100000;
  x->maxX = 100000;

  if (x->masse<=0) x->masse=1;

  makeseed();

  return (void *)x;
}

void masse_setup(void) 
{

  masse_class = class_new(gensym("masse"),
        (t_newmethod)masse_new,
        (t_method)masse_free,
		sizeof(t_masse),
        CLASS_DEFAULT, A_DEFSYM, A_DEFFLOAT, A_DEFFLOAT,0);
  class_addcreator((t_newmethod)masse_new, gensym("mass"), A_DEFSYM, A_DEFFLOAT, A_DEFFLOAT,0);
  class_addcreator((t_newmethod)masse_new, gensym("pmpd.mass"), A_DEFSYM, A_DEFFLOAT, A_DEFFLOAT,0);
  class_addfloat(masse_class, masse_float);
  class_addbang(masse_class, masse_bang);
  class_addmethod(masse_class, (t_method)masse_set_masse, gensym("setM"), A_DEFFLOAT, 0);
  class_addmethod(masse_class, (t_method)masse_setX, gensym("setX"), A_DEFFLOAT, 0);
  class_addmethod(masse_class, (t_method)masse_dX, gensym("dX"), A_DEFFLOAT, 0);
  class_addmethod(masse_class, (t_method)masse_reset, gensym("reset"), 0);
  class_addmethod(masse_class, (t_method)masse_resetF, gensym("resetF"), 0);
  class_addmethod(masse_class, (t_method)masse_minX, gensym("setXmin"), A_DEFFLOAT, 0);
  class_addmethod(masse_class, (t_method)masse_maxX, gensym("setXmax"), A_DEFFLOAT, 0);
  class_addmethod(masse_class, (t_method)masse_loadbang, gensym("loadbang"), 0);
}

