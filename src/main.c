#include "gbx.h"
#include <argp.h>

#define VER "1.0.8"

enum COMP_MODE {
    AUTO = 0,
    COMPRESS = 1,
    UNCOMPRESS = 2
};

bool verbose = false;
uint8_t compmode = AUTO;
char* inpath = NULL;
char* outpath = NULL;

const char *argp_program_version = "GBXLZO v" VER " by GreffMASTER";
const char *argp_program_bug_address = ""; //Insert an appropriate E-mail address here
static char doc[] = "A tool for decompressing/compressing GameBox files. (*.Gbx)";
static char args_doc[] = "<infile>";
static struct argp_option options[] = {
    {"compress",   'c', 0,           0, "only compress"},
    {"uncompress", 'u', 0,           0, "only decompress"},
    {"output",     'o', "FILE", 0, "output path. If not provided, the input file will be used as output"},
    {"verbose",    'v', 0,           0, "verbose"},
    { 0 }
};

error_t argp_parser (int key, char *arg, struct argp_state *state){
    switch(key){
        case 'v':
            verbose = true;
            break;

        case 'c':
            compmode = COMPRESS;
            break;

        case 'd':
            compmode = UNCOMPRESS;
            break;

        case 'o':
            outpath = arg;
            break;

        case ARGP_KEY_NO_ARGS: //No arguments
            argp_usage(state); //No break, this exits

        case ARGP_KEY_ARG: //Input file name
            inpath = arg;
            state->next = state->argc;
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }       
    return 0;
}

static struct argp argp = { options, argp_parser, args_doc, doc, 0, 0, 0 };

int main(int argc, char** argv) {
    
    argp_parse(&argp, argc, argv, 0, 0, 0);
    if(outpath==NULL) outpath = inpath; // if no output is provided, use input as output (replace the file)
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
