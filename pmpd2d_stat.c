void pmpd2d_massPosMean(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, somme;
    t_int i,j;
    t_atom mean[3];

    sommeX = 0;
    sommeY = 0;
    somme = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            { 
                sommeX += x->mass[i].posX;
                sommeY += x->mass[i].posY;
                somme +=  sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY)); // distance au centre
                j++;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_mass; i++)
        {
                sommeX += x->mass[i].posX;
                sommeY += x->mass[i].posY;
                somme +=  sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY)); // distance au centre
                j++;
        }
    }    
    
    sommeX /= j;
    sommeY /= j;
    somme  /= j;    
    
    SETFLOAT(&(mean[0]),sommeX);
    SETFLOAT(&(mean[1]),sommeY);
    SETFLOAT(&(mean[2]),somme);
    
    outlet_anything(x->main_outlet, gensym("massPosMean"),3 , mean);
}

void pmpd2d_massPosStd(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, somme;
    t_int i,j;
    t_float stdX, stdY,std;
    t_atom std_out[3];

    sommeX = 0;
    sommeY = 0;
    somme  = 0;
    stdX = 0;
    stdY = 0;
    std  = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            { 
                sommeX += x->mass[i].posX;
                sommeY += x->mass[i].posY;
                somme +=  sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY)); // distance au centre
                j++;
            }
        }
        sommeX /= j;
        sommeY /= j;
        somme /= j;
        for (i=0; i< x->nb_mass; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            { 
                stdX += sqr(x->mass[i].posX-sommeX);
                stdY += sqr(x->mass[i].posY-sommeY);
                std  +=  sqr(sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY))-somme);
            }
        }        
    }
    else
    {
        for (i=0; i< x->nb_mass; i++)
        {
            sommeX += x->mass[i].posX;
            sommeY += x->mass[i].posY;
            somme +=  sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY)); // distance au centre
            j++;
        }
        sommeX /= j;
        sommeY /= j;
        somme /= j;
        for (i=0; i< x->nb_mass; i++)
        {
            stdX += sqr(x->mass[i].posX-sommeX);
            stdY += sqr(x->mass[i].posY-sommeY);
            std  += sqr(sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY))-somme);
        }
    }    
    
    stdX = sqrt(stdX/j);
    stdY = sqrt(stdY/j);
    std  = sqrt(std /j);    

    SETFLOAT(&(std_out[0]),stdX);
    SETFLOAT(&(std_out[1]),stdY);
    SETFLOAT(&(std_out[2]),std);
    
    outlet_anything(x->main_outlet, gensym("massPosStd"),3 , std_out);
}

void pmpd2d_massForcesMean(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, somme;
    t_int i,j;
    t_atom mean[3];

    sommeX = 0;
    sommeY = 0;
    somme = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            { 
                sommeX += x->mass[i].forceX;
                sommeY += x->mass[i].forceY;
                somme +=  sqrt(sqr(x->mass[i].forceX) + sqr(x->mass[i].forceY)); // distance au centre
                j++;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_mass; i++)
        {
                sommeX += x->mass[i].forceX;
                sommeY += x->mass[i].forceY;
                somme +=  sqrt(sqr(x->mass[i].forceX) + sqr(x->mass[i].forceY)); // distance au centre
                j++;
        }
    }    
    
    sommeX /= j;
    sommeY /= j;
    somme  /= j;    
    
    SETFLOAT(&(mean[0]),sommeX);
    SETFLOAT(&(mean[1]),sommeY);
    SETFLOAT(&(mean[2]),somme);
    
    outlet_anything(x->main_outlet, gensym("massForcesMean"),3 , mean);
}

