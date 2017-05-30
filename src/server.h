//
// Created by jb on 4/3/17.
//

#ifndef BOX_H
#define BOX_H

typedef struct{
    float x, y, w, h;
} box;

#endif

#ifndef DARKNET_SERVER_H
#define DARKNET_SERVER_H

#include "image.h"

void hello();

int acceptClientConnection();

IplImage* recieveFrame();

int sendDetections(image im, char *matrix, int num, float thresh, box *boxes, float **probs, char **names, image **alphabet, int classes);

int terminateConnection();

char *recieveMatrixString();

image get_image_from_client();

#endif //DARKNET_SERVER_H
