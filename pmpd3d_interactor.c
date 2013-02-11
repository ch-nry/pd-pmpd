void pmpd3d_iCylinder_i(t_pmpd3d *x, int i, t_float xc, t_float yc, t_float zc, t_float a, t_float b, t_float c, t_float d, t_float R, t_float K, t_float power, t_float Kt, t_float powert, t_float Rmin, t_float Rmax)
{
	t_float profondeur, rayon, Xb, Yb, Zb, Xt, Yt, Zt, tmp;

	profondeur = a * x->mass[i].posX + b * x->mass[i].posY + c * x->mass[i].posZ - d;

	Xb = x->mass[i].posX -xc - profondeur * a;
	Yb = x->mass[i].posY -yc - profondeur * b;
	Zb = x->mass[i].posZ -zc - profondeur * c;

	rayon = sqrt ( sqr(Xb) + sqr(Yb)  + sqr(Zb) );

	if (rayon != 0)
	{
		Xb /= rayon;  // normalisation
		Yb /= rayon;
		Zb /= rayon;
	}
	else
	{
		Xb = 0;  // normalisation
		Yb = 0;
		Zb = 0;
	}
	
	Xt = b*Zb - c*Yb; // vecteur tengentiel au cercle
	Yt = c*Xb - a*Zb;
	Zt = a*Yb - b*Xb;
	
// Xb, Yb, Zb : vecteur unitaire normal au cercle
// Xt, Yt, Zt : vecteur unitaire tengent au cercle
// rayon : distance au centre.
	
	if ( ((rayon>Rmin) || (Rmin<0)) && ((rayon<Rmax) || (Rmax<0)) )
	{
		rayon -= R;
		
		tmp = pow(K * R, power);
		x->mass[i].forceX += Xb * tmp;
		x->mass[i].forceY += Yb * tmp;
		x->mass[i].forceZ += Zb * tmp;

		tmp = pow(Kt * R, powert);
		x->mass[i].forceX += Xt * tmp;
		x->mass[i].forceY += Yt * tmp;
		x->mass[i].forceZ += Zt * tmp;
	}
}

void pmpd3d_iCylinder(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
	// Argument : 
	// 0 : mass to apply this interactor
	// 1,2,3 : XYZ : center vector of the cylinder
	// 4,5,6 : XYZ : center point of the cylinder
	// 7 : cylinder radius
	// 8 : K
	// [9] : power of the force
	// [10] : Kt
	// [11] : power of the force tengential force
	// [12] : min radium of the interactor
	// [13] : max radium of the interactor
	
	t_float xc, yc, zc, a, b, c, d, r, K, Kt, power, powert, tmp, Rmin, Rmax;
	t_int i;
	
	a = atom_getfloatarg(1, argc, argv);
	b = atom_getfloatarg(2, argc, argv);
	c = atom_getfloatarg(3, argc, argv);

	tmp = sqrt (a*a + b*b + c*c);
	if (tmp != 0)
	{
		a /= tmp;
		b /= tmp;
		c /= tmp;
	}
	else
	{
		a=1;
		b=0;
		c=0;
	}
	xc = atom_getfloatarg(4, argc, argv);
	yc = atom_getfloatarg(5, argc, argv);
	zc = atom_getfloatarg(6, argc, argv);
	
	d = a * xc + b * yc + c * zc;
	
	r = atom_getfloatarg(7, argc, argv);

	K = atom_getfloatarg(8, argc, argv);
	power = atom_getfloatarg(9, argc, argv);
	if (power == 0) power = 1;

	Kt = atom_getfloatarg(10, argc, argv);
	powert = atom_getfloatarg(11, argc, argv);
	if (powert == 0) power = 1;
	
	Rmin = -1;
	Rmax = -1;
	if (argc > 12) { Rmin = atom_getfloatarg(12, argc, argv); }
	if (argc > 13) { Rmax = atom_getfloatarg(13, argc, argv); }
	
	if (atom_getfloatarg(0,argc,argv) == -1) // all
	{
		for (i=0; i < x->nb_mass; i++)
		{
			pmpd3d_iCylinder_i(x, i, xc, yc, zc, a, b, c, d, r, K, power, Kt, powert, Rmin, Rmax);
		}
	}
	else if (argv[0].a_type == A_FLOAT)
	{
		pmpd3d_iCylinder_i(x, atom_getfloatarg(0,argc,argv), xc, yc, zc, a, b, c, d, r, K, power, Kt, powert, Rmin, Rmax);
	}
	else if (argv[0].a_type == A_SYMBOL)
	{
		for (i=0; i < x->nb_mass; i++)
		{
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{
				pmpd3d_iCylinder_i(x, i, xc, yc, zc, a, b, c, d, r, K, power, Kt, powert, Rmin, Rmax);
			}
		}
	}
}

void pmpd3d_iPlane_i(t_pmpd3d *x, int i, t_float a, t_float b, t_float c, t_float d, t_float K, t_float power)
{

	t_float profondeur, force;
	
	profondeur = a * x->mass[i].posX + b * x->mass[i].posY + c * x->mass[i].posZ - d;
	if (profondeur < 0)
	{
		force = K * pow(abs(profondeur), power);
		
		x->mass[i].forceX += a * force;
		x->mass[i].forceY += b * force;
		x->mass[i].forceZ += c * force;
	}
}

void pmpd3d_iPlane(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
	// Argument : 
	// 0 : mass to apply this interactor
	// 1,2,3 : XYZ : vector perpendicular to the plane
	// 4,5,6 : XYZ : one point of the plane
	// 7 : K
	// 8 : power of the force
	
	// ax+by+cz-d=0
	// d est tel que aXcenter +bYcenter + cYcenter = d
	t_float a, b, c, d, K, power, tmp;
	t_int i;

	a = atom_getfloatarg(1, argc, argv);
	b = atom_getfloatarg(2, argc, argv);
	c = atom_getfloatarg(3, argc, argv);

	tmp = sqrt (a*a + b*b + c*c);
	if (tmp != 0)
	{
		a /= tmp;
		b /= tmp;
		c /= tmp;
	}
	else
	{
		a=1;
		b=0;
		c=0;
	}

	d = a * atom_getfloatarg(4, argc, argv) + b * atom_getfloatarg(5, argc, argv) + c * atom_getfloatarg(6, argc, argv);
	
	K = atom_getfloatarg(7, argc, argv);
	power = atom_getfloatarg(8, argc, argv);
	if (power == 0) power = 1;
	
	if (atom_getfloatarg(0,argc,argv) == -1) // all
	{
		for (i=0; i < x->nb_mass; i++)
		{
			pmpd3d_iPlane_i(x, i, a, b, c, d, K, power);
		}
	}
	else if (argv[0].a_type == A_FLOAT)
	{
		pmpd3d_iPlane_i(x,atom_getfloatarg(0,argc,argv), a, b, c, d, K, power);
	}
	else if (argv[0].a_type == A_SYMBOL)
	{
		for (i=0; i < x->nb_mass; i++)
		{
			if (atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
			{
				pmpd3d_iPlane_i(x, i, a, b, c, d, K, power);
			}
		}
	}	
}
