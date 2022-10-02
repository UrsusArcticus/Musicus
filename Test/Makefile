MusicusTests: Tests.o Library.spec.o Library.o Utils.o
	g++ Tests.o Library.spec.o Library.o Utils.o /usr/lib/libid3.so -o MusicusTests

Tests.o: Tests.cpp
	g++ -c Tests.cpp

Library.spec.o: Library.spec.cpp
	g++ -c Library.spec.cpp

Library.o: ../Source/Library.cpp
	g++ -c ../Source/Library.cpp

Utils.o: ../Source/Utils.cpp
	g++ -c ../Source/Utils.cpp

clean:
	rm *.o MusicusTests

target: dependencies
	action
