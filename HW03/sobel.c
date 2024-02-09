#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <math.h>

#include <jpeglib.h>

#include "papi.h"

/* Filters */
static int sobel_x_filter[9]={ -1, 0,+1  ,-2, 0,+2  ,-1, 0,+1};
static int sobel_y_filter[9]={ -1,-2,-1  , 0, 0, 0  , 1, 2,+1};

struct image_t {
	int xsize;
	int ysize;
	int depth;	/* bytes */
	unsigned char *pixels;
};


static void generic_convolve(struct image_t *input_image,
				struct image_t *output_image,
				int *filter) {

// Comment
	/* Look at the above image_t definition			*/
	/* You can use input_image->xsize (width)  		*/
	/*             input_image->ysize (height) 		*/
	/*         and input_image->depth (bytes per pixel)	*/
	/* input_image->pixels points to the RGB values		*/

	/******************/
	/* Your code here */
	/******************/

//	output_image->pixels = input_image->pixels;

	int current;

	for (int i = 3; i < input_image->ysize - 3; i++) {
		for (int j = 3; j < 3 * input_image->xsize - 3; j++) {
			//printf("X: %d Y: %d\n", j, i);		
//			sum = 0;
			//output_image->pixels[input_image->xsize * (j - 1) + i] = 0;
			printf("X: %d Y: %d\n", j, i);
			for (int k = -1; k <= 1; k++) {
				for (int l = -1; l <= 1; l++) {
					current = input_image->xsize * input_image->depth * (j - 1) + i;
					printf("Index: %d\n",  input_image->xsize * input_image->depth * (i + k) + j + l * input_image->depth);				
//	output_image->pixels[current] += input_image->pixels[input_image->xsize * input_image->depth * (i + k) + j + l * input_image->depth];

	//output_image->pixels[current] += input_image->pixels[current] * filter[input_image->depth * (1 + k) + (1 + l)];
	
//	printf("Pixel: %d\n", input_image->xsize * (i + k) + j + l);
//					sum += (input_image->pixels[input_image->xsize * (i + k) + j + l] * filter[3 * (1 + k) + (1 + l)]);

	//printf("Row: %d | Col: %d\n", 7 * (i + l) + j + k, 3 * (1 + l) + (1 - k));	
				//output_image->pixels[7 * (i + l) + j + k] *= sobel_x_filter[3 * (1 + l) + (1 - k)];
				}
			}
/*
			if (sum < 0) {
				sum = 0;
			} else if (sum > 255) {
				sum = 255;
			}
*/
//			printf("Sum: %d\n", sum);

			//printf("%d\n", input_image->xsize * (i - 1) + j);
		//	printf("Sum: %d | Mod 255: %d\n", sum, sum % 255);
		//	output_image->pixels[input_image->xsize * (i - 1) + j] = sum % 255;;
	//		output_image->pixels[input_image->xsize * (j - 1) + i] = 255;


			//	printf("Sum: %d\n", sum);
//output_image->pixels[j * input_image->ysize + i] = input_image->pixels[i * input_image->xsize + j];
		}
	}

	
}

static int combine(struct image_t *sobel_x, struct image_t *sobel_y,
		struct image_t *new_image) {
	int pixel;
	for (int i = 0; i < sobel_x->xsize; i++) {
		for (int j = 0; j < sobel_x->ysize; j++) {
			pixel = sobel_x->xsize * j + i;

			new_image->pixels[pixel] = sobel_x->pixels[pixel] + sobel_y->pixels[pixel];
		}
	}
	/******************/
	/* your code here */
	/******************/

	return 0;
}

