#include "stdafx.h"
#include "../common.h"
#include <math.h>
//#include <nr.h>
#include "asa243.h"
#include "dcdflib.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/*
#define MAX_F	1e3
#define MAX_DDF	1e3
#define MIN_DDF 5e-1
#define MY_EPS	1e-12
*/
#define MAX_NC	400.0


float es_roc = 0.05f ;
float rocfom_nh, rocfom_ah ;
float jafrocfom_nh, jafrocfom_ah ;
float ss_alpha = 0.05f ;
float desired_power = 0.8f ;

float nh_roc_mu, nh_roc_lambda, nh_roc_nu ;
float ah_roc_mu, ah_roc_lambda, ah_roc_nu ;

int ssd_success ;

int sse_sm_param ;

float Noncentral_T_Dist_interface( float t, float df, float delta )
{
	int ifault ;
	float ret ;

	ret = (float)tnc( (double)t, (double)df, (double)delta, &ifault ) ;

	if( ifault != 0 ) {
		// error on noncentral_t_distribution
	}

	return ret ;
}


float Noncentral_F_Stat_interface( float f_in, float dfn_in, float dfd_in, float np_in )
{
	int which = 1 ;
	double p = 0.0;
	double q = 0.0 ;
	int status = 0 ;
	double bound = 0.0 ;

	double f = (double)f_in ;
	double dfn = (double)dfn_in ;
	double dfd = (double)dfd_in ;
	double np = (double)np_in ;

	cdffnc( &which, &p, &q, &f, &dfn, &dfd, &np, &status, &bound ) ;

	return (float)p ;
}




int Power_Paired_T_Test( float NormalizedEffectSize, float alpha )
{
	int c = 19 ;
	float actual_power = 0.0f ;

	while( actual_power < desired_power && c < MAX_NCASES_80 ) {
		c++ ;
		int df = c - 1 ;
		float nc = NormalizedEffectSize * sqrt( (float)c ) ;
		float T = t_cvf( alpha / 2.0f, df ) ;

		actual_power = 1.0f - Noncentral_T_Dist_interface( T, df, nc ) ;
	}

	return c ;

}



#define	CALC_POWER_RANDOM_ALL		1000
#define CALC_POWER_RANDOM_CASES		1010
#define CALC_POWER_RANDOM_READERS	1020


int what_is_random ;
float EffectSize ;


float calc_power_diff( int MAX_READERS_80, int c1 )					  
{
	float t = 2.0f ;
	float r = (float)MAX_READERS_80 ;
	float c = (float)c1 ;

	float alpha = ss_alpha ;

	float nc, ddf ;

	if( what_is_random == CALC_POWER_RANDOM_ALL ) {
		if( fabs( c * var_tr + r * var_tc + var_trc ) < MY_EPS ) {
			nc = MAX_NC ;
		} else {
			nc = ( ( r * c ) * 0.5f * EffectSize * EffectSize / ( c * var_tr + r * var_tc + var_trc ) ) ;
			if( nc > MAX_NC ) nc = MAX_NC ;
		}
		if( var_tc <= 0.0f ) {
			ddf = ( t - 1.0f ) * ( r - 1.0f ) ;
		} else {
			float num = pow( c * var_tr + r * var_tc + var_trc, 2 ) ;
			float num_den = pow( c * var_tr + var_trc, 2 ) ;
			float den_den = ( t - 1.0f ) * ( r - 1.0f ) ;
			if( fabs( num_den ) < MY_EPS ) {
				ddf = MAX_DDF ;
			} else {
				ddf = ( num / ( num_den / den_den ) ) ;
				if( ddf > MAX_DDF ) ddf = MAX_DDF ;
				if( ddf < MIN_DDF ) ddf = MIN_DDF ;
			}
		}
		float f = f_cvf( alpha, t - 1.0f, ddf ) ;
		float actual_power = 1.0f - Noncentral_F_Stat_interface( f, t - 1.0f, ddf, nc ) ;
		if( ddf < t - 1.0f ) actual_power = 1.0f ;

		return ( desired_power - actual_power ) ;
	}
	else if( what_is_random == CALC_POWER_RANDOM_CASES ) {
		if( fabs( r * var_tc + var_trc ) < MY_EPS ) {
			nc = MAX_NC ;
		} else {
			nc = ( ( r * c ) * 0.5f * EffectSize * EffectSize / ( r * var_tc + var_trc ) ) ;
			if( nc > MAX_NC ) nc = MAX_NC ;
		}
		ddf = ( c - 1.0f ) ;

		float f = f_cvf( alpha, t - 1.0f, ddf ) ;
		float actual_power = 1.0f - Noncentral_F_Stat_interface( f, t - 1.0f, ddf, nc ) ;
		if( ddf < t - 1.0f ) actual_power = 1.0f ;

		return ( desired_power - actual_power ) ;
	}
	else if( what_is_random == CALC_POWER_RANDOM_READERS ) {
		if( fabs( c * var_tr + var_trc ) < MY_EPS ) {
			nc = MAX_NC ;
		} else { 
			nc = ( ( r * c ) * 0.5f * EffectSize * EffectSize / ( c * var_tr + var_trc ) ) ;
			if( nc > MAX_NC ) nc = MAX_NC ;
		}
		ddf = ( r - 1.0f ) ;

		float f = f_cvf( alpha, t - 1.0f, ddf ) ;
		float actual_power = 1.0f - Noncentral_F_Stat_interface( f, t - 1.0f, ddf, nc ) ;
		if( ddf < t - 1.0f ) actual_power = 1.0f ;

		return ( desired_power - actual_power ) ;
	} else {
		// shouldn't be here - error
		return 0.0f ;
	}

	return 0.0f ;
}


