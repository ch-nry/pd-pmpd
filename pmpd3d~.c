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

#include <stdio.h>
#include <math.h>

#include "pmpd_export.h"
#include "pmpd_version.h"

#include "m_pd.h"

#define max(a,b) ( ((a) > (b)) ? (a) : (b) )
#define min(a,b) ( ((a) < (b)) ? (a) : (b) )
#define clamp(a,b,c) (min(max((a), (b)), (c)))

#define NB_MAX_LINK_DEFAULT 10000
#define NB_MAX_MASS_DEFAULT 10000
#define NB_MAX_IN_DEFAULT    1000
#define NB_MAX_OUT_DEFAULT   1000

static t_class *pmpd3d_tilde_class;

struct _mass {
    t_float invM;
    t_float speedX;
    t_float speedY;
    t_float speedZ;
    t_float posX;
    t_float posY;
    t_float posZ;
    t_float forceX;
    t_float forceY;
    t_float forceZ;
    t_float D; // damping de vitesse
    t_float Doffset;
    t_int Id;
};

struct _link {
    struct _mass *mass1;
    struct _mass *mass2;
    t_float K1, D1, L0, L;
};

struct _NLlink {
    struct _mass *mass1;
    struct _mass *mass2;
    t_float K1, D1, L0, L, Lmin, Lmax, Pow;
};

struct _inPos {
    // in en position
    t_int nbr_inlet;
    struct _mass *mass;
    t_float influence;
};

struct _inForce {
    // in en force
    t_int nbr_inlet;
    struct _mass *mass;
    t_float influence;
};

struct _outPos {
    // out en position
    t_int nbr_outlet;
    struct _mass *mass;
    t_float influence;
};

struct _outSpeed {
    // out en vitesse
    t_int nbr_outlet;
    struct _mass *mass;
    t_float influence;
};

typedef struct _pmpd3d_tilde {
    t_object  x_obj;

    struct _link *link;
    struct _NLlink *NLlink;
    struct _mass *mass;

    struct _inPos *inPosX;
    struct _inPos *inPosY;
    struct _inPos *inPosZ;
    struct _inForce *inForceX;
    struct _inForce *inForceY;
    struct _inForce *inForceZ;
    struct _outPos *outPosX;
    struct _outPos *outPosY;
    struct _outPos *outPosZ;
    struct _outSpeed *outSpeedX;
    struct _outSpeed *outSpeedY;
    struct _outSpeed *outSpeedZ;
    struct _outSpeed *outSpeed;

    t_float *outlet;
    t_sample **inlet_vector;
    t_sample **outlet_vector;
    t_int nb_max_link, nb_max_mass;
    t_int nb_link, nb_NLlink, nb_mass;
    t_int nb_inlet, nb_outlet, nb_max_in, nb_max_out;
    t_int nb_inPosX, nb_inPosY, nb_inPosZ, nb_inForceX, nb_inForceY, nb_inForceZ;
    t_int nb_outPosX, nb_outPosY, nb_outPosZ, nb_outSpeedX, nb_outSpeedY, nb_outSpeedZ, nb_outSpeed;
    t_sample f; // used for signal inlet
    t_int loop, nb_loop; // to be able not to compute everything a each iteration
} t_pmpd3d_tilde;

