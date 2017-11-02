
//#include "stdafx.h"
#include "memory.h"
#include "time.h"

#include "omp.h"

#include <IL/ilut.h>
#include <IL/ilu.h>

#include "emmintrin.h"
#include "nmmintrin.h"

#include "defs.h"
#include "func.h"


void main()
{
	ilInit();
	iluInit();
	ILboolean ret;
	ILuint ilImg = 0;
	ilGenImages(1, &ilImg);
	ilBindImage(ilImg);
	ret = ilLoadImage((const char*)("input.jpg"));
	ILubyte* imgData = ilGetData(); 

	int imgWidth	= ilGetInteger(IL_IMAGE_WIDTH);
	int imgHeight	= ilGetInteger(IL_IMAGE_HEIGHT);
	ILint imgOrigin	= ilGetInteger(IL_ORIGIN_MODE);

	printf("Input resolution: %4dx%4d\n", imgWidth, imgHeight);

	unsigned char *img;
	int imgWidthF = imgWidth + FILTER_W - 1;
	int imgHeightF = imgHeight + FILTER_H - 1;
	int imgFOffsetW = (FILTER_W-1)/2;
	int imgFOffsetH = (FILTER_H-1)/2;
	img = (unsigned char *)(_aligned_malloc(4 * imgWidthF*imgHeightF*sizeof(unsigned char), 32));
	int row, col;

	for (row=0; row<imgHeightF; row++)
	{
		for (col=0; col<imgWidthF;col++)
		{
			int pixel = (row*imgWidthF + col)*4;
			img[pixel] = 0;
			img[pixel + 1] = 0;
			img[pixel + 2] = 0;
			img[pixel + 3] = 0;
		}
	}
	for (row = 0; row < imgHeight; row++){
		for (col = 0; col < imgWidth; col++){
			int base = ((row + imgFOffsetH) * imgWidthF + col + imgFOffsetW) * 4;
			img[base + 0] = imgData[(row * imgWidth + col) * 3];
			img[base + 1] = imgData[(row * imgWidth + col) * 3 + 1];
			img[base + 2] = imgData[(row * imgWidth + col) * 3 + 2];
			img[base + 3] = 0;
			//printf("%d, %d, %d, %d\r\n", img[base + 0], img[base + 1], img[base + 2], img[base + 3]);
		}
	}


// IMAGE PROCESSING
//---------------------------------------------------------------------------------------
	clock_t s0, e0;
	double d0;

	unsigned char *imgRes;
	imgRes = (unsigned char *)(_aligned_malloc(4 * imgWidthF * imgHeightF * sizeof(unsigned char), 32));

	double mpixel;

	s0 = clock();
	for (int r = 0; r < RUNS; r++)
	{
		printf("%d\r\n", r);
		median_filter_C(imgHeight, imgWidth, imgWidthF, imgFOffsetW, imgFOffsetH, img, imgRes);


		printf("%d\r\n", r);

	}

	e0 = clock();
	d0 = (double)(e0-s0)/(RUNS*CLOCKS_PER_SEC);
	mpixel = (imgWidth*imgHeight/d0)/1000000;
	printf("C CPU TIME: %4.4f\n", d0);
	printf("C Mpixel/s: %4.4f\n", mpixel);
	getchar();


	unsigned char *imgWrite;
	imgWrite = imgRes;

//---------------------------------------------------------------------------------------
// IMAGE PROCESSING END


	for (row=0; row<imgHeight; row++)
	{
		for (col=0; col<imgWidth;col++)
		{
			int pixel_src = (row*imgWidth + col)*4;
			int pixel_dst = (row*imgWidth + col)*3;
			*(imgData+pixel_dst+0) = (ILubyte)(*(imgWrite+pixel_src+0));
			*(imgData+pixel_dst+1) = (ILubyte)(*(imgWrite+pixel_src+1));
			*(imgData+pixel_dst+2) = (ILubyte)(*(imgWrite+pixel_src+2));
		}
	}


	_aligned_free(img);
	_aligned_free(imgRes);

	ret = ilSetData(imgData);
	ilEnable(IL_FILE_OVERWRITE);
	ilSaveImage((const char*)("output.jpg"));
	ilDeleteImages(1, &ilImg);


}
