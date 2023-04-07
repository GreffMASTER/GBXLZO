#include "gbx.h"

enum COMP_MODE {
    AUTO = 0,
    COMPRESS = 1,
    UNCOMPRESS = 2
};

bool verbose = false;
uint8_t compmode = AUTO;

const char* VER = "1.0.4";

void prnt_help() {
    printf("GBXLZO v%s by GreffMASTER\n", VER);
    printf("A tool for decompressing/compressing GameBox files. (*.Gbx)\n");
    printf("Usage: gbxlzo <infile> [params]\n");
    printf("Parameters:\n");
    printf("-v         -  verbose\n");
    printf("-c         -  only compress\n");
    printf("-u         -  only decompress\n");
    printf("-o <path>  -  output path");
    printf("If output path is not provided, the input file will be used as output");
    printf("By default, the program will decompress compressed files and vice versa");
}

int main(int argc, char** argv) {
    char* inpath = NULL;
    char* outpath = NULL;
    int requiredargs = 2;
    if(argc<2) {
        prnt_help();
        return 0;
    }
    inpath = argv[1];
    outpath = argv[1];
    for(int i=2;i<argc;i++) {
        if(strcmp(argv[i], "-v") == 0) {
            requiredargs += 1;
            verbose = true;
        } else if(strcmp(argv[i], "-c") == 0) {
            requiredargs += 1;
            compmode = COMPRESS;
        } else if(strcmp(argv[i], "-u") == 0) {
            requiredargs += 1;
            compmode = UNCOMPRESS;
        } else if(strcmp(argv[i], "-o") == 0) {
            requiredargs += 2;
            if(argc<requiredargs) {
                printf("-o parameter requires a path");
                prnt_help();
                return 0;
            }
            outpath = argv[i+1];
        }
    }
    if (lzo_init() != LZO_E_OK) {
        printf("Error: failed to initialize LZO\n");
        return 3;
    }
    printf("\"%s\" -> \"%s\"\n", inpath, outpath);
    gbx_file* gbx = read_gbx_file(inpath);
    if(gbx!=NULL) {
        if(compmode == AUTO) {
            if(gbx->bodycompr=='C') write_gbx_file(outpath, gbx, false);
            if(gbx->bodycompr=='U') write_gbx_file(outpath, gbx, true);
        }
        else if(compmode == COMPRESS) write_gbx_file(outpath, gbx, true);
        else if(compmode == UNCOMPRESS) write_gbx_file(outpath, gbx, false);
        free_gbx_file(gbx);
    } else {
        printf("Failed to read the file \"%s\"\n", inpath);
    }
    return 0;
}
