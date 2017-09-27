//
// Created by jb on 4/3/17.
//

#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <memory.h>
#include <stdint.h>
#include <errno.h>

#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <opencv/cv.h>
#include <stdio.h>
#include <stdlib.h>

#include "server.h"


#define PORT 11000
#define WRITE_BUFFER_SIZE 4096

int listenfd = -1;
int clientfd = -1;

char client_write_buffer[WRITE_BUFFER_SIZE];

int connected_to_client = 0;

struct sockaddr_in serv_addr;

int send_int(int num)
{
	int fd = clientfd;
    int32_t conv = htonl(num);
    char *data = (char*)&conv;
    int left = sizeof(conv);
    int rc;
    do {
        rc = write(fd, data, left);
        if (rc < 0) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // use select() or epoll() to wait for the socket to be writable again
            }
            else if (errno != EINTR) {
                return -1;
            }
        }
        else {
            data += rc;
            left -= rc;
        }
    }
    while (left > 0);
    return 0;
}

int acceptClientConnection() {

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	
    listen(listenfd, 10);
	printf("Waiting for client...\n");
    clientfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

    shutdown(listenfd,SHUT_RDWR);
    close(listenfd);

	printf("client connected\n");

	connected_to_client = 1;

    return 0;
}

IplImage* recieveFrame() {
	printf("recieving");
	ssize_t byte_count;
	int32_t frame_size = 0;
	byte_count = recv(clientfd, &frame_size, sizeof(int32_t), 0);
	if (byte_count <= 0) {
		printf("Something went wrong recieving frame size\n");
	}
	frame_size = ntohl(frame_size);
	printf("frame size: %d\n", frame_size);
	unsigned char *frame_buffer = malloc(frame_size);
	unsigned char *buffer_marker = frame_buffer;
	ssize_t total_frame_bytes_recieved = 0;
	while (total_frame_bytes_recieved < frame_size) {
		byte_count = recv(clientfd, buffer_marker, frame_size - total_frame_bytes_recieved, 0);
		if (byte_count <= 0) {
			printf("Something went wrong recieving frame data\n");
		}
		buffer_marker += byte_count;
		total_frame_bytes_recieved += byte_count;
	}
	if (total_frame_bytes_recieved > frame_size) {
		printf("somehow we got too many bytes\n");
	}
	printf("recieved %d bytes\n",total_frame_bytes_recieved);
	CvMat mat = cvMat(1152,2048,CV_8UC3, (void*) frame_buffer);

  	IplImage* img = cvDecodeImage(&mat, CV_LOAD_IMAGE_COLOR);
	
	free(frame_buffer);
	return img;
	
}

char * recieveMatrixString() {
	printf("recieving matrix\n");
	ssize_t byte_count;
	int32_t matrix_size = 128; //2* 16 * 4byte floats for 4x4 matrix
	unsigned char *matrix_buffer = malloc(matrix_size);
	unsigned char *buffer_marker = matrix_buffer;
	ssize_t total_matrix_bytes_recieved = 0;
	while (total_matrix_bytes_recieved < matrix_size) {
		byte_count = recv(clientfd, buffer_marker, matrix_size - total_matrix_bytes_recieved, 0);
		if (byte_count <= 0) {
			printf("Something went wrong recieving matrix data\n");
		}
		buffer_marker += byte_count;
		total_matrix_bytes_recieved += byte_count;
	}
	if (total_matrix_bytes_recieved > matrix_size) {
		printf("somehow we got too many bytes\n");
	}
	printf("recieved %d bytes\n",total_matrix_bytes_recieved);
	
	return matrix_buffer;
}

int terminateClientConnection() {
    shutdown(clientfd,SHUT_RDWR);
	connected_to_client = 0;
    return close(clientfd);
}

image get_image_from_client()
{
	if (connected_to_client != 1) {
		acceptClientConnection();
	}
    IplImage* src = recieveFrame();
    if (!src) return make_empty_image(0,0,0);
    image im = ipl_to_image(src);
    rgbgr_image(im);
    return im;
}

int sendDetectionsHeader(char *matrix, int num, int total_size) {
	//total size of header should be 128 + 4 + 4 = 136 bytes
	write(clientfd,matrix,128);
	send_int(num);
	send_int(total_size);
	return 0;
}

int sendDetectionBox(int left, int top, int right, int bottom, float red, float green, float blue, char *name) {
    send_int(left);
    send_int(top);
    send_int(right);
    send_int(bottom);

    send_int(red * 255);
    send_int(green * 255);
    send_int(blue * 255);

    int size = strlen(name);
    send_int(size);
    write(clientfd,name,size);
    return 0;
}

int sendDetections(image im, char * matrix, int num, float thresh, box *boxes, float **probs, char **names, image **alphabet, int classes) {
	int total_size = 0;
	int i;
	int detected = 0;
    for(i = 0; i < num; ++i){
        int class = max_index(probs[i], classes);
        float prob = probs[i][class];
        if(prob > thresh){
        	detected++;
        	total_size += strlen(names[class]);
            total_size += 32; //8*4byte ints for box parameters   
        } 
    }
    sendDetectionsHeader(matrix, detected, total_size);

    for(i = 0; i < num; ++i){
        int class = max_index(probs[i], classes);
        float prob = probs[i][class];
        if(prob > thresh){

            int width = im.h * .012;

            int offset = class*123457 % classes;
            float red = get_color(2,offset,classes);
            float green = get_color(1,offset,classes);
            float blue = get_color(0,offset,classes);
            float rgb[3];

            //width = prob*20+2;

            rgb[0] = red;
            rgb[1] = green;
            rgb[2] = blue;
            box b = boxes[i];

            int left  = (b.x-b.w/2.)*im.w;
            int right = (b.x+b.w/2.)*im.w;
            int top   = (b.y-b.h/2.)*im.h;
            int bot   = (b.y+b.h/2.)*im.h;

            if(left < 0) left = 0;
            if(right > im.w-1) right = im.w-1;
            if(top < 0) top = 0;
            if(bot > im.h-1) bot = im.h-1;

            sendDetectionBox(left, top, right, bot, red, green, blue, names[class]);
        }
    }


   
}



/*
int main() {
	char * buffer;	
	cvNamedWindow("myfirstwindow", CV_WINDOW_AUTOSIZE);
	acceptClientConnection();
	while (1) {
		IplImage* img = recieveFrame();
		
  		if (!img) {
    			printf("Image can NOT Load!!!\n");
    			return 1;
  		}


		cvShowImage("myfirstwindow", img);
		cvReleaseImage(&img);
  		cvWaitKey(1);
  		
	}
}
*/
