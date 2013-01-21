void pmpd3d_infosL(t_pmpd3d *x)
{
	t_int i;
	post("list of mass");
	post("number, Id, mobile, mass, Damping, positionX Y Z, speedX Y Z, forcesX Y Z");
	for(i=0; i < x->nb_mass; i++)
	{
		post("masse %i: %s, %d, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f",i, x->mass[i].Id->s_name, \
			x->mass[i].mobile, 1/x->mass[i].invM, x->mass[i].D2, x->mass[i].posX, x->mass[i].posY, \
			x->mass[i].posZ, x->mass[i].speedX, x->mass[i].speedY, x->mass[i].speedZ, \
			x->mass[i].forceX, x->mass[i].forceY, x->mass[i].forceZ );
	}

	post("list of link");
	post("number, Id, mass1, mass2, K, D, Pow, L, Lmin, Lmax");
	for(i=0; i < x->nb_link; i++)
	{
		switch (x->link[i].lType)
		{
		case 0:
			post("link %i: %s, %i, %i, %f, %f, %f, %f, %f, %f", i, x->link[i].Id->s_name, \
				x->link[i].mass1->num, x->link[i].mass2->num, x->link[i].K, x->link[i].D, \
				x->link[i].Pow, x->link[i].L, x->link[i].Lmin, x->link[i].Lmax);
			break;
		case 1:
			post("tLink %i: %s, %i, %i, %f, %f, %f, %f, %f, %f, %f, %f, %f", i, x->link[i].Id->s_name, \
				x->link[i].mass1->num, x->link[i].mass2->num, x->link[i].K, x->link[i].D, \
				x->link[i].Pow, x->link[i].L, x->link[i].Lmin, x->link[i].Lmax, \
				x->link[i].VX, x->link[i].VY, x->link[i].VZ);
			break;
		case 2:
			post("tabLink %i: %s, %i, %i, %f, %f, %s, %f, %s, %f", i, x->link[i].Id->s_name, \
				x->link[i].mass1->num, x->link[i].mass2->num, x->link[i].K, x->link[i].D, \
				x->link[i].arrayK->s_name, x->link[i].K_L, x->link[i].arrayD->s_name, x->link[i].D_L);
			break;
		}
	}
}

void pmpd3d_force(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// add a force to a specific mass
	t_int tmp, i;

	if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
	{
		tmp = atom_getfloatarg(0, argc, argv);
		tmp = max(0, min( x->nb_mass-1, tmp));
		x->mass[tmp].forceX += atom_getfloatarg(1, argc, argv);
		x->mass[tmp].forceY += atom_getfloatarg(2, argc, argv);
		x->mass[tmp].forceZ += atom_getfloatarg(3, argc, argv);
	}
	if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
	{
		for (i=0; i< x->nb_mass; i++)
		{
			if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{
				x->mass[i].forceX += atom_getfloatarg(1, argc, argv);
				x->mass[i].forceY += atom_getfloatarg(2, argc, argv);
				x->mass[i].forceZ += atom_getfloatarg(3, argc, argv);
			}
		}
	}
}

void pmpd3d_forceX(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// add a force to a specific mass
	t_int tmp, i;

	if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
	{
		tmp = atom_getfloatarg(0, argc, argv);
		tmp = max(0, min( x->nb_mass-1, tmp));
		x->mass[tmp].forceX += atom_getfloatarg(1, argc, argv);
	}
	if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
	{
		for (i=0; i< x->nb_mass; i++)
		{
			if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{
				x->mass[i].forceX += atom_getfloatarg(1, argc, argv);
			}
		}
	}
}

void pmpd3d_forceY(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// add a force to a specific mass
	t_int tmp, i;

	if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
	{
		tmp = atom_getfloatarg(0, argc, argv);
		tmp = max(0, min( x->nb_mass-1, tmp));
		x->mass[tmp].forceY += atom_getfloatarg(1, argc, argv);
	}
	if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
	{
		for (i=0; i< x->nb_mass; i++)
		{
			if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{
				x->mass[i].forceY += atom_getfloatarg(1, argc, argv);
			}
		}
	}
}

void pmpd3d_forceZ(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// add a force to a specific mass
	t_int tmp, i;

	if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
	{
		tmp = atom_getfloatarg(0, argc, argv);
		tmp = max(0, min( x->nb_mass-1, tmp));
		x->mass[tmp].forceZ += atom_getfloatarg(1, argc, argv);
	}
	if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
	{
		for (i=0; i< x->nb_mass; i++)
		{
			if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{
				x->mass[i].forceZ += atom_getfloatarg(1, argc, argv);
			}
		}
	}
}

