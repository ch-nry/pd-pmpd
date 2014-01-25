

void pmpd2d_iCircle_i(t_pmpd2d *x, int i, t_float a, t_float b, t_float r, t_float K, t_float power, t_float Kt, t_float powert, t_float Rmin, t_float Rmax)
{
	t_float distance, X, Y, rayon, tmp;

	X = x->mass[i].posX - a;
	Y = x->mass[i].posY - b;

	rayon = sqrt ( sqr(X) + sqr(Y) );
	distance = rayon - r;
	
	if (rayon != 0)
	{
		X /= rayon;  // normalisation
		Y /= rayon;
	}
	else
	{
		X = 0;  // normalisation
		Y = 0;
	}
	
// X, Y : vecteur unitaire normal au cercle
// rayon : distance au centre.
	
	if ( (distance>Rmin) && (distance<=Rmax) )
	{
		tmp = -pow_ch(K * distance, power);
		x->mass[i].forceX += X * tmp;
		x->mass[i].forceY += Y * tmp;
		tmp = -pow_ch(Kt * distance, power);
		x->mass[i].forceX += -Y * tmp;
		x->mass[i].forceY += X * tmp;
		
	}
}

void pmpd2d_iCircle(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
	// Argument : 
	// 0 : mass to apply this interactor
	// 1,2 : XY : center of the Circle
	// 3 : Circle radius
	// 4 : K
	// [5] : power of the force
	// [6] : Kt
	// [7] : power of the tengential force
	// [8] : min radium of the interactor
	// [9] : max radium of the interactor
	
	
	t_float a, b, R, K, power, Kt, powert, Rmin, Rmax;
	t_int i;

	if (!((argc>=5) && (argv[1].a_type == A_FLOAT)&& (argv[2].a_type == A_FLOAT)&& (argv[3].a_type == A_FLOAT) ))
	{
		post("bad argument for iCircle");
		return;
	}
	
	a = atom_getfloatarg(1, argc, argv);
	b = atom_getfloatarg(2, argc, argv);

	R = atom_getfloatarg(3, argc, argv);

	K = atom_getfloatarg(4, argc, argv);
	power = atom_getfloatarg(5, argc, argv);
	if (power == 0) power = 1;
	
	Kt = atom_getfloatarg(6, argc, argv);
	powert = atom_getfloatarg(7, argc, argv);
	if (powert == 0) power = 1;
	
	Rmin = 0;
	if ((argc>=9) && (argv[6].a_type == A_FLOAT)) { Rmin = (atom_getfloatarg(8,argc,argv));}
	Rmax =  1000000;
	if ((argc>=10) && (argv[7].a_type == A_FLOAT)) { Rmax = (atom_getfloatarg(9,argc,argv));}

	if ((argc>0) && (argv[0].a_type == A_FLOAT) && (atom_getfloatarg(0,argc,argv) == -1)) // all
	{
		for (i=0; i < x->nb_mass; i++)
		{
			pmpd2d_iCircle_i(x, i, a, b, R, K, power, Kt, powert, Rmin, Rmax);
		}
	}
	else if (((argc>0) && argv[0].a_type == A_FLOAT))
	{
		pmpd2d_iCircle_i(x, atom_getfloatarg(0,argc,argv), a, b, R, K, power, Kt, powert, Rmin, Rmax);
	}
	else if ((argc>0) && (argv[0].a_type == A_SYMBOL))
	{
		for (i=0; i < x->nb_mass; i++)
		{
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{
				pmpd2d_iCircle_i(x, i, a, b, R, K, power, Kt, powert, Rmin, Rmax);
			}
		}
	}	
}

// --------------------------------------------------------

void pmpd2d_iLine_i(t_pmpd2d *x, int i, t_float a, t_float b, t_float c, t_float K, t_float power, t_float Rmin, t_float Rmax)
{
    t_float distance, force;

    distance = (a * x->mass[i].posX)  + (b * x->mass[i].posY) + c;

	if ( (distance>Rmin) && (distance<=Rmax) )
	{
		force = -pow_ch(K * distance, power);
		x->mass[i].forceX += a * force;
		x->mass[i].forceY += b * force;
	}
}

