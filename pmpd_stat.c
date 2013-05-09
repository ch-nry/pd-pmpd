void pmpd_massPosMean(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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
    outlet_anything(x->main_outlet, gensym("massPosMean"),1 , mean);
}

void pmpd_massPosStd(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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
                stdX += sqr(x->mass[i].posX-sommeX);
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
            stdX += sqr(x->mass[i].posX-sommeX);
        }
    }    
    
    stdX = sqrt(stdX/j);
    SETFLOAT(&(std_out[0]),stdX);
    outlet_anything(x->main_outlet, gensym("massPosStd"),1 , std_out);
}

void pmpd_massForceMean(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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
    outlet_anything(x->main_outlet, gensym("massForceMean"),1 , mean);
}

void pmpd_massForceStd(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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
                stdX += sqr(x->mass[i].forceX-sommeX);
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
            stdX += sqr(x->mass[i].forceX-sommeX);
        }
    }    
    
    stdX = sqrt(stdX/j);

    SETFLOAT(&(std_out[0]),stdX);
    
    outlet_anything(x->main_outlet, gensym("massForceStd"),1 , std_out);
}

void pmpd_massSpeedMean(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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
    
    outlet_anything(x->main_outlet, gensym("massSpeedMean"),1 , mean);
}

void pmpd_massSpeedStd(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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
                stdX += sqr(x->mass[i].speedX-sommeX);
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
            stdX += sqr(x->mass[i].speedX-sommeX);
        }
    }    
    
    stdX = sqrt(stdX/j);
    SETFLOAT(&(std_out[0]),stdX);
    outlet_anything(x->main_outlet, gensym("massSpeedStd"),1 , std_out);
}

//-----------------------------

void pmpd_linkPosMean(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float sommeX;
    t_int i,j;
    t_atom mean[1];

    sommeX = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                sommeX += (x->link[i].mass1->posX + x->link[i].mass2->posX)/2;
                j++;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
                sommeX += (x->link[i].mass1->posX + x->link[i].mass2->posX)/2;
                j++;
        }
    }    
    
    if ( j> 0)
	{
		sommeX /= j;
    }
    
    SETFLOAT(&(mean[0]),sommeX);   
    outlet_anything(x->main_outlet, gensym("linkPosMean"),1 , mean);
}

void pmpd_linkLengthMean(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float sommeX, somme;
    t_int i,j;
    t_atom mean[2];

    sommeX = 0;
    somme = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                sommeX += x->link[i].mass1->posX - x->link[i].mass2->posX;
                somme  += x->link[i].distance;
                j+=1;
            }
        }
    }
    else if (argc == 0)
    {
        for (i=0; i< x->nb_link; i++)
        {
            sommeX += x->link[i].mass1->posX - x->link[i].mass2->posX;
            somme  += x->link[i].distance;
            j+=1;
        }
    }    
    
    if (j>0)
    {
		sommeX /= j;
		somme  /= j;    
    }
    
    SETFLOAT(&(mean[0]),sommeX);
    SETFLOAT(&(mean[1]),somme);
    outlet_anything(x->main_outlet, gensym("linkLengthMean"),2 , mean);
}

void pmpd_linkPosSpeedMean(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float sommeX;
    t_int i,j;
    t_atom mean[1];

    sommeX = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                sommeX += (x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2;
                j++;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
            sommeX += (x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2;
            j++;
        }
    }    
    
    if (j>0)
    {
		sommeX /= j;
    }   
    
    SETFLOAT(&(mean[0]),sommeX);    
    outlet_anything(x->main_outlet, gensym("linkPosSpeedMean"),1 , mean);
}

void pmpd_linkLengthSpeedMean(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float sommeX;
    t_int i,j;
    t_atom mean[1];

    sommeX = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                sommeX += fabs(x->link[i].mass1->speedX - x->link[i].mass2->speedX);
				j++;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
			sommeX += fabs(x->link[i].mass1->speedX - x->link[i].mass2->speedX);
			j++;
        }
    }    
    
    if (j>0)
    {
		sommeX /= j;   
    }    
    
    SETFLOAT(&(mean[0]),sommeX);
    outlet_anything(x->main_outlet, gensym("linkLengthSpeedMean"),0 , mean);
}

