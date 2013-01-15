void pmpd3d_massesPosL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[3*x->nb_mass];
    
    if (argc==0) {
		for (i=0; i < x->nb_mass; i++)
		{
			SETFLOAT(&(pos_list[3*i]  ),x->mass[i].posX);
			SETFLOAT(&(pos_list[3*i+1]),x->mass[i].posY);
			SETFLOAT(&(pos_list[3*i+2]),x->mass[i].posZ);
		}
		outlet_anything(x->main_outlet, gensym("massesPosL"),3*x->nb_mass , pos_list);       
    }
    else
    if ((argc==1) && (argv[1].a_type == A_SYMBOL)) {
        i = 0;
        j = 0;
        while  (j < x->nb_mass){
            if (atom_getsymbolarg(0,argc,argv) == x->mass[j].Id) {
				SETFLOAT(&(pos_list[3*i]  ),x->mass[i].posX);
				SETFLOAT(&(pos_list[3*i+1]),x->mass[i].posY);
				SETFLOAT(&(pos_list[3*i+2]),x->mass[i].posZ);
                i++;
            }
        }
        outlet_anything(x->main_outlet, gensym("massesPosL"),3*i , pos_list);
    }
    else 
    if ((argc==1) && (argv[0].a_type == A_FLOAT)) {
        SETFLOAT(&(pos_list[0]),x->mass[(int)atom_getfloatarg(0, argc, argv)].posX);
		SETFLOAT(&(pos_list[1]),x->mass[(int)atom_getfloatarg(0, argc, argv)].posY);
		SETFLOAT(&(pos_list[2]),x->mass[(int)atom_getfloatarg(0, argc, argv)].posZ);
			
        outlet_anything(x->main_outlet, gensym("massesPosL"),1 , pos_list);        
    }
}

void pmpd3d_massesForcesL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[3*x->nb_mass];
 
    if (argc==0) {
        for (i=0; i< x->nb_mass; i++) {
        SETFLOAT(&(pos_list[3*i]  ),x->mass[i].forceX);
        SETFLOAT(&(pos_list[3*i+1]),x->mass[i].forceY);
        SETFLOAT(&(pos_list[3*i+2]),x->mass[i].forceZ);
        }
        outlet_anything(x->main_outlet, gensym("massesForcesL"),3*x->nb_mass , pos_list);          
    }
    else
    if ((argc==1) && (argv[1].a_type == A_SYMBOL)) {
        i = 0;
        j = 0;
        while  (j < x->nb_mass){
            if (atom_getsymbolarg(0,argc,argv) == x->mass[j].Id) {
				SETFLOAT(&(pos_list[3*i]  ),x->mass[i].forceX);
				SETFLOAT(&(pos_list[3*i+1]),x->mass[i].forceY);
				SETFLOAT(&(pos_list[3*i+2]),x->mass[i].forceZ);
                i++;
            }
        }
        outlet_anything(x->main_outlet, gensym("massesForcesL"),3*i , pos_list);
    }
    else 
    if ((argc==1) && (argv[0].a_type == A_FLOAT)) {
        SETFLOAT(&(pos_list[0]),x->mass[(int)atom_getfloatarg(0, argc, argv)].forceX);
        SETFLOAT(&(pos_list[1]),x->mass[(int)atom_getfloatarg(0, argc, argv)].forceY);
        SETFLOAT(&(pos_list[2]),x->mass[(int)atom_getfloatarg(0, argc, argv)].forceZ);
        
        outlet_anything(x->main_outlet, gensym("massesForcesZL"),3 , pos_list);        
    }
}

