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

int main(int argc, char *argv[])
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

    char pat[20];
    strcpy(pat, argv[1]);
    
    
    //define last core
    if(core_number == 0)
    {
        
        
        int M = strlen(pat);
        int N = strlen(txt);
        int lps[M];
        int newN = N/cores+M;

        computeLPSArray(pat, M, lps);
        for(int k = 1; k<cores; k++)
            {
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
        for(int k = 1; k<cores; k++)
        {
            int foundValues;
            MPI_Recv(&foundValues, 1, MPI_INT, k, 0, MPI_COMM_WORLD, NULL);
            if(foundValues > 0)
            {
                for(j = 0; j<foundValues;j++)
                {
                    int value;
                    MPI_Recv(&value, 1, MPI_INT, k, 0, MPI_COMM_WORLD, NULL);
                    indexes[x_indexes] = value;
                    x_indexes++;
                }
            }
        }
        for(int k = 0; k<x_indexes; k++)
        {
            printf("%d\n", indexes[k]);
        }

    }

    //define other cores
    else
    {
        int local_M, local_N;
        int local_lps[local_M];
        MPI_Recv(&local_M, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&local_lps, local_M, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&local_N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int start = (local_N/cores)*core_number;
        int end, i;
        if(core_number==cores-1)
        {
            end = local_N;
        }
        else
        {
            end = (local_N/cores)*(core_number+1)+local_M;
        }
        //printf("process %d, number %d", core_number, local_M);
        int possible_targets = end-start;
        int next_target = 0;
        int targets_array[possible_targets];
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
                targets_array[next_target] = i-j;
                next_target++; 
                //printf("core %d found pattern at index %d \n", core_number, i - j);
                j = local_lps[j - 1];
            }
  
            // mismatch after j matches
            else if (i < end && pat[j] != txt[i])
            {
            // Do not match lps[0..lps[j-1]] characters,
            // they will match anyway
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
        MPI_Send(&next_target, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        if(next_target > 0)
        {
            
            for(j = 0; j<next_target; j++)
            {
                int value = targets_array[j];
                MPI_Send(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
        }
        
    }

    MPI_Finalize();

}