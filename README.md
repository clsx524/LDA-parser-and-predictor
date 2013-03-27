590Project
==========

## preprocess the files
	./lda -pprocess test1 1 // generate training files
	./lda -pprocess test2 2 // generate predict files
	
## training 
	./lda -est -alpha 0.5 -beta 0.1 -ntopics 3 -niters 1000 -savestep 100 -twords 20 -dfile model/trndata.txt
	
## prediction
	./lda -inf -dir model -model model-final -niters 30 -twords 20 -dfile model/predata.txt
	
## ranking
	./lda -ranking <int> -disp <int> -dir model -model model-final