t_int *pmpd3d_tilde_perform(t_int *w)
///////////////////////////////////////////////////////////////////////////////////
{
    t_pmpd3d_tilde *x = (t_pmpd3d_tilde *)(w[1]);
    int n = (int)(w[2]);

    t_float F, FX, FY, FZ, L, LX, LY, LZ, deltaL;
    t_int i, si;

    t_sample **in = x->inlet_vector;
    t_sample **out = x->outlet_vector;

    for (i=0; i<x->nb_inlet; i++) in[i]= x->inlet_vector[i];
    for (i=0; i<x->nb_outlet; i++) out[i]= x->outlet_vector[i];

    for (si=0; si<n; si++)
    {
        x->loop = 0;
        while (x->loop++ < x->nb_loop)
        {
            for (i=0; i<x->nb_inPosX; i++)
                // get inlet value and make it a position to the specified mass
                x->inPosX[i].mass->posX = x->inPosX[i].influence * in[x->inPosX[i].nbr_inlet][si];
            for (i=0; i<x->nb_inPosY; i++)
                x->inPosY[i].mass->posY = x->inPosY[i].influence * in[x->inPosY[i].nbr_inlet][si];
            for (i=0; i<x->nb_inPosZ; i++)
                x->inPosZ[i].mass->posZ = x->inPosZ[i].influence * in[x->inPosZ[i].nbr_inlet][si];
            for (i=0; i<x->nb_inForceX; i++)
                // get inlet value and make it a force to the specified mass
                x->inForceX[i].mass->forceX += x->inForceX[i].influence * in[x->inForceX[i].nbr_inlet][si];
            for (i=0; i<x->nb_inForceY; i++)
                x->inForceY[i].mass->forceY += x->inForceY[i].influence * in[x->inForceY[i].nbr_inlet][si];
            for (i=0; i<x->nb_inForceZ; i++)
                x->inForceZ[i].mass->forceZ += x->inForceZ[i].influence * in[x->inForceZ[i].nbr_inlet][si];
            for (i=0; i<x->nb_link; i++)
            // compute forces generated by links (spring / dashpot)
            {
                LX = x->link[i].mass2->posX - x->link[i].mass1->posX;
                LY = x->link[i].mass2->posY - x->link[i].mass1->posY;
                LZ = x->link[i].mass2->posZ - x->link[i].mass1->posZ;
                L = sqrt(LY*LY + LX*LX + LZ*LZ);
                F  = x->link[i].K1 * (L - x->link[i].L0);
                // spring

                F  += x->link[i].D1 * (L - x->link[i].L); // on derive la longeur L calculé précedement
                x->link[i].L = L; // on la sauve pour la prochaine itération
                // dashpot

                if(L !=0 ) { // si L = 0 : on ne sais pas dans quel direction apliquer la force : c'est un point d'equilibre instable
                    FX = F * LX/L;
                    FY = F * LY/L;
                    FZ = F * LZ/L;
                } else {
                    FX = 0;
                    FY = 0;
                    FZ = 0;
                }

                x->link[i].mass1->forceX += FX;
                x->link[i].mass2->forceX -= FX;
                x->link[i].mass1->forceY += FY;
                x->link[i].mass2->forceY -= FY;
                x->link[i].mass1->forceZ += FZ;
                x->link[i].mass2->forceZ -= FZ;
            }

            for (i=0; i<x->nb_NLlink; i++)
            // compute forces generated by NLlinks (spring / dashpot)
            {
                LX = x->NLlink[i].mass2->posX - x->NLlink[i].mass1->posX;
                LY = x->NLlink[i].mass2->posY - x->NLlink[i].mass1->posY;
                LZ = x->NLlink[i].mass2->posZ - x->NLlink[i].mass1->posZ;
                L = sqrt(LY*LY + LX*LX + LZ*LZ);
                if ((L < x->NLlink[i].Lmax) && (L > x->NLlink[i].Lmin)) {
                    deltaL = L - x->NLlink[i].L0;
                    F  = x->NLlink[i].K1 * pow(fabs(deltaL), x->NLlink[i].Pow);
                    if (deltaL < 0) F *= -1;
                    // spring

                    F += x->NLlink[i].D1 * (L - x->NLlink[i].L); // on derive la longeur L calculé précedement
                    x->NLlink[i].L = L; // on la sauve pour la prochaine itération
                    // dashpot

                    if(L !=0 ) { // si L = 0 : on ne sais pas dans quel direction apliquer la force : c'est un point d'equilibre instable
                        FX = F * LX/L;
                        FY = F * LY/L;
                        FZ = F * LZ/L;
                    } else {
                        FX = 0;
                        FY = 0;
                        FZ = 0;
                    }
                    x->NLlink[i].mass1->forceX += FX;
                    x->NLlink[i].mass2->forceX -= FX;
                    x->NLlink[i].mass1->forceY += FY;
                    x->NLlink[i].mass2->forceY -= FY;
                    x->NLlink[i].mass1->forceZ += FZ;
                    x->NLlink[i].mass2->forceZ -= FZ;
                }
                else x->NLlink[i].L = L; // on la sauve pour la prochaine itération (uniquement pour le "bang")
            }
            for (i=0; i<x->nb_mass; i++)
            {
            // compute new masses position
            // a mass does not move if M=0 (i.e : invM = 0)
                if ( (x->mass[i].D != 0) && ( (x->mass[i].speedX != 0) || (x->mass[i].speedY != 0) || (x->mass[i].speedZ != 0) ) ) { // amortissement en fct de la vitesse
                    L = sqrt(x->mass[i].speedX * x->mass[i].speedX + x->mass[i].speedY * x->mass[i].speedY + x->mass[i].speedZ * x->mass[i].speedZ);
                    F = -(L - x->mass[i].Doffset) * x->mass[i].D;
                     x->mass[i].forceX += F * x->mass[i].speedX / L;
                     x->mass[i].forceY += F * x->mass[i].speedY / L;
                     x->mass[i].forceZ += F * x->mass[i].speedZ / L;
                }
                x->mass[i].speedX += x->mass[i].forceX * x->mass[i].invM;
                x->mass[i].speedY += x->mass[i].forceY * x->mass[i].invM;
                x->mass[i].speedZ += x->mass[i].forceZ * x->mass[i].invM;
                x->mass[i].forceX = 0;
                x->mass[i].forceY = 0;
                x->mass[i].forceZ = 0;
                x->mass[i].posX += x->mass[i].speedX;
                x->mass[i].posY += x->mass[i].speedY;
                x->mass[i].posZ += x->mass[i].speedZ;
            }
        }

        // compute output vector value
        for (i=0; i<x->nb_outlet; i++) x->outlet[i] = 0;
        for (i=0; i<x->nb_outPosX; i++) x->outlet[x->outPosX[i].nbr_outlet] += x->outPosX[i].mass->posX * x->outPosX[i].influence;
        for (i=0; i<x->nb_outPosY; i++) x->outlet[x->outPosY[i].nbr_outlet] += x->outPosY[i].mass->posY * x->outPosY[i].influence;
        for (i=0; i<x->nb_outPosZ; i++) x->outlet[x->outPosZ[i].nbr_outlet] += x->outPosZ[i].mass->posZ * x->outPosZ[i].influence;
        for (i=0; i<x->nb_outSpeedX; i++) x->outlet[x->outSpeedX[i].nbr_outlet] += x->outSpeedX[i].mass->speedX * x->outSpeedX[i].influence;
        for (i=0; i<x->nb_outSpeedY; i++) x->outlet[x->outSpeedY[i].nbr_outlet] += x->outSpeedY[i].mass->speedY * x->outSpeedY[i].influence;
        for (i=0; i<x->nb_outSpeedZ; i++) x->outlet[x->outSpeedZ[i].nbr_outlet] += x->outSpeedZ[i].mass->speedZ * x->outSpeedZ[i].influence;
        for (i=0; i<x->nb_outSpeed; i++) x->outlet[x->outSpeed[i].nbr_outlet] += sqrt(x->outSpeed[i].mass->speedX*x->outSpeed[i].mass->speedX + x->outSpeed[i].mass->speedY * x->outSpeed[i].mass->speedY + x->outSpeed[i].mass->speedZ * x->outSpeed[i].mass->speedZ) * x->outSpeed[i].influence;

        // send vector value to the vector pointer
        for (i=0; i<x->nb_outlet; i++)
            out[i][si] = x->outlet[i];
    }
    return(w+3);
}

void pmpd3d_tilde_dsp(t_pmpd3d_tilde *x, t_signal **sp)
{
    int i;
    for (i=0; i<x->nb_inlet; i++)
        x->inlet_vector[i] = sp[i]->s_vec;

    for (i=0; i<x->nb_outlet; i++)
        x->outlet_vector[i] = sp[i+x->nb_inlet]->s_vec;

    dsp_add(pmpd3d_tilde_perform, 2, x, sp[0]->s_n); // S_n : la taille du vecteur
}

