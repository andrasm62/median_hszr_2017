int partition(int *nums, int *pos, int left, int right);

void quicksort(char *nums, char* pos, int l, int r);

void insertionsort(float *nums, int* pos, int n);

void sort_and_merge(char* a, char* b, char* c, signed char* a_pos, signed char* b_pos, signed char* c_p);

void median_filter_C(int imgHeight, int imgWidth, int imgWidthF, int imgFOffsetW, int imgFOffsetH, float* imgSrc, float* imgDst);

void median_filter_SSE(int imgHeight, int imgWidth, int imgWidthF, int imgFOffsetW, int imgFOffsetH, float* imgSrc, float* imgDst);
