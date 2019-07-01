#include "stdafx.h"
#include "JAFROC_CORE_DEF.h"
#include <math.h>

/*
// note that the studentized range is written in FORTRAN
extern "C" {
	double studentized_range( double Q, double V, double R, int *ERR ) ;
}

float studrange_dll( float Q, float V, float R )
{
	int err ;
	return (float)studentized_range( (double)Q, (double)V, (double)R, &err ) ;
}

*/
double prtrng( double q, double v, double r, int *err ) ;

float studrange( float Q, float V, float R )
{
	int err ;
	return (float)prtrng( (double)Q, (double)V, (double)R, &err ) ;
}

float inv_studrange( float alpha, float df, float treatments )
{
	float q = 4.7f ;
	int dir = 1 ;
	float step = 0.1f ;
	q = q - step * dir ; // to undo the first step, so it starts out at the right spot
	float min_val = 100.0f ;
	float best_q = 1.0f ;

	do {	// vary q
	    q = q + step * dir ;
		float min_new = fabs( ( studrange( q, df, treatments ) - ( 1.0f - alpha ) ) ) ;
		if( min_new < min_val ) {
			min_val = min_new ;
			best_q = q ;
		} else {
			q = best_q ;	// undo the last step
			step = step / 2.0f ;
			dir = dir * ( -1 ) ;
		}
	} while( step > 0.001f ) ;

	return best_q ;
}



// translation of studrange.for

double alnorm( double x, bool upper ) ;

double prtrng( double q, double v, double r, int *err )
{
	double pcutj = 0.00003, pcutk = 0.0001, step = 0.45, vmax = 120.0, zero = 0.0, fifth = 0.2,
		half = 0.5, one = 1.0, two = 2.0, cv1 = 0.193064705, cv2 = 0.293525326,
		cvmax = 0.39894228, cv[ 4 ] = { 0.318309886, -0.268132716e-2, 0.347222222e-2, 0.833333333e-1 } ;
	int jmin = 3, jmax = 15, kmin = 7, kmax = 15 ;

	double prob = zero ;
	int ifault = 0 ;
	if( v < one || r < two ) ifault = 1 ;
	if( q <= zero || ifault == 1) {
		*err = ifault ;
		return 0.0 ;
	}

	double g = step * pow( r, (-fifth) ) ;
	double gmid = half * log( r ) ;
	double r1 = r - one ;
	double c = log( r * g * cvmax ) ;
	double h, v2 ;
	double vw[ 30 ], qw[ 30 ] ;
	if( v <= vmax ) {
		h = step * pow( v, (-half) ) ;
		v2 = v * half ;
		if( v == one ) c = cv1 ;
		if( v == two ) c = cv2 ;
		if( !( v == one || v == two ) ) c = sqrt( v2 ) * cv[ 0 ] / ( one + ( ( cv[ 1 ] / v2 + cv[ 2 ] ) / v2 + cv[ 3 ] ) / v2) ;
		c = log( c * r * g * h ) ;
	}
	double gstep = g ;
	qw[ 0 ] = -one ;
	qw[ jmax ] = -one ;
	double pk1 = one ;
	double pk2 = one ;
	double gk, pk, w0, pz, x, jump, ehj, pj ;
	int jj ; 
	double hj ;
	for( int k = 1 ; k <= kmax ; k++ ) {
		gstep = gstep - g ;
		// 21
		do {
			gstep = -gstep ;
			gk = gmid + gstep ;
			pk = zero ;
			if( !( pk2 <= pcutk && k > kmin ) ) {
				w0 = c - gk * gk * half ;
				pz = alnorm( gk, true ) ;
				x = alnorm( gk - q, true ) - pz ;
				if( x > zero ) pk = exp( w0 + r1 * log( x ) ) ;
				if( !( v > vmax ) ) {
					jump = -jmax ;
					do {
						// 22
						jump = jump + jmax ;
						for( int j = 1 ; j <= jmax ; j++ ) {
							jj = j + jump ;
							if( !(qw[jj-1] > zero) ) {
								hj = h * j ;
								if( j < jmax ) qw[ jj ] = -one ;
								ehj = exp( hj ) ;
								qw[ jj - 1 ] = q * ehj ;
								vw[ jj - 1 ] = v * ( hj + half - ehj * ehj * half ) ;
							}
							// 23
							pj = zero ;
							x = alnorm( gk - qw[ jj - 1 ], true ) - pz ;
							if( x > zero ) pj = exp( w0 + vw[ jj - 1 ] + r1 * log(x) ) ;
							pk = pk + pj ;
//							if( pj > pcutj ) CYCLE ;
//							if( jj > jmin || k > kmin ) EXIT ;
						}
						h = -h ;
					} while( h < zero ) ;
				}
			}
			// 26
			prob = prob + pk ;
			if( k > kmin && pk <= pcutk && pk1 <= pcutk) {
				*err = ifault ;
				return prob ;
			}
			pk2 = pk1 ;
			pk1 = pk ;
		} while( gstep > zero ) ;
	}
	return prob ;
}

