gcc writer-reader/writer_mmap.c writer-reader/binary_sems.c -o writer_mmap
gcc writer-reader/reader_mmap.c writer-reader/binary_sems.c -o reader_mmap
echo "Hello hello" > input.txt
./writer_mmap < input.txt &
./reader_mmap > output.txt
