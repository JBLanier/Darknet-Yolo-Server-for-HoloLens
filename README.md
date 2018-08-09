This code is forked from Darknet by [Joseph Redmon](https://github.com/pjreddie)   
https://github.com/pjreddie/darknet

## Differences from Original
Changes made to original darknet code (at the time) are in [src/demo.c](src/demo.c).  
[server.h](src/server.h) and [server.c](src/server.c) contain functionality to interface with a HoloLens client.

## To Run:

To run the server, use:  
```
./darknet detector demo /cfg/coco.data /cfg/yolo.cfg yolo.weights
```
To run the server without having to hassle with old cuda and opencv versions (you really don't), with [Nvidia Docker](https://github.com/NVIDIA/nvidia-docker) installed, use:
```
docker build -t yolo-server .
docker run --runtime=nvidia -it yolo-server
```