void pmpd2d_massForcesStd(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, somme;
    t_int i,j;
    t_float stdX, stdY,std;
    t_atom std_out[3];

    sommeX = 0;
    sommeY = 0;
    somme  = 0;
    stdX = 0;
    stdY = 0;
    std  = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            { 
                sommeX += x->mass[i].forceX;
                sommeY += x->mass[i].forceY;
                somme +=  sqrt(sqr(x->mass[i].forceX) + sqr(x->mass[i].forceY)); // distance au centre
                j++;
            }
        }
        sommeX /= j;
        sommeY /= j;
        somme /= j;
        for (i=0; i< x->nb_mass; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            { 
                stdX += sqr(x->mass[i].forceX-sommeX);
                stdY += sqr(x->mass[i].forceY-sommeY);
                std  +=  sqr(sqrt(sqr(x->mass[i].forceX) + sqr(x->mass[i].forceY))-somme);
            }
        }        
    }
    else
    {
        for (i=0; i< x->nb_mass; i++)
        {
            sommeX += x->mass[i].forceX;
            sommeY += x->mass[i].forceY;
            somme +=  sqrt(sqr(x->mass[i].forceX) + sqr(x->mass[i].forceY)); // distance au centre
            j++;
        }
        sommeX /= j;
        sommeY /= j;
        somme /= j;
        for (i=0; i< x->nb_mass; i++)
        {
            stdX += sqr(x->mass[i].forceX-sommeX);
            stdY += sqr(x->mass[i].forceY-sommeY);
            std  += sqr(sqrt(sqr(x->mass[i].forceX) + sqr(x->mass[i].forceY))-somme);
        }
    }    
    
    stdX = sqrt(stdX/j);
    stdY = sqrt(stdY/j);
    std  = sqrt(std /j);    

    SETFLOAT(&(std_out[0]),stdX);
    SETFLOAT(&(std_out[1]),stdY);
    SETFLOAT(&(std_out[2]),std);
    
    outlet_anything(x->main_outlet, gensym("massForcesStd"),3 , std_out);
}

void pmpd2d_massSpeedsMean(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, somme;
    t_int i,j;
    t_atom mean[3];

    sommeX = 0;
    sommeY = 0;
    somme = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            { 
                sommeX += x->mass[i].speedX;
                sommeY += x->mass[i].speedY;
                somme +=  sqrt(sqr(x->mass[i].speedX) + sqr(x->mass[i].speedY)); // distance au centre
                j++;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_mass; i++)
        {
                sommeX += x->mass[i].speedX;
                sommeY += x->mass[i].speedY;
                somme +=  sqrt(sqr(x->mass[i].speedX) + sqr(x->mass[i].speedY)); // distance au centre
                j++;
        }
    }    
    
    sommeX /= j;
    sommeY /= j;
    somme  /= j;    
    
    SETFLOAT(&(mean[0]),sommeX);
    SETFLOAT(&(mean[1]),sommeY);
    SETFLOAT(&(mean[2]),somme);
    
    outlet_anything(x->main_outlet, gensym("massSpeedsMean"),3 , mean);
}

