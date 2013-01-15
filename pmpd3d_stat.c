void pmpd3d_massesPosMean(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, sommeZ, somme;
    t_int i,j;
    t_atom mean[4];

    sommeX = 0;
    sommeY = 0;
    sommeZ = 0;
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
                sommeZ += x->mass[i].posZ;
                somme +=  sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY) + sqr(x->mass[i].posZ)); // distance au centre
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
                sommeZ += x->mass[i].posZ;
                somme +=  sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY) + sqr(x->mass[i].posZ)); // distance au centre
                j++;
        }
    }    
    
    sommeX /= j;
    sommeY /= j;
    sommeZ /= j;
    somme  /= j;    
    
    SETFLOAT(&(mean[0]),sommeX);
    SETFLOAT(&(mean[1]),sommeY);
    SETFLOAT(&(mean[2]),sommeZ);
    SETFLOAT(&(mean[3]),somme);
    
    outlet_anything(x->main_outlet, gensym("massesPosMean"),4 , mean);
}

void pmpd3d_massesPosStd(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, sommeZ, somme;
    t_int i,j;
    t_float stdX, stdY, stdZ, std;
    t_atom std_out[4];

    sommeX = 0;
    sommeY = 0;
    sommeZ = 0;
    somme  = 0;
    stdX = 0;
    stdY = 0;
    stdZ = 0;
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
                sommeZ += x->mass[i].posZ;
                somme +=  sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY) + sqr(x->mass[i].posZ)); // distance au centre
                j++;
            }
        }
        sommeX /= j;
        sommeY /= j;
        sommeZ /= j;
        somme /= j;
        for (i=0; i< x->nb_mass; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            { 
                stdX += sqr(x->mass[i].posX-sommeX);
                stdY += sqr(x->mass[i].posY-sommeY);
                stdZ += sqr(x->mass[i].posZ-sommeZ);
                std  +=  sqr(sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY) + sqr(x->mass[i].posZ))-somme);
            }
        }        
    }
    else
    {
        for (i=0; i< x->nb_mass; i++)
        {
            sommeX += x->mass[i].posX;
            sommeY += x->mass[i].posY;
            sommeZ += x->mass[i].posZ;
            somme +=  sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY) + sqr(x->mass[i].posZ)); // distance au centre
            j++;
        }
        sommeX /= j;
        sommeY /= j;
        sommeZ /= j;
        somme /= j;
        for (i=0; i< x->nb_mass; i++)
        {
            stdX += sqr(x->mass[i].posX-sommeX);
            stdY += sqr(x->mass[i].posY-sommeY);
            stdZ += sqr(x->mass[i].posZ-sommeZ);
            std  += sqr(sqrt(sqr(x->mass[i].posX) + sqr(x->mass[i].posY) + sqr(x->mass[i].posZ))-somme);
        }
    }    
    
    stdX = sqrt(stdX/j);
    stdY = sqrt(stdY/j);
    stdZ = sqrt(stdZ/j);
    std  = sqrt(std /j);    

    SETFLOAT(&(std_out[0]),stdX);
    SETFLOAT(&(std_out[1]),stdY);
    SETFLOAT(&(std_out[2]),stdZ);
    SETFLOAT(&(std_out[3]),std);
    
    outlet_anything(x->main_outlet, gensym("massesPosStd"),4 , std_out);
}

void pmpd3d_massesForcesMean(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, sommeZ, somme;
    t_int i,j;
    t_atom mean[4];

    sommeX = 0;
    sommeY = 0;
    sommeZ = 0;
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
                sommeZ += x->mass[i].forceZ;
                somme +=  sqrt(sqr(x->mass[i].forceX) + sqr(x->mass[i].forceY) + sqr(x->mass[i].forceZ)); // force total
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
                sommeZ += x->mass[i].forceZ;
                somme +=  sqrt(sqr(x->mass[i].forceX) + sqr(x->mass[i].forceY) + sqr(x->mass[i].forceZ)); // force
                j++;
        }
    }    
    
    sommeX /= j;
    sommeY /= j;
    sommeZ /= j;
    somme  /= j;

    SETFLOAT(&(mean[0]),sommeX);
    SETFLOAT(&(mean[1]),sommeY);
    SETFLOAT(&(mean[2]),sommeZ);
    SETFLOAT(&(mean[3]),somme);
    
    outlet_anything(x->main_outlet, gensym("massesForcesMean"),4 , mean);
}

