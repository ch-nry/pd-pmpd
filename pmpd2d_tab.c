// --------------------------------------------------------------------------
// This file is part of the pmpd software.
//
//    pmpd software is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    pmpd firmware is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with pmpd software. If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------------- 
//
// pmpd = physical modeling for pure data
// ch@chnry.net


void pmpd2d_massesPosT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_mass;
            taille_max = min(taille_max, vecsize/2 );
            for (i=0; i < taille_max ; i++)
            {
                vec[2*i  ].w_float = x->mass[i].posX;
                vec[2*i+1].w_float = x->mass[i].posY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].posX;
                    i++;
                    vec[i].w_float = x->mass[j].posY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_massesSpeedsT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_mass;
            taille_max = min(taille_max, vecsize/2 );
            for (i=0; i < taille_max ; i++)
            {
                vec[2*i  ].w_float = x->mass[i].speedX;
                vec[2*i+1].w_float = x->mass[i].speedY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-1) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].speedX;
                    i++;
                    vec[i].w_float = x->mass[j].speedY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_massesForcesT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_mass;
            taille_max = min(taille_max, vecsize/2) ;
            for (i=0; i < taille_max ; i++)
            {
                vec[2*i  ].w_float = x->mass[i].forceX;
                vec[2*i+1].w_float = x->mass[i].forceY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-1) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].forceX;
                    i++;
                    vec[i].w_float = x->mass[j].forceY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_massesPosXT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->mass[i].posX;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].posX;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_massesSpeedsXT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->mass[i].speedX;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].speedX;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_massesForcesXT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->mass[i].forceX;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].forceX;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_massesPosYT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->mass[i].posY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].posY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_massesSpeedsYT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->mass[i].speedY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].speedY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_massesForcesYT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->mass[i].forceY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].forceY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

// ---------------------------------------------------------------------

void pmpd2d_massesPosNormT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = sqrt(pmpd2d_sqr(x->mass[i].posX)+pmpd2d_sqr(x->mass[i].posY));
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = sqrt(pmpd2d_sqr(x->mass[j].posX)+pmpd2d_sqr(x->mass[j].posY));
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_massesSpeedsNormT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{

    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = sqrt(pmpd2d_sqr(x->mass[i].speedX)+pmpd2d_sqr(x->mass[i].speedY));
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = sqrt(pmpd2d_sqr(x->mass[j].speedX)+pmpd2d_sqr(x->mass[j].speedY));
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_massesForcesNormT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = sqrt(pmpd2d_sqr(x->mass[i].forceX)+pmpd2d_sqr(x->mass[i].forceY));
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = sqrt(pmpd2d_sqr(x->mass[j].forceX)+pmpd2d_sqr(x->mass[j].forceY));
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}
// ---------------------------------------------------------------------

