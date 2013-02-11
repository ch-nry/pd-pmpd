int test_3d_mass(int i, t_pmpd3d *x, int argc, t_atom *argv)
{
	t_int tmp, tmp2;
	t_int j = 1;
	while (j < argc) 
	{
		if (atom_getsymbolarg(j,argc,argv) == gensym("Id") )
		{
			if ( x->mass[i].Id != atom_getsymbolarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if (atom_getsymbolarg(j,argc,argv) == gensym("posXSup") )
		{
			if ( x->mass[i].posX < atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("posXInf") )
		{
			if ( x->mass[i].posX >= atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if (atom_getsymbolarg(j,argc,argv) == gensym("posYSup") )
		{
			if ( x->mass[i].posY < atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("posYInf") )
		{
			if ( x->mass[i].posY >= atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if (atom_getsymbolarg(j,argc,argv) == gensym("posZSup") )
		{
			if ( x->mass[i].posZ < atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("posZInf") )
		{
			if ( x->mass[i].posZ >= atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("speedXSup") )
		{
			if ( x->mass[i].speedX < atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("speedXInf") )
		{
			if ( x->mass[i].speedX >= atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("speedYSup") )
		{
			if ( x->mass[i].speedY < atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("speedYInf") )
		{
			if ( x->mass[i].speedY >= atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("speedZSup") )
		{
			if ( x->mass[i].speedZ < atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("speedZInf") )
		{
			if ( x->mass[i].speedZ >= atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("speedSup") )
		{
			tmp  = x->mass[i].speedX * x->mass[i].speedX;
			tmp += x->mass[i].speedY * x->mass[i].speedY;
			tmp += x->mass[i].speedZ * x->mass[i].speedZ;

			if ( ( tmp < atom_getfloatarg(j+1,argc,argv)) * atom_getfloatarg(j+1,argc,argv) ) { return(0); }

			j+= 2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("speedInf") )
		{
			tmp  = x->mass[i].speedX * x->mass[i].speedX;
			tmp += x->mass[i].speedY * x->mass[i].speedY;
			tmp += x->mass[i].speedZ * x->mass[i].speedZ;

			if ( ( tmp >= atom_getfloatarg(j+1,argc,argv)) * atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if (atom_getsymbolarg(j,argc,argv) == gensym("distanceCircleInf"))
		{ 
			tmp = x->mass[i].posX - atom_getfloatarg(j+1,argc,argv);
			tmp *= tmp;
			tmp2 = x->mass[i].posY - atom_getfloatarg(j+2,argc,argv);
			tmp = tmp + tmp2 * tmp2;
			tmp2 = x->mass[i].posZ - atom_getfloatarg(j+3,argc,argv);
			tmp = tmp + tmp2 * tmp2;
			tmp2 = atom_getfloatarg(j+4,argc,argv);

			if ( tmp >= (tmp2 * tmp2) ) { return(0); }
			j += 5;
		}
		else if (atom_getsymbolarg(j,argc,argv) == gensym("distanceCircleSup"))
		{ 
			tmp = x->mass[i].posX - atom_getfloatarg(j+1,argc,argv);
			tmp *= tmp;
			tmp2 = x->mass[i].posY - atom_getfloatarg(j+2,argc,argv);
			tmp = tmp + tmp2 * tmp2;
			tmp2 = x->mass[i].posZ - atom_getfloatarg(j+3,argc,argv);
			tmp = tmp + tmp2 * tmp2;
			tmp2 = atom_getfloatarg(j+4,argc,argv);

			if ( tmp < (tmp2 * tmp2) ) { return(0); }
			j += 5;
		}
		else if (atom_getsymbolarg(j,argc,argv) == gensym("massSup"))
		{ 
			tmp = 1/ x->mass[i].invM;
			if ( tmp < atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j += 2;
		}
		else if (atom_getsymbolarg(j,argc,argv) == gensym("massInf"))
		{ 
			tmp = 1/ x->mass[i].invM;
			if ( tmp >= atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j += 2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("forceSup") )
		{
			tmp  = x->mass[i].forceX * x->mass[i].forceX;
			tmp += x->mass[i].forceY * x->mass[i].forceY;
			tmp += x->mass[i].forceZ * x->mass[i].forceZ;

			if ( ( tmp < atom_getfloatarg(j+1,argc,argv)) * atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+= 2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("forceInf") )
		{
			tmp  = x->mass[i].forceX * x->mass[i].forceX;
			tmp += x->mass[i].forceY * x->mass[i].forceY;
			tmp += x->mass[i].forceZ * x->mass[i].forceZ;

			if ( ( tmp >= atom_getfloatarg(j+1,argc,argv)) * atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+= 2;
		}
		else if (atom_getsymbolarg(j,argc,argv) == gensym("forceXSup") )
		{
			if ( x->mass[i].forceX < atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if (atom_getsymbolarg(j,argc,argv) == gensym("forceXInf") )
		{
			if ( x->mass[i].forceX >= atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if (atom_getsymbolarg(j,argc,argv) == gensym("forceYSup") )
		{
			if ( x->mass[i].forceY < atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if (atom_getsymbolarg(j,argc,argv) == gensym("forceYInf") )
		{
			if ( x->mass[i].forceY >= atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if (atom_getsymbolarg(j,argc,argv) == gensym("forceZSup") )
		{
			if ( x->mass[i].forceZ < atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if (atom_getsymbolarg(j,argc,argv) == gensym("forceZInf") )
		{
			if ( x->mass[i].forceZ >= atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else
		{
			post("nor reconize %s",atom_getsymbolarg(j,argc,argv));
			j+=1;
		}
	}
	return(1);	
}

int test_3d_link(int i, t_pmpd3d *x, int argc, t_atom *argv)
{
	t_int j, tmp, tmp2;
	j = 1;
	
	while (j < argc-1) 
	{
		if (atom_getsymbolarg(j,argc,argv) == gensym("Id") )
		{
			if ( x->link[i].Id != atom_getsymbolarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
			else if (atom_getsymbolarg(j,argc,argv) == gensym("forceXSup") )
		{
			if ( x->link[i].forceX < atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("forceXInf") )
		{
			if ( x->link[i].forceX >= atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
			else if (atom_getsymbolarg(j,argc,argv) == gensym("forceYSup") )
		{
			if ( x->link[i].forceY < atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("forceYInf") )
		{
			if ( x->link[i].forceY >= atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
			else if (atom_getsymbolarg(j,argc,argv) == gensym("forceZSup") )
		{
			if ( x->link[i].forceZ < atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("forceZInf") )
		{
			if ( x->link[i].forceZ >= atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
			else if (atom_getsymbolarg(j,argc,argv) == gensym("forceSup") )
		{
			tmp = x->link[i].forceX;
			tmp *= tmp;
			tmp2 = x->link[i].forceY;
			tmp = tmp + tmp2*tmp2;				
			tmp2 = x->link[i].forceZ;
			tmp = tmp + tmp2*tmp2;
			
			tmp2 = atom_getfloatarg(j+1,argc,argv);
			tmp2 *= tmp2;
			
			if ( tmp < tmp2 ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("forceInf") )
		{
			tmp = x->link[i].forceX;
			tmp = tmp * tmp;
			tmp2 = x->link[i].forceY;
			tmp = tmp + tmp2*tmp2;				
			tmp2 = x->link[i].forceZ;
			tmp = tmp + tmp2*tmp2;
			
			tmp2 = atom_getfloatarg(j+1,argc,argv);
			tmp2 *= tmp2;
			
			if ( tmp >= tmp2 ) { return(0); }
			j+=2;
		}
			else if (atom_getsymbolarg(j,argc,argv) == gensym("lengthSup") )
		{
			if ( x->link[i].distance < atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("lengthInf") )
		{
			if ( x->link[i].distance >= atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("lengthXInf") )
		{
			tmp = abs(x->link[i].mass1->posX - x->link[i].mass2->posX);
			if ( tmp >= atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("lengthXSup") )
		{
			tmp = abs(x->link[i].mass1->posX - x->link[i].mass2->posX);
			if ( tmp < atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}		
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("lengthYInf") )
		{
			tmp = abs(x->link[i].mass1->posY - x->link[i].mass2->posY);
			if ( tmp >= atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("lengthYSup") )
		{
			tmp = abs(x->link[i].mass1->posY - x->link[i].mass2->posY);
			if ( tmp < atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("lengthZInf") )
		{
			tmp = abs(x->link[i].mass1->posZ - x->link[i].mass2->posZ);
			if ( tmp >= atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("lengthZSup") )
		{
			tmp = abs(x->link[i].mass1->posZ - x->link[i].mass2->posZ);
			if ( tmp < atom_getfloatarg(j+1,argc,argv) ) { return(0); }
			j+=2;
		}
		else if ( atom_getsymbolarg(j,argc,argv) == gensym("connectedTo") )
		{
			if (argv[j+1].a_type == A_SYMBOL)
			{
				if (!( (x->link[i].mass1->Id == atom_getsymbolarg(j+1,argc,argv)) || (x->link[i].mass2->Id == atom_getsymbolarg(j+1,argc,argv)) )) 
				{ return(0); }
				j+=2;				
			}
			else if (argv[j+1].a_type == A_FLOAT)
			{
				if (!( (x->link[i].mass1->num == atom_getfloatarg(j+1,argc,argv)) || (x->link[i].mass2->num == atom_getfloatarg(j+1,argc,argv)) ))
				{ return(0); }
				j+=2;
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
}

//----------------------------------------------------------------------

void pmpd3d_testMass(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
	t_int i;
	t_atom std_out[2];

	SETSYMBOL(&(std_out[0]),atom_getsymbolarg(0,argc,argv));

	for (i=0; i < x->nb_mass; i++)
	{
		if (test_3d_mass(i,x,argc,argv))
		{
			SETFLOAT(&(std_out[1]),i);
			outlet_anything(x->main_outlet, gensym("testMass"),2,std_out);
		}
	}
}

void pmpd3d_testLink(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
	t_int i;
	t_atom std_out[2];

	SETSYMBOL(&(std_out[0]),atom_getsymbolarg(0,argc,argv));

	for (i=0; i < x->nb_link; i++)
	{
		if (test_3d_link(i,x,argc,argv))
		{
			SETFLOAT(&(std_out[1]),i);
			outlet_anything(x->main_outlet, gensym("testLink"),1,std_out);
		}
	}
}

void pmpd3d_testLinkT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, vecsize;
    t_garray *a;
    t_word *vec;
    
    if (argv[0].a_type == A_SYMBOL)
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
			int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize);
            
            for (i=0; i < taille_max; i++)
            {
				vec[i].w_float = (t_float)test_3d_link(i,x,argc,argv);
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_testMassT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, vecsize;
    t_garray *a;
    t_word *vec;
    
    if (argv[0].a_type == A_SYMBOL)
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
			int taille_max = x->nb_mass;
            taille_max = min(taille_max, vecsize);
            
            for (i=0; i < taille_max; i++)
            {
				vec[i].w_float = (t_float)test_3d_mass(i,x,argc,argv);
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_testLinkL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_atom list[x->nb_link+1];

    for (i=0; i < x->nb_link; i++)
	{
		SETFLOAT(&(list[i+1]), test_3d_link(i,x,argc,argv));
	}
	SETSYMBOL(&(list[0]),atom_getsymbolarg(0,argc,argv));
	outlet_anything(x->main_outlet, gensym("testLink"),x->nb_link+1 , list);
}

void pmpd3d_testMassL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_atom list[x->nb_mass+1];

    for (i=0; i < x->nb_mass; i++)
	{
		SETFLOAT(&(list[i+1]), test_3d_link(i,x,argc,argv));
	}
	SETSYMBOL(&(list[0]),atom_getsymbolarg(0,argc,argv));
	outlet_anything(x->main_outlet, gensym("testMass"),x->nb_mass+1 , list);
}
