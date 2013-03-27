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
        // parameter estimation
        lda.estimate();
    }
    
    if (lda.model_status == MODEL_STATUS_INF) {
        // do inference
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
    printf("Command line usage:\n");
    printf("\tlda -pprocess <string> <int>\n");
    printf("\tlda -est -alpha <double> -beta <double> -ntopics <int> -niters <int> -savestep <int> -twords <int> -dfile <string>\n");
    printf("\tlda -estc -dir <string> -model <string> -niters <int> -savestep <int> -twords <int>\n");
    printf("\tlda -inf -dir <string> -model <string> -niters <int> -twords <int> -dfile <string>\n");
    // printf("\tlda -inf -dir <string> -model <string> -niters <int> -twords <int> -dfile <string> -withrawdata\n");
}

