#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include "strtokenizer.h"
#include "utils.h"
#include "model.h"
#define NUM 10000
using namespace std;

void utils::readfile(string ofile, ofstream& fout, strtokenizer& strtok) {
    ifstream fin;
    string str;
    fin.open(ofile.c_str(), ifstream::in);
    if (!fin.is_open()) {
        printf("Cannot open file %s to read!\n", ofile.c_str());
        return;
    }  
    while (fin >> str) {
        strtok.strtokenizer_operate(str, "", true);
    }
    int n;
    if (strtok.count_tokens() < NUM) {
        n = strtok.count_tokens();
    } else {
        n = NUM;
    }
    for (vector<string>::size_type i = 0; i < n; i++) {
        fout << strtok.token(i) << " ";
    }
    fout << endl;
}

void utils::addfile(string name, ofstream& fout, strtokenizer& strtok, int& size) {
    string filepath;
    struct stat info;
    DIR *dir;
    stat(name.c_str(), &info);

    if ((info.st_mode & S_IFDIR) == S_IFDIR)
    {
        dir = opendir(name.c_str());
        struct dirent *dirEntry;
        if (dir == NULL)
        {
            cout << "error directory name!" << endl;
            return;
        }
        while ((dirEntry = readdir(dir))) {
            filepath.assign(dirEntry->d_name);
            if(filepath.at(0) == '.') {
                continue;
            }
            filepath = name + '/' + filepath;
            addfile(filepath, fout, strtok, size);
        }
        closedir(dir);
    } else if ((info.st_mode & S_IFREG) == S_IFREG) {
        cout << size << "   " << name << endl;
        readfile(name, fout, strtok);
        size++;
    } else {
        cout << "Invalid type of file: " << name << endl;
    }
    return;
}

