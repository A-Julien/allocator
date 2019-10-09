#include "mem.h"
#include "mem_os.h"
#include "common.h"
#include <stdio.h>
#include <jmorecfg.h>
#include <stdbool.h>

//-------------------------------------------------------------
// mem_init
//-------------------------------------------------------------


/**
 * init memory
 */
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

    fb_t* fb_found = mem_h->strategy(mem_h->first_block, ((size + sizeof(fb_t) ) - sizeof(rb_t)));
    if(!fb_found) return NULL;

    fb_t* fb_previous = fb_found->previous;
    fb_t* fb_next = fb_found->next;

    if (fb_found->size > size + sizeof(fb_t) + sizeof(void*)){

        fb_t* new_fb = (void *)fb_found + (size + sizeof(rb_t));

        new_fb->size = (fb_found->size  - sizeof(rb_t)) - size;

        new_fb->previous = fb_previous;
        new_fb->next = fb_next;
        if (fb_previous)fb_previous->next = new_fb;
        if (fb_next)fb_next->previous = new_fb;

        rb_t* new_rb = (rb_t *)fb_found;
        new_rb->size = size;
        new_rb->previous_fb = fb_previous;

        if (!fb_previous) mem_h->first_block = new_fb; //maj header

        return ((void *)fb_found) + sizeof(rb_t);
    }

    if (fb_previous) fb_previous->next = fb_next;
    if (fb_next) fb_next->previous = fb_previous;
    if (!fb_previous && !fb_next) mem_h->first_block = NULL; //maj header

    rb_t* new_rb = (rb_t *)fb_found;
    new_rb->size = size;

    new_rb->previous_fb = fb_previous;

    return ((void *) fb_found) + sizeof(rb_t);
}

bool is_adjoining_block_fb(fb_t *block_1, fb_t *block_2){
    return ((void *)block_1 + sizeof(fb_t) + block_1->size) == (void *)block_2;
}

bool adjoining_block_rb(rb_t * block_1, rb_t * block_2){
    return ((void *)block_1 + sizeof(rb_t) + block_1->size )== (void *)block_2;
}

//FUSIOOOOONNNNNNNNN (DBZ)
void right_fusion(fb_t* fb){
    size_t old_size = fb->next->size;
    fb->next = fb->next->next;
    if(fb->next && fb->next->previous)fb->previous->next = fb;

    fb->size += sizeof(fb_t) + old_size;
}

//FUSIOOOOONNNNNNNNN (DBZ)
void left_fusion(fb_t* fb){
    fb->previous->next = fb->next;
    if(fb->next)fb->next->previous = fb->previous;

    fb->previous->size += sizeof(fb_t) +  fb->size;
}

void update_rb(fb_t* fb){
    if(fb->next){ // rb between fb and fb->next
        rb_t *rb_curent = (void *)fb + fb->size;
        while ((void *)fb->next != (void *)rb_curent ){
            rb_curent->previous_fb = fb;
            rb_curent = (void *)rb_curent + rb_curent->size;
        }
        return;
    }

    void* end_of_memory = get_memory_adr() + get_memory_size();

    if( ((void *)(fb + fb->size + sizeof(fb_t))) != end_of_memory){ //not end of memory
        //rb_t* rb_curent = (void *)fb + fb->size + sizeof(fb_t);
        rb_t* rb_curent = (void *)fb + (fb->size + sizeof(fb_t));

        while ((void *)rb_curent != end_of_memory ){
            rb_curent->previous_fb = fb;
            rb_curent = (void *)rb_curent + rb_curent->size + sizeof(rb_t);
        }
        return;
    }
}

void update_fb_next(fb_t *fb){
    if (fb->previous->next){
        fb->previous->next->previous = fb;
        fb->next = fb->previous->next;
    }
}

void update_fb_previous(fb_t *fb){
    if(fb->previous->next) fb->previous->next = fb;
}


//-------------------------------------------------------------
// mem_free
//-------------------------------------------------------------
void mem_free(void* zone) {
    rb_t* rb = (rb_t *)(zone - sizeof(rb_t));
    void* memory = get_memory_adr();

    size_t free_size = (rb->size + sizeof(rb_t)) - sizeof(fb_t);
    fb_t* rb_previous = rb->previous_fb;

    fb_t* new_fb = (fb_t *) rb;
    new_fb->size = free_size;
    new_fb->next = NULL;
    new_fb->previous = rb_previous;

    if(new_fb->previous) {
        if (new_fb->previous->next && is_adjoining_block_fb(new_fb, new_fb->previous->next)) {
            right_fusion(new_fb);
        } else {
            update_fb_next(new_fb);
        }
        update_rb(new_fb);

        if (is_adjoining_block_fb(new_fb->previous, new_fb)) {
            left_fusion(new_fb);
        } else {
            update_fb_previous(new_fb);
        }
        update_rb(new_fb);

        new_fb->size = (free_size + sizeof(rb_t)) - sizeof(fb_t) ;
        return;
    }

    memory_head_t* mem_h = (memory_head_t*)memory ;
    if (mem_h->first_block) {
        new_fb->next = mem_h->first_block ;

        if(is_adjoining_block_fb(new_fb, new_fb->next)) {
            right_fusion(new_fb);
        } else{
            update_fb_next(new_fb);
        }
        update_rb(new_fb);
    }
    mem_h->first_block = new_fb;

    new_fb->size = (free_size + sizeof(rb_t)) - sizeof(fb_t) ;
}

//-------------------------------------------------------------
// Itérateur(parcours) sur le contenu de l'allocateur
// mem_show
//-------------------------------------------------------------
void mem_show(void (*print)(void *, size_t, int free)) {

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