void pmpd3d_massesSpeedsL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[3*x->nb_mass];
	
    if (argc==0) {
		for (i=0; i< x->nb_mass; i++) {
			SETFLOAT(&(pos_list[3*i]  ),x->mass[i].speedX);
			SETFLOAT(&(pos_list[3*i+1]),x->mass[i].speedY);
			SETFLOAT(&(pos_list[3*i+2]),x->mass[i].speedZ);
		}
		outlet_anything(x->main_outlet, gensym("massesSpeedsL"),3*x->nb_mass , pos_list);         
    }
    else
    if ((argc==1) && (argv[1].a_type == A_SYMBOL)) {
        i = 0;
        j = 0;
        while  (j < x->nb_mass){
            if (atom_getsymbolarg(0,argc,argv) == x->mass[j].Id) {
				SETFLOAT(&(pos_list[3*i]  ),x->mass[i].speedX);
				SETFLOAT(&(pos_list[3*i+1]),x->mass[i].speedY);
				SETFLOAT(&(pos_list[3*i+2]),x->mass[i].speedZ);
                i++;
            }
        }
        outlet_anything(x->main_outlet, gensym("massesSpeedsL"),3*i , pos_list);
    }
    else 
    if ((argc==1) && (argv[0].a_type == A_FLOAT)) {
        SETFLOAT(&(pos_list[0]),x->mass[(int)atom_getfloatarg(0, argc, argv)].speedX);
        SETFLOAT(&(pos_list[1]),x->mass[(int)atom_getfloatarg(0, argc, argv)].speedY);
        SETFLOAT(&(pos_list[2]),x->mass[(int)atom_getfloatarg(0, argc, argv)].speedZ);
        
        outlet_anything(x->main_outlet, gensym("massesSpeedsL"),3 , pos_list);        
    }
}

void pmpd3d_massesPosXL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_mass];
    
    if (argc==0) {
        for (i=0; i < x->nb_mass; i++)
        {
            SETFLOAT(&(pos_list[i]),x->mass[i].posX);
        }
        outlet_anything(x->main_outlet, gensym("massesPosXL"),x->nb_mass , pos_list);        
    }
    else
    if ((argc==1) && (argv[1].a_type == A_SYMBOL)) {
        i = 0;
        j = 0;
        while  (j < x->nb_mass){
            if (atom_getsymbolarg(0,argc,argv) == x->mass[j].Id) {
                SETFLOAT(&(pos_list[i]),x->mass[i].posX);
                i++;
            }
        }
        outlet_anything(x->main_outlet, gensym("massesPosXL"),i , pos_list);
    }
    else 
    if ((argc==1) && (argv[0].a_type == A_FLOAT)) {
        SETFLOAT(&(pos_list[0]),x->mass[(int)atom_getfloatarg(0, argc, argv)].posX);
        outlet_anything(x->main_outlet, gensym("massesPosXL"),1 , pos_list);        
    }
}

void pmpd3d_massesForcesXL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_mass];
 
    if (argc==0) {
        for (i=0; i< x->nb_mass; i++)
        {
            SETFLOAT(&(pos_list[i]),x->mass[i].forceX);
        }
        outlet_anything(x->main_outlet, gensym("massesForcesXL"),x->nb_mass , pos_list);          
    }
    else
    if ((argc==1) && (argv[1].a_type == A_SYMBOL)) {
        i = 0;
        j = 0;
        while  (j < x->nb_mass){
            if (atom_getsymbolarg(0,argc,argv) == x->mass[j].Id) {
                SETFLOAT(&(pos_list[i]),x->mass[i].forceX);
                i++;
            }
        }
        outlet_anything(x->main_outlet, gensym("massesForcesXL"),i , pos_list);
    }
    else 
    if ((argc==1) && (argv[0].a_type == A_FLOAT)) {
        SETFLOAT(&(pos_list[0]),x->mass[(int)atom_getfloatarg(0, argc, argv)].forceX);
        outlet_anything(x->main_outlet, gensym("massesForcesXL"),1 , pos_list);        
    }
}

void pmpd3d_massesSpeedsXL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_mass];
	
    if (argc==0) {
		for (i=0; i< x->nb_mass; i++)
		{
			SETFLOAT(&(pos_list[i]),x->mass[i].speedX);
		}
		outlet_anything(x->main_outlet, gensym("massesSpeedsXL"),x->nb_mass , pos_list);         
    }
    else
    if ((argc==1) && (argv[1].a_type == A_SYMBOL)) {
        i = 0;
        j = 0;
        while  (j < x->nb_mass){
            if (atom_getsymbolarg(0,argc,argv) == x->mass[j].Id) {
                SETFLOAT(&(pos_list[i]),x->mass[i].speedX);
                i++;
            }
        }
        outlet_anything(x->main_outlet, gensym("massesSpeedsXL"),i , pos_list);
    }
    else 
    if ((argc==1) && (argv[0].a_type == A_FLOAT)) {
        SETFLOAT(&(pos_list[0]),x->mass[(int)atom_getfloatarg(0, argc, argv)].speedX);
        outlet_anything(x->main_outlet, gensym("massesSpeedsXL"),1 , pos_list);        
    }
}

