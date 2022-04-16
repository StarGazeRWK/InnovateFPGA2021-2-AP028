myapp: mainFPGA.o mainImage.o mainOdour.o mainCapacitive.o mainProject.o
	g++ -o myapp mainFPGA.o mainImage.o mainOdour.o mainCapacitive.o mainProject.o -std=c++11 `pkg-config --libs opencv`

mainProject.o: mainProject.cpp
	g++ -c -o mainProject.o mainProject.cpp -Wall -std=c++11

mainOdour.o: mainOdour.cpp
	g++ -c -o mainOdour.o mainOdour.cpp -Wall -std=c++11 `pkg-config --cflags opencv`

mainCapacitive.o: mainCapacitive.cpp
	g++ -c -o mainCapacitive.o mainCapacitive.cpp -Wall -std=c++11 `pkg-config --cflags opencv`

mainImage.o: mainImage.cpp
	g++ -c -o mainImage.o mainImage.cpp -Wall -std=c++11 `pkg-config --cflags opencv`

mainFPGA.o: mainFPGA.c
	gcc -c -o mainFPGA.o mainFPGA.c -Dsoc_cv_av

.PHONY: clean
clean:
	rm -f *.a *.o *~ 
	rm -f myapp 
