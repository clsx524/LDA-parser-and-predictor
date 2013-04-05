#include "constants.h"
#include "strtokenizer.h"
#include "dataset.h"

using namespace std;

// write the mapword2id into a file
// input : filename and mapword2id
int dataset::write_wordmap(string wordmapfile, mapword2id * pword2id) {
    ofstream fout;
    fout.open(wordmapfile.c_str(), ofstream::out);
    if (!fout.is_open()) {
        cout << "Cannot open file " <<  wordmapfile << " to write!" << endl;
        return 1;
    }
    
    fout << pword2id->size() << endl;
    for (mapword2id::iterator it = pword2id->begin(); it != pword2id->end(); it++) {
        fout << it->first << " " << it->second << endl;
    }
    
    fout.close();
    return 0;
}

// write data of file into a pword2id
int dataset::read_wordmap(string wordmapfile, mapword2id * pword2id) {
    pword2id->clear();
    strtokenizer strtok;
    ifstream fin;
    fin.open(wordmapfile.c_str(), ifstream::in);
    if (!fin.is_open()) {
		cout << "Cannot open file " <<  wordmapfile << " to read!" << endl;
		return 1;
    }
    
    string line;
    
    getline(fin, line);
    int nwords = atoi(line.c_str());
    
    for (long i = 0; i < nwords; i++) {
		getline(fin, line);
		strtok.split(line, " \t\r\n", false);
		if (strtok.count_tokens() != 2) {
	    	continue;
		}
        
		pword2id->insert(pair<string, int>(strtok.token(0), atoi(strtok.token(1).c_str())));
    	strtok.clear();
    }
    
    fin.close();
    return 0;
}

int dataset::read_wordmap(string wordmapfile, mapid2word * pid2word) {
    pid2word->clear();
    strtokenizer strtok;
    ifstream fin; 
    fin.open(wordmapfile.c_str(), ifstream::in);
    if (!fin.is_open()) {
        cout << "Cannot open file " <<  wordmapfile << " to read!" << endl;
        return 1;
    }
    
    string line;
    
    getline(fin, line);
    int nwords = atoi(line.c_str());

    for (long i = 0; i < nwords; i++) {
        getline(fin, line);
        strtok.split(line, " \t\r\n", false);
        if (strtok.count_tokens() != 2) {
            continue;
        }
        
        pid2word->insert(pair<int, string>(atoi(strtok.token(1).c_str()), strtok.token(0)));
    	strtok.clear();
    }
    
    fin.close();
    return 0;
}

int dataset::read_trndata(string classfile, string dfile, string wordmapfile, vector<pair<string, int> >& movie_classes) {
   
    mapword2id word2id;

    ifstream fin;
    string line;
    strtokenizer strtok;

    fin.open(classfile.c_str(), ifstream::in);
    if (!fin.is_open()) {
        cout << "Cannot open file " <<  classfile << " to read!" << endl;
        return 1;
    }

    while(!fin.eof()) {
        getline(fin, line);
        strtok.parse(line, " \t\r\n");
        movie_classes.push_back(pair<string, int>(strtok.token(0), atoi(strtok.token(1).c_str())));
        cout << movie_classes[movie_classes.size()-1].first << " " << movie_classes[movie_classes.size()-1].second << endl;
        strtok.clear();
    }
    fin.close();

    fin.open(dfile.c_str(), ifstream::in);
    if (!fin.is_open()) {
		cout << "Cannot open file " <<  dfile << " to read!" << endl;
		return 1;
    }
    
    mapword2id::iterator it;
    
    
    // get the number of documents
    getline(fin, line);
    M = atoi(line.c_str());

    if (M <= 0) {
		cout << "No document available!" << endl;
		return 1;
    }
    
    // allocate memory for corpus
    if (docs) {
		deallocate();
    } else {
		docs = new document*[M];
    }
    
    // set number of words to zero
    V = 0;
    
    for (int i = 0; i < M; i++) {
		getline(fin, line);
        cout << i << endl;
		strtok.split(line, " \t\r\n", false);
		int length = strtok.count_tokens();
        
		if (length <= 0) {
	    	cout << "Invalid (empty) document!" << endl;
	    	deallocate();
	    	M = V = 0;
	    	return 1;
		}
        
		// allocate new document
		document * pdoc = new document(length);
        
		for (int j = 0; j < length; j++) {
	    	it = word2id.find(strtok.token(j));
	    	if (it == word2id.end()) {
                // word not found, i.e., new word
                pdoc->words[j] = word2id.size();
                word2id.insert(pair<string, int>(strtok.token(j), word2id.size()));
	    	} else {
				pdoc->words[j] = it->second;
	    	}
		}
        
		// add new doc to the corpus
		add_doc(pdoc, i);
		strtok.clear();
    }
    
    fin.close();
    
    // write word map to file
    if (write_wordmap(wordmapfile, &word2id)) {
		return 1;
    }
    
    // update number of words
    V = word2id.size();
    
    return 0;
}

