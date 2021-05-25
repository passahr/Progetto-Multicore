/*
Author: Gianfranco "kanema" Passanisi
Description: Implementing an algorithm that will look for a set of strings inside a set of TCP/UDP data packets that
utilizes Knuth-Moris-Pratt algorithm. This is the parralel implementation made with OpenMP. The general workflow is the following:
 -text and pattern are globally shared
 -"pragma parallel" directive will behaviour differently as each thread analyze a different chunk of the text
 -last thread will analyze text till the end
 -race condition is found while updating the array that will contain all the indexes, it's treated with a "pragma critical" section
 -a "pragma parallel for" will print all the indexes in the array
 -end.
This code will be also implemented with:
- Serial algorithm                          --> KMP_Serial.c
- Parallel distributed programming with MPI --> KMP_Parallel_MPI.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

void computeLPSArray(char* pat, int M, int* lps)
{
    // length of the previous longest prefix suffix
    int len = 0;
  
    lps[0] = 0; // lps[0] is always 0
  
    // the loop calculates lps[i] for i = 1 to M-1
    int i = 1;
    while (i < M)
    {
        if (pat[i] == pat[len])
        {
            len++;
            lps[i] = len;
            i++;
        }
        else // (pat[i] != pat[len])
        {
        
            if (len != 0)
            {
                len = lps[len - 1];
  
                
            }
            else // if (len == 0)
            {
                lps[i] = 0;
                i++;
            }
        }
    }
}

double KMP_Parallel_OpenMP(char* pattern)
{
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


    double start, end; 
    char pat[20];
    strcpy(pat, pattern);
    int M = strlen(pat);
    int N = strlen(txt);
    int lps[M];
    int indexes[N];
    int next_index = 0;
    start = omp_get_wtime(); 
    //COMPUTE LPS
    computeLPSArray(pat, M, lps);
    
    #pragma omp parallel shared(indexes, next_index)
    {
        int thread_number = omp_get_num_threads();
        int thread_rank = omp_get_thread_num();
        int start = (N/thread_number*thread_rank);
        int end = (N/thread_number*(thread_rank+1))-1+M;

        //last thread computes till the end
        if(thread_rank==thread_number-1)
        {
            end = N;
        }
    

        int i = start;
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
                //RACE CONDITION
                #pragma omp critical
                {
                    indexes[next_index] = i - j;
                    next_index++;
                }
                j = lps[j - 1];
            }
  
            else if (i < end && pat[j] != txt[i])
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
    
        
    }
    free(txt);
    end = omp_get_wtime(); 
    #pragma omp parallel for
    for(i = 0; i<next_index; i++)
    {
        printf("%d\n", indexes[i]);
    }
    
    
    return (end-start);
}

int main(int argc, char *argv[])
{
    double final_time = 0.0;
    for(int i=1; i<argc; i++)
    {
        char *patt = argv[i];
        double time = KMP_Parallel_OpenMP(patt);
        final_time += time;
        //printf("%s\n", patt);
    }
    //
    printf("OpenMP execution time is: %f\n", final_time);
}