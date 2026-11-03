CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

all: build/grid_search build/linear_regression build/knn_classifier

build/grid_search: search/grid_search.cpp
	mkdir -p build && $(CXX) $(CXXFLAGS) $< -o $@
build/linear_regression: machine-learning/linear_regression.cpp
	mkdir -p build && $(CXX) $(CXXFLAGS) $< -o $@
build/knn_classifier: machine-learning/knn_classifier.cpp
	mkdir -p build && $(CXX) $(CXXFLAGS) $< -o $@

run: all
	./build/grid_search && ./build/linear_regression && ./build/knn_classifier

clean:
	rm -rf build