void pmpd_linkPosStd(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                sommeX += (x->link[i].mass1->posX + x->link[i].mass2->posX)/2;
                j++;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
            sommeX += (x->link[i].mass1->posX + x->link[i].mass2->posX)/2;
            j++;
        }
    }    
    
    if (j>0)
    {
		sommeX /= j;
    }    
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                stdX += sqr((x->link[i].mass1->posX + x->link[i].mass2->posX)/2 - sommeX);
                j+=1;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
            stdX += sqr((x->link[i].mass1->posX + x->link[i].mass2->posX)/2 - sommeX);
            j+=1;
        }
    }        
    
    if ( j > 0)
    {
		stdX = sqrt(stdX/j);
	}
	
    SETFLOAT(&(std_out[0]),stdX);
    outlet_anything(x->main_outlet, gensym("linkPosStd"),1 , std_out);
}

void pmpd_linkLengthStd(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float sommeX, somme;
    t_int i,j;
    t_float stdX, std;
    t_atom std_out[2];

    sommeX = 0;
    somme = 0;
    stdX = 0;
    std  = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                sommeX += x->link[i].mass1->posX - x->link[i].mass2->posX;
                somme  += x->link[i].distance;
                j+=1;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
            sommeX += x->link[i].mass1->posX - x->link[i].mass2->posX;
            somme  += x->link[i].distance;
            j+=1;
        }
    }    
    
    if ( j> 0)
	{
		sommeX /= j;
		somme  /= j;    
    }
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                stdX += sqr((x->link[i].mass1->posX - x->link[i].mass2->posX)-sommeX);
                std  += sqr(x->link[i].distance - somme);
                j+=1;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
            stdX += sqr((x->link[i].mass1->posX - x->link[i].mass2->posX) - sommeX);
            std  += sqr(x->link[i].distance - somme);
            j+=1;
        }
    }   
     
    if ( j > 0)
    {
		stdX = sqrt(stdX/j);
	}
  
    SETFLOAT(&(std_out[0]),stdX);
    SETFLOAT(&(std_out[1]),std);
    outlet_anything(x->main_outlet, gensym("linkLengthStd"),2 , std_out);
}

void pmpd_linkPosSpeedStd(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float sommeX;
	t_float stdX;
    t_int i,j;
    t_atom std_out[1];

    sommeX = 0;
    stdX = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                sommeX += (x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2;
                j++;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
                sommeX += (x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2;
                j++;
        }
    }    
    
    if ( j> 0)
	{
		sommeX /= j;   
    }  

    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                stdX += sqr((x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2-sommeX);
                j++;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
            stdX += sqr((x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2-sommeX);
            j++;
        }
    }     
    
    if ( j > 0)
    {
		stdX = sqrt(stdX/j);   
	}

    SETFLOAT(&(std_out[0]),stdX);
    outlet_anything(x->main_outlet, gensym("linkPosSpeedStd"),1 , std_out);
}

void pmpd_linkLengthSpeedStd(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float sommeX;
    t_float stdX;
    t_int i,j;
    t_atom std_out[1];

    sommeX = 0;
    stdX = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                sommeX += fabs(x->link[i].mass1->speedX - x->link[i].mass2->speedX);
				j++;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
			sommeX += fabs(x->link[i].mass1->speedX - x->link[i].mass2->speedX);
			j++;
        }
    }    
    
    if ( j> 0)
	{
		sommeX /= j;  
    }    

    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                stdX += sqr(fabs(x->link[i].mass1->speedX - x->link[i].mass2->speedX) - sommeX);
				j++;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
            stdX += sqr(fabs(x->link[i].mass1->speedX - x->link[i].mass2->speedX) - sommeX);
			j++;
        }
    }

    if ( j > 0)
    {
		stdX = sqrt(stdX/j);
    
	}
    
    SETFLOAT(&(std_out[0]),stdX);
    outlet_anything(x->main_outlet, gensym("linkLengthSpeedStd"),1 , std_out);
}
