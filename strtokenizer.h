/*
 * Copyright (C) 2007 by
 * 
 * 	Xuan-Hieu Phan
 *	hieuxuan@ecei.tohoku.ac.jp or pxhieu@gmail.com
 * 	Graduate School of Information Sciences
 * 	Tohoku University
 *
 * GibbsLDA++ is a free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * GibbsLDA++ is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GibbsLDA++; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#ifndef _STRTOKENIZER_H
#define _STRTOKENIZER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <boost/regex.hpp>
#include <algorithm>
#include <assert.h>

using namespace std;

class strtokenizer {
protected:
    vector<string> tokens;
    vector<string> stopList;
    int idx;

    class stemmer {
	public:
  		char* str;
  		int k;
  		int j;
	};

	stemmer* z;

public:
    strtokenizer(string str, string seperators = " ", bool preEnable = false);    
    
    void parse(string str, string seperators);
    void preprocess(string text);
    
    int count_tokens();
    string next_token();   
    void start_scan();

    string token(int i);

    // read stop list
    void read_stop_list();
    string stopword_remover(string str);
    string verify(string str);

    // stemming processes
    bool cons(stemmer* z, int i);
    int m(stemmer* z);
    bool vowelinstem(stemmer* z);
    bool doublec(stemmer* z, int j);
    bool cvc(stemmer * z, int i);
    bool ends(stemmer * z, const char * s);
    void setto(stemmer * z, const char * s);
    void r(stemmer * z, const char * s);
    void step1ab(stemmer * z);
    void step1c(stemmer * z);
    void step2(stemmer * z);
    void step3(stemmer * z);
    void step4(stemmer * z);
    void step5(struct stemmer * z);
    string stem(stemmer * z, string b, int k);
};

#endif

