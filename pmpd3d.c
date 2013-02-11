/*
--------------------------  pmpd3d  ---------------------------------------- 
     
      
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

#include "pmpd3d.h"
#include "pmpd3d_core.c"
#include "pmpd3d_set.c"
#include "pmpd3d_get.c"
#include "pmpd3d_list.c"
#include "pmpd3d_tab.c"
#include "pmpd3d_test.c"
#include "pmpd3d_stat.c"
#include "pmpd3d_interactor.c"
#include "pmpd3d_various.c"

void pmpd3d_setup(void) 
{
 pmpd3d_class = class_new(gensym("pmpd3d"),
        (t_newmethod)pmpd3d_new,
        0, sizeof(t_pmpd3d),CLASS_DEFAULT, 0);

// pmpd3d_core
    class_addbang(pmpd3d_class, pmpd3d_bang);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_reset,           		gensym("reset"), 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_mass,            		gensym("mass"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_link,            		gensym("link"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_tLink,           		gensym("tLink"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_tabLink,         		gensym("tabLink"), A_GIMME, 0);
    
// pmpd3d_set
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setK,            		gensym("setK"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setD,            		gensym("setD"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setDEnv,         		gensym("setDEnv"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setDEnvOffset,   		gensym("setDEnvOffset"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setL,            		gensym("setL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setLCurrent,            	gensym("setLCurrent"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setLKTab,        		gensym("setLKTab"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setLDTab,        		gensym("setLDTab"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setLinkId,       		gensym("setLinkId"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setMassId,       		gensym("setMassId"), A_GIMME, 0);    
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setFixed,        		gensym("setFixed"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setMobile,       		gensym("setMobile"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setSpeedX,       		gensym("setSpeedX"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setSpeedY,       		gensym("setSpeedY"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setSpeedZ,       		gensym("setSpeedZ"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setSpeed,        		gensym("setSpeed"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setForceX,       		gensym("setForceX"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setForceY,       		gensym("setForceY"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setForceZ,       		gensym("setForceZ"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setForce,        		gensym("setForce"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setActive,        		gensym("setActive"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_setInactive,       		gensym("setInactive"), A_GIMME, 0);     
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_pos,             		gensym("pos"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_posX,            		gensym("posX"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_posY,            		gensym("posY"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_posZ,            		gensym("posZ"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_pos,             		gensym("setpos"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_posX,            		gensym("setposX"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_posY,            		gensym("setposY"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_posZ,          		  	gensym("setposZ"), A_GIMME, 0);
      
        
// pmpd3d_get
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_get,             		gensym("get"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPos,       		gensym("massPos"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeeds,    		gensym("massSpeed"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForces,    		gensym("massForce"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPos,        		gensym("linkPos"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksEnds,       		gensym("linkEnd"), A_GIMME, 0);

//pmpd3d_list
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosL,      		gensym("massesPosL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsL,   		gensym("massesSpeedsL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesL,   		gensym("massesForcesL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosXL,     		gensym("massesPosXL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsXL,  		gensym("massesSpeedsXL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesXL,  		gensym("massesForcesXL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosYL,     		gensym("massesPosYL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsYL,  		gensym("massesSpeedsYL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesYL,  		gensym("massesForcesYL"), A_GIMME, 0);    
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosZL,     		gensym("massesPosZL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsZL,  		gensym("massesSpeedsZL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesZL,  		gensym("massesForcesZL"), A_GIMME, 0);
    
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosL,      		gensym("massPosL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsL,   		gensym("massSpeedL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesL,   		gensym("massForceL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosXL,     		gensym("massPosXL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsXL,  		gensym("massSpeedXL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesXL,  		gensym("massForceXL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosYL,     		gensym("massPosYL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsYL,  		gensym("massSpeedYL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesYL,  		gensym("massForceYL"), A_GIMME, 0);    
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosZL,     		gensym("massPosZL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsZL,  		gensym("massSpeedZL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesZL,  		gensym("massForceZL"), A_GIMME, 0);
    
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosNormL,      	gensym("massesPosNormL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsNormL,   	gensym("massesSpeedsNormL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesNormL,   	gensym("massesForcesNormL"), A_GIMME, 0);    
    
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosNormL,      	gensym("massPosNormL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsNormL,   	gensym("massSpeedNormL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesNormL,   	gensym("massForceNormL"), A_GIMME, 0);    
    
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosL,           	gensym("linksPosL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthL,            gensym("linksLengthL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedL,          gensym("linksPosSpeedL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedL,       gensym("linksLengthSpeedL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosXL,              gensym("linksPosXL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthXL,           gensym("linksLengthXL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedXL,         gensym("linksPosSpeedXL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedXL,      gensym("linksLengthSpeedXL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosYL,              gensym("linksPosYL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthYL,           gensym("linksLengthYL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedYL,         gensym("linksPosSpeedYL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedYL,      gensym("linksLengthSpeedYL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosZL,              gensym("linksPosZL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthZL,           gensym("linksLengthZL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedZL,         gensym("linksPosSpeedZL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedZL,      gensym("linksLengthSpeedZL"), A_GIMME, 0);
    
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosL,           	gensym("linkPosL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthL,            gensym("linkLengthL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedL,          gensym("linkPosSpeedL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedL,       gensym("linkLengthSpeedL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosXL,              gensym("linkPosXL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthXL,           gensym("linkLengthXL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedXL,         gensym("linkPosSpeedXL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedXL,      gensym("linkLengthSpeedXL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosYL,              gensym("linkPosYL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthYL,           gensym("linkLengthYL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedYL,         gensym("linkPosSpeedYL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedYL,      gensym("linkLengthSpeedYL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosZL,              gensym("linkPosZL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthZL,           gensym("linkLengthZL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedZL,         gensym("linkPosSpeedZL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedZL,      gensym("linkLengthSpeedZL"), A_GIMME, 0);

    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosNormL,           gensym("linksPosNormL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthNormL,        gensym("linksLengthNormL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedNormL,      gensym("linksPosSpeedNormL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedNormL,   gensym("linksLengthSpeedNormL"), A_GIMME, 0);

    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosNormL,           gensym("linkPosNormL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthNormL,        gensym("linkLengthNormL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedNormL,      gensym("linkPosSpeedNormL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedNormL,   gensym("linkLengthSpeedNormL"), A_GIMME, 0);
    
// pmpd3d_tab
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosT,      		gensym("massesPosT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsT,   		gensym("massesSpeedsT"),A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesT,  	 		gensym("massesForcesT"),A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosXT,     		gensym("massesPosXT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsXT,  		gensym("massesSpeedsXT"),A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesXT,  		gensym("massesForcesXT"),A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosYT,     		gensym("massesPosYT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsYT,  		gensym("massesSpeedsYT"),A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesYT,  		gensym("massesForcesYT"),A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosZT,     		gensym("massesPosZT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsZT,  		gensym("massesSpeedsZT"),A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesZT,  		gensym("massesForcesZT"),A_GIMME, 0);
    
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosT,      		gensym("massPosT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsT,   		gensym("massSpeedT"),A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesT,  	 		gensym("massForceT"),A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosXT,     		gensym("massPosXT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsXT,  		gensym("massSpeedXT"),A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesXT,  		gensym("massForceXT"),A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosYT,     		gensym("massPosYT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsYT,  		gensym("massSpeedYT"),A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesYT,  		gensym("massForceYT"),A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosZT,     		gensym("massPosZT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsZT,  		gensym("massSpeedZT"),A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesZT,  		gensym("massForceZT"),A_GIMME, 0);
    
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosNormT,      	gensym("massesPosNormT"),  A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsNormT,   	gensym("massesSpeedsNormT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesNormT,   	gensym("massesForcesNormT"), A_GIMME, 0);
		
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosNormT,      	gensym("massPosNormT"),  A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsNormT,   	gensym("massSpeedNormT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesNormT,   	gensym("massForceNormT"), A_GIMME, 0);
    
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosT,           	gensym("linksPosT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthT,            gensym("linksLengthT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedT,          gensym("linksPosSpeedT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedT,       gensym("linksLengthSpeedT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosXT,              gensym("linksPosXT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthXT,       	gensym("linksLengthXT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedXT,      	gensym("linksPosSpeedXT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedXT,      gensym("linksLengthSpeedXT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosYT,              gensym("linksPosYT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthYT,           gensym("linksLengthYT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedYT,         gensym("linksPosSpeedYT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedYT,      gensym("linksLengthSpeedYT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosZT,              gensym("linksPosZT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthZT,           gensym("linksLengthZT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedZT,         gensym("linksPosSpeedZT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedZT,      gensym("linksLengthSpeedZT"), A_GIMME, 0);
    
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosT,           	gensym("linkPosT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthT,            gensym("linkLengthT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedT,          gensym("linkPosSpeedT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedT,       gensym("linkLengthSpeedT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosXT,              gensym("linkPosXT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthXT,       	gensym("linkLengthXT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedXT,      	gensym("linkPosSpeedXT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedXT,      gensym("linkLengthSpeedXT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosYT,              gensym("linkPosYT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthYT,           gensym("linkLengthYT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedYT,         gensym("linkPosSpeedYT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedYT,      gensym("linkLengthSpeedYT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosZT,              gensym("linkPosZT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthZT,           gensym("linkLengthZT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedZT,         gensym("linkPosSpeedZT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedZT,      gensym("linkLengthSpeedZT"), A_GIMME, 0);

    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosNormT,           gensym("linksPosNormT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthNormT,        gensym("linksLengthNormT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedNormT,      gensym("linksPosSpeedNormT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedNormT,   gensym("linksLengthSpeedNormT"), A_GIMME, 0);

    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosNormT,           gensym("linkPosNormT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthNormT,        gensym("linkLengthNormT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksPosSpeedNormT,      gensym("linkPosSpeedNormT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksLengthSpeedNormT,   gensym("linkLengthSpeedNormT"), A_GIMME, 0);
    
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksEndT,               gensym("linkEndT"),   A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksEnd1T,              gensym("linkEnd1T"),  A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksEnd2T,              gensym("linkEnd2T"),  A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksEndXT,              gensym("linkEndXT"),  A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksEnd1XT,             gensym("linkEnd1XT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksEnd2XT,             gensym("linkEnd2XT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksEndYT,              gensym("linkEndYT"),  A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksEnd1YT,             gensym("linkEnd1YT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksEnd2YT,             gensym("linkEnd2YT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksEndZT,              gensym("linkEndZT"),  A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksEnd1ZT,             gensym("linkEnd1ZT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linksEnd2ZT,             gensym("linkEnd2ZT"), A_GIMME, 0);
    
// pmpd3d_test
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_testMass,       			gensym("testMass"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_testLink,       			gensym("testLink"), A_GIMME, 0); 
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_testMassT,       		gensym("testMassT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_testLinkT,       		gensym("testLinkT"), A_GIMME, 0); 
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_testMassT,       		gensym("testMassL"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_testLinkT,       		gensym("testLinkL"), A_GIMME, 0); 
    
// pmpd3d_stat
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosMean,       	gensym("massesPosMean"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosStd,        	gensym("massesPosStd"),A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesMean,    	gensym("massesForecesMean"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesStd,     	gensym("massesForcesStd"),A_GIMME, 0);    
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsMean,    	gensym("massesSpeedsMean"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsStd,     	gensym("massesSpeedsStd"),A_GIMME, 0);

    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosMean,       	gensym("massPosMean"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesPosStd,        	gensym("massPosStd"),A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesMean,    	gensym("massForecesMean"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesForcesStd,     	gensym("massForcesStd"),A_GIMME, 0);    
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsMean,    	gensym("massSpeedsMean"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesSpeedsStd,     	gensym("massSpeedsStd"),A_GIMME, 0);

    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linkPosMean,				gensym("linkPosMean"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linkLengthMean,         	gensym("linkLengthMean"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linkPosSpeedMean,       	gensym("linkPosSpeedMean"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linkLengthSpeedMean,    	gensym("linkLengthSpeedMean"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linkPosStd,             	gensym("linkPosStd"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linkLengthStd,          	gensym("linkLengthStd"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linkPosSpeedStd,        	gensym("linkPosSpeedStd"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_linkLengthSpeedStd,		gensym("linkLengthSpeedStd"), A_GIMME, 0);

// pmpd3d_interactor
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_iCylinder,				gensym("iCylinder"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_iCylinder,				gensym("iPlane"), A_GIMME, 0);

// pmpd3d_various
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_infosL,          		gensym("infosL"), 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_infosL,          		gensym("print"), 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_force,           		gensym("force"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_forceX,          		gensym("forceX"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_forceY,          		gensym("forceY"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_forceZ,          		gensym("forceZ"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_min,             		gensym("min"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_max,             		gensym("max"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_minX,            		gensym("Xmin"), A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_maxX,            		gensym("Xmax"), A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_minY,            		gensym("Ymin"), A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_maxY,            		gensym("Ymax"), A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_minZ,            		gensym("Zmin"), A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_maxZ,            		gensym("Zmax"), A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_addPos,          		gensym("addPos"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_addPosX,         		gensym("addPosX"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_addPosY,         		gensym("addPosY"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_addPosZ,         		gensym("addPosZ"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_grabMass,        		gensym("grabMass"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_closestMass,     		gensym("closestMass"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_massesDistances,     	gensym("massDistance"), A_GIMME, 0);
    
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_forcesXT,       			gensym("forceXT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_forcesYT,       			gensym("forceYT"), A_GIMME, 0);
    class_addmethod(pmpd3d_class, (t_method)pmpd3d_forcesZT,       			gensym("forceZT"), A_GIMME, 0);
  
}

