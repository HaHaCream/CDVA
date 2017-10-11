//============================================================================
// Name        : cdva_eval_mex.cpp
// Author      : MB
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <vector>
#include "mex.h"
#include "CSVReader.h"

using namespace std;
using namespace mpeg7cdva;

#if defined(_WIN32) || defined(_WIN64) 
#define strcasecmp _stricmp // use _stricmp() in Windows 
#endif


static const char * descrLengths[] = {"16K", "64K", "256K", "16K_256K"};


// shared global variables

vector<retrieval_t> truth;					///< define the retrieval ground truth
vector<retrieval_t> ignore_gt;				///< define the ignore groud truth
timing_map_t timing_map; 					///< map of timing data for each query

int n_truth, n_ignore, n_timing;			///< size of ground truth lists



/*
 * @class Evaluation
 * container class for evaluation of results.
 */
class Evaluation {
private:

	/**
	 * R-Precision (using ignore list)
	 * @param truth ground truth
	 * @param results ranked lists of results
	 * @param ignore matches to be ignored
	 * @param n number of records (queries)
	 * @return the mean average precision value
	 */
	static double r_precision_ignore (const vector<retrieval_t> & truth, const vector<retrieval_t> & results, const vector<retrieval_t> & ignore, int n)
	{
	  int relevant, queries = 0;
	  double r_precision, MRP = 0.0;
	  int i, j, k;
	  int num_ignored;	// either 1 or 0

	  /* scan records: */
	  for (i=0; i<n; i++) {
	    /* sanity check: */
	    if (!strcasecmp(truth[i].query, results[i].query)) {  // _strcasecmp ignores case in comparison
	      relevant = 0;
	      r_precision = 0.;
	      num_ignored = 0;
	      /* scan results (up to R elements) */
	      for (j=0; j<truth[i].n_matches; j++) {
	        /* scan ground truth */
	        for (k=0; k<truth[i].n_matches; k++) {
	          if (!strcasecmp(truth[i].matches[k], results[i].matches[j]))
	          {
	        	  if ((ignore[i].n_matches > 0) && (!strcasecmp(ignore[i].matches[0], results[i].matches[j])))
	        		  num_ignored = 1;
	        	  else
					  relevant ++;

				  break;	// found!
	          }
	        }
	      }

	      if ((truth[i].n_matches - num_ignored) > 0)
	      {
	    	  r_precision = (double) relevant / ((double) (truth[i].n_matches - num_ignored));
	    	  MRP += r_precision;
	          queries ++;
	      }
	    }
	  }

	  /* normalize and return MRP value: */
	  if (queries) MRP /= (double) queries;
	  return MRP;
	}




	/**
	 * Mean Average Precision (using ignore list)
	 * @param truth ground truth
	 * @param results ranked lists of results
	 * @param ignore matches to be ignored
	 * @param n number of records (queries)
	 * @return the mean average precision value
	 */
	static double mean_average_precision_ignore (const vector<retrieval_t> & truth, const vector<retrieval_t> & results, const vector<retrieval_t> & ignore, int n)
	{
	  int relevant, queries = 0;
	  double average_precision, MAP = 0.0;
	  int i, j, k;
	  int num_ignored;	// either 1 or 0

	  /* scan records: */
	  for (i=0; i<n; i++) {
	    /* sanity check: */
	    if (!strcasecmp(truth[i].query, results[i].query)) {  // _strcasecmp ignores case in comparison
	      relevant = 0;
	      average_precision = 0.;
	      num_ignored = 0;
	      /* scan results	*/
	      for (j=0; j<results[i].n_matches; j++) {
	        /* scan ground truth */
	        for (k=0; k<truth[i].n_matches; k++) {
	          if (!strcasecmp(truth[i].matches[k], results[i].matches[j]))
	          {
	    		  relevant ++;

	        	  if ((ignore[i].n_matches > 0) && (!strcasecmp(ignore[i].matches[0], results[i].matches[j])))
	        		  num_ignored = 1;
	        	  else
					  average_precision += (double)relevant / (double) (j + 1); // j+1 = rank

				  break;	// found!
	          }
	        }
	      }
	      /* normalize average precision and add it to MAP:  */
	      if ((truth[i].n_matches - num_ignored) > 1) average_precision /= (truth[i].n_matches - num_ignored);
	      MAP += average_precision;
	      queries ++;
	    }
	  }

	  /* normalize and return MAP value: */
	  if (queries) MAP /= (double) queries;
	  return MAP;
	}



