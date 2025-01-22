#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "cJSON.h"
#include "types.h"


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
    cJSON* n_rounds = cJSON_GetObjectItem(config, "Number_of_Rounds");

    free(config_str);
    fclose(cfg_reader);

    printf("\n====================\n%s\n====================\n", gamename->valuestring);
    printf(" 1. Game is %.2f %% confident that the coin is biased\n", init_confidence->valuedouble*100);
    printf(" 2. Playing %d rounds\n", n_rounds->valueint);
    printf(" 3. For a biased coin, the probability of {Heads:%.2f, Tails:%.2f}\n", biased_coin_heads_pbb->valuedouble, (1-biased_coin_heads_pbb->valuedouble));

    // Game Starts:
    // Get H or T (or x) from user

    int user_input=0;
    int n = 0;

    //
    bin_pbb_t biased_coin_pbb = {biased_coin_heads_pbb->valuedouble, 1-biased_coin_heads_pbb->valuedouble};
    bin_pbb_t unbiased_coin_pbb = {0.5, 0.5};
    double confidence = init_confidence->valuedouble;

    double Pd_biased = 1;
    double Pd_unbiased = 1;

    do
    {
        printf("\n\nEnter 0 (for Tails) or 1 (for Heads), (or x to exit): ");
        scanf("%d", &user_input);

        Pd_biased *= (user_input)? biased_coin_pbb.P0:biased_coin_pbb.P1; // P0 is heads pbb
        Pd_unbiased*= 0.5; // equal probability for unbiased coin

        // Posterior
        double Ppost_biased = init_confidence->valuedouble*Pd_biased;
        double Ppost_unbiased = (1-init_confidence->valuedouble)*Pd_unbiased;

        //printf("[debug] : Confidence biased=%f, unbiased=%f\n", confidence, 1-confidence);
        //printf("[debug] : Priors biased=%f, unbiased=%f\n", Pd_biased, Pd_unbiased);
        //printf("[debug] : Posteriors biased=%f, unbiased=%f\n", Ppost_biased, Ppost_unbiased);

        // Normalized Posterior
        confidence = Ppost_biased/(Ppost_biased + Ppost_unbiased);


        printf("\n %.2f%% confident that the coin is biased towardsd heads\n", confidence*100);

    }while(user_input != 'x' && n++<n_rounds->valueint);


    return 0;
}