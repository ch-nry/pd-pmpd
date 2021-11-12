# Makefile to build class 'helloworld' for Pure Data.
# Needs Makefile.pdlibbuilder as helper makefile for platform-dependent build
# settings and rules.

# library name
lib.name = pmpd

# input source file (class name == source file basename)
class.sources = \
iAmbient2D.c \
iAmbient3D.c \
iCircle2D.c \
iCircle3D.c \
iCylinder3D.c \
iLine2D.c \
iPlane3D.c \
iSeg2D.c \
iSphere3D.c \
link.c \
link2D.c \
link3D.c \
mass.c \
mass2D.c \
mass3D.c \
pmpd.c \
pmpd2d.c \
pmpd3d.c \
pmpd~.c \
tCircle2D.c \
tCircle3D.c \
tCube3D.c \
tCylinder3D.c \
tLine2D.c \
tLink2D.c \
tLink3D.c \
tPlane3D.c \
tSeg2D.c \
tSphere3D.c \
tSquare2D.c

# all extra files to be included in binary distribution of the library
datafiles = \
examples/00_pmpd.pd \
examples/01_basics.pd \
examples/02_string.pd \
examples/03_chaos2D.pd \
examples/04_3D_exemple.pd \
examples/05_corde2D.pd \
examples/06_pyramide3D.pd \
examples/07_corde3D.pd \
examples/08_ball2D.pd \
examples/09_tutorial2D.pd \
examples/10_game.pd \
examples/11_comportement.pd \
examples/12_exitation.pd \
examples/13_plane3D.pd \
examples/14_MP_curve3d.pd \
examples/15_constant_force_field.pd \
examples/16_name_and_interactors.pd \
examples/17_rnd_mouvmnt_and_obstacles.pd \
examples/18_flipper.pd \
examples/19_vertex.pd \
examples/20_moving_vertex.pd \
examples/21_fluid_circulation_cylinder.pd \
examples/22_gaz_molecules.pd \
examples/23_test.pd \
examples/24_sand.pd \
examples/25_sand2.pd \
examples/26_sand3.pd \
examples/27_tLia.pd \
examples/28_Lia.pd \
examples/29_aglom.pd \
examples/30_falling_aglom.pd \
examples/31_paste.pd \
examples/32_Kelvin_Helmoltz_instability.pd \
examples/33_vorticity_ellipse.pd \
examples/34_cigarette_smoke.pd \
examples/35_gravitation.pd \
examples/36_3D_interactors.pd \
examples/37_hollywood_planette_explosion.pd \
examples/38_elastique_membrane_on_a_sphere.pd \
examples/39_blob.pd \
examples/40_i3D.pd \
examples/41_morfing.pd \
examples/42_tentacule.pd \
examples/43_game.pd \
examples/44_flag.pd \
examples/45_newWave.pd \
examples/46_non_linear.pd \
examples/47_scann_synth.pd \
examples/48_pmpd~.pd \
examples/49_simple_oscilator~.pd \
examples/50_string~.pd \
examples/51_pmpd_test.pd \
examples/52_pmpd_string.pd \
examples/53_pmpd_wave.pd \
examples/54_pmpd2d_test.pd \
examples/55_pmpd2d_additive.pd \
examples/56_pmpd2d_ball.pd \
examples/57_pmpd2d_gravitation.pd \
examples/58_pmpd2d_sand.pd \
examples/59_pmpd3d_test.pd \
examples/60_draw_link_force.pd \
examples/61_draw_link_force_2d.pd \
examples/62_NL.pd \
examples/63_random_network.pd \
examples/64_distordable_object.pd \
examples/65_2d_interactors.pd \
examples/66_setK_table.pd \
examples/aglom.pd \
examples/aglom2.pd \
examples/aglom3.pd \
examples/aglom4.pd \
examples/blob.pd \
examples/ch_gemwin.pd \
examples/ch_uzi.pd \
examples/constructor.pd \
examples/explose1.pd \
examples/explose2.pd \
examples/fluide_mass.pd \
examples/fluide_mass2.pd \
examples/fluide_mass3.pd \
examples/fluide_mass4.pd \
examples/fluide_mass5.pd \
examples/fluide_mass6.pd \
examples/fluide_masse.pd \
examples/fluide_masse2.pd \
examples/fluide_masse3.pd \
examples/fluide_masse4.pd \
examples/fluide_masse5.pd \
examples/fluide_masse6.pd \
examples/game_line.pd \
examples/gemLia.pd \
examples/gemLia2.pd \
examples/gemMasse.pd \
examples/gemMasse2.pd \
examples/gemMasse3.pd \
examples/i3D.pd \
examples/i3D2.pd \
examples/mass_link.pd \
examples/mass_link2.pd \
examples/mass_link3.pd \
examples/mountain.pd \
examples/pd_lia.pd \
examples/pd_lia2.pd \
examples/pd_link.pd \
examples/pd_link2.pd \
examples/pd_mass.pd \
examples/pd_mass2.pd \
examples/pd_masse.pd \
examples/pd_masse2.pd \
examples/rain.pd \
examples/rain1.pd \
examples/sand.pd \
examples/smoke.pd \
examples/smoke1.pd \
examples/smoke_vortex.pd \
examples/tut_link.pd \
examples/tut_mass.pd \
examples/tut_masse.pd \
examples/vortex.pd \
examples/vortex2.pd \
examples/sound_sable.pd \
manual/pmpd.pdf \
manual/pmpd.sxw


# include Makefile.pdlibbuilder from submodule directory 'pd-lib-builder'
PDLIBBUILDER_DIR=pd-lib-builder/
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder