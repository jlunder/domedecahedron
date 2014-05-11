from illuminatedpoints import points

"""
print 'points = [ # ' + str(len(points)) + ' flowers'
for a in points:
    print '    [ # ' + str(len(a)) + ' dodecahedrons'
    for b in a:
        print '        [ # ' + str(len(b)) + ' points'
        for c in b:
            print '            ' + repr(c) + ','
        print '        ],'
    print '    ],'
print ']'
"""

vertexindices = {}
vertices = []
indices = []
dodecahedronindices = []
groupindices = []

for a in points:
    groupindices.append((len(dodecahedronindices), len(a),))
    for b in a:
        dodecahedronindices.append((len(vertexindices), len(b),))
        for c in b:
            if not (c in vertexindices):
                vertexindices[c] = len(vertices)
                vertices.append(c)
            indices.append(vertexindices[c])

print 'typedef struct {'
print '    float x, y, z;'
print '} vertex_t;'
print
print 'typedef uint16_t index_t;'
print
print 'typedef struct {'
print '    index_t * index_start;'
print '    size_t num_indices;'
print '} shape_t;'
print
print 'typedef struct {'
print '    index_t * shape_start;'
print '    size_t num_shapes;'
print '} group_t;'
print
print 'vertex_t vertices[%d] = {' % (len(vertices),)
for a in vertices:
    print '    {%s, %s, %s,},' % (repr(a[0]), repr(a[1]), repr(a[2]),)
print '};'
print
print 'uint16_t indices[%d] = {' % (len(indices),)
for i in range((len(indices) + 9) / 10):
    print '    ' + ' '.join(['%5d,' % (a,) for a in indices[i * 10:(i + 1) * 10]])
print '};'
print
print 'shape_t shapes[%d] = {' % (len(dodecahedronindices),)
for a in dodecahedronindices:
    print '    {.index_start = indices + %d, .num_indices = %d,},' % (a[0], a[1],)
print '};'
print
print 'group_t groups[%d] = {' % (len(groupindices),)
for a in groupindices:
    print '    {.shape_start = shapes + %d, .num_shapes = %d},' % (a[0], a[1],)
print '};'