int utils::parse_args(int argc, char ** argv, model * pmodel) {
    int model_status = MODEL_STATUS_UNKNOWN;
    string dir = "";
    string model_name = "";
    string dfile = "";
    string originDir = "";

    double alpha = -1.0;
    double beta = -1.0;
    int K = 0;
    int niters = 0;
    int savestep = 0;
    int twords = 0;
    int withrawdata = 0;
    int file_type = 0;
    int rank_num = 1;
    int disp = 10;
    
    int i = 0;
    while (i < argc) {
		string arg = argv[i];
        
		if (arg == "-est") {
	    	model_status = MODEL_STATUS_EST;
            
		} else if (arg == "-estc") {
	    	model_status = MODEL_STATUS_ESTC;
            
		} else if (arg == "-inf") {
	    	model_status = MODEL_STATUS_INF;
            
		} else if (arg == "-dir") {
	    	dir = argv[++i];
            
		} else if (arg == "-dfile") {
	    	dfile = argv[++i];
            
		} else if (arg == "-model") {
	    	model_name = argv[++i];
            
		} else if (arg == "-alpha") {
	    	alpha = atof(argv[++i]);
            
		} else if (arg == "-beta") {
	    	beta = atof(argv[++i]);
            
		} else if (arg == "-ntopics") {
	    	K = atoi(argv[++i]);
            
		} else if (arg == "-niters") {
	    	niters = atoi(argv[++i]);
            
		} else if (arg == "-savestep") {
	    	savestep = atoi(argv[++i]);
            
		} else if (arg == "-twords") {
	    	twords = atoi(argv[++i]);
            
		} else if (arg == "-withrawdata") {
	    	withrawdata = 1;
            
		} else if (arg == "-pprocess") {
			originDir = argv[++i];
            file_type = atoi(argv[++i]);
			model_status = MODEL_STATUS_PREPROCESS;

		} else if (arg == "-ranking") {
            rank_num = atoi(argv[++i]);
            model_status = MODEL_STATUS_RANKING;

        } else if (arg == "-disp") {
            disp = atoi(argv[++i]);
        }

        else {
	    	// any more?
		}
		i++;
    }
    
    if (model_status == MODEL_STATUS_EST) {
		if (dfile == "") {
	    	printf("Please specify the input data file for model estimation!\n");
	    	return 1;
		}
        
		pmodel->model_status = model_status;
        
		if (K > 0) {
	    	pmodel->K = K;
		}
        
		if (alpha >= 0.0) {
	    	pmodel->alpha = alpha;
		} else {
            // default value for alpha
            pmodel->alpha = 50.0 / pmodel->K;
        }
        
        if (beta >= 0.0) {
            pmodel->beta = beta;
        }
        
        if (niters > 0) {
            pmodel->niters = niters;
        }
        
        if (savestep > 0) {
            pmodel->savestep = savestep;
        }
        
        if (twords > 0) {
            pmodel->twords = twords;
        }
        
        pmodel->dfile = dfile;
        
        string::size_type idx = dfile.find_last_of("/");
        if (idx == string::npos) {
            pmodel->dir = "./";
        } else {
            pmodel->dir = dfile.substr(0, idx + 1);
            pmodel->dfile = dfile.substr(idx + 1, dfile.size() - pmodel->dir.size());
            printf("dir = %s\n", pmodel->dir.c_str());
            printf("dfile = %s\n", pmodel->dfile.c_str());
        }
    }
    
    if (model_status == MODEL_STATUS_ESTC) {
        if (dir == "") {
            printf("Please specify model directory!\n");
            return 1;
        }
        
        if (model_name == "") {
            printf("Please specify model name upon that you want to continue estimating!\n");
            return 1;
        }
        
        pmodel->model_status = model_status;
        
        if (dir[dir.size() - 1] != '/') {
            dir += "/";
        }
        pmodel->dir = dir;
        
        pmodel->model_name = model_name;
        
        if (niters > 0) {
            pmodel->niters = niters;
        }
        
        if (savestep > 0) {
            pmodel->savestep = savestep;
        }
        
        if (twords > 0) {
            pmodel->twords = twords;
        }
        
        // read <model>.others file to assign values for ntopics, alpha, beta, etc.
        if (read_and_parse(pmodel->dir + pmodel->model_name + pmodel->others_suffix, pmodel)) {
            return 1;
        }
    }
    
    if (model_status == MODEL_STATUS_INF) {
        if (dir == "") {
            printf("Please specify model directory please!\n");
            return 1;
        }
        
        if (model_name == "") {
            printf("Please specify model name for inference!\n");
            return 1;
        }
        
        if (dfile == "") {
            printf("Please specify the new data file for inference!\n");
            return 1;
        }
        
        pmodel->model_status = model_status;
        
        if (dir[dir.size() - 1] != '/') {
            dir += "/";
        }
        pmodel->dir = dir;
        
        pmodel->model_name = model_name;
        
        pmodel->dfile = dfile;
        
        if (niters > 0) {
            pmodel->niters = niters;
        } else {
            // default number of Gibbs sampling iterations for doing inference
            pmodel->niters = 20;
        }
        
        if (twords > 0) {
            pmodel->twords = twords;
        }
        
        if (withrawdata > 0) {
            pmodel->withrawstrs = withrawdata;
        }
		
        // read <model>.others file to assign values for ntopics, alpha, beta, etc.
        if (read_and_parse(pmodel->dir + pmodel->model_name + pmodel->others_suffix, pmodel)) {
            return 1;
        }
    }

    if (model_status == MODEL_STATUS_PREPROCESS) {
    	if (originDir == "") {
    		printf("Please specify the input data file for text preprocess!\n");
	    	return 1;
    	}

    	pmodel->model_status = model_status;
    	pmodel->originDir = originDir;
        pmodel->file_type = file_type;
    }

    if (model_status == MODEL_STATUS_RANKING) {
        pmodel->disp = disp;
        pmodel->rank_num = rank_num;
        pmodel->model_status = model_status;

        if (dir == "") {
            printf("Please specify model directory please!\n");
            return 1;
        }
        
        if (model_name == "") {
            printf("Please specify model name for inference!\n");
            return 1;
        }
        if (dir[dir.size() - 1] != '/') {
            dir += "/";
        }
        pmodel->dir = dir;
        pmodel->model_name = model_name;
        if (read_and_parse(pmodel->dir + pmodel->model_name + pmodel->others_suffix, pmodel)) {
            return 1;
        }
    }
    
    if (model_status == MODEL_STATUS_UNKNOWN) {
        printf("Please specify the task you would like to perform (-est/-estc/-inf/-preprocess)!\n");
        return 1;
    }
    
    return 0;
}

