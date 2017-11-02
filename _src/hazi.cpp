#include "omp.h"
#include <stdio.h>
#include "emmintrin.h"
#include "nmmintrin.h"
#include "defs.h" 

int partition(unsigned char* nums, signed char* pos, int left, int right){
	unsigned char tmp;
	signed char tmp_pos;
	unsigned char pivot = nums[right];
	int i, j;

#define SWAP(a, b) { tmp = nums[a]; nums[a] = nums[b]; nums[b] = tmp; tmp_pos = pos[a]; pos[a]= pos[b]; pos[b] = tmp_pos; }

	for (j = left, i = left; j < right; j++){
		if (nums[j] <= pivot){
			SWAP(i, j);
			i++;
		}
	}
	SWAP(i, right);
	return i;
}


void quicksort(unsigned char *nums, signed char* pos, int left, int right){
	int p;	//p is position of pivot in the partitioned array
	
	if (left < right){
		p = partition(nums, pos, left, right);
		quicksort(nums, pos, left, p - 1);
		quicksort(nums, pos, p + 1, right);
	}
	return;
}

void bubblesort(unsigned char *nums, signed char* pos, int n){
	unsigned char tmp;
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

/*
int quickselect(char *nums, char* pos, int left, int right, int k){
	int p = partition(nums, new_p, left, right);	//p is position of pivot in the partitioned array
	int r;
	
	//k equals pivot got lucky
	if (p == k-1){
		r = nums[p];
	}
	//k less than pivot
	else if (k - 1 < p){
		r = quickselect(nums, new_p, left, p - 1, k);
	}
	//k greater than pivot
	else{
		r = quickselect(nums, new_p, p + 1, right, k);
	}
	pos = new_p;
	return r;
}
*/

//Merges A+C into B
void sort_and_merge(unsigned char* a, unsigned  char* b, unsigned char* c, signed char* a_pos, signed char* b_pos, signed char* c_pos){
	int i, j, k;
	//quicksort(c, c_pos, 0, FILTER_W - 1);
	bubblesort(c, c_pos, FILTER_W);
	/*
	for (i = 0; i < 25; i++) printf("%d, ", a[i]);
	printf("\r\n");
	for (i = 0; i < 25; i++) printf("%d, ", a_pos[i]);
	printf("\r\n");
	for (i = 0; i < 5; i++) printf("%d, ", c[i]);
	printf("\r\n");
	for (i = 0; i < 5; i++) printf("%d, ", c_pos[i]);
	printf("\r\n");
	*/
	i = 0;
	j = 0;
	k = 0;
	do{
		while (a_pos[i + k] < 1 && k<6){
			//printf("a_pos[i(%d) + k(%d)]: %d \r\n", i, k, a_pos[i + k]);
			k++;
		}
		//printf("a[i(%d)+k(%d)]: %d, c[j(%d)]: %d \r\n", i, k, a[i + k], j, c[j]);
		if (((a[i + k] < c[j]) || (j == FILTER_W)) && (k<6)) {
			b[i + j] = a[i + k];
			b_pos[i + j] = a_pos[i + k];
			//printf("b[i(%d)+j(%d)]: %d, a[i(%d)+k(%d)]: %d \r\n", i, j, b[i + j], i, k, a[i + k]);
			//printf("b_pos[i(%d)+j(%d)]: %d, a_pos[i(%d)+k(%d)]: %d \r\n", i, j, b_pos[i + j], i, k, a_pos[i + k]);
			i++;
		}
		else {
			b[i + j] = c[j];
			b_pos[i + j] = c_pos[j];
			//printf("b[i(%d)+j(%d)]: %d, c[j(%d)]: %d \r\n", i, j, b[i + j], j, c[j]);
			//printf("b_pos[i(%d)+j(%d)]: %d, c_pos[j(%d)]: %d \r\n", i, j, b_pos[i + j], j, c_pos[j]);
			j++;
		}
	} while ((i + j) < FILTER_W * (FILTER_H - 0));

	/*
	for(i = 0,j = 0, k = 0; i + j < FILTER_W * (FILTER_H); i++){
		while ((a_pos[i+k] < 1) && (k<6)) { k++;}
		if (a[i + k] <= c[j]) {
			b[i] = a[i+k];
			b_pos[i] = a_pos[i+k];
			printf("i+k: %d\r\n", i + k);
		}
		else {
			b[i] = c[j];
			b_pos[i] = c_pos[j];
			j++;
			i--;
			printf("j:%d\r\n", j);
		}
	}
	*/
	/*
	for (i = 0; i < 25; i++) printf("%d, ", b[i]);
		printf("\r\n");
	for (i = 0; i < 25; i++) printf("%d, ", b_pos[i]);
		printf("\r\n");
	for (i = 0, j = 0; i < 25; i++)
		j += b_pos[i];
	if (j != 325){
		printf("_________________________");
		for (j = 0; j < 5; j++){
			printf("%d, ", c[j]);
			printf("%d, ", c_pos[j]);
		}
		printf("\r\n");
		j = 0;
	}
	*/
	/*
	for (i = 1; i < 25; i++)
	if (b[i] < b[i - 1]){
		printf("ERROR");
		while (1);
	}
	*/

}


void median_filter_C(int imgHeight, int imgWidth, int imgWidthF, int imgFOffsetW, int imgFOffsetH, unsigned  char* imgSrc, unsigned char* imgDst){

	int X = 0, Y = 0;
	int i, j;

	//Tables for filter windows
	unsigned char r_a[FILTER_H * FILTER_W] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	unsigned char r_b[FILTER_H * FILTER_W] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	unsigned char g_a[FILTER_H * FILTER_W] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	unsigned char g_b[FILTER_H * FILTER_W] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	unsigned char b_a[FILTER_H * FILTER_W] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	unsigned char b_b[FILTER_H * FILTER_W] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	signed char r_a_p[FILTER_H * FILTER_W] = { 5, 5, 5, 5, 5, 26, 21, 16, 11, 6, 27, 22, 17, 12, 7, 28, 23, 18, 13, 8, 29, 24, 19, 14, 9 };//{1,6,11,16,21, 2,7,12,17,22, 3,8,13,18,23, 4,9,14,19,24, 5,10,15,20,25};
	signed char r_b_p[FILTER_H * FILTER_W] = { 5, 5, 5, 5, 5, 26, 21, 16, 11, 6, 27, 22, 17, 12, 7, 28, 23, 18, 13, 8, 29, 24, 19, 14, 9 };
	signed char g_a_p[FILTER_H * FILTER_W] = { 5, 5, 5, 5, 5, 26, 21, 16, 11, 6, 27, 22, 17, 12, 7, 28, 23, 18, 13, 8, 29, 24, 19, 14, 9 };
	signed char g_b_p[FILTER_H * FILTER_W] = { 5, 5, 5, 5, 5, 26, 21, 16, 11, 6, 27, 22, 17, 12, 7, 28, 23, 18, 13, 8, 29, 24, 19, 14, 9 };
	signed char b_a_p[FILTER_H * FILTER_W] = { 5, 5, 5, 5, 5, 26, 21, 16, 11, 6, 27, 22, 17, 12, 7, 28, 23, 18, 13, 8, 29, 24, 19, 14, 9 };
	signed char b_b_p[FILTER_H * FILTER_W] = { 5, 5, 5, 5, 5, 26, 21, 16, 11, 6, 27, 22, 17, 12, 7, 28, 23, 18, 13, 8, 29, 24, 19, 14, 9 };
	unsigned char r_new[FILTER_H];
	unsigned char g_new[FILTER_H];
	unsigned char b_new[FILTER_H];
	signed char r_new_p[FILTER_H] = { 29, 24, 19, 14, 9 }; //{21,22,23,24,25};
	signed char g_new_p[FILTER_H] = { 29, 24, 19, 14, 9 };
	signed char b_new_p[FILTER_H] = { 29, 24, 19, 14, 9 };
	unsigned int base, base_out;

	signed char lut_fw[26] = { 0, 0, 21, 22, 23, 24, 0, 16, 17, 18, 19, 0, 11, 12, 13, 14, 0, 6, 7, 8, 9, 0, 1, 2, 3, 4 };
	/*
	for (i = 0; i < imgHeight; i++){
		for (j = 0; j < imgWidth; j++){
			int base = ((i + imgFOffsetH) * imgWidthF + j + imgFOffsetW) * 4;
			imgDst[(i * imgWidth + j) * 4] = imgSrc[base + 0];
			imgDst[(i * imgWidth + j) * 4 + 1] = imgSrc[base + 1];
			imgDst[(i * imgWidth + j) * 4 + 2] = imgSrc[base + 2];
		}
	}
	return;
	*/
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

	while (Y < imgHeight){
		//printf("\r\nX - Y: %d - %d\r\n", X, Y);
		//Vertical movement, left side

		base = (imgWidthF * (Y + FILTER_H - 1)) * 4;	//Setting up base addresses
		base_out = Y * imgWidth * 4;
		//printf("B: %d\r\nBO: %d\r\n", base, base_out);

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
		//printf("B: %d\r\nBO: %d\r\n", base, base_out);
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

			//printf("\r\nX - Y: %d - %d\r\n", X, Y);
			//Updating base addresses
			base += 4;
			base_out += 4;
			//printf("B: %d\r\nBO: %d\r\n", base, base_out);
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
			/*
			for (i = 0; i < 5; i++) {
				printf("%d, ", r_new[i]); printf("%d, ", g_new[i]); printf("%d, ", b_new[i]);
				printf("\r\n");
			}
			*/
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
			//printf("B: %d\r\nBO: %d\r\n", base, base_out);

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
				//printf("%d\r\n", base + i * imgWidthF * 4);
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
		Y++;
		//Vertical movement, right side

		//printf("\r\nX - Y: %d - %d\r\n", X, Y);
		base = (imgWidthF * (Y + FILTER_H) - FILTER_W) * 4;	//Setting up base addresses
		base_out = ((Y + 1) * imgWidth - 1) * 4;
		//printf("B: %d\r\nBO: %d\r\n", base, base_out);

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
			//printf("%d\r\n", base + i * 4);
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
		//printf("B: %d\r\nBO: %d\r\n", base, base_out);

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
			//printf("%d\r\n", base + i * imgWidthF * 4);
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
			//printf("\r\nX - Y: %d - %d\r\n", X, Y);
			//Updating base addresses
			base -= 4;
			base_out -= 4;
			//printf("B: %d\r\nBO: %d\r\n", base, base_out);

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
				//printf("%d\r\n", base + i * imgWidthF * 4);
				g_new[i] = imgSrc[base + i * imgWidthF * 4 + 1];
				b_new[i] = imgSrc[base + i * imgWidthF * 4 + 2];
			}
			/*
			for (i = 0; i < 5; i++) {
			printf("%d, ", r_new[i]); printf("%d, ", g_new[i]); printf("%d, ", b_new[i]);
			printf("\r\n");
			}
			*/
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
			//printf("B: %d\r\nBO: %d\r\n", base, base_out);

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
		Y++;
		//printf("%d\r\n", Y);
	}
	return;
}
