LDA parser and predictor
==========

Before using this program, you should first install boost C++ library to support regex and mysql to support database access.

## preprocess the files
	./lda -pprocess info 1 // generate training files
	./lda -pprocess test2 2 // generate predict files
	
## training 
	./lda -est -alpha 0.7912 -beta 0.00464 -ntopics 50 -niters 1000 -savestep 10 -twords 20 -dfile model/trndata.txt
	
## prediction
	./lda -inf -dir model -model model-final -niters 30 -twords 20 -dfile model/predata.txt
	
## ranking
	./lda -ranking 78 -disp 10 -dir model -model model-final

## classification
	./lda -class -dir model -model model-final
	
## server
	./lda -server 6000 -disp 12 -dir model -model model-final -niters 5 -twords 20 -dfile predata.txt
	

1000 group
10000 iterations, topics size 100init: alpha = 0.8, beta = 0.1thinning interval = 10 iterationsposterior mean for α is 0.050982006.posterior mean for β is 0.002609193.95% credential interval for α is [0.0502137 0.0519432]. 
95% credential interval for β is [0.00259244 0.00262603].

5600 group
10000 iterations, topics size 100init: alpha = 0.8, beta = 0.1thinning interval = 10 iterationsposterior mean for α is 0.026575741.posterior mean for β is 0.002815483.95% credential interval for α is [0.0262913 0.0268430]. 
95% credential interval for β is [0.00280798 0.00282316].