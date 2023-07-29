#include "gbx.h"

#define VER "1.1.0"

enum COMP_MODE {
    AUTO = 0,
    COMPRESS = 1,
    DECOMPRESS = 2
};

bool verbose = false;
uint8_t compmode = AUTO;
char* inpath = NULL;
char* outpath = NULL;
const char* comp_str = "Compressing";
const char* decomp_str = "Decompressing";
const char* auto_str = "De/Compressing";

#ifdef _WIN32 // use legacy command line parser ( no gnulib on windows :( )
    void prnt_help() {
        printf("GBXLZO v%s by GreffMASTER\n", VER);
        printf("A tool for decompressing/compressing GameBox files. (*.Gbx)\n");
        printf("Usage: gbxlzo.exe <infile> [params]\n\n");
        printf("Parameters:\n");
        printf("-c         -  only compress\n");
        printf("-d         -  only decompress\n");
        printf("-o <path>  -  output path\n");
        printf("-v         -  verbose\n\n");
        printf("If output path is not provided, the input file will be used as output\n");
        printf("By default, the program will decompress compressed files and vice versa\n");
        printf("If you find any bugs, please contact me at mrdsdrdv@gmail.com\n");
    }

    int parse_args_legacy(int argc, char** argv) {
        int requiredargs = 2;
        if(argc<2) {
            prnt_help();
            return 1;
        }
        inpath = argv[1];
        for(int i=2;i<argc;i++) {
            if(strcmp(argv[i], "-v") == 0) {
                requiredargs += 1;
                verbose = true;
            } else if(strcmp(argv[i], "-c") == 0) {
                requiredargs += 1;
                compmode = COMPRESS;
            } else if(strcmp(argv[i], "-d") == 0) {
                requiredargs += 1;
                compmode = DECOMPRESS;
            } else if(strcmp(argv[i], "-o") == 0) {
                requiredargs += 2;
                if(argc<requiredargs) {
                    printf("-o parameter requires a path\n");
                    prnt_help();
                    return 1;
                }
                outpath = argv[i+1];
            }
        }
        return 0;
    }
#elif __linux__ // linux
    #include <argp.h>

    const char *argp_program_version = "GBXLZO v" VER " by GreffMASTER";
    const char *argp_program_bug_address = "mrdsdrdv@gmail.com";
    static char doc[] = "A tool for decompressing/compressing GameBox files. (*.Gbx)";
    static char args_doc[] = "<infile>";
    static struct argp_option options[] = {
        {"compress",   'c', 0,           0, "only compress"},
        {"decompress", 'd', 0,           0, "only decompress"},
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
                compmode = DECOMPRESS;
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
#endif

int main(int argc, char** argv) {

    #ifdef _WIN32 // use legacy command line argument parser ( no gnulib on windows :( )
        if(parse_args_legacy(argc, argv) > 0)
            return 0;
    #elif __linux__ // linux
        argp_parse(&argp, argc, argv, 0, 0, 0);
    #endif

    if(outpath==NULL) outpath = inpath; // if no output is provided, use input as output (replace the file)
    if (lzo_init() != LZO_E_OK) {
        printf("Error: failed to initialize LZO\n");
        return 3;
    }
    char* mode_msg = NULL;
    if(compmode == AUTO)
        mode_msg = auto_str;
    else if(compmode == COMPRESS)
        mode_msg = comp_str;
    else
        mode_msg = decomp_str;
    printf("%s \"%s\" -> \"%s\"\n",mode_msg, inpath, outpath);
    gbx_file* gbx = read_gbx_file(inpath);

    if(gbx!=NULL) {
        int retcode = 0;
        if(compmode == AUTO) {
            if(gbx->bodycompr=='C')
                retcode = write_gbx_file(outpath, gbx, false);
            if(gbx->bodycompr=='U')
                retcode = write_gbx_file(outpath, gbx, true);
        }
        else if(compmode == COMPRESS)
            retcode = write_gbx_file(outpath, gbx, true);
        else if(compmode == DECOMPRESS)
            retcode = write_gbx_file(outpath, gbx, false);
        if(retcode > 0) {
            printf("Writing failed!\n");
            return 2;
        }
        free_gbx_file(gbx);
    } else {
        printf("Parsing failed!\n");
        return 1;
    }
    return 0;
}
