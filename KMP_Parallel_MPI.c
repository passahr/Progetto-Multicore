/*
Author: Gianfranco "kanema" Passanisi
Description: Implementing an algorithm that will look for a set of strings inside a set of TCP/UDP data packets that
utilizes Knuth-Moris-Pratt algorithm. This is the parallel implementation made with MPI. The general workflow is the following:
 -the text file to analyze is locally known by every core
 -master core gets pattern to match
 -master core sends the elaborated pattern-string
 -master core sends indexes of the chunks (beginning and end)
 -master core elaboretes its chunk meanwhile the other cores do the same thing as well
 -master core wille receive indexes of the pattern, if found from other cores and it will store them in an array
 -master core will print found indexes
 -end.
This code will be also implemented with:
- Serial algorithm                          --> KMP_Serial.c
- Parallel shared programming with OpenMP   --> KMP_Parallel_OpenMP.c
*/
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <mpi.h>

void computeLPSArray(char* pat, int M, int* lps)
{
    // length of the previous longest prefix suffix
    int len = 0;
  
    lps[0] = 0; // lps[0] is always 0
  
    // the loop calculates lps[i] for i = 1 to M-1
    int i = 1;
    while (i < M) {
        if (pat[i] == pat[len]) {
            len++;
            lps[i] = len;
            i++;
        }
        else // (pat[i] != pat[len])
        {
            // This is tricky. Consider the example.
            // AAACAAAA and i = 7. The idea is similar
            // to search step.
            if (len != 0) {
                len = lps[len - 1];
  
                // Also, note that we do not increment
                // i here
            }
            else // if (len == 0)
            {
                lps[i] = 0;
                i++;
            }
        }
    }
}




int main(int argc, char *argv[])
{
/*#################################
  # PROCESSING THE TCP/UDP STREAM #
  #################################
*/ 
    char ch1;
    FILE *fp;

    fp = fopen("processed.txt", "r"); // read mode
    
    if (fp == NULL)
    {
       perror("Error while opening the file.\n");
       
    }

    char *txt;
    int i = 0;
    

    while((ch1 = fgetc(fp)) != EOF)
    {
        i++;
    }
    fclose(fp);

    //with this first analysis I get important information about the size of the data to store.
    

    //here I initialize the stream that will contain all the data that I'll have to analyze  
    txt = (char *)malloc(sizeof(char)*i);

    i = 0;

    //second file analysis to fill the datastream
    fp = fopen("processed.txt", "r"); // read mode
    
    if (fp == NULL)
    {
       perror("Error while opening the file.\n");
       
    }

    while((ch1 = fgetc(fp)) != EOF)
    {
        txt[i] = ch1;
        i++;
    }

    fclose(fp);

/*####################################
  # Beginning of the parallelization #
  ####################################
*/ 

    MPI_Init(NULL, NULL);
    
    double t1, t2; 

    t1 = MPI_Wtime();
        //char pattern_to_find = patterns[i];
        // Get the number of processes
    int cores;
    MPI_Comm_size(MPI_COMM_WORLD, &cores);
    // Get the rank of the process
    int core_number;
    MPI_Comm_rank(MPI_COMM_WORLD, &core_number);
    
    //MASTER CORE BEHAVIOUR
    if(core_number == 0)
    {
        char pat[20];
        for(int k = 1; k<argc; k++)
        {
            
            strcpy(pat, argv[k]);
            printf("Now looking for %s\n", argv[k]);
        
            
            
            int M = strlen(pat);
            int N = strlen(txt);
            int lps[M];
            int newN = N/cores+M;
            computeLPSArray(pat, M, lps);
            for(int k = 1; k<cores; k++)
                {
                    MPI_Send(&pat, M, MPI_CHAR, k, 0, MPI_COMM_WORLD);
                    MPI_Send(&M, 1, MPI_INT, k, 0, MPI_COMM_WORLD);
                    MPI_Send(&lps, M, MPI_INT, k, 0, MPI_COMM_WORLD);
                    MPI_Send(&N, 1, MPI_INT, k, 0, MPI_COMM_WORLD);
                }
            int indexes[N];
            int x_indexes = 0;
            int i = 0;
            int end = newN;
            int j = 0; // index for pat[]
            while (i < end)
            {
                if (pat[j] == txt[i])
                {
                    j++;
                    i++;
                }

                if (j == M) 
                {
                    indexes[x_indexes] = i - j;
                    x_indexes++;
                    j = lps[j - 1];
                }

                else if (i < N && pat[j] != txt[i])
                {
                    if (j != 0)
                    {
                        j = lps[j - 1];
                    }
                    else
                    {
                        i = i + 1;
                    }
                }
            }
            for(int k = 1; k<cores; k++)
            {
                int foundValue;
                //RECEIVES INDEXES UNTIL ITS NEGATIVE
                MPI_Recv(&foundValue, 1, MPI_INT, k, 0, MPI_COMM_WORLD, NULL);
                while(foundValue >= 0)
                {
                    indexes[x_indexes] = foundValue;
                    x_indexes++;
                    MPI_Recv(&foundValue, 1, MPI_INT, k, 0, MPI_COMM_WORLD, NULL);
                }
            }
            t2 = MPI_Wtime();
            //PRINTS INDEXES
            for(int k = 0; k<x_indexes; k++)
            {
                printf("%d\n", indexes[k]);
            }
            
        }
        
        printf("MPI execution time is:%f\n", t2-t1);
    }
    //OTHER CORES BEHAVIOUR
    else
    {
        for(int k = 1; k<argc; k++)
        {
            char pat[20];
            int local_M, local_N;
            int local_lps[local_M];
            //RECEIVE LSP, LSP_LENGTH AND STRING LENGTH
            MPI_Recv(&pat, 20, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&local_M, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&local_lps, local_M, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&local_N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int start = (local_N/cores)*core_number;
            int end, i;
            if(core_number==cores-1)
            {
                //IF LAST CORE IT COMPUTES UNTIL THE END
                end = local_N;
            }
            else
            {
                end = (local_N/cores)*(core_number+1)+local_M;
            }
            int next_target;
            i = start; // index for txt[]
            int j = 0; // index for pat[]
            while (i < end)
            {
                if (pat[j] == txt[i])
                {
                    j++;
                    i++;
                }

                if (j == local_M) 
                {
                    next_target = i-j;
                    //POSITIVE INDEXES ARE SENT FROM HERE
                    MPI_Send(&next_target, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                    j = local_lps[j - 1];
                }

                // mismatch after j matches
                else if (i < end && pat[j] != txt[i])
                {
                    if (j != 0)
                    {
                        j = local_lps[j - 1];
                    }
                    else
                    {
                        i = i + 1;
                    }
                }
            }
            next_target = -1;
            //SEND A NEGATIVE INDEX, STOPS THE STREAM
            MPI_Send(&next_target, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }   
    free(txt);
    ///MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();


}