	/**
	 * Mean Average Precision (simplified version)
	 * @param truth ground truth
	 * @param results ranked lists of results
	 * @param n number of records (queries)
	 * @return the mean average precision value
	 */
	static double mean_average_precision (const vector<retrieval_t> & truth, const vector<retrieval_t> & results, int n)
	{
	  int relevant, queries = 0;
	  double average_precision, MAP = 0.0;
	  int i, j, k;

	  /* scan records: */
	  for (i=0; i<n; i++) {
	    /* sanity check: */
	    if (!strcasecmp(truth[i].query, results[i].query)) {  // _strcasecmp ignores case in comparison
	      relevant = 0;
	      average_precision = 0.;
	      /* scan results	*/
	      for (j=0; j<results[i].n_matches; j++) {
	        /* scan ground truth */
	        for (k=0; k<truth[i].n_matches; k++) {
	          if (!strcasecmp(truth[i].matches[k], results[i].matches[j])) {
	            /* ground truth match found */
	            relevant ++;
	            average_precision += (double)relevant / (double) (j+1); // j+1 = rank
	            break;
	          }
	        }
	      }
	      /* normalize average precision and add it to MAP:  */
	      if (truth[i].n_matches) average_precision /= truth[i].n_matches;
	      MAP += average_precision;
	      queries ++;
	    }
	  }

	  /* normalize and return MAP value: */
	  if (queries) MAP /= (double) queries;
	  return MAP;
	}


public:

	static void check(const vector <retrieval_t> & data, const vector <retrieval_t> & truth, int numData, int numTruth)
	{
		if (numData != numTruth)
			cerr << "warning: results and ground truth have different size: " << numData << " - " << numTruth << endl;

		int sameQuery = 0;

		for (int k=0; k<min(numData, numTruth); ++k)
		{
			// the query must be the same!
			if (!strcasecmp(truth[k].query, data[k].query))
				++sameQuery;
		}

		if (sameQuery < numData)
			cout << numData - sameQuery << " missing queries" << endl;
		else
			cout << "ok" << endl;

	}




	static void print(const vector <retrieval_t> & data) {
		cout << "id, query, references[]" << endl;

		for (int i=0; i<50; ++i)
		{
			cout << i + 1 << ":" << data[i].query ;
			for (int k=0; k<data[i].n_matches; ++k)
			{
				cout << ", " << data[i].matches[k];
			}
			cout << endl;
		}
	}

public:
	bool verbose;				// verbose mode (more checks are performed in this mode)
	int n_queries;				// size of queries
	int n_references;			// size of references
	int n_retrieval;			// size of retrieval
	int n_matchingPairs;		// size of matching pairs
	int n_nonMatchingPairs;		// size of non-matching pairs
	double avQueryLen;			// average query length
	double maxQueryLen;			// max query length
	double avReferenceLen;		// average reference length
	double maxReferenceLen;		// max reference length
	double MAP;					// mean average precision
	double r_precision;			// R-Precision
	double TPR;					// true positive rate
	double jaccard;				// Jaccard index

	vector<descriptor_t> queries;			// query descriptors
	vector<descriptor_t> references;		// reference descriptors
	vector<retrieval_t> retrieval;			// retrieval results
	vector<pair_t> matchingPairs;			// matching pair results
	vector<pair_t> nonMatchingPairs;		// matching pair results

	Evaluation(bool verboseMode = false) {
		verbose = verboseMode;
		n_queries = n_references = n_retrieval = n_matchingPairs = n_nonMatchingPairs = 0;
		avQueryLen = maxQueryLen = avReferenceLen = maxReferenceLen = MAP = r_precision = TPR = jaccard = 0.0;
	}
    
    virtual ~Evaluation() {}        // destructor

