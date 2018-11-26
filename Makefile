mk_ife:
	g++ main.cpp configuration.cpp -lboost_system -lboost_filesystem -lboost_thread -o ife

CC=$(CXX)

main: main.o configuraiton.o
	g++ main.cpp configuration.cpp -lboost_system -lboost_filesystem -o ife

configuration:	configuration.cpp
		g++ -c configuration.cpp


clean:
	