int FindNCases80( int x1, int x2, int acc, int MAX_READERS_80 )
{
	float fmid = calc_power_diff( MAX_READERS_80, x2 )	;
	float f = calc_power_diff( MAX_READERS_80, x1 ) ;

	if( f * fmid > 0.0f ) {
		if( f > 0.0f && fmid > 0.0f ) 
			return MAX_NCASES_80 ;
		else
			return MIN_NCASES_80 - 1 ;
	}

	int rtbis ;
	int dx ;

	if( f < 0.0f ) {
		rtbis = x1 ;
		dx = x2 - x1 ;
	} else {
		rtbis = x2 ;
		dx = x1 - x2 ;
	}

	int count = 0, count2 = 0, xmid, xmid_old = 0 ;
	float fmid_old = 0.0f ;
	while( 1 ) {
		count++ ;
		if( count > 200 ) {
			break ;
		}
		int dx2 = (int)( ( (float)dx * 0.5 ) + 0.5 ) ;
		dx = (int)( ( (float)dx * 0.5 ) + 0.5 ) ;
		if( dx >= -1 ) dx = -1 ;
		
		xmid = rtbis + dx ;
		fmid = calc_power_diff( MAX_READERS_80, xmid ) ;

		if( fmid < 0.0f ) rtbis = xmid ;
		if( abs( dx ) <= acc && fabs( fmid ) < 0.01 ) {
			break ;
		}

		if( xmid == xmid_old && fmid == fmid_old ) {
			count2++ ;
			if( count2 >= 10 ) {
				break ;
			}
		} else {
			count2 = 0 ;
			xmid_old = xmid ;
			fmid_old = fmid ;
		}
	}
	return xmid ;

}



int sample_size_calculator( int random, float es, int max_readers_80 )
{
	what_is_random = random ;
	EffectSize = es ;

	return FindNCases80( MIN_NCASES_80, MAX_NCASES_80, 1, max_readers_80 ) ;
}


int sample_size_calculator_bf( int random, float es, int max_reader_80 )
{
	what_is_random = random ;
	EffectSize = es ;

	float min_diff = 1e15f ;
	int opt_ncases_80 ;
	int inc_count = 0 ;

	for( int c1 = 20 ; c1 <= MAX_NCASES_80 ; c1++ ) {
		float diff = fabs( calc_power_diff( max_reader_80, c1 ) ) ;
		if( diff < min_diff ) {
			min_diff = diff ;
			opt_ncases_80 = c1 ;
			inc_count = 0 ;
		} else {
			inc_count++ ;
			if( inc_count > 10 ) break ;
		}
	}

	return opt_ncases_80 ;
}


int *sample_size_modality = NULL ;
int **sample_size_arr = NULL ;
void clear_sample_size_arr( void ) ;

void init_sample_size_arr( void )
{
	clear_sample_size_arr() ;

	sample_size_arr = new int *[ 3 ] ;
	for( int i = 0 ; i < 3 ; i++ ) {
		sample_size_arr[ i ] = new int [ MAX_SS_READERS + 1 ] ;
		for( int j = 0 ; j <= MAX_SS_READERS ; j++ ) {
			sample_size_arr[ i ][ j ] = -2000 ;
		}
	}
	sample_size_modality = new int[ N_TREATMENTS ] ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		sample_size_modality[ i ] = 1 ;
	}
}

void clear_sample_size_arr( void )
{
	if( sample_size_arr == NULL ) return ;

	for( int i = 0 ; i < 3 ; i++ ) {
		delete[] sample_size_arr[ i ] ;
	}
	delete[] sample_size_arr ;

	delete[] sample_size_modality ;

	sample_size_arr = NULL ;
	sample_size_modality = NULL ;
}


float CheckFOM( float var, void *param )
{
	float mu = var ;
	float *p = (float *)param ;
	float lambda = p[ 0 ] ;
	float nu = p[ 1 ] ;
	float target_fom = p[ 2 ] ;

	return ( target_fom - sm_auc( mu, lambda, nu, N_Lesions_Per_Image, MAX_CASES[ 1 ] ) ) ;
}


