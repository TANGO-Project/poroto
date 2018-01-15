                        /* random.h : generation de nombres aleatoire */
#ifndef RANDOM_H
#define RANDOM_H

/**********************************************************************/
/********* Generation de nombres aleatoires ***************************/
/**********************************************************************/

extern void   initRandom(void);                 /* Initialisation generateur */
extern int    myRandomMinMax(int min, int max); /* Generation dans [min,max] */
extern double myRandom01(void);                 /* Generation dans [0.0,1.0] */

#endif /* RANDOM_H */
