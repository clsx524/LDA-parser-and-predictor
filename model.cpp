#include "constants.h"
#include "strtokenizer.h"
#include "utils.h"
#include "dataset.h"
#include "model.h"

using namespace std;

model::~model() {
    if (p) { delete p; }
    
    if (ptrndata) { delete ptrndata; }
    
    if (pnewdata) { delete pnewdata; }
    
    if (z) {
        for (int m = 0; m < M; m++) {
            if (z[m]) {
                delete z[m];
            }
        }
    }
    
    if (nw) {
        for (int w = 0; w < V; w++) {
            if (nw[w]) {
                delete nw[w];
            }
        }
    }
    
    if (nd) {
        for (int m = 0; m < M; m++) {
            if (nd[m]) {
                delete nd[m];
            }
        }
    }
    
    if (nwsum) {
        delete nwsum;
    }
    
    if (ndsum) {
        delete ndsum;
    }
    
    if (theta) {
        for (int m = 0; m < M; m++) {
            if (theta[m]) {
                delete theta[m];
            }
        }
    }
    
    if (phi) {
        for (int k = 0; k < K; k++) {
            if (phi[k]) {
                delete phi[k];
            }
        }
    }
    
    // only for inference
    if (newz) {
        for (int m = 0; m < newM; m++) {
            if (newz[m]) {
                delete newz[m];
            }
        }
    }
    
    if (newnw) {
        for (int w = 0; w < newV; w++) {
            if (newnw[w]) {
                delete newnw[w];
            }
        }
    }
    
    if (newnd) {
        for (int m = 0; m < newM; m++) {
            if (newnd[m]) {
                delete newnd[m];
            }
        }
    }
    
    if (newnwsum) {
        delete newnwsum;
    }
    
    if (newndsum) {
        delete newndsum;
    }
    
    if (newtheta) {
        for (int m = 0; m < newM; m++) {
            if (newtheta[m]) {
                delete newtheta[m];
            }
        }
    }
    
    if (newphi) {
        for (int k = 0; k < K; k++) {
            if (newphi[k]) {
                delete newphi[k];
            }
        }
    }
}

void model::set_default_values() {
    wordmapfile = "wordmap.txt";
    trainlogfile = "trainlog.txt";
    tassign_suffix = ".tassign";
    theta_suffix = ".theta";
    phi_suffix = ".phi";
    others_suffix = ".others";
    twords_suffix = ".twords";
    classes_trn_file = "classes_trn.txt";
    classes_pre_file = "classes_pre.txt";
    
    dir = "./";
    dfile = "trndocs.dat";
    model_name = "model-final";
    model_status = MODEL_STATUS_UNKNOWN;
    
    ptrndata = NULL;
    pnewdata = NULL;
    
    M = 0;
    V = 0;
    K = 100;

    alpha = -1.0;
    beta = -1.0;

    niters = 2000;
    liter = 0;
    savestep = 200;
    twords = 0;
    withrawstrs = 0;
    
    p = NULL;
    z = NULL;
    nw = NULL;
    nd = NULL;
    nwsum = NULL;
    ndsum = NULL;
    theta = NULL;
    phi = NULL;
    
    newM = 0;
    newV = 0;
    newz = NULL;
    newnw = NULL;
    newnd = NULL;
    newnwsum = NULL;
    newndsum = NULL;
    newtheta = NULL;
    newphi = NULL;
}

int model::init() {
    if (model_status == MODEL_STATUS_EST) {
	    // estimating the model from scratch
	    if (init_est()) {
	        return 1;
        }
        
    } else if (model_status == MODEL_STATUS_INF) {
    	// do inference
	    if (init_inf()) {
	        return 1;
	    }
    } else if (model_status == MODEL_STATUS_ESTC) {
        // estimating the model from a previously estimated one
        if (init_estc()) {
            return 1;
        }
        
    } else if (model_status == MODEL_STATUS_PREPROCESS) {

    } else if (model_status == MODEL_STATUS_RANKING) {
        if (init_ranking()) {
            return 1;
        }

    } else if (model_status == MODEL_STATUS_CLASSIFIER) {
        if (init_ranking()) {
            return 1;
        }

    } else if (model_status == MODEL_STATUS_SERVER) {
        if (init_ranking()) {
            return 1;
        }

    }
    return 0;
}

