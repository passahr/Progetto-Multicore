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
    //################################
    //# ANALISI DELLO STREAM TCP/UDP #
    //################################
    char ch1;
    FILE *fp;

    fp = fopen("processed.txt", "r"); // read mode
    
    if (fp == NULL)
    {
       perror("Error while opening the file.\n");
       
    }

    char *txt;
    int i = 0;
    int num_packet = 1;

    while((ch1 = fgetc(fp)) != EOF)
    {
        if(ch1 == '\n')
        {
            num_packet++;
        }
        
        i++;
    }
    fclose(fp);

    //with this first analysis I get important information about the size of the data to store.
    num_packet--;

    //printf("%d \n", i);
    //printf("%d \n", num_packet);

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

    //#####################################
    //# ANALISI DELLO STREAM TCP/UDP FINE #
    //#####################################

    char pat[20];
    strcpy(pat, argv[1]);
    int M = strlen(pat);
    int N = strlen(txt);
    int lps[M];
    int indexes[N];
    int next_index = 0;

    computeLPSArray(pat, M, lps);

    #pragma omp parallel shared(indexes, next_index)
    {
        int thread_number = omp_get_num_threads();
        int thread_rank = omp_get_thread_num();
        int start = (N/thread_number*thread_rank);
        int end = (N/thread_number*(thread_rank+1))-1+M;

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
                #pragma omp critical
                {
                    indexes[next_index] = i - j;
                    next_index++;
                }
                j = lps[j - 1];
            }
  
            // mismatch after j matches
            else if (i < end && pat[j] != txt[i])
            {
            // Do not match lps[0..lps[j-1]] characters,
            // they will match anyway
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
    #pragma omp parallel for
    for(i = 0; i<next_index; i++)
    {
        printf("%d\n", indexes[i]);
    }
}