float FindMu( void *param, float x1, float x2, float acc )
{
	float f = CheckFOM( x1, param ) ;
	float fmid = CheckFOM( x2, param ) ;

	if( f * fmid >= 0.0f ) {
		f = CheckFOM( x2, param ) ;
		return -1.0f ;
	}

	float rtbis, dx ;
	if( f < 0.0f ) {
		rtbis = x1 ;
		dx = x2 - x1 ;
	} else {
		rtbis = x2 ;
		dx = x1 - x2 ;
	}

	int count = 0 ;
	float xmid, xmid_old = 0.0f, fmid_old = 0.0f ;
	
	while( 1 ) {
		count++ ;
		if( count > 500 ) break ;
		dx *= 0.5f ;
		xmid = rtbis + dx ;
		fmid = CheckFOM( xmid, param ) ;
		if( fmid < 0.0f ) rtbis = xmid ;
		if( fabs( dx ) <= acc ) break ;
		xmid_old = xmid ;
		fmid_old = fmid ;
	}
	return xmid ;
}


float JAFROCEffectSize( void )
{
	if( roc_fit_parms == NULL ) {
		sm_roc_ml_fit( "" ) ;
	}
	/*
	// test hjy
	// sm param manual input - to cross validate with IDL
	roc_fit_parms[ 0 ][ 0 ][ 0 ] = 1.09850 ; roc_fit_parms[ 0 ][ 0 ][ 1 ] = 0.930103 ; roc_fit_parms[ 0 ][ 0 ][ 2 ] = 1.84840 ; roc_fit_parms[ 0 ][ 0 ][ 3 ] = 0.820791 ; roc_fit_parms[ 0 ][ 0 ][ 4 ] = 1.13230 ;
	roc_fit_parms[ 0 ][ 1 ][ 0 ] = 0.904495 ; roc_fit_parms[ 0 ][ 1 ][ 1 ] = 0.884586       ; roc_fit_parms[ 0 ][ 1 ][ 2 ] = 2.49763      ; roc_fit_parms[ 0 ][ 1 ][ 3 ] = 0.890230      ; roc_fit_parms[ 0 ][ 1 ][ 4 ] = 0.972888 ;
	roc_fit_parms[ 0 ][ 2 ][ 0 ] = 4.93019       ; roc_fit_parms[ 0 ][ 2 ][ 1 ] = 3.54164       ; roc_fit_parms[ 0 ][ 2 ][ 2 ] = 2.27375      ; roc_fit_parms[ 0 ][ 2 ][ 3 ] = 0.999682       ; roc_fit_parms[ 0 ][ 2 ][ 4 ] = 1.87256 ;
	roc_fit_parms[ 0 ][ 3 ][ 0 ] = 0.708596       ; roc_fit_parms[ 0 ][ 3 ][ 1 ] = 10.9886      ; roc_fit_parms[ 0 ][ 3 ][ 2 ] = 0.913982      ; roc_fit_parms[ 0 ][ 3 ][ 3 ] = 0.999900      ; roc_fit_parms[ 0 ][ 3 ][ 4 ] = 0.889885 ;
	roc_fit_parms[ 0 ][ 4 ][ 0 ] = 0.245850       ; roc_fit_parms[ 0 ][ 4 ][ 1 ] = 1.74332      ; roc_fit_parms[ 0 ][ 4 ][ 2 ] = 0.994693      ; roc_fit_parms[ 0 ][ 4 ][ 3 ] = 0.823435       ; roc_fit_parms[ 0 ][ 4 ][ 4 ] = 1.72088 ;

	roc_fit_parms[ 1 ][ 0 ][ 0 ] = 1.10679       ; roc_fit_parms[ 1 ][ 0 ][ 1 ] = 1.19287       ; roc_fit_parms[ 1 ][ 0 ][ 2 ] = 1.05752      ; roc_fit_parms[ 1 ][ 0 ][ 3 ] = 0.716767       ; roc_fit_parms[ 1 ][ 0 ][ 4 ] = 1.43893 ;
	roc_fit_parms[ 1 ][ 1 ][ 0 ] = 1.26821       ; roc_fit_parms[ 1 ][ 1 ][ 1 ] = 4.19044      ; roc_fit_parms[ 1 ][ 1 ][ 2 ] = 0.952754      ; roc_fit_parms[ 1 ][ 1 ][ 3 ] = 0.981546       ; roc_fit_parms[ 1 ][ 1 ][ 4 ] = 2.15535 ;
	roc_fit_parms[ 1 ][ 2 ][ 0 ] = 2.86714       ; roc_fit_parms[ 1 ][ 2 ][ 1 ] = 1.50618       ; roc_fit_parms[ 1 ][ 2 ][ 2 ] = 1.87215      ; roc_fit_parms[ 1 ][ 2 ][ 3 ] = 0.940382       ; roc_fit_parms[ 1 ][ 2 ][ 4 ] = 2.28937 ;
	roc_fit_parms[ 1 ][ 3 ][ 0 ] = 0.195154       ; roc_fit_parms[ 1 ][ 3 ][ 1 ] = 3.09668      ; roc_fit_parms[ 1 ][ 3 ][ 2 ] = 0.428222      ; roc_fit_parms[ 1 ][ 3 ][ 3 ] = 0.734480       ; roc_fit_parms[ 1 ][ 3 ][ 4 ] = 1.65413 ;
	roc_fit_parms[ 1 ][ 4 ][ 0 ] = 1.20897       ; roc_fit_parms[ 1 ][ 4 ][ 1 ] = 3.95002      ; roc_fit_parms[ 1 ][ 4 ][ 2 ] = 0.838300      ; roc_fit_parms[ 1 ][ 4 ][ 3 ] = 0.963531       ; roc_fit_parms[ 1 ][ 4 ][ 4 ] = 2.36053 ;
	*/

	float mu = 0.0f, lambda = 0.0f, nu = 0.0f ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		if( sample_size_modality[ i ] ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				mu += roc_fit_parms[ i ][ j ][ 2 ] ;
				lambda += roc_fit_parms[ i ][ j ][ 0 ] ;
				nu += roc_fit_parms[ i ][ j ][ 3 ] ;
			}
		}
	}
	float den = 0.0f ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) den += (float)sample_size_modality[ i ] ;
	mu /= (float)( den * MAX_READERS ) ;
	lambda /= (float)( den * MAX_READERS ) ;
	nu /= (float)( den * MAX_READERS ) ;

	sse_sm_param = 1 ;

	// if search model ROC parameter estimation is unrealistic, try JAFROC fit instead
	if( mu >= 5.0f ) {
		if( afroc_fit_parms == NULL ) {
			sm_afroc_ml_fit( "" ) ;
		}
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			if( sample_size_modality[ i ] ) {
				for( int j = 0 ; j < MAX_READERS ; j++ ) {
					mu += afroc_fit_parms[ i ][ j ][ 2 ] ;
					lambda += afroc_fit_parms[ i ][ j ][ 0 ] ;
					nu += afroc_fit_parms[ i ][ j ][ 3 ] ;
				}
			}
		}
		mu /= (float)( den * MAX_READERS ) ;
		lambda /= (float)( den * MAX_READERS ) ;
		nu /= (float)( den * MAX_READERS ) ;

		sse_sm_param = 2 ;	
	}

	// if search model AFROC parameter estimation is unrealistic, try FROC fit instead
	if( mu >= 5.0f ) {
		if( froc_fit_parms == NULL ) {
			sm_froc_ml_fit( "" ) ;
		}
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			if( sample_size_modality[ i ] ) {
				for( int j = 0 ; j < MAX_READERS ; j++ ) {
					mu += froc_fit_parms[ i ][ j ][ 2 ] ;
					lambda += froc_fit_parms[ i ][ j ][ 0 ] ;
					nu += froc_fit_parms[ i ][ j ][ 3 ] ;
				}
			}
		}
		mu /= (float)( den * MAX_READERS ) ;
		lambda /= (float)( den * MAX_READERS ) ;
		nu /= (float)( den * MAX_READERS ) ;

		sse_sm_param = 3 ;
	}

	// if none of them works, return error
	if( mu >= 5.0f ) {
		return -1.0f ;
	}

	nh_roc_mu = mu ;
	nh_roc_lambda = lambda ;
	nh_roc_nu = nu ;

	rocfom_nh = sm_auc( mu, lambda, nu, N_Lesions_Per_Image, MAX_CASES[ 1 ] ) ;
	jafrocfom_nh = jafroc_fom( mu, lambda, nu ) ;

	if( rocfom_nh + es_roc > 1.0f ) {
		es_roc = 0.999f - rocfom_nh ;
	}

	rocfom_ah = rocfom_nh + es_roc ;

	float mu1 = -1.0f ;
