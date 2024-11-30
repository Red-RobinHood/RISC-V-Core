all: sim_tb.o sim.o assembler.o
	g++ -c sim.cpp -o sim.o
	g++ -c sim_tb.cpp -o sim_tb.o
	g++ -c assembler.cpp -o assembler.o
	g++ assembler.o sim.o sim_tb.o -o sim_tb
	rm -f *.o
