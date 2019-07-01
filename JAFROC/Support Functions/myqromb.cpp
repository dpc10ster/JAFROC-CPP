#include "stdafx.h"
#include "../common.h"
#include <cmath>
#include "nr.h"
using namespace std;


DP myqtrap(DP func(const DP), const DP a, const DP b)
{
	const int JMAX=20;
	const DP EPS=1.0e-10;
	int j;
	DP s,olds=0.0;

	for (j=0;j<JMAX;j++) {
		s=NR::trapzd(func,a,b,j+1);
		if (j > 5)
			if (fabs(s-olds) < EPS*fabs(olds) ||
				(s == 0.0 && olds == 0.0)) return s;
		olds=s;
	}
//	nrerror("Too many steps in routine qtrap");
	return 0.0;
}


DP myqsimp(DP func(const DP), const DP a, const DP b)
{
	const int JMAX=20;
	const DP EPS=1.0e-10;
	int j;
	DP s,st,ost=0.0,os=0.0;

	for (j=0;j<JMAX;j++) {
		st=NR::trapzd(func,a,b,j+1);
		s=(4.0*st-ost)/3.0;
		if (j > 5)
			if (fabs(s-os) < EPS*fabs(os) ||
				(s == 0.0 && os == 0.0)) return s;
		os=s;
		ost=st;
	}
//	nrerror("Too many steps in routine qsimp");
//	return 0.0;
	return myqtrap( func, a, b ) ;
}

DP myqromb(DP func(const DP), DP a, DP b)
{
	const int JMAX=20, JMAXP=JMAX+1, K=5;
	const DP EPS=1.0e-10;
	DP ss,dss;
	Vec_DP s(JMAX),h(JMAXP),s_t(K),h_t(K);
	int i,j;

	h[0]=1.0;
	for (j=1;j<=JMAX;j++) {
		s[j-1]=NR::trapzd(func,a,b,j);
		if (j >= K) {
			for (i=0;i<K;i++) {
				h_t[i]=h[j-K+i];
				s_t[i]=s[j-K+i];
			}
			NR::polint(h_t,s_t,0.0,ss,dss);
			if (fabs(dss) <= EPS*fabs(ss)) return ss;
		}
		h[j]=0.25*h[j-1];
	}
//	nrerror("Too many steps in routine qromb");
	return myqsimp( func, a, b ) ;
}