int model::load_model(string model_name) {
    string::size_type i, j;
    strtokenizer strtok, tok;
    string filename = dir + model_name + tassign_suffix;
    ifstream fin;
    fin.open(filename.c_str(), ifstream::in);
    if (!fin.is_open()) {
        cout << "Cannot open file " << filename << " to load model!" << endl;
        return 1;
    }
    
    string line;
    
    // allocate memory for z and ptrndata
    z = new int*[M];
    ptrndata = new dataset(M);
    ptrndata->V = V;
    
    for (i = 0; i < (string::size_type)M; i++) {
	    getline(fin, line);
	    if (line == "") {
	        cout << "Invalid word-topic assignment file, check the number of docs!" << endl;
	        return 1;
	    }
        
	    strtok.split(line, " \t\r\n", false);
	    int length = strtok.count_tokens();
        
        vector<int> words;
        vector<int> topics;
        for (j = 0; j < (string::size_type)length; j++) {
            string token = strtok.token(j);

            tok.split(token, ":", false);

            if (tok.count_tokens() != 2) {
                cout << "Invalid word-topic assignment line!" << endl;
                return 1;
            }
            
            words.push_back(atoi(tok.token(0).c_str()));
            topics.push_back(atoi(tok.token(1).c_str()));
            tok.clear();
        }
        
        // allocate and add new document to the corpus
        document * pdoc = new document(words);
        ptrndata->add_doc(pdoc, i);
        
        // assign values for z
        z[i] = new int[topics.size()];
        for (j = 0; j < topics.size(); j++) {
            z[i][j] = topics[j];
        }
        strtok.clear();
    }
    
    fin.close();
    
    return 0;
}

int model::save_model(string model_name) {
    if (save_model_tassign(dir + model_name + tassign_suffix)) {
        return 1;
    }
    
    if (save_model_others(dir + model_name + others_suffix)) {
        return 1;
    }
    
    if (save_model_theta(dir + model_name + theta_suffix)) {
        return 1;
    }
    
    if (save_model_phi(dir + model_name + phi_suffix)) {
        return 1;
    }
    
    if (twords > 0) {
        if (save_model_twords(dir + model_name + twords_suffix)) {
            return 1;
        }
    }
    
    return 0;
}

int model::save_model_tassign(string filename) {
    int i, j;
    
    ofstream fout;
    fout.open(filename.c_str(), ofstream::out);
    if (!fout.is_open()) {
        cout << "Cannot open file " << filename << " to save!" << endl;
        return 1;
    }
    
    // wirte docs with topic assignments for words
    for (i = 0; i < ptrndata->M; i++) {
        for (j = 0; j < ptrndata->docs[i]->length; j++) {
            fout << " " << ptrndata->docs[i]->words[j] << ":" << z[i][j] << " ";
        }
        fout << endl;
    }
    
    fout.close();
    
    return 0;
}

int model::save_model_theta(string filename) {
    ofstream fout;
    fout.open(filename.c_str(), ofstream::out);
    if (!fout.is_open()) {
        cout << "Cannot open file " << filename << " to save!" << endl;
        return 1;
    }
    
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < K; j++) {
            fout << theta[i][j] << " ";
        }
        fout << endl;
    }
    
    fout.close();
    
    return 0;
}

int model::save_model_phi(string filename) {
    ofstream fout;
    fout.open(filename.c_str(), ofstream::out);
    if (!fout.is_open()) {
        cout << "Cannot open file " << filename << " to save!" << endl;
        return 1;
    }
    
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < V; j++) {
            fout << phi[i][j] << " ";
        }
        fout << endl;
    }
    
    fout.close();
    
    return 0;
}

int model::save_model_others(string filename) {
    ofstream fout;
    fout.open(filename.c_str(), ofstream::ate);
    if (!fout.is_open()) {
        cout << "Cannot open file " << filename.c_str() << " to save!" << endl;
        return 1;
    }

    fout << "alpha=" << alpha << endl;
    fout << "beta=" << beta << endl;
    fout << "ntopics=" << K << endl;
    fout << "ndocs=" << M << endl;
    fout << "nwords=" << V << endl;
    fout << "liter=" << liter << endl;

    fout << "nwsum=";
    for (int i = 0; i < K; ++i) {
        fout << nwsum[i] << " ";
    }
    fout << endl << "ndsum=";

    for (int i = 0; i < M; ++i) {
        fout << ndsum[i] << " ";
    }
    fout << endl;

    fout.close();
    
    return 0;
}

int model::save_model_twords(string filename) {
    ofstream fout;
    fout.open(filename.c_str(), ofstream::out);
    if (!fout.is_open()) {
        cout << "Cannot open file " << filename << " to save!" << endl;
        return 1;
    }
    
    if (twords > V) {
        twords = V;
    }
    mapid2word::iterator it;
    
    for (int k = 0; k < K; k++) {
        vector<pair<int, double> > words_probs;
        pair<int, double> word_prob;
        for (int w = 0; w < V; w++) {
            word_prob.first = w;
            word_prob.second = phi[k][w];
            words_probs.push_back(word_prob);
        }
        
        // quick sort to sort word-topic probability
        utils::quicksort(words_probs, 0, words_probs.size() - 1);
        
        fout << "Topic " << k << "th:" << endl;
        for (int i = 0; i < twords; i++) {
            it = id2word.find(words_probs[i].first);
            if (it != id2word.end()) {
                fout << "   " << (it->second).c_str() << "   " << words_probs[i].second << endl;
            }
        }
    }
    
    fout.close();
    
    return 0;
}