//	float lambda1 = lambda * 0.8f ;
//	float nu1 = nu * 1.1f ;
	float lambda1 = lambda * 0.98f ;
	float nu1 = nu * 1.01f ;
	if( nu1 > 0.999999f ) nu1 = 0.999999f ;

	float param[ 3 ] ;
	param[ 0 ] = lambda1 ;
	param[ 1 ] = nu1 ;
	param[ 2 ] = rocfom_ah ;

	int count = 0 ;
//	while( mu1 < 0.0 ) {
	mu1 = FindMu( param, 0.1f, 10.0f, 0.000001f ) ;
	while( mu1 < mu || mu1 > mu * 1.3f ) {
		count++ ;
		if( count > 20 && mu1 < 0.0f ) {
			return -1.0f ;
		}
//		lambda1 = lambda1 * 0.8f ;
//		nu1 = nu1 * 1.1f ;
		lambda1 = lambda1 * 0.98f ;
		nu1 = nu1 * 1.01f ;
		if( nu1 > 0.999999f ) nu1 = 0.999999f ;
		param[ 0 ] = lambda1 ;
		param[ 1 ] = nu1 ;
		mu1 = FindMu( param, 0.1f, 10.0f, 0.000001f ) ;
		if( count == 20 ) {
			break ;
		}
	}

	rocfom_ah = sm_auc( mu1, lambda1, nu1, N_Lesions_Per_Image, MAX_CASES[ 1 ] ) ;
	jafrocfom_ah = jafroc_fom( mu1, lambda1, nu1 ) ;

	ah_roc_mu = mu1 ;
	ah_roc_lambda = lambda1 ;
	ah_roc_nu = nu1 ;

	return fabs( jafrocfom_ah - jafrocfom_nh ) ;
}



