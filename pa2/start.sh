rm -rf a.out
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/home/tynrol/Code/pa2/libruntime.so";
echo "----------------------------------------------------------------------------------------"
clang -std=c99 -Wall -pedantic *.c -L. -lruntime
echo "----------------------------------------------------------------------------------------"
LD_PRELOAD=/home/tynrol/Code/CLionProjects/distributed_labs/pa2/libruntime.so ./a.out -p 2 10 20