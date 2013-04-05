#ifndef _UTILS_H
#define _UTILS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <cmath>
#include <boost/regex.hpp>
#include "strtokenizer.h"
#include "model.h"

using namespace std;

class model;

class utils {
public:
    // parse command line arguments
    static int parse_args(int argc, char ** argv, model * pmodel);
    
    // read and parse model parameters from <model_name>.others
    static int read_and_parse(string filename, model * model); 
    
    // generate the model name for the current iteration
    // iter = -1 => final model
    static string generate_model_name(int iter); 

    static double stod(const string& str); 
    
    // sort    
    static pair<int, double> quicksort_wr(vector<pair<int, double> > vect, int left, int right);
    static void quicksort(vector<pair<int, double> > & vect, int left, int right);

    static void readfile(string ofile, ofstream& fout, strtokenizer& strtok);
    static void readfile(string ofile, strtokenizer& strtok);

    static void addfile(string name, vector<string>& pathset, vector<string>::size_type& size);
    static void addfile(string name, ofstream& fout, strtokenizer& strtok, vector<pair<string, int> >& classes, int& size);
};

#endif