/*
double qtrng( double *p, double *v, double *r )
{
	double q1, p1, q2, p2, d, e1, e2 ;
	int ifault, j, nfault, jmax = 8 ;
	double pcut = 0.001, p75 = 0.75, p80 = 0.80, p90 = 0.9, p99 = 0.99, p995 = 0.995,
		p175 = 1.75, one = 1.0, two = 2.0, five = 5.0, eps = 1.0e-04 ;

	ifault = 0 ;
	nfault = 0 ;
	if( *v < one || *r < two ) ifault = 1 ;
	if( *p < p90 || *p > p99 ) ifault = 2 ;
	if( ifault != 0 ) {
		return 0.0 ;
	}

	q1 = qtrng0( p, v, r ) ;
	p1 = prtrng( q1, v, r, err ) ;
IF (nfault /= 0) GO TO 99
quantile = q1
IF (ABS(p1-p) < pcut) GO TO 99
IF (p1 > p) p1 = p175 * p - p75 * p1
IF (p1 < p) p2 = p + (p - p1) * (one - p) / (one - p1) * p75
IF (p2 < p80) p2 = p80
IF (p2 > p995) p2 = p995
q2 = qtrng0(p2, v, r)
IF (nfault /= 0) GO TO 99

!        Refine approximation

DO  j = 2, jmax
  p2 = prtrng(q2, v, r, err)
  IF (nfault /= 0) GO TO 99
  e1 = p1 - p
  e2 = p2 - p
  quantile = (q1 + q2) / two
  d = e2 - e1
  IF (ABS(d) > eps) quantile = (e2 * q1 - e1 * q2) / d
  IF(ABS(e1) < ABS(e2)) GO TO 12
  q1 = q2
  p1 = p2
  12   IF (ABS(p1 - p) < pcut * five) GO TO 99
  q2 = quantile
END DO

99 IF (nfault /= 0) ifault = 9
!IF (ifault /= 0) WRITE(*, '(a, i4, a)') ' IFAULT = ', ifault, ' from QTRNG'
IF (ifault /= 0) ERR = ifault
RETURN
END FUNCTION qtrng



FUNCTION qtrng0(p, v, r) RESULT(initq)

! N.B. Argument IFAULT has been removed.

REAL (dp), INTENT(IN)  :: p
REAL (dp), INTENT(IN)  :: v
REAL (dp), INTENT(IN)  :: r
REAL (dp)              :: initq

!  Algorithm AS 190.2  Appl. Statist. (1983) Vol.32, No.2

!  Calculates an initial quantile p for a studentized range distribution
!  having v degrees of freedom and r samples for probability p, 0.8 < p < 0.995

!  Uses function ppnd - Algorithm AS 241

INTEGER    :: ifault
REAL (dp)  :: q, t
REAL (dp), PARAMETER  :: vmax = 120.0_dp, half = 0.5_dp, one = 1.0_dp,  &
                         four = 4.0_dp, c1 = 0.8843_dp, c2 = 0.2368_dp, &
                         c3 = 1.214_dp, c4 = 1.208_dp, c5 = 1.4142_dp

CALL ppnd16(half + half * p, t, ifault)
IF (v < vmax) t = t + (t * t* t + t) / v / four
q = c1 - c2 * t
IF (v < vmax) q = q - c3 / v + c4 * t / v
initq = t * (q * LOG(r - one) + c5)

RETURN
END FUNCTION qtrng0


*/