void pmpd2d_massSpeedsStd(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, somme;
    t_int i,j;
    t_float stdX, stdY,std;
    t_atom std_out[3];

    sommeX = 0;
    sommeY = 0;
    somme  = 0;
    stdX = 0;
    stdY = 0;
    std  = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            { 
                sommeX += x->mass[i].speedX;
                sommeY += x->mass[i].speedY;
                somme +=  sqrt(sqr(x->mass[i].speedX) + sqr(x->mass[i].speedY)); // distance au centre
                j++;
            }
        }
        sommeX /= j;
        sommeY /= j;
        somme /= j;
        for (i=0; i< x->nb_mass; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            { 
                stdX += sqr(x->mass[i].speedX-sommeX);
                stdY += sqr(x->mass[i].speedY-sommeY);
                std  +=  sqr(sqrt(sqr(x->mass[i].speedX) + sqr(x->mass[i].speedY))-somme);
            }
        }        
    }
    else
    {
        for (i=0; i< x->nb_mass; i++)
        {
            sommeX += x->mass[i].speedX;
            sommeY += x->mass[i].speedY;
            somme +=  sqrt(sqr(x->mass[i].speedX) + sqr(x->mass[i].speedY)); // distance au centre
            j++;
        }
        sommeX /= j;
        sommeY /= j;
        somme /= j;
        for (i=0; i< x->nb_mass; i++)
        {
            stdX += sqr(x->mass[i].speedX-sommeX);
            stdY += sqr(x->mass[i].speedY-sommeY);
            std  += sqr(sqrt(sqr(x->mass[i].speedX) + sqr(x->mass[i].speedY))-somme);
        }
    }    
    
    stdX = sqrt(stdX/j);
    stdY = sqrt(stdY/j);
    std  = sqrt(std /j);    

    SETFLOAT(&(std_out[0]),stdX);
    SETFLOAT(&(std_out[1]),stdY);
    SETFLOAT(&(std_out[2]),std);
    
    outlet_anything(x->main_outlet, gensym("massSpeedsStd"),3 , std_out);
}

void pmpd2d_linkPosMean(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float sommeX, sommeY, somme;
    t_int i,j;
    t_atom mean[3];

    sommeX = 0;
    sommeY = 0;
    somme = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                sommeX += (x->link[i].mass1->posX + x->link[i].mass2->posX)/2;
                sommeY += (x->link[i].mass1->posY + x->link[i].mass2->posY)/2;
                somme  += sqrt(sqr((x->link[i].mass1->posX + x->link[i].mass2->posX)/2) + sqr((x->link[i].mass1->posY + x->link[i].mass2->posY)/2) );
                j++;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
                sommeX += (x->link[i].mass1->posX + x->link[i].mass2->posX)/2;
                sommeY += (x->link[i].mass1->posY + x->link[i].mass2->posY)/2;
                somme  += sqrt(sqr((x->link[i].mass1->posX + x->link[i].mass2->posX)/2) + sqr((x->link[i].mass1->posY + x->link[i].mass2->posY)/2) );
                j++;
        }
    }    
    
    if ( j> 0)
	{
		sommeX /= j;
		sommeY /= j;
		somme  /= j;    
    }
    
    SETFLOAT(&(mean[0]),sommeX);
    SETFLOAT(&(mean[1]),sommeY);
    SETFLOAT(&(mean[2]),somme);
    
    outlet_anything(x->main_outlet, gensym("linkPosMean"),3 , mean);
}

void pmpd2d_linkLengthMean(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float sommeX, sommeY, somme;
    t_int i,j;
    t_atom mean[3];

    sommeX = 0;
    sommeY = 0;
    somme = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                sommeX += fabs(x->link[i].mass1->posX - x->link[i].mass2->posX);
                sommeY += fabs(x->link[i].mass1->posY - x->link[i].mass2->posY);
                somme  += x->link[i].distance;
                j+=1;
            }
        }
    }
    else if (argc == 0)
    {
        for (i=0; i< x->nb_link; i++)
        {
            sommeX += fabs(x->link[i].mass1->posX - x->link[i].mass2->posX);
            sommeY += fabs(x->link[i].mass1->posY - x->link[i].mass2->posY);
            somme  += x->link[i].distance;
            j+=1;
        }
    }    
    
    if (j>0)
    {
		sommeX /= j;
		sommeY /= j;
		somme  /= j;    
    }
    
    SETFLOAT(&(mean[0]),sommeX);
    SETFLOAT(&(mean[1]),sommeY);
    SETFLOAT(&(mean[2]),somme);
    
    outlet_anything(x->main_outlet, gensym("linkLengthMean"),3 , mean);
}

