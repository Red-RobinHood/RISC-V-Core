all: sim_tb

sim_tb: sim.o assembler.o sim_tb.o
	g++ sim.o assembler.o sim_tb.o -o sim_tb

sim_tb.o: sim_tb.cpp
	g++ -c sim_tb.cpp -o sim_tb.o

sim.o: sim.cpp
	g++ -c sim.cpp -o sim.o

assembler.o: assembler.cpp
	g++ -c assembler.cpp -o assembler.o

clean:
	rm -f sim_tb.o sim.o assembler.o sim_tb output.hex
