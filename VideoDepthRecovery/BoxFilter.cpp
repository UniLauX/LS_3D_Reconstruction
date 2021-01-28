#include "BoxFilter.h"


using namespace IMGAlgorithm;


int IMGAlgorithm::borderIndex(int index, int len, EBorderMode m) {
	// TODO:  this code is common with Convolve.cpp:  should be merged...

	// returns correct index depending on border mode
	// cannot be used with eBorderZero since it returns index, not value

	if (index < 0) 
	{
		assert(-index < len);   // "double" wrapping not supported
		switch (m) {
		case eBorderReplicate:
			return 0;
		case eBorderReflect:
			return -index;
		case eBorderCyclic:
			return index+len;
		default:
			printf("borderIndex: bad borderMode");
		}
	}
	else if (index >= len)
	{
		assert(len+len-index >= 2); // "double" wrapping not supported
		switch (m) {
		case eBorderReplicate:
			return len-1;
		case eBorderReflect:
			return len+len-index-2;
		case eBorderCyclic:
			return index-len;
		default:
			printf("borderIndex: bad borderMode");
		}
	}
	else
		return index;
}
///////////////////////////////////////////////////////////////////////////
// box filter functions

