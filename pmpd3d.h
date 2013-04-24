/*
 *  pmpd3d.h
 *  
 */

#define nb_max_link   100000
#define nb_max_mass   100000

#define max(a,b) ( ((a) > (b)) ? (a) : (b) ) 
#define min(a,b) ( ((a) < (b)) ? (a) : (b) ) 

static t_class *pmpd3d_class;

typedef struct _mass {
    t_symbol *Id;
    int mobile;
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
    t_float D2;
    t_float D2offset;
    t_float overdamp;
    int num;
} foo;

typedef struct _link {
    t_symbol *Id;
    int lType;
    struct _mass *mass1;
    struct _mass *mass2;
    t_int active;
    t_float K;
    t_float D;
    t_float L;	// taille du lien
    t_float Pow;
    t_float Lmin;
    t_float Lmax;
    t_float distance; // memoire de la taille du lien
    t_float VX; // vecteur portant la liaison, si c'est le cas
    t_float VY;
    t_float VZ;
    t_symbol *arrayK;
    t_symbol *arrayD;
    t_float K_L; // longeur du tabeau K
    t_float D_L; // longeur du tabeau D
    t_float forceX;
    t_float forceY;
    t_float forceZ;
} foo1 ;

typedef struct _pmpd3d {
	t_object  x_obj;
    struct _link link[nb_max_link];
    struct _mass mass[nb_max_mass];
    t_outlet *main_outlet;
    t_outlet *info_outlet;
    int nb_link;
    int nb_mass;
    t_float minX, maxX, minY, maxY, minZ, maxZ;
    t_int grab; // si on grab une mass ou pas
    t_int grab_nb; // la masse grabé
} t_pmpd3d;
