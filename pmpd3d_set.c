void pmpd3d_setK(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc==2) &&( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].K = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
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

void pmpd3d_setD(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc==2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].D = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
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

void pmpd3d_setDEnv(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].D2 = atom_getfloatarg(1, argc, argv);
    }
    if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].D2 = atom_getfloatarg(1, argc, argv);
            }
        }
    }
    if ( (argc == 1) && ( argv[0].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            x->mass[i].D2 = atom_getfloatarg(0, argc, argv);
        }
    }
}

void pmpd3d_setDEnvOffset(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].D2offset = atom_getfloatarg(1, argc, argv);
    }
    if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].D2offset = atom_getfloatarg(1, argc, argv);
            }
        }
    }
    if ( (argc == 1) && ( argv[0].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            x->mass[i].D2offset = atom_getfloatarg(0, argc, argv);
        }
    }
}

void pmpd3d_setL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].L = atom_getfloatarg(1, argc, argv);
    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
                x->link[i].L = atom_getfloatarg(1, argc, argv);
            }
        }
    }
    else if ( (argc == 1) && ( argv[0].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].L = x->link[tmp].mass2->posX - x->link[tmp].mass1->posX;
    }
    else if ( (argc == 1) && ( argv[0].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
                x->link[i].L = x->link[i].mass2->posX - x->link[i].mass1->posX;
            }
        }
    }
}
void pmpd3d_setLCurrent(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argc == 1 ) && ( argv[0].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].L = x->link[tmp].mass2->posX - x->link[tmp].mass1->posX;
    }
    else if ( ( argc == 1 ) && ( argv[0].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
                x->link[i].L = x->link[i].mass2->posX - x->link[i].mass1->posX;
            }
        }
    }
    else if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        i = atom_getfloatarg(0, argc, argv);
        i = max(0, min( x->nb_link-1, i));
        x->link[i].L = mix(x->link[i].L,x->link[i].distance,atom_getfloatarg(1, argc, argv));
    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
                x->link[i].L = mix(x->link[i].L,x->link[i].distance,atom_getfloatarg(1, argc, argv));
            }
        }
    }
}



void pmpd3d_setLKTab(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
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
    if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
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

void pmpd3d_setLDTab(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
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
    if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
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

void pmpd3d_setLinkId(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_SYMBOL ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].Id = atom_getsymbolarg(1, argc, argv);
    }
    if ( ((argc == 2) &&  argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_SYMBOL ) )
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

void pmpd3d_setMassId(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_SYMBOL ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].Id = atom_getsymbolarg(1, argc, argv);
    }
    if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_SYMBOL ) )
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

void pmpd3d_setFixed(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 1) && (argv[0].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].mobile = 0;
    }
    if ( (argc == 2) && (argv[0].a_type == A_SYMBOL ) )
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

void pmpd3d_setMobile(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 1) && ( argv[0].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].mobile = 1;
    }
    if ( (argc == 1) && ( argv[0].a_type == A_SYMBOL ) )
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

void pmpd3d_setSpeed(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 4) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].speedX = atom_getfloatarg(1, argc, argv);
        x->mass[tmp].speedY = atom_getfloatarg(2, argc, argv);
        x->mass[tmp].speedZ = atom_getfloatarg(3, argc, argv);
    }
    if ( (argc == 4) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].speedX = atom_getfloatarg(1, argc, argv);
                x->mass[i].speedY = atom_getfloatarg(2, argc, argv);
                x->mass[i].speedZ = atom_getfloatarg(3, argc, argv);
            }
        }
    }
}

void pmpd3d_setSpeedX(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].speedX = atom_getfloatarg(1, argc, argv);
    }
    if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
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

void pmpd3d_setSpeedY(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].speedY = atom_getfloatarg(1, argc, argv);
    }
    if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
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