void pmpd3d_tilde_bang(t_pmpd3d_tilde *x)
{
    t_int i;
    for (i=0; i<x->nb_mass; i++) logpost(x, 2, "mass:%ld, M:%f, posX:%f, posY:%f, posZ:%f, D2:%f, D2offset:%f",i, x->mass[i].invM<=0.?0:1/x->mass[i].invM, x->mass[i].posX,x->mass[i].posY,x->mass[i].posZ,x->mass[i].D,x->mass[i].Doffset);
    for (i=0; i<x->nb_link; i++) logpost(x, 2, "link:%ld, mass1:%ld, mass2:%ld, K:%f, D:%f, L0:%f, L:%f", i, x->link[i].mass1->Id, x->link[i].mass2->Id, x->link[i].K1, x->link[i].D1, x->link[i].L0, x->link[i].L);
    for (i=0; i<x->nb_NLlink; i++) logpost(x, 2, "NLlink:%ld, mass1:%ld, mass2:%ld, K:%f, D:%f, L0:%f, L:%f, Lmin:%f, Lmax:%f, Pow:%f, blabla", i, x->NLlink[i].mass1->Id, x->NLlink[i].mass2->Id, x->NLlink[i].K1, x->NLlink[i].D1, x->NLlink[i].L0, x->NLlink[i].L, x->NLlink[i].Lmin, x->NLlink[i].Lmax, x->NLlink[i].Pow);
    for (i=0; i<x->nb_inPosX; i++) logpost(x, 2, "inPosX:%ld, Inlet:%ld, Mass:%ld, Amplitude:%f", i, x->inPosX[i].nbr_inlet, x->inPosX[i].mass->Id, x->inPosX[i].influence);
    for (i=0; i<x->nb_inPosY; i++) logpost(x, 2, "inPosY:%ld, Inlet:%ld, Mass:%ld, Amplitude:%f", i, x->inPosY[i].nbr_inlet, x->inPosY[i].mass->Id, x->inPosY[i].influence);
    for (i=0; i<x->nb_inPosZ; i++) logpost(x, 2, "inPosZ:%ld, Inlet:%ld, Mass:%ld, Amplitude:%f", i, x->inPosZ[i].nbr_inlet, x->inPosZ[i].mass->Id, x->inPosZ[i].influence);
    for (i=0; i<x->nb_inForceX; i++) logpost(x, 2, "inForceX:%ld, Inlet:%ld, Mass:%ld, Amplitude:%f", i, x->inForceX[i].nbr_inlet, x->inForceX[i].mass->Id, x->inForceX[i].influence);
    for (i=0; i<x->nb_inForceY; i++) logpost(x, 2, "inForceY:%ld, Inlet:%ld, Mass:%ld, Amplitude:%f", i, x->inForceY[i].nbr_inlet, x->inForceY[i].mass->Id, x->inForceY[i].influence);
    for (i=0; i<x->nb_inForceZ; i++) logpost(x, 2, "inForceZ:%ld, Inlet:%ld, Mass:%ld, Amplitude:%f", i, x->inForceZ[i].nbr_inlet, x->inForceZ[i].mass->Id, x->inForceZ[i].influence);
    for (i=0; i<x->nb_outPosX; i++) logpost(x, 2, "outPosX:%ld, Outlet:%ld, Mass:%ld, Amplitude:%f", i, x->outPosX[i].nbr_outlet, x->outPosX[i].mass->Id, x->outPosX[i].influence);
    for (i=0; i<x->nb_outPosY; i++) logpost(x, 2, "outPosY:%ld, Outlet:%ld, Mass:%ld, Amplitude:%f", i, x->outPosY[i].nbr_outlet, x->outPosY[i].mass->Id, x->outPosY[i].influence);
    for (i=0; i<x->nb_outPosZ; i++) logpost(x, 2, "outPosZ:%ld, Outlet:%ld, Mass:%ld, Amplitude:%f", i, x->outPosZ[i].nbr_outlet, x->outPosZ[i].mass->Id, x->outPosZ[i].influence);
    for (i=0; i<x->nb_outSpeed; i++) logpost(x, 2, "outSpeed:%ld, Outlet:%ld, Mass:%ld, Amplitude:%f", i, x->outSpeed[i].nbr_outlet, x->outSpeed[i].mass->Id, x->outSpeed[i].influence);
    for (i=0; i<x->nb_outSpeedX; i++) logpost(x, 2, "outSpeedX:%ld, Outlet:%ld, Mass:%ld, Amplitude:%f", i, x->outSpeedX[i].nbr_outlet, x->outSpeedX[i].mass->Id, x->outSpeedX[i].influence);
    for (i=0; i<x->nb_outSpeedY; i++) logpost(x, 2, "outSpeedY:%ld, Outlet:%ld, Mass:%ld, Amplitude:%f", i, x->outSpeedY[i].nbr_outlet, x->outSpeedY[i].mass->Id, x->outSpeedY[i].influence);
    for (i=0; i<x->nb_outSpeedZ; i++) logpost(x, 2, "outSpeedZ:%ld, Outlet:%ld, Mass:%ld, Amplitude:%f", i, x->outSpeedZ[i].nbr_outlet, x->outSpeedZ[i].mass->Id, x->outSpeedZ[i].influence);
}

int validate_index(t_pmpd3d_tilde *x, int idx, t_int count, const char* type)
{
    if (idx < 0 || idx+1 > count) {
        pd_error(x, "pmpd3d~: no %s at index %i", type, idx);
        return 0;
    }
    return 1;
}

void pmpd3d_tilde_forceX(t_pmpd3d_tilde *x, t_float idx_mass, t_float force)
{
// add a force to a specific mass
    if (!validate_index(x, (int)idx_mass, x->nb_mass, "mass")) return;
    x->mass[(int)idx_mass].forceX += force;
}

void pmpd3d_tilde_forceY(t_pmpd3d_tilde *x, t_float idx_mass, t_float force)
{
// add a force to a specific mass
    if (!validate_index(x, (int)idx_mass, x->nb_mass, "mass")) return;
    x->mass[(int)idx_mass].forceY += force;
}

