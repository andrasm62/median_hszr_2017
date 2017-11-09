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
	float sum = 0.0;
	int i;

	if (dir == 1){
		(*index)++;
		if ((*index) == (FILTER_W * FILTER_H))
		(*index) = 0;
	}
	else {
		(*index)--;
		if ((*index) == -1)
		(*index) = FILTER_W * FILTER_H - 1;
	}
	old_val = val[*index];
	val[*index] = _mm_set_ps(256.0, 256.0, 256.0, 256.0);
	//pos[*index] = _mm_set_ps(2 * FILTER_W * FILTER_H + 0.0, 2 * FILTER_W * FILTER_H + 0.0, 2 * FILTER_W * FILTER_H + 0.0, 2 * FILTER_W * FILTER_H + 0.0);
	//for (i = (((*index) + 1) % (FILTER_W * FILTER_H)); i != ((*index) % (FILTER_W * FILTER_H)); i = ((i + 1) % (FILTER_W * FILTER_H))){
	for (i = 0; i < (FILTER_W * FILTER_H); i++){
		//printf("Index: %d\r\npos_o:%2.0f ",*index, pos[i].m128_f32[0]);
		//if (i > *index)
		//temp = _mm_cmpge_ps(val[i], val[*index]);
		//else
		//temp = _mm_cmpgt_ps(val[i], val[*index]);
		pos[i] =_mm_sub_ps(pos[i],_mm_and_ps(_mm_or_ps(_mm_and_ps(_mm_cmpeq_ps(val[i], old_val),_mm_cmpgt_ps(pos[i], pos[*index])),_mm_cmpgt_ps(val[i], old_val)),one));
		//printf("pos_n: %2.0f val[i]: %2.0f > val[index]: %2.0f i: %d\r\n", pos[i].m128_f32[0], val[i].m128_f32[0], val[*index].m128_f32[0], i);
		//printf("val[i]: %2.0f >= new_val: %2.0f\r\npos_o[i]: %2.0f ", val[i].m128_f32[0], new_val.m128_f32[0], pos[i].m128_f32[0]);
		pos[i] = _mm_add_ps(pos[i], _mm_and_ps(_mm_cmpge_ps(val[i], new_val), one));
		//printf("pos[i]:%2.0f \r\ntemp_pos_o: %2.0f ", pos[i].m128_f32[0], temp_pos.m128_f32[0]);
		temp_pos = _mm_add_ps(temp_pos, _mm_and_ps(_mm_cmpgt_ps(new_val, val[i]), one));
		//printf("temp_pos:%2.0f new_val: %2.0f > val[i]: %2.0f\r\n", temp_pos.m128_f32[0], new_val.m128_f32[0], pos[i].m128_f32[0]);
		median = _mm_add_ps(median, _mm_and_ps(_mm_cmpeq_ps(pos[i], med_of_filter_width), val[i]));
	}
	temp_pos = _mm_sub_ps(temp_pos, _mm_and_ps(_mm_cmpgt_ps(new_val, val[*index]), one));
	//printf("temp_pos: %2.0f\r\n\r\n", temp_pos.m128_f32[0]);
	val[*index] = new_val;
	pos[*index] = temp_pos;
	median = _mm_add_ps(median, _mm_and_ps(_mm_cmpeq_ps(temp_pos, med_of_filter_width), new_val));
	/*for (int i = 0; i < 25; i++) {
		sum += pos[i].m128_f32[0];
		}
		if (sum != 300.0)
		while (1);
	*/
	/*
	printf("%2.0f\r\n", new_val.m128_f32[0]);
	for (i = 0; i < 25; i++){
		printf("%2.0f ", val[i].m128_f32[0]);
	}
	printf("\r\n");
	for (i = 0; i < 25; i++){
		printf("%2.0f ", pos[i].m128_f32[0]);
	}
	printf("\r\nmedian:%2.0f %2.0f %2.0f %2.0f \r\n", median.m128_f32[0], median.m128_f32[1], median.m128_f32[2], median.m128_f32[3]);
	*/
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

	//one = _mm_castsi128_ps(_mm_set1_epi32(0x01));
	one = _mm_set_ps(1.0, 1.0, 1.0, 1.0);

	for (i = 0; i < FILTER_W * FILTER_H; i++){
		val[i] = _mm_set_ps(0.0, 0.0, 0.0, 0.0);
		pos[i] = _mm_set_ps(i + 0.0, i + 0.0, i + 0.0, i + 0.0);
	}

