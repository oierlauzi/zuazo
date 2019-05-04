#/bin/bash

FLAGS="-std=c++17 -lzuazo -lavutil -lavformat -lavcodec -lswscale -lglfw $(/usr/lib/x86_64-linux-gnu/ImageMagick-6.9.10/bin-q16/Magick++-config --cppflags --cxxflags --ldflags --libs)"

#Example 1
echo "Compiling Example1"
g++ Example1.cpp -o Example1 $FLAGS

#Example 2
echo "Compiling Example2"
g++ Example2.cpp -o Example2 $FLAGS

#Example 3
echo "Compiling Example3"
g++ Example3.cpp -o Example3 $FLAGS

#Example 4
echo "Compiling Example4"
g++ Example4.cpp -o Example4 $FLAGS

#Example 5
echo "Compiling Example5"
g++ Example5.cpp -o Example5 $FLAGS

#Example 6
echo "Compiling Example6"
g++ Example6.cpp -o Example6 $FLAGS