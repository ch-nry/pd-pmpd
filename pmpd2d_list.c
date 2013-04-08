void pmpd2d_massPosL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[2*x->nb_mass];

    for (i=0; i < x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[2*i]  ),x->mass[i].posX);
        SETFLOAT(&(pos_list[2*i+1]),x->mass[i].posY);
    }
    outlet_anything(x->main_outlet, gensym("massPosL"),2*x->nb_mass , pos_list);
}

void pmpd2d_massForcesL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[2*x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[2*i]  ),x->mass[i].forceX);
        SETFLOAT(&(pos_list[2*i+1]),x->mass[i].forceY);
    }
    outlet_anything(x->main_outlet, gensym("massForcesL"),2*x->nb_mass , pos_list);
}

void pmpd2d_massSpeedsL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[2*x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[2*i]  ),x->mass[i].speedX);
        SETFLOAT(&(pos_list[2*i+1]),x->mass[i].speedY);
    }
    outlet_anything(x->main_outlet, gensym("massSpeedsL"),2*x->nb_mass , pos_list);
}

void pmpd2d_massPosXL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i < x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].posX);
    }
    outlet_anything(x->main_outlet, gensym("massPosXL"),x->nb_mass , pos_list);
}

void pmpd2d_massForcesXL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].forceX);
    }
    outlet_anything(x->main_outlet, gensym("massForcesXL"),x->nb_mass , pos_list);
}

void pmpd2d_massSpeedsXL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].speedX);
    }
    outlet_anything(x->main_outlet, gensym("massSpeedsXL"),x->nb_mass , pos_list);
}

void pmpd2d_massPosYL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i < x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].posY);
    }
    outlet_anything(x->main_outlet, gensym("massPosYL"),x->nb_mass , pos_list);
}

void pmpd2d_massForcesYL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].forceY);
    }
    outlet_anything(x->main_outlet, gensym("massForcesYL"),x->nb_mass , pos_list);
}

void pmpd2d_massSpeedsYL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].speedY);
    }
    outlet_anything(x->main_outlet, gensym("massSpeedsYL"),x->nb_mass , pos_list);
}

void pmpd2d_massPosNormL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i < x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),sqrt(sqr(x->mass[i].posX)+sqr(x->mass[i].posY)));
    }
    outlet_anything(x->main_outlet, gensym("massPosNormL"),x->nb_mass , pos_list);
}

void pmpd2d_massForcesNormL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),sqrt(sqr(x->mass[i].forceX)+sqr(x->mass[i].forceY)));
    }
    outlet_anything(x->main_outlet, gensym("massForcesNormL"),x->nb_mass , pos_list);
}

void pmpd2d_massSpeedsNormL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_mass];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),sqrt(sqr(x->mass[i].speedX)+sqr(x->mass[i].speedY)));
    }
    outlet_anything(x->main_outlet, gensym("massSpeedsNormL"),x->nb_mass , pos_list);
}

// --------------------------------------------

void pmpd2d_linkPosL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[2*x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[2*i]  ),(x->link[i].mass2->posX + x->link[i].mass1->posX)/2);
        SETFLOAT(&(pos_list[2*i+1]),(x->link[i].mass2->posY + x->link[i].mass1->posY)/2);
    }
    outlet_anything(x->main_outlet, gensym("linkPosL"),2*x->nb_link , pos_list);
}

void pmpd2d_linkLengthL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[2*x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[2*i]  ),x->link[i].mass2->posX - x->link[i].mass1->posX);
        SETFLOAT(&(pos_list[2*i+1]),x->link[i].mass2->posY - x->link[i].mass1->posY);
    }
    outlet_anything(x->main_outlet, gensym("linkLengthL"),2*x->nb_link , pos_list);
}

void pmpd2d_linkPosSpeedL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[2*x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[2*i]  ),(x->link[i].mass2->speedX + x->link[i].mass1->speedX)/2);
        SETFLOAT(&(pos_list[2*i+1]),(x->link[i].mass2->speedY + x->link[i].mass1->speedY)/2);
    }
    outlet_anything(x->main_outlet, gensym("linkPosSpeedL"),3*x->nb_link , pos_list);
}