	void computeMAP(const char * retrievalFileName) {
		if (verbose)
		{
			cout << "Reading CSV file: " << retrievalFileName << endl;
			CSVReader retrievalReader(retrievalFileName);
			n_retrieval = retrievalReader.read(retrieval);
			cout << "checking data consistency... " << endl;
			cout << "	with ground truth: ";
			check(retrieval, truth, n_retrieval, n_truth);
			cout << "	with ignore list: ";
			check(retrieval, ignore_gt, n_retrieval, n_ignore);
			cout << "computing mean average precision on " << n_retrieval << " retrieval results...";
			MAP =  mean_average_precision_ignore (truth, retrieval, ignore_gt, n_retrieval);
			r_precision = r_precision_ignore (truth, retrieval, ignore_gt, n_retrieval);
			cout << " done." << endl << endl;
		}
		else
		{
			CSVReader retrievalReader(retrievalFileName);
			n_retrieval = retrievalReader.read(retrieval);
			MAP =  mean_average_precision_ignore (truth, retrieval, ignore_gt, n_retrieval);
			r_precision = r_precision_ignore (truth, retrieval, ignore_gt, n_retrieval);
		}
	}


	int computeQueryLen(const char * queryFileName) {
		CSVReader queryReader(queryFileName);
		n_queries = queryReader.read(queries);

		double sum = 0;
		double max = 0;
		int skipped = 0;

		for (int k=0; k<n_queries; ++k)
		{
			if (queries[k].time > 0)
			{
				double mysize =  queries[k].descriptorSize / queries[k].time;
				sum += mysize;
				if (mysize > max)
					max = mysize;
			}
			else
				++skipped;
		}

		avQueryLen = sum / (n_queries - skipped);
		maxQueryLen = max;
		return skipped;
	}

	int computeReferenceLen(const char * referenceFileName) {
		CSVReader refReader(referenceFileName);
		n_references = refReader.read(references);

		double sum = 0;
		double max = 0;
		int skipped = 0;

		for (int k=0; k<n_references; ++k)
		{
			if (references[k].time > 0)
			{
				double mysize =  references[k].descriptorSize / references[k].time;
				sum += mysize;
				if (mysize > max)
					max = mysize;
			}
			else
				++skipped;
		}

		avReferenceLen = sum / (n_references - skipped);
		maxReferenceLen = max;
		return skipped;
	}

	/*
	 * order MatchData elements by descending score
	 */
	static bool byDescendingScore(const pair_t & m1, const pair_t & m2) {
		return (m1.score > m2.score);
	}

	void computeTPR(const char * matchingPairsFile, const char * nonMatchingPairsFile) {
		CSVReader mpReader(matchingPairsFile);
		CSVReader nmpReader(nonMatchingPairsFile);
		n_matchingPairs = mpReader.read(matchingPairs);
		n_nonMatchingPairs = nmpReader.read(nonMatchingPairs);

		// TPR at 1% FPR - sort non patching pairs to get the top 1% values
		int one_percent = (n_nonMatchingPairs / 100) + 1;

		// partially sort non matching pairs by descending score
		partial_sort(nonMatchingPairs.begin(), nonMatchingPairs.begin() + one_percent, nonMatchingPairs.end(), byDescendingScore);
		double threshold = nonMatchingPairs[one_percent - 1].score;

		// TPR = TP / P
		// Jaccard index = intersections / unions

		int TP = 0;
		jaccard = 0.0;
		int jcounter = 0;

		for (int k=0; k<n_matchingPairs; ++k)
		{
			if (matchingPairs[k].score > threshold)
			{
				++TP;
			}

			// compute Jaccard index

			timing_map_t::iterator iter = timing_map.find(matchingPairs[k].queryName);
			if (iter != timing_map.end()) {
				timing_t myIntersect, myUnion;
				myIntersect.start = max(matchingPairs[k].firstMatchTime, iter->second.start);
				myIntersect.end = min(matchingPairs[k].lastMatchTime, iter->second.end);
				myUnion.start = min(matchingPairs[k].firstMatchTime, iter->second.start);
				myUnion.end = max(matchingPairs[k].lastMatchTime, iter->second.end);

				if (myIntersect.end > myIntersect.start)	// there is an intersection
				{
					jaccard += ((myIntersect.end - myIntersect.start) / (myUnion.end - myUnion.start));
				}
				// else jaccard += 0;	// therefore, nothing to do!

				jcounter++;
			}
		}	// end for cycle

		TPR = (double) TP / ((double) n_matchingPairs);
		jaccard /= jcounter; // average Jaccard index

	}	// end computeTPR method

};


