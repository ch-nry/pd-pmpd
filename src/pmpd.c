/*
--------------------------  pmpd  ---------------------------------------- 
                                                                              
 pmpd = physical modeling for pure data                                     
 Written by Cyrille Henry (cyrille.henry@la-kitchen.fr)
 
 Get sources at http://drpichon.free.fr/pure-data/physical-modeling/

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

#ifndef VERSION
#define VERSION "0.05"
#endif

#include "m_pd.h"

#ifndef __DATE__ 
#define __DATE__ ""
#endif

#include "masse.c"
#include "lia.c"
#include "masse2D.c"
#include "lia2D.c"
#include "masse3D.c"
#include "lia3D.c"

#include "iAmbient2D.c"
#include "iLine2D.c"
#include "iSeg2D.c"
#include "iCircle2D.c"

#include "tSquare2D.c"
#include "tLine2D.c"
#include "tSeg2D.c"
#include "tCircle2D.c"
#include "tLia2D.c"

#include "iAmbient3D.c"
#include "iSphere3D.c"
#include "iPlane3D.c"
#include "iCircle3D.c"
#include "iCylinder3D.c"


#include "tCube3D.c"
#include "tSphere3D.c"
#include "tPlane3D.c"
#include "tCircle3D.c"
#include "tCylinder3D.c"
#include "tLia3D.c"

static t_class *pmpd_class;

typedef struct _pmpd 
{
  t_object  x_obj;
} t_pmpd;

t_float max(t_float x, t_float y)
{
    if (x >= y)
    {
        return x;
    }
    return y;
}

t_float min(t_float x, t_float y)
{
    if (x <= y)
    {
        return x;
    }
    return y;
}

void *pmpd_new(void)
{ 
  t_pmpd *x = (t_pmpd *)pd_new(pmpd_class);
  return (void *)x;
}

void pmpd_setup(void) 
{
 pmpd_class = class_new(gensym("pmpd"),
        (t_newmethod)pmpd_new,
        0, sizeof(t_pmpd),0,0);

 post("");
 post("     pmpd = Physical Modeling for Pure Data");
 post("     version "VERSION);
 post("     compiled "__DATE__);
 post("     Contact : cyrille.henry@la-kitchen.fr");
 post("");

masse_setup() ;
lia_setup() ;
masse2D_setup() ;
lia2D_setup() ;
masse3D_setup() ;
lia3D_setup() ;

iAmbient2D_setup();
iLine2D_setup();
iSeg2D_setup();
iCircle2D_setup();

tSquare2D_setup();
tCircle2D_setup();
tLine2D_setup();
tSeg2D_setup();
tLia2D_setup();

iAmbient3D_setup();
iSphere3D_setup();
iPlane3D_setup();
iCircle3D_setup();
iCylinder3D_setup();

tLia3D_setup();
tCube3D_setup();
tPlane3D_setup();
tSphere3D_setup();
tCylinder3D_setup();
tCircle3D_setup();

}