void pmpd3d_massesPosYL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_mass];
    
    if (argc==0) {
        for (i=0; i < x->nb_mass; i++)
        {
            SETFLOAT(&(pos_list[i]),x->mass[i].posY);
        }
        outlet_anything(x->main_outlet, gensym("massesPosYL"),x->nb_mass , pos_list);         
    }
    else
    if ((argc==1) && (argv[1].a_type == A_SYMBOL)) {
        i = 0;
        j = 0;
        while  (j < x->nb_mass){
            if (atom_getsymbolarg(0,argc,argv) == x->mass[j].Id) {
                SETFLOAT(&(pos_list[i]),x->mass[i].posY);
                i++;
            }
        }
        outlet_anything(x->main_outlet, gensym("massesPosYL"),i , pos_list);
    }
    else 
    if ((argc==1) && (argv[0].a_type == A_FLOAT)) {
        SETFLOAT(&(pos_list[0]),x->mass[(int)atom_getfloatarg(0, argc, argv)].posY);
        outlet_anything(x->main_outlet, gensym("massesPosYL"),1 , pos_list);        
    }
}

void pmpd3d_massesForcesYL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_mass];
 
    if (argc==0) {
        for (i=0; i< x->nb_mass; i++)
        {
            SETFLOAT(&(pos_list[i]),x->mass[i].forceY);
        }
        outlet_anything(x->main_outlet, gensym("massesForcesYL"),x->nb_mass , pos_list);          
    }
    else
    if ((argc==1) && (argv[1].a_type == A_SYMBOL)) {
        i = 0;
        j = 0;
        while  (j < x->nb_mass){
            if (atom_getsymbolarg(0,argc,argv) == x->mass[j].Id) {
                SETFLOAT(&(pos_list[i]),x->mass[i].forceY);
                i++;
            }
        }
        outlet_anything(x->main_outlet, gensym("massesForcesYL"),i , pos_list);
    }
    else 
    if ((argc==1) && (argv[0].a_type == A_FLOAT)) {
        SETFLOAT(&(pos_list[0]),x->mass[(int)atom_getfloatarg(0, argc, argv)].forceY);
        outlet_anything(x->main_outlet, gensym("massesForcesYL"),1 , pos_list);        
    }
}

void pmpd3d_massesSpeedsYL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_mass];
	
    if (argc==0) {
		for (i=0; i< x->nb_mass; i++)
		{
			SETFLOAT(&(pos_list[i]),x->mass[i].speedY);
		}
		outlet_anything(x->main_outlet, gensym("massesSpeedsYL"),x->nb_mass , pos_list);         
    }
    else
    if ((argc==1) && (argv[1].a_type == A_SYMBOL)) {
        i = 0;
        j = 0;
        while  (j < x->nb_mass){
            if (atom_getsymbolarg(0,argc,argv) == x->mass[j].Id) {
                SETFLOAT(&(pos_list[i]),x->mass[i].speedY);
                i++;
            }
        }
        outlet_anything(x->main_outlet, gensym("massesSpeedsYL"),i , pos_list);
    }
    else 
    if ((argc==1) && (argv[0].a_type == A_FLOAT)) {
        SETFLOAT(&(pos_list[0]),x->mass[(int)atom_getfloatarg(0, argc, argv)].speedY);
        outlet_anything(x->main_outlet, gensym("massesSpeedsYL"),1 , pos_list);        
    }
}

