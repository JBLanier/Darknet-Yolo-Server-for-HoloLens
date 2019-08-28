This code is forked from Darknet by [Joseph Redmon](https://github.com/pjreddie)   
https://github.com/pjreddie/darknet

## Differences from Original
Changes made to original darknet code (at the time) are in [src/demo.c](src/demo.c).  
[server.h](src/server.h) and [server.c](src/server.c) contain functionality to interface with a HoloLens client.

## Installation
If you can, just use nvidia-docker. 

If not, you need to first install OpenCV 2.4.13, CUDA 8, and Cudnn 5 (other versions may work, but I've only tested with these on linux). 

Then compile with 
```
make
```

For more details, check out the [darknet installation guide](https://pjreddie.com/darknet/install/) or the [original repo](https://github.com/pjreddie/darknet)

## To Run:

To run the server, use:  
```
./darknet detector demo cfg/coco.data cfg/yolo.cfg yolo.weights
```
To run the server without having to hassle with old cuda and opencv versions (you really don't), with [Nvidia Docker](https://github.com/NVIDIA/nvidia-docker) installed, use:
```
docker build -t yolo-server .
docker run --runtime=nvidia -p 11000:11000 -it yolo-server
```