void pmpd3d_pos(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// displace a mass to a certain position
	t_int tmp, i;

	if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
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
	if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
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

	if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
	{
		tmp = atom_getfloatarg(0, argc, argv);
		tmp = max(0, min( x->nb_mass-1, tmp));
		x->mass[tmp].posX = atom_getfloatarg(1, argc, argv);
		   x->mass[tmp].speedX = 0;
		x->mass[tmp].forceX = 0;

	}
	if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
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

	if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
	{
		tmp = atom_getfloatarg(0, argc, argv);
		tmp = max(0, min( x->nb_mass-1, tmp));
		x->mass[tmp].posY = atom_getfloatarg(1, argc, argv);
		   x->mass[tmp].speedY = 0;
		x->mass[tmp].forceY = 0;

	}
	if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
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

	if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
	{
		tmp = atom_getfloatarg(0, argc, argv);
		tmp = max(0, min( x->nb_mass-1, tmp));
		x->mass[tmp].posZ = atom_getfloatarg(1, argc, argv);
		   x->mass[tmp].speedZ = 0;
		x->mass[tmp].forceZ = 0;

	}
	if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
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

void pmpd3d_min(t_pmpd3d *x, t_float minX, t_float minY, t_float minZ)
{
	x->minX = minX;
	x->minY = minY;
	x->minZ = minZ;
}

void pmpd3d_max(t_pmpd3d *x, t_float maxX, t_float maxY, t_float maxZ)
{
	x->maxX = maxX;
	x->maxY = maxY;
	x->maxZ = maxZ;
}

void pmpd3d_minX(t_pmpd3d *x, t_float min)
{
	x->minX = min;
}

void pmpd3d_maxX(t_pmpd3d *x, t_float max)
{
	x->maxX = max;
}

void pmpd3d_minY(t_pmpd3d *x, t_float min)
{
	x->minY = min;
}

void pmpd3d_maxY(t_pmpd3d *x, t_float max)
{
	x->maxY = max;
}

void pmpd3d_minZ(t_pmpd3d *x, t_float min)
{
	x->minZ = min;
}

void pmpd3d_maxZ(t_pmpd3d *x, t_float max)
{
	x->maxZ = max;
}

void pmpd3d_addPos(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
	t_int tmp, i;

	if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
	{
		tmp = atom_getfloatarg(0, argc, argv);
		tmp = max(0, min( x->nb_mass-1, tmp));
		x->mass[tmp].posX += atom_getfloatarg(1, argc, argv);
		x->mass[tmp].posY += atom_getfloatarg(2, argc, argv);
		x->mass[tmp].posZ += atom_getfloatarg(3, argc, argv);
	}
	if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) && ( argv[3].a_type == A_FLOAT ) )
	{
		for (i=0; i< x->nb_mass; i++)
		{
			if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{
				x->mass[i].posX += atom_getfloatarg(1, argc, argv);
				x->mass[i].posY += atom_getfloatarg(2, argc, argv);
				x->mass[i].posZ += atom_getfloatarg(3, argc, argv);
			}
		}
	}
}

void pmpd3d_addPosX(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
	t_int tmp, i;

	if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
	{
		tmp = atom_getfloatarg(0, argc, argv);
		tmp = max(0, min( x->nb_mass-1, tmp));
		x->mass[tmp].posX += atom_getfloatarg(1, argc, argv);
	}
	if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
	{
		for (i=0; i< x->nb_mass; i++)
		{
			if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{
				x->mass[i].posX += atom_getfloatarg(1, argc, argv);
			}
		}
	}
}

void pmpd3d_addPosY(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
	t_int tmp, i;

	if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
	{
		tmp = atom_getfloatarg(0, argc, argv);
		tmp = max(0, min( x->nb_mass-1, tmp));
		x->mass[tmp].posY += atom_getfloatarg(1, argc, argv);
	}
	if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
	{
		for (i=0; i< x->nb_mass; i++)
		{
			if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{
				x->mass[i].posY += atom_getfloatarg(1, argc, argv);
			}
		}
	}
}

