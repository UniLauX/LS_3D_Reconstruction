#ifndef __BOXFILTER__H
#define __BOXFILTER__H

#include <assert.h>
#include "ZImage.h"
#include <stdio.h>

namespace IMGAlgorithm
{
	enum EBorderMode
	{
		eBorderZero         = 0,    // zero padding
		eBorderReplicate    = 1,    // replicate border values
		eBorderReflect      = 2,    // reflect border pixels
		eBorderCyclic       = 3     // wrap pixel values
	};



	int borderIndex(int index, int len, EBorderMode m);

	template <class T>
	void boxFilterLines(T* src, T* dst, int n_sums,
		int len, int stride, int w, int pl, int pr, EBorderMode borderMode, float borderPenalty,
		bool average)
	{
		// Box filter multiple rows or columns of an image
		// w      - window width (e.g., 5)
		// pl     - offset to first cell to left of window  (e.g., -3)
		// pr     - offset to last cell in window (e.g., 2)
		// src    - pointer to original data
		// dst    - pointer to result
		// n_sums - number of running sums to be kept
		// len    - length of each row or column
		// stride - distance between elements in line
		// average- compute an average


		int i, k, x, rx;

		T scale = (T)((average) ? 1.0 / w : 1);  // factor for computing average

		// compute first value (x==0)
		for (k=0; k<n_sums; k++) 
		{   
			T sum = 0;
			//left half:
			if (borderMode != eBorderZero)
			{
				for (i=pl+1; i<0; i++)
					sum += scale * src[k+stride * borderIndex(i, len, borderMode)];
			}
			else{
				for (i=pl+1; i<0; i++)
					sum += scale * borderPenalty;
			}
			// right half:
			for (i=0; i<=pr; i++)
				sum += scale * src[k+stride * i];

			// store average
			dst[k] = sum;
		}

		// compute values in left border area (x=1..-pl+1)
		x = 1;
		rx = x * stride;
		T *dst_current = &dst[rx];
		T *dst_previous = &dst[rx-stride];
		T *src_left = &src[rx+pl*stride];
		T *src_right = &src[rx+pr*stride];

		if (borderMode != eBorderZero){
			for (; x < -pl; x++) 
			{
				for (k=0; k<n_sums; k++) 
				{
					dst_current[k] = dst_previous[k] 
					+ scale *
						(-src[k+stride * borderIndex(x+pl, len, borderMode)]    // -src_left[k]
					+ src_right[k]);
				}
				dst_current += stride;
				dst_previous += stride;
				src_left += stride;
				src_right += stride;
			}
		}
		else{
			for (; x < -pl; x++) 
			{
				for (k=0; k<n_sums; k++) 
				{
					dst_current[k] = dst_previous[k] 
					+ scale *
						(-borderPenalty    // -src_left[k]
						+ src_right[k]);
				}
				dst_current += stride;
				dst_previous += stride;
				src_left += stride;
				src_right += stride;
			}
		}


		// compute values in center area (x=-pl .. len-pr-1)
		x = -pl;
		rx = x * stride;
		assert (dst_current == &dst[rx]);


		for (; x < len-pr; x++) 
		{
			for (k=0; k<n_sums; k++) 
			{
				dst_current[k] = dst_previous[k] + scale * (-src_left[k] + src_right[k]);
			}
			dst_current += stride;
			dst_previous += stride;
			src_left += stride;
			src_right += stride;
		}


		// compute values in right border area (x=len-pr .. len-1)
		x = len-pr;
		rx = x * stride;
		assert (dst_current == &dst[rx]);

		if (borderMode != eBorderZero){
			for (; x < len; x++) 
			{
				for (k=0; k<n_sums; k++) 
				{
					dst_current[k] = dst_previous[k] 
					+ scale * 
						(-src_left[k] 
					+src[k+stride * borderIndex(x+pr, len, borderMode)]);   // src_right[k];
				}
				dst_current += stride;
				dst_previous += stride;
				src_left += stride;
				src_right += stride;
			}
		}
		else{
			for (; x < len; x++) 
			{
				for (k=0; k<n_sums; k++) 
				{
					dst_current[k] = dst_previous[k] 
					+ scale * 
						(-src_left[k] 
					+borderPenalty);   // src_right[k];
				}
				dst_current += stride;
				dst_previous += stride;
				src_left += stride;
				src_right += stride;
			}
		}

	}

	template <class T>
	void BoxFilter(ZImage<T>& src, ZImage<T>& dst,
		int xWidth, int yWidth, EBorderMode borderMode, float borderPenalty, bool average)
	{
		// Box filter operation
		// second version - operates in memory order to avoid swapping
		if (xWidth != yWidth)
			printf("BoxFilter: xWidth != yWidth not implemented yet");          

		int width = src.GetWidth(), height = src.GetHeight(), n_bands = src.GetChannel();

		dst.Create(width,height,n_bands);     // allocate memory for copy of src (n_bands=1)
		ZImage<T> tmp;
		tmp.Create(width,height,n_bands);     // allocate memory for copy of src    

		int w = xWidth;  // window size      (e.g.,  5)
		int pr = w / 2;     // pointer to right (e.g.,  2): value to add
		int pl = pr - w;    // pointer to left  (e.g., -3): value to subtract

		// aggregate each row, all disparity levels in parallel

		for (int y = 0; y < height; y++)
		{
			T* src_row = &src.Pixel(0, y, 0);
			T* dst_row = &tmp.Pixel(0, y, 0);

			boxFilterLines(src_row, dst_row, n_bands, width, n_bands, w, pl, pr, borderMode, borderPenalty, average);
		}

		// aggregate all columns at all disparity levels in parallel

		// compute vertical stride
		int stride  = &dst.Pixel(0, 1, 0) - &dst.Pixel(0, 0, 0);
		int stride2 = &tmp.Pixel(0, 1, 0) - &tmp.Pixel(0, 0, 0);
		assert(stride == stride2);

		T* src_cols = &tmp.Pixel(0, 0, 0);
		//T* src_cols = &src.Pixel(0, 0, 0);
		T* dst_cols = &dst.Pixel(0, 0, 0);

		boxFilterLines(src_cols, dst_cols, n_bands*width, height, stride, w, pl, pr, borderMode, borderPenalty, average);
		//dst = tmp;
	}


}



#endif

