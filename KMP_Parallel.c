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

int main()
{
  /*############################################################################################
    # Definizione Del LPS                                                                      #
    ############################################################################################*/

    MPI_Init(NULL, NULL);
        //char pattern_to_find = patterns[i];
        // Get the number of processes
    int cores;
    MPI_Comm_size(MPI_COMM_WORLD, &cores);
    // Get the rank of the process
    int core_number;
    MPI_Comm_rank(MPI_COMM_WORLD, &core_number);
    
    
    //define last core
    if(core_number == 0)
    {

        char txt[] = "abcabcdebcdefgtabishganababceabeevadaaaabcdeabcdebcdefgtabishganababceabeevadaaabcdefgtabishganababceabeevadaaadebcdefgtabishganababceabeevadaaaabcdeabcdebcdefgtabishganababceabeevadaaabcdefgtabishganababceabeevadaaa";
        char pat[] = "abc";
        int M = strlen(pat);
        int lps[M];
        computeLPSArray(pat, M, lps);
        printf("process %d, number %d", core_number, M);
        for(int k = 1; k<cores; k++)
            {
                MPI_Send(&M, 1, MPI_INT, k, 0, MPI_COMM_WORLD);
                MPI_Send(&lps, M, MPI_INT, k, 0, MPI_COMM_WORLD);
            }

        /*int N = strlen(txt);
        int cores = world_size;
        //printf("%d\n", N);
        int newN = N;
        int i;
        int newN_chunk = newN/cores;

        while(newN%cores != 0)
        {
            newN++;
        }
    
        char padded_txt[newN];
        strcpy(padded_txt, txt);
    
        for(int v = N; v<newN; v++)
        {
            padded_txt[v] = '$';
        }
        i = newN_chunk*world_rank; // index for txt[]
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
                printf("core %d found pattern at index %d \n", world_rank, i - j);
                j = lps[j - 1];
            }
  
            // mismatch after j matches
            else if (i < N && pat[j] != txt[i])
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
        }*/
    }

    //define other cores
    else
    {
        int local_M;
        int local_lps[local_M];
        MPI_Recv(&local_M, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&local_lps, local_M, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //printf("process %d, number %d", core_number, local_M);
        for(int i = 0; i<local_M; i++)
        {
            printf("%d\n", local_lps[i]);
        }
        /*i = newN_chunk*world_rank; // index for txt[]
        int j = 0; // index for pat[]

        while (i < newN_chunk*(world_rank+1)+M)
        {
            if (pat[j] == txt[i])
            {
                j++;
                i++;
            }
  
            if (j == M) 
            {
                printf("core %d found pattern at index %d \n", world_rank, i - j);
                j = lps[j - 1];
            }
  
            // mismatch after j matches
            else if (i < newN_chunk*(world_rank+1)+M && pat[j] != txt[i])
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
        }*/
    }

    MPI_Finalize();

}