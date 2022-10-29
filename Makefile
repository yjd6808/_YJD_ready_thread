src_files := $(wildcard *.cpp)
obj_files := $(src_files:.cpp=.o)

main: $(obj_files) 
	g++ -o $@ $< -lpthread -std=c++20

%.o: %.cpp
	g++ -o $@ -c $<

clean:
	rm -f *.o main


	