#pragma omp parallel for num_threads(NUM_OF_THREADS) private(X, Y, i, j, k, base, base_out, new_val, median, tmp_1, tmp_2, val, pos)
	for (int part = 0; part < NUM_OF_THREADS; part++){
		Y = part * imgHeight / NUM_OF_THREADS;
		for (k = 0; k < FILTER_H - 1; k++){
			for (i = imgFOffsetW; i < FILTER_W; i++){
				for (j = 0; j < 4; j++)
				new_val.m128_f32[j] = imgSrc[((imgFOffsetH + 1) * imgWidthF + i) * 4 + j];
				index = (i * FILTER_H) + k;
				insert_and_get_median(new_val, val, pos, &index, 1);
			}
		}
//---------------
//Image processing
//---------------

		while (Y < imgHeight){

			//Vertical movement, left side
			index = FILTER_H * FILTER_W - 1;
			base = (imgWidthF * (Y + FILTER_H - 1)) * 4;	//Setting up base addresses
			base_out = Y * imgWidth * 4;

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
				new_val.m128_f32[j] = imgSrc[base + i * 4 + j];
				index = (index + FILTER_H) % (FILTER_H * FILTER_W) - 1;
				median = insert_and_get_median(new_val, val, pos, &index, 1);
			}

			imgDst[base_out + 0] = median.m128_f32[0];
			imgDst[base_out + 1] = median.m128_f32[1];
			imgDst[base_out + 2] = median.m128_f32[2];

			base = (imgWidthF * Y + FILTER_W - 1) * 4;

			//Horizontal part, forward
			for (X = 1; X < imgWidth; X++){
				base += 4;
				base_out += 4;
				for (i = 0; i < FILTER_H; i++){
					for (j = 0; j < 4; j++)
					new_val.m128_f32[j] = imgSrc[base + (i * imgWidthF) * 4 + j];
					median = insert_and_get_median(new_val, val, pos, &index, 1);
				}
				//printf("x: %d - i: %d\r\n", X, index);
				imgDst[base_out + 0] = median.m128_f32[0];
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
				new_val.m128_f32[j] = imgSrc[base + i * 4 + j];
				index = (index + FILTER_H) % (FILTER_H * FILTER_W) - 1;
				median = insert_and_get_median(new_val, val, pos, &index, 1);
			}
			index = (index + 1) % (FILTER_H * FILTER_W);

			imgDst[base_out + 0] = median.m128_f32[0];
			imgDst[base_out + 1] = median.m128_f32[1];
			imgDst[base_out + 2] = median.m128_f32[2];

			base = (imgWidthF * (Y + 1) - FILTER_W) * 4;	//Setting up base addresses

			//Horizontal part, backward
			for (X = imgWidth - 2; X >= 0; X--){
				base -= 4;
				base_out -= 4;
				for (i = FILTER_H - 1; i >= 0; i--){
					for (j = 0; j < 4; j++)
					new_val.m128_f32[j] = imgSrc[base + (i * imgWidthF) * 4 + j];
					median = insert_and_get_median(new_val, val, pos, &index, -1);
				}
				imgDst[base_out + 0] = median.m128_f32[0];
				imgDst[base_out + 1] = median.m128_f32[1];
				imgDst[base_out + 2] = median.m128_f32[2];
			}
			Y = Y + 1;
		}
	}
#pragma omp barrier
	return;
}
