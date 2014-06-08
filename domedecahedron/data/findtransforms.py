import random
import dodecahedronverts as dd
from euclid import *
from math import *

flatverts = []
shrunkverts = []

q = Quaternion.new_rotate_axis(pi / 2.0 - atan((1.0 + sqrt(5.0)) / 2.0), Vector3(0, 1, 0))

#print "flatverts:"
for v in dd.verts:
    flatv = (q * Vector3(v[0], v[1], v[2])) * 0.4 # dds are 40cm wide face-to-face
    flatverts.append(Point3(flatv.x, flatv.y, flatv.z))
    shrunkv = flatv * 0.875
    shrunkv = Point3(shrunkv.x, shrunkv.y, shrunkv.z)
    shrunkverts.append(shrunkv)
    #print "    {%sf, %sf, %sf}, // %s" % (shrunkv.x, shrunkv.y, shrunkv.z, flatv)

def facecentroid(face):
    a = Vector3()
    for i in range(5):
        a += flatverts[dd.faces[face][i]]
    a /= 5.0
    return Point3(a.x, a.y, a.z)

def makexlat(face):
    return facecentroid(face) - facecentroid(dd.faceopp[face])

def makerotate(face):
    r = makexlat(face)
    r.normalize()
    return Matrix4.new_rotate_axis(pi * 2.0 / 5.0, r)

def rotateto(fromface, toface, fromface2, toface2):
    fromv = facecentroid(fromface)
    tov = facecentroid(toface)
    fromv2 = facecentroid(fromface2)
    tov2 = facecentroid(toface2)
    m = Matrix4()
    
    def dist(mm, fromv, tov):
        d = tov - mm * fromv
        return d.dot(d)
    
    lastdist = dist(m, fromv, tov)
    
    for q in range(1000):
        m2 = m * makerotate(random.randrange(12))
        newdist = dist(m2, fromv, tov)
        if newdist < lastdist:
            m = m2
            if newdist < 0.00001:
                break
    else:
        print "rotateto FAIL 1!"
        return None
    
    r = makerotate(toface)
    for q in range(5):
        if dist(m, fromv2, tov2) < 0.00001:
            break
        m = m * makerotate(fromface)
    else:
        print "rotateto FAIL 2!"
        return None
    
    return m

xlat = makexlat(10)
rotaxis = xlat.copy().normalize()
xlatb = makexlat(9)
rotaxisb = xlatb.copy().normalize()
xlatc = makexlat(4)
rotaxisc = xlatc.copy().normalize()
xlatd = makexlat(1)
rotaxisd = xlatd.copy().normalize()

f = open("coords.h", "wt")
def writef(s):
    f.write(s + '\n')
    #print s

def printm(m):
    #print "    // [%sf, %sf, %sf, %sf," % (repr(m[0]), repr(m[1]), repr(m[2]), repr(m[3]),)
    #print "    //  %sf, %sf, %sf, %sf," % (repr(m[4]), repr(m[5]), repr(m[6]), repr(m[7]),)
    #print "    //  %sf, %sf, %sf, %sf," % (repr(m[8]), repr(m[9]), repr(m[10]), repr(m[11]),)
    #print "    //  %sf, %sf, %sf, %sf]" % (repr(m[12]), repr(m[13]), repr(m[14]), repr(m[15]),)
    for v in shrunkverts:
        v = m * v
        writef("    {%sf, %sf, %sf}," % (repr(v.x), repr(v.y), repr(v.z),))

print
#print "flower transforms:"
#print "{"
print xlat

def printgroup(g, basem):
    m = basem.copy()
    writef("    // group %d, dodecahedron 0" % (g,))
    printm(m * rotateto(0, 11, 1, 6))
    for i in range(5):
        m = basem.copy()
        m.rotate_axis(-i * pi * 2.0 / 5.0, Vector3(0.0, 0.0, 1.0))
        m.rotate_axis(pi, rotaxis)
        m.translate(xlat.x, xlat.y, xlat.z)
        writef("    // group %d, dodecahedron %d" % (g, i + 1,))
        printm(m * rotateto(0, 4, 1, 0))

m2 = Matrix4.new_rotate_axis(0.5 * pi * 2.0 / 5.0, Vector3(0.0, 0.0, 1.0))


printgroup(0, Matrix4.new_identity())
for i in range(5):
    m = Matrix4.new_identity()
    m.rotate_axis(-i * pi * 2.0 / 5.0, Vector3(0.0, 0.0, 1.0))
    
    m.translate(xlatb.x, xlatb.y, xlatb.z)
    m.translate(xlat.x, xlat.y, xlat.z)
    m.translate(xlat.x, xlat.y, xlat.z)
    
    m.rotate_axis(pi / 5.0, rotaxisb)
    
    m.rotate_axis(2 * pi * 2.0 / 5.0, rotaxisd)
    m.rotate_axis(pi / 5.0, rotaxisc)
    m.translate(xlatc.x, xlatc.y, xlatc.z)
    
    if i == 3:
        m.rotate_axis(1 * pi * 2.0 / 5.0, Vector3(0.0, 0.0, 1.0))
    if i == 4:
        m.rotate_axis(3 * pi * 2.0 / 5.0, Vector3(0.0, 0.0, 1.0))
    
    printgroup(i + 1, m)

"""
# print out unlit verts
for i in range(5):
    m = Matrix4.new_identity()
    m.rotate_axis(-i * pi * 2.0 / 5.0, Vector3(0.0, 0.0, 1.0))
    m.translate(xlat.x * 2, xlat.y * 2, xlat.z * 2)
    writef("    // unlit connectors, dodecahedron %d" % (i + 1,))
    for v in shrunkverts:
        v = m * v
        writef("    {%sf, %sf, %sf}," % (repr(v.x), repr(v.y), repr(v.z),))
# and a filler
for i in range(20):
    writef("    {%sf, %sf, %sf}," % (repr(0.0), repr(0.0), repr(0.0),))
"""

#print "}"
del f
