#ifndef _STRTOKENIZER_H
#define _STRTOKENIZER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <assert.h>

using namespace std;

class strtokenizer {
protected:
    vector<string> tokens;
    vector<string> stopList;
    bool stopListRead;

    class stemmer {
	public:
  		string str;
  		int j; // general offset
        stemmer() : j(0) {}
        stemmer(const string& s) : str(s), j(0) {}
        ~stemmer() {}
	};

	stemmer* z;

public:
    strtokenizer() : stopListRead(false), z(NULL) {
        if (stopListRead == false) {
            read_stop_list();
        }
    }  
    ~strtokenizer() {
        delete z;
    }

    void split(string str, string seperators = " ", bool preEnable = false);
    void parse(string str, string seperators);
    void preprocess(string text);
    void addToken(const string& str);
    
    vector<string>::size_type count_tokens();

    string token(int i);
    void clear();
    void print();

    // read stop list
    void read_stop_list();
    string stopword_remover(string str);
    string verify(string str);

    // stemming processes
    bool cons(int i);
    int m();
    bool vowelinstem();
    bool doublec(int j);
    bool cvc(int i);
    bool ends(string s);
    void setto(string s);
    void r(string s);
    void step1ab();
    void step1c();
    void step2();
    void step3();
    void step4();
    void step5();
    string stem(string b);
};

#endif

