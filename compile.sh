g++ -Wall -Wextra -O2 -std=c++17 -c encstrset.cc -o encstrset.o
gcc -Wall -Wextra -O2 -std=c11 -c encstrset_test1.c -o encstrset_test1.o
g++ -Wall -Wextra -O2 -std=c++17 -c encstrset_test2.cc -o encstrset_test2.o
g++ encstrset_test1.o encstrset.o -o encstrset_test1
g++ encstrset_test2.o encstrset.o -o encstrset_test2
