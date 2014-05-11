from illuminatedpoints import points
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

def makexlat(face):
    a = Point3()
    b = Point3()
    for i in range(5):
        a += flatverts[dd.faces[dd.faceopp[face]][i]]
        b += flatverts[dd.faces[face][i]]
    return (b - a) / 5.0

xlat = makexlat(10)
rotaxis = xlat.copy().normalize()
xlatb = makexlat(9)
rotaxisb = xlatb.copy().normalize()
xlatc = makexlat(4)
rotaxisc = xlatc.copy().normalize()
xlatd = makexlat(1)
rotaxisd = xlatd.copy().normalize()

f = open("coords.h", "wt")

def printm(m):
    #print "    // [%sf, %sf, %sf, %sf," % (repr(m[0]), repr(m[1]), repr(m[2]), repr(m[3]),)
    #print "    //  %sf, %sf, %sf, %sf," % (repr(m[4]), repr(m[5]), repr(m[6]), repr(m[7]),)
    #print "    //  %sf, %sf, %sf, %sf," % (repr(m[8]), repr(m[9]), repr(m[10]), repr(m[11]),)
    #print "    //  %sf, %sf, %sf, %sf]" % (repr(m[12]), repr(m[13]), repr(m[14]), repr(m[15]),)
    for v in shrunkverts:
        v = m * v
        #print "    {%sf, %sf, %sf}," % (repr(v.x), repr(v.y), repr(v.z),)
        f.write("    {%sf, %sf, %sf},\n" % (repr(v.x), repr(v.y), repr(v.z),))

print
#print "flower transforms:"
#print "{"
print xlat

def printgroup(g, basem):
    m = basem.copy()
    #print "    // group %d, dodecahedron 0" % (g,)
    f.write("    // group %d, dodecahedron 0\n" % (g,))
    printm(m)
    for i in range(5):
        m = basem.copy()
        m.rotate_axis(-i * pi * 2.0 / 5.0, Vector3(0.0, 0.0, 1.0))
        m.rotate_axis(pi, rotaxis)
        m.translate(xlat.x, xlat.y, xlat.z)
        #print "    // group %d, dodecahedron %d" % (g, i + 1,)
        f.write("    // group %d, dodecahedron %d\n" % (g, i + 1,))
        printm(m)

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
    
    printgroup(i + 1, m)

"""
# print out unlit verts
for i in range(5):
    m = Matrix4.new_identity()
    m.rotate_axis(-i * pi * 2.0 / 5.0, Vector3(0.0, 0.0, 1.0))
    m.translate(xlat.x * 2, xlat.y * 2, xlat.z * 2)
    #print "    // group %d, dodecahedron %d" % (g, i + 1,)
    f.write("    // unlit connectors, dodecahedron %d\n" % (i + 1,))
    for v in shrunkverts:
        v = m * v
        #print "    {%sf, %sf, %sf}," % (repr(v.x), repr(v.y), repr(v.z),)
        f.write("    {%sf, %sf, %sf},\n" % (repr(v.x), repr(v.y), repr(v.z),))
# and a filler
for i in range(20):
    f.write("    {%sf, %sf, %sf},\n" % (repr(0.0), repr(0.0), repr(0.0),))
"""

#print "}"
del f
