import glob
import os
Import('env prefix')

pmpd = env.SharedLibrary(target = 'pmpd', source = 'src/pmpd.c')
env.Alias('install', env.Install(os.path.join(prefix, 'extra'), pmpd))
env.Alias('install', env.Install(os.path.join(prefix, 'doc/pmpd'), glob.glob('help/*.pd')))
env.Alias('install', env.Install(os.path.join(prefix, 'doc/pmpd/exemples'), glob.glob('exemples/*.pd')))
Default(pmpd)
