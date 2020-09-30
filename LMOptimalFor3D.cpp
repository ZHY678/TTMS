#include "StdAfx.h"
#include "LMOptimalFor3D.h"
#include <vector>
#include <math.h>

LMOptimalFor3D::LMOptimalFor3D(void)
{

}

LMOptimalFor3D::~LMOptimalFor3D(void)
{

}

#define REL_SENSOR_TOLERANCE_ftol    0.0001
#define REL_PARAM_TOLERANCE_xtol     0.0001
#define ORTHO_TOLERANCE_gtol         0.0
#define MAXFEV                       10
#define EPSFCN                       0.0001//1.0E-16     /* Do not set to zero! */
#define MODE                         1           
#define FACTOR                       100.0 

#define EPSILON		1.0E-16            //指定的误差容限;
/* Table of constant values */
static int c__1 = 1;
static int c__2 = 2;


void LMOptimalFor3D::TNFP_LM(double *para, int m, int n, double* fdis, vector<vector<cv::Point2f>> pImgL, vector<vector<cv::Point2f>> pImgR, vector<cv::Point3f> ObjectPoints)
{
	if (pImgL.size() != pImgR.size() || pImgL.size() < 3)
	{
		return;
	}

	int i;
	double *fvec = new double[m];
	double ftol = REL_SENSOR_TOLERANCE_ftol;
	double xtol = REL_PARAM_TOLERANCE_xtol;
	double gtol = ORTHO_TOLERANCE_gtol;
	int maxfev = MAXFEV*n;
	double epsfcn = EPSFCN;
	double *diag = new double[n];
	int mode=MODE;
	double factor = FACTOR;
	int nprint = 0, info = 0, nfev = 0;
	double *fjac = new double[m*n];
	int ldfjac = m;
	int *ipvt = new int [n];
	int iflag = 1;
	double *qtf=new double[n];
	double *wa1=new double[n];
	double *wa2=new double[n];
	double *wa3=new double[n];
	double *wa4=new double[m];
	if(mode==2)
	{
		for(i=0;i<n;i++)
			diag[i]=1.0;
	}
	//计算优化之前的投影残差;
	lmdif_ (Costfunction,&m, &n, para, fvec, fdis, pImgL, pImgR, ObjectPoints, &ftol, &xtol, &gtol, &maxfev, &epsfcn,
		diag, &mode, &factor, &nprint, &info, &nfev, fjac, &ldfjac,
		ipvt, qtf, wa1, wa2, wa3, wa4);//调用lm非线性优化函数;

	//计算欧式比例因子及平面误差;
	//CalScaleAndPlaneErr(para, fdis, pImgL, pImgR, ObjectPoints);

	//测试误差输出;
	//ofstream file("D://err.txt");

	//计算优化误差;
	int m_count = 0;
	double m_err = 0;
	for (i = 0; i < m; i++)
	{
		m_err = m_err + *(fvec+i);
	}

	*fdis = m_err / m;

	//file.close();

	//释放内存;
	delete [] fjac; delete [] fvec; delete [] diag; delete [] ipvt;
	delete [] qtf; delete [] wa1; delete [] wa2; delete [] wa3; delete [] wa4; 
}

/* Subroutine */
int LMOptimalFor3D::lmdif_(void (*fcn)(int* a,int* b,double* c,double* d,int* e, double* fdis, vector<vector<cv::Point2f>> pImgL, vector<vector<cv::Point2f>> pImgR, vector<cv::Point3f> ObjectPoints),
	int* m, int* n, double* x, double* fvec, double* fdis, vector<vector<cv::Point2f>> pImgL, vector<vector<cv::Point2f>> pImgR, vector<cv::Point3f> ObjectPoints, double* ftol, 
	double* xtol, double* gtol, int* __maxfev,double* epsfcn, 
	double* diag, int* mode, double* factor,int* nprint, 
	int* info, int* nfev, double* fjac, int*ldfjac,int* ipvt,
	double* qtf, double* wa1,double* wa2, double* wa3, double* wa4)

