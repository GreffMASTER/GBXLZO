# GBXLZO
### A tool for decompressing/compressing GameBox files. (*.Gbx)


Usage (Windows): `gbxlzo <infile> [params]`  
Parameters:  
- `-c`          -  only compress
- `-d`          -  only decompress
- `-o <path>`   -  output path
- `-v`          -  verbose  
  
Usage (Linux): `gbxlzo [OPTION...] <infile>`  
Parameters:  
- `c`, `--compress`             only compress
- `d`, `--decompress`           only decompress
- `o`, `--output=FILE`          output path
- `v`, `--verbose`              verbose
- `?`, `--help`                 Give this help list
- `--usage`                     Give a short usage message
- `V`, `--version`              Print program version  
  
Examples:  
`gbxlzo Race01.Challenge.Gbx -d -v -o Race01.Uncompressed.Challenge.Gbx`  
`gbxlzo Race08.Challenge.Gbx`  
  
If output path is not provided, the input file will be used as output.  
By default, the program will decompress compressed files and vice versa.  
You can force the program to only de/compress the file using the `-d` and `-c` parameters.  

## miniLZO  
This project uses the miniLZO library. You can read the license [here](https://github.com/GreffMASTER/GBXLZO/blob/main/LZOLICENSE).  

## Compile instructions  
This project requires gcc compiler, make and cmake.  

```
$ git clone https://github.com/GreffMASTER/GBXLZO/  
$ mkdir build && cd build  
$ cmake ..    
$make
```