typedef struct {
    bool setVerbose;
    bool setSimple;
    bool skipRetrieval;
    char csvPath[256];
} param_t;


/*
 *  The evaluation is controlled by the following parameters:
 *
 *	bool setVerbose = false;			// verbose mode
	bool setSimple = false;				// perform only 16K operations
	bool skipRetrieval = false;			// ignore the retrieval results
 */
int run_cdva_eval (vector<Evaluation> & eval, const param_t & params)
{
//	cout << "Reading CSV files...";
    
	int  n_op = 3;
	int  n_opmix = 4;

	int qskip[3];
	int rskip[3];

	for (int k=0; k<3; ++k)
		qskip[k] = rskip[k] = 0;	// initialize skipped queries and references

	if (params.setSimple)
	{
		n_op = 1;
		n_opmix = 1;
	}

    if (params.setVerbose)
        for (int k=0; k<n_opmix; ++k)
            eval[k].verbose = true;
    
    string runpath = params.csvPath;

	CSVReader truthReader("Retrieval.GT.csv");
	CSVReader ignoreReader("Ignore.GT.csv");
	CSVReader timingReader("Timing.GT.csv");

	n_truth = truthReader.read(truth);
	n_ignore = ignoreReader.read(ignore_gt);
	n_timing = timingReader.read(timing_map);

	if (params.setSimple)
	{
		qskip[0] = eval[0].computeQueryLen((runpath + "/Queries.16K.csv").c_str());
		rskip[0] = eval[0].computeReferenceLen((runpath + "/References.16K.csv").c_str());
		eval[0].computeTPR((runpath + "/MatchingPairs.16K.csv").c_str(), (runpath + "/NonMatchingPairs.16K.csv").c_str());
		if (!params.skipRetrieval) eval[0].computeMAP((runpath + "/Retrieval.16K.csv").c_str());
	}
	else	// full computation
	{
		qskip[0] = eval[0].computeQueryLen((runpath + "/Queries.16K.csv").c_str());
		qskip[1] = eval[1].computeQueryLen((runpath + "/Queries.64K.csv").c_str());
		qskip[2] = eval[2].computeQueryLen((runpath + "/Queries.256K.csv").c_str());

		rskip[0] = eval[0].computeReferenceLen((runpath + "/References.16K.csv").c_str());
		rskip[1] = eval[1].computeReferenceLen((runpath + "/References.64K.csv").c_str());
		rskip[2] = eval[2].computeReferenceLen((runpath + "/References.256K.csv").c_str());

		eval[0].computeTPR((runpath + "/MatchingPairs.16K.csv").c_str(), (runpath + "/NonMatchingPairs.16K.csv").c_str());
		eval[1].computeTPR((runpath + "/MatchingPairs.64K.csv").c_str(), (runpath + "/NonMatchingPairs.64K.csv").c_str());
		eval[2].computeTPR((runpath + "/MatchingPairs.256K.csv").c_str(), (runpath + "/NonMatchingPairs.256K.csv").c_str());
		eval[3].computeTPR((runpath + "/MatchingPairs.16K_256K.csv").c_str(), (runpath + "/NonMatchingPairs.16K_256K.csv").c_str());

		if (!params.skipRetrieval)
		{
			eval[0].computeMAP((runpath + "/Retrieval.16K.csv").c_str());
			eval[1].computeMAP((runpath + "/Retrieval.64K.csv").c_str());
			eval[2].computeMAP((runpath + "/Retrieval.256K.csv").c_str());
		}
	}

	//	print warnings

	for (int k=0; k<3; ++k)
	{
		if (qskip[k]) printf("WARNING: %d query descriptors at %s were skipped because the reported video duration time was null\n", qskip[k], descrLengths[k]);
	}
	for (int k=0; k<3; ++k)
	{
		if (rskip[k]) printf("WARNING: %d reference descriptors at %s were skipped because the reported video duration time was null\n", rskip[k], descrLengths[k]);
	}

	printf("\n");

	// print results

	printf("%53.53s ", "Descriptor lengths (Bps):");         for (int k=0; k<n_op; ++k) printf("%10.10s ", descrLengths[k]); printf("\n");
	printf("%53.53s ", "Query average lengths:");            for (int k=0; k<n_op; ++k) printf("%10.2f ", eval[k].avQueryLen);  printf("\n");
	printf("%53.53s ", "Query max lengths:");                for (int k=0; k<n_op; ++k) printf("%10.2f ", eval[k].maxQueryLen); printf("\n");
	printf("%53.53s ", "Reference average lengths:");        for (int k=0; k<n_op; ++k) printf("%10.2f ", eval[k].avReferenceLen);  printf("\n");
	printf("%53.53s ", "Reference max lengths:");            for (int k=0; k<n_op; ++k) printf("%10.2f ", eval[k].maxReferenceLen); printf("\n");
	printf("\n\n");

	if (!params.skipRetrieval)
	{
		// Retrieval
		printf("%53.53s ", "Retrieval performance at:");	for (int k=0; k<n_op; ++k) printf("%10.10s ", descrLengths[k]); printf("\n");
		printf("%53.53s ", "Mean average precision:");		for (int k=0; k<n_op; ++k) printf("%10.3f ", eval[k].MAP);  printf("\n");
		printf("%53.53s ", "r-Precision:");					for (int k=0; k<n_op; ++k) printf("%10.3f ", eval[k].r_precision);  printf("\n");
		printf("\n");
	}

	// Pairwise matching
	printf("%53.53s ", "Pairwise matching performance at:");             for (int k=0; k<n_opmix; ++k) printf("%10.10s", descrLengths[k]); printf("\n");
	printf("%53.53s ", "True positive rate at 1% false positive rate:"); for (int k=0; k<n_opmix; ++k) printf("%10.3f", eval[k].TPR); printf("\n");
	printf("%53.53s ", "Mean Jaccard index for temporal localisation:"); for (int k=0; k<n_opmix; ++k) printf("%10.3f", eval[k].jaccard); printf("\n");

	return 0;
}