void pmpd3d_massesPosZL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_mass];
    
    if (argc==0) {
        for (i=0; i < x->nb_mass; i++)
        {
            SETFLOAT(&(pos_list[i]),x->mass[i].posZ);
        }
        outlet_anything(x->main_outlet, gensym("massesPosZL"),x->nb_mass , pos_list);         
    }
    else
    if ((argc==1) && (argv[1].a_type == A_SYMBOL)) {
        i = 0;
        j = 0;
        while  (j < x->nb_mass){
            if (atom_getsymbolarg(0,argc,argv) == x->mass[j].Id) {
                SETFLOAT(&(pos_list[i]),x->mass[i].posZ);
                i++;
            }
        }
        outlet_anything(x->main_outlet, gensym("massesPosZL"),i , pos_list);
    }
    else 
    if ((argc==1) && (argv[0].a_type == A_FLOAT)) {
        SETFLOAT(&(pos_list[0]),x->mass[(int)atom_getfloatarg(0, argc, argv)].posZ);
        outlet_anything(x->main_outlet, gensym("massesPosZL"),1 , pos_list);        
    }
}

void pmpd3d_massesForcesZL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_mass];
 
    if (argc==0) {
        for (i=0; i< x->nb_mass; i++)
        {
            SETFLOAT(&(pos_list[i]),x->mass[i].forceZ);
        }
        outlet_anything(x->main_outlet, gensym("massesForcesZL"),x->nb_mass , pos_list);          
    }
    else
    if ((argc==1) && (argv[1].a_type == A_SYMBOL)) {
        i = 0;
        j = 0;
        while  (j < x->nb_mass){
            if (atom_getsymbolarg(0,argc,argv) == x->mass[j].Id) {
                SETFLOAT(&(pos_list[i]),x->mass[i].forceZ);
                i++;
            }
        }
        outlet_anything(x->main_outlet, gensym("massesForcesZL"),i , pos_list);
    }
    else 
    if ((argc==1) && (argv[0].a_type == A_FLOAT)) {
        SETFLOAT(&(pos_list[0]),x->mass[(int)atom_getfloatarg(0, argc, argv)].forceZ);
        outlet_anything(x->main_outlet, gensym("massesForcesZL"),1 , pos_list);        
    }
}

void pmpd3d_massesSpeedsZL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_mass];
	
    if (argc==0) {
		for (i=0; i< x->nb_mass; i++)
		{
			SETFLOAT(&(pos_list[i]),x->mass[i].speedZ);
		}
		outlet_anything(x->main_outlet, gensym("massesSpeedsZL"),x->nb_mass , pos_list);         
    }
    else
    if ((argc==1) && (argv[1].a_type == A_SYMBOL)) {
        i = 0;
        j = 0;
        while  (j < x->nb_mass){
            if (atom_getsymbolarg(0,argc,argv) == x->mass[j].Id) {
                SETFLOAT(&(pos_list[i]),x->mass[i].speedZ);
                i++;
            }
        }
        outlet_anything(x->main_outlet, gensym("massesSpeedsZL"),i , pos_list);
    }
    else 
    if ((argc==1) && (argv[0].a_type == A_FLOAT)) {
        SETFLOAT(&(pos_list[0]),x->mass[(int)atom_getfloatarg(0, argc, argv)].speedZ);
        outlet_anything(x->main_outlet, gensym("massesSpeedsZL"),1 , pos_list);        
    }
}

// ---------------------------------------------------------------------

void pmpd3d_massesPosNormL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_mass];
    
    if (argc==0) {
		for (i=0; i< x->nb_mass; i++) {
			SETFLOAT(&(pos_list[i]),sqrt(sqr(x->mass[i].posX)+sqr(x->mass[i].posY)+sqr(x->mass[i].posZ)));
		}
    outlet_anything(x->main_outlet, gensym("massesPosNormL"),x->nb_mass , pos_list);
    }
    else
    if ((argc==1) && (argv[1].a_type == A_SYMBOL)) {
        i = 0;
        j = 0;
        while  (j < x->nb_mass){
            if (atom_getsymbolarg(0,argc,argv) == x->mass[j].Id) {
				SETFLOAT(&(pos_list[i]),sqrt(sqr(x->mass[i].posX)+sqr(x->mass[i].posY)+sqr(x->mass[i].posZ)));
                i++;
            }
        }
        outlet_anything(x->main_outlet, gensym("massesPosNormL"),i , pos_list);
    }
    else 
    if ((argc==1) && (argv[0].a_type == A_FLOAT)) {
		i=(int)atom_getfloatarg(0, argc, argv);
		SETFLOAT(&(pos_list[0]),sqrt(sqr(x->mass[i].posX)+sqr(x->mass[i].posY)+sqr(x->mass[i].posZ)));
        outlet_anything(x->main_outlet, gensym("massesPosNormL"),1 , pos_list);        
    }
}

