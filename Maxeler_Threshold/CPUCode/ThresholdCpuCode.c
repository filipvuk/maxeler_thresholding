#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Maxfiles.h"
#include <MaxSLiCInterface.h>

#include "timer.h"

#include <string.h>

#define BITS_PER_PIXEL 24

#define BITMAPFILEHEADER_SIZE 14
#define BITMAPINFOHEADER_SIZE 40

//assumption: little-endian
void write_BITMAPFILEHEADER(FILE* f, unsigned int width, unsigned int height)
  {
	unsigned int offset = BITMAPFILEHEADER_SIZE + BITMAPINFOHEADER_SIZE, fileSize = width*height*(BITS_PER_PIXEL/8) + offset;
	unsigned char arrayToWrite[4];

	//must always be set to 'BM' to declare that this is a .bmp-file
	arrayToWrite[0] = 'B';
	arrayToWrite[1] = 'M';
    fwrite(arrayToWrite, sizeof(char), 2, f);

	//the size of the file in bytes
	arrayToWrite[0] = (unsigned char)(fileSize);
	arrayToWrite[1] = (unsigned char)(fileSize >> 8);
	arrayToWrite[2] = (unsigned char)(fileSize >> 16);
	arrayToWrite[3] = (unsigned char)(fileSize >> 24);
	fwrite(arrayToWrite, sizeof(char), 4, f);

	//reserved, must always be set to zero
	arrayToWrite[0] = 0;
	arrayToWrite[1] = 0;
	arrayToWrite[2] = 0;
	arrayToWrite[3] = 0;
	fwrite(arrayToWrite, sizeof(char), 4, f);

	//the offset from the beginning of the file to the bitmap data
	arrayToWrite[0] = (unsigned char)(offset);
	arrayToWrite[1] = (unsigned char)(offset >> 8);
	arrayToWrite[2] = (unsigned char)(offset >> 16);
	arrayToWrite[3] = (unsigned char)(offset >> 24);
	fwrite(arrayToWrite, sizeof(char), 4, f);
  }

//assumption: little-endian
void write_BITMAPINFOHEADER(FILE* f, unsigned int width, unsigned int height)
  {
    unsigned char arrayToWrite[4];

	//the size of the BITMAPINFOHEADER structure, in bytes
	arrayToWrite[0] = (unsigned char)(BITMAPINFOHEADER_SIZE);
	arrayToWrite[1] = (unsigned char)(BITMAPINFOHEADER_SIZE >> 8);
	arrayToWrite[2] = (unsigned char)(BITMAPINFOHEADER_SIZE >> 16);
	arrayToWrite[3] = (unsigned char)(BITMAPINFOHEADER_SIZE >> 24);
    fwrite(arrayToWrite, sizeof(char), 4, f);

	//the width of the image, in pixels
	arrayToWrite[0] = (unsigned char)(width);
	arrayToWrite[1] = (unsigned char)(width >> 8);
	arrayToWrite[2] = (unsigned char)(width >> 16);
	arrayToWrite[3] = (unsigned char)(width >> 24);
    fwrite(arrayToWrite, sizeof(char), 4, f);

	//the height of the image, in pixels
	arrayToWrite[0] = (unsigned char)(height);
	arrayToWrite[1] = (unsigned char)(height >> 8);
	arrayToWrite[2] = (unsigned char)(height >> 16);
	arrayToWrite[3] = (unsigned char)(height >> 24);
    fwrite(arrayToWrite, sizeof(char), 4, f);

	//the number of planes of the target device, must be set to one
	arrayToWrite[0] = 1;
	arrayToWrite[1] = 0;
    fwrite(arrayToWrite, sizeof(char), 2, f);

	//the number of bits per pixel
	arrayToWrite[0] = (unsigned char)(BITS_PER_PIXEL);
	arrayToWrite[1] = (unsigned char)(BITS_PER_PIXEL >> 8);
    fwrite(arrayToWrite, sizeof(char), 2, f);

	//the type of compression, usually set to zero (no compression)
	arrayToWrite[0] = 0;
	arrayToWrite[1] = 0;
	arrayToWrite[2] = 0;
	arrayToWrite[3] = 0;
    fwrite(arrayToWrite, sizeof(char), 4, f);

	//the size of the image data, in bytes. If there is no compression, it is valid to set this member to zero
	arrayToWrite[0] = 0;
	arrayToWrite[1] = 0;
	arrayToWrite[2] = 0;
	arrayToWrite[3] = 0;
    fwrite(arrayToWrite, sizeof(char), 4, f);

	//the horizontal pixels per meter on the designated targer device, usually set to zero
	arrayToWrite[0] = 0;
	arrayToWrite[1] = 0;
	arrayToWrite[2] = 0;
	arrayToWrite[3] = 0;
    fwrite(arrayToWrite, sizeof(char), 4, f);

	//the vertical pixels per meter on the designated targer device, usually set to zero
	arrayToWrite[0] = 0;
	arrayToWrite[1] = 0;
	arrayToWrite[2] = 0;
	arrayToWrite[3] = 0;
    fwrite(arrayToWrite, sizeof(char), 4, f);

	//the number of colors used in the bitmap, if set to zero the number of colors is calculated using the biBitCount member
	arrayToWrite[0] = 0;
	arrayToWrite[1] = 0;
	arrayToWrite[2] = 0;
	arrayToWrite[3] = 0;
    fwrite(arrayToWrite, sizeof(char), 4, f);

	//the number of color that are 'important' for the bitmap, if set to zero, all colors are important
	arrayToWrite[0] = 0;
	arrayToWrite[1] = 0;
	arrayToWrite[2] = 0;
	arrayToWrite[3] = 0;
    fwrite(arrayToWrite, sizeof(char), 4, f);
  }

