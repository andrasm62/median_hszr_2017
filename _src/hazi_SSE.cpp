#include "omp.h"
#include <stdio.h>
#include "emmintrin.h"
#include "nmmintrin.h"
#include "defs.h" 
#include "limits.h"

__m128 one;


__m128 insert_and_get_median(__m128 new_val, __m128* val, __m128* pos, int* index, int dir){ //Esetleg legyen pointer a new_val???
	__m128 median = _mm_set_ps(0.0, 0.0, 0.0, 0.0);
	__m128 temp_pos = _mm_set_ps(0.0, 0.0, 0.0, 0.0);
	__m128 old_val;
	__m128 med_of_filter_width = _mm_set_ps((FILTER_W * FILTER_H - 1) / 2, (FILTER_W * FILTER_H - 1) / 2, (FILTER_W * FILTER_H - 1) / 2, (FILTER_W * FILTER_H - 1) / 2);
	int i;

	//If we go forward on the picture, we need to move the filter window forward as a circular puffer.
	if (dir == 1){
		(*index)++;
		if ((*index) == (FILTER_W * FILTER_H))
		(*index) = 0;
	}
	//The other way we need to go backward on the circular puffer
	else {
		(*index)--;
		if ((*index) == -1)
		(*index) = FILTER_W * FILTER_H - 1;
	}

	old_val = val[*index]; //Saving the oldest value
	val[*index] = _mm_set_ps(256.0, 256.0, 256.0, 256.0);	//Setting the oldest value MAX + 1
	
	//Filtering
	for (i = 0; i < (FILTER_W * FILTER_H); i++){
		//If (ACT > OLD) OR ((ACT == OLD) AND (POS_ACT > POS_OLD)), we need to decrease the postion with one 
		pos[i] =_mm_sub_ps(pos[i],_mm_and_ps(_mm_or_ps(_mm_and_ps(_mm_cmpeq_ps(val[i], old_val),_mm_cmpgt_ps(pos[i], pos[*index])),_mm_cmpgt_ps(val[i], old_val)),one));
		
		//If ACT >= NEW, we need to increase the position with one
		pos[i] = _mm_add_ps(pos[i], _mm_and_ps(_mm_cmpge_ps(val[i], new_val), one));
		
		//If NEW > ACT, we need to increase the POS_NEW with one
		temp_pos = _mm_add_ps(temp_pos, _mm_and_ps(_mm_cmpgt_ps(new_val, val[i]), one));
		
		//If the POS_ACT value equals to the median position, we have found the median value. 
		median = _mm_add_ps(median, _mm_and_ps(_mm_cmpeq_ps(pos[i], med_of_filter_width), val[i]));
	}
	//Save the NEW and POS_NEW values
	val[*index] = new_val;
	pos[*index] = temp_pos;
	//If the new value is the median, then set it as the median
	median = _mm_add_ps(median, _mm_and_ps(_mm_cmpeq_ps(temp_pos, med_of_filter_width), new_val));

	return median;
}