void pmpd3d_setSpeedZ(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].speedZ = atom_getfloatarg(1, argc, argv);
    }
    if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].speedZ = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd3d_setForce(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 4) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].forceX = atom_getfloatarg(1, argc, argv);
        x->mass[tmp].forceY = atom_getfloatarg(2, argc, argv);
        x->mass[tmp].forceZ = atom_getfloatarg(3, argc, argv);
    }
    if ( (argc == 4) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].forceX = atom_getfloatarg(1, argc, argv);
                x->mass[i].forceY = atom_getfloatarg(2, argc, argv);
                x->mass[i].forceZ = atom_getfloatarg(3, argc, argv);
            }
        }
    }
}

void pmpd3d_setForceX(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].forceX = atom_getfloatarg(1, argc, argv);
    }
    if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
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

void pmpd3d_setForceY(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].forceY = atom_getfloatarg(1, argc, argv);
    }
    if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
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

void pmpd3d_setForceZ(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].forceZ = atom_getfloatarg(1, argc, argv);
    }
    if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].forceZ = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd3d_setActive(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 1) && ( argv[0].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].active = 1;
    }
    else if ( (argc == 1) && ( argv[0].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id )
            {
                x->link[i].active = 1;
            }
        }
    }
    else if ( argc == 0 ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
			x->link[i].active = 1;
        }
    }
}

void pmpd3d_setInactive(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd3d_pos(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// displace a mass to a certain position
	t_int tmp, i;

	if ( (argc == 4) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
	{
		tmp = atom_getfloatarg(0, argc, argv);
		tmp = max(0, min( x->nb_mass-1, tmp));
		x->mass[tmp].posX = atom_getfloatarg(1, argc, argv);
		   x->mass[tmp].speedX = 0;
		x->mass[tmp].forceX = 0;
		   x->mass[tmp].posY = atom_getfloatarg(2, argc, argv);
		   x->mass[tmp].speedY = 0;
		x->mass[tmp].forceY = 0;
		   x->mass[tmp].posZ = atom_getfloatarg(3, argc, argv);
		   x->mass[tmp].speedZ = 0;
		x->mass[tmp].forceZ = 0;
	}
	if ( (argc == 4) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
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
				x->mass[i].posZ = atom_getfloatarg(3, argc, argv);
				x->mass[i].speedZ = 0;
				x->mass[i].forceZ = 0;
			}
		}
	}
}

void pmpd3d_posX(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// displace a mass to a certain position
	t_int tmp, i;

	if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
	{
		tmp = atom_getfloatarg(0, argc, argv);
		tmp = max(0, min( x->nb_mass-1, tmp));
		x->mass[tmp].posX = atom_getfloatarg(1, argc, argv);
		   x->mass[tmp].speedX = 0;
		x->mass[tmp].forceX = 0;

	}
	if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
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

void pmpd3d_posY(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// displace a mass to a certain position
	t_int tmp, i;

	if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
	{
		tmp = atom_getfloatarg(0, argc, argv);
		tmp = max(0, min( x->nb_mass-1, tmp));
		x->mass[tmp].posY = atom_getfloatarg(1, argc, argv);
		   x->mass[tmp].speedY = 0;
		x->mass[tmp].forceY = 0;

	}
	if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
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

void pmpd3d_posZ(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// displace a mass to a certain position
	t_int tmp, i;

	if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
	{
		tmp = atom_getfloatarg(0, argc, argv);
		tmp = max(0, min( x->nb_mass-1, tmp));
		x->mass[tmp].posZ = atom_getfloatarg(1, argc, argv);
		   x->mass[tmp].speedZ = 0;
		x->mass[tmp].forceZ = 0;

	}
	if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
	{
		for (i=0; i< x->nb_mass; i++)
		{
			if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{
				x->mass[i].posZ = atom_getfloatarg(1, argc, argv);
				x->mass[i].speedZ = 0;
				x->mass[i].forceZ = 0;
			}
		}
	}
}
s
