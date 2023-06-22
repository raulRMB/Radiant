set outputpath=C:/Users/Mike/Desktop/LinuxServer
docker build -t "radiantserver:01" %outputpath%/.
docker tag radiantserver:01 customerfar7b4ix6ffps.azurecr.io/radiantserver:01
docker push customerfar7b4ix6ffps.azurecr.io/radiantserver:01
pause