void pmpd3d_massesForcesNormL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{   
    int i,j;
    t_atom pos_list[x->nb_mass];
    
    if (argc==0) {
		for (i=0; i< x->nb_mass; i++) {
			SETFLOAT(&(pos_list[i]),sqrt(sqr(x->mass[i].forceX)+sqr(x->mass[i].forceY)+sqr(x->mass[i].forceZ)));
		}
    outlet_anything(x->main_outlet, gensym("massesForcesNormL"),x->nb_mass , pos_list);
    }
    else
    if ((argc==1) && (argv[1].a_type == A_SYMBOL)) {
        i = 0;
        j = 0;
        while  (j < x->nb_mass){
            if (atom_getsymbolarg(0,argc,argv) == x->mass[j].Id) {
				SETFLOAT(&(pos_list[i]),sqrt(sqr(x->mass[i].forceX)+sqr(x->mass[i].forceY)+sqr(x->mass[i].forceZ)));
                i++;
            }
        }
        outlet_anything(x->main_outlet, gensym("massesForcesNormL"),i , pos_list);
    }
    else 
    if ((argc==1) && (argv[0].a_type == A_FLOAT)) {
		i=(int)atom_getfloatarg(0, argc, argv);
        SETFLOAT(&(pos_list[0]),sqrt(sqr(x->mass[i].forceX)+sqr(x->mass[i].forceY)+sqr(x->mass[i].forceZ)));
        outlet_anything(x->main_outlet, gensym("massesForcesNormL"),1 , pos_list);        
    }
}

void pmpd3d_massesSpeedsNormL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_mass];
    
    if (argc==0) {
		for (i=0; i< x->nb_mass; i++) {
			SETFLOAT(&(pos_list[i]),sqrt(sqr(x->mass[i].speedX)+sqr(x->mass[i].speedY)+sqr(x->mass[i].speedZ)));
		}
    outlet_anything(x->main_outlet, gensym("massesSpeedsNormL"),x->nb_mass , pos_list);
    }
    else
    if ((argc==1) && (argv[1].a_type == A_SYMBOL)) {
        i = 0;
        j = 0;
        while  (j < x->nb_mass){
            if (atom_getsymbolarg(0,argc,argv) == x->mass[j].Id) {
				SETFLOAT(&(pos_list[i]),sqrt(sqr(x->mass[i].speedX)+sqr(x->mass[i].speedY)+sqr(x->mass[i].speedZ)));
                i++;
            }
        }
        outlet_anything(x->main_outlet, gensym("massesSpeedsNormL"),i , pos_list);
    }
    else 
    if ((argc==1) && (argv[0].a_type == A_FLOAT)) {
		i=(int)atom_getfloatarg(0, argc, argv);
        SETFLOAT(&(pos_list[i]),sqrt(sqr(x->mass[i].speedX)+sqr(x->mass[i].speedY)+sqr(x->mass[i].speedZ)));
        outlet_anything(x->main_outlet, gensym("massesSpeedsNormL"),1 , pos_list);        
    }
    
}

// ---------------------------------------------------------------------

void pmpd3d_linksPosL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[3*x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[3*i]  ),(x->link[i].mass2->posX + x->link[i].mass1->posX)/2);
			SETFLOAT(&(pos_list[3*i+1]),(x->link[i].mass2->posY + x->link[i].mass1->posY)/2);
			SETFLOAT(&(pos_list[3*i+2]),(x->link[i].mass2->posZ + x->link[i].mass1->posZ)/2);
		}
		outlet_anything(x->main_outlet, gensym("linksPosL"),3*x->nb_link , pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[3*j]  ),(x->link[i].mass2->posX + x->link[i].mass1->posX)/2);
				SETFLOAT(&(pos_list[3*j+1]),(x->link[i].mass2->posY + x->link[i].mass1->posY)/2);
				SETFLOAT(&(pos_list[3*j+2]),(x->link[i].mass2->posZ + x->link[i].mass1->posZ)/2);
				j++;
            }
        }
  		outlet_anything(x->main_outlet, gensym("linksPosL"), 3*j, pos_list);   
    }
}