void pmpd3d_tilde_forceZ(t_pmpd3d_tilde *x, t_float idx_mass, t_float force)
{
// add a force to a specific mass
    if (!validate_index(x, (int)idx_mass, x->nb_mass, "mass")) return;
    x->mass[(int)idx_mass].forceZ += force;
}

void pmpd3d_tilde_posX(t_pmpd3d_tilde *x, t_float idx_mass, t_float posX)
{
// move a mass to a certain position
    if (!validate_index(x, (int)idx_mass, x->nb_mass, "mass")) return;
    x->mass[(int)idx_mass].posX = posX;
}

void pmpd3d_tilde_posY(t_pmpd3d_tilde *x, t_float idx_mass, t_float posY)
{
// move a mass to a certain position
    if (!validate_index(x, (int)idx_mass, x->nb_mass, "mass")) return;
    x->mass[(int)idx_mass].posY = posY;
}

void pmpd3d_tilde_posZ(t_pmpd3d_tilde *x, t_float idx_mass, t_float posZ)
{
// move a mass to a certain position
    if (!validate_index(x, (int)idx_mass, x->nb_mass, "mass")) return;
    x->mass[(int)idx_mass].posZ = posZ;
}

void pmpd3d_tilde_setD2(t_pmpd3d_tilde *x, t_float idx_mass, t_float D)
{
    if (!validate_index(x, (int)idx_mass, x->nb_mass, "mass")) return;
    x->mass[(int)idx_mass].D = D;
}

void pmpd3d_tilde_setD2offset(t_pmpd3d_tilde *x, t_float idx_mass, t_float D)
{
    if (!validate_index(x, (int)idx_mass, x->nb_mass, "mass")) return;
    x->mass[(int)idx_mass].Doffset = D;
}

void pmpd3d_tilde_setM(t_pmpd3d_tilde *x, t_float idx_mass, t_float M)
{
    if (!validate_index(x, (int)idx_mass, x->nb_mass, "mass")) return;
    x->mass[(int)idx_mass].invM = M>0 ? 1/M : 0;
}

void pmpd3d_tilde_setK(t_pmpd3d_tilde *x, t_float idx_link, t_float K)
{
    if (!validate_index(x, (int)idx_link, x->nb_link, "link")) return;
    x->link[(int)idx_link].K1 = K;
}

void pmpd3d_tilde_setD(t_pmpd3d_tilde *x, t_float idx_link, t_float D)
{
    if (!validate_index(x, (int)idx_link, x->nb_link, "link")) return;
    x->link[(int)idx_link].D1 = D;
}

void pmpd3d_tilde_setL(t_pmpd3d_tilde *x, t_float idx_link, t_float L)
{
    if (!validate_index(x, (int)idx_link, x->nb_link, "link")) return;
    x->link[(int)idx_link].L0 = L;
}

void pmpd3d_tilde_setLCurrent(t_pmpd3d_tilde *x, t_symbol *s, int argc, t_atom *argv)
{
    int idx_link;
    t_float percent;
    if (argc < 1)
    {
        pd_error(x, "pmpd3d~: 'setLCurrent' requires link index argument");
        return;
    } 
    if (argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "pmpd3d~: invalid link index for 'setLCurrent'");
        return;
    }
    idx_link = (int)atom_getfloatarg(0,argc,argv);
    if (!validate_index(x, idx_link, x->nb_link, "link")) return;
    if (argc > 1 && argv[1].a_type == A_FLOAT)
        percent = atom_getfloatarg(1,argc,argv);
    else
        percent = 1;
    x->link[idx_link].L0 += percent * (x->link[idx_link].L - x->link[idx_link].L0);
}

void pmpd3d_tilde_setNLD(t_pmpd3d_tilde *x, t_float idx_NLlink, t_float D)
{
    if (!validate_index(x, (int)idx_NLlink, x->nb_NLlink, "NLlink")) return;
    x->NLlink[(int)idx_NLlink].D1 = D;
}

void pmpd3d_tilde_setNLK(t_pmpd3d_tilde *x, t_symbol *s, int argc, t_atom *argv)
{
    int idx_NLlink;
    if (argc < 2)
    {
        pd_error(x, "pmpd3d~: 'setNLK' requires link index and rigidity arguments");
        return;
    }
    if (argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "pmpd3d~: invalid link index for 'setNLK'");
        return;
    }
    idx_NLlink = (int)atom_getfloatarg(0,argc,argv);
    if (!validate_index(x, idx_NLlink, x->nb_NLlink, "NLlink")) return;
    if (argv[1].a_type == A_FLOAT)
        x->NLlink[idx_NLlink].K1 = atom_getfloatarg(1,argc,argv);
    if (argc > 2 && argv[2].a_type == A_FLOAT)
        x->NLlink[idx_NLlink].Pow = atom_getfloatarg(2,argc,argv);
}

void pmpd3d_tilde_setNLKPow(t_pmpd3d_tilde *x, t_float idx_NLlink, t_float Pow)
{
    if (!validate_index(x, (int)idx_NLlink, x->nb_NLlink, "NLlink")) return;
    x->NLlink[(int)idx_NLlink].Pow = Pow;
}

void pmpd3d_tilde_setNLL(t_pmpd3d_tilde *x, t_float idx_NLlink, t_float L)
{
    if (!validate_index(x, (int)idx_NLlink, x->nb_NLlink, "NLlink")) return;
    x->NLlink[(int)idx_NLlink].L0 = L;
}

void pmpd3d_tilde_setNLLMin(t_pmpd3d_tilde *x, t_float idx_NLlink, t_float M)
{
    if (!validate_index(x, (int)idx_NLlink, x->nb_NLlink, "NLlink")) return;
    x->NLlink[(int)idx_NLlink].Lmin = M;
}

void pmpd3d_tilde_setNLLMax(t_pmpd3d_tilde *x, t_float idx_NLlink, t_float M)
{
    if (!validate_index(x, (int)idx_NLlink, x->nb_NLlink, "NLlink")) return;
    x->NLlink[(int)idx_NLlink].Lmax = M;
}