{
	static double one = 1.;
	static double p1 = .1;
	static double p5 = .5;
	static double p25 = .25;
	static double p75 = .75;
	static double p0001 = 1e-4;
	static double zero = 0.;
	int fjac_dim1, fjac_offset, i__1, i__2;
	double d__1, d__2, d__3;
	static int iter;
	static double temp, temp1, temp2;
	static int i, j, l, iflag;
	static double delta;
	static double ratio;
	static double fnorm, gnorm;
	static double pnorm, xnorm, fnorm1, actred, dirder, epsmch, prered;
	static double par, sum;
	--wa4;
	--wa3;
	--wa2;
	--wa1;
	--qtf;
	--ipvt;
	fjac_dim1 = *ldfjac;
	fjac_offset = fjac_dim1 + 1;
	fjac -= fjac_offset;
	--diag;
	--fvec;
	--x;

	epsmch = dpmpar_(&c__1);
	*info = 0;
	iflag = 0;
	*nfev = 0;
	if (*n <= 0 || *m < *n || *ldfjac < *m || *ftol < zero || *xtol < zero || 
		*gtol < zero || *__maxfev <= 0 || *factor <= zero) 
	{
		goto L300;
	}
	if (*mode != 2)
	{
		goto L20;
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		if (diag[j] <= zero)
		{
			goto L300;
		}

	}
L20:

	iflag = 1;
	(*fcn)(m, n, &x[1], &fvec[1], &iflag, fdis, pImgL, pImgR, ObjectPoints);
	*nfev = 1;
	if (iflag < 0)
	{
		goto L300;
	}
	fnorm = enorm_(m, &fvec[1]);

	par = zero;
	iter = 1;

L30:

	iflag = 2;
	fdjac2_(fcn, m, n, &x[1], &fvec[1], fdis, pImgL, pImgR, ObjectPoints, &fjac[fjac_offset], ldfjac, &iflag, 
		epsfcn, &wa4[1]);
	*nfev += *n;
	if (iflag < 0)
	{
		goto L300;
	}

	if (*nprint <= 0)
	{
		goto L40;
	}
	iflag = 0;
	if ((iter - 1) % *nprint == 0) 
	{
		(*fcn)(m, n, &x[1], &fvec[1], &iflag, fdis, pImgL, pImgR, ObjectPoints);
	}
	if (iflag < 0)
	{
		goto L300;
	}
L40:
	qrfac_(m, n, &fjac[fjac_offset], ldfjac, &c__1, &ipvt[1], n, &wa1[1], &
		wa2[1], &wa3[1]);

	if (iter != 1)
	{
		goto L80;
	}
	if (*mode == 2)
	{
		goto L60;
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		diag[j] = wa2[j];
		if (wa2[j] == zero) 
		{
			diag[j] = one;
		}

	}
L60:

	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		wa3[j] = diag[j] * x[j];

	}
	xnorm = enorm_(n, &wa3[1]);
	delta = *factor * xnorm;
	if (delta == zero)
	{
		delta = *factor;
	}
L80:

	i__1 = *m;
	for (i = 1; i <= i__1; ++i)
	{
		wa4[i] = fvec[i];

	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		if (fjac[j + j * fjac_dim1] == zero)
		{
			goto L120;
		}
		sum = zero;
		i__2 = *m;
		for (i = j; i <= i__2; ++i)
		{
			sum += fjac[i + j * fjac_dim1] * wa4[i];

		}
		temp = -sum / fjac[j + j * fjac_dim1];
		i__2 = *m;
		for (i = j; i <= i__2; ++i)
		{
			wa4[i] += fjac[i + j * fjac_dim1] * temp;
		}
L120:
		fjac[j + j * fjac_dim1] = wa1[j];
		qtf[j] = wa4[j];

	}

	gnorm = zero;
	if (fnorm == zero)
	{
		goto L170;
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		l = ipvt[j];
		if (wa2[l] == zero) 
		{
			goto L150;
		}
		sum = zero;
		i__2 = j;
		for (i = 1; i <= i__2; ++i)
		{
			sum += fjac[i + j * fjac_dim1] * (qtf[i] / fnorm);

		}

		d__2 = gnorm, d__3 = (d__1 = sum / wa2[l], fabs(d__1));
		gnorm = __max(d__2,d__3);
L150:

		;
	}
L170:

	if (gnorm <= *gtol)
	{
		*info = 4;
	}
	if (*info != 0)
	{
		goto L300;
	}

	if (*mode == 2)
	{
		goto L190;
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{

		d__1 = diag[j], d__2 = wa2[j];
		diag[j] = __max(d__1,d__2);

	}
L190:

L200:

	lmpar_(n, &fjac[fjac_offset], ldfjac, &ipvt[1], &diag[1], &qtf[1], &delta,
		&par, &wa1[1], &wa2[1], &wa3[1], &wa4[1]);

	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		wa1[j] = -wa1[j];
		wa2[j] = x[j] + wa1[j];
		wa3[j] = diag[j] * wa1[j];

	}
	pnorm = enorm_(n, &wa3[1]);

	if (iter == 1) 
	{
		delta = __min(delta,pnorm);
	}

	iflag = 1;
	(*fcn)(m, n, &wa2[1], &wa4[1], &iflag, fdis, pImgL, pImgR, ObjectPoints);
	++(*nfev);
	if (iflag < 0)
	{
		goto L300;
	}
	fnorm1 = enorm_(m, &wa4[1]);

	actred = -one;
	if (p1 * fnorm1 < fnorm) 
	{

		d__1 = fnorm1 / fnorm;
		actred = one - d__1 * d__1;
	}

	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		wa3[j] = zero;
		l = ipvt[j];
		temp = wa1[l];
		i__2 = j;
		for (i = 1; i <= i__2; ++i)
		{
			wa3[i] += fjac[i + j * fjac_dim1] * temp;

		}

	}
	temp1 = enorm_(n, &wa3[1]) / fnorm;
	temp2 = sqrt(par) * pnorm / fnorm;

	d__1 = temp1;

	d__2 = temp2;
	prered = d__1 * d__1 + d__2 * d__2 / p5;

	d__1 = temp1;

	d__2 = temp2;
	dirder = -(d__1 * d__1 + d__2 * d__2);

	ratio = zero;
	if (prered != zero) 
	{
		ratio = actred / prered;
	}

	if (ratio > p25)
	{
		goto L240;
	}
	if (actred >= zero)
	{
		temp = p5;
	}
	if (actred < zero)
	{
		temp = p5 * dirder / (dirder + p5 * actred);
	}
	if (p1 * fnorm1 >= fnorm || temp < p1)
	{
		temp = p1;
	}

	d__1 = delta, d__2 = pnorm / p1;
	delta = temp * __min(d__1,d__2);
	par /= temp;
	goto L260;
