void pmpd_setK(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
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

void pmpd_setD(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
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

void pmpd_setD2(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].D2 = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].D2 = atom_getfloatarg(1, argc, argv);
            }
        }
    }
    if ( ( argv[0].a_type == A_FLOAT ) && ( argc == 1 ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            x->mass[i].D2 = atom_getfloatarg(0, argc, argv);
        }
    }

}

void pmpd_setL(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].L = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
                x->link[i].L = atom_getfloatarg(1, argc, argv);
            }
        }
    }
    if ( ( argv[0].a_type == A_FLOAT ) && ( argc == 1 ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].L = x->link[tmp].mass2->posX - x->link[tmp].mass1->posX;
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argc == 1 ) )
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

void pmpd_setLCurrent(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_setLKTab(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;
    t_float K_l = atom_getfloatarg(1, argc, argv);
    if (K_l <=  0) K_l = 1;
    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].K_L = K_l;
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
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

void pmpd_setLDTab(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;
    t_float D_l = atom_getfloatarg(1, argc, argv);
    if (D_l <=  0) D_l = 1;
    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].D_L = D_l;
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
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

void pmpd_setLinkId(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_SYMBOL ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
        x->link[tmp].Id = atom_getsymbolarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_SYMBOL ) )
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

void pmpd_setMassId(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_SYMBOL ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].Id = atom_getsymbolarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_SYMBOL ) )
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
void pmpd_setFixed(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( argv[0].a_type == A_FLOAT ) 
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].mobile = 0;
    }
    if ( argv[0].a_type == A_SYMBOL )
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

void pmpd_setMobile(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( argv[0].a_type == A_FLOAT )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].mobile = 1;
    }
    if ( argv[0].a_type == A_SYMBOL ) 
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

void pmpd_setSpeedX(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].speedX = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
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
void pmpd_setForceX(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].forceX = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
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

void pmpd_setActivei(t_pmpd *x, int i)
{
	float L;
    L = x->link[i].mass1->posX - x->link[i].mass2->posX;
	x->link[i].distance = L; 
	x->link[i].active = 1;
}

void pmpd_setActive(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( (argc == 1) && ( argv[0].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
		pmpd_setActivei(x,tmp);
    }
    else if ( (argc == 1) && ( argv[0].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id )
            {
				pmpd_setActivei(x,i);
            }
        }
    }
    else if ( argc == 0 ) 
    {
        for (i=0; i< x->nb_link; i++)
        {
			pmpd_setActivei(x,i);
        }
    }
}

void pmpd_setInactive(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_posX(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
// displace a mass to a certain position
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
        x->mass[tmp].posX = atom_getfloatarg(1, argc, argv);
           x->mass[tmp].speedX = 0; // ??? TODO : esce la bonne chose a faire?
        x->mass[tmp].forceX = 0; // ??? TODO : esce la bonne chose a faire?
        
    }
    if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
                x->mass[i].posX = atom_getfloatarg(1, argc, argv);
                x->mass[i].speedX = 0; // ??? TODO : esce la bonne chose a faire?
                x->mass[i].forceX = 0; // ??? TODO : esce la bonne chose a faire?

            }
        }
    }
}

void pmpd_overdamp(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
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

void pmpd_setConnection1i(t_pmpd *x, int i, int j)
{
	x->link[i].mass1=&x->mass[max(0, min( x->nb_mass-1, j))];
	x->link[i].distance = x->link[i].mass1->posX - x->link[i].mass2->posX;
}

void pmpd_setEnd1(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
	int tmp, i;
	
	if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
		pmpd_setConnection1i(x,tmp,atom_getfloatarg(1, argc, argv));

    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id )
            {
				pmpd_setConnection1i(x,i,atom_getfloatarg(1, argc, argv));
            }
        }
    }
}

void pmpd_setConnection2i(t_pmpd *x, int i, int j)
{
	x->link[i].mass2=&x->mass[max(0, min( x->nb_mass-1, j))];
	x->link[i].distance = x->link[i].mass1->posX - x->link[i].mass2->posX;
}

void pmpd_setEnd2(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
	int tmp, i;
	
	if ( (argc == 2) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
		pmpd_setConnection2i(x,tmp,atom_getfloatarg(1, argc, argv));

    }
    else if ( (argc == 2) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id )
            {
				pmpd_setConnection2i(x,i,atom_getfloatarg(1, argc, argv));
            }
        }
    }
}

void pmpd_setConnectioni(t_pmpd *x, int i, int j, int k)
{
	x->link[i].mass1=&x->mass[max(0, min( x->nb_mass-1, j))];
	x->link[i].mass2=&x->mass[max(0, min( x->nb_mass-1, k))];
	x->link[i].distance = x->link[i].mass1->posX - x->link[i].mass2->posX;
}

void pmpd_setEnd(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
	int tmp, i;
	
	if ( (argc == 3) && ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
		pmpd_setConnectioni(x,tmp,atom_getfloatarg(1, argc, argv),atom_getfloatarg(2, argc, argv));

    }
    else if ( (argc == 3) && ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id )
            {
				pmpd_setConnectioni(x,i,atom_getfloatarg(1, argc, argv),atom_getfloatarg(2, argc, argv));
            }
        }
    }
}