static int load_jpeg(char *filename, struct image_t *image) {

	FILE *fff;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW output_data;
	unsigned int scanline_len;
	int scanline_count=0;

	fff=fopen(filename,"rb");
	if (fff==NULL) {
		fprintf(stderr, "Could not load %s: %s\n",
			filename, strerror(errno));
		return -1;
	}

	/* set up jpeg error routines */
	cinfo.err = jpeg_std_error(&jerr);

	/* Initialize cinfo */
	jpeg_create_decompress(&cinfo);

	/* Set input file */
	jpeg_stdio_src(&cinfo, fff);

	/* read header */
	jpeg_read_header(&cinfo, TRUE);

	/* Start decompressor */
	jpeg_start_decompress(&cinfo);

	printf("output_width=%d, output_height=%d, output_components=%d\n",
		cinfo.output_width,
		cinfo.output_height,
		cinfo.output_components);

	image->xsize=cinfo.output_width;
	image->ysize=cinfo.output_height;
	image->depth=cinfo.output_components;

	scanline_len = cinfo.output_width * cinfo.output_components;
	image->pixels=malloc(cinfo.output_width * cinfo.output_height * cinfo.output_components);

	while (scanline_count < cinfo.output_height) {
		output_data = (image->pixels + (scanline_count * scanline_len));
		jpeg_read_scanlines(&cinfo, &output_data, 1);
		scanline_count++;
	}

	/* Finish decompressing */
	jpeg_finish_decompress(&cinfo);

	jpeg_destroy_decompress(&cinfo);

	fclose(fff);

	return 0;
}

static int store_jpeg(char *filename, struct image_t *image) {

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	int quality=90; /* % */
	int i;

	FILE *fff;

	JSAMPROW row_pointer[1];
	int row_stride;

	/* setup error handler */
	cinfo.err = jpeg_std_error(&jerr);

	/* initialize jpeg compression object */
	jpeg_create_compress(&cinfo);

	/* Open file */
	fff = fopen(filename, "wb");
	if (fff==NULL) {
		fprintf(stderr, "can't open %s: %s\n",
			filename,strerror(errno));
		return -1;
	}

	jpeg_stdio_dest(&cinfo, fff);

	/* Set compression parameters */
	cinfo.image_width = image->xsize;
	cinfo.image_height = image->ysize;
	cinfo.input_components = image->depth;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);

	/* start compressing */
	jpeg_start_compress(&cinfo, TRUE);

	row_stride=image->xsize*image->depth;

	for(i=0;i<image->ysize;i++) {
		row_pointer[0] = & image->pixels[i * row_stride];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	/* finish compressing */
	jpeg_finish_compress(&cinfo);

	/* close file */
	fclose(fff);

	/* clean up */
	jpeg_destroy_compress(&cinfo);

	return 0;
}

int main(int argc, char **argv) {

	struct image_t image,sobel_x,sobel_y,new_image;
	int result;

	/* Check command line usage */
	if (argc<2) {
		fprintf(stderr,"Usage: %s image_file\n",argv[0]);
		return -1;
	}

	result=PAPI_library_init(PAPI_VER_CURRENT);
	if (result!=PAPI_VER_CURRENT) {
		fprintf(stderr,"Warning!  PAPI error %s\n",
			PAPI_strerror(result));
	}

	/* Load an image */
	result=load_jpeg(argv[1],&image);
	if (result<0) {
		fprintf(stderr,"Error opening %s\n",argv[1]);
		return -1;
	}

	/* Allocate space for output image */
	new_image.xsize=image.xsize;
	new_image.ysize=image.ysize;
	new_image.depth=image.depth;
	new_image.pixels=calloc(image.xsize*image.ysize*image.depth,sizeof(char));

	/* Allocate space for sobel_x output image */
	sobel_x.xsize=image.xsize;
	sobel_x.ysize=image.ysize;
	sobel_x.depth=image.depth;
	sobel_x.pixels=calloc(image.xsize*image.ysize*image.depth,sizeof(char));

	/* Allocate space for sobel_y output image */
	sobel_y.xsize=image.xsize;
	sobel_y.ysize=image.ysize;
	sobel_y.depth=image.depth;
	sobel_y.pixels=calloc(image.xsize*image.ysize*image.depth,sizeof(char));

	/* do the Sobel X convolution */
	generic_convolve(&image,&sobel_x,
			sobel_x_filter);

	/* Uncomment to generate an image for the x results */
	store_jpeg("outx.jpg",&sobel_x);

	/* do the Sobel Y convolution */
	generic_convolve(&image,&sobel_y,
			sobel_y_filter);

	/* Uncomment to generate an image for the y results */
	store_jpeg("outy.jpg",&sobel_y);

	/* Combine the results */
	combine(&sobel_x,&sobel_y,&new_image);

	/* Write out the result */
	store_jpeg("out.jpg",&new_image);

	PAPI_shutdown();

	return 0;
}