void pmpd3d_tilde_setNLLCurrent(t_pmpd3d_tilde *x, t_symbol *s, int argc, t_atom *argv)
{
    int idx_NLlink;
    t_float percent;
    if (argc < 1)
    {
        pd_error(x, "pmpd3d~: 'setNLLCurrent' requires link index argument");
        return;
    }
    if (argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "pmpd3d~: invalid link index for 'setNLLCurrent'");
        return;
    }
    idx_NLlink = (int)atom_getfloatarg(0,argc,argv);
    if (!validate_index(x, idx_NLlink, x->nb_NLlink, "NLlink")) return;
    if (argc > 1 && argv[1].a_type == A_FLOAT)
        percent = atom_getfloatarg(1,argc,argv);
    else
        percent = 1;
    x->NLlink[idx_NLlink].L0 += percent * (x->NLlink[idx_NLlink].L - x->NLlink[idx_NLlink].L0);
}

int validate_count(t_pmpd3d_tilde *x, t_int count, t_int count_max, const char* type)
{
    if (count == count_max) {
        pd_error(x, "pmpd3d~: too many %ss (increase limit with creation argument)", type);
        return 0;
    }
    return 1;
}

void pmpd3d_tilde_mass(t_pmpd3d_tilde *x, t_float M, t_float posX, t_float posY, t_float posZ, t_float D)
{
// add a mass
//invM speedX posX force
    if (!validate_count(x, x->nb_mass, x->nb_max_mass, "masses")) return;
    x->mass[x->nb_mass].invM = M>0 ? 1/M : 0;
    x->mass[x->nb_mass].speedX = 0;
    x->mass[x->nb_mass].speedY = 0;
    x->mass[x->nb_mass].speedZ = 0;
    x->mass[x->nb_mass].posX = posX;
    x->mass[x->nb_mass].posY = posY;
    x->mass[x->nb_mass].posZ = posZ;
    x->mass[x->nb_mass].forceX = 0;
    x->mass[x->nb_mass].forceY = 0;
    x->mass[x->nb_mass].forceZ = 0;
    x->mass[x->nb_mass].D = D;
    x->mass[x->nb_mass].Doffset = 0;
    x->mass[x->nb_mass].Id = x->nb_mass;
    x->nb_mass++;
}

void pmpd3d_tilde_link(t_pmpd3d_tilde *x, t_float mass_1, t_float mass_2, t_float K1, t_float D1, t_float L0)
{
// add a link
// *mass1, *mass2, K1, D1;
    if (!validate_count(x, x->nb_link, x->nb_max_link, "links")) return;
    t_float LX, LY, LZ;
    x->link[x->nb_link].mass1 = &x->mass[clamp((int)mass_1, 0, x->nb_mass-1)];
    x->link[x->nb_link].mass2 = &x->mass[clamp((int)mass_2, 0, x->nb_mass-1)];
    x->link[x->nb_link].K1 = K1;
    x->link[x->nb_link].D1 = D1;
    x->link[x->nb_link].L0 = L0;

    // initialize L with actual distance between masses
    LX = x->link[x->nb_link].mass2->posX - x->link[x->nb_link].mass1->posX;
    LY = x->link[x->nb_link].mass2->posY - x->link[x->nb_link].mass1->posY;
    LZ = x->link[x->nb_link].mass2->posZ - x->link[x->nb_link].mass1->posZ;
    x->link[x->nb_link].L = sqrt(LY*LY + LX*LX + LZ*LZ);
    x->nb_link++;
}

void pmpd3d_tilde_NLlink(t_pmpd3d_tilde *x, t_symbol *s, int argc, t_atom *argv)
{
// t_float mass_1, t_float mass_2, t_float K1, t_float D1, t_float Pow, t_float L0, t_float Lmin, t_float Lmax
// add a NLlink
    if (!validate_count(x, x->nb_link, x->nb_max_link, "links")) return;
    t_float LX, LY, LZ;
    x->NLlink[x->nb_NLlink].mass1 = &x->mass[clamp((int)atom_getfloatarg(0, argc, argv), 0, x->nb_mass-1)];
    x->NLlink[x->nb_NLlink].mass2 = &x->mass[clamp((int)atom_getfloatarg(1, argc, argv), 0, x->nb_mass-1)];
    x->NLlink[x->nb_NLlink].K1 = (argc >= 3) ? atom_getfloatarg(2, argc, argv) : 0;
    x->NLlink[x->nb_NLlink].D1 = (argc >= 4) ? atom_getfloatarg(3, argc, argv) : 0;
    x->NLlink[x->nb_NLlink].Pow = (argc >= 5) ? atom_getfloatarg(4, argc, argv) : 1;
    x->NLlink[x->nb_NLlink].L0 = (argc >= 6) ? atom_getfloatarg(5, argc, argv) : 0;
    x->NLlink[x->nb_NLlink].Lmin = (argc >= 7) ? atom_getfloatarg(6, argc, argv) : -1000000;
    x->NLlink[x->nb_NLlink].Lmax = (argc >= 8) ? atom_getfloatarg(7, argc, argv) : 1000000;

    // initialize L with actual distance between masses
    LX = x->NLlink[x->nb_NLlink].mass2->posX - x->NLlink[x->nb_NLlink].mass1->posX;
    LY = x->NLlink[x->nb_NLlink].mass2->posY - x->NLlink[x->nb_NLlink].mass1->posY;
    LZ = x->NLlink[x->nb_NLlink].mass2->posZ - x->NLlink[x->nb_NLlink].mass1->posZ;
    x->NLlink[x->nb_NLlink].L = sqrt(LY*LY + LX*LX + LZ*LZ);
    x->nb_NLlink++;
}