void pmpd3d_linksLengthL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[3*x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[3*i]  ),x->link[i].mass2->posX - x->link[i].mass1->posX);
			SETFLOAT(&(pos_list[3*i+1]),x->link[i].mass2->posY - x->link[i].mass1->posY);
			SETFLOAT(&(pos_list[3*i+2]),x->link[i].mass2->posZ - x->link[i].mass1->posZ);
		}
		outlet_anything(x->main_outlet, gensym("linksLengthL"),3*x->nb_link , pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[3*j]  ),x->link[i].mass2->posX - x->link[i].mass1->posX);
				SETFLOAT(&(pos_list[3*j+1]),x->link[i].mass2->posY - x->link[i].mass1->posY);
				SETFLOAT(&(pos_list[3*j+2]),x->link[i].mass2->posZ - x->link[i].mass1->posZ);
				j++;
            }
        }
  		outlet_anything(x->main_outlet, gensym("linksLengthL"), 3*j, pos_list);   
    }
}

void pmpd3d_linksPosSpeedL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[3*x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[3*i]  ),(x->link[i].mass2->speedX + x->link[i].mass1->speedX)/2);
			SETFLOAT(&(pos_list[3*i+1]),(x->link[i].mass2->speedY + x->link[i].mass1->speedY)/2);
			SETFLOAT(&(pos_list[3*i+2]),(x->link[i].mass2->speedZ + x->link[i].mass1->speedZ)/2);
		}
		outlet_anything(x->main_outlet, gensym("linksPosSpeedL"),3*x->nb_link , pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[3*j]  ),(x->link[i].mass2->speedX + x->link[i].mass1->speedX)/2);
				SETFLOAT(&(pos_list[3*j+1]),(x->link[i].mass2->speedY + x->link[i].mass1->speedY)/2);
				SETFLOAT(&(pos_list[3*j+2]),(x->link[i].mass2->speedZ + x->link[i].mass1->speedZ)/2);
				j++;
            }
        }
		outlet_anything(x->main_outlet, gensym("linksPosSpeedL"),3*j , pos_list);
    }
}

void pmpd3d_linksLengthSpeedL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[3*x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[3*i]  ),x->link[i].mass2->speedX - x->link[i].mass1->speedX);
			SETFLOAT(&(pos_list[3*i+1]),x->link[i].mass2->speedY - x->link[i].mass1->speedY);
			SETFLOAT(&(pos_list[3*i+2]),x->link[i].mass2->speedZ - x->link[i].mass1->speedZ);
		}
		outlet_anything(x->main_outlet, gensym("linksLengthSpeedL"),3*x->nb_link , pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[3*j]  ),x->link[i].mass2->speedX - x->link[i].mass1->speedX);
				SETFLOAT(&(pos_list[3*j+1]),x->link[i].mass2->speedY - x->link[i].mass1->speedY);
				SETFLOAT(&(pos_list[3*j+2]),x->link[i].mass2->speedZ - x->link[i].mass1->speedZ);
				j++;
            }
        }
		outlet_anything(x->main_outlet, gensym("linksLengthSpeedL"),3*j , pos_list);
    }
}

void pmpd3d_linksPosXL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[i]),(x->link[i].mass1->posX + x->link[i].mass2->posX)/2);
		}
		outlet_anything(x->main_outlet, gensym("linksPosXL"),x->nb_link , pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[j]),(x->link[i].mass1->posX + x->link[i].mass2->posX)/2);
				j++;
            }
        }
		outlet_anything(x->main_outlet, gensym("linksPosXL"),j , pos_list);
    }
}

void pmpd3d_linksLengthXL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[i]),x->link[i].mass2->posX - x->link[i].mass1->posX);
		}
		outlet_anything(x->main_outlet, gensym("linksLengthXL"),x->nb_link , pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[j]),x->link[i].mass2->posX - x->link[i].mass1->posX);
				j++;
            }
        }
		outlet_anything(x->main_outlet, gensym("linksLengthXL"),j , pos_list);
    }
}