L240:
	if (par != zero && ratio < p75) 
	{
		goto L250;
	}
	delta = pnorm / p5;
	par = p5 * par;
L250:
L260:

	if (ratio < p0001)
	{
		goto L290;
	}

	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		x[j] = wa2[j];
		wa2[j] = diag[j] * x[j];

	}
	i__1 = *m;
	for (i = 1; i <= i__1; ++i)
	{
		fvec[i] = wa4[i];

	}
	xnorm = enorm_(n, &wa2[1]);
	fnorm = fnorm1;
	++iter;
L290:

	if (fabs(actred) <= *ftol && prered <= *ftol && p5 * ratio <= one)
	{
		*info = 1;
	}
	if (delta <= *xtol * xnorm)
	{
		*info = 2;
	}
	if (fabs(actred) <= *ftol && prered <= *ftol && p5 * ratio <= one && *info 
		== 2)
	{
		*info = 3;
	}
	if (*info != 0) 
	{
		goto L300;
	}

	if (*nfev >= *__maxfev)
	{
		*info = 5;
	}
	if (fabs(actred) <= epsmch && prered <= epsmch && p5 * ratio <= one)
	{
		*info = 6;
	}
	if (delta <= epsmch * xnorm)
	{
		*info = 7;
	}
	if (gnorm <= epsmch)
	{
		*info = 8;
	}
	if (*info != 0) 
	{
		goto L300;
	}

	if (ratio < p0001)
	{
		goto L200;
	}

	goto L30;
L300:

	if (iflag < 0)
	{
		*info = iflag;
	}
	iflag = 0;
	if (*nprint > 0) 
	{
		(*fcn)(m, n, &x[1], &fvec[1], &iflag, fdis, pImgL, pImgR, ObjectPoints);
	}
	return 0;

} 
/*******************************************************************************************/
int LMOptimalFor3D::qrfac_(int* m, int* n, double* a, int* lda,int* pivot, int* ipvt, int* lipvt, double* rdiag,double* acnorm,double* wa)
{


	static double one = 1.;
	static double p05 = .05;
	static double zero = 0.;


	int a_dim1, a_offset, i__1, i__2, i__3;
	double d__1, d__2, d__3;


	static int k__max;
	static double temp;
	static int i, j, k, __minmn;
	static double epsmch;
	static double ajnorm;
	static int jp1;
	static double sum;
	--wa;
	--acnorm;
	--rdiag;
	--ipvt;
	a_dim1 = *lda;
	a_offset = a_dim1 + 1;
	a -= a_offset;



	epsmch = dpmpar_(&c__1);


	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
		acnorm[j] = enorm_(m, &a[j * a_dim1 + 1]);
		rdiag[j] = acnorm[j];
		wa[j] = rdiag[j];
		if (*pivot) {
			ipvt[j] = j;
		}
	}


	__minmn = __min(*m,*n);
	i__1 = __minmn;
	for (j = 1; j <= i__1; ++j) {
		if (! (*pivot)) {
			goto L40;
		}


		k__max = j;
		i__2 = *n;
		for (k = j; k <= i__2; ++k) {
			if (rdiag[k] > rdiag[k__max]) {
				k__max = k;
			}

		}
		if (k__max == j) {
			goto L40;
		}
		i__2 = *m;
		for (i = 1; i <= i__2; ++i) {
			temp = a[i + j * a_dim1];
			a[i + j * a_dim1] = a[i + k__max * a_dim1];
			a[i + k__max * a_dim1] = temp;

		}
		rdiag[k__max] = rdiag[j];
		wa[k__max] = wa[j];
		k = ipvt[j];
		ipvt[j] = ipvt[k__max];
		ipvt[k__max] = k;
L40:

		i__2 = *m - j + 1;
		ajnorm = enorm_(&i__2, &a[j + j * a_dim1]);
		if (ajnorm == zero) {
			goto L100;
		}
		if (a[j + j * a_dim1] < zero) {
			ajnorm = -ajnorm;
		}
		i__2 = *m;
		for (i = j; i <= i__2; ++i) {
			a[i + j * a_dim1] /= ajnorm;

		}
		a[j + j * a_dim1] += one;


		jp1 = j + 1;
		if (*n < jp1) {
			goto L100;
		}
		i__2 = *n;
		for (k = jp1; k <= i__2; ++k) {
			sum = zero;
			i__3 = *m;
			for (i = j; i <= i__3; ++i) {
				sum += a[i + j * a_dim1] * a[i + k * a_dim1];

			}
			temp = sum / a[j + j * a_dim1];
			i__3 = *m;
			for (i = j; i <= i__3; ++i) {
				a[i + k * a_dim1] -= temp * a[i + j * a_dim1];

			}
			if (! (*pivot) || rdiag[k] == zero) {
				goto L80;
			}
			temp = a[j + k * a_dim1] / rdiag[k];

			d__3 = temp;
			d__1 = zero, d__2 = one - d__3 * d__3;
			rdiag[k] *= sqrt((__max(d__1,d__2)));

			d__1 = rdiag[k] / wa[k];
			if (p05 * (d__1 * d__1) > epsmch) {
				goto L80;
			}
			i__3 = *m - j;
			rdiag[k] = enorm_(&i__3, &a[jp1 + k * a_dim1]);
			wa[k] = rdiag[k];
L80:

			;
		}
L100:
		rdiag[j] = -ajnorm;

	}
	return 0;

}
/*************************************************************************************************/
int LMOptimalFor3D::lmpar_(int* n, double* r, int* ldr, int* ipvt,double* diag, double*qtb, double* delta, double* par,double* x, double* sdiag,double* wa1, double* wa2)
{
	static double p1 = .1;
	static double p001 = .001;
	static double zero = 0.;

	int r_dim1, r_offset, i__1, i__2;
	double d__1, d__2;


	static double parc, parl;
	static int iter;
	static double temp, paru;
	static int i, j, k, l;
	static double dwarf;
	static int nsing;
	static double gnorm, fp;
	static double dxnorm;
	static int jm1, jp1;
	static double sum;
	--wa2;
	--wa1;
	--sdiag;
	--x;
	--qtb;
	--diag;
	--ipvt;
	r_dim1 = *ldr;
	r_offset = r_dim1 + 1;
	r -= r_offset;


	dwarf = dpmpar_(&c__2);

	nsing = *n;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
		wa1[j] = qtb[j];
		if (r[j + j * r_dim1] == zero && nsing == *n) {
			nsing = j - 1;
		}
		if (nsing < *n) {
			wa1[j] = zero;
		}

	}
	if (nsing < 1) {
		goto L50;
	}
	i__1 = nsing;
	for (k = 1; k <= i__1; ++k) {
		j = nsing - k + 1;
		wa1[j] /= r[j + j * r_dim1];
		temp = wa1[j];
		jm1 = j - 1;
		if (jm1 < 1) {
			goto L30;
		}
		i__2 = jm1;
		for (i = 1; i <= i__2; ++i) {
			wa1[i] -= r[i + j * r_dim1] * temp;

		}
L30:

		;
	}
