#if !defined(__MEM_H)
#define __MEM_H
#include <stddef.h>
#include <stdbool.h>
#include "mem_os.h"

/* -----------------------------------------------*/
/* Interface utilisateur de votre allocateur      */
/* -----------------------------------------------*/

void mem_init(void);
void* mem_alloc(size_t);
void mem_free(void*);
size_t mem_get_size(void *);
bool adjoining_block_rb(rb_t * , rb_t*);
bool is_adjoining_block_fb(fb_t *, fb_t *);
void update_rb(fb_t *, bool);
void update_fb(fb_t *);


void right_fusion(fb_t*);
fb_t* left_fusion(fb_t*);


/* Itérateur sur le contenu de l'allocateur */
void mem_show(void (*print)(void *, size_t, int free));

#endif