int model::save_inf_model(string model_name) {
    cout << "here" << model_name << endl;
    if (save_inf_model_tassign(dir + model_name + tassign_suffix)) {
        return 1;
    }
    
    if (save_inf_model_others(dir + model_name + others_suffix)) {
        return 1;
    }
    
    if (save_inf_model_newtheta(dir + model_name + theta_suffix)) {
        return 1;
    }
    
    if (save_inf_model_newphi(dir + model_name + phi_suffix)) {
        return 1;
    }
    
    if (twords > 0) {
        if (save_inf_model_twords(dir + model_name + twords_suffix)) {
            return 1;
        }
    }
    
    return 0;
}

int model::save_inf_model_tassign(string filename) {
    int i, j;
    ofstream fout;
    fout.open(filename.c_str(), ofstream::out);
    if (!fout.is_open()) {
        cout << "Cannot open file " << filename << " to save!" << endl;
        return 1;
    }
    
    // wirte docs with topic assignments for words
    for (i = 0; i < pnewdata->M; i++) {
        for (j = 0; j < pnewdata->docs[i]->length; j++) {
            fout << pnewdata->docs[i]->words[j] << ":" << newz[i][j] << " ";
        }
        fout << endl;
    }
    
    fout.close();
    
    return 0;
}

int model::save_inf_model_newtheta(string filename) {
    int i, j;
    ofstream fout;
    fout.open(filename.c_str(), ofstream::out);
    if (!fout.is_open()) {
        cout << "Cannot open file " << filename << " to save!" << endl;
        return 1;
    }
    
    for (i = 0; i < newM; i++) {
        for (j = 0; j < K; j++) {
            fout << newtheta[i][j] << " ";
        }
        fout << endl;
    }
    
    fout.close();
    
    return 0;
}

int model::save_inf_model_newphi(string filename) {
    ofstream fout;
    fout.open(filename.c_str(), ofstream::out);
    if (!fout.is_open()) {
        cout << "Cannot open file " << filename << " to save!" << endl;
        return 1;
    }
    
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < newV; j++) {
            fout << newphi[i][j] << " ";
        }
        fout << endl;
    }
    
    fout.close();
    
    return 0;
}

int model::save_inf_model_others(string filename) {
    ofstream fout;
    fout.open(filename.c_str(), ofstream::ate);
    if (!fout.is_open()) {
        cout << "Cannot open file " << filename.c_str() << " to save!" << endl;
        return 1;
    }

    fout << "ntopics=" << K << endl;
    fout << "ndocs=" << newM << endl;
    fout << "nwords=" << newV << endl;
    fout << "liter=" << inf_liter << endl;

    fout << "alpha=" << alpha << endl;
    fout << "beta=" << beta << endl;
    fout << "ntopics=" << K << endl;
    fout << "ndocs=" << M << endl;
    fout << "nwords=" << V << endl;
    fout << "liter=" << liter << endl;

    fout << "nwsum=";
    for (int i = 0; i < K; ++i) {
        fout << nwsum[i] << " ";
    }
    fout << endl << "ndsum=";

    for (int i = 0; i < M; ++i) {
        fout << ndsum[i] << " ";
    }
    fout << endl;

    fout.close();
    
    return 0;
}

int model::save_inf_model_twords(string filename) {
    ofstream fout;
    fout.open(filename.c_str(), ofstream::out);
    if (!fout.is_open()) {
        cout << "Cannot open file " << filename << " to save!" << endl;
        return 1;
    }
    
    if (twords > newV) {
        twords = newV;
    }
    mapid2word::iterator it;
    map<int, int>::iterator _it;
    
    for (int k = 0; k < K; k++) {
        vector<pair<int, double> > words_probs;
        pair<int, double> word_prob;
        for (int w = 0; w < newV; w++) {
            word_prob.first = w;
            word_prob.second = newphi[k][w];
            words_probs.push_back(word_prob);
        }
        
        // quick sort to sort word-topic probability
        utils::quicksort(words_probs, 0, words_probs.size() - 1);
        
        fout << "Topic " << " " << k << "th:" << endl;
        for (int i = 0; i < twords; i++) {
            _it = pnewdata->_id2id.find(words_probs[i].first);
            if (_it == pnewdata->_id2id.end()) {
                continue;
            }
            it = id2word.find(_it->second);
            if (it != id2word.end()) {
                fout << "   " << (it->second).c_str() << "  " << words_probs[i].second << endl;
            }
        }
    }
    
    fout.close();
    
    return 0;
}

