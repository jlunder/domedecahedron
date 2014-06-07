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
]

home = os.environ['HOME']
arduino = os.path.join(home, 'Documents/Arduino/Domedecahedron')
arduino_repo = os.path.join(home, 'Documents/hg/domedecahedron/Arduino')
test_repo = os.path.join(home, 'Documents/hg/domedecahedron/domedecahedron')

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
    sync(os.path.join(arduino, fs[1]), os.path.join(arduino_repo, fs[1]))
    sync(os.path.join(test_repo, fs[0]), os.path.join(arduino_repo, fs[1]))
    sync(os.path.join(test_repo, fs[0]), os.path.join(arduino, fs[1]))

sync(os.path.join(home, 'Documents/Arduino/Domedecahedron/Domedecahedron.ino'),
    os.path.join(home, 'Documents/hg/domedecahedron/Arduino/Domedecahedron/Domedecahedron.ino'))