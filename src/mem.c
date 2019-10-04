
#include "mem.h"
#include "mem_os.h"
#include "common.h"
#include <stdio.h>
#include <jmorecfg.h>
#include <stdbool.h>

//-------------------------------------------------------------
// mem_init
//-------------------------------------------------------------
void mem_init() {

    void* memory = get_memory_adr();

    //déclaration & initialisation de l'entête de notre mémoire
    memory_head_t* mem_h = (memory_head_t*)memory ;
    mem_h->strategy = mem_first_fit;
    mem_h->first_block = memory + sizeof(memory_head_t);

    //init first fb_t
    fb_t* first_fb_t = mem_h->first_block;
    first_fb_t->size = get_memory_size() - sizeof(memory_head_t);
    first_fb_t->next = NULL;
    first_fb_t->previous = NULL ;

}

//-------------------------------------------------------------
// mem_alloc
//-------------------------------------------------------------
void* mem_alloc(size_t size) {
    void* memory = get_memory_adr();
    memory_head_t* mem_h = (memory_head_t*)memory ;

    if (!mem_h->first_block) return NULL;

    fb_t* fb_found = mem_h->strategy(mem_h->first_block, ((size + sizeof(fb_t)) - sizeof(rb_t)));
    if(!fb_found) return NULL;

    fb_t* fb_previous = fb_found->previous;
    fb_t* fb_next = fb_found->next;

    if ((sizeof(rb_t) + size) < fb_found->size ){
        fb_t* new_fb = fb_found + (sizeof(rb_t) + size);
        new_fb->size = fb_found->size - size ;

        new_fb->previous = fb_previous;
        new_fb->next = fb_next;
        if (fb_previous)fb_previous->next = new_fb;
        if (fb_next)fb_next->previous = new_fb;

        rb_t* new_rb = (rb_t *)fb_found;
        new_rb->size = size;
        new_rb->previous_fb = fb_previous;

        if (!fb_previous) mem_h->first_block = new_fb;//maj header

        return fb_found + sizeof(rb_t);
    }

    if (fb_previous) fb_previous->next = fb_next;
    if (fb_next) fb_next->previous = fb_previous;
    if (!fb_previous) mem_h->first_block = NULL; //maj header

    rb_t* new_rb = (rb_t *)fb_found;
    new_rb->size = size;

    new_rb->previous_fb = fb_previous;


    return fb_found + sizeof(rb_t);;
}

bool adjoining_block_fb(fb_t * block_1, fb_t * block_2){
    return block_1 + sizeof(fb_t) + block_1->size == block_2;
}

bool adjoining_block_rb(rb_t * block_1, rb_t * block_2){
    return block_1 + sizeof(rb_t) + block_1->size == block_2;
}

void maj_rb(fb_t * fb, rb_t* rb){
    size_t size_ofset = 0;
    rb_t* rb_adjoining = NULL;
    while ((void *)fb->next != (void *)rb_adjoining ){
        rb_adjoining = rb + (rb->size + size_ofset);
        rb_adjoining->previous_fb = fb;
        size_ofset += rb_adjoining->size + sizeof(rb_t);
    }
}
//FUSIOOOOONNNNNNNNN (DBZ)
void right_fusion(fb_t* fb, rb_t* rb){
    if (rb->previous_fb->next && adjoining_block_fb(fb, fb->next)){
        fb->next = fb->next->next;
        fb->previous->next = fb;

        fb->size = sizeof(fb_t)*2 + fb->next->size;
    } else {
        maj_rb(fb, rb);
    }
}
//FUSIOOOOONNNNNNNNN (DBZ)
void left_fusion(fb_t* fb){
    if (adjoining_block_fb(fb->previous, fb)){
        fb->previous->next = fb->next;
        fb->next->previous = fb->previous;

        fb->previous->size += sizeof(fb_t) +  fb->size;
    }
}


//-------------------------------------------------------------
// mem_free
//-------------------------------------------------------------
void mem_free(void* zone) {
    rb_t* rb = (rb_t*) (zone - sizeof(rb_t));
    void* memory = get_memory_adr();
    size_t free_size = rb->size;

    fb_t* next_rb = rb->previous_fb->next;
    fb_t* new_fb = (fb_t *) rb;

    new_fb->size = free_size;
    new_fb->next = next_rb;



    if (rb->previous_fb){
        right_fusion(new_fb, rb);
        left_fusion(new_fb);
        return;
    }

    memory_head_t* mem_h = (memory_head_t*)memory ;

    new_fb->next = mem_h->first_block->next ;
    if (new_fb->next){

        new_fb->next->previous = new_fb ;
        right_fusion(new_fb, rb);
    }

    new_fb->size = (free_size + sizeof(rb_t)) - sizeof(fb_t) ;

    mem_h->first_block = new_fb;
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
    void* memory = get_memory_adr();
    memory_head_t* mem_h = (memory_head_t*)memory ;
    mem_h->strategy = mff;
}
//-------------------------------------------------------------
// Stratégies d'allocation 
//-------------------------------------------------------------
struct fb* mem_first_fit(fb_t* head, size_t size) {
    fb_t *p = head;

    while(p->next != NULL){
        if(p->size < size) p = p->next;
        return p;
    }

    if (head->size >= size) return head;

    return NULL;
}
//-------------------------------------------------------------
struct fb* mem_best_fit(fb_t* head, size_t size) {
    fb_t *p = head;
    fb_t *best_fit = head;
    while(p->next != NULL){
        if(p->size < best_fit->size && p->size <= size) best_fit = p;
        p = p->next;
    }
    if(best_fit->size >= size) return NULL;
    return best_fit;

}
//-------------------------------------------------------------
struct fb* mem_worst_fit(fb_t* head, size_t size) {
    fb_t *p = head;
    fb_t *worst_fit = head;
    while(p->next != NULL){
        if(p->size > worst_fit->size && p->size >= size) worst_fit = p;
        p = p->next;
    }
    if(worst_fit->size >= size) return NULL;
    return worst_fit;
}