void pmpd2d_iLine(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
	// Argument : 
	// 0 : mass to apply this interactor
	// 1, 2 : X1 Y1 : 1st point of the line
	// 3, 4 : X2 Y2 : 2nd point of the line
	// 5 : K
	// [6] : power of the force
	// [7] : min radium of the interactor
	// [8] : max radium of the interactor
	
	
	t_float a, b, c, X1, X2, Y1, Y2, K, power, tmp, Rmin, Rmax;
	t_int i;

	if (!((argc>=6) && (argv[1].a_type == A_FLOAT) && (argv[2].a_type == A_FLOAT) && (argv[3].a_type == A_FLOAT) && (argv[4].a_type == A_FLOAT) && (argv[5].a_type == A_FLOAT) ))
	{
		post("bad argument for iLine");
		return;
	}
	
	X1 = atom_getfloatarg(1, argc, argv);
	Y1 = atom_getfloatarg(2, argc, argv);

	X2 = atom_getfloatarg(3, argc, argv);
	Y2 = atom_getfloatarg(4, argc, argv);

    a = Y2 - Y1;
    b = X1 - X2;
    tmp = sqrt(a*a + b*b);
    if (tmp == 0)
    {
        a = 1;
        b = 0;
        tmp = 1;
    }
    a /= tmp;
    b /= tmp;
    c = - (a * X1 + b * Y1); 
    // line equation : aX + bY + c = 0 

	K = atom_getfloatarg(5, argc, argv);
	
	power = 1;
	if ((argc>=7) && (argv[6].a_type == A_FLOAT)) { power = atom_getfloatarg(6, argc, argv);}
	if (power == 0) power = 1;
	
	Rmin = -1000000;
	if ((argc>=8) && (argv[7].a_type == A_FLOAT)) { Rmin = (atom_getfloatarg(7,argc,argv));}
	Rmax =  1000000;
	if ((argc>=9) && (argv[8].a_type == A_FLOAT)) { Rmax = (atom_getfloatarg(8,argc,argv));}

	if ((argc>0) && (argv[0].a_type == A_FLOAT) && (atom_getfloatarg(0,argc,argv) == -1)) // all
	{
		for (i=0; i < x->nb_mass; i++)
		{
			pmpd2d_iLine_i(x, i, a, b, c, K, power, Rmin, Rmax);
		}
	}
	else if (((argc>0) && argv[0].a_type == A_FLOAT))
	{
		pmpd2d_iLine_i(x, atom_getfloatarg(0,argc,argv), a, b, c, K, power, Rmin, Rmax);
	}
	else if ((argc>0) && (argv[0].a_type == A_SYMBOL))
	{
		for (i=0; i < x->nb_mass; i++)
		{
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{
				pmpd2d_iLine_i(x, i, a, b, c, K, power, Rmin, Rmax);
			}
		}
	}	
}

// --------------------------------------------------------

void pmpd2d_iTable_i(t_pmpd2d *x, int i, t_float zone_x_min, t_float zone_x_max, t_float zone_y_min, t_float zone_y_max, int taille_x, int taille_y, t_word *tableX, t_word *tableY)
{
	t_float Xtable, Ytable, Xindex, Yindex, force1, force2, force;
	int index;
	 
	if ( (x->mass[i].posX >= zone_x_min) && (x->mass[i].posX < zone_x_max) && (x->mass[i].posY >= zone_y_min) && (x->mass[i].posY < zone_y_max) )
	{
		Xtable = (x->mass[i].posX - zone_x_min) / (zone_x_max - zone_x_min);
		Ytable = (x->mass[i].posY - zone_y_min) / (zone_y_max - zone_y_min);
		Xtable *= taille_x - 1; //from [ 0 to table size - 1[
		Ytable *= taille_y - 1; 
		index = (int)Xtable;
		index += (int)Ytable*taille_y;
		Xtable = Xtable - (int)(Xtable);
		Ytable = Ytable - (int)(Ytable);
		force1 = (1-Xtable) * tableX[index].w_float + (Xtable) * tableX[index+1].w_float ;
		force2 = (1-Xtable) * tableX[index+taille_y].w_float + (Xtable) * tableX[index+1+taille_y].w_float;
		force = (1-Ytable) * force1 + Ytable * force2;
		x->mass[i].forceX += force;
		force1 = (1-Xtable) * tableY[index].w_float + (Xtable) * tableY[index+1].w_float ;
		force2 = (1-Xtable) * tableY[index+taille_y].w_float + (Xtable) * tableY[index+1+taille_y].w_float;
		force = (1-Ytable) * force1 + Ytable * force2;
		x->mass[i].forceY += force;
	}
}

