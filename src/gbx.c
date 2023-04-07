#include "gbx.h"

gbx_file* read_gbx_file(char* path) {
    errno = 0;
    FILE* ingbxfile = fopen(path, "rb");
    if(ingbxfile==NULL) {
        printf("Error: could not open file \"%s\", errno: %d\n", path, errno);
        return NULL;
    }
    // Reading the file
    int red = 0;
    gbx_file* gbx = malloc(sizeof(gbx_file));
    char gbxmagic[4] = {0};
    red += fread(&gbxmagic, 3, 1, ingbxfile);
    if(strcmp("GBX", gbxmagic) != 0) {
        fclose(ingbxfile);
        printf("Error: not a GBX file (incorrect magic)\n");
        return NULL;
    }
    if(verbose) printf("GBX file\n");
    red += fread(&gbx->version, sizeof(gbx->version), 1, ingbxfile);
    if(verbose) printf("Version %d\n", gbx->version);
    red += fread(&gbx->type, sizeof(gbx->type), 1, ingbxfile);
    if(verbose) printf("Type %c\n", gbx->type);
    switch(gbx->type) {
        case 'T':
            fclose(ingbxfile);
            printf("Error: text file type not supported\n");
            return NULL;
        case 'B':
            break;
        default:
            fclose(ingbxfile);
            printf("Error: unknown file type\n");
            return NULL;
    }
    red += fread(&gbx->headcompr, sizeof(gbx->headcompr), 1, ingbxfile); // unused, always U
    red += fread(&gbx->bodycompr, sizeof(gbx->bodycompr), 1, ingbxfile);
    if(verbose) printf("Body compressed %c\n", gbx->bodycompr);
    red += fread(&gbx->unknown, sizeof(gbx->unknown), 1, ingbxfile);
    if(verbose) printf("Unknown byte %c\n", gbx->unknown);
    red += fread(&gbx->gbxclass, sizeof(gbx->gbxclass), 1, ingbxfile);
    red += fread(&gbx->headsize, sizeof(gbx->headsize), 1, ingbxfile);
    if(verbose) printf("Header size: %d\n", gbx->headsize);
    gbx->headdata = malloc(gbx->headsize);
    red += fread(gbx->headdata, sizeof(char), gbx->headsize, ingbxfile);

    red += fread(&gbx->numnodes, sizeof(gbx->numnodes), 1, ingbxfile);
    if(verbose) printf("Number of nodes: %d\n", gbx->numnodes);
    red += fread(&gbx->numexnodes, sizeof(gbx->numexnodes), 1, ingbxfile);
    if(verbose) printf("Number of ex nodes: %d\n", gbx->numexnodes);
    gbx->reftabdata = NULL;
    
    if(gbx->numexnodes > 0) {
        int refred = 0; // size of the reference table
        int reftaboffset = ftell(ingbxfile);    // ref tab position
        int anclev; // ancestor level
        refred += fread(&anclev, sizeof(anclev), 1, ingbxfile) * sizeof(anclev);
        if(verbose) printf("Ancestor level %d\n", anclev);
        int subdir; // subdir count
        refred += fread(&subdir, sizeof(subdir), 1, ingbxfile) * sizeof(subdir);
        refred += read_reftab_dir(ingbxfile, subdir);
        refred += read_ref_files(ingbxfile, gbx->numexnodes);
        gbx->reftabdata = malloc(refred);
        fseek(ingbxfile, reftaboffset, SEEK_SET);
        red += fread(gbx->reftabdata, sizeof(char), refred, ingbxfile);
        gbx->reftabsize = refred;
    }


    // BODY!!!

    int curpos;
    int cosize;
    int bsize = 0;
    char* codata = NULL;
    switch(gbx->bodycompr) {
        case 'C':
            red += fread(&gbx->bodysize, sizeof(gbx->bodysize), 1, ingbxfile);
            bsize = gbx->bodysize;
            red += fread(&cosize, sizeof(cosize), 1, ingbxfile);
            if(verbose) printf("Uncompressed size: %d, Compressed size: %d\n", gbx->bodysize, cosize);
            
            codata = malloc(cosize);
            red += fread(codata, sizeof(char), cosize, ingbxfile);
            gbx->bodydata = malloc(bsize);
            if(verbose) printf("Decompressing...\n");
            lzo1x_decompress(codata,cosize,gbx->bodydata,&bsize,NULL);
            
            free(codata);
            break;
        case 'U':
            curpos = ftell(ingbxfile);
            fseek(ingbxfile, 0, SEEK_END);
            int filesize = ftell(ingbxfile);
            fseek(ingbxfile, curpos, SEEK_SET);
            gbx->bodysize = filesize - curpos;
            if(verbose) printf("Uncompressed size: %d\n", gbx->bodysize);
            gbx->bodydata = malloc(gbx->bodysize);
            red = fread(gbx->bodydata, sizeof(char), gbx->bodysize, ingbxfile);
            break;
    }
    fclose(ingbxfile);
    return gbx;
}