float ROCEffectSize( void )
{
	if( roc_fit_parms == NULL ) {
		sm_roc_ml_fit( "" ) ;
	}
	/*
	// test hjy
	// sm param manual input - to cross validate with IDL
	roc_fit_parms[ 0 ][ 0 ][ 0 ] = 1.09850 ; roc_fit_parms[ 0 ][ 0 ][ 1 ] = 0.930103 ; roc_fit_parms[ 0 ][ 0 ][ 2 ] = 1.84840 ; roc_fit_parms[ 0 ][ 0 ][ 3 ] = 0.820791 ; roc_fit_parms[ 0 ][ 0 ][ 4 ] = 1.13230 ;
	roc_fit_parms[ 0 ][ 1 ][ 0 ] = 0.904495 ; roc_fit_parms[ 0 ][ 1 ][ 1 ] = 0.884586       ; roc_fit_parms[ 0 ][ 1 ][ 2 ] = 2.49763      ; roc_fit_parms[ 0 ][ 1 ][ 3 ] = 0.890230      ; roc_fit_parms[ 0 ][ 1 ][ 4 ] = 0.972888 ;
	roc_fit_parms[ 0 ][ 2 ][ 0 ] = 4.93019       ; roc_fit_parms[ 0 ][ 2 ][ 1 ] = 3.54164       ; roc_fit_parms[ 0 ][ 2 ][ 2 ] = 2.27375      ; roc_fit_parms[ 0 ][ 2 ][ 3 ] = 0.999682       ; roc_fit_parms[ 0 ][ 2 ][ 4 ] = 1.87256 ;
	roc_fit_parms[ 0 ][ 3 ][ 0 ] = 0.708596       ; roc_fit_parms[ 0 ][ 3 ][ 1 ] = 10.9886      ; roc_fit_parms[ 0 ][ 3 ][ 2 ] = 0.913982      ; roc_fit_parms[ 0 ][ 3 ][ 3 ] = 0.999900      ; roc_fit_parms[ 0 ][ 3 ][ 4 ] = 0.889885 ;
	roc_fit_parms[ 0 ][ 4 ][ 0 ] = 0.245850       ; roc_fit_parms[ 0 ][ 4 ][ 1 ] = 1.74332      ; roc_fit_parms[ 0 ][ 4 ][ 2 ] = 0.994693      ; roc_fit_parms[ 0 ][ 4 ][ 3 ] = 0.823435       ; roc_fit_parms[ 0 ][ 4 ][ 4 ] = 1.72088 ;

	roc_fit_parms[ 1 ][ 0 ][ 0 ] = 1.10679       ; roc_fit_parms[ 1 ][ 0 ][ 1 ] = 1.19287       ; roc_fit_parms[ 1 ][ 0 ][ 2 ] = 1.05752      ; roc_fit_parms[ 1 ][ 0 ][ 3 ] = 0.716767       ; roc_fit_parms[ 1 ][ 0 ][ 4 ] = 1.43893 ;
	roc_fit_parms[ 1 ][ 1 ][ 0 ] = 1.26821       ; roc_fit_parms[ 1 ][ 1 ][ 1 ] = 4.19044      ; roc_fit_parms[ 1 ][ 1 ][ 2 ] = 0.952754      ; roc_fit_parms[ 1 ][ 1 ][ 3 ] = 0.981546       ; roc_fit_parms[ 1 ][ 1 ][ 4 ] = 2.15535 ;
	roc_fit_parms[ 1 ][ 2 ][ 0 ] = 2.86714       ; roc_fit_parms[ 1 ][ 2 ][ 1 ] = 1.50618       ; roc_fit_parms[ 1 ][ 2 ][ 2 ] = 1.87215      ; roc_fit_parms[ 1 ][ 2 ][ 3 ] = 0.940382       ; roc_fit_parms[ 1 ][ 2 ][ 4 ] = 2.28937 ;
	roc_fit_parms[ 1 ][ 3 ][ 0 ] = 0.195154       ; roc_fit_parms[ 1 ][ 3 ][ 1 ] = 3.09668      ; roc_fit_parms[ 1 ][ 3 ][ 2 ] = 0.428222      ; roc_fit_parms[ 1 ][ 3 ][ 3 ] = 0.734480       ; roc_fit_parms[ 1 ][ 3 ][ 4 ] = 1.65413 ;
	roc_fit_parms[ 1 ][ 4 ][ 0 ] = 1.20897       ; roc_fit_parms[ 1 ][ 4 ][ 1 ] = 3.95002      ; roc_fit_parms[ 1 ][ 4 ][ 2 ] = 0.838300      ; roc_fit_parms[ 1 ][ 4 ][ 3 ] = 0.963531       ; roc_fit_parms[ 1 ][ 4 ][ 4 ] = 2.36053 ;
	*/

	float mu = 0.0f, lambda = 0.0f, nu = 0.0f ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		if( sample_size_modality[ i ] ) {
			for( int j = 0 ; j < MAX_READERS ; j++ ) {
				mu += roc_fit_parms[ i ][ j ][ 2 ] ;
				lambda += roc_fit_parms[ i ][ j ][ 0 ] ;
				nu += roc_fit_parms[ i ][ j ][ 3 ] ;
			}
		}
	}
	float den = 0.0f ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) den += (float)sample_size_modality[ i ] ;
	mu /= (float)( den * MAX_READERS ) ;
	lambda /= (float)( den * MAX_READERS ) ;
	nu /= (float)( den * MAX_READERS ) ;

	sse_sm_param = 1 ;

	// if search model ROC parameter estimation is unrealistic, try JAFROC fit instead
	if( mu >= 5.0f ) {
		if( afroc_fit_parms == NULL ) {
			sm_afroc_ml_fit( "" ) ;
		}
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			if( sample_size_modality[ i ] ) {
				for( int j = 0 ; j < MAX_READERS ; j++ ) {
					mu += afroc_fit_parms[ i ][ j ][ 2 ] ;
					lambda += afroc_fit_parms[ i ][ j ][ 0 ] ;
					nu += afroc_fit_parms[ i ][ j ][ 3 ] ;
				}
			}
		}
		mu /= (float)( den * MAX_READERS ) ;
		lambda /= (float)( den * MAX_READERS ) ;
		nu /= (float)( den * MAX_READERS ) ;

		sse_sm_param = 2 ;	
	}

	// if search model AFROC parameter estimation is unrealistic, try FROC fit instead
	if( mu >= 5.0f ) {
		if( froc_fit_parms == NULL ) {
			sm_froc_ml_fit( "" ) ;
		}
		for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
			if( sample_size_modality[ i ] ) {
				for( int j = 0 ; j < MAX_READERS ; j++ ) {
					mu += froc_fit_parms[ i ][ j ][ 2 ] ;
					lambda += froc_fit_parms[ i ][ j ][ 0 ] ;
					nu += froc_fit_parms[ i ][ j ][ 3 ] ;
				}
			}
		}
		mu /= (float)( den * MAX_READERS ) ;
		lambda /= (float)( den * MAX_READERS ) ;
		nu /= (float)( den * MAX_READERS ) ;

		sse_sm_param = 3 ;
	}

	// if none of them works, return error
	if( mu >= 5.0f ) {
		return -1.0f ;
	}

	nh_roc_mu = mu ;
	nh_roc_lambda = lambda ;
	nh_roc_nu = nu ;

	rocfom_nh = sm_auc( mu, lambda, nu, N_Lesions_Per_Image, MAX_CASES[ 1 ] ) ;

	if( rocfom_nh + es_roc > 1.0f ) {
		es_roc = 0.999f - rocfom_nh ;
	}

	rocfom_ah = rocfom_nh + es_roc ;

	float mu1 = mu = -1.0f ;
	float lambda1 = lambda * 0.8f ;
	float nu1 = nu * 1.1f ;
	if( nu1 > 0.999999f ) nu1 = 0.999999f ;

	float param[ 3 ] ;
	param[ 0 ] = lambda1 ;
	param[ 1 ] = nu1 ;
	param[ 2 ] = rocfom_ah ;

	int count = 0 ;
	while( mu1 < 0.0f ) {
		mu1 = FindMu( param, 0.1f, 10.0f, 0.000001f ) ;
		if( mu1 < 0.0f ) {
			count++ ;
			if( count > 5 ) return -1.0f ;
			lambda1 = lambda1 * 0.8f ;
			nu1 = nu1 * 1.1f ;
			if( nu1 > 0.999999f ) nu1 = 0.999999f ;
		}
	}

	rocfom_ah = sm_auc( mu1, lambda1, nu1, N_Lesions_Per_Image, MAX_CASES[ 1 ] ) ;

	ah_roc_mu = mu1 ;
	ah_roc_lambda = lambda1 ;
	ah_roc_nu = nu1 ;

	return es_roc ;
}