int model::init_est() {
    int m, n, w, k;

    p = new double[K]; // K : number of topics
    
    // + read training data
    ptrndata = new dataset;
    if (ptrndata->read_trndata(dir + classes_trn_file, dir + dfile, dir + wordmapfile, movie_classes)) {
        cout << "Fail to read training data!" << endl;
        return 1;
    }
    
    // + allocate memory and assign values for variables
    M = ptrndata->M;
    V = ptrndata->V;
    // K: from command line or default value
    // alpha, beta: from command line or default values
    // niters, savestep: from command line or default values
    
    nw = new int*[V]; //nw : VxK
    for (w = 0; w < V; w++) {
        nw[w] = new int[K];
        for (k = 0; k < K; k++) {
    	    nw[w][k] = 0;
        }
    }
	
    nd = new int*[M]; //nd : MxK
    for (m = 0; m < M; m++) {
        nd[m] = new int[K];
        for (k = 0; k < K; k++) {
    	    nd[m][k] = 0;
        }
    }
	
    nwsum = new int[K]; // nwsum : 1:K
    for (k = 0; k < K; k++) {
        nwsum[k] = 0;
    }
    
    ndsum = new int[M]; // ndsum : 1:M
    for (m = 0; m < M; m++) {
        ndsum[m] = 0;
    }
    //int i = 0, j = movie_classes[0].second, topic;

    srandom(time(0)); // initialize for random number generation
    z = new int*[M]; // M : number of docs
    for (m = 0; m < ptrndata->M; m++) {
        cout << "Loading file " << m << endl;
	    int N = ptrndata->docs[m]->length;
	    z[m] = new int[N]; // z(M,N) docs : words
        
        // initialize for z
        for (n = 0; n < N; n++) {
    	    int topic = (int)(((double)random() / RAND_MAX) * K);
            // if (j == 0) {
            //     topic = ++i;
            //     j = movie_classes[i].second-1;
            // } else {
            //     topic = i;
            //     j--;
            // }

    	    z[m][n] = topic;
    	    
    	    // number of instances of word i assigned to topic j
    	    nw[ptrndata->docs[m]->words[n]][topic] += 1;
    	    // number of words in document i assigned to topic j
    	    nd[m][topic] += 1;
    	    // total number of words assigned to topic j
    	    nwsum[topic] += 1;
        }
        // total number of words in document i
        ndsum[m] = N;
    }
    
    theta = new double*[M];  // each docs for each topic has one theta
    for (m = 0; m < M; m++) {
        theta[m] = new double[K];
    }
	
    phi = new double*[K]; // each topic for each word has one phi
    for (k = 0; k < K; k++) {
        phi[k] = new double[V];
    }

    //alpha0 = new double[K]; 
    //for (k = 0; k < K; k++) {
    //    alpha0[k] = alpha;
    //}

    a = 1;
    b = 20;
    c = 3;
    d = 100;

    beta0 = new double[V];
    for (k = 0; k < V; k++) {
        beta0[k] = beta;
    }
    return 0;
}

int model::init_estc() {
    // estimating the model from a previously estimated one
    int m, n, w, k;
    
    p = new double[K];
    
    // load moel, i.e., read z and ptrndata
    if (load_model(model_name)) {
        cout << "Fail to load word-topic assignment file of the model!" << endl;
        return 1;
    }
    
    nw = new int*[V];
    for (w = 0; w < V; w++) {
        nw[w] = new int[K];
        for (k = 0; k < K; k++) {
            nw[w][k] = 0;
        }
    }
    
    nd = new int*[M];
    for (m = 0; m < M; m++) {
        nd[m] = new int[K];
        for (k = 0; k < K; k++) {
            nd[m][k] = 0;
        }
    }
    
    nwsum = new int[K];
    for (k = 0; k < K; k++) {
        nwsum[k] = 0;
    }
    
    ndsum = new int[M];
    for (m = 0; m < M; m++) {
        ndsum[m] = 0;
    }
    
    for (m = 0; m < ptrndata->M; m++) {
        int N = ptrndata->docs[m]->length;
        
        // assign values for nw, nd, nwsum, and ndsum
        for (n = 0; n < N; n++) {
            int w = ptrndata->docs[m]->words[n];
            int topic = z[m][n];
            
            // number of instances of word i assigned to topic j
            nw[w][topic] += 1;
            // number of words in document i assigned to topic j
            nd[m][topic] += 1;
            // total number of words assigned to topic j
            nwsum[topic] += 1;
        }
        // total number of words in document i
        ndsum[m] = N;
    }
    
    theta = new double*[M];
    for (m = 0; m < M; m++) {
        theta[m] = new double[K];
    }
    
    phi = new double*[K];
    for (k = 0; k < K; k++) {
        phi[k] = new double[V];
    }
    
    return 0;
}