double alnorm( double x, bool upper )
{
	double fn_val ;
	double zero = 0.0, one = 1.0, half = 0.5, con = 1.28 ;
	double z, y ;
	bool up ;
	double ltone = 7.0, utzero = 18.66 ;
	double p = 0.398942280444, q = 0.39990348504,
		r = 0.398942280385, a1 = 5.75885480458,
		a2 = 2.62433121679, a3 = 5.92885724438,
		b1 = -29.8213557807, b2 = 48.6959930692,
		c1 = -3.8052E-8, c2 = 3.98064794E-4,
		c3 = -0.151679116635, c4 = 4.8385912808,
		c5 = 0.742380924027, c6 = 3.99019417011,
		d1 = 1.00000615302, d2 = 1.98615381364,
		d3 = 5.29330324926, d4 = -15.1508972451,
		d5 = 30.789933034 ;

	up = upper ;
	z = x ;
	if( !( z >=  zero ) ) {
		if( up ) up = false ;
		else up = true ;
		z = -z ;
	}
	if( !( z <= ltone || up && z <= utzero ) ) {
		fn_val = zero ;
		if( !up ) fn_val = one - fn_val ;
		return fn_val ;
	}
	y = half * z * z ;
	if( z <= con ) {
		fn_val = half - z*(p-q*y/(y+a1+b1/(y+a2+b2/(y+a3)))) ;
		if( !up ) fn_val = one - fn_val ;
		return fn_val ;
	}
	fn_val = r*exp(-y)/(z+c1+d1/(z+c2+d2/(z+c3+d3/(z+c4+d4/(z+c5+d5/(z+c6)))))) ;
	if( !up ) fn_val = one - fn_val ;

	return fn_val ;
}


