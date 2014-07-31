#ifndef TGAHEADER_H_INCLUDED
#define TGAHEADER_H_INCLUDED

#include <gl/gl.h>

typedef struct _TGAHeader TGAHeader;

struct __attribute__((__packed__)) _TGAHeader {
    GLbyte id_len;
	GLbyte map_t;
	GLbyte img_t;
	GLshort map_first;
	GLshort map_len;
	GLbyte map_entry;
	GLshort x;
	GLshort y;
	GLshort width;
	GLshort height;
	GLbyte depth;
	GLbyte alpha;
};

#endif // TGAHEADER_H_INCLUDED
