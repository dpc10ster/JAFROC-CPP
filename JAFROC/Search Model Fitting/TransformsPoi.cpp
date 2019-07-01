#include "stdafx.h"
#include "../common.h"
#include <gsl/gsl_rng.h>
#include <math.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void InverseTransform_all(float * parameters_in, float * parameters_fwd)
{
int i;
float save_parms[MAX_PARAMETERS];

	if (error_sm_fit != 0) return;

	/*
	;; order
	;lambda
	;beta
	;ctffs
	;;
	*/

	for (i=0; i<n_parameters_sm_fit; i++)	save_parms[i] = parameters_in[i];
	
	//both lambda and beta have to be positive, hence similar transformations	
	parameters_fwd[ 0 ] = LAMBDA_MIN+(LAMBDA_MAX-LAMBDA_MIN)*(float)exp(-exp((double)save_parms[0]));// lambda' --> lambda
	parameters_fwd[ 1 ] = BETA_MIN+(BETA_MAX-BETA_MIN)*(float)exp(-exp((double)save_parms[1]));// beta' --> beta
	parameters_fwd[ 2 ] = save_parms[ 2 ];							// zeta_1

	for (i=0;i < R_sm_fit-1;i++) {
		parameters_fwd[3+i] = (float)exp((double)save_parms[3+i]) + parameters_fwd[2+i];
	}

	return ;
}




void ForwardTransform_all(float * parameters_in, float * parameters_fwd)
{
int i;
float save_parms[MAX_PARAMETERS];

	if (error_sm_fit != 0) return;

	/*
	;; order
	;lambda
	;beta
	;ctffs
	;;
	*/


	for (i=0; i<n_parameters_sm_fit; i++)	save_parms[i] = parameters_in[i];
	//both lambda and beta have to be positive, hence similar transformations	
	parameters_fwd[ 0 ] = (float)log(-log(((double)(save_parms[0]-LAMBDA_MIN))/(LAMBDA_MAX-LAMBDA_MIN)));// lambda to lambda'
	parameters_fwd[ 1 ] = (float)log(-log(((double)(save_parms[1]-BETA_MIN))  /(BETA_MAX-BETA_MIN)));// beta to beta'
	parameters_fwd[ 2 ] = save_parms[ 2 ];								// zeta_1

	for (i=0;i<R_sm_fit-1;i++) {
		parameters_fwd[3+i] = (float)log((double)(save_parms[3+i] - save_parms[2+i]));
	}

return;

}



void InverseTransform_mu(float * parameters_fwd, float * parameters_nor)
{
float save_parms;

	if (error_sm_fit != 0) return;
	save_parms = *parameters_fwd;
	*parameters_nor = MU_MIN+(MU_MAX-MU_MIN)*(float)exp(-exp((double)save_parms));// mu' --> mu

	return ;
}




void ForwardTransform_mu(float * parameters_nor, float * parameters_fwd)
{
float save_parms;

	if (error_sm_fit != 0) return;
	save_parms = *parameters_nor;
	*parameters_fwd = (float)log(-log(((double)(save_parms-MU_MIN))/(MU_MAX-MU_MIN)));// mu to mu'

	return;

}




void InverseTransform_lambda_beta(float * parameters_fwd, float * parameters_nor)
{
int i;
float save_parms[2];

	if (error_sm_fit != 0) return;

	/*
	;; order
	;lambda
	;beta
	;;
	*/

	for (i=0; i<2; i++)	save_parms[i] = parameters_fwd[i];
	
	//both lambda and beta have to be positive, hence similar transformations	
	parameters_nor[ 0 ] = LAMBDA_MIN+(LAMBDA_MAX-LAMBDA_MIN)*(float)exp(-exp((double)save_parms[0]));// lambda' --> lambda
	parameters_nor[ 1 ] = BETA_MIN+(BETA_MAX-BETA_MIN)*(float)exp(-exp((double)save_parms[1]));// beta' --> beta

	return ;
}




void ForwardTransform_lambda_beta(float * parameters_nor, float * parameters_fwd)
{
int i;
float save_parms[2];

	if (error_sm_fit != 0) return;

	/*
	;; order
	;lambda
	;beta
	;;
	*/


	for (i=0; i<2; i++)	save_parms[i] = parameters_nor[i];
	//both lambda and beta have to be positive, hence similar transformations	
	parameters_fwd[ 0 ] = (float)log(-log(((double)(save_parms[0]-LAMBDA_MIN))/(LAMBDA_MAX-LAMBDA_MIN)));// lambda to lambda'
	parameters_fwd[ 1 ] = (float)log(-log(((double)(save_parms[1]-BETA_MIN))  /(BETA_MAX-BETA_MIN)));// beta to beta'

	return;

}



void InverseTransform_lambda(float * lambda_fwd, float * lambda_nor)
{
float save_lambda;

	if (error_sm_fit != 0) return;

	save_lambda = *lambda_fwd;
	*lambda_nor = LAMBDA_MIN+(LAMBDA_MAX-LAMBDA_MIN)*(float)exp(-exp((double)save_lambda));// lambda' --> lambda

	return;

}

	


void ForwardTransform_lambda(float * lambda_nor, float * lambda_fwd)
{
float save_lambda;

	if (error_sm_fit != 0) return;

	save_lambda = *lambda_nor;
	*lambda_fwd = (float)log(-log(((double)(save_lambda-LAMBDA_MIN))/(LAMBDA_MAX-LAMBDA_MIN)));// lambda to lambda'

	return;

}


