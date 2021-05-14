#include <mpi.h>
#include <stdio.h>

const int MAX_STRING = 100;

int main(int argc, char** argv) {
    int k;
    char sentence[MAX_STRING];
    sprintf(sentence, "frase da analizzare estremamente lunga mannaggia se è lunga oddiooooooooo eee");
    char patterns[] = {'a', 'e', 'g', 'o'};
    int patterns_len = sizeof(patterns)/sizeof(patterns[0]);

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    for(int i = 0; i<patterns_len; i++)
    {
        char pattern_to_find = patterns[i];
        // Get the number of processes
        int world_size;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);

        // Get the rank of the process
        int world_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

        // Get the name of the processor
        char processor_name[MPI_MAX_PROCESSOR_NAME];
        int name_len;
        MPI_Get_processor_name(processor_name, &name_len);

        // Print off a hello world message
        for(k = world_rank; k<MAX_STRING; k+=world_size)
        {
            char string_piece = sentence[k];
            if(pattern_to_find == string_piece)
            {
                printf("Process %d found pattern %c at index %d!\n", world_rank, pattern_to_find, k);
            }
        }
    }

    // Finalize the MPI environment.
    MPI_Finalize();
}
