import glob
import os
import re
Import('env prefix')

pmpd = env.SharedLibrary(target = 'pmpd', source = 'src/pmpd.c')
env.Alias('install', env.Install(os.path.join(prefix, 'extra'), pmpd))
env.Alias('install', env.Install(os.path.join(prefix, 'doc/5.reference'), glob.glob('help/*.pd')))
env.Alias('install', env.Install(os.path.join(prefix, 'doc/5.reference/pmpd'), glob.glob('exemples/*.pd')))
Default(pmpd)