void write_gbx_file(char* path, gbx_file* gbx, bool compress) {
    errno = 0;
    FILE* outgbxfile = fopen(path, "wb");
    if(outgbxfile==NULL) {
        printf("Error: could not open file \"%s\", errno: %d\n", path, errno);
    }
    if(verbose) printf("Saving to \"%s\"\n", path);
    int wrote = 0;
    wrote += fwrite("GBX", sizeof(char), 3, outgbxfile);                        // Magic
    wrote += fwrite(&gbx->version, sizeof(gbx->version), 1, outgbxfile);        // Version
    wrote += fwrite(&gbx->type, sizeof(gbx->type), 1, outgbxfile);              // Type (B)
    wrote += fwrite("U", sizeof(char), 1, outgbxfile);                          // Head compress (U)
    if(compress) {                                                              // Body compress
        wrote += fwrite("C", sizeof(char), 1, outgbxfile);                      // (C)
    } else {
        wrote += fwrite("U", sizeof(char), 1, outgbxfile);                      // (U)
    }
    wrote += fwrite(&gbx->unknown, sizeof(char), 1, outgbxfile);                // Unknown (R)

    wrote += fwrite(&gbx->gbxclass, sizeof(gbx->gbxclass), 1, outgbxfile);      // Node class
    wrote += fwrite(&gbx->headsize, sizeof(gbx->headsize), 1, outgbxfile);      // Head size
    wrote += fwrite(gbx->headdata, sizeof(char), gbx->headsize, outgbxfile);    // Head data[Head size]
    
    wrote += fwrite(&gbx->numnodes, sizeof(gbx->numnodes), 1, outgbxfile);      // Num nodes
    wrote += fwrite(&gbx->numexnodes, sizeof(gbx->numexnodes), 1, outgbxfile);  // Num ex nodes
    wrote += fwrite(gbx->reftabdata, sizeof(char), gbx->reftabsize, outgbxfile);// Ref table
    
    if(compress) {
        int compsize;
        char* compdata = malloc( (gbx->bodysize + gbx->bodysize) / 16 + 64 + 3);
        if(verbose) printf("Compressing...\n");
        lzo1x_1_compress(gbx->bodydata, gbx->bodysize, compdata, &compsize, wrkmem);
        if(verbose) printf("Compressed, compressed size %d\n", compsize);
        wrote += fwrite(&gbx->bodysize, sizeof(gbx->bodysize), 1, outgbxfile);
        wrote += fwrite(&compsize, sizeof(compsize), 1, outgbxfile);
        wrote += fwrite(compdata, sizeof(char), compsize, outgbxfile);
        printf("File compressed successfully!\n");
    } else {
        wrote += fwrite(gbx->bodydata, sizeof(char), gbx->bodysize, outgbxfile);
        printf("File decompressed successfully!\n");
    }
    fclose(outgbxfile);
}

void free_gbx_file(gbx_file* gbx) {
    if(gbx->headdata != NULL)   free(gbx->headdata);
    if(gbx->reftabdata != NULL) free(gbx->reftabdata);
    
    
    free(gbx);
}

int read_reftab_dir(FILE* ingbxfile, int count) {
    int red = 0;
    for(int i=0;i<count;i++) {
        int leng;
        red += fread(&leng, sizeof(leng), 1, ingbxfile) * sizeof(leng);
        char* dirname = malloc(leng);
        red += fread(dirname, sizeof(char), leng, ingbxfile);
        int subdir;
        red += fread(&subdir, sizeof(subdir), 1, ingbxfile) * sizeof(subdir);
        red += read_reftab_dir(ingbxfile, subdir);
        free(dirname);
    }
    return red;
}

int read_ref_files(FILE* ingbxfile, int count) {
    int refred = 0;
    for(int i=0;i<count;i++) {
        int flags;
        refred += fread(&flags, sizeof(flags), 1, ingbxfile) * sizeof(flags);
        int leng;
        refred += fread(&leng, sizeof(leng), 1, ingbxfile) * sizeof(leng);
        char* filname = malloc(leng);
        refred += fread(filname, sizeof(char), leng, ingbxfile);
        refred += fread(&flags, sizeof(flags), 1, ingbxfile) * sizeof(flags);  // Useless for this purpose
        refred += fread(&flags, sizeof(flags), 1, ingbxfile) * sizeof(flags);  // Useless for this purpose
        refred += fread(&flags, sizeof(flags), 1, ingbxfile) * sizeof(flags);  // Useless for this purpose
        free(filname);
    }
    return refred;
}