void median_filter_SSE(int imgHeight, int imgWidth, int imgWidthF, int imgFOffsetW, int imgFOffsetH, float* imgSrc, float* imgDst){

	__m128 new_val, median, tmp_1, tmp_2;
	__m128 val[FILTER_H * FILTER_W];
	__m128 pos[FILTER_H * FILTER_W];
	int index;
	int X = 0, Y = 0;
	int i, j, k;
	unsigned int base, base_out;

	one = _mm_set_ps(1.0, 1.0, 1.0, 1.0);	//Globally used constant

	//Initialising starting values and positions
	for (i = 0; i < FILTER_W * FILTER_H; i++){
		val[i] = _mm_set_ps(0.0, 0.0, 0.0, 0.0);
		pos[i] = _mm_set_ps(i + 0.0, i + 0.0, i + 0.0, i + 0.0);
	}

#pragma omp parallel for num_threads(NUM_OF_THREADS) private(X, Y, i, j, k, base, base_out, new_val, median, tmp_1, tmp_2, val, pos, index)
	for (int part = 0; part < NUM_OF_THREADS; part++){
		Y = part * imgHeight / NUM_OF_THREADS;//Each thread works on a different part of the image

		//Initialising kernel window for start
		for (k = 0; k < FILTER_H - 1; k++){
			for (i = imgFOffsetW; i < FILTER_W; i++){//There are imgFOffsetW 0 values on the edge of the input picture
				for (j = 0; j < 4; j++)
					new_val.m128_f32[j] = imgSrc[((Y + k) * imgWidthF + i) * 4 + j];
				index = (i * FILTER_H) + k;
				insert_and_get_median(new_val, val, pos, &index, 1);
			}
		}
//---------------
//Image processing
//---------------

		while (Y < (part + 1) * imgHeight / NUM_OF_THREADS){

			//Vertical movement, left side

			//Reset Index value. We need to sub 1 from the value given at the bottom, but in normal operation it is equivalent to set it to this value
			index = FILTER_H * FILTER_W - 1;

			base = (imgWidthF * (Y + FILTER_H - 1)) * 4;	//Setting up base addresses
			base_out = Y * imgWidth * 4;

			//Rotating values for stepping down with the filter window
			tmp_1 = val[0];
			tmp_2 = pos[0];
			for (i = 0; i < (FILTER_H * FILTER_W - 1); i++){
				val[i] = val[i + 1];
				pos[i] = pos[i + 1];
			}
			val[FILTER_H * FILTER_W - 1] = tmp_1;
			pos[FILTER_H * FILTER_W - 1] = tmp_2;

			for (i = 0; i < FILTER_H; i++){
				for (j = 0; j < 4; j++)
					new_val.m128_f32[j] = imgSrc[base + i * 4 + j];		//Reading new values
				index = (index + FILTER_H) % (FILTER_H * FILTER_W) - 1;	//At stepping down, we need an extra index value calculation
				median = insert_and_get_median(new_val, val, pos, &index, 1);	//Get median value of the new window.
			}

			imgDst[base_out + 0] = median.m128_f32[0];	//Writing out median values
			imgDst[base_out + 1] = median.m128_f32[1];
			imgDst[base_out + 2] = median.m128_f32[2];

			base = (imgWidthF * Y + FILTER_W - 1) * 4;	//Updating base addresses

			//Horizontal part, forward
			for (X = 1; X < imgWidth; X++){
				//Updating base addresses
				base += 4;
				base_out += 4;

				for (i = 0; i < FILTER_H; i++){
					for (j = 0; j < 4; j++)
						new_val.m128_f32[j] = imgSrc[base + (i * imgWidthF) * 4 + j];	//Reading new values
					median = insert_and_get_median(new_val, val, pos, &index, 1);		//Get median value of the new window.
				}
				//printf("x: %d - i: %d\r\n", X, index);
				imgDst[base_out + 0] = median.m128_f32[0];	//Writing out median values
				imgDst[base_out + 1] = median.m128_f32[1];
				imgDst[base_out + 2] = median.m128_f32[2];
			}
			Y = Y + 1;

			//Vertical movement, right side

			base = (imgWidthF * (Y + FILTER_H) - FILTER_W) * 4;	//Setting up base addresses
			base_out = ((Y + 1) * imgWidth - 1) * 4;

			tmp_1 = val[0];
			tmp_2 = pos[0];
			for (i = 0; i < (FILTER_H * FILTER_W - 1); i++){
				val[i] = val[i + 1];
				pos[i] = pos[i + 1];
			}
			val[FILTER_H * FILTER_W - 1] = tmp_1;
			pos[FILTER_H * FILTER_W - 1] = tmp_2;

			for (i = 0; i < FILTER_H; i++){
				for (j = 0; j < 4; j++)
					new_val.m128_f32[j] = imgSrc[base + i * 4 + j];				//Reading new values
				index = (index + FILTER_H) % (FILTER_H * FILTER_W) - 1;			//At stepping down, we need an extra index value calculation
				median = insert_and_get_median(new_val, val, pos, &index, 1);	//Get median value of the new window.
			}
			index = (index + 1) % (FILTER_H * FILTER_W);

			imgDst[base_out + 0] = median.m128_f32[0];	//Writing out median values
			imgDst[base_out + 1] = median.m128_f32[1];
			imgDst[base_out + 2] = median.m128_f32[2];

			base = (imgWidthF * (Y + 1) - FILTER_W) * 4;	//Setting up base addresses

			//Horizontal part, backward
			for (X = imgWidth - 2; X >= 0; X--){
				//Updating base addresses
				base -= 4;
				base_out -= 4;

				for (i = FILTER_H - 1; i >= 0; i--){
					for (j = 0; j < 4; j++)
						new_val.m128_f32[j] = imgSrc[base + (i * imgWidthF) * 4 + j];	//Reading new values
					median = insert_and_get_median(new_val, val, pos, &index, -1);		//Get median value of the new window.
				}
				imgDst[base_out + 0] = median.m128_f32[0];	//Writing out median values
				imgDst[base_out + 1] = median.m128_f32[1];
				imgDst[base_out + 2] = median.m128_f32[2];
			}
			Y = Y + 1;
		}
	}
#pragma omp barrier
	return;
}
