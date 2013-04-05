#include "utils.h"

using namespace std;
using namespace boost;

double utils::stod(const string& str) {
    double num = 0;
    int n = 0;
    int j = str.find_first_of(".");
    string sub = str.substr(j+1);
    for (string::size_type k = 0; k < sub.size(); k++) {
        assert(isdigit(sub[k]));
        if (sub[k] == '0') {
            n++;
        } else {
            break;
        }
    }
    long tmp = atol(sub.c_str());
    num = tmp / pow(10.0, (int)sub.size());

    return num;
}

void utils::readfile(string ofile, strtokenizer& strtok) {
    ifstream fin;
    string str;
    fin.open(ofile.c_str(), ifstream::in);

    if (!fin.is_open()) {
        cout << "Cannot open file " << ofile << " to read!" << endl;
        return;
    }  
    boost::regex re1("\'");
    boost::regex re2("_");
    string rep1 = "\'\'";
    string rep2 = " ";

    while (!fin.eof()) {
        getline(fin, str);
        str = regex_replace(str, re1, rep1, boost::match_default | boost::format_all);

        if (strtok.count_tokens() == 0) {
            str = regex_replace(str, re2, rep2, boost::match_default | boost::format_all);
            strtok.addToken(str);
        } else if (str.find("Released Year:") == 0) {
            strtok.addToken(str.substr(15));
        } else if (str.find("Running time:") == 0) {
            strtok.addToken(str.substr(14));
        } else if (str.find("Director:") == 0) {
            strtok.addToken(str.substr(10));
        } else if (str.find("Cast:") == 0) {
            strtok.addToken(str.substr(6));
        } else if (str.find("Content:") == 0) {
            strtok.addToken(str.substr(9));
        } else if (str.find("Wiki info:") == 0 || str == "") {
            continue;
        } else {
            strtok.addToken(str);
        }
    }
    fin.close();
}

void utils::addfile(string name, vector<string>& pathset, vector<string>::size_type& size) {
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
            addfile(filepath, pathset, size);
        }
        closedir(dir);
    } else if ((info.st_mode & S_IFREG) == S_IFREG) {
        if (name.find("[Image]") != string::npos) { return; }
        cout << size << "   " << name << endl;
        pathset.push_back(name);
        size++;
    } else {
        cout << "Invalid type of file: " << name << endl;
    }
    return;
}

void utils::readfile(string ofile, ofstream& fout, strtokenizer& strtok) {
    ifstream fin;
    string str;
    bool startRead = false;
    fin.open(ofile.c_str(), ifstream::in);
    strtokenizer tok;
    if (!fin.is_open()) {
        cout << "Cannot open file " << ofile << " to read!" << endl;
        return;
    }  
    // read the title
    getline(fin, str);
    strtok.split(str, " \t\n\r", true);

    while (!fin.eof()) {
        getline(fin, str);
        if (str.find("Content:") == string::npos) { 
            continue; 
        } else {
            startRead = true;
            break;
        }
    }
    assert(startRead);
    tok.parse(str.substr(9), " \t\n\r");
    for (vector<string>::size_type i = 0; i < tok.count_tokens(); i++) {
        strtok.split(tok.token(i), " \t\n\r", true);
    }

    getline(fin, str);
    assert(str.find("Wiki info:") != string::npos);

    while (fin >> str) {
        strtok.split(str, " \t\n\r", true);
    }
    for (vector<string>::size_type i = 0; i < strtok.count_tokens(); i++) {
        fout << strtok.token(i) << " ";
    }
    fout << endl;
    strtok.clear();
}

void utils::addfile(string name, ofstream& fout, strtokenizer& strtok, vector<pair<string, int> >& classes, int& size) {
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
            addfile(filepath, fout, strtok, classes, size);
        }
        closedir(dir);
    } else if ((info.st_mode & S_IFREG) == S_IFREG) {
        if (name.find("[Image]") != string::npos) { return; }
        cout << size << "   " << name << endl;
        readfile(name, fout, strtok);
        size++;

        strtokenizer strname;
        strname.parse(name, "/");
        assert(strname.count_tokens() == 3);
        for (vector<pair<string, int> >::size_type i = 0; i < classes.size(); i++) {
            if (classes[i].first == strname.token(1)) {
                classes[i].second += 1;
                return;
            }
        } 
        classes.push_back(pair<string, int>(strname.token(1),1));
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
	    	cout << "Please specify the input data file for model estimation!" << endl;
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
            cout << "dir = " << pmodel->dir.c_str() << endl;
            cout << "dfile =" << pmodel->dfile.c_str() << endl;
        }
    }
    
    if (model_status == MODEL_STATUS_ESTC) {
        if (dir == "") {
            cout << "Please specify model directory!" << endl;
            return 1;
        }
        
        if (model_name == "") {
            cout << "Please specify model name upon that you want to continue estimating!" << endl;
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
            cout << "Please specify model directory please!" << endl;
            return 1;
        }
        
        if (model_name == "") {
            cout << "Please specify model name for inference!" << endl;
            return 1;
        }
        
        if (dfile == "") {
            cout << "Please specify the new data file for inference!" << endl;
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
    		cout << "Please specify the input data file for text preprocess!" << endl;
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
            cout << "Please specify model directory please!" << endl;
            return 1;
        }
        
        if (model_name == "") {
            cout << "Please specify model name for inference!" << endl;
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
        cout << "Please specify the task you would like to perform (-est/-estc/-inf/-preprocess)!" << endl;
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
        strtok.split(line, "= \t\r\n", false);
        
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
                pmodel->nwsum[i] = atoi(&(strtok.token(i+1)[0]));
            }
        } else if (optstr == "ndsum") {
            assert((int)strtok.count_tokens() == pmodel->M+1);
            pmodel->ndsum = new int[pmodel->M];
            for (int i = 0; i < pmodel->M; ++i) {
                pmodel->ndsum[i] = atoi(&(strtok.token(i+1)[0]));
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
    ostringstream ostr;
    ostr << iter;

    string str;
    string s = ostr.str();

    if (0 <= iter && iter < 10) {
        str = "0000" + s;
    } else if (10 <= iter && iter < 100) {
        str = "000" + s;
    } else if (100 <= iter && iter < 1000) {
        str = "00" + s;
    } else if (1000 <= iter && iter < 10000) {
        str = "0" + s;
    } else {
        str = s;
    }
    
    if (iter >= 0) {
        model_name += str;
    } else {
        model_name += "final";
    }
    
    return model_name;
}

pair<int, double> utils::quicksort_wr(vector<pair<int, double> > vect, int left, int right) {
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
    return vect[0]; 
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



