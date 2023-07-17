#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

float getStandardDeviation(int cyper[], int size, int k);
void printPlainText(int c[], int size, int nKey, int key[]);

int main(int argc, char const *argv[]) {
    FILE *fpIn, *fpOut;
    int ch;
    int size = 0;
    int *c = malloc(sizeof(*c));
    int *q = malloc(256*sizeof(*q));
    int *stream = malloc(sizeof(*stream));
    int keys[256] = {0};
    float freq;
    int nKey;
    int index;
    int *key = malloc(sizeof(*key));

    // read file and put crypto char in c[]
    if(c) {
        fpIn = fopen("cryptoText.txt", "r");
        while (fscanf(fpIn, "%02X", &ch) != EOF) {
            c[size++] = ch;
            int *tmp = realloc(c, (size + 1) * sizeof(*c));
            if (tmp) {
                c = tmp;
            }
        }
        fclose(fpIn);
    }

    //print the crypto text
    for (int i=0; i<size; i++){
        printf ("%02X",c[i]);
    }
    printf("\n");
    
    //try to find the lenght of key
    for (int nk=2; nk<10; nk++){
        if(q) {
            for (int i=0; i<256; i++){
                q[i]=0;
            }
        }
        
        for (int i=0; i<size; i+=nk){
            q[c[i]]++;
        }
        
        freq=0.0;
        for (int i=0; i<256; i++){
            if (q[i] != 0){
                freq += ((float)q[i]/(size/nk))*((float)q[i]/(size/nk));
            }
        }
        
        if (freq > 0.065) {
            printf("\nFound key length N=%d\n**********************\n", nk);
            nKey = nk;
            int *tmp = realloc(key, nk+1 * sizeof(*key));
            if (tmp) {
                key = tmp;
            }
        }
    }

    for (int t=0; t<nKey; t++){
        index=0;
        float minSd = 1.0;
        int foundK;
        if(stream){
            for (int i=t; i<size; i+=nKey){
                stream[index++] = c[i];
                int *tmp = realloc(stream, (index + 1) * sizeof(*stream));
                if (tmp){
                    stream = tmp;
                }
            }

            //now stream[] cointains the nth char of crypto text
            //the char have to be between 32 and 127 in order to have
            //plain text
            for (int k=0; k<256; k++){
                int fail=0;
                int counter=0;
                for (int i=0; i<index; i++){
                    int p=stream[i]^k;
                    if (isalpha(p)) { 
                        counter++;
                    }
                    if (p < 32 || p > 127){
                        fail = 1;
                        break;
                    }
                }
                if (fail==0){
                    //stream[] of candidates
                    float sd = getStandardDeviation(stream,counter,k);
                    if (sd < minSd && sd != 0.0 && counter >= (index/2)) {
                        minSd = sd;
                        foundK = k;
                    }
                }
            }
            key[t] = foundK;
        }
    }

    printf("key=");
    for (int t=0; t<nKey; t++){
        printf("%02X ", key[t]);
    }
    printf("\n**********************\n");

    printPlainText(c, size, nKey, key);
    
    free(c);
    free(q);
    free(stream);
    free(key);
    return 0;
}

float getStandardDeviation(int cyper[], int size, int k){
    float english[26] = {0.08167, 0.01492, 0.02782, 0.04253, 
		0.12702, 0.02228, 0.02015, 0.06094, 0.06966, 0.00153, 
		0.00772, 0.04025, 0.02406, 0.06749, 0.07507, 0.01929, 
		0.00095, 0.05987, 0.06327, 0.09056, 0.02758, 0.00978, 
		0.02360, 0.00150, 0.01974, 0.00074};
    
    int myText[26] = {0};
    float sd=0.0; 
    
    //count occurrency of char in a stream cyper
    for (int alpha='a'; alpha<'z'+1; alpha++){
        for (int i=0; i<size; i++){
            int p=cyper[i]^k;
            if (p == alpha) {
                myText[alpha-97]++;
            }
        }
    }

    //calc the standard deviation in sd
    for (int i=0; i<26; i++){
        if (myText[i] != 0) {
            sd += (float)myText[i]/(float)size - english[i];
        }       
    }

    return sd;
}

void printPlainText(int c[], int size, int nKey, int key[]){
    int indexKey=0;
    for (int i=0; i<size; i++){
        printf ("%c",c[i]^key[indexKey]);
        indexKey = (++indexKey)%nKey;
    }
    printf("\n");
}
