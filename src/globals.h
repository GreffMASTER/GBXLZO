#ifndef __GLOBALS_H_INCLUDED
#define __GLOBALS_H_INCLUDED 1

enum COMP_MODE {
    AUTO = 0,
    COMPRESS = 1,
    UNCOMPRESS = 2
};

extern bool verbose;
extern uint8_t compmode;

#endif /* already included */
