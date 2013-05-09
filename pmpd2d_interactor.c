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

    distance = ( (a * x->mass[i].posX)  + (b * x->mass[i].posY) )  - c;

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
	power = atom_getfloatarg(6, argc, argv);
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
