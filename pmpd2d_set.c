void pmpd2d_setK(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].K = atom_getfloatarg(1, argc, argv);
    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
                x->link[i].K = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd2d_setD(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].D = atom_getfloatarg(1, argc, argv);
    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
                x->link[i].D = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd2d_setDEnv(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].D2 = atom_getfloatarg(1, argc, argv);
    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].D2 = atom_getfloatarg(1, argc, argv);
            }
        }
    }
    else if ( (argc == 1) && ( argv[0].a_type == A_FLOAT ) && ( argc == 1 ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            x->mass[i].D2 = atom_getfloatarg(0, argc, argv);
        }
    }
}

void pmpd2d_setDEnvOffset(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if (  (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].D2offset = atom_getfloatarg(1, argc, argv);
    }
    else if (  (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].D2offset = atom_getfloatarg(1, argc, argv);
            }
        }
    }
    else if (  (argc == 1) && ( argv[0].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            x->mass[i].D2offset = atom_getfloatarg(0, argc, argv);
        }
    }
}

void pmpd2d_setL(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    { // set a link to a specific length
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].L = atom_getfloatarg(1, argc, argv);
    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    { // set a class of link to a specific length
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
                x->link[i].L = atom_getfloatarg(1, argc, argv);
            }
        }
    }
    else if ( (argc == 1) && ( argv[0].a_type == A_FLOAT ) && ( argc == 1 ) )
    { // set a link to it's current length
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].L = x->link[tmp].distance;
    }
    else if ( (argc == 1) && ( argv[0].a_type == A_SYMBOL ) && ( argc == 1 ) )
    { // set a class of link to there current length
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
                x->link[i].L = x->link[i].distance;
            }
        }
    }
}

void pmpd2d_setLCurrent(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argc == 1 ) && ( argv[0].a_type == A_FLOAT ) )
    { // set a link to it's current length
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].L = x->link[tmp].distance;
    }
    else if ( ( argc == 1 ) && ( argv[0].a_type == A_SYMBOL ) )
    { // set a class of link to there current length
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
                x->link[i].L = x->link[i].distance;
            }
        }
    }
    else if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    { // set a link to a mix between it's curent length and it's size
        i = atom_getfloatarg(0, argc, argv);
        i = max(0, min( x->nb_link-1, i));
        x->link[i].L = mix(x->link[i].L,x->link[i].distance,atom_getfloatarg(1, argc, argv));
    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    { // set a class of link to a mix between it's curent length and it's size
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
                x->link[i].L = mix(x->link[i].L,x->link[i].distance,atom_getfloatarg(1, argc, argv));
            }
        }
    }
}

void pmpd2d_setLKTab(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;
    t_float K_l = atom_getfloatarg(1, argc, argv);
    if (K_l <=  0) K_l = 1;
    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].K_L = K_l;
    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
                x->link[i].K_L = K_l;
            }
        }
    }
}

void pmpd2d_setLDTab(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;
    t_float D_l = atom_getfloatarg(1, argc, argv);
    if (D_l <=  0) D_l = 1;
    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].D_L = D_l;
    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
                x->link[i].D_L = D_l;
            }
        }
    }
}

void pmpd2d_setLinkId(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_SYMBOL ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].Id = atom_getsymbolarg(1, argc, argv);
    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
                x->link[i].Id = atom_getsymbolarg(1, argc, argv);
            }
        }
    }
}

void pmpd2d_setMassId(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_SYMBOL ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].Id = atom_getsymbolarg(1, argc, argv);
    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].Id = atom_getsymbolarg(1, argc, argv);
            }
        }
    }
}

void pmpd2d_setFixed(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 1) && (argv[0].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].mobile = 0;
    }
    else if ( (argc == 1) && (argv[0].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].mobile = 0;
            }
        }
    }
}

void pmpd2d_setMobile(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 1) && (argv[0].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].mobile = 1;
    }
    else if ( (argc == 2) && (argv[0].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].mobile = 1;
            }
        }
    }
}

void pmpd2d_setSpeed(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 3) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].speedX = atom_getfloatarg(1, argc, argv);
        x->mass[tmp].speedY = atom_getfloatarg(2, argc, argv);
    }
    else if ( (argc == 3) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].speedX = atom_getfloatarg(1, argc, argv);
                x->mass[i].speedY = atom_getfloatarg(2, argc, argv);
            }
        }
    }
}

void pmpd2d_setSpeedX(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].speedX = atom_getfloatarg(1, argc, argv);
    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].speedX = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd2d_setSpeedY(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].speedY = atom_getfloatarg(1, argc, argv);
    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].speedY = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd2d_setForce(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 3) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].forceX = atom_getfloatarg(1, argc, argv);
        x->mass[tmp].forceY = atom_getfloatarg(2, argc, argv);
    }
    else if ( (argc == 3) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].forceX = atom_getfloatarg(1, argc, argv);
                x->mass[i].forceY = atom_getfloatarg(2, argc, argv);
            }
        }
    }
}

void pmpd2d_setForceX(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].forceX = atom_getfloatarg(1, argc, argv);
    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].forceX = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd2d_setForceY(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].forceY = atom_getfloatarg(1, argc, argv);
    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].forceY = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd2d_setActivei(t_pmpd2d *x, int i)
{
	float Lx, Ly, L;
	Lx = x->link[i].mass1->posX - x->link[i].mass2->posX;
	Ly = x->link[i].mass1->posY - x->link[i].mass2->posY;
	L = sqrt( sqr(Lx) + sqr(Ly) );
	x->link[i].distance = L; 
	x->link[i].active = 1;
}