void pmpd3d_massesForcesStd(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, sommeZ, somme;
    t_int i,j;
    t_float stdX, stdY, stdZ, std;
    t_atom std_out[4];

    sommeX = 0;
    sommeY = 0;
    sommeZ = 0;
    somme  = 0;
    stdX = 0;
    stdY = 0;
    stdZ = 0;
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
                sommeZ += x->mass[i].forceZ;
                somme +=  sqrt(sqr(x->mass[i].forceX) + sqr(x->mass[i].forceY) + sqr(x->mass[i].forceZ)); // force
                j++;
            }
        }
        sommeX /= j;
        sommeY /= j;
        sommeZ /= j;
        somme /= j;
        for (i=0; i< x->nb_mass; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            { 
                stdX += sqr(x->mass[i].forceX-sommeX);
                stdY += sqr(x->mass[i].forceY-sommeY);
                stdZ += sqr(x->mass[i].forceZ-sommeZ);
                std  +=  sqr(sqrt(sqr(x->mass[i].forceX) + sqr(x->mass[i].forceY) + sqr(x->mass[i].forceZ))-somme);
            }
        }        
    }
    else
    {
        for (i=0; i< x->nb_mass; i++)
        {
            sommeX += x->mass[i].forceX;
            sommeY += x->mass[i].forceY;
            sommeZ += x->mass[i].forceZ;
            somme +=  sqrt(sqr(x->mass[i].forceX) + sqr(x->mass[i].forceY) + sqr(x->mass[i].forceZ)); 
            j++;
        }
        sommeX /= j;
        sommeY /= j;
        sommeZ /= j;
        somme /= j;
        for (i=0; i< x->nb_mass; i++)
        {
            stdX += sqr(x->mass[i].forceX-sommeX);
            stdY += sqr(x->mass[i].forceY-sommeY);
            stdZ += sqr(x->mass[i].forceZ-sommeZ);
            std  += sqr(sqrt(sqr(x->mass[i].forceX) + sqr(x->mass[i].forceY) + sqr(x->mass[i].forceZ))-somme);
        }
    }    
    
    stdX = sqrt(stdX/j);
    stdY = sqrt(stdY/j);
    stdZ = sqrt(stdZ/j);
    std  = sqrt(std /j);    

    SETFLOAT(&(std_out[0]),stdX);
    SETFLOAT(&(std_out[1]),stdY);
    SETFLOAT(&(std_out[2]),stdZ);
    SETFLOAT(&(std_out[3]),std);
    
    outlet_anything(x->main_outlet, gensym("massesForcesStd"),4 , std_out);
}

void pmpd3d_massesSpeedsMean(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, sommeZ, somme;
    t_int i,j;
    t_atom mean[4];

    sommeX = 0;
    sommeY = 0;
    sommeZ = 0;
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
                sommeZ += x->mass[i].speedZ;
                somme +=  sqrt(sqr(x->mass[i].speedX) + sqr(x->mass[i].speedY) + sqr(x->mass[i].speedZ)); // speed total
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
                sommeZ += x->mass[i].speedZ;
                somme +=  sqrt(sqr(x->mass[i].speedX) + sqr(x->mass[i].speedY) + sqr(x->mass[i].speedZ)); // speed
                j++;
        }
    }    
    
    sommeX /= j;
    sommeY /= j;
    sommeZ /= j;
    somme  /= j;

    SETFLOAT(&(mean[0]),sommeX);
    SETFLOAT(&(mean[1]),sommeY);
    SETFLOAT(&(mean[2]),sommeZ);
    SETFLOAT(&(mean[3]),somme);
    
    outlet_anything(x->main_outlet, gensym("massesSpeedsMean"),4 , mean);
}

void pmpd3d_massesSpeedsStd(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float sommeX, sommeY, sommeZ, somme;
    t_int i,j;
    t_float stdX, stdY, stdZ, std;
    t_atom std_out[4];

    sommeX = 0;
    sommeY = 0;
    sommeZ = 0;
    somme  = 0;
    stdX = 0;
    stdY = 0;
    stdZ = 0;
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
                sommeZ += x->mass[i].speedZ;
                somme +=  sqrt(sqr(x->mass[i].speedX) + sqr(x->mass[i].speedY) + sqr(x->mass[i].speedZ)); // speed
                j++;
            }
        }
        sommeX /= j;
        sommeY /= j;
        sommeZ /= j;
        somme /= j;
        for (i=0; i< x->nb_mass; i++)
        {
            if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            { 
                stdX += sqr(x->mass[i].speedX-sommeX);
                stdY += sqr(x->mass[i].speedY-sommeY);
                stdZ += sqr(x->mass[i].speedZ-sommeZ);
                std  +=  sqr(sqrt(sqr(x->mass[i].speedX) + sqr(x->mass[i].speedY) + sqr(x->mass[i].speedZ))-somme);
            }
        }        
    }
    else
    {
        for (i=0; i< x->nb_mass; i++)
        {
            sommeX += x->mass[i].speedX;
            sommeY += x->mass[i].speedY;
            sommeZ += x->mass[i].speedZ;
            somme +=  sqrt(sqr(x->mass[i].speedX) + sqr(x->mass[i].speedY) + sqr(x->mass[i].speedZ)); 
            j++;
        }
        sommeX /= j;
        sommeY /= j;
        sommeZ /= j;
        somme /= j;
        for (i=0; i< x->nb_mass; i++)
        {
            stdX += sqr(x->mass[i].speedX-sommeX);
            stdY += sqr(x->mass[i].speedY-sommeY);
            stdZ += sqr(x->mass[i].speedZ-sommeZ);
            std  += sqr(sqrt(sqr(x->mass[i].speedX) + sqr(x->mass[i].speedY) + sqr(x->mass[i].speedZ))-somme);
        }
    }    
    
    stdX = sqrt(stdX/j);
    stdY = sqrt(stdY/j);
    stdZ = sqrt(stdZ/j);
    std  = sqrt(std /j);    

    SETFLOAT(&(std_out[0]),stdX);
    SETFLOAT(&(std_out[1]),stdY);
    SETFLOAT(&(std_out[2]),stdZ);
    SETFLOAT(&(std_out[3]),std);
    
    outlet_anything(x->main_outlet, gensym("massesSpeedsStd"),4 , std_out);
}