#ifdef DEBUGGING


int main() {
	param_t params;

	params.setVerbose = false;
	params.setSimple = true;
	params.skipRetrieval = false;
	sprintf(params.csvPath, "%s", "../run");

	vector<Evaluation> eval;
	eval.resize(4);

	try {
		 run_cdva_eval (eval, params);    // run evaluation
	}
	catch(exception & ex)				// catch any exception
	{
		cout << "An exception occurred. " <<  ex.what() << endl;
	}

	eval.clear();
}

#else

void loadParameters(const mxArray *pm, param_t & options)
{
    /* check options */
    if (mxGetClassID(pm) != mxSTRUCT_CLASS){
            mexErrMsgIdAndTxt("MATLAB:cdvs:argin",
                "Input options must be a STRUCT.");
    }

    /* get data from MATLAB struct and assign values to Parameters */
    
    mxLogical * mxverbose = (mxLogical *) mxGetData(mxGetField(pm, 0, "setVerbose"));
    mxLogical * mxsimple = (mxLogical *) mxGetData(mxGetField(pm, 0, "setSimple"));
    mxLogical * mxskip = (mxLogical *) mxGetData(mxGetField(pm, 0, "skipRetrieval"));
    
    options.setVerbose = (bool) *mxverbose;
    options.setSimple  = (bool) *mxsimple;
    options.skipRetrieval = (bool) *mxskip;
    
    mxGetString(mxGetField(pm, 0, "csvPath"), options.csvPath, mxGetN(mxGetField(pm, 0, "csvPath"))+1);
}