L50:
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
		l = ipvt[j];
		x[l] = wa1[j];
	}

	iter = 0;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
		wa2[j] = diag[j] * x[j];
	}
	dxnorm = enorm_(n, &wa2[1]);
	fp = dxnorm - *delta;
	if (fp <= p1 * *delta) {
		goto L220;
	}

	parl = zero;
	if (nsing < *n) {
		goto L120;
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
		l = ipvt[j];
		wa1[j] = diag[l] * (wa2[l] / dxnorm);

	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
		sum = zero;
		jm1 = j - 1;
		if (jm1 < 1) {
			goto L100;
		}
		i__2 = jm1;
		for (i = 1; i <= i__2; ++i) {
			sum += r[i + j * r_dim1] * wa1[i];

		}
L100:
		wa1[j] = (wa1[j] - sum) / r[j + j * r_dim1];

	}
	temp = enorm_(n, &wa1[1]);
	parl = fp / *delta / temp / temp;
L120:

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
		sum = zero;
		i__2 = j;
		for (i = 1; i <= i__2; ++i) {
			sum += r[i + j * r_dim1] * qtb[i];

		}
		l = ipvt[j];
		wa1[j] = sum / diag[l];
	}
	gnorm = enorm_(n, &wa1[1]);
	paru = gnorm / *delta;
	if (paru == zero) {
		paru = dwarf / __min(*delta,p1);
	}

	*par = __max(*par,parl);
	*par = __min(*par,paru);
	if (*par == zero) {
		*par = gnorm / dxnorm;
	}

L150:
	++iter;


	if (*par == zero) {
		d__1 = dwarf, d__2 = p001 * paru;
		*par = __max(d__1,d__2);
	}
	temp = sqrt(*par);
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
		wa1[j] = temp * diag[j];

	}
	qrsolv_(n, &r[r_offset], ldr, &ipvt[1], &wa1[1], &qtb[1], &x[1], &sdiag[1]
	, &wa2[1]);
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
		wa2[j] = diag[j] * x[j];
	}
	dxnorm = enorm_(n, &wa2[1]);
	temp = fp;
	fp = dxnorm - *delta;

	if (fabs(fp) <= p1 * *delta || parl == zero && fp <= temp && temp < zero ||
		iter == 10) {
			goto L220;
	}

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
		l = ipvt[j];
		wa1[j] = diag[l] * (wa2[l] / dxnorm);
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
		wa1[j] /= sdiag[j];
		temp = wa1[j];
		jp1 = j + 1;
		if (*n < jp1) {
			goto L200;
		}
		i__2 = *n;
		for (i = jp1; i <= i__2; ++i) {
			wa1[i] -= r[i + j * r_dim1] * temp;
		}
L200:

		;
	}
	temp = enorm_(n, &wa1[1]);
	parc = fp / *delta / temp / temp;

	if (fp > zero) {
		parl = __max(parl,*par);
	}
	if (fp < zero) {
		paru = __min(paru,*par);
	}

	d__1 = parl, d__2 = *par + parc;
	*par = __max(d__1,d__2);

	goto L150;
