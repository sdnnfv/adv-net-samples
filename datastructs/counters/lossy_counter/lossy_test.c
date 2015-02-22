#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

#include "lossycon.h"
#include "zipfgen.h"

//show result and free memory
void show_result(Counter *result) {
        int len = 0;
        int i;

        len = result[0].item;
        printf("Item            Count\n");

        for (i = 1; i <= len; i++) {
                printf("%-16d%-16d\n", result[i].item, result[i].count);
        }   
}

void counter_test(char *lossy_phi, char *zipf_N, char *zipf_alpha, 
                        char *runs, char *outrate, int debug) {
        LC_type *lcounter;
        Counter *result = NULL;
        int i, key;
        double alpha, phi;
        int N, T, R;

        phi = atof(lossy_phi);
        if (phi == 0) {
                perror("invalid input");
                exit(-1);
        }
        lcounter = LC_Init(phi);
        alpha = atof(zipf_alpha);
        N = atoi(zipf_N);
        T = atoi(runs);
        R = atoi(outrate);
        if (N == 0 || T == 0 || R == 0) {
                perror("invalid input");
                exit(-1);
        }   
        if (debug == 1) {
                printf("Lossy speed tester parameters:\n");
                printf("        Lossy phi :     %f\n", phi); 
                printf("        zipf N :        %d\n", N); 
                printf("        zipf alpha :    %f\n", alpha); 
                printf("        running time :  %d\n", T); 
                printf("        output period : %d\n", R); 
        }

        for (i = 0; i < T; i++) {
                key = get_zipf_key(alpha, N); 
                LC_Update(lcounter, key);
                if (i % R == 0) {
                        if (result != NULL) {
                                free(result);
                                result = NULL;
                        }
                        result = LC_Output(lcounter, 1);
                        if (debug == 1) show_result(result);
                }
        }
        return;
}

int main(int argc, char ** argv)
{
        char* phi = "0.01";
        char* N = "1000";
        char* alpha = "0.5";
        char* runs = "2";
        char* outrate = "100";
        int debug = 0;
        int o;

        /* Command line args: */
        printf("Lossy speed tester usage: \n");
        printf("        -p [lossy phi] -n [zipf N] -a [zipf alpha]\n");
        printf("        -t [running seconds] -v [show results] -r [output period]\n");

        while ((o = getopt (argc, argv, "p:n:a:t:vr:")) != -1) {
                switch(o){
                case 'p':
                        phi = optarg;
                        break;
                case 'n':
                        N = optarg;
                        break;
                case 'a':
                        alpha = optarg;
                        break;
                case 't':
                        runs = optarg;
                        break;
                case 'v':
                        debug = 1;
                        break;
                case 'r':
                        outrate = optarg;
                        break;
                case '?':
                        if(optopt == 'p' || optopt == 'n' || optopt == 'a' 
                                || optopt == 't' || optopt == 'r') {
                                fprintf (stderr, "Option %c requires an argument.\n", optopt);
                        }
                        else {
                                fprintf (stderr, "Unknown argument: %c.\n", optopt);
                        }
                        break;
                }
        }

        counter_test(phi, N, alpha, runs, outrate, debug);

        return 0;
}