void extractPixels(char* imageName, unsigned char** bitmapBits, int* width, int* height)
  {
	FILE* f = fopen(imageName,"rb");
	unsigned char arrayToRead[4], skipArray[1100];
	unsigned int offset;

	//BM
	fread(arrayToRead, sizeof(char), 2, f);

	//the size of the file in bytes
	fread(arrayToRead, sizeof(char), 4, f);

	//reserved
	fread(arrayToRead, sizeof(char), 4, f);

	//the offset from the beginning of the file to the bitmap data
	fread(arrayToRead, sizeof(char), 4, f);
	offset = (unsigned int)arrayToRead[0];
	offset = offset | ((unsigned int)arrayToRead[1] << 8);
	offset = offset | ((unsigned int)arrayToRead[2] << 16);
	offset = offset | ((unsigned int)arrayToRead[3] << 24);

	//the size of the BITMAPINFOHEADER structure, in bytes
    fread(arrayToRead, sizeof(char), 4, f);

	//the width of the image, in pixels
    fread(arrayToRead, sizeof(char), 4, f);
	*width = (unsigned int)arrayToRead[0];
	*width = *width | ((unsigned int)arrayToRead[1] << 8);
	*width = *width | ((unsigned int)arrayToRead[2] << 16);
	*width = *width | ((unsigned int)arrayToRead[3] << 24);

	//the height of the image, in pixels
    fread(arrayToRead, sizeof(char), 4, f);
	*height = (unsigned int)arrayToRead[0];
	*height = *height | ((unsigned int)arrayToRead[1] << 8);
	*height = *height | ((unsigned int)arrayToRead[2] << 16);
	*height = *height | ((unsigned int)arrayToRead[3] << 24);

	//the number of planes of the target device
	fread(arrayToRead, sizeof(char), 2, f);

	//the number of bits per pixel
    fread(arrayToRead, sizeof(char), 2, f);

	//the type of compression
    fread(arrayToRead, sizeof(char), 4, f);

	//the size of the image data, in bytes
    fread(arrayToRead, sizeof(char), 4, f);

	//the horizontal pixels per meter on the designated targer device
    fread(arrayToRead, sizeof(char), 4, f);

	//the vertical pixels per meter on the designated targer device
    fread(arrayToRead, sizeof(char), 4, f);

	//the number of colors used in the bitmap
    fread(arrayToRead, sizeof(char), 4, f);

	//the number of color that are 'important' for the bitmap
    fread(arrayToRead, sizeof(char), 4, f);

	//skip to the image data
    fread(skipArray, sizeof(char), offset - BITMAPINFOHEADER_SIZE - BITMAPFILEHEADER_SIZE, f);

	*bitmapBits = (unsigned char*)malloc((*width)*(*height)*(BITS_PER_PIXEL/8)*sizeof(char));

	if (*bitmapBits==NULL)
	  {
	    fprintf(stderr, "ERROR - not enough memory\n");
		exit(EXIT_FAILURE);
	  }

	fread(*bitmapBits, sizeof(char), (*width)*(*height)*(BITS_PER_PIXEL/8), f);

	fclose(f);
  }

