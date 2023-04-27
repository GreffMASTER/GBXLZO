# GBXLZO
### A tool for decompressing/compressing GameBox files. (*.Gbx)

Usage: `gbxlzo <infile> [params]`  
Parameters:  
- `-v`  -  verbose
- `-c`  -  only compress
- `-u`  -  only uncompress
- `-o <path>`  -  output path  

Examples:  
`gbxlzo Race01.Challenge.Gbx -u -v -o Race01.Uncompressed.Challenge.Gbx`  
`gbxlzo Race08.Challenge.Gbx`  
  
If output path is not provided, the input file will be used as output.
By default, the program will decompress compressed files and vice versa.

## miniLZO  
This project uses the miniLZO library. You can read the license [here](https://github.com/GreffMASTER/GBXLZO/blob/main/LZOLICENSE).  

## Compile instructions  
This project requires gcc compiler, make and cmake.  

`git clone https://github.com/GreffMASTER/GBXLZO/`  
`mkdir build && cd build`  
`cmake ..`  
`make`
