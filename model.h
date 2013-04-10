#ifndef	_MODEL_H
#define	_MODEL_H

#include "constants.h"
#include "dataset.h"
#include "utils.h"
#include "database.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <boost/math/special_functions/digamma.hpp>

using namespace std;
using namespace boost::math;

// LDA model
class model {
public:
    vector<pair<string, int> > movie_classes;
    string movie_classes_trn_file;// file containing movie classes
    string movie_classes_pre_file;
    string wordmapfile;		// file that contains word map [string -> integer id]
    string trainlogfile;	// training log file
    string tassign_suffix;	// suffix for topic assignment file
    string theta_suffix;	// suffix for theta file
    string phi_suffix;		// suffix for phi file
    string others_suffix;	// suffix for file containing other parameters
    string twords_suffix;	// suffix for file containing words-per-topics

    string dir;			    // model directory
    string dfile;		    // data file    
    string model_name;		// model name
    string originDir;       // directory needs be be preprocessed

    int file_type;
    int model_status;		// model status:
				            // MODEL_STATUS_UNKNOWN: unknown status
				            // MODEL_STATUS_EST: estimating from scratch
                            // MODEL_STATUS_ESTC: continue to estimate the model from a previous one
				            // MODEL_STATUS_INF: do inference
                            // MODEL_STATUS_PREPROCESS: preprocess texts
                            // MODEL_STATUS_RANKING: ranking
                            // MODEL_STATUS_CLASSIFIER
                            // MODEL_STATUS_SERVER

    dataset * ptrndata;	    // pointer to training dataset object
    dataset * pnewdata;     // pointer to new dataset object

    mapid2word id2word;     // word map [int => string]
    
    // --- model parameters and variables ---    
    int M;                  // dataset size (i.e., number of docs)
    int V;                  // vocabulary size
    int K;                  // number of topics
    double alpha, beta;     // LDA hyperparameters 
    int niters;             // number of Gibbs sampling iterations
    int liter;              // the iteration at which the model was saved
    int savestep;           // saving period
    int twords;             // print out top words per each topic
    int withrawstrs;
    int disp;
    int rank_num;

    double * p;             // temp variable for sampling
    int ** z;               // topic assignments for words, size M x doc.size()
    int ** nw;              // cwt[i][j]: number of instances of word/term i assigned to topic j, size V x K
    int ** nd;              // na[i][j]: number of words in document i assigned to topic j, size M x K
    int * nwsum;            // nwsum[j]: total number of words assigned to topic j, size K
    int * ndsum;            // ndsum[i]: total number of words in document i, size M
    double ** theta;        // theta: document-topic distributions, size M x K
    double ** phi;          // phi: topic-word distributions, size K x V
    
    // for inference only
    int inf_liter;
    int newM;
    int newV;
    int ** newz;
    int ** newnw;
    int ** newnd;
    int * newnwsum;
    int * newndsum;
    double ** newtheta;
    double ** newphi;
    // --------------------------------------
    
    model() {
	   set_default_values();
    }
          
    ~model();
    
    // set default values for variables
    void set_default_values();   
    
    // initialize the model
    int init();
    int textpreprocessor();
    
    // load LDA model to continue estimating or to do inference
    int load_model(string model_name);
    
    // save LDA model to files
    // model_name.tassign: topic assignments for words in docs
    // model_name.theta: document-topic distributions
    // model_name.phi: topic-word distributions
    // model_name.others: containing other parameters of the model (alpha, beta, M, V, K)
    int save_model(string model_name);
    int save_model_tassign(string filename);
    int save_model_theta(string filename);
    int save_model_phi(string filename);
    int save_model_others(string filename);
    int save_model_twords(string filename);
    
    // saving inference outputs
    int save_inf_model(string model_name);
    int save_inf_model_tassign(string filename);
    int save_inf_model_newtheta(string filename);
    int save_inf_model_newphi(string filename);
    int save_inf_model_others(string filename);
    int save_inf_model_twords(string filename);
    
    // init for estimation
    int init_est();
    int init_estc();
	
    // estimate LDA model using Gibbs sampling
    void estimate();
    int sampling(int m, int n);
    void compute_theta();
    void compute_phi();
    void compute_alpha();
    void compute_beta();
    
    // init for inference
    int init_inf();
    // inference for new (unseen) data based on the estimated LDA model
    void inference();
    int inf_sampling(int m, int n);
    void compute_newtheta();
    void compute_newphi();

    void preprocess();

    int init_ranking();
    void ranking();
    vector<int> ranking(vector<int> candidate);

    void classification();
};

#endif

