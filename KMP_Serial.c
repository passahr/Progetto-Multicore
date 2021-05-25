/*
Author: Gianfranco "kanema" Passanisi
Description: Implementing an algorithm that will look for a set of strings inside a set of TCP/UDP data packets that
 utilizes Knuth-Moris-Pratt algorithm. This is the serial implementation, this code will be also implemented with:
- Parallel distributed programming with MPI --> KMP_Parallel_MPI.c
- Parallel shared programming with OpenMP   --> KMP_Parallel_OpenMP.c
*/
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <time.h>

//Function that elaborate the pattern
void computeLPSArray(char* pat, int M, int* lps)
{
    // length of the previous longest prefix suffix
    int len = 0;
  
    lps[0] = 0;
  
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

//KMP Function
void KMPSearch(char* pat, char* txt)
{
    int M = strlen(pat);
    int N = strlen(txt);
  

    int lps[M];
  
    // Preprocess the pattern (calculate lps[] array)
    computeLPSArray(pat, M, lps);
  
    int i = 0; // index for txt[]
    int j = 0; // index for pat[]
    while (i < N)
    {
        if (pat[j] == txt[i])
        {
            j++;
            i++;
        }
  
        if (j == M)
        {
            printf("%d\n", i - j);
            j = lps[j - 1];
        }
  
        
        else if (i < N && pat[j] != txt[i])
        {
            if (j != 0)
                j = lps[j - 1];
            else
                i = i + 1;
        }
    }
}
  
int main(int argc, char *argv[])
{
/*#################################
  # PROCESSING THE TCP/UDP STREAM #
  #################################
*/ 

    //open file
    char ch1;
    FILE *fp;

    fp = fopen("processed.txt", "r"); // read mode
    
    if (fp == NULL)
    {
       perror("Error while opening the file.\n");
       
    }

    char *datastream;
    int i = 0;
    int last_i = 0;
    int size_of_packet, datastream_length;
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

    //here I initialize the stream that will contain all the data that I'll have to analyze  
    datastream = (char *)malloc(sizeof(char)*i);

    datastream_length = i;

    i = 0;

    //second file analysis to fill the datastream
    fp = fopen("processed.txt", "r"); // read mode
    
    if (fp == NULL)
    {
       perror("Error while opening the file.\n");
       
    }

    while((ch1 = fgetc(fp)) != EOF)
    {
        datastream[i] = ch1;
        i++;
    }

    fclose(fp);

    //char pat[20];
    //get pattern
    //strcpy(pat, argv[1]);
    
    

    //execute KMP            
    

    double final_time = 0.0;
    for(int i=1; i<argc; i++)
    {
        char *patt = argv[i];
        clock_t begin = clock();
        KMPSearch(patt, datastream);
        clock_t end = clock();
        printf("%s\n", patt);
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        final_time += time_spent;
        
    }
    
    
    
    printf("Serial algorithm time is: %f\n", final_time);

    return 0;
}
