# GBXLZO
### A tool for decompressing/compressing GameBox files. (*.Gbx)

Usage: `gbxlzo <infile> [params]`  
Parameters:  
- `-v`  -  verbose
- `-c`  -  only compress
- `-u`  -  only uncompress
- `-o <path>`  -  output path  
  
If output path is not provided, the input file will be used as output.
By default, the program will decompress compressed files and vice versa.

## Compile instructions  
This project requires gcc compiler, make and cmake.  

`git clone https://github.com/GreffMASTER/GBXLZO/`  
`mkdir build && cmake ..`  
`make`