L220:

	if (iter == 0) {
		*par = zero;
	}
	return 0;


} 
/**************************************************************************************************/
double LMOptimalFor3D::enorm_(int* n, double* x)
{
	static double one = 1.;
	static double zero = 0.;
	static double rdwarf = 3.834e-20;
	static double rgiant = 1.304e19;

	int i__1;
	double ret_val, d__1;


	static double xabs, x1__max, x3__max;
	static int i;
	static double s1, s2, s3, agiant, floatn;

	--x;
	s1 = zero;
	s2 = zero;
	s3 = zero;
	x1__max = zero;
	x3__max = zero;
	floatn = (double) (*n);
	agiant = rgiant / floatn;
	i__1 = *n;
	for (i = 1; i <= i__1; ++i) {
		xabs = (d__1 = x[i], fabs(d__1));
		if (xabs > rdwarf && xabs < agiant) {
			goto L70;
		}
		if (xabs <= rdwarf) {
			goto L30;
		}

		if (xabs <= x1__max) {
			goto L10;
		}
		d__1 = x1__max / xabs;
		s1 = one + s1 * (d__1 * d__1);
		x1__max = xabs;
		goto L20;
L10:
		d__1 = xabs / x1__max;
		s1 += d__1 * d__1;
L20:
		goto L60;
L30:


		if (xabs <= x3__max) {
			goto L40;
		}
		d__1 = x3__max / xabs;
		s3 = one + s3 * (d__1 * d__1);
		x3__max = xabs;
		goto L50;
L40:
		if (xabs != zero) {
			d__1 = xabs / x3__max;
			s3 += d__1 * d__1;
		}
L50:
L60:
		goto L80;
L70:

		d__1 = xabs;
		s2 += d__1 * d__1;
L80:
		;
	}

	if (s1 == zero) {
		goto L100;
	}
	ret_val = x1__max * sqrt(s1 + s2 / x1__max / x1__max);
	goto L130;
L100:
	if (s2 == zero) {
		goto L110;
	}
	if (s2 >= x3__max) {
		d__1 = x3__max * s3;
		ret_val = sqrt(s2 * (one + x3__max / s2 * d__1));
	}
	if (s2 < x3__max) {
		ret_val = sqrt(x3__max * (s2 / x3__max + x3__max * s3));
	}
	goto L120;
L110:
	ret_val = x3__max * sqrt(s3);
L120:
L130:
	return ret_val;


}
/***************************************************************************************************/
int LMOptimalFor3D::fdjac2_(void (*fcn)(int* a,int* b,double* c,double* d,int* e, double* fdis, vector<vector<cv::Point2f>> pImgL, vector<vector<cv::Point2f>> pImgR, vector<cv::Point3f> ObjectPoints), 
	int* m, int* n, double* x,double* fvec, double* fdis, vector<vector<cv::Point2f>> pImgL, vector<vector<cv::Point2f>> pImgR, vector<cv::Point3f> ObjectPoints , double* fjac, int* ldfjac,int* iflag, double* epsfcn,double* wa)
{
	static double zero = 0.;
	int fjac_dim1, fjac_offset, i__1, i__2;

	static double temp, h;
	static int i, j;
	static double epsmch;
	static double eps;
	--wa;
	fjac_dim1 = *ldfjac;
	fjac_offset = fjac_dim1 + 1;
	fjac -= fjac_offset;
	--fvec;
	--x;
	epsmch = dpmpar_(&c__1);

	eps = sqrt((__max(*epsfcn,epsmch)));
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
		temp = x[j];
		h = eps * fabs(temp);
		if (h == zero) {
			h = eps;
		}
		x[j] = temp + h;
		(*fcn)(m, n, &x[1], &wa[1], iflag, fdis, pImgL, pImgR, ObjectPoints);
		if (*iflag < 0) {
			goto L30;
		}
		x[j] = temp;
		i__2 = *m;
		for (i = 1; i <= i__2; ++i) {
			fjac[i + j * fjac_dim1] = (wa[i] - fvec[i]) / h;
		}
	}