void pmpd3d_tilde_inPosX(t_pmpd3d_tilde *x, t_float idx_inlet, t_float idx_mass, t_float influence)
{
//add an input point
// nbr_inlet, *mass, influence;
    if (!validate_count(x, x->nb_inPosX, x->nb_max_in, "inlet assignment") ||
        !validate_index(x, idx_inlet, x->nb_inlet, "inlet") ||
        !validate_index(x, idx_mass, x->nb_mass, "mass")) return;
    x->inPosX[x->nb_inPosX].nbr_inlet = (int)idx_inlet;
    x->inPosX[x->nb_inPosX].mass = &x->mass[(int)idx_mass];
    x->inPosX[x->nb_inPosX].influence = influence;
    x->nb_inPosX++;
}

void pmpd3d_tilde_inPosY(t_pmpd3d_tilde *x, t_float idx_inlet, t_float idx_mass, t_float influence)
{
//add an input point
// nbr_inlet, *mass, influence;
    if (!validate_count(x, x->nb_inPosY, x->nb_max_in, "inlet assignment") ||
        !validate_index(x, idx_inlet, x->nb_inlet, "inlet") ||
        !validate_index(x, idx_mass, x->nb_mass, "mass")) return;
    x->inPosY[x->nb_inPosY].nbr_inlet = (int)idx_inlet;
    x->inPosY[x->nb_inPosY].mass = &x->mass[(int)idx_mass];
    x->inPosY[x->nb_inPosY].influence = influence;
    x->nb_inPosY++;
}

void pmpd3d_tilde_inPosZ(t_pmpd3d_tilde *x, t_float idx_inlet, t_float idx_mass, t_float influence)
{
//add an input point
// nbr_inlet, *mass, influence;
    if (!validate_count(x, x->nb_inPosZ, x->nb_max_in, "inlet assignment") ||
        !validate_index(x, idx_inlet, x->nb_inlet, "inlet") ||
        !validate_index(x, idx_mass, x->nb_mass, "mass")) return;
    x->inPosZ[x->nb_inPosZ].nbr_inlet = (int)idx_inlet;
    x->inPosZ[x->nb_inPosZ].mass = &x->mass[(int)idx_mass];
    x->inPosZ[x->nb_inPosZ].influence = influence;
    x->nb_inPosZ++;
}

void pmpd3d_tilde_inForceX(t_pmpd3d_tilde *x, t_float idx_inlet, t_float idx_mass, t_float influence)
{
    if (!validate_count(x, x->nb_inForceX, x->nb_max_in, "inlet assignment") ||
        !validate_index(x, idx_inlet, x->nb_inlet, "inlet") ||
        !validate_index(x, idx_mass, x->nb_mass, "mass")) return;
    x->inForceX[x->nb_inForceX].nbr_inlet = (int)idx_inlet;
    x->inForceX[x->nb_inForceX].mass = &x->mass[(int)idx_mass];
    x->inForceX[x->nb_inForceX].influence = influence;
    x->nb_inForceX++;
}

void pmpd3d_tilde_inForceY(t_pmpd3d_tilde *x, t_float idx_inlet, t_float idx_mass, t_float influence)
{
    if (!validate_count(x, x->nb_inForceY, x->nb_max_in, "inlet assignment") ||
        !validate_index(x, idx_inlet, x->nb_inlet, "inlet") ||
        !validate_index(x, idx_mass, x->nb_mass, "mass")) return;
    x->inForceY[x->nb_inForceY].nbr_inlet = (int)idx_inlet;
    x->inForceY[x->nb_inForceY].mass = &x->mass[(int)idx_mass];
    x->inForceY[x->nb_inForceY].influence = influence;
    x->nb_inForceY++;
}

void pmpd3d_tilde_inForceZ(t_pmpd3d_tilde *x, t_float idx_inlet, t_float idx_mass, t_float influence)
{
    if (!validate_count(x, x->nb_inForceZ, x->nb_max_in, "inlet assignment") ||
        !validate_index(x, idx_inlet, x->nb_inlet, "inlet") ||
        !validate_index(x, idx_mass, x->nb_mass, "mass")) return;
    x->inForceZ[x->nb_inForceZ].nbr_inlet = (int)idx_inlet;
    x->inForceZ[x->nb_inForceZ].mass = &x->mass[(int)idx_mass];
    x->inForceZ[x->nb_inForceZ].influence = influence;
    x->nb_inForceZ++;
}

void pmpd3d_tilde_outPosX(t_pmpd3d_tilde *x, t_float idx_outlet, t_float idx_mass, t_float influence)
{
    if (!validate_count(x, x->nb_outPosX, x->nb_max_out, "outlet assignment") ||
        !validate_index(x, idx_outlet, x->nb_outlet, "outlet") ||
        !validate_index(x, idx_mass, x->nb_mass, "mass")) return;
    x->outPosX[x->nb_outPosX].nbr_outlet = (int)idx_outlet;
    x->outPosX[x->nb_outPosX].mass = &x->mass[(int)idx_mass];
    x->outPosX[x->nb_outPosX].influence = influence;
    x->nb_outPosX++;
}

void pmpd3d_tilde_outPosY(t_pmpd3d_tilde *x, t_float idx_outlet, t_float idx_mass, t_float influence)
{
    if (!validate_count(x, x->nb_outPosY, x->nb_max_out, "outlet assignment") ||
        !validate_index(x, idx_outlet, x->nb_outlet, "outlet") ||
        !validate_index(x, idx_mass, x->nb_mass, "mass")) return;
    x->outPosY[x->nb_outPosY].nbr_outlet = (int)idx_outlet;
    x->outPosY[x->nb_outPosY].mass = &x->mass[(int)idx_mass];
    x->outPosY[x->nb_outPosY].influence = influence;
    x->nb_outPosY++;
}

void pmpd3d_tilde_outPosZ(t_pmpd3d_tilde *x, t_float idx_outlet, t_float idx_mass, t_float influence)
{
    if (!validate_count(x, x->nb_outPosZ, x->nb_max_out, "outlet assignment") ||
        !validate_index(x, idx_outlet, x->nb_outlet, "outlet") ||
        !validate_index(x, idx_mass, x->nb_mass, "mass")) return;
    x->outPosZ[x->nb_outPosZ].nbr_outlet = (int)idx_outlet;
    x->outPosZ[x->nb_outPosZ].mass = &x->mass[(int)idx_mass];
    x->outPosZ[x->nb_outPosZ].influence = influence;
    x->nb_outPosZ++;
}

