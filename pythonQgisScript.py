exeDir = "/home/loukas438/DIT/GeometricalOptimization/GeometricalOptimization/build" 
polygonPath = exeDir + "/polygon.wkt" 
pointsPath = exeDir + "/points.wkt" 
hullPath = exeDir + "/hull.wkt" 
strPolygon = open(polygonPath, 'r').read() 
strPoints = open(pointsPath, 'r').read() 
strHull = open(hullPath, 'r').read() 


iface.show_wkt(f'{strHull}', 'hullLayer') 
iface.show_wkt(f'{strPolygon}', 'polygonLayer')
iface.show_wkt(f'{strPoints}', 'pointsLayer')
 