int SampleSizeMultiReader( void )
{
	if( sample_size_arr == NULL )
		init_sample_size_arr() ;

	int sum = 0 ;
	for( int k = 0 ; k < N_TREATMENTS ; k++ ) {
		sum += sample_size_modality[ k ] ;
	}
	if( sum == 0 ) {
		for( int k = 0 ; k < N_TREATMENTS ; k++ ) {
			sample_size_modality[ k ] = 1 ;
		}
	}

	float es ;
	
	switch( analysis_method ) {
		case ANALYSIS_METHOD_JAFROC1:
		case ANALYSIS_METHOD_JAFROC:
			es = JAFROCEffectSize() ;
			break ;
		case ANALYSIS_METHOD_HR_ROC:
			es = ROCEffectSize() ;
			break ;
		case ANALYSIS_METHOD_JAFROC1_WEIGHTED:
		case ANALYSIS_METHOD_JAFROC_WEIGHTED:
			es = JAFROCEffectSize() ;
			break ;
		case ANALYSIS_METHOD_SONG_FOM_A1:
		case ANALYSIS_METHOD_SONG_FOM_A2:
			es = ROCEffectSize() ;
			break ;
	}
	
	ssd_success = 1 ;
	if( es < 0.0f ) {
		ssd_success = 0 ;
		return 1 ;
	}

	int what_is_random = CALC_POWER_RANDOM_ALL ;
	int i = 0 ;
	for( int j = 2 ; j <= MAX_SS_READERS ; j++ ) {
		sample_size_arr[ i ][ j ] = sample_size_calculator( what_is_random, es, j ) ;
//		if( sample_size_arr[ i ][ j ] <= MIN_NCASES_80 ) break ;
	}

	what_is_random = CALC_POWER_RANDOM_CASES ;
	i = 1 ;
	for( int j = 2 ; j <= MAX_SS_READERS ; j++ ) {
		sample_size_arr[ i ][ j ] = sample_size_calculator( what_is_random, es, j ) ;
//		if( sample_size_arr[ i ][ j ] <= MIN_NCASES_80 ) break ;
	}

	what_is_random = CALC_POWER_RANDOM_READERS ;
	i = 2 ;
	for( int j = 2 ; j <= MAX_SS_READERS ; j++ ) {
		sample_size_arr[ i ][ j ] = sample_size_calculator( what_is_random, es, j ) ;
//		if( sample_size_arr[ i ][ j ] <= MIN_NCASES_80 ) break ;
	}

	return 0 ;
}


