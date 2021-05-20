/*
Author: Gianfranco "kanema" Passanisi
Description: Implementing an algorithm that will look for a set of strings inside a set of TCP/UDP data packets that
 utilizes Knuth-Moris-Pratt algorithm. This is the serial implementation, this code will be also implemented with:
- Parallel distributed programming with MPI --> KMP_Parallel_MPI_Chainless.c
- Parallel distributed programming with MPI --> KMP_Parallel_MPI_Chainful.c
- Parallel shared programming with OpenMP   --> KMP_Parallel_OpenMP.c
*/
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <time.h>


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


  
// Prints occurrences of txt[] in pat[]
void KMPSearch(char* pat, char* txt)
{
    int M = strlen(pat);
    int N = strlen(txt);
  
    // create lps[] that will hold the longest prefix suffix
    // values for pattern
    int lps[M];
  
    // Preprocess the pattern (calculate lps[] array)
    computeLPSArray(pat, M, lps);
  
    int i = 0; // index for txt[]
    int j = 0; // index for pat[]
    while (i < N) {
        if (pat[j] == txt[i]) {
            j++;
            i++;
        }
  
        if (j == M) {
            printf("Found pattern at index %d\n", i - j);
            j = lps[j - 1];
        }
  
        // mismatch after j matches
        else if (i < N && pat[j] != txt[i]) {
            // Do not match lps[0..lps[j-1]] characters,
            // they will match anyway
            if (j != 0)
                j = lps[j - 1];
            else
                i = i + 1;
        }
    }
}
  
// Fills lps[] for given patttern pat[0..M-1]



  
// Driver program to test above function
int main(int argc, char *argv[])
{

    //Preprocesing of the previously processed file with python

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


    //here i fill the array containing the packets structs, each struct contains the starting and ending point of each packet in the datastream

    char pat[20];
    strcpy(pat, argv[1]);
    
    clock_t begin = clock();

            
    KMPSearch(pat, datastream);
    
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Elapsed time: %f\n", time_spent);

    return 0;
}
