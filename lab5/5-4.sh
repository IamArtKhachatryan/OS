truncate -s 0 mfile.txt
gcc mmap.c -o mmap
./mmap mfile.txt "Hello"

