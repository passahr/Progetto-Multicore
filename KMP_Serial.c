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

#define MAX_STRING_LENGTH 40
#define MAX_STRING_NUM 30


  
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


  
// Driver program to test above function
int main()
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

    printf("%d \n", i);
    printf("%d \n", num_packet);

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

    packet pack;
    packet packets_array[num_packet];
    last_i = 0;
    num_packet = 0;

    for(i = 0; i<datastream_length; i++)
    {   
        if(datastream[i]=='\n')
        {
            pack.start = last_i;
            pack.end = i-1;
            pack.size = -(last_i-i);
            last_i = i+1;
            pack.number = num_packet+1;
            packets_array[num_packet] = pack;
            num_packet++;
        }
    }
    
   char character; 
   int j, index;
   char string_patterns[MAX_STRING_NUM][MAX_STRING_LENGTH] = {"hhh", "karooooooo", "play"};
   for(i = 0; i<num_packet; i++)
   {
       packet pack = packets_array[i];

       char stringa_da_confrontare[pack.size];
       index = 0;
       for(j = pack.start; j<=pack.end; j++)
        {
            character = datastream[j];
            stringa_da_confrontare[index] = character;
            index++;
        }
        
        

        for(j = 0; j<3; j++  )
        {
            
            KMPSearch(string_patterns[j], stringa_da_confrontare, pack.number);

        }

        printf("\n");
        
   }
    
    return 0;
}
