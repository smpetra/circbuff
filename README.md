## How to build:
    $> make


## How to clean:
    $> make clean


## How to run:
    $> ./circbuff <test_file> [-v]  
test_file : file that will be used for feeding the circular buffer  
-v : use for verbose mode, optional  
### Example:
    ./circbuff circbuff.c -v


## How to change maximum buffer size:
Open Makefile and change value of the preprocessor definition MAX_CIRCBUFF_SIZE to desired value,
e.g. to change the max buffer size to 128 use `-DMAX_CIRCBUFF_SIZE=128`.  
Provided max value must be power of two.


## Notes:
* All commands are assumed to be ran from project root.
* Tested on Linux (lubuntu), but should work in any system with make and gcc.
