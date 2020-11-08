mkdir -p build

g++ -Wall -Wextra -O2 -std=c++17 -c src/encstrset.cc -o encstrset.o
gcc -Wall -Wextra -O2 -std=c11 -c specyfikacja/encstrset_test1.c -o encstrset_test1.o
g++ encstrset_test1.o encstrset.o -o build/encstrset_test1

g++ -Wall -Wextra -O2 -std=c++17 -c specyfikacja/encstrset_test2.cc -o encstrset_test2.o
g++ encstrset_test2.o encstrset.o -o build/encstrset_test2

rm -f *.o