void pmpd3d_addPosZ(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
	t_int tmp, i;

	if ( ( argv[0].a_type == A_FLOAT ) && ( argv[1].a_type == A_FLOAT ) )
	{
		tmp = atom_getfloatarg(0, argc, argv);
		tmp = max(0, min( x->nb_mass-1, tmp));
		x->mass[tmp].posZ += atom_getfloatarg(1, argc, argv);
	}
	if ( ( argv[0].a_type == A_SYMBOL ) && ( argv[1].a_type == A_FLOAT ) )
	{
		for (i=0; i< x->nb_mass; i++)
		{
			if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{
				x->mass[i].posZ += atom_getfloatarg(1, argc, argv);
			}
		}
	}
}

void pmpd3d_grabMass(t_pmpd3d *x, t_float posX, t_float posY, t_float posZ, t_float grab)
{
	t_float dist, tmp;
	t_int i;

	if (grab == 0)
		x->grab=0;
	if ((x->grab == 0)&(grab == 1)&(x->nb_mass > 0))
	{
		x->grab=1;
		x->grab_nb= 0;
		dist = sqr(x->mass[0].posX - posX) + sqr(x->mass[0].posY - posY) + sqr(x->mass[0].posZ - posZ);
		for (i=1; i<x->nb_mass; i++)
		{
			tmp = sqr(x->mass[i].posX - posX) + sqr(x->mass[i].posY - posY) + sqr(x->mass[i].posZ - posZ);
			if (tmp < dist)
			{
				dist = tmp;
				x->grab_nb= i;
			}
		}
	}
	if (x->grab == 1)
	{
		x->mass[x->grab_nb].posX = posX;
		x->mass[x->grab_nb].posY = posY;
		x->mass[x->grab_nb].posZ = posZ;
	}
}

void pmpd3d_closestMass(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float dist, tmp;
	t_int i;
	t_atom std_out[4];

	t_float posX, posY, posZ;
	posX = atom_getfloatarg(0, argc, argv);
	posY = atom_getfloatarg(1, argc, argv);
	posZ = atom_getfloatarg(2, argc, argv);
	if ( (argc == 4)  && (argv[3].a_type == A_SYMBOL) )
	{
		//t_symbol *mass_name = atom_getsymbolarg(2, argc, argv);
		if ((x->nb_mass > 0))
		{
			dist = 1000000000;//sqr(x->mass[0].posX - posX) + sqr(x->mass[0].posY - posY);
			for (i=0; i<x->nb_mass; i++)
			{
				if (atom_getsymbolarg(3,argc,argv) == x->mass[i].Id)
				{
					tmp = sqr(x->mass[i].posX - posX) + sqr(x->mass[i].posY - posY) + sqr(x->mass[i].posZ - posZ);
					if (tmp < dist)
					{
						dist = tmp;
						x->grab_nb= i;
					}
				}
			}
		}
	}
	else {
		if ((x->nb_mass > 0))
		{
			dist = sqr(x->mass[0].posX - posX) + sqr(x->mass[0].posY - posY) + sqr(x->mass[0].posZ - posZ);
			for (i=1; i<x->nb_mass; i++)
			{
				tmp = sqr(x->mass[i].posX - posX) + sqr(x->mass[i].posY - posY) + sqr(x->mass[i].posZ - posZ);
				if (tmp < dist)
				{
					dist = tmp;
					x->grab_nb= i;
				}
			}
		}
	}

	SETFLOAT(&(std_out[0]),x->grab_nb);
	SETFLOAT(&(std_out[1]), x->mass[x->grab_nb].posX);
	SETFLOAT(&(std_out[2]), x->mass[x->grab_nb].posY);
	SETFLOAT(&(std_out[3]), x->mass[x->grab_nb].posZ);
	outlet_anything(x->main_outlet, gensym("closestMass"),4,std_out);

}

void pmpd3d_massesDistances_f_f(t_pmpd3d *x, t_int i, t_int j)
{
	t_float dist, tmp;
	t_atom to_out[3];

	tmp = x->mass[i].posX - x->mass[j].posX;
	dist = tmp*tmp;
	tmp = x->mass[i].posY - x->mass[j].posY;
	dist += tmp*tmp;
	tmp = x->mass[i].posZ - x->mass[j].posZ;
	dist += tmp*tmp;
	dist = sqrt(dist);

	SETFLOAT(&(to_out[0]), i);
	SETFLOAT(&(to_out[1]), j);
	SETFLOAT(&(to_out[2]), dist);
	outlet_anything(x->main_outlet, gensym("distances"), 3, to_out);
}