/*
SUBROUTINE ppnd16 (p, normal_dev, ifault)

! ALGORITHM AS241  APPL. STATIST. (1988) VOL. 37, NO. 3

! Produces the normal deviate Z corresponding to a given lower
! tail area of P; Z is accurate to about 1 part in 10**16.

! The hash sums below are the sums of the mantissas of the
! coefficients.   They are included for use in checking
! transcription.

! This ELF90-compatible version by Alan Miller - 20 August 1996
! N.B. The original algorithm is as a function; this is a subroutine

REAL (dp), INTENT(IN)   :: p
INTEGER, INTENT(OUT)    :: ifault
REAL (dp), INTENT(OUT)  :: normal_dev

! Local variables

REAL (dp) :: zero = 0.d0, one = 1.d0, half = 0.5d0, split1 = 0.425d0,  &
             split2 = 5.d0, const1 = 0.180625d0, const2 = 1.6d0, q, r

! Coefficients for P close to 0.5

REAL (dp) :: a0 = 3.3871328727963666080D0, &
             a1 = 1.3314166789178437745D+2, &
             a2 = 1.9715909503065514427D+3, &
             a3 = 1.3731693765509461125D+4, &
             a4 = 4.5921953931549871457D+4, &
             a5 = 6.7265770927008700853D+4, &
             a6 = 3.3430575583588128105D+4, &
             a7 = 2.5090809287301226727D+3, &
             b1 = 4.2313330701600911252D+1, &
             b2 = 6.8718700749205790830D+2, &
             b3 = 5.3941960214247511077D+3, &
             b4 = 2.1213794301586595867D+4, &
             b5 = 3.9307895800092710610D+4, &
             b6 = 2.8729085735721942674D+4, &
             b7 = 5.2264952788528545610D+3
! HASH SUM AB    55.8831928806149014439

! Coefficients for P not close to 0, 0.5 or 1.

REAL (dp) :: c0 = 1.42343711074968357734D0, &
             c1 = 4.63033784615654529590D0, &
             c2 = 5.76949722146069140550D0, &
             c3 = 3.64784832476320460504D0, &
             c4 = 1.27045825245236838258D0, &
             c5 = 2.41780725177450611770D-1, &
             c6 = 2.27238449892691845833D-2, &
             c7 = 7.74545014278341407640D-4, &
             d1 = 2.05319162663775882187D0, &
             d2 = 1.67638483018380384940D0, &
             d3 = 6.89767334985100004550D-1, &
             d4 = 1.48103976427480074590D-1, &
             d5 = 1.51986665636164571966D-2, &
             d6 = 5.47593808499534494600D-4, &
             d7 = 1.05075007164441684324D-9
! HASH SUM CD    49.33206503301610289036

! Coefficients for P near 0 or 1.

REAL (dp) :: e0 = 6.65790464350110377720D0, &
             e1 = 5.46378491116411436990D0, &
             e2 = 1.78482653991729133580D0, &
             e3 = 2.96560571828504891230D-1, &
             e4 = 2.65321895265761230930D-2, &
             e5 = 1.24266094738807843860D-3, &
             e6 = 2.71155556874348757815D-5, &
             e7 = 2.01033439929228813265D-7, &
             f1 = 5.99832206555887937690D-1, &
             f2 = 1.36929880922735805310D-1, &
             f3 = 1.48753612908506148525D-2, &
             f4 = 7.86869131145613259100D-4, &
             f5 = 1.84631831751005468180D-5, &
             f6 = 1.42151175831644588870D-7, &
             f7 = 2.04426310338993978564D-15
! HASH SUM EF    47.52583317549289671629

ifault = 0
q = p - half
IF (ABS(q) <= split1) THEN
  r = const1 - q * q
  normal_dev = q * (((((((a7*r + a6)*r + a5)*r + a4)*r + a3)*r + a2)*r + a1)*r + a0) / &
           (((((((b7*r + b6)*r + b5)*r + b4)*r + b3)*r + b2)*r + b1)*r + one)
  RETURN
ELSE
  IF (q < zero) THEN
    r = p
  ELSE
    r = one - p
  END IF
  IF (r <= zero) THEN
    ifault = 1
    normal_dev = zero
    RETURN
  END IF
  r = SQRT(-LOG(r))
  IF (r <= split2) THEN
    r = r - const2
    normal_dev = (((((((c7*r + c6)*r + c5)*r + c4)*r + c3)*r + c2)*r + c1)*r + c0) / &
             (((((((d7*r + d6)*r + d5)*r + d4)*r + d3)*r + d2)*r + d1)*r + one)
  ELSE
    r = r - split2
    normal_dev = (((((((e7*r + e6)*r + e5)*r + e4)*r + e3)*r + e2)*r + e1)*r + e0) / &
             (((((((f7*r + f6)*r + f5)*r + f4)*r + f3)*r + f2)*r + f1)*r + one)
  END IF
  IF (q < zero) normal_dev = - normal_dev
  RETURN
END IF
RETURN
END SUBROUTINE ppnd16

END FUNCTION prtrng



SUBROUTINE STUDRANGE(Q,V,R,RET,ERR)
!DEC$ ATTRIBUTES C :: STUDRANGE
!DEC$ ATTRIBUTES REFERENCE:: Q,V,R,RET,ERR

DOUBLE PRECISION Q,V,R,RET
INTEGER ERR

RET = prtrng(Q,V,R,ERR)
RETURN

END

*/