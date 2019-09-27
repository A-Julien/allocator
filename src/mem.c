
#include "mem.h"
#include "mem_os.h"
#include "common.h"
#include <stdio.h>

//-------------------------------------------------------------
// mem_init
//-------------------------------------------------------------
void mem_init() {

  void* memory = get_memory_adr();

  //déclaration & initialisation du 1er block libre
  fb_t f_fb ;
  f_fb.size = get_memory_size() - sizeof(memory_head_t) - sizeof(fb_t) ;
  f_fb.next = NULL ;

  //déclaration & initialisation de l'entête de notre mémoire
  memory_head_t mem_h ;

  mem_h.first_block = sizeof(memory_head_t) + (void*)memory ;

  mem_h.strategy = &mem_first_fit ;

  ((void *) memory) = mem_h ;

  ((void *)memory + sizeof(memory_head_t)) = f_fb ;

}

//-------------------------------------------------------------
// mem_alloc
//-------------------------------------------------------------
void* mem_alloc(size_t size) {
   /* A COMPLETER */ 
        return NULL;
}

//-------------------------------------------------------------
// mem_free
//-------------------------------------------------------------
void mem_free(void* zone) {
   /* A COMPLETER */ 
}

//-------------------------------------------------------------
// Itérateur(parcours) sur le contenu de l'allocateur
// mem_show
//-------------------------------------------------------------
void mem_show(void (*print)(void *, size_t, int free)) {
   /* A COMPLETER */ 
}

//-------------------------------------------------------------
// mem_fit
//-------------------------------------------------------------
void mem_fit(mem_fit_function_t* mff) {
   /* A COMPLETER */ 
}

//-------------------------------------------------------------
// Stratégies d'allocation 
//-------------------------------------------------------------
struct fb* mem_first_fit(struct fb* head, size_t size) {
   /* A COMPLETER */ 
    return NULL;
}
//-------------------------------------------------------------
struct fb* mem_best_fit(struct fb* head, size_t size) {
   /* A COMPLETER */ 
    return NULL;

}
//-------------------------------------------------------------
struct fb* mem_worst_fit(struct fb* head, size_t size) {
   /* A COMPLETER */ 
    return NULL;
}
