exeDir = "/home/loukas438/DIT/GeometricalOptimization/GeometricalOptimization2/build" 
polygonPath = exeDir + "/initial.wkt" 
pointsPath = exeDir + "/points.wkt" 
hullPath = exeDir + "/optimal.wkt" 
strPolygon = open(polygonPath, 'r').read() 
strPoints = open(pointsPath, 'r').read() 
strHull = open(hullPath, 'r').read() 


iface.show_wkt(f'{strHull}', 'hullLayer') 
iface.show_wkt(f'{strPolygon}', 'polygonLayer')
iface.show_wkt(f'{strPoints}', 'pointsLayer')
 