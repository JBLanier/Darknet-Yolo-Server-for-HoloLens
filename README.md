This code is modified from Darknet by [Joseph Redmon](https://github.com/pjreddie)   
https://github.com/pjreddie/darknet

![Darknet Logo](http://pjreddie.com/media/files/darknet-black-small.png)

# Darknet
Darknet is an open source neural network framework written in C and CUDA. It is fast, easy to install, and supports CPU and GPU computation.

For more information see the [Darknet project website](http://pjreddie.com/darknet).

For questions or issues please use the [Google Group](https://groups.google.com/forum/#!forum/darknet).

# Differences from Original
Changes made to original darknet code are in src/demo.c
Server.h/c contain functionality to interface with Hololens clients

To run server, use:  
`./darknet detector demo /cfg/coco.data /cfg/yolo.cfg yolo.weights`