L30:
	return 0;

}
/*****************************************************************************************************/
double LMOptimalFor3D::dpmpar_(int* i)
{
	static struct
	{
		int e_1[6];
		double fill_2[1];
		double e_3;
	} equiv_2 = { 1018167296, 0, 1048576, 0, 2146435071, -1, {0}, 0. };

	double ret_val;

#define dmach ((double *)&equiv_2)
#define __minmag ((int *)&equiv_2 + 2)
#define __maxmag ((int *)&equiv_2 + 4)
#define mcheps ((int *)&equiv_2)
	ret_val = dmach[*i - 1];
	return ret_val;

} 
#undef mcheps
#undef __maxmag
#undef __minmag
#undef dmach
/*****************************************************************************************************/
int LMOptimalFor3D::qrsolv_(int* n, double* r, int* ldr, int* ipvt, double* diag,double* qtb, double* x, double* sdiag, double* wa)
{

	static double p5 = .5;
	static double p25 = .25;
	static double zero = 0.;

	int r_dim1, r_offset, i__1, i__2, i__3;
	double d__1, d__2;
	static double temp;
	static int i, j, k, l;
	static double cotan;
	static int nsing;
	static double qtbpj;
	static int jp1, kp1;
	static double tan_, cos_, sin_, sum;

	--wa;
	--sdiag;
	--x;
	--qtb;
	--diag;
	--ipvt;
	r_dim1 = *ldr;
	r_offset = r_dim1 + 1;
	r -= r_offset;

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i = j; i <= i__2; ++i) {
			r[i + j * r_dim1] = r[j + i * r_dim1];
		}
		x[j] = r[j + j * r_dim1];
		wa[j] = qtb[j];
	}

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {


		l = ipvt[j];
		if (diag[l] == zero) {
			goto L90;
		}
		i__2 = *n;
		for (k = j; k <= i__2; ++k) {
			sdiag[k] = zero;

		}
		sdiag[j] = diag[l];

		qtbpj = zero;
		i__2 = *n;
		for (k = j; k <= i__2; ++k) {

			if (sdiag[k] == zero) {
				goto L70;
			}
			if ((d__1 = r[k + k * r_dim1], fabs(d__1)) >= (d__2 = sdiag[k], 
				fabs(d__2))) {
					goto L40;
			}
			cotan = r[k + k * r_dim1] / sdiag[k];
			d__1 = cotan;
			sin_ = p5 / sqrt(p25 + p25 * (d__1 * d__1));
			cos_ = sin_ * cotan;
			goto L50;
L40:
			tan_ = sdiag[k] / r[k + k * r_dim1];
			d__1 = tan_;
			cos_ = p5 / sqrt(p25 + p25 * (d__1 * d__1));
			sin_ = cos_ * tan_;
L50:

			r[k + k * r_dim1] = cos_ * r[k + k * r_dim1] + sin_ * sdiag[k];
			temp = cos_ * wa[k] + sin_ * qtbpj;
			qtbpj = -sin_ * wa[k] + cos_ * qtbpj;
			wa[k] = temp;

			kp1 = k + 1;
			if (*n < kp1) {
				goto L70;
			}
			i__3 = *n;
			for (i = kp1; i <= i__3; ++i) {
				temp = cos_ * r[i + k * r_dim1] + sin_ * sdiag[i];
				sdiag[i] = -sin_ * r[i + k * r_dim1] + cos_ * sdiag[i];
				r[i + k * r_dim1] = temp;
			}
L70:
			;
		}
L90:


		sdiag[j] = r[j + j * r_dim1];
		r[j + j * r_dim1] = x[j];
	}

	nsing = *n;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
		if (sdiag[j] == zero && nsing == *n) {
			nsing = j - 1;
		}
		if (nsing < *n) {
			wa[j] = zero;
		}
	}
	if (nsing < 1) {
		goto L150;
	}
	i__1 = nsing;
	for (k = 1; k <= i__1; ++k) {
		j = nsing - k + 1;
		sum = zero;
		jp1 = j + 1;
		if (nsing < jp1) {
			goto L130;
		}
		i__2 = nsing;
		for (i = jp1; i <= i__2; ++i) {
			sum += r[i + j * r_dim1] * wa[i];
		}
L130:
		wa[j] = (wa[j] - sum) / sdiag[j];
	}
L150:

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
		l = ipvt[j];
		x[l] = wa[j];
	}
	return 0;

} 
/******************************************************************************************************/
void LMOptimalFor3D::Costfunction(int *m_ptr, int *n_ptr, double *param, double *err, int *iflag, double* fdis, vector<vector<cv::Point2f>> pImgL, vector<vector<cv::Point2f>> pImgR, vector<cv::Point3f> ObjectP)