void model::preprocess() {
    ofstream fout, fout2;
    string output, output2;
    int size = 65535;
    if (file_type == 1) {
        output = "model/trndata.txt";
        output2 = "model/classes_trn.txt";

    } else if (file_type == 2){
        output = "model/predata.txt";
        output2 = "model/classes_pre.txt";
    } else {
        cout << "Invalid file_type" << endl;
    }
    
    fout.open(output.c_str(), ofstream::ate);
    fout2.open(output2.c_str(), ofstream::ate);
    strtokenizer strtok;
    fout << size;
    size = 0;
    utils::addfile(originDir, fout, strtok, movie_classes, size);
    fout.seekp(0);
    fout << size << endl;
    fout.close();

    for (vector<pair<string, int> >::size_type i = 0; i < movie_classes.size(); i++) {
        fout2 << movie_classes[i].first << " " << movie_classes[i].second << endl;
    }
    fout2.close();

    return;
}

void model::estimate() {
    ofstream fout, fout2;
    fout.open("model/alpha.txt", ofstream::out);
    fout2.open("model/beta.txt", ofstream::out);

    if (!fout.is_open() || !fout2.is_open()) {
        cout << "can't open file" << endl;
        return;
    }
    if (twords > 0) {
        // print out top words per topic
        dataset::read_wordmap(dir + wordmapfile, &id2word);
    }

    sumbeta = 0;
    for (int i = 0; i < V; i++) {
        sumbeta += beta0[i];
    }
    beta = sumbeta / V;
    
    cout << "Sampling " << niters << " iterations!" << endl;
    
    int last_iter = liter;
    for (liter = last_iter + 1; liter <= niters + last_iter; liter++) {
        cout << "Iteration " << liter << " ..." << endl;

        //sumalpha = 0;
        //for (int i = 0; i < K; i++) {
        //    sumalpha += alpha0[i];
        //}
        //alpha = sumalpha / K;
        
	    // for all z_i
	    for (int m = 0; m < M; m++) {
	        for (int n = 0; n < ptrndata->docs[m]->length; n++) {
                // (z_i = z[m][n])
                // sample from p(z_i|z_-i, w)
                int topic = sampling(m, n);
                z[m][n] = topic;
	        }
	    }
            //for (int i = 0; i < K; i++) {
            //    fout << alpha0[i] << " ";
            //}
            //fout << endl;
            
            //for (int i = 0; i < V; i++) {
            //    fout2 << beta0[i] << " ";
            //}
            //fout2 << endl; 

            //sumbeta = 0;
            //for (int i = 0; i < V; i++) {
            //    sumbeta += beta0[i];
            //}
            //beta = sumbeta / V;
            fout << alpha << " " << beta << endl;
            cout << alpha << " " << beta << endl;
        if (liter % 10 == 0) {
            //compute_alpha();
            //compute_beta();
            compute_theta();
            compute_phi();            
        }

	    if (savestep > 0 && liter % savestep == 0) {
            // saving the model
            cout << "Saving the model at iteration " << liter << " ..." << endl;
            save_model(utils::generate_model_name(liter));
	    }
    }
    
    cout << "Gibbs sampling completed!" << endl;
    cout << "Saving the final model!" << endl;

    compute_theta();
    compute_phi();
    //compute_alpha();
    //compute_beta();
    liter--;
    save_model(utils::generate_model_name(-1));

    fout.close();
    fout2.close();
}

int model::sampling(int m, int n) {
    // remove z_i from the count variables
    int topic = z[m][n];
    int w = ptrndata->docs[m]->words[n];
    nw[w][topic] -= 1;
    nd[m][topic] -= 1;
    nwsum[topic] -= 1;
    ndsum[m] -= 1;
    
    //double Kalpha = K * alpha;
    //double Vbeta = V * beta;

    // do multinomial sampling via cumulative method
    for (int k = 0; k < K; k++) {
	    p[k] = (nw[w][k] + beta0[w]) / (nwsum[k] + sumbeta) *
        (nd[m][k] + alpha); /// (ndsum[m] + Kalpha);
    }
    // cumulate multinomial parameters
    for (int k = 1; k < K; k++) {
        p[k] += p[k - 1];
    }
    // scaled sample because of unnormalized p[]
    double u = ((double)random() / RAND_MAX) * p[K - 1];
    
    for (topic = 0; topic < K; topic++) {
	    if (p[topic] > u) {
	        break;
	    }
    }
    
    // add newly estimated z_i to count variables
    nw[w][topic] += 1;
    nd[m][topic] += 1;
    nwsum[topic] += 1;
    ndsum[m] += 1;
    return topic;
}