int SampleSizeSingleReader( void )
{
	if( sample_size_arr == NULL )
		init_sample_size_arr() ;

//	float es = JAFROCEffectSize() ;
	float es ;

	switch( analysis_method ) {
		case ANALYSIS_METHOD_JAFROC1:
		case ANALYSIS_METHOD_JAFROC:
			es = JAFROCEffectSize() ;
			break ;
		case ANALYSIS_METHOD_HR_ROC:
			es = ROCEffectSize() ;
			break ;
		case ANALYSIS_METHOD_JAFROC1_WEIGHTED:
		case ANALYSIS_METHOD_JAFROC_WEIGHTED:
			es = JAFROCEffectSize() ;
			break ;
		case ANALYSIS_METHOD_SONG_FOM_A1:
		case ANALYSIS_METHOD_SONG_FOM_A2:
			es = ROCEffectSize() ;
			break ;
	}

	ssd_success = 1 ;
	if( es < 0.0f ) {
		ssd_success = 0 ;
		return 1 ;
	}

	sample_size_arr[ 1 ][ 0 ] = Power_Paired_T_Test( es, ss_alpha ) ;

	return 0 ;
}



void SampleSizeReport( FILE *stream )
{
	fprintf( stream, "================================================================================\n" ) ;

	int hit = 0 ;
	for( int i = 0 ; i < N_TREATMENTS ; i++ ) {
		for( int j = 0 ; j < MAX_READERS ; j++ ) {
			switch( sse_sm_param ) {
			case 1:
				if( n_parameters_ROC[ i ][ j ] == 3 ) hit = 1 ;
				break ;
			case 2:
				if( n_parameters_AFROC[ i ][ j ] == 3 ) hit = 1 ;
				break ;
			case 3:
				if( n_parameters_FROC[ i ][ j ] == 3 ) hit = 1 ;
				break; 
			}
		}
	}

	if( sse_sm_param > 1 ) {
		if( sse_sm_param == 2 ) {
			fprintf( stream, " Note: Search model parameters were derived from fitting the AFROC data.\n" ) ;
			if( hit )
				fprintf( stream, " Warning: Some readers yielded only one AFROC operating point, which may result in unreliable estimates of SM parameters.\n" ) ;
		}
		if( sse_sm_param == 3 ) {
			fprintf( stream, " Note: Search model parameters were derived from fitting the FROC data.\n" ) ;
			if( hit )
				fprintf( stream, " Warning: Some readers yielded only one FROC operating point, which may result in unreliable estimates of SM parameters.\n" ) ;
		}
		fprintf( stream, "\n" ) ;
	} else {
		if( hit ) {
			fprintf( stream, " Warning: Some readers yielded only one ROC operating point,\n" ) ;
			fprintf( stream, "          which may result in unreliable estimates of parameters.\n" ) ;
		}
		fprintf( stream, "\n" ) ;
	}

	fprintf( stream, "                            SAMPLE SIZE ESTIMATION\n\n" ) ;
	fprintf( stream, "NH ROC FOM: %g\n", rocfom_nh ) ;
	fprintf( stream, "AH ROC FOM: %g\n", rocfom_ah ) ;
	fprintf( stream, "ROC Effect Size: %g\n\n", rocfom_ah - rocfom_nh ) ;

	if( analysis_method != ANALYSIS_METHOD_HR_ROC && 
		analysis_method != ANALYSIS_METHOD_SONG_FOM_A1 && 
		analysis_method != ANALYSIS_METHOD_SONG_FOM_A2 ) {
		fprintf( stream, "NH JAFROC FOM: %g\n", jafrocfom_nh ) ;
		fprintf( stream, "AH JAFROC FOM: %g\n", jafrocfom_ah ) ;
		fprintf( stream, "JAFROC Effect Size: %g\n\n", jafrocfom_ah - jafrocfom_nh ) ;
	}

	fprintf( stream, "var_tr: %g\n", var_tr ) ;
	fprintf( stream, "var_tc: %g\n", var_tc ) ;
	fprintf( stream, "var_trc: %g\n\n", var_trc ) ;

	fprintf( stream, "Alpha: %g\n", ss_alpha ) ;
	fprintf( stream, "Desired power: %g\n\n", desired_power ) ;

	fprintf( stream, "Search Model parameters\n" ) ;
	fprintf( stream, "NH mu    : %g\n", nh_roc_mu ) ;
	fprintf( stream, "NH lambda: %g\n", nh_roc_lambda ) ;
	fprintf( stream, "NH nu    : %g\n\n", nh_roc_nu ) ;

	fprintf( stream, "AH mu    : %g\n", ah_roc_mu ) ;
	fprintf( stream, "AH lambda: %g\n", ah_roc_lambda ) ;
	fprintf( stream, "AH nu    : %g\n\n", ah_roc_nu ) ;

	fprintf( stream, " Prediction\n" ) ;
	if( MAX_READERS > 1 ) {
		for( int i = 0 ; i < 3 ; i++ ) {
			switch( i ) {
			case 0:
				fprintf( stream, " Random All\n" ) ;
				break ;
			case 1:
				fprintf( stream, " Random Cases\n" ) ;
				break ;
			case 2:
				fprintf( stream, " Random Readers\n" ) ;
				break ;
			}
			for( int j = 2 ; j <= MAX_SS_READERS ; j++ ) {
//				if( sample_size_arr[ i ][ j ] < MAX_NCASES_80 && sample_size_arr[ i ][ j ] >= MIN_NCASES_80 ) {

					int si = j ;
					int k ;
					for( k = j + 1 ; k <= MAX_SS_READERS ; k++ ) {
						if( sample_size_arr[ i ][ j ] != sample_size_arr[ i ][ k ] ) break ;
					}
					int ei = k ;
					if( sample_size_arr[ i ][ j ] >= MAX_NCASES_80 ) {
						if( ei - 1 > 2 ) fprintf( stream, "# Readers: ~%d\t\t# Cases: >= %d\n", ei - 1, MAX_NCASES_80 ) ;
						else fprintf( stream, "# Readers: %d\t\t# Cases: >= %d\n", ei - 1, MAX_NCASES_80 ) ;
					} else if( sample_size_arr[ i ][ j ] < MIN_NCASES_80 ) {
						fprintf( stream, "# Readers: %d~\t\t# Cases: < %d\n", si, MIN_NCASES_80 ) ;
					} else {
						if( ei - si > 1 ) { 
							fprintf( stream, "# Readers: %d~%d\t# Cases: %d\n", si, ei - 1, sample_size_arr[ i ][ j ] ) ;
						} else {
							fprintf( stream, "# Readers: %d\t\t# Cases: %d\n", j, sample_size_arr[ i ][ j ] ) ;
						}
					}
					j = ei - 1 ;
//				}
			}
			fprintf( stream, "\n" ) ;
		}
	} else {
		if( sample_size_arr[ 0 ][ 0 ] < MAX_NCASES_80 && sample_size_arr[ 0 ][ 0 ] >= MIN_NCASES_80 ) {
			fprintf( stream, "Number of cases: %d\n", sample_size_arr[ 0 ][ 0 ] ) ;
		} else {
			fprintf( stream, "Number of cases: Unable to find.\r\n" ) ;
		}
		fprintf( stream, "\n" ) ;
	}

	WriteToConsole( "Sample Size Estimation reported.\r\n" ) ;
}