void pmpd3d_tilde_outSpeedX(t_pmpd3d_tilde *x, t_float idx_outlet, t_float idx_mass, t_float influence)
{
    if (!validate_count(x, x->nb_outSpeedX, x->nb_max_out, "outlet assignment") ||
        !validate_index(x, idx_outlet, x->nb_outlet, "outlet") ||
        !validate_index(x, idx_mass, x->nb_mass, "mass")) return;
    x->outSpeedX[x->nb_outSpeedX].nbr_outlet = (int)idx_outlet;
    x->outSpeedX[x->nb_outSpeedX].mass = &x->mass[(int)idx_mass];
    x->outSpeedX[x->nb_outSpeedX].influence = influence;
    x->nb_outSpeedX++;
}

void pmpd3d_tilde_outSpeedY(t_pmpd3d_tilde *x, t_float idx_outlet, t_float idx_mass, t_float influence)
{
    if (!validate_count(x, x->nb_outSpeedY, x->nb_max_out, "outlet assignment") ||
        !validate_index(x, idx_outlet, x->nb_outlet, "outlet") ||
        !validate_index(x, idx_mass, x->nb_mass, "mass")) return;
    x->outSpeedY[x->nb_outSpeedY].nbr_outlet = (int)idx_outlet;
    x->outSpeedY[x->nb_outSpeedY].mass = &x->mass[(int)idx_mass];
    x->outSpeedY[x->nb_outSpeedY].influence = influence;
    x->nb_outSpeedY++;
}

void pmpd3d_tilde_outSpeedZ(t_pmpd3d_tilde *x, t_float idx_outlet, t_float idx_mass, t_float influence)
{
    if (!validate_count(x, x->nb_outSpeedZ, x->nb_max_out, "outlet assignment") ||
        !validate_index(x, idx_outlet, x->nb_outlet, "outlet") ||
        !validate_index(x, idx_mass, x->nb_mass, "mass")) return;
    x->outSpeedZ[x->nb_outSpeedZ].nbr_outlet = (int)idx_outlet;
    x->outSpeedZ[x->nb_outSpeedZ].mass = &x->mass[(int)idx_mass];
    x->outSpeedZ[x->nb_outSpeedZ].influence = influence;
    x->nb_outSpeedZ++;
}

void pmpd3d_tilde_outSpeed(t_pmpd3d_tilde *x, t_float idx_outlet, t_float idx_mass, t_float influence)
{
    if (!validate_count(x, x->nb_outSpeed, x->nb_max_out, "outlet assignment") ||
        !validate_index(x, idx_outlet, x->nb_outlet, "outlet") ||
        !validate_index(x, idx_mass, x->nb_mass, "mass")) return;
    x->outSpeed[x->nb_outSpeed].nbr_outlet = (int)idx_outlet;
    x->outSpeed[x->nb_outSpeed].mass = &x->mass[(int)idx_mass];
    x->outSpeed[x->nb_outSpeed].influence = influence;
    x->nb_outSpeed++;
}

void pmpd3d_tilde_reset(t_pmpd3d_tilde *x)
{
    x->nb_link      = 0;
    x->nb_NLlink    = 0;
    x->nb_mass      = 0;
    x->nb_inPosX    = 0;
    x->nb_inPosY    = 0;
    x->nb_inPosZ    = 0;
    x->nb_inForceX  = 0;
    x->nb_inForceY  = 0;
    x->nb_inForceZ  = 0;
    x->nb_outSpeedX = 0;
    x->nb_outSpeedY = 0;
    x->nb_outSpeedZ = 0;
    x->nb_outSpeed  = 0;
    x->nb_outPosX   = 0;
    x->nb_outPosY   = 0;
    x->nb_outPosZ   = 0;
}

void pmpd3d_tilde_free(t_pmpd3d_tilde *x)
{
    if (x->outlet) freebytes(x->outlet, x->nb_outlet * sizeof(t_float));

    if (x->inlet_vector) freebytes(x->inlet_vector, x->nb_inlet * sizeof(t_sample *));
    if (x->outlet_vector) freebytes(x->outlet_vector, x->nb_outlet * sizeof(t_sample *));

    if (x->mass) freebytes(x->mass, x->nb_mass * sizeof(struct _mass));
    if (x->link) freebytes(x->link, x->nb_link * sizeof(struct _link));
    if (x->NLlink) freebytes(x->NLlink, x->nb_NLlink * sizeof(struct _NLlink));

    if (x->inPosX) freebytes(x->inPosX, x->nb_inPosX * sizeof(struct _inPos));
    if (x->inPosY) freebytes(x->inPosY, x->nb_inPosY * sizeof(struct _inPos));
    if (x->inPosZ) freebytes(x->inPosZ, x->nb_inPosZ * sizeof(struct _inPos));
    if (x->inForceX) freebytes(x->inForceX, x->nb_inForceX * sizeof(struct _inForce));
    if (x->inForceY) freebytes(x->inForceY, x->nb_inForceY * sizeof(struct _inForce));
    if (x->inForceZ) freebytes(x->inForceZ, x->nb_inForceZ * sizeof(struct _inForce));
    if (x->outPosX) freebytes(x->outPosX, x->nb_outPosX * sizeof(struct _outPos));
    if (x->outPosY) freebytes(x->outPosY, x->nb_outPosY * sizeof(struct _outPos));
    if (x->outPosZ) freebytes(x->outPosZ, x->nb_outPosZ * sizeof(struct _outPos));
    if (x->outSpeedX) freebytes(x->outSpeedX, x->nb_outSpeedX * sizeof(struct _outSpeed));
    if (x->outSpeedY) freebytes(x->outSpeedY, x->nb_outSpeedY * sizeof(struct _outSpeed));
    if (x->outSpeedZ) freebytes(x->outSpeedZ, x->nb_outSpeedZ * sizeof(struct _outSpeed));
    if (x->outSpeed) freebytes(x->outSpeed, x->nb_outSpeed * sizeof(struct _outSpeed));
}