void model::compute_theta() {
    for (int m = 0; m < M; m++) {
        for (int k = 0; k < K; k++) {
            theta[m][k] = (nd[m][k] + alpha) / (ndsum[m] + K * alpha);
        }
    }
}

void model::compute_phi() {
    for (int k = 0; k < K; k++) {
        for (int w = 0; w < V; w++) {
            phi[k][w] = (nw[w][k] + beta0[w]) / (nwsum[k] + sumbeta);
        }
    }
}

void model::compute_alpha() {
    double sum1 = 0, sum2 = 0;

    for (int j = 0; j < M; j++) {
        sum2 += digamma(ndsum[j] + K * alpha);
    }

    for (int i = 0; i < K; i++) {
        for (int j = 0; j < M; j++) {
            sum1 += digamma(nd[j][i] + alpha);
        }
        
    }
    alpha = alpha * (a - 1 + sum1 / K - M * digamma(alpha)) / (b + sum2 - M * digamma(K * alpha));
}

void model::compute_beta() {
    double sum1 = 0, sum2 = 0;

    for (int i = 0; i < K; i++) {
        sum2 += digamma(nwsum[i] + sumbeta);
    }

    for (int j = 0; j < V; j++) {
        sum1 = 0;
        //for (int i = 0; i < V; i++) {
        //    sumbeta += beta0[i];
        //}
        for (int i = 0; i < K; i++) {
            sum1 += digamma(nw[j][i] + beta0[j]);
        }
        beta0[j] = beta0[j] * (c - 1 + sum1 - K * digamma(beta0[j])) / (d + sum2 - digamma(sumbeta));
    }
}

int model::init_inf() {
    // estimating the model from a previously estimated one
    int m, n, w, k;
    
    p = new double[K];
    
    // load moel, i.e., read z and ptrndata
    if (load_model(model_name)) {
        cout << "Fail to load word-topic assignment file of the model!" << endl;
        return 1;
    }
    
    nw = new int*[V];
    for (w = 0; w < V; w++) {
        nw[w] = new int[K];
        for (k = 0; k < K; k++) {
    	    nw[w][k] = 0;
        }
    }
	
    nd = new int*[M];
    for (m = 0; m < M; m++) {
        nd[m] = new int[K];
        for (k = 0; k < K; k++) {
    	    nd[m][k] = 0;
        }
    }
	
    nwsum = new int[K];
    for (k = 0; k < K; k++) {
        nwsum[k] = 0;
    }
    
    ndsum = new int[M];
    for (m = 0; m < M; m++) {
        ndsum[m] = 0;
    }
    
    for (m = 0; m < ptrndata->M; m++) {
        int N = ptrndata->docs[m]->length;
        
        // assign values for nw, nd, nwsum, and ndsum
        for (n = 0; n < N; n++) {
    	    int w = ptrndata->docs[m]->words[n];
    	    int topic = z[m][n];
    	    
    	    // number of instances of word i assigned to topic j
    	    nw[w][topic] += 1;
    	    // number of words in document i assigned to topic j
    	    nd[m][topic] += 1;
    	    // total number of words assigned to topic j
    	    nwsum[topic] += 1;
        }
        // total number of words in document i
        ndsum[m] = N;
    }
    
    // read new data for inference
    pnewdata = new dataset;
    if (withrawstrs) {
        if (pnewdata->read_newdata_withrawstrs(dir + dfile, dir + wordmapfile)) {
    	    cout << "Fail to read new data!" << endl;
    	    return 1;
        }
    } else {
        if (pnewdata->read_newdata(dir + dfile, dir + wordmapfile)) {
    	    cout << "Fail to read new data!" << endl;
    	    return 1;
        }
    }
    
    newM = pnewdata->M;
    newV = pnewdata->V;
    
    newnw = new int*[newV];
    for (w = 0; w < newV; w++) {
        newnw[w] = new int[K];
        for (k = 0; k < K; k++) {
    	    newnw[w][k] = 0;
        }
    }
	
    newnd = new int*[newM];
    for (m = 0; m < newM; m++) {
        newnd[m] = new int[K];
        for (k = 0; k < K; k++) {
    	    newnd[m][k] = 0;
        }
    }
	
    newnwsum = new int[K];
    for (k = 0; k < K; k++) {
        newnwsum[k] = 0;
    }
    
    newndsum = new int[newM];
    for (m = 0; m < newM; m++) {
        newndsum[m] = 0;
    }
    
    srandom(time(0)); // initialize for random number generation
    newz = new int*[newM];
    for (m = 0; m < pnewdata->M; m++) {
        int N = pnewdata->docs[m]->length;
        newz[m] = new int[N];
        
        // assign values for nw, nd, nwsum, and ndsum
        for (n = 0; n < N; n++) {
    	    //int w = pnewdata->docs[m]->words[n];
    	    int _w = pnewdata->_docs[m]->words[n];
    	    int topic = (int)(((double)random() / RAND_MAX) * K);
    	    newz[m][n] = topic;
    	    
    	    // number of instances of word i assigned to topic j
    	    newnw[_w][topic] += 1;
    	    // number of words in document i assigned to topic j
    	    newnd[m][topic] += 1;
    	    // total number of words assigned to topic j
    	    newnwsum[topic] += 1;
        }
        // total number of words in document i
        newndsum[m] = N;
    }
    
    newtheta = new double*[newM];
    for (m = 0; m < newM; m++) {
        newtheta[m] = new double[K];
    }
	
    newphi = new double*[K];
    for (k = 0; k < K; k++) {
        newphi[k] = new double[newV];
    }
    
    return 0;
}