void pmpd3d_massesDistances(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
	t_int i,j;

	if ( (argc == 2) && (argv[0].a_type == A_FLOAT) && (argv[1].a_type == A_FLOAT) )
	{
		pmpd3d_massesDistances_f_f(x, atom_getfloatarg(0, argc, argv), atom_getfloatarg(1, argc, argv));
	}
	else if ( (argc == 2) && (argv[0].a_type == A_FLOAT) && (argv[1].a_type == A_SYMBOL) )
	{
		for (i=0; i < x->nb_mass; i++)
		{
			if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
			{
				pmpd3d_massesDistances_f_f(x, atom_getfloatarg(0, argc, argv), i);
			}
		}
	}
	else if ( (argc == 2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_FLOAT) )
	{
		for (i=0; i < x->nb_mass; i++)
		{
			if ( (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id) )
			{
				pmpd3d_massesDistances_f_f(x, atom_getfloatarg(1, argc, argv), i);
			}
		}
	}
	else if ( (argc == 2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
	{
		for (i=0; i < x->nb_mass; i++)
		{
			if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{
				for (j=i+1; j < x->nb_mass; j++)
				{
					if ( atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
					{
						pmpd3d_massesDistances_f_f(x,i, j);
					}
				}
			}
		}
	}
}

void pmpd3d_enumMasses(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
	t_int i, j, test, tmp, tmp2;
	t_atom std_out[1];

	j = 0;
	test = 1;

	for (i=0; i < x->nb_mass; i++)
	{
		while ( (j < argc-1) && (test) )
		{
			if (atom_getsymbolarg(j,argc,argv) == gensym("Id") )
			{
				if ( x->mass[i].Id != atom_getsymbolarg(j+1,argc,argv) ) { test = 0; j+=2;}
			}
			else if (atom_getsymbolarg(j,argc,argv) == gensym("posXSup") )
			{
				if ( x->mass[i].posX < atom_getfloatarg(j+1,argc,argv) ) { test = 0; j+=2;}
			}
			else if ( atom_getsymbolarg(j,argc,argv) == gensym("posXInf") )
			{
				if ( x->mass[i].posX > atom_getfloatarg(j+1,argc,argv) ) { test = 0; j+=2;}
			}
			else if (atom_getsymbolarg(j,argc,argv) == gensym("posYSup") )
			{
				if ( x->mass[i].posY < atom_getfloatarg(j+1,argc,argv) ) { test = 0; j+=2;}
			}
			else if ( atom_getsymbolarg(j,argc,argv) == gensym("posYInf") )
			{
				if ( x->mass[i].posY > atom_getfloatarg(j+1,argc,argv) ) { test = 0; j+=2;}
			}
			else if (atom_getsymbolarg(j,argc,argv) == gensym("posZSup") )
			{
				if ( x->mass[i].posZ < atom_getfloatarg(j+1,argc,argv) ) { test = 0; j+=2;}
			}
			else if ( atom_getsymbolarg(j,argc,argv) == gensym("posZInf") )
			{
				if ( x->mass[i].posZ > atom_getfloatarg(j+1,argc,argv) ) { test = 0; j+=2;}
			}
			else if ( atom_getsymbolarg(j,argc,argv) == gensym("speedXSup") )
			{
				if ( x->mass[i].speedX < atom_getfloatarg(j+1,argc,argv) ) { test = 0; j+=2;}
			}
			else if ( atom_getsymbolarg(j,argc,argv) == gensym("speedXInf") )
			{
				if ( x->mass[i].speedX > atom_getfloatarg(j+1,argc,argv) ) { test = 0; j+=2;}
			}
			else if ( atom_getsymbolarg(j,argc,argv) == gensym("speedYSup") )
			{
				if ( x->mass[i].speedY < atom_getfloatarg(j+1,argc,argv) ) { test = 0; j+=2;}
			}
			else if ( atom_getsymbolarg(j,argc,argv) == gensym("speedYInf") )
			{
				if ( x->mass[i].speedY > atom_getfloatarg(j+1,argc,argv) ) { test = 0; j+=2;}
			}
			else if ( atom_getsymbolarg(j,argc,argv) == gensym("speedZSup") )
			{
				if ( x->mass[i].speedZ < atom_getfloatarg(j+1,argc,argv) ) { test = 0; j+=2;}
			}
			else if ( atom_getsymbolarg(j,argc,argv) == gensym("speedZInf") )
			{
				if ( x->mass[i].speedZ > atom_getfloatarg(j+1,argc,argv) ) { test = 0; j+=2;}
			}
			else if ( atom_getsymbolarg(j,argc,argv) == gensym("speedSup") )
			{
				tmp  = x->mass[i].speedX * x->mass[i].speedX;
				tmp += x->mass[i].speedY * x->mass[i].speedY;
				tmp += x->mass[i].speedZ * x->mass[i].speedZ;

				if ( ( tmp < atom_getfloatarg(j+1,argc,argv)) * atom_getfloatarg(j+1,argc,argv) ) test = 0;

				j+= 2;
			}
			else if ( atom_getsymbolarg(j,argc,argv) == gensym("speedInf") )
			{
				tmp  = x->mass[i].speedX * x->mass[i].speedX;
				tmp += x->mass[i].speedY * x->mass[i].speedY;
				tmp += x->mass[i].speedZ * x->mass[i].speedZ;

				if ( ( tmp > atom_getfloatarg(j+1,argc,argv)) * atom_getfloatarg(j+1,argc,argv) ) test = 0;
				j+=2;
			}
			else if (atom_getsymbolarg(j,argc,argv) == gensym("distanceCicleInf"))
			{ 
				tmp = x->mass[i].posX - atom_getfloatarg(j+1,argc,argv);
				tmp *= tmp;
				tmp2 = x->mass[i].posY - atom_getfloatarg(j+2,argc,argv);
				tmp = tmp + tmp2 * tmp2;
				tmp2 = x->mass[i].posZ - atom_getfloatarg(j+3,argc,argv);
				tmp = tmp + tmp2 * tmp2;
				tmp2 = atom_getfloatarg(j+4,argc,argv);

				if ( tmp > (tmp2 * tmp2) ) test = 0;
				j += 5;
			}
			else if (atom_getsymbolarg(j,argc,argv) == gensym("distanceCicleSup"))
			{ 
				tmp = x->mass[i].posX - atom_getfloatarg(j+1,argc,argv);
				tmp *= tmp;
				tmp2 = x->mass[i].posY - atom_getfloatarg(j+2,argc,argv);
				tmp = tmp + tmp2 * tmp2;
				tmp2 = x->mass[i].posZ - atom_getfloatarg(j+3,argc,argv);
				tmp = tmp + tmp2 * tmp2;
				tmp2 = atom_getfloatarg(j+4,argc,argv);

				if ( tmp < (tmp2 * tmp2) ) test = 0;
				j += 5;
			}
			else
			{
				post("nor reconize %s",atom_getsymbolarg(j,argc,argv));
				j+=1;
			}
		}
		if (test)
		{
			SETFLOAT(&(std_out[0]),i);
			outlet_anything(x->main_outlet, gensym("listMasses"),1,std_out);
		}
	}
}

void pmpd3d_enumLinks(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
	t_int i, j, test;
	t_atom std_out[1];

	j = 0;
	test = 1;

	for (i=0; i < x->nb_link; i++)
	{
		while ( (j < argc-1) && (test) )
		{

			if (atom_getsymbolarg(j,argc,argv) == gensym("Id") )
			{
				if ( x->link[i].Id != atom_getsymbolarg(j+1,argc,argv) ) { test = 0; j+=2;}
			}
/*				else if (atom_getsymbolarg(j,argc,argv) == gensym("forcesXSup") )
			{
				if ( x->link[i].forceX < atom_getfloatarg(j+1,argc,argv) ) { test = 0; j+=2;}
			}
			else if ( atom_getsymbolarg(j,argc,argv) == gensym("forcesXInf") )
			{
				if ( x->link[i].forceX > atom_getfloatarg(j+1,argc,argv) ) { test = 0; j+=2;}
			}
*/				else if (atom_getsymbolarg(j,argc,argv) == gensym("lengthSup") )
			{
				if ( x->link[i].distance < atom_getfloatarg(j+1,argc,argv) ) { test = 0; j+=2;}
			}
			else if ( atom_getsymbolarg(j,argc,argv) == gensym("lengthInf") )
			{
				if ( x->link[i].distance > atom_getfloatarg(j+1,argc,argv) ) { test = 0; j+=2;}
			}
			else if ( atom_getsymbolarg(j,argc,argv) == gensym("connectedTo") )
			{
				if (argv[j+1].a_type == A_SYMBOL)
				{
					if (!( (x->link[i].mass1->Id == atom_getsymbolarg(j+1,argc,argv)) || (x->link[i].mass2->Id == atom_getsymbolarg(j+1,argc,argv)) )) 
					{ test = 0; j+=2;}					
				}
				else if (argv[j+1].a_type == A_FLOAT)
				{
					if (!( (x->link[i].mass1->num == atom_getfloatarg(j+1,argc,argv)) || (x->link[i].mass2->num == atom_getfloatarg(j+1,argc,argv)) ))
					{ test = 0; j+=2;}
				}
				else
				{
					post("bad argument for connectedTo");
					j+=1;
				}
			}
			else
			{
				post("nor reconize %s",atom_getsymbolarg(j,argc,argv));
				j+=1;
			}
		}
		if (test)
		{
			SETFLOAT(&(std_out[0]),i);
			outlet_anything(x->main_outlet, gensym("listLinks"),1,std_out);
		}
	}
}

void pmpd3d_forcesXT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// add forces to masses. forces comes from a table, masse can be filter on ther Id or not

	t_int i, j;
	t_garray *a;
	int npoints;
	t_word *vec;
	t_symbol *array;

	if ( argv[0].a_type == A_SYMBOL )
	{
		array = atom_getsymbolarg(0,argc,argv);
		if (!(a = (t_garray *)pd_findbyclass(array, garray_class)))
			pd_error(x, "%s: no such array", array->s_name);
		else if (!garray_getfloatwords(a, &npoints, &vec))
			pd_error(x, "%s: bad template for tabLink", array->s_name);
		else
		{
			if ( argc == 1 )  // apply forces on all masses
			{
				j = max(x->nb_mass, npoints);
				for (i=0; i< j; i++)
				{
					x->mass[i].forceX += vec[i].w_float;
				}
			}
			else if ( argv[1].a_type == A_SYMBOL )
			{
				i=0;
				j=0;

				while ( (i < x->nb_mass) && (j < npoints) )
				{
					if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
					{
						x->mass[i].forceX += vec[j].w_float;
						j++;
					}
					i++;
				}
			}
		}
	}
}

void pmpd3d_forcesYT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// add forces to masses. forces comes from a table, masse can be filter on ther Id or not

	t_int i, j;
	t_garray *a;
	int npoints;
	t_word *vec;
	t_symbol *array;

	if ( argv[0].a_type == A_SYMBOL )
	{
		array = atom_getsymbolarg(0,argc,argv);
		if (!(a = (t_garray *)pd_findbyclass(array, garray_class)))
			pd_error(x, "%s: no such array", array->s_name);
		else if (!garray_getfloatwords(a, &npoints, &vec))
			pd_error(x, "%s: bad template for tabLink", array->s_name);
		else
		{
			if ( argc == 1 )  // apply forces on all masses
			{
				j = max(x->nb_mass, npoints);
				for (i=0; i< j; i++)
				{
					x->mass[i].forceY += vec[i].w_float;
				}
			}
			else if ( argv[1].a_type == A_SYMBOL )
			{
				i=0;
				j=0;

				while ( (i < x->nb_mass) && (j < npoints) )
				{
					if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
					{
						x->mass[i].forceY += vec[j].w_float;
						j++;
					}
					i++;
				}
			}
		}
	}
}

void pmpd3d_forcesZT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
// add forces to masses. forces comes from a table, masse can be filter on ther Id or not

	t_int i, j;
	t_garray *a;
	int npoints;
	t_word *vec;
	t_symbol *array;

	if ( argv[0].a_type == A_SYMBOL )
	{
		array = atom_getsymbolarg(0,argc,argv);
		if (!(a = (t_garray *)pd_findbyclass(array, garray_class)))
			pd_error(x, "%s: no such array", array->s_name);
		else if (!garray_getfloatwords(a, &npoints, &vec))
			pd_error(x, "%s: bad template for tabLink", array->s_name);
		else
		{
			if ( argc == 1 )  // apply forces on all masses
			{
				j = max(x->nb_mass, npoints);
				for (i=0; i< j; i++)
				{
					x->mass[i].forceZ += vec[i].w_float;
				}
			}
			else if ( argv[1].a_type == A_SYMBOL )
			{
				i=0;
				j=0;

				while ( (i < x->nb_mass) && (j < npoints) )
				{
					if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
					{
						x->mass[i].forceZ += vec[j].w_float;
						j++;
					}
					i++;
				}
			}
		}
	}
}

