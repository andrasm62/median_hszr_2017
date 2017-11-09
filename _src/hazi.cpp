#include "omp.h"
#include <stdio.h>
#include "emmintrin.h"
#include "nmmintrin.h"
#include "defs.h" 
#include "limits.h"


int partition(float* nums, signed char* pos, int left, int right){
	float tmp;
	signed char tmp_pos;
	float pivot = nums[right];
	int i, x;

	#define SWAP(a, b) { tmp = nums[a]; nums[a] = nums[b]; nums[b] = tmp; tmp_pos = pos[a]; pos[a] = pos[b]; pos[b] = tmp_pos; }

	for (x = left, i = left; x < right; x++){
		if (nums[x] <= pivot){
			SWAP(i, x);
			i++;
		}
	}
	SWAP(i, right);
	return i;
}

void quicksort(float *nums, signed char* pos, int left, int right){
	int p;	//p is position of pivot in the partitioned array
	//printf("%d-%d\n", left, right);
	if (left < right){
		p = partition(nums, pos, left, right);
		quicksort(nums, pos, left, p - 1);
		quicksort(nums, pos, p + 1, right);
	}
	return;
}

void bubblesort(float *nums, signed char* pos, int n){
	float tmp;
	signed char tmp_pos;
	int i, j;
#define SWAP(a, b) { tmp = nums[a]; nums[a] = nums[b]; nums[b] = tmp; tmp_pos = pos[a]; pos[a]= pos[b]; pos[b] = tmp_pos; }

	for (i = 0; i < (n - 1); i++){
		for (j = 0; j < n - i - 1; j++){
			if (nums[j] > nums[j + 1]){
				SWAP(j, j + 1);
			}
		}
	}
}

//Merges A+C into B
void sort_and_merge(float* a, float* b, float* c, signed char* a_pos, signed char* b_pos, signed char* c_pos){
	int i, j, k;
	quicksort(c, c_pos, 0, FILTER_W - 1);
	//bubblesort(c, c_pos, FILTER_W);
	i = 0;
	j = 0;
	k = 0;
	do{
		while (a_pos[i + k] < 1 && k<6){
			k++;
		}
		if (((a[i + k] < c[j]) || (j == FILTER_W)) && (k<6)) {
			b[i + j] = a[i + k];
			b_pos[i + j] = a_pos[i + k];
			i++;
		}
		else {
			b[i + j] = c[j];
			b_pos[i + j] = c_pos[j];
			j++;
		}
	} while ((i + j) < FILTER_W * (FILTER_H - 0));
}