void pmpd3d_linksPosSpeedXL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[i]),(x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2);
		}
		outlet_anything(x->main_outlet, gensym("linksPosSpeedXL"),x->nb_link , pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[j]),(x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2);
				j++;
            }
        }
		outlet_anything(x->main_outlet, gensym("linksPosSpeedXL"),j , pos_list);
    }
}

void pmpd3d_linksLengthSpeedXL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[i]),x->link[i].mass2->speedX - x->link[i].mass1->speedX);
		}
		outlet_anything(x->main_outlet, gensym("linksLengthSpeedXL"), x->nb_link, pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[j]),x->link[i].mass2->speedX - x->link[i].mass1->speedX);
				j++;
            }
        }
		outlet_anything(x->main_outlet, gensym("linksPosSpeedXL"), j, pos_list);
    }
}

void pmpd3d_linksPosYL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[i]),(x->link[i].mass1->posY + x->link[i].mass2->posY)/2);
		}
		outlet_anything(x->main_outlet, gensym("linksPosYL"),x->nb_link , pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[j]),(x->link[i].mass1->posY + x->link[i].mass2->posY)/2);
				j++;
            }
        }
		outlet_anything(x->main_outlet, gensym("linksPosYL"), j, pos_list);
    }
}

void pmpd3d_linksLengthYL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[i]),x->link[i].mass2->posY - x->link[i].mass1->posY);
		}
		outlet_anything(x->main_outlet, gensym("linksLengthYL"),x->nb_link , pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[j]),x->link[i].mass2->posY - x->link[i].mass1->posY);
				j++;
            }
        }
		outlet_anything(x->main_outlet, gensym("linksLengthYL"), j, pos_list);
    }
}

void pmpd3d_linksPosSpeedYL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[i]),(x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2);
		}
		outlet_anything(x->main_outlet, gensym("linksPosSpeedYL"),x->nb_link , pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[j]),(x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2);
				j++;
            }
        }
		outlet_anything(x->main_outlet, gensym("linksPosSpeedYL"), j, pos_list);
    }
}

void pmpd3d_linksLengthSpeedYL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[i]),x->link[i].mass2->speedY - x->link[i].mass1->speedY);
		}
		outlet_anything(x->main_outlet, gensym("linksLengthSpeedYL"),x->nb_link , pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[j]),x->link[i].mass2->speedY - x->link[i].mass1->speedY);
				j++;
            }
        }
		outlet_anything(x->main_outlet, gensym("linksLengthSpeedYL"), j, pos_list);
    }
}

void pmpd3d_linksPosZL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[i]),(x->link[i].mass1->posZ + x->link[i].mass2->posZ)/2);
		}
		outlet_anything(x->main_outlet, gensym("linksPosZL"),x->nb_link , pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[j]),(x->link[i].mass1->posZ + x->link[i].mass2->posZ)/2);
				j++;
            }
        }
		outlet_anything(x->main_outlet, gensym("linksPosZL"), j, pos_list);
    }
}

void pmpd3d_linksLengthZL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[i]),x->link[i].mass2->posZ - x->link[i].mass1->posZ);
		}
		outlet_anything(x->main_outlet, gensym("linksLengthZL"),x->nb_link , pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[j]),x->link[i].mass2->posZ - x->link[i].mass1->posZ);
				j++;
            }
        }
		outlet_anything(x->main_outlet, gensym("linksLengthZL"), j, pos_list);
    }
}

void pmpd3d_linksPosSpeedZL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[i]),(x->link[i].mass1->speedZ + x->link[i].mass2->speedZ)/2);
		}
		outlet_anything(x->main_outlet, gensym("linksPosSpeedZL"),x->nb_link , pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[j]),(x->link[i].mass1->speedZ + x->link[i].mass2->speedZ)/2);
				j++;
            }
        }
		outlet_anything(x->main_outlet, gensym("linksPosSpeedZL"), j, pos_list);
    }
}