void pmpd2d_setActive(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 1) && ( argv[0].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
		pmpd2d_setActivei(x,tmp);
    }
    else if ( (argc == 1) && ( argv[0].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id )
            {
		        pmpd2d_setActivei(x,i);
            }
        }
    }
    else if ( argc == 0 ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
		    pmpd2d_setActivei(x,i);
        }
    }
}

void pmpd2d_setInactive(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 1) && ( argv[0].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].active = 0;
    }
    else if ( (argc == 1) && ( argv[0].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
                x->link[i].active = 0;
            }
        }
    }
    else if ( argc == 0 ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
			x->link[i].active = 0;
        }
    }
}

void pmpd2d_pos(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
// displace a mass to a certain position
    int tmp, i;

    if ( (argc == 3) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].posX = atom_getfloatarg(1, argc, argv);
           x->mass[tmp].speedX = 0; 
        x->mass[tmp].forceX = 0; 
           x->mass[tmp].posY = atom_getfloatarg(2, argc, argv);
           x->mass[tmp].speedY = 0; 
        x->mass[tmp].forceY = 0; 
        
    }
    else if ( (argc == 3) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].posX = atom_getfloatarg(1, argc, argv);
                x->mass[i].speedX = 0; 
                x->mass[i].forceX = 0;
                x->mass[i].posY = atom_getfloatarg(2, argc, argv);
                x->mass[i].speedY = 0; 
                x->mass[i].forceY = 0;

            }
        }
    }
}

void pmpd2d_posX(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
// displace a mass to a certain position
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].posX = atom_getfloatarg(1, argc, argv);
        x->mass[tmp].speedX = 0; 
        x->mass[tmp].forceX = 0; 
    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].posX = atom_getfloatarg(1, argc, argv);
                x->mass[i].speedX = 0; 
                x->mass[i].forceX = 0;
            }
        }
    }
}

void pmpd2d_posY(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
// displace a mass to a certain position
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].posY = atom_getfloatarg(1, argc, argv);
           x->mass[tmp].speedY = 0; 
        x->mass[tmp].forceY = 0; 
        
    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].posY = atom_getfloatarg(1, argc, argv);
                x->mass[i].speedY = 0; 
                x->mass[i].forceY = 0;
            }
        }
    }
}

void pmpd2d_overdamp(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
// set the overdamped factor to a mass
	t_int tmp, i;

	if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
	{
		tmp = atom_getfloatarg(0, argc, argv);
		tmp = max(0, min( x->nb_mass-1, tmp));
		x->mass[tmp].overdamp = atom_getfloatarg(1, argc, argv);
	}
	else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
	{
		for (i=0; i< x->nb_mass; i++)
		{
			if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{
				x->mass[i].overdamp = atom_getfloatarg(1, argc, argv);
			}
		}
	}
}

void pmpd2d_setConnection1i(t_pmpd2d *x, int i, int j)
{
	float Lx, Ly, L;

	x->link[i].mass1=&x->mass[max(0, min( x->nb_mass-1, j))];
	Lx = x->link[i].mass1->posX - x->link[i].mass2->posX;
	Ly = x->link[i].mass1->posY - x->link[i].mass2->posY;
	L = sqrt( sqr(Lx) + sqr(Ly) );
	x->link[i].distance = L; 
}

void pmpd2d_setEnd1(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
	int tmp, i;
	
	if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
		pmpd2d_setConnection1i(x,tmp,atom_getfloatarg(1, argc, argv));

    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id )
            {
				pmpd2d_setConnection1i(x,i,atom_getfloatarg(1, argc, argv));
            }
        }
    }
}

void pmpd2d_setConnection2i(t_pmpd2d *x, int i, int j)
{
	float Lx, Ly, L;

	x->link[i].mass2=&x->mass[max(0, min( x->nb_mass-1, j))];
	Lx = x->link[i].mass1->posX - x->link[i].mass2->posX;
	Ly = x->link[i].mass1->posY - x->link[i].mass2->posY;
	L = sqrt( sqr(Lx) + sqr(Ly) );
	x->link[i].distance = L; 
}

void pmpd2d_setEnd2(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
	int tmp, i;
	
	if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
		pmpd2d_setConnection2i(x,tmp,atom_getfloatarg(1, argc, argv));

    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id )
            {
				pmpd2d_setConnection2i(x,i,atom_getfloatarg(1, argc, argv));
            }
        }
    }
}

void pmpd2d_setConnectioni(t_pmpd2d *x, int i, int j, int k)
{
	float Lx, Ly, L;

	x->link[i].mass1=&x->mass[max(0, min( x->nb_mass-1, j))];
	x->link[i].mass2=&x->mass[max(0, min( x->nb_mass-1, k))];

	Lx = x->link[i].mass1->posX - x->link[i].mass2->posX;
	Ly = x->link[i].mass1->posY - x->link[i].mass2->posY;
	L = sqrt( sqr(Lx) + sqr(Ly) );
	x->link[i].distance = L; 
}

void pmpd2d_setEnd(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
	int tmp, i;
	
	if ( (argc == 3) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
		pmpd2d_setConnectioni(x,tmp,atom_getfloatarg(1, argc, argv),atom_getfloatarg(2, argc, argv));

    }
    else if ( (argc == 3) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id )
            {
				pmpd2d_setConnectioni(x,i,atom_getfloatarg(1, argc, argv),atom_getfloatarg(2, argc, argv));
            }
        }
    }
}

