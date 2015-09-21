compile: contractpd.cpp Constants.cpp ./Include/Fcts.h ./Include/Constants.h
	g++ -I /usr/include/gsl -I ./Include contractpd.cpp Constants.cpp -lgsl -lgslcblas -lm -Wall -O2

clean: 
	rm *~
	rm ./Include/*~
	rm *.txt
