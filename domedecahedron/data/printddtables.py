from operator import itemgetter
import dodecahedronverts
from dodecahedronverts import verts

faceverts = [
    [ 0,  4,  3,  2,  1],
    [ 3,  4,  5,  6,  7],
    [ 2,  3,  7,  8,  9],
    [ 1,  2,  9, 10, 11],
    [ 0,  1, 11, 12, 13],
    [ 0, 13, 14,  5,  4],
    [ 5, 14, 15, 16,  6],
    [12, 19, 15, 14, 13],
    [10, 18, 19, 12, 11],
    [ 8, 17, 18, 10,  9],
    [ 6, 16, 17,  8,  7],
    [15, 19, 18, 17, 16],
]

print
for v in verts:
    print "    {%sf, %sf, %sf}," % (repr(v[0]), repr(v[1]), repr(v[2]),)

def distsqr(a, b):
    dx = b[0] - a[0]
    dy = b[1] - a[1]
    dz = b[2] - a[2]
    return dx * dx + dy * dy + dz * dz

def nearest3(i):
    l = [(j, distsqr(verts[i], verts[j]),) for j in range(20)]
    return sorted([x[0] for x in sorted(l, key=itemgetter(1))[1:4]])

def middle6(i):
    l = [(j, distsqr(verts[i], verts[j]),) for j in range(20)]
    return sorted([x[0] for x in sorted(l, key=itemgetter(1))[4:10]])

def middlefar6(i):
    l = [(j, distsqr(verts[i], verts[j]),) for j in range(20)]
    return sorted([x[0] for x in sorted(l, key=itemgetter(1))[10:16]])

def far3(i):
    l = [(j, distsqr(verts[i], verts[j]),) for j in range(20)]
    return sorted([x[0] for x in sorted(l, key=itemgetter(1))[16:19]])

def farthest1(i):
    l = [(j, -distsqr(verts[i], verts[j]),) for j in range(20)]
    return sorted(l, key=itemgetter(1))[0][0]

print
print "adjacency:"
for vv in [nearest3(i) for i in range(20)]:
    print "{%d, %d, %d}," % (vv[0], vv[1], vv[2],)
print
print "adjacency-midnear:"
for vv in [middle6(i) for i in range(20)]:
    print "{%d, %d, %d, %d, %d, %d}," % (vv[0], vv[1], vv[2], vv[3], vv[4], vv[5],)
print
print "adjacency-midfar:"
for vv in [middlefar6(i) for i in range(20)]:
    print "{%d, %d, %d, %d, %d, %d}," % (vv[0], vv[1], vv[2], vv[3], vv[4], vv[5],)
print
print "adjacency-far:"
for vv in [far3(i) for i in range(20)]:
    print "{%d, %d, %d}," % (vv[0], vv[1], vv[2],)
print
print "opposition:"
print [farthest1(i) for i in range(20)]
print
print "face-adjacency:"
for i in range(12):
    faces = {}
    for j in range(5):
        for k in range(12):
            if k == i:
                continue
            for l in range(5):
                if faceverts[i][j] == faceverts[k][l]:
                    faces[k] = True;
    faces = list(faces.keys())
    faces.sort()
    print "{%2d, %2d, %2d, %2d, %2d}," % (faces[0], faces[1], faces[2], faces[3], faces[4],)
print
print "face-opposition:"
print dodecahedronverts.faceopp