void pmpd2d_linkPosSpeedMean(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float sommeX, sommeY, somme;
    t_int i,j;
    t_atom mean[3];

    sommeX = 0;
    sommeY = 0;
    somme = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                sommeX += (x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2;
                sommeY += (x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2;
                somme  += sqrt(sqr((x->link[i].mass1->speedX+x->link[i].mass2->speedX)/2) + sqr((x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2));
                j++;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
            sommeX += (x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2;
            sommeY += (x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2;
            somme  += sqrt(sqr((x->link[i].mass1->speedX+x->link[i].mass2->speedX)/2) + sqr((x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2));
            j++;
        }
    }    
    
    if (j>0)
    {
		sommeX /= j;
		sommeY /= j;
		somme  /= j;    
    }   
    
    SETFLOAT(&(mean[0]),sommeX);
    SETFLOAT(&(mean[1]),sommeY);
    SETFLOAT(&(mean[2]),somme);
    
    outlet_anything(x->main_outlet, gensym("linkPosSpeedMean"),3 , mean);
}

void pmpd2d_linkLengthSpeedMean(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float sommeX, sommeY, somme;
    t_int i,j;
    t_atom mean[3];

    sommeX = 0;
    sommeY = 0;
    somme = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                sommeX += fabs(x->link[i].mass1->speedX - x->link[i].mass2->speedX);
                sommeY += fabs(x->link[i].mass1->speedY - x->link[i].mass2->speedY);
                somme  += sqrt(sqr(x->link[i].mass1->speedX - x->link[i].mass2->speedX) +  
							sqr(x->link[i].mass1->speedY - x->link[i].mass2->speedY) );
				j++;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
			sommeX += fabs(x->link[i].mass1->speedX - x->link[i].mass2->speedX);
			sommeY += fabs(x->link[i].mass1->speedY - x->link[i].mass2->speedY);
			somme  += sqrt(sqr(x->link[i].mass1->speedX - x->link[i].mass2->speedX) +  
						sqr(x->link[i].mass1->speedY - x->link[i].mass2->speedY) );
			j++;
        }
    }    
    
    if (j>0)
    {
		sommeX /= j;
		sommeY /= j;
		somme  /= j;    
    }    
    
    SETFLOAT(&(mean[0]),sommeX);
    SETFLOAT(&(mean[1]),sommeY);
    SETFLOAT(&(mean[2]),somme);
    
    outlet_anything(x->main_outlet, gensym("linkLengthSpeedMean"),3 , mean);
}

void pmpd2d_linkPosStd(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, somme;
    t_int i,j;
    t_float stdX, stdY, std;
    t_atom std_out[3];

    sommeX = 0;
    sommeY = 0;
    somme = 0;
    stdX = 0;
    stdY = 0;
    std  = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                sommeX += (x->link[i].mass1->posX + x->link[i].mass2->posX)/2;
                sommeY += (x->link[i].mass1->posY + x->link[i].mass2->posY)/2;
                somme  += sqrt(sqr((x->link[i].mass1->posX + x->link[i].mass2->posX)/2) + sqr((x->link[i].mass1->posY + x->link[i].mass2->posY)/2));
                j++;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
            sommeX += (x->link[i].mass1->posX + x->link[i].mass2->posX)/2;
            sommeY += (x->link[i].mass1->posY + x->link[i].mass2->posY)/2;
            somme  += sqrt(sqr((x->link[i].mass1->posX + x->link[i].mass2->posX)/2) + sqr((x->link[i].mass1->posY + x->link[i].mass2->posY)/2));
            j++;
        }
    }    
    
    if (j>0)
    {
		sommeX /= j;
		sommeY /= j;
		somme  /= j;    
    }    
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                stdX += sqr((x->link[i].mass1->posX + x->link[i].mass2->posX)/2 - sommeX);
                stdY += sqr((x->link[i].mass1->posY + x->link[i].mass2->posY)/2 - sommeY);
                std  += sqr(sqrt(sqr((x->link[i].mass1->posX + x->link[i].mass2->posX)/2) + sqr((x->link[i].mass1->posY + x->link[i].mass2->posY)/2)) - somme);
                j+=1;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
            stdX += sqr((x->link[i].mass1->posX + x->link[i].mass2->posX)/2 - sommeX);
            stdY += sqr((x->link[i].mass1->posY + x->link[i].mass2->posY)/2 - sommeY);
            std  += sqr(sqrt(sqr((x->link[i].mass1->posX + x->link[i].mass2->posX)/2) + sqr((x->link[i].mass1->posY + x->link[i].mass2->posY)/2)) - somme);
            j+=1;
        }
    }        
    
    if ( j > 0)
    {
		stdX = sqrt(stdX/j);
		stdY = sqrt(stdY/j);
		std  = sqrt(std /j);    
	}
	
    SETFLOAT(&(std_out[0]),stdX);
    SETFLOAT(&(std_out[1]),stdY);
    SETFLOAT(&(std_out[2]),std);
    
    outlet_anything(x->main_outlet, gensym("linkPosStd"),3 , std_out);
}

