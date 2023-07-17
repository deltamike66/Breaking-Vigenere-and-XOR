#include <stdio.h>
#define KEY_LENGTH 7

main()
{
    unsigned char ch;
    FILE *fpIn, *fpOut;
    int i;
    unsigned char key[KEY_LENGTH] = {0xC0, 0x00, 0XFF, 0xFF, 0xEE, 0xEE, 0x66};

    fpIn = fopen("plainText.txt", "r");
    fpOut = fopen("cryptoText.txt", "w");
    i = 0;
    while (fscanf(fpIn, "%c", &ch) != EOF)
    {
        if (ch != '\n')
        {
            fprintf(fpOut, "%02X", ch ^ key[i % KEY_LENGTH]); // ^ is logical XOR
            i++;
        }
    }

    fclose(fpIn);
    fclose(fpOut);
    return;
}