void pmpd2d_iTable(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
	// Argument : 
	// 0 : mass to apply this interactor
	// 1, 2 : Xmin, Xmax 
	// 3, 4 : Ymin, Ymax physical location of the interator
	// 5, 6 : matrix size
	// 7, 8 : table name containing the matrix
	
	t_garray *a1, *a2;
    int npoints1, npoints2;
    t_word *vec1, *vec2;
    
    t_float Xmin, Xmax, Ymin, Ymax;
    int X, Y, i;
    
	if (!((argc==9) && (argv[1].a_type == A_FLOAT) && (argv[2].a_type == A_FLOAT) &&
		(argv[3].a_type == A_FLOAT) && (argv[4].a_type == A_FLOAT) && (argv[5].a_type == A_FLOAT) && 
		(argv[6].a_type == A_FLOAT) && (argv[7].a_type == A_SYMBOL) && (argv[8].a_type == A_SYMBOL) ) )
	{
		pd_error(x,"bad argument for iTable");
		return;
	}

	if (!(a1 = (t_garray *)pd_findbyclass(atom_getsymbolarg(7,argc,argv), garray_class)))
		pd_error(x, "%s: no such array", atom_getsymbolarg(7,argc,argv)->s_name);
	else if (!garray_getfloatwords(a1, &npoints1, &vec1))
		pd_error(x, "%s: bad template for tabLink", atom_getsymbolarg(7,argc,argv)->s_name);
	else if (!(a2 = (t_garray *)pd_findbyclass(atom_getsymbolarg(8,argc,argv), garray_class)))
		pd_error(x, "%s: no such array", atom_getsymbolarg(8,argc,argv)->s_name);
	else if (!garray_getfloatwords(a2, &npoints2, &vec2))
		pd_error(x, "%s: bad template for tabLink", atom_getsymbolarg(8,argc,argv)->s_name);
	else if ( ( npoints1 < atom_getfloatarg(5,argc,argv) * atom_getfloatarg(6,argc,argv) ) || ( npoints2 < atom_getfloatarg(5,argc,argv) * atom_getfloatarg(6,argc,argv) ) )
		pd_error(x, "not enough point in tables for iTable");
	else
	{
		 
		Xmin = atom_getfloatarg(1, argc, argv);
		Xmax = atom_getfloatarg(2, argc, argv);
		Ymin = atom_getfloatarg(3, argc, argv);
		Ymax = atom_getfloatarg(4, argc, argv);
		X = atom_getfloatarg(5, argc, argv);
		Y = atom_getfloatarg(6, argc, argv);
		
		if ( argv[0].a_type == A_FLOAT )
		{
			pmpd2d_iTable_i(x, (int)atom_getfloatarg(0,argc,argv), Xmin, Xmax, Ymin, Ymax, X, Y, vec1, vec2);
		}
		else if ( argv[0].a_type == A_SYMBOL )
		{	for (i=0; i < x->nb_mass; i++)
			{	if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
				{
					pmpd2d_iTable_i(x, i, Xmin, Xmax, Ymin, Ymax, X, Y, vec1, vec2);
				}
			}
		}
	}
}