void pmpd2d_linkLengthStd(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float sommeX, sommeY, somme;
    t_int i,j;
    t_float stdX, stdY, std;
    t_atom std_out[4];

    sommeX = 0;
    sommeY = 0;
    somme = 0;
    stdX = 0;
    stdY = 0;
    std  = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                sommeX += fabs(x->link[i].mass1->posX - x->link[i].mass2->posX);
                sommeY += fabs(x->link[i].mass1->posY - x->link[i].mass2->posY);
                somme  += x->link[i].distance;
                j+=1;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
            sommeX += fabs(x->link[i].mass1->posX - x->link[i].mass2->posX);
            sommeY += fabs(x->link[i].mass1->posY - x->link[i].mass2->posY);
            somme  += x->link[i].distance;
            j+=1;
        }
    }    
    
    if ( j> 0)
	{
		sommeX /= j;
		sommeY /= j;
		somme  /= j;    
    }
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                stdX += sqr(fabs(x->link[i].mass1->posX - x->link[i].mass2->posX)-sommeX);
                stdY += sqr(fabs(x->link[i].mass1->posY - x->link[i].mass2->posY)-sommeY);
                std  += sqr(x->link[i].distance - somme);
                j+=1;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
            stdX += sqr(fabs(x->link[i].mass1->posX - x->link[i].mass2->posX) - sommeX);
            stdY += sqr(fabs(x->link[i].mass1->posY - x->link[i].mass2->posY) - sommeY);
            std  += sqr(x->link[i].distance - somme);
            j+=1;
        }
    }   
     
    if ( j > 0)
    {
		stdX = sqrt(stdX/j);
		stdY = sqrt(stdY/j);
		std  = sqrt(std /j);    
	}
  

    SETFLOAT(&(std_out[0]),stdX);
    SETFLOAT(&(std_out[1]),stdY);
    SETFLOAT(&(std_out[2]),std);
    
    outlet_anything(x->main_outlet, gensym("linkLengthStd"),3 , std_out);
}