void pmpd3d_linksLengthSpeedZL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[i]),x->link[i].mass2->speedZ - x->link[i].mass1->speedZ);
		}
		outlet_anything(x->main_outlet, gensym("linksLengthSpeedZL"),x->nb_link , pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[j]),x->link[i].mass2->speedZ - x->link[i].mass1->speedZ);
				j++;
            }
        }
		outlet_anything(x->main_outlet, gensym("linksLengthSpeedZL"), j, pos_list);
    }
}

// ---------------------------------------------------------------------

void pmpd3d_linksPosNormL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[i]),sqrt( \
                            sqr((x->link[i].mass1->posX + x->link[i].mass2->posX)/2) + \
                            sqr((x->link[i].mass1->posY + x->link[i].mass2->posY)/2) + \
                            sqr((x->link[i].mass1->posZ + x->link[i].mass2->posZ)/2) ));
		}
		outlet_anything(x->main_outlet, gensym("linksPosNormL"),x->nb_link , pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[j]),sqrt( \
                            sqr((x->link[i].mass1->posX + x->link[i].mass2->posX)/2) + \
                            sqr((x->link[i].mass1->posY + x->link[i].mass2->posY)/2) + \
                            sqr((x->link[i].mass1->posZ + x->link[i].mass2->posZ)/2) ));
				j++;
            }
        }
		outlet_anything(x->main_outlet, gensym("linksPosNormL"), j, pos_list);
    }		
}

void pmpd3d_linksLengthNormL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[i]),sqrt( \
                            sqr(x->link[i].mass2->posX - x->link[i].mass1->posX) + \
                            sqr(x->link[i].mass2->posY - x->link[i].mass1->posY) + \
                            sqr(x->link[i].mass2->posZ - x->link[i].mass1->posZ) ));
		}
		outlet_anything(x->main_outlet, gensym("linksLengthNormL"),x->nb_link , pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[j]),sqrt( \
                            sqr(x->link[i].mass2->posX - x->link[i].mass1->posX) + \
                            sqr(x->link[i].mass2->posY - x->link[i].mass1->posY) + \
                            sqr(x->link[i].mass2->posZ - x->link[i].mass1->posZ) ));
				j++;
            }
        }
		outlet_anything(x->main_outlet, gensym("linksLengthNormL"), j, pos_list);
    }
}

void pmpd3d_linksPosSpeedNormL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[i]),sqrt( \
                            sqr((x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2) + \
                            sqr((x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2) + \
                            sqr((x->link[i].mass1->speedZ + x->link[i].mass2->speedZ)/2) ));
		}
		outlet_anything(x->main_outlet, gensym("linksPosSpeedNormL"),x->nb_link , pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[j]),sqrt( \
                            sqr((x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2) + \
                            sqr((x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2) + \
                            sqr((x->link[i].mass1->speedZ + x->link[i].mass2->speedZ)/2) ));
				j++;
            }
        }
		outlet_anything(x->main_outlet, gensym("linksPosSpeedNormL"), j, pos_list);
    }
}

void pmpd3d_linksLengthSpeedNormL(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i,j;
    t_atom pos_list[x->nb_link];

    if (argc==0)
    {
		for (i=0; i < x->nb_link; i++)
		{
			SETFLOAT(&(pos_list[i]),sqrt( \
                            sqr(x->link[i].mass2->speedX - x->link[i].mass1->speedX) + \
                            sqr(x->link[i].mass2->speedY - x->link[i].mass1->speedY) + \
                            sqr(x->link[i].mass2->speedZ - x->link[i].mass1->speedZ) ));
		}
		outlet_anything(x->main_outlet, gensym("linksLengthSpeedNormL"),x->nb_link , pos_list);
	}
	else 
	if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
	{
		j=0;
		for (i=0; i < x->nb_link; i++)
        {
			if (atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
				SETFLOAT(&(pos_list[j]),sqrt( \
                            sqr((x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2) + \
                            sqr((x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2) + \
                            sqr((x->link[i].mass1->speedZ + x->link[i].mass2->speedZ)/2) ));
				j++;
            }
        }
		outlet_anything(x->main_outlet, gensym("linksLengthSpeedNormL"), j, pos_list);
    }
}

