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

int main()
{
    
    char txt[] = "abcabcdebcdefgtabishganababceabeevadaaaabcdeabcdebcdefgtabishganababceabeevadaaabcdefgtabishganababceabeevadaaadebcdefgtabishganababceabeevadaaaabcdeabcdebcdefgtabishganababceabeevadaaabcdefgtabishganababceabeevadaaa";
    char pat[] = "abc";
    
    
  /*############################################################################################
    # Definizione Del LPS                                                                      #
    ############################################################################################*/
    int M = strlen(pat);

    int lps[M];

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

    MPI_Init(NULL, NULL);
        //char pattern_to_find = patterns[i];
        // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    
    //padding the txt for separation
    int N = strlen(txt);
    int cores = world_size;
    //printf("%d\n", N);
    int newN = N;
    while(newN%cores != 0)
    {
        newN++;
    }
    //printf("%d\n", newN);

    char padded_txt[newN];
    strcpy(padded_txt, txt);

    for(int v = N; v<newN; v++)
    {
        padded_txt[v] = '$';
    }

    int newN_chunk = newN/cores;

    //define last core
    if(world_rank == world_size-1)
    {
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
        }
    }

    //define other cores
    else
    {
        i = newN_chunk*world_rank; // index for txt[]
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
        }
    }

    MPI_Finalize();

}