int dataset::read_newdata(string dfile, string wordmapfile) {
    mapword2id word2id;
    map<int, int> id2_id;
    
    read_wordmap(wordmapfile, &word2id);
    if (word2id.size() <= 0) {
		cout << "No word map available!" << endl;
		return 1;
    }
    
    ifstream fin;
    fin.open(dfile.c_str(), ifstream::in);
    if (!fin.is_open()) {
		cout << "Cannot open file " <<  dfile << " to read!" << endl;
		return 1;
    }
    
    mapword2id::iterator it;
    map<int, int>::iterator _it;
    string line;
    
    // get number of new documents
    getline(fin, line);
    M = atoi(line.c_str());
    
    if (M <= 0) {
		cout << "No document available!" << endl;
		return 1;
    }
    
    // allocate memory for corpus
    if (docs) {
		deallocate();
    } else {
		docs = new document*[M];
    }
    _docs = new document*[M];
    
    // set number of words to zero
    V = 0;
    strtokenizer strtok;
    for (int i = 0; i < M; i++) {
		getline(fin, line);
		strtok.split(line, " \t\r\n",false);
		int length = strtok.count_tokens();
        
		vector<int> doc;
		vector<int> _doc;
		for (int j = 0; j < length; j++) {
	    	it = word2id.find(strtok.token(j));
	    	if (it == word2id.end()) {
                // word not found, i.e., word unseen in training data
                // do anything? (future decision)
	    	} else {
				int _id;
				_it = id2_id.find(it->second);
				if (_it == id2_id.end()) {
		    		_id = id2_id.size();
		    		id2_id.insert(pair<int, int>(it->second, _id));
		    		_id2id.insert(pair<int, int>(_id, it->second));
				} else {
		    		_id = _it->second;
				}
                
				doc.push_back(it->second);
				_doc.push_back(_id);
	    	}
		}
        
		// allocate memory for new doc
		document * pdoc = new document(doc);
		document * _pdoc = new document(_doc);
        
		// add new doc
		add_doc(pdoc, i);
		_add_doc(_pdoc, i);
		strtok.clear();
    }
    
    fin.close();
    
    // update number of new words
    V = id2_id.size();
    
    return 0;
}

int dataset::read_newdata_withrawstrs(string dfile, string wordmapfile) {
    mapword2id word2id;
    map<int, int> id2_id;
    
    read_wordmap(wordmapfile, &word2id);
    if (word2id.size() <= 0) {
		cout << "No word map available!" << endl;
		return 1;
    }
    
    ifstream fin;
    fin.open(dfile.c_str(), ifstream::in);
    if (!fin.is_open()) {
		cout << "Cannot open file " <<  dfile << " to read!" << endl;
		return 1;
    }
    
    mapword2id::iterator it;
    map<int, int>::iterator _it;
    string line;
    
    // get number of new documents
    getline(fin, line);
    M = atoi(line.c_str());
    
    if (M <= 0) {
		cout << "No document available!" << endl;
		return 1;
    }
    
    // allocate memory for corpus
    if (docs) {
		deallocate();
    } else {
		docs = new document*[M];
    }
    _docs = new document*[M];
    
    // set number of words to zero
    V = 0;
    strtokenizer strtok;
    for (int i = 0; i < M; i++) {
		getline(fin, line);
		strtok.split(line, " \t\r\n", false);
		int length = strtok.count_tokens();
        
		vector<int> doc;
		vector<int> _doc;
		for (int j = 0; j < length - 1; j++) {
	    	it = word2id.find(strtok.token(j));
	    	if (it == word2id.end()) {
                // word not found, i.e., word unseen in training data
                // do anything? (future decision)
	    	} else {
				int _id;
				_it = id2_id.find(it->second);
				if (_it == id2_id.end()) {
		    		_id = id2_id.size();
		    		id2_id.insert(pair<int, int>(it->second, _id));
		    		_id2id.insert(pair<int, int>(_id, it->second));
				} else {
		    		_id = _it->second;
				}
                
				doc.push_back(it->second);
				_doc.push_back(_id);
	    	}
		}
        
		// allocate memory for new doc
		document * pdoc = new document(doc, line);
		document * _pdoc = new document(_doc, line);
        
		// add new doc
		add_doc(pdoc, i);
		_add_doc(_pdoc, i);
		strtok.clear();
    }
    
    fin.close();
    
    // update number of new words
    V = id2_id.size();
    
    return 0;
}