void mexFunction(int           nOutput,    /* number of output arguments */
                 mxArray       *Output[],  /* array of output arguments */
                 int           nInput,     /* number of input arguments */
                 const mxArray *Input[]    /* array of input arguments */
                )
{
    /* enumerate the input and the output */
    enum {INPUT_PARAM=0, INPUT_END};
    enum {OUTPUT_DATA=0, OUTPUT_END};
    
    /* check for proper number of arguments */
    if (nInput > INPUT_END){
        mexErrMsgIdAndTxt("MATLAB:cdvs:nargin", 
            "Too many input arguments.");
    } else if (nOutput > OUTPUT_END){
        mexErrMsgIdAndTxt("MATLAB:cdvs:nargout",
            "Too many output arguments.");
    }
        
    param_t params;
    params.setVerbose = false;                  // default value
    params.setSimple = false;                   // default value
    params.skipRetrieval = false;               // default value
    sprintf(params.csvPath, "%s", "../run");    // default value
           
    if (nInput > INPUT_PARAM){
        loadParameters(Input[INPUT_PARAM], params);
    }
    
    vector<Evaluation> eval;
    eval.resize(4);
    
    try {
         run_cdva_eval (eval, params);    // run evaluation
    }
    catch(exception & ex)				// catch any exception
    {
        cout << "An exception occurred. " <<  ex.what() << endl;
    }
    
    if (nOutput > 0)
    {
        /* Set output struct */
        const int nFields = 8;
        const char *fieldnames[nFields];
        fieldnames[0]="QueryAverageLen";
        fieldnames[1]="QueryMaxLen";
        fieldnames[2]="ReferenceAverageLen";
        fieldnames[3]="ReferenceMaxLen";
        fieldnames[4]="MeanAveragePrecision";
        fieldnames[5]="R_Precision";
        fieldnames[6]="TruePositiveRate";
        fieldnames[7]="MeanJaccardIndex";
        
        int nres = 4;       // four results
        if (params.setSimple)
            nres = 1;       // one result

        Output[OUTPUT_DATA] = mxCreateStructMatrix(1,nres,nFields,fieldnames);

        mxArray *pm = Output[OUTPUT_DATA];

        /* create mxArray data structures to hold the data to be assigned for the structure. */
        
        for (int k=0; k<nres; ++k)
        {

            mxArray * avQueryLen_0      = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL);
            mxArray * maxQueryLen_1     = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL);
            mxArray * avReferenceLen_2  = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL);
            mxArray * maxReferenceLen_3 = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL);
            mxArray * MAP_4             = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL);
            mxArray * r_precision_5     = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL);
            mxArray * TPR_6             = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL);
            mxArray * jaccard_7         = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL);


            /* fill with the values */
            *(mxGetPr(avQueryLen_0))              = eval[k].avQueryLen;
            *(mxGetPr(maxQueryLen_1))             = eval[k].maxQueryLen;
            *(mxGetPr(avReferenceLen_2))          = eval[k].avReferenceLen;
            *(mxGetPr(maxReferenceLen_3))         = eval[k].maxReferenceLen;
            *(mxGetPr(MAP_4))                     = eval[k].MAP;
            *(mxGetPr(r_precision_5))             = eval[k].r_precision;
            *(mxGetPr(TPR_6))                     = eval[k].TPR;
            *(mxGetPr(jaccard_7))                 = eval[k].jaccard;


            /* assign the field values */
            mxSetFieldByNumber(pm,k,0, avQueryLen_0);     // average query length
            mxSetFieldByNumber(pm,k,1, maxQueryLen_1);    // max query length
            mxSetFieldByNumber(pm,k,2, avReferenceLen_2); // average reference length
            mxSetFieldByNumber(pm,k,3, maxReferenceLen_3);// max reference length
            mxSetFieldByNumber(pm,k,4, MAP_4);            // mean average precision
            mxSetFieldByNumber(pm,k,5, r_precision_5);    // R-Precision
            mxSetFieldByNumber(pm,k,6, TPR_6);            // true positive rate
            mxSetFieldByNumber(pm,k,7, jaccard_7);        // Jaccard index
            
        }   // end for
    }   // end if
     
}

#endif
