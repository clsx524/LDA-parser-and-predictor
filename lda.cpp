#include "model.h"

using namespace std;

void show_help();

int main(int argc, char ** argv) {
    model lda;
    
    if (lda.init(argc, argv)) {
        show_help();
        return 1;
    }

    if (lda.model_status == MODEL_STATUS_EST || lda.model_status == MODEL_STATUS_ESTC) {
        lda.estimate();
    }
    
    if (lda.model_status == MODEL_STATUS_INF) {
        lda.inference();
    }

    if (lda.model_status == MODEL_STATUS_PREPROCESS) {
        lda.preprocess();
    }

    if (lda.model_status == MODEL_STATUS_RANKING) {
        lda.ranking();
    }
    
    return 0;
}

void show_help() {
    cout << "Command line usage:" << endl;
    cout << '\t' << "lda -pprocess <string> <int>" << endl;
    cout << '\t' << "lda -est -alpha <double> -beta <double> -ntopics <int> -niters <int> -savestep <int> -twords <int> -dfile <string>" << endl;
    cout << '\t' << "lda -estc -dir <string> -model <string> -niters <int> -savestep <int> -twords <int>" << endl;
    cout << '\t' << "lda -inf -dir <string> -model <string> -niters <int> -twords <int> -dfile <string>" << endl;
    cout << '\t' << "lda -ranking <int> -disp <int> -dir <string> -model <string>" << endl;
    // printf("\tlda -inf -dir <string> -model <string> -niters <int> -twords <int> -dfile <string> -withrawdata\n");
}

