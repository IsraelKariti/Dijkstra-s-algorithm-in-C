# Dijkstra-s-algorithm-in-C  
A project that analyzes .bmp image files and calculate best route alternatives based on Dijkstra's algorithm  
The input for this software is a .bmp file that contain a map.  
All the pixels on the map are white exept for the pixels of the pools on the map which are blue
![image](https://user-images.githubusercontent.com/89970476/226582148-b9ab1b0c-f366-4638-802e-de970f54618d.png)


Once the project is loaded the user can choose between 3 options of choice  
option 1:  
Scan the .bmp file for pools and their respective location.  
The output for this option will be the data (location and size) of the first pool,  
as well as the total number of pools in the .bmp file  
![image](https://user-images.githubusercontent.com/89970476/226576353-5a3cd6c0-329d-4ef6-844d-2458ee8cc889.png)
The list of pools will be also extracted into a .txt file
![image](https://user-images.githubusercontent.com/89970476/226576692-64d5cc13-e2fb-4be1-9ece-9fd6edff1c30.png)



Option 2:  
The list of the pools in the pools.txt file that was created in option1 will be read,  
and the pools in it will be sorted by size, using bubble sort, and the result will be printed to the cosole
![image](https://user-images.githubusercontent.com/89970476/226577008-f3c6c8c2-1970-4a85-87fd-5ac57733055f.png)

Option 3:  
The program will calculate the shortest path between two points.  
The user will choose the starting point, but the end point will always be the top right corner of the .bmp file  
This calculation simultates a scenario where a human traveler has to travel between two points, but has a limited amount of water\oil\fuel  
or any other perishable substance.
The amount of oil is determined by the user.  
The amount of oil\water decreases while the traveler progresses.  
The traveler can only recharge the oil\water in the pools.
The program calculates the shortest path the person has to travel through, and adds pools to the path if the amount of water\oil is too little.  
The program will also print the path on the image
The calculation of the path between the pools will be done using the dijkstra's algorithm,  
Every time the traveler will reach a pool he will compare his current oil amount to the last time in which he was in the same pool.  
The path that wasted the least amount of oil (the shortest path) will be chosen and printed on the picture.  

![image](https://user-images.githubusercontent.com/89970476/226580381-7ddcea63-dab9-440f-8366-42dbfebb25ad.png)
![image](https://user-images.githubusercontent.com/89970476/226580524-b1777785-de08-497c-82e5-302d1c8af2f7.png)
