//
//  MyStrings.h
//  JAFROC
//
//  Created by Dev Chakraborty on 10/12/12.
//  Copyright (c) 2012 University of Pittsburgh. All rights reserved.
//

#ifndef JAFROC_MyStrings_h
#define JAFROC_MyStrings_h

char * DisclaimerText =
"JAFROC SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, \
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE \
AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, \
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT \
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n\n";

char * HelpStrOnBnClickedAnalysisJafroc =
"JAFROC: empirical probability that lesion rating exceeds highest \
rated non-lesion rating on normal images; comparisons are made between \
lesion ratings and highest rated non-lesion rating on normal images \
only.This implementation does not follow the 2004 Medical Physics \
definition. NH behaviour has been tested.\r\n\r\n";

char * HelpStrOnBnClickedAnalysisJafroc1 =
"JAFROC1: empirical probability that lesion rating exceeds highest \
rated non-lesion rating; comparisons are made between lesion ratings \
and highest rated non-lesion rating on ALL images; unmarked lesions \
gets the -2000 rating, and any image with no non-lesions gets a -2000 \
highest non-lesion rating. Use of this figure of merit is recommended \
only if there are no normal cases. This implementation does not follow \
the 2004 Medical Physics definition. NH behaviour has been tested.\r\n\r\n";

char * HelpStrOnBnClickedAnalysisRoc =
"Inferred ROC: Uses highest rating (or -2000 if there are no marks) to \
define the inferred ROC rating; then computes area under empirical ROC \
curve via Wilcoxon statistic.\r\n\r\n";

char * HelpStrOnMnCheckedHrSensitivity = 
"This uses as FOM the highest rating derived ROC sensitivity counting all marks.\r\n\r\n";

char * HelpStrOnMnCheckedHrSpecificity = 
"This uses as FOM the highest rating derived ROC specificity counting all marks.\r\n\r\n";

char * HelpStrOnBnClickedAnalysisJafroc1Weighted =
"JAFROC1 Weighted: this follows the 2004 Medical Physics definition. \
Weights are picked up from Truth sheet; must use Excel input format \
with this.\r\n\r\n";

char * HelpStrOnBnClickedAnalysisJafrocWeighted =
"JAFROC Weighted: this follows the 2004 Medical Physics definition. \
Weights are picked up from Truth sheet; must use Excel input format \
with this.\r\n\r\n";

char * helpStrOnBnClickedAnalysisSongFomA1 =
"Song A1: See paper by Song et al, Medical Physics, 35:1547, 2008. This \
averages the ratings to get the inferred ROC. The figure of merit is \
the area under the empirical ROC.\r\n\r\n";

char * helpStrOnBnClickedAnalysisSongFomA1Sensitivity = 
	"";

char * helpStrOnBnClickedAnalysisSongFomA1Specificity = 
	"";

char * helpStrOnBnClickedAnalysisSongFomA2Sensitivity = 
	"";

char * helpStrOnBnClickedAnalysisSongFomA2Specificity = 
	"";

char * helpStrOnBnClickedAnalysisSongFomA0 =
"Song A0: See paper by Song et al, Medical Physics, 35:1547, 2008. This \
should yield identical results as Inferred ROC above.\r\n\r\n";

char * helpStrOnBnClickedAnalysisSongFomA2 =
"Song A2: See paper by Song et al, Medical Physics, 35:1547, 2008. This \
uses a \"stochastic dominance\" concept to define the area under an \
inferred ROC.\r\n\r\n";

char *helpStrANALYSIS_METHOD_ROI =
"ROI: Nancy Obuchowski's Region of Interest Figure of Merit; every \
image must get an equal number of ratings corresponding to a fixed \
number of regions of interest per image; software checks for this and \
an error is generated if this condition is not met; the figure of \
merit is the empirical area under the ROC constructed by assuming all \
lesion containing regions are abnormal images and all lesion absent \
regions are normal image.\r\n\r\n";

char * HelpStrOnBnClickedAnalysisMaxNLF =
"This uses maximum NLF (non-lesion localization fraction) as the figure of merit.\r\n\r\n";


char * HelpStrOnBnClickedAnalysisMaxLLF =
"This uses maximum LLF (lesion localization fraction) as the figure of merit.\r\n\r\n";

char * HelpStrOnMnCheckedExpTransformSpecificityAnalysis = 
"This uses as FOM the maximum inferred specificity using the exponential transformation suggested by Popescu.\r\n\r\n";

char * HelpStrOnMnCheckedFrocCustomAnalysis =
"Custom analysis, only for Dr. Chakraborty and Xuetong Zhai.\r\n\r\n";

char * helpStrNoNormalCases =
"Date set has no normal cases.\r\n";


char *helpStrNotAnROIFile =
"Data file does not fit the requirements of an ROI data file.\r\n\r\n";

char *versionStr = "JAFROC Version 4.2\r\n";

#endif
