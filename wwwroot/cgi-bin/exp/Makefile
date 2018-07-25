.PHONY:test
all:exp test
exp:main.cpp expSolution.cpp
	g++ -o $@ $^ -lstdc++

test:test.cpp expSolution.cpp
	g++ -o $@ $^ -lstdc++

.PHONY:clean
clean:
	rm exp 
