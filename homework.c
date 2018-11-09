#include "homework.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int num_threads;
int resize_factor;


void print_error(char error_msg[MSG_LEN]) {
	//puts(error_msg);
	exit(1);
}

int Gk[3][3] = {
	{1, 2, 1},
	{2, 4, 2},
	{1, 2, 1}
};

int get_type(image *img) {
		if(memcmp(img->type, "P5", TYPE_LEN) == 0) {
			return 0;
		} else {
			return 1;
		}
}

void readInput(const char * fileName, image *img) {

	/* we first open the file */
	FILE *input_file = fopen(fileName, "rb");
	if(input_file == 0) {
		print_error("The file couldn't be opened");
	}

	/* reads the header of the file to see the format */
	char file_header[HEADER_LEN];
	fgets(file_header, HEADER_LEN, input_file);

	/* sets the type of the image */
	memcpy(img->type, file_header, TYPE_LEN);
	img->type[2] = '\0';
	if(memcmp(img->type, "P5", TYPE_LEN) != 0 &&
	   memcmp(img->type, "P6", TYPE_LEN) != 0) {
        print_error("The header type is wrong");
	}

	/* reads with and height of the image */
	if(fscanf(input_file, "%d %d\n", &(img->width), &(img->height)) != 2) {
		print_error("The height and the width were not red corectly");
	}

	/* initialize the matrix of pixels */
	if(get_type(img) == 1)
		img->pixels = (pixel*) malloc(sizeof(pixel*) * img->height  * img->width);
	else
		img->pixels_g = (uchar*) malloc(sizeof(char*) * img->height  * img->width);

	/* reads maxValue */
	if(fscanf(input_file, "%d\n", &(img->maxValue)) != 1) {
		print_error("The height and the width were not red corectly");
	}

	/* reading the image content */
	if(memcmp(img->type, "P5", TYPE_LEN) == 0) {
		fread(img->pixels_g, img->width, img->height, input_file);
	} else {
		fread(img->pixels, img->width * 3, img->height, input_file);
    }

   fclose(input_file);
}

void writeData(const char * fileName, image *img) {

	FILE *output_file = fopen(fileName, "wb+");
	if(output_file == 0) {
		print_error("The file couldn't be opened");
	}

	/* wrinting the type of the image */
	fprintf(output_file, "%s\n", img->type);

	/* writing the width and the height of the image */
	fprintf(output_file, "%d %d\n", img->width, img->height);

	/* writng the maxValue*/
	fprintf(output_file, "%d\n", img->maxValue);

	/* write data back to file */
	if(memcmp(img->type, "P5", TYPE_LEN) == 0) {
		fwrite(img->pixels_g, img->width, img->height, output_file);
	} else {
		fwrite(img->pixels, img->width * 3, img->height, output_file);
    }

	//fclose(output_file);
}


pixel get_arithmetic_mean(image *in, int starti, int startj) {
	pixel new_pixel;
	new_pixel.R = 0;
	new_pixel.G = 0;
	new_pixel.B = 0;
	int sum_R = 0, sum_G = 0, sum_B = 0;

	if(get_type(in) == 1) {
		for(int i = starti; i < starti + resize_factor; i++) {
			for(int j = startj; j < startj + resize_factor; j++) {
				sum_R += in->pixels[i * in->width + j].R;
				sum_G += in->pixels[i * in->width + j].G;
				sum_B += in->pixels[i * in->width + j].B;
			}
		}

		new_pixel.R = (uchar)(sum_R / (resize_factor*resize_factor));
		new_pixel.G = (uchar)(sum_G / (resize_factor*resize_factor));
		new_pixel.B = (uchar)(sum_B / (resize_factor*resize_factor));
	} else {
		for(int i = starti; i < starti + resize_factor; i++) {
			for(int j = startj; j < startj + resize_factor; j++) {
				sum_R += in->pixels_g[i * in->width + j];
			}
		}

		new_pixel.R = (uchar)(sum_R / (resize_factor*resize_factor));
	}

	return new_pixel;
}

void resize_helper(image *in, image *out, pixel (*convert_pixels)(image*, int, int)) {
	int i = 0;
	int j = 0;
	int k = 0;
	int ret = get_type(in);
	int diff1 = in->height % resize_factor;
	int diff2 = in->width % resize_factor;

	for(i = 0; i < in->height-diff1; i += resize_factor) {
		for(j = 0; j < in->width-diff2; j += resize_factor) {
printf("%d\n", k);
			if(ret == 1)
				out->pixels[k++] = convert_pixels(in, i, j);
			else
				out->pixels_g[k++] = (convert_pixels(in, i, j)).R;
			
		}
	}
}

pixel get_even(image *in, int starti, int startj) {
	pixel new_pixel;
	new_pixel.R = 0;
	new_pixel.G = 0;
	new_pixel.B = 0;
	int sum_R = 0, sum_G = 0, sum_B = 0;

	if(get_type(in) == 1) {

		for(int i = starti, k = 0; i < starti + resize_factor && k < 3; i++, k++) {
			for(int j = startj, r = 0; j < startj + resize_factor && r < 3; j++, r++) {
				sum_R += (int)in->pixels[i * in->width + j].R * Gk[k][r];
				sum_G += (int)in->pixels[i * in->width + j].G * Gk[k][r];
				sum_B += (int)in->pixels[i * in->width + j].B * Gk[k][r];
			}
		}

		new_pixel.R = (uchar)(sum_R / (GAUSSIAN_KERNEL_SUM));
		new_pixel.G = (uchar)(sum_G / (GAUSSIAN_KERNEL_SUM));
		new_pixel.B = (uchar)(sum_B / (GAUSSIAN_KERNEL_SUM));

	} else {
		for(int i = starti, k = 0; i < starti + resize_factor && k < 3; i++, k++) {
			for(int j = startj, r = 0; j < startj + resize_factor && r < 3; j++, r++) {
				sum_R += in->pixels_g[i * in->width + j] * Gk[k][r];
			}
		}

		new_pixel.R = (uchar)(sum_R / (GAUSSIAN_KERNEL_SUM));
	}

	return new_pixel;
}

void resize(image *in, image * out) {

	/* initialize the output image*/
	out->maxValue = in->maxValue;
	if(resize_factor % 2 == 0) {
		out->width = in->width / resize_factor;
		out->height = in->height / resize_factor;
	} else {
		out->width = in->width / resize_factor;
		out->height = in->height / resize_factor;
	}

	if(get_type(in) == 1)
		out->pixels = (pixel*) malloc(sizeof(pixel) * out->height * out-> width);
	else
		out->pixels_g = (uchar*) malloc(sizeof(uchar) * out->height * out-> width);
	memcpy(out->type, in->type, TYPE_LEN+1);

	if(resize_factor % 2 == 0) {
		resize_helper(in, out, get_arithmetic_mean);
	} else {
		resize_helper(in, out, get_even);
	}
}