void pmpd2d_linksPosT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize/2 );
            for (i=0; i < taille_max ; i++)
            {
                vec[2*i  ].w_float = (x->link[i].mass2->posX + x->link[i].mass1->posX)/2;
                vec[2*i+1].w_float = (x->link[i].mass2->posY + x->link[i].mass1->posY)/2;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-1) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = (x->link[j].mass2->posX + x->link[j].mass1->posX)/2;
                    i++;
                    vec[i].w_float = (x->link[j].mass2->posY + x->link[j].mass1->posY)/2;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linksLengthT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize/2);
            for (i=0; i < taille_max ; i++)
            {
                vec[2*i  ].w_float = x->link[i].mass2->posX - x->link[i].mass1->posX;
                vec[2*i+1].w_float = x->link[i].mass2->posY - x->link[i].mass1->posY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-1) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass2->posX + x->link[j].mass1->posX;
                    i++;
                    vec[i].w_float = x->link[j].mass2->posY + x->link[j].mass1->posY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linksPosSpeedT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize/2);
            for (i=0; i < taille_max ; i++)
            {
                vec[2*i  ].w_float = (x->link[i].mass2->speedX + x->link[i].mass1->speedX)/2;
                vec[2*i+1].w_float = (x->link[i].mass2->speedY + x->link[i].mass1->speedY)/2;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-1) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = (x->link[j].mass2->speedX + x->link[j].mass1->speedX)/2;
                    i++;
                    vec[i].w_float = (x->link[j].mass2->speedY + x->link[j].mass1->speedY)/2;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linksLengthSpeedT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize/2);
            for (i=0; i < taille_max ; i++)
            {
                vec[2*i  ].w_float = x->link[i].mass2->speedX - x->link[i].mass1->speedX;
                vec[2*i+1].w_float = x->link[i].mass2->speedY - x->link[i].mass1->speedY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-1) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass2->speedX + x->link[j].mass1->speedX;
                    i++;
                    vec[i].w_float = x->link[j].mass2->speedY + x->link[j].mass1->speedY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linksPosXT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = (x->link[i].mass1->posX + x->link[i].mass2->posX)/2;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = (x->link[j].mass1->posX + x->link[j].mass2->posX)/2;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linksLengthXT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->link[i].mass2->posX - x->link[i].mass1->posX;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass2->posX - x->link[j].mass1->posX;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linksPosSpeedXT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = (x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = (x->link[j].mass1->speedX + x->link[j].mass2->speedX)/2;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linksLengthSpeedXT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->link[i].mass2->speedX - x->link[i].mass1->speedX;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass2->speedX - x->link[j].mass1->speedX;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linksPosYT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = (x->link[i].mass1->posY + x->link[i].mass2->posY)/2;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = (x->link[j].mass1->posY + x->link[j].mass2->posY)/2;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linksLengthYT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->link[i].mass2->posY - x->link[i].mass1->posY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass2->posY - x->link[j].mass1->posY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linksPosSpeedYT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = (x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = (x->link[j].mass1->speedY + x->link[j].mass2->speedY)/2;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linksLengthSpeedYT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->link[i].mass2->speedY - x->link[i].mass1->speedY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass2->speedY - x->link[j].mass1->speedY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

// ---------------------------------------------------------------------

void pmpd2d_linksPosNormT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = sqrt( \
                            pmpd2d_sqr((x->link[i].mass1->posX + x->link[i].mass2->posX)/2) + \
                            pmpd2d_sqr((x->link[i].mass1->posY + x->link[i].mass2->posY)/2) );
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[j].w_float = sqrt( \
                            pmpd2d_sqr((x->link[j].mass1->posX + x->link[j].mass2->posX)/2) + \
                            pmpd2d_sqr((x->link[j].mass1->posY + x->link[j].mass2->posY)/2) );
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linksLengthNormT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = sqrt( \
                            pmpd2d_sqr(x->link[i].mass2->posX - x->link[i].mass1->posX) + \
                            pmpd2d_sqr(x->link[i].mass2->posY - x->link[i].mass1->posY) );
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = sqrt( \
                            pmpd2d_sqr(x->link[j].mass2->posX - x->link[j].mass1->posX) + \
                            pmpd2d_sqr(x->link[j].mass2->posY - x->link[j].mass1->posY) );
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linksPosSpeedNormT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = sqrt( \
                            pmpd2d_sqr((x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2) + \
                            pmpd2d_sqr((x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2) );
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = sqrt( \
                            pmpd2d_sqr((x->link[j].mass1->speedX + x->link[j].mass2->speedX)/2) + \
                            pmpd2d_sqr((x->link[j].mass1->speedY + x->link[j].mass2->speedY)/2) );
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linksLengthSpeedNormT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = sqrt( \
                            pmpd2d_sqr(x->link[i].mass2->speedX - x->link[i].mass1->speedX) + \
                            pmpd2d_sqr(x->link[i].mass2->speedY - x->link[i].mass1->speedY) );
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = sqrt( \
                            pmpd2d_sqr(x->link[j].mass2->speedX - x->link[j].mass1->speedX) + \
                            pmpd2d_sqr(x->link[j].mass2->speedY - x->link[j].mass1->speedY) );
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

// ---------------------------------------------------------------------


void pmpd2d_linkEndT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize/4 );
            for (i=0; i < taille_max ; i++)
            {
                vec[4*i  ].w_float = x->link[i].mass1->posX;
                vec[4*i+1].w_float = x->link[i].mass1->posY;
                vec[4*i+2].w_float = x->link[i].mass2->posX;
                vec[4*i+3].w_float = x->link[i].mass2->posY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-3) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass1->posX;
                    i++;
                    vec[i].w_float = x->link[j].mass1->posY;
                    i++;
                    vec[i].w_float = x->link[j].mass2->posX;
                    i++;
                    vec[i].w_float = x->link[j].mass2->posY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linkEndXT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize/2);
            for (i=0; i < taille_max ; i++)
            {
                vec[2*i  ].w_float = x->link[i].mass1->posX;
                vec[2*i+1].w_float = x->link[i].mass2->posX;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-1) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass1->posX;
                    i++;
                    vec[i].w_float = x->link[j].mass2->posX;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linkEndYT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize/2);
            for (i=0; i < taille_max ; i++)
            {
                vec[2*i  ].w_float = x->link[i].mass1->posY;
                vec[2*i+1].w_float = x->link[i].mass2->posY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-1) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass1->posY;
                    i++;
                    vec[i].w_float = x->link[j].mass2->posY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linkEnd1T(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize/2);
            for (i=0; i < taille_max ; i++)
            {
                vec[2*i  ].w_float = x->link[i].mass1->posX;
                vec[2*i+1].w_float = x->link[i].mass1->posY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-1) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass1->posX;
                    i++;
                    vec[i].w_float = x->link[j].mass1->posY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linkEnd1XT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->link[i].mass1->posX;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass1->posX;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linkEnd1YT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->link[i].mass1->posY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass1->posY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linkEnd2T(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize/2);
            for (i=0; i < taille_max ; i++)
            {
                vec[2*i  ].w_float = x->link[i].mass2->posX;
                vec[2*i+1].w_float = x->link[i].mass2->posY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-1) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass2->posX;
                    i++;
                    vec[i].w_float = x->link[j].mass2->posY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linkEnd2XT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->link[i].mass2->posX;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass2->posX;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd2d_linkEnd2YT(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
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
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->link[i].mass2->posY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass2->posY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

// ---------------------------------------------------------------------

void pmpd2d_linkEndNormeForce(t_pmpd2d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    t_float tmp;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize/2 );
            for (i=0; i < taille_max ; i++)
            {
                tmp = sqrt(pmpd2d_sqr(x->link[i].forceX) + pmpd2d_sqr(x->link[i].forceY));
                vec[2*i  ].w_float = -tmp;
                vec[2*i+1].w_float = tmp;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-1) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    tmp = sqrt(pmpd2d_sqr(x->link[j].forceX) + pmpd2d_sqr(x->link[j].forceY));
                    vec[i].w_float = -tmp;
                    i++;
                    vec[i].w_float = tmp;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