{
	//m_ptr误差方程个数;
	//n_ptr待优化的未知数个数;
	//param待优化参数的初始值;


	//检测输入数据是否满足要求;
	UINT nStep = 0, PointNum = 99;
	if (pImgL.size() == pImgR.size() && pImgL.size() != 0)
	{
		nStep = pImgL.size();
		PointNum = pImgL[0].size();
	}
	else
	{
		AfxMessageBox(_T("左右图像幅数不对应!"));
		return;
	}
	for (UINT i = 0; i < nStep; i++)
	{
		if (pImgL[i].size() != pImgR[i].size() || pImgL[i].size() != PointNum)
		{
			PointNum = 1000;
			break;
		}
	}
	if (PointNum == 1000)
	{
		AfxMessageBox(_T("左右图像点输入不对应!"));
		return;
	}

	//初始化内部参数;
	int ndex = 0;

	//左相机内参数;
	CvMat* LCameraIntrinsic = cvCreateMat(3,3,CV_64FC1);
	cvmSet(LCameraIntrinsic,0,0,*(param+ndex++));
	cvmSet(LCameraIntrinsic,1,1,*(param+ndex++));
	cvmSet(LCameraIntrinsic,0,2,*(param+ndex++));
	cvmSet(LCameraIntrinsic,1,2,*(param+ndex++));
	cvmSet(LCameraIntrinsic,2,2,1.0);

	//左相机畸变参数;
	CvMat* LCameraDistor = cvCreateMat(4,1,CV_64FC1);
	cvmSet(LCameraDistor,0,0,*(param+ndex++));
	cvmSet(LCameraDistor,1,0,*(param+ndex++));
	cvmSet(LCameraDistor,2,0,*(param+ndex++));
	cvmSet(LCameraDistor,3,0,*(param+ndex++));
	//cvmSet(LCameraDistor,4,0,*(param+ndex++));

	//右相机内参数;
	CvMat* RCameraIntrinsic = cvCreateMat(3,3,CV_64FC1);
	cvmSet(RCameraIntrinsic,0,0,*(param+ndex++));
	cvmSet(RCameraIntrinsic,1,1,*(param+ndex++));
	cvmSet(RCameraIntrinsic,0,2,*(param+ndex++));
	cvmSet(RCameraIntrinsic,1,2,*(param+ndex++));
	cvmSet(RCameraIntrinsic,2,2,1.0);
	//右相机畸变参数;
	CvMat* RCameraDistor = cvCreateMat(4,1,CV_64FC1);
	cvmSet(RCameraDistor,0,0,*(param+ndex++));
	cvmSet(RCameraDistor,1,0,*(param+ndex++));
	cvmSet(RCameraDistor,2,0,*(param+ndex++));
	cvmSet(RCameraDistor,3,0,*(param+ndex++));
	//cvmSet(RCameraDistor,4,0,*(param+ndex++));
	//双目相机旋转关系M;
	CvMat* OurAngle = cvCreateMat(1, 3, CV_64FC1);
	CvMat* R = cvCreateMat(3, 3, CV_64FC1);
	CvMat* M = cvCreateMat(4,4,CV_64FC1);
	CvMat* MInv = cvCreateMat(4,4,CV_64FC1);

	cvmSet(OurAngle, 0, 0, *(param+ndex++));
	cvmSet(OurAngle, 0, 1, *(param+ndex++));
	cvmSet(OurAngle, 0, 2, *(param+ndex++));
	cvRodrigues2(OurAngle, R);

	cvSetZero(M);
	for (UINT a=0; a<3; a++)
	{
		for (UINT b=0; b<3; b++)
		{
			cvmSet(M,a,b,cvmGet(R,a,b));
		}
		cvmSet(M,a,3,*(param+ndex++));
	}
	cvmSet(M,3,3,1);
	cvInvert(M,MInv);

	cvReleaseMat(&OurAngle);
	cvReleaseMat(&R);
	cvReleaseMat(&MInv);

	//标定板初始三维点;
	vector<cv::Point3f> ObjectPoints;
	cv::Point3f ObjectPointsTemp;
	for (UINT i = 0; i < PointNum; i++)
	{
		ObjectPointsTemp.x = *(param+ndex++);
		ObjectPointsTemp.y = *(param+ndex++);
		ObjectPointsTemp.z = *(param+ndex++);
		ObjectPoints.push_back(ObjectPointsTemp);
	}

	CvMat* LCameraRotation  = cvCreateMat(1, 3, CV_64FC1);
	CvMat* LCameraTranslation  = cvCreateMat(1, 3, CV_64FC1);
	CvMat* RCameraRotation  = cvCreateMat(1, 3, CV_64FC1);
	CvMat* RCameraTranslation  = cvCreateMat(1, 3, CV_64FC1);

	int ErrNum = 0;
	vector<float> LErrEveryStep;
	LErrEveryStep.clear();
	vector<float> RErrEveryStep;
	RErrEveryStep.clear();
	for (UINT i = 0; i < nStep; i++)
	{
		//左相机的R;
		cvmSet(LCameraRotation, 0, 0, *(param+ndex++));
		cvmSet(LCameraRotation, 0, 1, *(param+ndex++));
		cvmSet(LCameraRotation, 0, 2, *(param+ndex++));

		//左相机的T;
		cvmSet(LCameraTranslation, 0, 0, *(param+ndex++));
		cvmSet(LCameraTranslation, 0, 1, *(param+ndex++));
		cvmSet(LCameraTranslation, 0, 2, *(param+ndex++));

		//求取左相机的M (M = [R T]);
		CvMat* LCameraR = cvCreateMat(3, 3, CV_64FC1);
		CvMat* LCameraM = cvCreateMat(4, 4, CV_64FC1);

		cvSetZero(LCameraM);
		cvRodrigues2(LCameraRotation, LCameraR, 0);
		for (UINT j = 0; j < 3; j++)
		{
			cvmSet(LCameraM, j, 0, cvmGet(LCameraR, j, 0));
			cvmSet(LCameraM, j, 1, cvmGet(LCameraR, j, 1));
			cvmSet(LCameraM, j, 2, cvmGet(LCameraR, j, 2));
			cvmSet(LCameraM, j, 3, cvmGet(LCameraTranslation, 0, j));
		}
		cvmSet(LCameraM, 3, 3, 1);

		//求取右相机的R,T;
		CvMat* RCameraM = cvCreateMat(4,4,CV_64FC1);
		CvMat* RCameraR = cvCreateMat(3,3,CV_64FC1);

		cvGEMM(M, LCameraM, 1.0, NULL, 0.0, RCameraM, 0);
		for (UINT j = 0; j < 3; j++)
		{
			cvmSet(RCameraR, j, 0, cvmGet(RCameraM, j, 0));
			cvmSet(RCameraR, j, 1, cvmGet(RCameraM, j, 1));
			cvmSet(RCameraR, j, 2, cvmGet(RCameraM, j, 2));
			cvmSet(RCameraTranslation, 0, j, cvmGet(RCameraM, j, 3));
		}
		cvRodrigues2(RCameraR, RCameraRotation, 0);

		cvReleaseMat(&LCameraR);
		cvReleaseMat(&LCameraM);
		cvReleaseMat(&RCameraR);
		cvReleaseMat(&RCameraM);

		//求取左右相机三维点和对应的二维点;
		vector<cv::Point3f> vObjectPointsReal;
		vector<cv::Point2f> vLImagePointsReal;
		vector<cv::Point2f> vRImagePointsReal;

		cv::Point2f ImagePointTemp;
		for (UINT m = 0; m < PointNum; m++)
		{
			if (pImgL[i][m].x == -1 || pImgR[i][m].x == -1)
			{
				continue;
			}

			ImagePointTemp.x = pImgL[i][m].x;
			ImagePointTemp.y = pImgL[i][m].y;
			vLImagePointsReal.push_back(ImagePointTemp);

			ImagePointTemp.x = pImgR[i][m].x;
			ImagePointTemp.y = pImgR[i][m].y;
			vRImagePointsReal.push_back(ImagePointTemp);

			vObjectPointsReal.push_back(ObjectPoints[m]);
		}

		//将三维点转换成矩阵形式;
		CvMat* mObjectPointsReal = cvCreateMat(vObjectPointsReal.size(),3,CV_64FC1);
		for (UINT j = 0; j < vObjectPointsReal.size(); j++)
		{
			cvmSet(mObjectPointsReal, j, 0, vObjectPointsReal[j].x);
			cvmSet(mObjectPointsReal, j, 1, vObjectPointsReal[j].y);
			cvmSet(mObjectPointsReal, j, 2, vObjectPointsReal[j].z);
		}

		/************************************左相机的反投影残差************************************/

		//求左相机的反投影;
		CvMat* mLImagePoints2 = cvCreateMat(vObjectPointsReal.size(), 2, CV_64FC1);
		cvProjectPoints2( mObjectPointsReal, LCameraRotation, LCameraTranslation, LCameraIntrinsic,
			LCameraDistor, mLImagePoints2, NULL, NULL, NULL, NULL, NULL );

		//求取残差;
		vector<double> LCameraErr;
		double LErr;
		for (UINT j = 0; j < vObjectPointsReal.size(); j++)
		{
			double x = cvmGet(mLImagePoints2,j,0);
			double y = cvmGet(mLImagePoints2,j,1);
			LErr = (x-vLImagePointsReal[j].x)*(x-vLImagePointsReal[j].x)
				+ (y-vLImagePointsReal[j].y)*(y-vLImagePointsReal[j].y);
			LCameraErr.push_back(LErr);
		}

		cvReleaseMat(&mLImagePoints2);

		/************************************右相机的反投影残差************************************/

		//求右相机的反投影;
		CvMat* mRImagePoints2 = cvCreateMat(vObjectPointsReal.size(), 2, CV_64FC1);
		cvProjectPoints2( mObjectPointsReal, RCameraRotation, RCameraTranslation, RCameraIntrinsic,
			RCameraDistor, mRImagePoints2, NULL, NULL, NULL, NULL, NULL );

		//求取残差;
		vector<double> RCameraErr;
		double RErr;
		for (UINT j = 0; j < vObjectPointsReal.size(); j++)
		{
			double x = cvmGet(mRImagePoints2,j,0);
			double y = cvmGet(mRImagePoints2,j,1);
			RErr = (x-vRImagePointsReal[j].x)*(x-vRImagePointsReal[j].x)
				+ (y-vRImagePointsReal[j].y)*(y-vRImagePointsReal[j].y);
			RCameraErr.push_back(RErr);
		}

		cvReleaseMat(&mRImagePoints2);
		cvReleaseMat(&mObjectPointsReal);

		//将左右相机的对应点残差和极限约束值输出;
		float LLErr=0, RRErr=0;
		for (UINT j = 0; j < LCameraErr.size(); j++)
		{
			*(err+ErrNum++) = LCameraErr[j];
			*(err+ErrNum++) = RCameraErr[j];
			LLErr += LCameraErr[j];
			RRErr += RCameraErr[j];
		}
		LErrEveryStep.push_back(LLErr/LCameraErr.size());
		RErrEveryStep.push_back(RRErr/RCameraErr.size());
	}

	cvReleaseMat(&M);
	cvReleaseMat(&LCameraIntrinsic);
	cvReleaseMat(&LCameraRotation);
	cvReleaseMat(&LCameraTranslation);
	cvReleaseMat(&RCameraIntrinsic);
	cvReleaseMat(&RCameraRotation);
	cvReleaseMat(&RCameraTranslation);

	//计算平均误差;
// 	double SumErr = 0;
// 	for (UINT i = 0; i < *m_ptr; i++)
// 	{
// 		SumErr += abs(*(err+i));
// 	}
// 	*fdis = SumErr / *m_ptr;
// 计算每一步的反投影残差;
	ofstream fin("D:\\LRerrz.txt");
	if (fin)
	{
		for (UINT i=0; i<LErrEveryStep.size(); i++)
		{
			fin << LErrEveryStep[i] <<"\t" << RErrEveryStep[i] << endl;
		}

	}
	fin.close();

}

double LMOptimalFor3D::Cal2Dis(cv::Point3f point_1, cv::Point3f point_2)
{
	return sqrt((point_1.x-point_2.x)*(point_1.x-point_2.x)
		+(point_1.y-point_2.y)*(point_1.y-point_2.y)
		+(point_1.z-point_2.z)*(point_1.z-point_2.z));
}

