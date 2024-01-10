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


void pmpd_massesPosL(t_pmpd *x)
{
    int i;
    t_atom* pos_list = (t_atom*)malloc(x->nb_mass * sizeof(t_atom));

    for (i=0; i < x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].posX);
    }
    outlet_anything(x->main_outlet, gensym("massesPosL"),x->nb_mass , pos_list);

    free(pos_list);
}

void pmpd_massesForcesL(t_pmpd *x)
{
    int i;
    t_atom* pos_list = (t_atom*)malloc(x->nb_mass * sizeof(t_atom));

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].forceX);
    }
    outlet_anything(x->main_outlet, gensym("massesForcesL"),x->nb_mass , pos_list);

    free(pos_list);
}

void pmpd_massesSpeedsL(t_pmpd *x)
{
    int i;
    t_atom* pos_list = (t_atom*)malloc(x->nb_mass * sizeof(t_atom));

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].speedX);
    }
    outlet_anything(x->main_outlet, gensym("massesSpeedsL"),x->nb_mass , pos_list);

    free(pos_list);
}

// --------------------------------------------

void pmpd_linksPosL(t_pmpd *x)
{
    int i;
    t_atom* pos_list = (t_atom*)malloc(x->nb_link * sizeof(t_atom));

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),(x->link[i].mass1->posX + x->link[i].mass2->posX)/2);
    }
    outlet_anything(x->main_outlet, gensym("linksPosXL"),x->nb_link , pos_list);

    free(pos_list);
}

void pmpd_linksLengthL(t_pmpd *x)
{
    int i;
    t_atom* pos_list = (t_atom*)malloc(x->nb_link * sizeof(t_atom));

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),x->link[i].mass2->posX - x->link[i].mass1->posX);
    }
    outlet_anything(x->main_outlet, gensym("linksLengthXL"),x->nb_link , pos_list);

    free(pos_list);
}

void pmpd_linksPosSpeedL(t_pmpd *x)
{
    int i;
    t_atom* pos_list = (t_atom*)malloc(x->nb_link * sizeof(t_atom));

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),(x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2);
    }
    outlet_anything(x->main_outlet, gensym("linksPosSpeedXL"),x->nb_link , pos_list);

    free(pos_list);
}

void pmpd_linksLengthSpeedL(t_pmpd *x)
{
    int i;
    t_atom* pos_list = (t_atom*)malloc(x->nb_link * sizeof(t_atom));

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),x->link[i].mass2->speedX - x->link[i].mass1->speedX);
    }
    outlet_anything(x->main_outlet, gensym("linksLengthSpeedXL"),x->nb_link , pos_list);

    free(pos_list);
}

// --------------------------------------------

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
                stdX += pmpd_sqr(x->mass[i].posX-sommeX);
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
            stdX += pmpd_sqr(x->mass[i].posX-sommeX);
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
                stdX += pmpd_sqr(x->mass[i].forceX-sommeX);
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
            stdX += pmpd_sqr(x->mass[i].forceX-sommeX);
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
                stdX += pmpd_sqr(x->mass[i].speedX-sommeX);
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
            stdX += pmpd_sqr(x->mass[i].speedX-sommeX);
        }
    }    
    
    stdX = sqrt(stdX/j);

    SETFLOAT(&(std_out[0]),stdX);
    
    outlet_anything(x->main_outlet, gensym("massesSpeedsStd"),1 , std_out);
}