void cpuThreshold(unsigned char* bitmapBits, int width, int height, char* imageName)
  {
	FILE* out = fopen(imageName, "wb");
	int i;
	
    for (i = 0; i < width*height*3; i+=3)
      if ((unsigned int)bitmapBits[i] + (unsigned int)bitmapBits[i+1] + (unsigned int)bitmapBits[i+2] >= 128*3)
        {
    	  bitmapBits[i] = 255;
    	  bitmapBits[i+1] = 255;
    	  bitmapBits[i+2] = 255;
        }
      else
        {
    	  bitmapBits[i] = 0;
    	  bitmapBits[i+1] = 0;
    	  bitmapBits[i+2] = 0;
        }

    write_BITMAPFILEHEADER(out, width, height);

    write_BITMAPINFOHEADER(out, width, height);

    fwrite(bitmapBits, 1, width*height*3, out);

    fclose(out);
  }

int main(void)
{
	int width = 0, height = 0, cnt, i, dataSize;
	unsigned char* bitmapBits = NULL;
	double elapsed_time;
    FILE* out;

	extractPixels("lenny.bmp", &bitmapBits, &width, &height);

	startTime();
	cpuThreshold(bitmapBits, width, height, "resultCPU.bmp");
	stopTime(elapsed_time);
    printf("CPU     - %.2f\n", elapsed_time * 1e-3);

	int32_t* array1 = (int32_t*)malloc(width*height*sizeof(int));
	int32_t* array2 = (int32_t*)malloc(width*height*sizeof(int));
	int32_t* array3 = (int32_t*)malloc(width*height*sizeof(int));

	cnt = 0;

	for (i = 0; i < width*height*(BITS_PER_PIXEL/8); i++)
	  if (i%3 == 0)
	    array1[cnt] = (int32_t)bitmapBits[i];
	  else
	    if (i%3 == 1)
	      array2[cnt] = (int32_t)bitmapBits[i];
	    else
	      array3[cnt++] = (int32_t)bitmapBits[i];

	dataSize = width * height * sizeof(int);

	int32_t *outImage1 = malloc(dataSize);
	int32_t *outImage2 = malloc(dataSize);
	int32_t *outImage3 = malloc(dataSize);

	startTime();
	Threshold(width * height, array1, array2, array3, outImage1,outImage2, outImage3 );
	stopTime(elapsed_time);
	printf("MAXELER - %.2f\n", elapsed_time * 1e-3);

	cnt = 0;

	for (i = 0; i < width*height*(BITS_PER_PIXEL/8); i++)
      if (i%3 == 0)
	    bitmapBits[i] = (char)outImage1[cnt];
      else
	    if (i%3 == 1)
	      bitmapBits[i] = (char)outImage2[cnt];
		else
		  bitmapBits[i] = (char)outImage3[cnt++];

	out = fopen("resultMaxeler.bmp", "wb");

    write_BITMAPFILEHEADER(out, width, height);

	write_BITMAPINFOHEADER(out, width, height);

	fwrite(bitmapBits, 1, width*height*(BITS_PER_PIXEL/8), out);

	return 0;
}
