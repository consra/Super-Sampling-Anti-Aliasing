#ifndef HOMEWORK_H
#define HOMEWORK_H


#define MSG_LEN               20
#define HEADER_LEN            10
#define TYPE_LEN              2
#define GAUSSIAN_KERNEL_SUM   16
typedef unsigned char uchar;

typedef struct {
	uchar R;
	uchar G;
	uchar B;
} pixel;

typedef struct {
	int width;
	int height;
	int maxValue;
	pixel *pixels;
	uchar *pixels_g; 
	uchar type[3];
} image;

void readInput(const char * fileName, image *img);

void writeData(const char * fileName, image *img);

void resize(image *in, image * out);

#endif /* HOMEWORK_H */