void pmpd2d_linkPosSpeedStd(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float sommeX, sommeY, somme;
	t_float stdX, stdY, std;
    t_int i,j;
    t_atom std_out[3];

    sommeX = 0;
    sommeY = 0;
    somme = 0;
    stdX = 0;
    stdY = 0;
    std  = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                sommeX += (x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2;
                sommeY += (x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2;
                somme  += sqrt(sqr((x->link[i].mass1->speedX+x->link[i].mass2->speedX)/2) + sqr((x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2) );
                j++;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
                sommeX += (x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2;
                sommeY += (x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2;
                somme  += sqrt(sqr((x->link[i].mass1->speedX+x->link[i].mass2->speedX)/2) + sqr((x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2) );
                j++;
        }
    }    
    
    if ( j> 0)
	{
		sommeX /= j;
		sommeY /= j;
		somme  /= j;    
    }  

    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                stdX += sqr((x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2-sommeX);
                stdY += sqr((x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2-sommeY);
                std  +=  sqr(sqrt(sqr((x->link[i].mass1->speedX+x->link[i].mass2->speedX)/2) + sqr((x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2) ) - somme);
                j++;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
            stdX += sqr((x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2-sommeX);
            stdY += sqr((x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2-sommeY);
            std  +=  sqr(sqrt(sqr((x->link[i].mass1->speedX+x->link[i].mass2->speedX)/2) + sqr((x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2) ) - somme);
            j++;
        }
    }     
    
    if ( j > 0)
    {
		stdX = sqrt(stdX/j);
		stdY = sqrt(stdY/j);
		std  = sqrt(std /j);    
	}
 

    SETFLOAT(&(std_out[0]),stdX);
    SETFLOAT(&(std_out[1]),stdY);
    SETFLOAT(&(std_out[2]),std);
    
    outlet_anything(x->main_outlet, gensym("linkPosSpeedStd"),3 , std_out);
}

void pmpd2d_linkLengthSpeedStd(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float sommeX, sommeY, somme;
    t_float stdX, stdY, std;
    t_int i,j;
    t_atom std_out[4];

    sommeX = 0;
    sommeY = 0;
    somme = 0;
    stdX = 0;
    stdY = 0;
    std  = 0;
    j = 0;
    
    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                sommeX += fabs(x->link[i].mass1->speedX - x->link[i].mass2->speedX);
                sommeY += fabs(x->link[i].mass1->speedY - x->link[i].mass2->speedY);
                somme  += sqrt(sqr(x->link[i].mass1->speedX - x->link[i].mass2->speedX) +  
							sqr(x->link[i].mass1->speedY - x->link[i].mass2->speedY));
				j++;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
			sommeX += fabs(x->link[i].mass1->speedX - x->link[i].mass2->speedX);
			sommeY += fabs(x->link[i].mass1->speedY - x->link[i].mass2->speedY);
			somme  += sqrt(sqr(x->link[i].mass1->speedX - x->link[i].mass2->speedX) +  
						sqr(x->link[i].mass1->speedY - x->link[i].mass2->speedY));
			j++;
        }
    }    
    
    if ( j> 0)
	{
		sommeX /= j;
		sommeY /= j;
		somme  /= j;    
    }    

    if ( (argc >= 1) && (argv[0].a_type == A_SYMBOL) ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            { 
                stdX += sqr(fabs(x->link[i].mass1->speedX - x->link[i].mass2->speedX) - sommeX);
                stdY += sqr(fabs(x->link[i].mass1->speedY - x->link[i].mass2->speedY) - sommeY);
                std  += sqr( sqrt(sqr(x->link[i].mass1->speedX - x->link[i].mass2->speedX) +  
							sqr(x->link[i].mass1->speedY - x->link[i].mass2->speedY)) - somme);
				j++;
            }
        }
    }
    else
    {
        for (i=0; i< x->nb_link; i++)
        {
            stdX += sqr(fabs(x->link[i].mass1->speedX - x->link[i].mass2->speedX) - sommeX);
            stdY += sqr(fabs(x->link[i].mass1->speedY - x->link[i].mass2->speedY) - sommeY);
            std  += sqr( sqrt(sqr(x->link[i].mass1->speedX - x->link[i].mass2->speedX) +  
							sqr(x->link[i].mass1->speedY - x->link[i].mass2->speedY) ) - somme);
			j++;
        }
    }

    if ( j > 0)
    {
		stdX = sqrt(stdX/j);
		stdY = sqrt(stdY/j);
		std  = sqrt(std /j);    
	}
    
    SETFLOAT(&(std_out[0]),stdX);
    SETFLOAT(&(std_out[1]),stdY);
    SETFLOAT(&(std_out[2]),std);
    
    outlet_anything(x->main_outlet, gensym("linkLengthSpeedStd"),3 , std_out);
}

