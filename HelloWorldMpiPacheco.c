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
#include <mpi.h>

#define MAX_STRING_LENGTH 40
#define MAX_STRING_NUM 30
const int MAX_STRING = 100;


  
void computeLPSArray(char* pat, int M, int* lps);

typedef struct
{
    int number;
    int start;
    int end;
    int size;
} packet;


  
// Prints occurrences of txt[] in pat[]
void KMPSearch(char* pat, char* txt, int pack)
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
            printf("Found pattern at index %d of packet %d\n", i - j, pack);
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



int main(void)
{
    char greeting[MAX_STRING];
    int comm_sz;
    int my_rank;
    //sprintf(greeting, "This string is very very long, look for chars in it, yeah!");
    MPI_Init(NULL, NULL);
    for(int i = 0; i<4; i++)
    {
        
        MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
        if (my_rank!=0)
        {

            sprintf(greeting, "Greetings from process %d of %d!\n", my_rank, comm_sz);

            for(int k = 0; k<2; k++)
            {
                MPI_Send(greeting, strlen(greeting)+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
        }
        else
        {
            for(int j = 0; j<2; j++)
            {
                printf("Greetings from process %d of %d!\n", my_rank, comm_sz);
                for(int q = 1; q<comm_sz; q++)
                {
                    MPI_Recv(greeting, MAX_STRING, MPI_CHAR, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    printf("%s\n", greeting);
                }
            }
        }
        
    }
    MPI_Finalize();
    return 0;


}