void pmpd2d_linkLengthSpeedL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[2*x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[2*i]  ),x->link[i].mass2->speedX - x->link[i].mass1->speedX);
        SETFLOAT(&(pos_list[2*i+1]),x->link[i].mass2->speedY - x->link[i].mass1->speedY);
    }
    outlet_anything(x->main_outlet, gensym("linkLengthSpeedL"),2*x->nb_link , pos_list);
}

void pmpd2d_linkPosXL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),(x->link[i].mass1->posX + x->link[i].mass2->posX)/2);
    }
    outlet_anything(x->main_outlet, gensym("linkPosXL"),x->nb_link , pos_list);
}

void pmpd2d_linkLengthXL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),x->link[i].mass2->posX - x->link[i].mass1->posX);
    }
    outlet_anything(x->main_outlet, gensym("linkLengthXL"),x->nb_link , pos_list);
}

void pmpd2d_linkPosSpeedXL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),(x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2);
    }
    outlet_anything(x->main_outlet, gensym("linkPosSpeedXL"),x->nb_link , pos_list);
}

void pmpd2d_linkLengthSpeedXL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),x->link[i].mass2->speedX - x->link[i].mass1->speedX);
    }
    outlet_anything(x->main_outlet, gensym("linkLengthSpeedXL"),x->nb_link , pos_list);
}

void pmpd2d_linkPosYL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),(x->link[i].mass1->posY + x->link[i].mass2->posY)/2);
    }
    outlet_anything(x->main_outlet, gensym("linkPosYL"),x->nb_link , pos_list);
}

void pmpd2d_linkLengthYL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),x->link[i].mass2->posY - x->link[i].mass1->posY);
    }
    outlet_anything(x->main_outlet, gensym("linkLengthYL"),x->nb_link , pos_list);
}

void pmpd2d_linkPosSpeedYL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),(x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2);
    }
    outlet_anything(x->main_outlet, gensym("linkPosSpeedYL"),x->nb_link , pos_list);
}

void pmpd2d_linkLengthSpeedYL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),x->link[i].mass2->speedY - x->link[i].mass1->speedY);
    }
    outlet_anything(x->main_outlet, gensym("linkLengthSpeedYL"),x->nb_link , pos_list);
}

void pmpd2d_linkPosNormL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),sqrt( \
                            sqr((x->link[i].mass1->posX + x->link[i].mass2->posX)/2) + \
                            sqr((x->link[i].mass1->posY + x->link[i].mass2->posY)/2) ));
    }
    outlet_anything(x->main_outlet, gensym("linkPosNormL"),x->nb_link , pos_list);
}

void pmpd2d_linkLengthNormL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),sqrt( \
                            sqr(x->link[i].mass2->posX - x->link[i].mass1->posX) + \
                            sqr(x->link[i].mass2->posY - x->link[i].mass1->posY)  ));
    }
    outlet_anything(x->main_outlet, gensym("linkLengthNormL"),x->nb_link , pos_list);
}

void pmpd2d_linkPosSpeedNormL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),sqrt( \
                            sqr((x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2) + \
                            sqr((x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2)  ));
    }
    outlet_anything(x->main_outlet, gensym("linkPosSpeedNormL"),x->nb_link , pos_list);
}

void pmpd2d_linkLengthSpeedNormL(t_pmpd2d *x)
{
    int i;
    t_atom pos_list[x->nb_link];

    for (i=0; i < x->nb_link; i++)
    {
        SETFLOAT(&(pos_list[i]),sqrt( \
                            sqr(x->link[i].mass2->speedX - x->link[i].mass1->speedX) + \
                            sqr(x->link[i].mass2->speedY - x->link[i].mass1->speedY) ));
    }
    outlet_anything(x->main_outlet, gensym("linkLengthSpeedNormL"),x->nb_link , pos_list);
}