void *pmpd3d_tilde_new(t_symbol *s, int argc, t_atom *argv)
{
    int i, arg;
    t_pmpd3d_tilde *x = (t_pmpd3d_tilde *)pd_new(pmpd3d_tilde_class);

    pmpd3d_tilde_reset(x);

    x->nb_inlet    = max(1, (int)atom_getfloatarg(0, argc, argv));
    x->nb_outlet   = max(1, (int)atom_getfloatarg(1, argc, argv));
    x->nb_loop     = max(1, (int)atom_getfloatarg(2, argc, argv) );
    x->nb_max_mass = (arg = (int)atom_getfloatarg(3, argc, argv)) > 0 ? arg : NB_MAX_MASS_DEFAULT;
    x->nb_max_link = (arg = (int)atom_getfloatarg(4, argc, argv)) > 0 ? arg : NB_MAX_LINK_DEFAULT;
    x->nb_max_in   = (arg = (int)atom_getfloatarg(5, argc, argv)) > 0 ? arg : NB_MAX_IN_DEFAULT;
    x->nb_max_out  = (arg = (int)atom_getfloatarg(6, argc, argv)) > 0 ? arg : NB_MAX_OUT_DEFAULT;

    x->outlet = (t_float *)getbytes(x->nb_outlet * sizeof(t_float));

    x->inlet_vector = (t_sample **)getbytes(x->nb_inlet * sizeof(t_sample *));
    x->outlet_vector = (t_sample **)getbytes(x->nb_outlet * sizeof(t_sample *));

    x->mass   = (struct _mass *)getbytes(x->nb_max_mass * sizeof(struct _link));
    x->link   = (struct _link *)getbytes(x->nb_max_link * sizeof(struct _link));
    x->NLlink = (struct _NLlink *)getbytes(x->nb_max_link * sizeof(struct _link));

    x->inPosX    = (struct _inPos *)getbytes(x->nb_max_in * sizeof(struct _inPos));
    x->inPosY    = (struct _inPos *)getbytes(x->nb_max_in * sizeof(struct _inPos));
    x->inPosZ    = (struct _inPos *)getbytes(x->nb_max_in * sizeof(struct _inPos));
    x->inForceX  = (struct _inForce *)getbytes(x->nb_max_in * sizeof(struct _inForce));
    x->inForceY  = (struct _inForce *)getbytes(x->nb_max_in * sizeof(struct _inForce));
    x->inForceZ  = (struct _inForce *)getbytes(x->nb_max_in * sizeof(struct _inForce));
    x->outPosX   = (struct _outPos *)getbytes(x->nb_max_out * sizeof(struct _outPos));
    x->outPosY   = (struct _outPos *)getbytes(x->nb_max_out * sizeof(struct _outPos));
    x->outPosZ   = (struct _outPos *)getbytes(x->nb_max_out * sizeof(struct _outPos));
    x->outSpeedX = (struct _outSpeed *)getbytes(x->nb_max_out * sizeof(struct _outSpeed));
    x->outSpeedY = (struct _outSpeed *)getbytes(x->nb_max_out * sizeof(struct _outSpeed));
    x->outSpeedZ = (struct _outSpeed *)getbytes(x->nb_max_out * sizeof(struct _outSpeed));
    x->outSpeed  = (struct _outSpeed *)getbytes(x->nb_max_out * sizeof(struct _outSpeed));

    for(i=0; i<x->nb_inlet-1; i++)
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    for(i=0; i<x->nb_outlet; i++)
        outlet_new(&x->x_obj, &s_signal);

    return (void *)x;
}

PMPD_EXPORT void pmpd3d_tilde_setup(void)
{
    pmpd3d_tilde_class = class_new(gensym("pmpd3d~"), (t_newmethod)pmpd3d_tilde_new, 0, sizeof(t_pmpd3d_tilde), CLASS_DEFAULT, A_GIMME, 0);

    if(!pmpd3d_tilde_class)
        return;

    verbose(4, "pmpd3d~ version %s (%s)", pmpd_tag(), pmpd_sha());

    CLASS_MAINSIGNALIN(pmpd3d_tilde_class, t_pmpd3d_tilde, f);

    class_addbang(pmpd3d_tilde_class, pmpd3d_tilde_bang);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_mass, gensym("mass"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_link, gensym("link"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_NLlink, gensym("NLlink"), A_GIMME, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_inPosX, gensym("inPosX"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_inPosY, gensym("inPosY"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_inPosZ, gensym("inPosZ"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_inForceX, gensym("inForceX"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_inForceY, gensym("inForceY"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_inForceZ, gensym("inForceZ"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_outPosX, gensym("outPosX"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_outPosY, gensym("outPosY"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_outPosZ, gensym("outPosZ"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_outSpeed, gensym("outSpeed"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_outSpeedX, gensym("outSpeedX"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_outSpeedY, gensym("outSpeedY"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_outSpeedZ, gensym("outSpeedZ"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_posX, gensym("posX"), A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_posY, gensym("posY"), A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_posZ, gensym("posZ"), A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_forceX, gensym("forceX"), A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_forceY, gensym("forceY"), A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_forceZ, gensym("forceZ"), A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_setD2, gensym("setDEnv"), A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_setD2offset, gensym("setDEnvOffset"), A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_setK, gensym("setK"), A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_setD, gensym("setD"), A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_setL, gensym("setL"), A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_setLCurrent, gensym("setLCurrent"), A_GIMME, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_setM, gensym("setM"), A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_setNLK, gensym("setNLK"), A_GIMME, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_setNLKPow, gensym("setNLKPow"), A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_setNLD, gensym("setNLD"), A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_setNLL, gensym("setNLL"), A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_setNLLMin, gensym("setNLLMin"), A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_setNLLMax, gensym("setNLLMax"), A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_setNLLCurrent, gensym("setNLLCurrent"), A_GIMME, 0);

    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_reset, gensym("reset"), 0);
    class_addmethod(pmpd3d_tilde_class, (t_method)pmpd3d_tilde_dsp, gensym("dsp"),  A_CANT, 0);
}