void model::inference() {
    if (twords > 0) {
        // print out top words per topic
        dataset::read_wordmap(dir + wordmapfile, &id2word);
    }
    
    cout << "Sampling " << niters << " iterations for inference!" << endl;
    
    for (inf_liter = 1; inf_liter <= niters; inf_liter++) {
        cout << "Iteration " << inf_liter << " ..." << endl;
        
        // for all newz_i
        for (int m = 0; m < newM; m++) {
            for (int n = 0; n < pnewdata->docs[m]->length; n++) {
                // (newz_i = newz[m][n])
                // sample from p(z_i|z_-i, w)
                int topic = inf_sampling(m, n);
                newz[m][n] = topic;
            }
        }
    }
    
    cout << "Gibbs sampling for inference completed!" << endl;
    cout << "Saving the inference outputs!" << endl;
    compute_newtheta();
    compute_newphi();
    inf_liter--;
    save_inf_model(dfile);
}

int model::inf_sampling(int m, int n) {
    // remove z_i from the count variables
    int topic = newz[m][n];
    int w = pnewdata->docs[m]->words[n];
    int _w = pnewdata->_docs[m]->words[n];
    newnw[_w][topic] -= 1;
    newnd[m][topic] -= 1;
    newnwsum[topic] -= 1;
    newndsum[m] -= 1;
    
    double Kalpha = K * alpha;
    double Vbeta = V * beta;   
    
    // do multinomial sampling via cumulative method
    for (int k = 0; k < K; k++) {
        p[k] = (nw[w][k] + newnw[_w][k] + beta) / (nwsum[k] + newnwsum[k] + Vbeta) *
        (newnd[m][k] + alpha) / (newndsum[m] + Kalpha);
    }
    // cumulate multinomial parameters
    for (int k = 1; k < K; k++) {
        p[k] += p[k - 1];
    }
    // scaled sample because of unnormalized p[]
    double u = ((double)random() / RAND_MAX) * p[K - 1];
    
    for (topic = 0; topic < K; topic++) {
        if (p[topic] > u) {
            break;
        }
    }
    
    // add newly estimated z_i to count variables
    newnw[_w][topic] += 1;
    newnd[m][topic] += 1;
    newnwsum[topic] += 1;
    newndsum[m] += 1;    
    
    return topic;
}

void model::compute_newtheta() {
    for (int m = 0; m < newM; m++) {
        for (int k = 0; k < K; k++) {
            newtheta[m][k] = (newnd[m][k] + alpha) / (newndsum[m] + K * alpha);
        }
    }
}

void model::compute_newphi() {

    map<int, int>::iterator it;
    for (int k = 0; k < K; k++) {
        for (int w = 0; w < newV; w++) {
            it = pnewdata->_id2id.find(w);
            if (it != pnewdata->_id2id.end()) {
                newphi[k][w] = (nw[it->second][k] + newnw[w][k] + beta) / (nwsum[k] + newnwsum[k] +  V * beta);
            }
        }
    }
}

int model::init_ranking() {
    // if (load_model(model_name)) {
    //     cout << "Fail to load word-topic assignment file of the model!" << endl;
    //     return 1;
    // }
    ifstream in;
    string input = dir + model_name + theta_suffix, str;

    strtokenizer strtok;
    in.open(input.c_str(), ifstream::in);
    if (!in.is_open()) {
        cout << "can't open theta file" << endl;
    }
    int i = 0;
    theta = new double*[M];
    while (1) {
        getline(in, str);
        if (in.eof()) {
            break;
        }
        strtok.split(str, " \t\r\n", false);
        assert((int)strtok.count_tokens() == K);
        theta[i] = new double[K];
        for (int j = 0; j < K; ++j) {
            theta[i][j] = utils::stod(strtok.token(j));
        }
        strtok.clear();
        i++;
    }
    return 0;
}

