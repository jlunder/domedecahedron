import math

ISQRT3 = 1.0 / math.sqrt(3.0)
PHI = ISQRT3 * (1.0 + math.sqrt(5.0)) * 0.5
IPHI = ISQRT3 * 2.0 / (1.0 + math.sqrt(5.0)) 
ONE = ISQRT3

# standard winding as determined by our wiring job :)
verts = [
    (  0.0, -IPHI,   PHI,),
    (  0.0,  IPHI,   PHI,),
    ( -ONE,   ONE,   ONE,),
    ( -PHI,   0.0,  IPHI,),
    ( -ONE,  -ONE,   ONE,),
    (-IPHI,  -PHI,   0.0,),
    ( -ONE,  -ONE,  -ONE,),
    ( -PHI,   0.0, -IPHI,),
    ( -ONE,   ONE,  -ONE,),
    (-IPHI,   PHI,   0.0,),
    ( IPHI,   PHI,   0.0,),
    (  ONE,   ONE,   ONE,),
    (  PHI,   0.0,  IPHI,),
    (  ONE,  -ONE,   ONE,),
    ( IPHI,  -PHI,   0.0,),
    (  ONE,  -ONE,  -ONE,),
    (  0.0, -IPHI,  -PHI,),
    (  0.0,  IPHI,  -PHI,),
    (  ONE,   ONE,  -ONE,),
    (  PHI,   0.0, -IPHI,),
]

faces = [
    [0, 4, 3, 2, 1],
    [3, 4, 5, 6, 7],
    [2, 3, 7, 8, 9],
    [1, 2, 9, 10, 11],
    [0, 1, 11, 12, 13],
    [0, 13, 14, 5, 4],
    [5, 14, 15, 16, 6],
    [12, 19, 15, 14, 13],
    [10, 18, 19, 12, 11],
    [8, 17, 18, 10, 9],
    [6, 16, 17, 8, 7],
    [15, 19, 18, 17, 16],
]

faceopp = [11, 8, 7, 6, 10, 9, 3, 2, 1, 5, 4, 0]
