exeDir = "/home/loukas438/DIT/GeometricalOptimization2/build" 
polygonPath = exeDir + "/initial.wkt" 
pointsPath = exeDir + "/points.wkt" 
hullPath = exeDir + "/optimal.wkt" 
strPolygon = open(polygonPath, 'r').read() 
strPoints = open(pointsPath, 'r').read() 
strHull = open(hullPath, 'r').read() 

iface.show_wkt(f'{strPolygon}', 'initialLayer')

for i in range(1, 12):
    path = exeDir + "/dump" + str(i) + ".wkt"
    strDump = open(path, 'r').read()
    iface.show_wkt(f'{strDump}', f'dump{i}') 

iface.show_wkt(f'{strHull}', 'optimalLayer') 
iface.show_wkt(f'{strPoints}', 'pointsLayer')
 