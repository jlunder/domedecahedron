import os, shutil

files = [
    ('domedecahedron.c', 'domedecahedron_lib.cpp',),
    ('domedecahedron.h', 'domedecahedron.h',),
    ('effect.c', 'effect.cpp',),
    ('effect.h', 'effect.h',),
    ('effect_util.c', 'effect_util.cpp',),
    ('effect_util.h', 'effect_util.h',),
    ('dais_input.c', 'dais_input.cpp',),
    ('dais_input.h', 'dais_input.h',),
    ('fix16.c', 'fix16.cpp',),
    ('fix16.h', 'fix16.h',),
    ('fix16_exp.c', 'fix16_exp.cpp',),
    ('fix16_sqrt.c', 'fix16_sqrt.cpp',),
    ('fix16_str.c', 'fix16_str.cpp',),
    ('fix16_trig.c', 'fix16_trig.cpp',),
    ('fix16_trig_sin_lut.h', 'fix16_trig_sin_lut.h',),
    ('fixmath.h', 'fixmath.h',),
    ('fract32.c', 'fract32.cpp',),
    ('fract32.h', 'fract32.h',),
    ('int64.h', 'int64.h',),
    ('uint32.c', 'uint32.cpp',),
    ('uint32.h', 'uint32.h',),
]

home = os.environ['HOME']
#arduino = os.path.join(home, 'Documents/Arduino/Domedecahedron')
#arduino_repo = os.path.join(home, 'Documents/hg/domedecahedron/Arduino/Domedecahedron')
#test_repo = os.path.join(home, 'Documents/hg/domedecahedron/domedecahedron')
arduino_repo = '../Arduino/Domedecahedron'
test_repo = '.'

def sync(a, b):
    if not os.path.isfile(a) and not os.path.isfile(b):
        return
    elif os.path.isfile(a) and not os.path.isfile(b):
        print '%s does not exist' % (b,)
        shutil.copy2(a, b)
    elif not os.path.isfile(a) and os.path.isfile(b):
        print '%s does not exist' % (a,)
        shutil.copy2(b, a)
    elif os.path.getmtime(a) > os.path.getmtime(b):
        print '%s newer than %s' % (a, b,)
        shutil.copy2(a, b)
    elif os.path.getmtime(a) < os.path.getmtime(b):
        print '%s newer than %s' % (b, a,)
        shutil.copy2(b, a)

for fs in files:
    #sync(os.path.join(arduino, fs[1]), os.path.join(arduino_repo, fs[1]))
    sync(os.path.join(test_repo, fs[0]), os.path.join(arduino_repo, fs[1]))
    #sync(os.path.join(test_repo, fs[0]), os.path.join(arduino, fs[1]))

sync(os.path.join(home, 'Documents/Arduino/Domedecahedron/Domedecahedron.ino'),
    os.path.join(home, 'Documents/hg/domedecahedron/Arduino/Domedecahedron/Domedecahedron.ino'))