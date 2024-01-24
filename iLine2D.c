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


#include "m_pd.h"
#include <math.h>

#include "pmpd_export.h"
#include "pmpd_version.h"

static t_class *iLine2D_class;

typedef struct _iLine2D {
  t_object  x_obj;
  t_atom  force[12];
  t_outlet *force_new;// outlet
  t_symbol *x_sym;  // send
} t_iLine2D;



void iLine2D_bang(t_iLine2D *x)
{
  if (x->x_sym->s_thing) typedmess(x->x_sym->s_thing, gensym("interactor_line_2D"), 12, x->force);

  outlet_anything(x->force_new, gensym("interactor_line_2D"), 12, x->force);
}

void iLine2D_X1(t_iLine2D *x, t_float Xmax)
{
  SETFLOAT(&(x->force[0]), Xmax);
}

void iLine2D_Y1(t_iLine2D *x, t_float Ymax)
{
  SETFLOAT(&(x->force[1]), Ymax);
}

void iLine2D_X2(t_iLine2D *x, t_float X)
{
  SETFLOAT(&(x->force[2]), X);
}

void iLine2D_Y2(t_iLine2D *x, t_float X)
{
  SETFLOAT(&(x->force[3]), X);
}

void iLine2D_setPmax(t_iLine2D *x, t_floatarg f1)
{
  SETFLOAT(&(x->force[4]), f1);
}

void iLine2D_setFN(t_iLine2D *x, t_floatarg f1)
{
  SETFLOAT(&(x->force[5]), f1);
}

void iLine2D_setFT(t_iLine2D *x, t_floatarg f1)
{
  SETFLOAT(&(x->force[6]), f1);
}

void iLine2D_setKN(t_iLine2D *x, t_floatarg f1)
{
  SETFLOAT(&(x->force[7]), f1);
}

void iLine2D_setDN(t_iLine2D *x, t_floatarg f1)
{
  SETFLOAT(&(x->force[8]), f1);
}

void iLine2D_setDT(t_iLine2D *x, t_floatarg f1)
{
  SETFLOAT(&(x->force[9]), f1);
}

void iLine2D_setdN(t_iLine2D *x, t_floatarg f1)
{
  SETFLOAT(&(x->force[10]), f1);
}

void iLine2D_setdT(t_iLine2D *x, t_floatarg f1)
{
  SETFLOAT(&(x->force[11]), f1);
}

void *iLine2D_new(t_symbol *s, int argc, t_atom *argv)
{
  t_iLine2D *x = (t_iLine2D *)pd_new(iLine2D_class);

  x->x_sym = atom_getsymbolarg(0, argc, argv);

  x->force_new=outlet_new(&x->x_obj, 0);


  if (argc>=13)
    SETFLOAT(&(x->force[11]), atom_getfloatarg(12, argc, argv));
  else
    SETFLOAT(&(x->force[11]), 0);
 
  if (argc>=12)
    SETFLOAT(&(x->force[10]), atom_getfloatarg(11, argc, argv));
  else
    SETFLOAT(&(x->force[10]), 0);

 if (argc>=11)
    SETFLOAT(&(x->force[9]), atom_getfloatarg(10, argc, argv));
  else
    SETFLOAT(&(x->force[9]), 0);
 
  if (argc>=10)
    SETFLOAT(&(x->force[8]), atom_getfloatarg(9, argc, argv));
  else
    SETFLOAT(&(x->force[8]), 0);

  if (argc>=9)
    SETFLOAT(&(x->force[7]), atom_getfloatarg(8, argc, argv));
  else
    SETFLOAT(&(x->force[7]), 0);

  if (argc>=8)
    SETFLOAT(&(x->force[6]), atom_getfloatarg(7, argc, argv));
  else
    SETFLOAT(&(x->force[6]), 0);

  if (argc>=7)
    SETFLOAT(&(x->force[5]), atom_getfloatarg(6, argc, argv));
  else
    SETFLOAT(&(x->force[5]), 0);

  if (argc>=6)
    SETFLOAT(&(x->force[4]), atom_getfloatarg(5, argc, argv));
  else
    SETFLOAT(&(x->force[4]), 1);

  if (argc>=5)
    SETFLOAT(&(x->force[3]), atom_getfloatarg(4, argc, argv));
  else
    SETFLOAT(&(x->force[3]), 0);

  if (argc>=4)
    SETFLOAT(&(x->force[2]), atom_getfloatarg(3, argc, argv));
  else
    SETFLOAT(&(x->force[2]), 1);

  if (argc>=3)
    SETFLOAT(&(x->force[1]), atom_getfloatarg(2, argc, argv));
  else
    SETFLOAT(&(x->force[1]), 0);

  if (argc>=2)
    SETFLOAT(&(x->force[0]), atom_getfloatarg(1, argc, argv));
  else
    SETFLOAT(&(x->force[0]), -1);

  return (x);
}

PMPD_EXPORT void iLine2D_setup(void)
{

  iLine2D_class = class_new(gensym("iLine2D"),
        (t_newmethod)iLine2D_new,
        0, sizeof(t_iLine2D),
        CLASS_DEFAULT, A_GIMME, 0);

  if(!iLine2D_class)
      return;

  verbose(4, "iLine2D version %s (%s)", pmpd_tag(), pmpd_sha());

  class_addcreator((t_newmethod)iLine2D_new, gensym("pmpd.iLine2D"),  A_GIMME, 0);
 
  class_addbang(iLine2D_class, iLine2D_bang);
  class_addmethod(iLine2D_class, (t_method)iLine2D_setPmax, gensym("setPmax"), A_DEFFLOAT, 0);
  class_addmethod(iLine2D_class, (t_method)iLine2D_setFN, gensym("setFN"), A_DEFFLOAT, 0);
  class_addmethod(iLine2D_class, (t_method)iLine2D_setFT, gensym("setFT"), A_DEFFLOAT, 0);
  class_addmethod(iLine2D_class, (t_method)iLine2D_setKN, gensym("setKN"), A_DEFFLOAT, 0);
  class_addmethod(iLine2D_class, (t_method)iLine2D_setDN, gensym("setDN"), A_DEFFLOAT, 0);
  class_addmethod(iLine2D_class, (t_method)iLine2D_setDT, gensym("setDT"), A_DEFFLOAT, 0);
  class_addmethod(iLine2D_class, (t_method)iLine2D_setdN, gensym("setdN"), A_DEFFLOAT, 0);
  class_addmethod(iLine2D_class, (t_method)iLine2D_setdT, gensym("setdT"), A_DEFFLOAT, 0);
  class_addmethod(iLine2D_class, (t_method)iLine2D_X1, gensym("setX1"), A_DEFFLOAT, 0);
  class_addmethod(iLine2D_class, (t_method)iLine2D_Y1, gensym("setY1"), A_DEFFLOAT, 0);
  class_addmethod(iLine2D_class, (t_method)iLine2D_X2, gensym("setX2"), A_DEFFLOAT, 0);
  class_addmethod(iLine2D_class, (t_method)iLine2D_Y2, gensym("setY2"), A_DEFFLOAT, 0);


}