int utils::read_and_parse(string filename, model * pmodel) {
    // open file <model>.others to read:
    // alpha=?
    // beta=?
    // ntopics=?
    // ndocs=?
    // nwords=?
    // citer=? // current iteration (when the model was saved)
    
    ifstream fin;
    fin.open(filename.c_str(), ifstream::in);
    if (!fin.is_open()) {
        cout << "Can't open file " << filename << endl;
        return 1; 
    }
    
    string line;
    strtokenizer strtok;

    while (1) {
        getline(fin, line);
        if (fin.eof()) {
            break;
        }
        strtok.strtokenizer_operate(line, "= \t\r\n", false);
        
        string optstr = strtok.token(0);
        string optval = strtok.token(1);
        
        if (optstr == "alpha") {
            pmodel->alpha = atof(optval.c_str());
            
        } else if (optstr == "beta") {
            pmodel->beta = atof(optval.c_str());
            
        } else if (optstr == "ntopics") {
            pmodel->K = atoi(optval.c_str());
            
        } else if (optstr == "ndocs") {
            pmodel->M = atoi(optval.c_str());
            
        } else if (optstr == "nwords") {
            pmodel->V = atoi(optval.c_str());
            
        } else if (optstr == "liter") {
            pmodel->liter = atoi(optval.c_str());
            
        } else if (optstr == "nwsum") {
            assert((int)strtok.count_tokens() == pmodel->K+1);
            pmodel->nwsum = new int[pmodel->K];
            for (int i = 0; i < pmodel->K; ++i) {
                pmodel->nwsum[i] = stoi(strtok.token(i+1));
            }
        } else if (optstr == "ndsum") {
            // cout << strtok.count_tokens() << endl;
            // strtok.print();
            assert((int)strtok.count_tokens() == pmodel->M+1);
            pmodel->ndsum = new int[pmodel->M];
            for (int i = 0; i < pmodel->M; ++i) {
                pmodel->ndsum[i] = stoi(strtok.token(i+1));
            }
        } else {
            // any more?
        }
        strtok.clear();
    }
    
    fin.close();
    
    return 0;
}

string utils::generate_model_name(int iter) {
    string model_name = "model-";
    
    char buff[BUFF_SIZE_SHORT];
    
    if (0 <= iter && iter < 10) {
        sprintf(buff, "0000%d", iter);
    } else if (10 <= iter && iter < 100) {
        sprintf(buff, "000%d", iter);
    } else if (100 <= iter && iter < 1000) {
        sprintf(buff, "00%d", iter);
    } else if (1000 <= iter && iter < 10000) {
        sprintf(buff, "0%d", iter);
    } else {
        sprintf(buff, "%d", iter);
    }
    
    if (iter >= 0) {
        model_name += buff;
    } else {
        model_name += "final";
    }
    
    return model_name;
}

void utils::sort(vector<double> & probs, vector<int> & words) {
    for (int i = 0; i < (int)probs.size() - 1; i++) {
        for (int j = i + 1; j < (int)probs.size(); j++) {
            if (probs[i] < probs[j]) {
                double tempprob = probs[i];
                int tempword = words[i];
                probs[i] = probs[j];
                words[i] = words[j];
                probs[j] = tempprob;
                words[j] = tempword;
            }
        }
    }
}

void utils::quicksort(vector<pair<int, double> > & vect, int left, int right) {
    int l_hold, r_hold;
    pair<int, double> pivot;
    
    l_hold = left;
    r_hold = right;    
    int pivotidx = left;
    pivot = vect[pivotidx];
    
    while (left < right) {
        while (vect[right].second <= pivot.second && left < right) {
            right--;
        }
        if (left != right) {
            vect[left] = vect[right];
            left++;
        }
        while (vect[left].second >= pivot.second && left < right) {
            left++;
        }
        if (left != right) {
            vect[right] = vect[left];
            right--;
        }
    }
    
    vect[left] = pivot;
    pivotidx = left;
    left = l_hold;
    right = r_hold;
    
    if (left < pivotidx) {
        quicksort(vect, left, pivotidx - 1);
    }
    if (right > pivotidx) {
        quicksort(vect, pivotidx + 1, right);
    }    
}


