#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "cJSON.h"
#include "types.h"
#include "gnuplot_i.h"


int main()
{
    // read a BCT_config file located in root directory
    FILE* cfg_reader = fopen("../BCT_config.json", "r");

    if(cfg_reader == NULL)
    {
        printf("Error: Unable to open configs file BCT_config.json\n");
        return 1;
    }

    fseek(cfg_reader, 0, SEEK_END);
    long fsize = ftell(cfg_reader);
    fseek(cfg_reader, 0, SEEK_SET);
    char* config_str = calloc(fsize + 1, sizeof* config_str);
    fread(config_str, fsize, 1, cfg_reader);
    // config file read into string

    cJSON* config = cJSON_Parse(config_str);
    if(config == NULL)
    {
        printf("[JSON-Error]: Unable to parse config file\n");
        return 1;
    }

    cJSON* testname = cJSON_GetObjectItem(config, "TestName");
    cJSON* biased_coin_heads_pbb = cJSON_GetObjectItem(config, "BiasedCoinHeadsPbb");
    cJSON* init_confidence = cJSON_GetObjectItem(config, "init_confidence");
    cJSON* test_rounds = cJSON_GetObjectItem(config, "TestRounds");
    cJSON* test_input = cJSON_GetObjectItem(config, "TestInput");
    cJSON* output_filename = cJSON_GetObjectItem(config, "OutputFileName");

    free(config_str);
    fclose(cfg_reader);
    //
    
    int* input_vals = calloc(test_rounds->valueint, sizeof*input_vals);
    int ntest_rounds = test_rounds->valueint;

    for(int i=0; i<ntest_rounds; i++)
    {
        cJSON* a_i = cJSON_GetArrayItem(test_input, i);
        //printf("[debug] : %d\n", i);
        if(a_i!=NULL) 
            input_vals[i] = a_i ->valueint;
        else
        {
            printf("\n[Error] : Number of inputs is smaller than said length. Reducing number of test rounds\n");
            input_vals[i] = -1;
            ntest_rounds = i;
            break;
        }
    }

    printf("\n====================\n%s\n====================\n", testname->valuestring);
    printf(" 1. Game is %.2f %% confident that the coin is biased\n", init_confidence->valuedouble*100);
    printf(" 2. Testing for %d rounds\n", ntest_rounds);
    printf(" 3. For a biased coin, the probability of {Heads:%.2f, Tails:%.2f}\n", biased_coin_heads_pbb->valuedouble, (1-biased_coin_heads_pbb->valuedouble));
    printf(" 4. Test Input:");

    for(int i=0; i<ntest_rounds; i++)
    {
        printf(" %d",input_vals[i]);
    }
    printf("\n====================\n");

    // Game Starts:
    // Get H or T (or x) from user

    double* op_confidence = calloc(ntest_rounds, sizeof*op_confidence);

    //
    coin_t biased_pbb_map = {biased_coin_heads_pbb->valuedouble, 1-biased_coin_heads_pbb->valuedouble};
    coin_t unbiased_pbb_map = {0.5, 0.5};
    double confidence = init_confidence->valuedouble;

    double Pd_biased = 1;
    double Pd_unbiased = 1;

    for(int i=0; i<ntest_rounds; i++)
    {
        Pd_biased   *= (input_vals[i])? biased_pbb_map.heads:biased_pbb_map.tails; // P0 is heads pbb
        Pd_unbiased *= 0.5; // equal probability for unbiased coin

        // Posterior
        double Ppost_biased     = init_confidence->valuedouble*Pd_biased;
        double Ppost_unbiased   = (1-init_confidence->valuedouble)*Pd_unbiased;

        //printf("[debug] : Confidence biased=%f, unbiased=%f\n", confidence, 1-confidence);
        //printf("[debug] : Priors biased=%f, unbiased=%f\n", Pd_biased, Pd_unbiased);
        //printf("[debug] : Posteriors biased=%f, unbiased=%f\n", Ppost_biased, Ppost_unbiased);

        // Normalized Posterior
        confidence = Ppost_biased/(Ppost_biased + Ppost_unbiased);


        printf("%.2f%% confident that the coin is biased (with PH=%.2f)\n", confidence*100, biased_pbb_map.heads);
        op_confidence[i] = confidence;
    }

    
    FILE* op_file = fopen(output_filename->valuestring, "w");
    printf("\nOutput written to %s\n", output_filename->valuestring);
    if(op_file == NULL)
    {
        printf("[Error] : Unable to open output file\n");
        return 1;
    }else 
    {
        for(int i=0; i<ntest_rounds; i++)
        {
            fprintf(op_file, "%d %f %.2f\n", i+1, (input_vals[i])*0.6+0.05, op_confidence[i]);
        }
        fclose(op_file);    
    }

    double* x_vals = calloc(ntest_rounds, sizeof*x_vals);
    for(int i=1; i<=ntest_rounds; i++) x_vals[i-1] = i;

    // Gnuplot plot with number of iterations on the x axis and confidence on the y axis
    gnuplot_ctrl *handle = gnuplot_init();

    // gnuplot to plot 3 columns in file bct_test_plot.txt with lines
    gnuplot_cmd(handle, " set yrange [0:2]; set xrange [1:%d]; set style fill solid 1; set boxwidth 0.05; plot \"bct_test_plot.txt\" using 1:3 title \'Confidence\' with lines, \"bct_test_plot.txt\" using 1:2 title \'CoinToss\' with boxes", ntest_rounds);
    char c;
    scanf("%c", &c);
    gnuplot_close (handle);

    cJSON_Delete(config);
    cJSON_Delete(testname);
    cJSON_Delete(biased_coin_heads_pbb);
    cJSON_Delete(init_confidence);
    cJSON_Delete(test_rounds);
    cJSON_Delete(test_input);
    free(input_vals);
    free(op_confidence);
    free(x_vals);

    return 0;
}