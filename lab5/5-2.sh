gcc mmap.c -o mmap

truncate -s 1024 mfile.txt     # 1KB, լրիվ զրոներով
./mmap mfile.txt "Hello world"
