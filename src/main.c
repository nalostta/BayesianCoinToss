#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "cJSON.h"


int main()
{
    // read a BCT_config file located in root directory
    FILE* cfg_reader = fopen("../BCT_config.json", "r");

    if(cfg_reader == NULL)
    {
        printf("Error: Unable to open file\n");
        return 1;
    }

    fseek(cfg_reader, 0, SEEK_END);
    long fsize = ftell(cfg_reader);
    fseek(cfg_reader, 0, SEEK_SET);
    char* config_str = malloc(fsize + 1);
    fread(config_str, fsize, 1, cfg_reader);
    // config file read into string

    cJSON* config = cJSON_Parse(config_str);
    if(config == NULL)
    {
        printf("Error: Unable to parse config file\n");
        return 1;
    }

    cJSON* gamename = cJSON_GetObjectItem(config, "gamename");
    cJSON* biased_coin_heads_pbb = cJSON_GetObjectItem(config, "BiasedCoinHeadsPbb");
    cJSON* init_confidence = cJSON_GetObjectItem(config, "init_confidence");

    free(config_str);
    fclose(cfg_reader);

    printf("\n====================\n%s\n====================\n", gamename->valuestring);
    printf(" 1. Game is %.2f %% confident that the coin is biased\n", init_confidence->valuedouble*100);
    printf(" 2. For a biased coin, the probability of {Heads:%.2f, Tails:%.2f}\n", biased_coin_heads_pbb->valuedouble, (1-biased_coin_heads_pbb->valuedouble));

    // Game Starts:
    // Get H or T (or x) from user

    char user_input='H';

    do
    {
        printf("\n\nEnter H or T, (or x to exit): ");
        scanf("%c", &user_input);

        
    }while(user_input != 'x');


    return 0;
}