#ifndef _NR_H_
#define _NR_H_
#include <fstream>
#include <complex>
#include "nrutil_nr.h"
#include "nrtypes_nr.h"
using namespace std;

namespace NR {


void simp1(Mat_I_DP &a, const int mm, Vec_I_INT &ll, const int nll,
	const int iabf, int &kp, DP &bmax);
void simp2(Mat_I_DP &a, const int m, const int n, int &ip, const int kp);
void simp3(Mat_IO_DP &a, const int i1, const int k1, const int ip,
	const int kp);
void simplx(Mat_IO_DP &a, const int m1, const int m2, const int m3,
	int &icase, Vec_O_INT &izrov, Vec_O_INT &iposv);

}
#endif /* _NR_H_ */