void model::ranking() {
    rank_num = rank_num - 1;
    assert(rank_num >= 0 && rank_num < M);
    database db;
    cout << " =============== " << rank_num+1 << " =============== " << endl;
    db.preciseFetchDisp(rank_num+1);

    vector<pair<int,double> > p;
    for (int i = 0; i < M; ++i) {
        double tmp = 0;
        if (rank_num == i) { 
            p.push_back(pair<int, double>(i, 0.0));  
            continue;
        }
        for (int j = 0; j < K; ++j) {
            tmp += theta[i][j]*ndsum[i]*theta[rank_num][j]/nwsum[j];
        }
        p.push_back(pair<int, double>(i,tmp));
    }
    utils::quicksort(p, 0, p.size()-1);

    for (int i = 0; i < disp; ++i) {
        cout << " =============== " << p[i].first+1 << " =============== " << endl;
        db.preciseFetchDisp(p[i].first+1);
    }
}

vector<int> model::ranking(vector<int> candidate, string type, string category) {
    // candidate: int - number
    utils::genInf(candidate);
    if (init_inf()) {
        cout << "error init inference" << endl;
    }
    inference();
    assert(newM == 1);

    database db;
    cout << "near" << endl;

    ifstream fin;
    string line;
    strtokenizer strtok;

    fin.open((dir + classes_trn_file).c_str(), ifstream::in);
    if (!fin.is_open()) {
        cout << "Cannot open file " <<  dir << classes_trn_file<< " to read!" << endl;
    }

    while(!fin.eof()) {
        getline(fin, line);
        strtok.parse(line, " \t\r\n");
        movie_classes.push_back(pair<string, int>(strtok.token(0), atoi(strtok.token(1).c_str())));
        cout << movie_classes[movie_classes.size()-1].first << " " << movie_classes[movie_classes.size()-1].second << endl;
        strtok.clear();
    }
    fin.close();

    int start = 0, end = 0;
    cout << start << " " << end << endl;
    if (type == "movies" && category != "") {
        for (vector<string>::size_type i = 0; i < 16; i++) {
            if (movie_classes[i].first == category) {
                end = start + movie_classes[i].second;
                break;
            }
            start += movie_classes[i].second;
        }
    } else if (type == "TV" && category != "") {
        for (vector<string>::size_type i = 0; i < 28; i++) {
            if (i > 15 && movie_classes[i].first == category) {
                end = start + movie_classes[i].second;
                break;
            }
            start += movie_classes[i].second;
        }
    } else if (type == "movies" && category == "") {
        for (vector<string>::size_type i = 0; i < 16; i++) {
            end += movie_classes[i].second;
        }      
    } else if (type == "TV" && category == "") {
        for (vector<string>::size_type i = 0; i < 16; i++) {
            start += movie_classes[i].second;
        }         
        end = start;
        for (vector<string>::size_type i = 16; i < 28; i++) {
            end += movie_classes[i].second;
        }       
    } else {
        end = M;
    }

    cout << start << " " << end << endl;
    vector<pair<int,double> > p;
    vector<int> ans;
    for (int i = start; i < end; ++i) {
        double tmp = 0;
        for (int j = 0; j < K; ++j) {
            tmp += theta[i][j]*ndsum[i]*newtheta[0][j]/nwsum[j];
        }
        p.push_back(pair<int, double>(i,tmp));
    }
    utils::quicksort(p, 0, p.size()-1); 

    for (int i = 0; i < N_DISP; i++) {
        ans.push_back(p[i].first);
    }  

    return ans;








    // database db;
    // vector<pair<int,double> > p;
    // vector<vector<int>*> all;

    // for (vector<int>::size_type k = 0; k < candidate.size(); k++) {
    //     vector<int>* part = new vector<int>();
    //     //cout << candidate[k] << " " << k << " " << candidate.size() << endl;
    //     p.clear();
    //     for (int i = 0; i < M; ++i) {
    //         double tmp = 0;
    //         if (candidate[k] == i) { 
    //             p.push_back(pair<int, double>(i, 0.0));  
    //             continue;
    //         }
    //         for (int j = 0; j < K; ++j) {
    //             tmp += theta[i][j] * ndsum[i] * theta[candidate[k]][j] / nwsum[j];
    //         }
    //         p.push_back(pair<int, double>(i,tmp));
    //     }
    //     utils::quicksort(p, 0, p.size()-1);
    //     for (int i = 0; i < N_RANKING; i++) {
    //         part->push_back(p[i].first);
    //     }
    //     all.push_back(part);
    // }
    // return utils::findCommon(all, candidate.size(), M);
}

void model::classification() {
    vector<pair<int,double> > p;

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < K; j++) {
           p.push_back(pair<int, double>(j, theta[i][j]));
        }
        pair<int,double> tmp = utils::quicksort_wr(p, 0, p.size()-1);
        cout << "Movie " << i  << "th: " << tmp.first << " " << tmp.second << endl;
        p.clear();
    }
}