void median_filter_C(int imgHeight, int imgWidth, int imgWidthF, int imgFOffsetW, int imgFOffsetH, float* imgSrc, float* imgDst){

	//Tables for filter windows
	float r_a[FILTER_H * FILTER_W] = { 0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0};
	float r_b[FILTER_H * FILTER_W] = { 0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0};
	float g_a[FILTER_H * FILTER_W] = { 0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0};
	float g_b[FILTER_H * FILTER_W] = { 0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0};
	float b_a[FILTER_H * FILTER_W] = { 0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0};
	float b_b[FILTER_H * FILTER_W] = { 0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0};
	signed char r_a_p[FILTER_H * FILTER_W] = { 5, 5, 5, 5, 5, 26, 21, 16, 11, 6, 27, 22, 17, 12, 7, 28, 23, 18, 13, 8, 29, 24, 19, 14, 9 };//{1,6,11,16,21, 2,7,12,17,22, 3,8,13,18,23, 4,9,14,19,24, 5,10,15,20,25};
	signed char r_b_p[FILTER_H * FILTER_W] = { 5, 5, 5, 5, 5, 26, 21, 16, 11, 6, 27, 22, 17, 12, 7, 28, 23, 18, 13, 8, 29, 24, 19, 14, 9 };
	signed char g_a_p[FILTER_H * FILTER_W] = { 5, 5, 5, 5, 5, 26, 21, 16, 11, 6, 27, 22, 17, 12, 7, 28, 23, 18, 13, 8, 29, 24, 19, 14, 9 };
	signed char g_b_p[FILTER_H * FILTER_W] = { 5, 5, 5, 5, 5, 26, 21, 16, 11, 6, 27, 22, 17, 12, 7, 28, 23, 18, 13, 8, 29, 24, 19, 14, 9 };
	signed char b_a_p[FILTER_H * FILTER_W] = { 5, 5, 5, 5, 5, 26, 21, 16, 11, 6, 27, 22, 17, 12, 7, 28, 23, 18, 13, 8, 29, 24, 19, 14, 9 };
	signed char b_b_p[FILTER_H * FILTER_W] = { 5, 5, 5, 5, 5, 26, 21, 16, 11, 6, 27, 22, 17, 12, 7, 28, 23, 18, 13, 8, 29, 24, 19, 14, 9 };
	float r_new[FILTER_H];
	float g_new[FILTER_H];
	float b_new[FILTER_H];
	signed char r_new_p[FILTER_H] = { 29, 24, 19, 14, 9 }; //{21,22,23,24,25};
	signed char g_new_p[FILTER_H] = { 29, 24, 19, 14, 9 };
	signed char b_new_p[FILTER_H] = { 29, 24, 19, 14, 9 };

	unsigned int base, base_out;
	int X = 0, Y = 0;
	int i, j;
	signed char lut_fw[26] = { 0, 0, 21, 22, 23, 24, 0, 16, 17, 18, 19, 0, 11, 12, 13, 14, 0, 6, 7, 8, 9, 0, 1, 2, 3, 4 };

	//[-2;0] pixel, Output stored in B

	for (i = 0; i < FILTER_W; i++){
		r_new[i] = imgSrc[(i + imgWidthF * imgFOffsetH) * 4];
		g_new[i] = imgSrc[(i + imgWidthF * imgFOffsetH) * 4 + 1];
		b_new[i] = imgSrc[(i + imgWidthF * imgFOffsetH) * 4 + 2];
	}
	quicksort(r_new, r_new_p, 0, FILTER_W - 1);
	quicksort(g_new, g_new_p, 0, FILTER_W - 1);
	quicksort(b_new, b_new_p, 0, FILTER_W - 1);
	//bubblesort(r_new, r_new_p, FILTER_W);
	//bubblesort(g_new, g_new_p, FILTER_W);
	//bubblesort(b_new, b_new_p, FILTER_W);

	for (i = (FILTER_H - 1) * FILTER_W, j = 0; i < FILTER_H * FILTER_W; i++, j++){
		r_b[i] = r_new[j];
		g_b[i] = g_new[j];
		b_b[i] = b_new[j];
		r_b_p[i] = r_new_p[j];
		g_b_p[i] = g_new_p[j];
		b_b_p[i] = b_new_p[j];
	}

	//[0;-1] pixel, Output stored in A

	//Updating B positions
	for (i = 0; i < FILTER_H * FILTER_W; i++){
		r_b_p[i] = r_b_p[i] - 5;
		g_b_p[i] = g_b_p[i] - 5;
		b_b_p[i] = b_b_p[i] - 5;
	}

	//Reading new values from the original image and getting new positions
	for (i = 0; i < FILTER_W; i++){
		r_new[i] = imgSrc[(i * imgWidthF + imgFOffsetW - 1) * 4];
		g_new[i] = imgSrc[(i * imgWidthF + imgFOffsetW - 1) * 4 + 1];
		b_new[i] = imgSrc[(i * imgWidthF + imgFOffsetW - 1) * 4 + 2];
		r_new_p[i] = 25 - i * 5;
		g_new_p[i] = 25 - i * 5;
		b_new_p[i] = 25 - i * 5;
	}

	//Sorting new values and merging the 2 tables
	sort_and_merge(r_b, r_a, r_new, r_b_p, r_a_p, r_new_p);
	sort_and_merge(g_b, g_a, g_new, g_b_p, g_a_p, g_new_p);
	sort_and_merge(b_b, b_a, b_new, b_b_p, b_a_p, b_new_p);

//---------------
//Image processing
//---------------

	while(Y < imgHeight){

		//Vertical movement, left side

		base = (imgWidthF * (Y + FILTER_H - 1)) * 4;	//Setting up base addresses
		base_out = Y * imgWidth * 4;

		//Updating A positions
		for (i = 0; i < FILTER_H * FILTER_W; i++){
			r_a_p[i] = lut_fw[r_a_p[i]];
			g_a_p[i] = lut_fw[g_a_p[i]];
			b_a_p[i] = lut_fw[b_a_p[i]];
		}
		for (i = 0; i < FILTER_W; i++){
			r_new_p[i] = (i + 1) * 5;
			g_new_p[i] = (i + 1) * 5;
			b_new_p[i] = (i + 1) * 5;
		}

		//Reading new values from the original image
		for (i = 0; i < FILTER_W; i++){
			r_new[i] = imgSrc[base + i];
			g_new[i] = imgSrc[base + i + 1];
			b_new[i] = imgSrc[base + i + 2];
		}

		//Sorting new values and merging the 2 tables
		//Output in B
		sort_and_merge(r_a, r_b, r_new, r_a_p, r_b_p, r_new_p);
		sort_and_merge(g_a, g_b, g_new, g_a_p, g_b_p, g_new_p);
		sort_and_merge(b_a, b_b, b_new, b_a_p, b_b_p, b_new_p);

		//Writing out calculated pixel
		imgDst[base_out + 0] = r_b[12];
		imgDst[base_out + 1] = g_b[12];
		imgDst[base_out + 2] = b_b[12];

		base = (Y * imgWidthF + FILTER_W) * 4;	//Setting up base addresses
		base_out += 4;
		//Updating B positions
		for (i = 0; i < FILTER_H * FILTER_W; i++){
			r_b_p[i] = r_b_p[i] - 5;
			g_b_p[i] = g_b_p[i] - 5;
			b_b_p[i] = b_b_p[i] - 5;
		}

		//Reading new values from the original image and getting new positions
		for (i = 0; i < FILTER_H; i++){
			r_new_p[i] = FILTER_H * (FILTER_W - 1) + i + 1;
			g_new_p[i] = FILTER_H * (FILTER_W - 1) + i + 1;
			b_new_p[i] = FILTER_H * (FILTER_W - 1) + i + 1;

			r_new[i] = imgSrc[base + i * imgWidthF * 4];
			g_new[i] = imgSrc[base + i * imgWidthF * 4 + 1];
			b_new[i] = imgSrc[base + i * imgWidthF * 4 + 2];
		}

		//Sorting new values and merging the 2 tables
		//Output in A
		sort_and_merge(r_b, r_a, r_new, r_b_p, r_a_p, r_new_p);
		sort_and_merge(g_b, g_a, g_new, g_b_p, g_a_p, g_new_p);
		sort_and_merge(b_b, b_a, b_new, b_b_p, b_a_p, b_new_p);

		//Writing out calculated pixel
		imgDst[base_out + 0] = r_a[12];
		imgDst[base_out + 1] = g_a[12];
		imgDst[base_out + 2] = b_a[12];

		//Horizontal part, forward
		for (X = 2; X < imgWidth; X += 2){

			//Updating base addresses
			base += 4;
			base_out += 4;
			//Updating A positions
			for (i = 0; i < FILTER_H * FILTER_W; i++){
				r_a_p[i] = r_a_p[i] - 5;
				g_a_p[i] = g_a_p[i] - 5;
				b_a_p[i] = b_a_p[i] - 5;
			}

			//Reading new values from the original image and getting new positions
			for (i = 0; i < FILTER_H; i++){
				r_new_p[i] = FILTER_H * (FILTER_W - 1) + i + 1;
				g_new_p[i] = FILTER_H * (FILTER_W - 1) + i + 1;
				b_new_p[i] = FILTER_H * (FILTER_W - 1) + i + 1;

				r_new[i] = imgSrc[base + i * imgWidthF * 4];
				g_new[i] = imgSrc[base + i * imgWidthF * 4 + 1];
				b_new[i] = imgSrc[base + i * imgWidthF * 4 + 2];
			}

			//Sorting new values and merging the 2 tables
			//First from A+C->B then B+C->A
			sort_and_merge(r_a, r_b, r_new, r_a_p, r_b_p, r_new_p);
			sort_and_merge(g_a, g_b, g_new, g_a_p, g_b_p, g_new_p);
			sort_and_merge(b_a, b_b, b_new, b_a_p, b_b_p, b_new_p);

			//Writing out calculated pixel
			imgDst[base_out + 0] = r_b[12];
			imgDst[base_out + 1] = g_b[12];
			imgDst[base_out + 2] = b_b[12];

			//Updating base addresses
			base += 4;
			base_out += 4;

			//Updating B positions
			for (i = 0; i < FILTER_H * FILTER_W; i++){
				r_b_p[i] = r_b_p[i] - 5;
				g_b_p[i] = g_b_p[i] - 5;
				b_b_p[i] = b_b_p[i] - 5;
			}

			//Reading new values from the original image and getting new positions
			for (i = 0; i < FILTER_H; i++){
				r_new_p[i] = FILTER_H * (FILTER_W - 1) + i + 1;
				g_new_p[i] = FILTER_H * (FILTER_W - 1) + i + 1;
				b_new_p[i] = FILTER_H * (FILTER_W - 1) + i + 1;

				r_new[i] = imgSrc[base + i * imgWidthF * 4];
				g_new[i] = imgSrc[base + i * imgWidthF * 4 + 1];
				b_new[i] = imgSrc[base + i * imgWidthF * 4 + 2];
			}

			//Sorting new values and merging the 2 tables
			sort_and_merge(r_b, r_a, r_new, r_b_p, r_a_p, r_new_p);
			sort_and_merge(g_b, g_a, g_new, g_b_p, g_a_p, g_new_p);
			sort_and_merge(b_b, b_a, b_new, b_b_p, b_a_p, b_new_p);

			//Writing out calculated pixel
			imgDst[base_out + 0] = r_a[12];
			imgDst[base_out + 1] = g_a[12];
			imgDst[base_out + 2] = b_a[12];
		}
		Y = Y + 1;

		//Vertical movement, right side

		base = (imgWidthF * (Y + FILTER_H) - FILTER_W) * 4;	//Setting up base addresses
		base_out = ((Y + 1) * imgWidth - 1) * 4;

		//Updating A positions
		for (i = 0; i < FILTER_H * FILTER_W; i++){
			r_a_p[i] = lut_fw[r_a_p[i]];
			g_a_p[i] = lut_fw[g_a_p[i]];
			b_a_p[i] = lut_fw[b_a_p[i]];
		}
		for (i = 0; i < FILTER_W; i++){
			r_new_p[i] = 25 - i * 5;
			g_new_p[i] = 25 - i * 5;
			b_new_p[i] = 25 - i * 5;
		}

		//Reading new values from the original image
		for (i = 0; i < FILTER_W; i++){
			r_new[i] = imgSrc[base + i * 4];
			g_new[i] = imgSrc[base + i * 4 + 1];
			b_new[i] = imgSrc[base + i * 4 + 2];
		}

		//Sorting new values and merging the 2 tables
		//Output in B
		sort_and_merge(r_a, r_b, r_new, r_a_p, r_b_p, r_new_p);
		sort_and_merge(g_a, g_b, g_new, g_a_p, g_b_p, g_new_p);
		sort_and_merge(b_a, b_b, b_new, b_a_p, b_b_p, b_new_p);

		//Writing out calculated pixel
		imgDst[base_out + 0] = r_b[12];
		imgDst[base_out + 1] = g_b[12];
		imgDst[base_out + 2] = b_b[12];

		base = ((Y + 1) * imgWidthF - FILTER_W - 1) * 4;	//Setting up base addresses
		base_out -= 4;

		//Updating B positions
		for (i = 0; i < FILTER_H * FILTER_W; i++){
			r_b_p[i] = r_b_p[i] - 5;
			g_b_p[i] = g_b_p[i] - 5;
			b_b_p[i] = b_b_p[i] - 5;
		}

		//Reading new values from the original image and getting new positions
		for (i = 0; i < FILTER_W; i++){
			r_new_p[i] = FILTER_H * (FILTER_W - 1) + i + 1;
			g_new_p[i] = FILTER_H * (FILTER_W - 1) + i + 1;
			b_new_p[i] = FILTER_H * (FILTER_W - 1) + i + 1;

			r_new[i] = imgSrc[base + i * imgWidthF * 4];
			g_new[i] = imgSrc[base + i * imgWidthF * 4 + 1];
			b_new[i] = imgSrc[base + i * imgWidthF * 4 + 2];
		}

		//Sorting and merging the 2 tables
		sort_and_merge(r_b, r_a, r_new, r_b_p, r_a_p, r_new_p);
		sort_and_merge(g_b, g_a, g_new, g_b_p, g_a_p, g_new_p);
		sort_and_merge(b_b, b_a, b_new, b_b_p, b_a_p, b_new_p);

		//Writing out calculated pixel
		imgDst[base_out + 0] = r_a[12];
		imgDst[base_out + 1] = g_a[12];
		imgDst[base_out + 2] = b_a[12];

		//Horizontal part, backward
		for (X = imgWidth - 3; X > 0; X -= 2){
			//Updating base addresses
			base -= 4;
			base_out -= 4;

			//Updating A positions
			for (i = 0; i < FILTER_H * FILTER_W; i++){
				r_a_p[i] = r_a_p[i] - 5;
				g_a_p[i] = g_a_p[i] - 5;
				b_a_p[i] = b_a_p[i] - 5;
			}

			//Reading new values from the original image and getting new positions
			for (i = 0; i < FILTER_W; i++){
				r_new_p[i] = FILTER_H * (FILTER_W - 1) + i + 1;
				g_new_p[i] = FILTER_H * (FILTER_W - 1) + i + 1;
				b_new_p[i] = FILTER_H * (FILTER_W - 1) + i + 1;

				r_new[i] = imgSrc[base + i * imgWidthF * 4];
				g_new[i] = imgSrc[base + i * imgWidthF * 4 + 1];
				b_new[i] = imgSrc[base + i * imgWidthF * 4 + 2];
			}

			//Sorting and merging the 2 tables
			//First from A+C->B then B+C->A
			sort_and_merge(r_a, r_b, r_new, r_a_p, r_b_p, r_new_p);
			sort_and_merge(g_a, g_b, g_new, g_a_p, g_b_p, g_new_p);
			sort_and_merge(b_a, b_b, b_new, b_a_p, b_b_p, b_new_p);

			//Writing out calculated pixel
			imgDst[base_out + 0] = r_b[12];
			imgDst[base_out + 1] = g_b[12];
			imgDst[base_out + 2] = b_b[12];

			//Updating base addresses
			base -= 4;
			base_out -= 4;

			//Updating B positions
			for (i = 0; i < FILTER_H * FILTER_W; i++){
				r_b_p[i] = r_b_p[i] - 5;
				g_b_p[i] = g_b_p[i] - 5;
				b_b_p[i] = b_b_p[i] - 5;
			}

			//Reading new values from the original image and getting new positions
			for (i = 0; i < FILTER_W; i++){
				r_new_p[i] = FILTER_H * (FILTER_W - 1) + i + 1;
				g_new_p[i] = FILTER_H * (FILTER_W - 1) + i + 1;
				b_new_p[i] = FILTER_H * (FILTER_W - 1) + i + 1;

				r_new[i] = imgSrc[base + i * imgWidthF * 4];
				g_new[i] = imgSrc[base + i * imgWidthF * 4 + 1];
				b_new[i] = imgSrc[base + i * imgWidthF * 4 + 2];
			}

			//Sorting and merging the 2 tables
			sort_and_merge(r_b, r_a, r_new, r_b_p, r_a_p, r_new_p);
			sort_and_merge(g_b, g_a, g_new, g_b_p, g_a_p, g_new_p);
			sort_and_merge(b_b, b_a, b_new, b_b_p, b_a_p, b_new_p);

			//Writing out calculated pixel
			imgDst[base_out + 0] = r_a[12];
			imgDst[base_out + 1] = g_a[12];
			imgDst[base_out + 2] = b_a[12];
		}
		Y = Y + 1;
	}
	return;
}
