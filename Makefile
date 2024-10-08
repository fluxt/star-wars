
# g++ src/main.cpp lodepng/lodepng.cpp -std=c++20 -I./src -I./lodepng -I./glm -g -ffast-math -Wall -Wunused -Wshadow=local -Wdouble-promotion -o main
all:
	g++ src/main.cpp lodepng/lodepng.cpp -std=c++20 -I./src -I./lodepng -I./glm -O3 -ffast-math -Wall -Wunused -Wshadow=local -Wdouble-promotion -o main

clean:
	rm main
	rm output.png
