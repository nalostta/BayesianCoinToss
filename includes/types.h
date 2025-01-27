#ifndef __INCLUDED_BAYESIANCOINTOSS_TYPES_H__
#define __INCLUDED_BAYESIANCOINTOSS_TYPES_H__

typedef struct 
{
    double P0;
    double P1;
} bin_pbb_t;


typedef struct 
{
    double heads;
    double tails;
} coin_t;

#endif // __INCLUDED_BAYESIANCOINTOSS_TYPES_H__