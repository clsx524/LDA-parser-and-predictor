setwd("/Users/eric/Dropbox/GitHub/LDA-parser-and-predictor/model")
ALPHA <- read.table("alpha.txt", head = FALSE)
ALPHA.mean <- apply(Y[1001:3000,], 2 ,mean)
