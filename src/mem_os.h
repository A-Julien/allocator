#if !defined(mem_os_h)
#define mem_os_h

// Définition du type mem_fit_function_t
// type des fonctions d'allocation
typedef struct fb* (mem_fit_function_t)(struct fb *, size_t);

struct fb {
	size_t size ;
	struct fb *next ;
} typedef fb_t ;

struct memory_head {
	fb_t *first_block ;
	mem_fit_function_t *strategy ;
}typedef memory_head_t ;

/* -----------------------------------------------*/
/* Interface de gestion de votre allocateur       */
/* -----------------------------------------------*/



// Choix de la fonction d'allocation
// = choix de la stratégie de l'allocation
void mem_fit(mem_fit_function_t*);

// Stratégies de base (fonctions) d'allocation
mem_fit_function_t mem_first_fit;
mem_fit_function_t mem_worst_fit;
mem_fit_function_t mem_best_fit;

#endif /* mem_os_h */
