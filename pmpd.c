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

#include "m_pd.h"









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
    
    class_addmethod(pmpd_class, (t_method)pmpd_linksPosL,               gensym("linksPosXL"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksLengthL,           gensym("linksLengthXL"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksPosSpeedL,        gensym("linksPosSpeedXL"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksLengthSpeedL,    gensym("linksLengthSpeedXL"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksPosT,               gensym("linksPosXT"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksLengthT,           gensym("linksLengthXT"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksPosSpeedT,        gensym("linksPosSpeedXT"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksLengthSpeedT,    gensym("linksLengthSpeedXT"), A_GIMME, 0);

    class_addmethod(pmpd_class, (t_method)pmpd_linksPosL,               gensym("linksPosL"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksLengthL,           gensym("linksLengthL"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksPosSpeedL,        gensym("linksPosSpeedL"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksLengthSpeedL,    gensym("linksLengthSpeedL"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksPosT,               gensym("linksPosT"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksLengthT,           gensym("linksLengthT"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksPosSpeedT,        gensym("linksPosSpeedT"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksLengthSpeedT,    gensym("linksLengthSpeedT"), A_GIMME, 0);
    
    class_addmethod(pmpd_class, (t_method)pmpd_linksEndT,               gensym("linksEndT"),   A_GIMME, 0);

/*    class_addmethod(pmpd_class, (t_method)pmpd_linksPosMean,               gensym("linksPosMean"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksLengthMean,               gensym("linksLengthMean"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksPosSpeedMean,        gensym("linksPosSpeedMean"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksLengthSpeedMean,    gensym("linksLengthSpeedMean"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksPosStd,                   gensym("linksPosStd"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksLengthStd,               gensym("linksLengthStd"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksPosSpeedStd,        gensym("linksPosSpeedStd"), A_GIMME, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_linksLengthSpeedStd0,    gensym("linksLengthSpeedStd"), A_GIMME, 0);
*/
    
    class_addmethod(pmpd_class, (t_method)pmpd_grabMass,        gensym("grabMass"), A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd_class, (t_method)pmpd_closestMass,        gensym("closestMass"), A_GIMME, 0);
}