void SSG_write( float mu, float lambda, float nu, FILE *stream )
{
	int N_PTS = 200 ;
	float delta_zeta = ( 6.0f + 4.0f ) / (float)( N_PTS ) ;
	float zeta = -4.0f ;

	for( int k = 0 ; k < N_PTS ; k++ ) {
		float fpf = fpf_poi( zeta, lambda ) ;
		float tpf = tpf_poi( zeta, mu, lambda, nu ) ;
		fprintf( stream, "%g, %g\n", fpf, tpf ) ;
		zeta += delta_zeta ;
	}
}


bool SampleSizeGraph( char *basename )
{
	char filename[ 200 ], str[ 200 ] ;
	FILE *stream ;

	sprintf( filename, "%s_SampleSize_NH_ROC.csv", basename ) ;
	stream = fopen( filename, "wt" ) ;
	if( stream == NULL ) return true ;

	SSG_write( nh_roc_mu, nh_roc_lambda, nh_roc_nu, stream ) ;

	fclose( stream ) ;

	sprintf( str, "%s Saved\r\n", filename ) ;
	WriteToConsole( str ) ;

	sprintf( filename, "%s_SampleSize_AH_ROC.csv", basename ) ;
	stream = fopen( filename, "wt" ) ;
	if( stream == NULL ) return true ;

	SSG_write( ah_roc_mu, ah_roc_lambda, ah_roc_nu, stream ) ;

	fclose( stream ) ;

	sprintf( str, "%s Saved\r\n\r\n", filename ) ;
	WriteToConsole( str ) ;

	return false ;
}