#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

/*
    Input: unsigned Integer between 0 and 255
    Transforms integer to hexadecimal
    Output: Char with the hexadecimal value of i
*/
char Int2Char(const int i) {
    if (i > 9) return (char) i - 10 + 'a';
    else return (char) (i + '0');
}

/*
    Input: unsigned Integer between 0 and 255 and a String that will be the output
    Transforms integer to hexadecimal but every 4 bits in a position of the array
    Output: -
*/
void Int2Hex(const int i, char* s) {
    s[0] = Int2Char(i%16);
    s[1] = Int2Char(i/16);
}

/*
    Input: Char with an ASCII value
    Transforms the ASCII value into Hex value
    Output: Char with a Hex value
*/
unsigned char Hex2Char (char c) {
    if ('0' <= c && c <= '9') return (unsigned char)(c - '0');
    if ('A' <= c && c <= 'F') return (unsigned char)(c - 'A' + 10);
    if ('a' <= c && c <= 'f') return (unsigned char)(c - 'a' + 10);
    return 0xFF;
}

/*
    Input: the input String and the output String
    Transforms the Hex plain text into a hexadecimal values
    Output: -
*/
void parse_to_bytes(unsigned char* input, unsigned char* output) {
    for (int i = 0; i < strlen(input)-1; i += 2) 
        output[i/2] = (Hex2Char(input[i]) << 4) + Hex2Char(input[i+1]);
}

/*
    Input: A String with the output of Openssl
    Parses the output of the Openssl command and saves in the same String
    Output: -
*/
void parse_output(char* ptr) {
    memcpy(ptr, &ptr[2], strlen(ptr)-3);
    int i = 0, j = 0;
    while (1) {
        if (ptr[i] == ' ') {
            if (ptr[i+1] == ' ') {
                ptr[i] = '\0';
                break;
            } else memcpy(&ptr[i], &ptr[i+1], sizeof ptr -1);
        } else i++;
    }
}

/*
    Input: m is the message to encrypt and a string with the key 
    Calculates the input message (m) 
    Output: The value of the message computed
*/
char get_M0(char m, char key[]) {
    printf("Guessing m[0] ...... \n");
    uint i, index;
    char s[2];
    char str[128];
    char buf[256];
    unsigned char *ptr, *ptr_res;
    uint mapm[256] = {0};
    FILE *fp;

    for (i = 0; i < 256; ++i) {
        Int2Hex(i, s);

        sprintf(str, "echo -n '%c' | openssl enc -K '01FF%c%c%s' -rc4 | xxd", m, s[1], s[0], key);
        fp = popen(str,"r"); 
        if (fgets(buf, sizeof buf, fp) != NULL) {
            ptr = strchr(buf, ':');
            parse_output(ptr);
            ptr_res = malloc((strlen(ptr)/2) * sizeof(char));
            parse_to_bytes(ptr, ptr_res);
            
            index = ((i+2)%256 ^ ptr_res[0]) % 256;
            mapm[index]++;
            fclose(fp);
        }
    }
    int res = 0;
    for (i = 1; i < 256; ++i) 
        if (mapm[i] > mapm[res]) res = i;
    
    printf("Guessed m[0] = %c (with freq. %u)\n", res, mapm[res]);
    return res;
}

int main() {
    uint i, j, index;
    char key[27];
    char m_res, v, acc = 0x03;
    char s[2];
    char str[128];
    char buf[256];
    unsigned char *ptr, *ptr_res, *key_bytes;
    const char m = 0x48;
    FILE *fp;

    fp = popen("openssl rand -hex 13","r"); 
    if (fgets(key, sizeof key, fp) != NULL) printf("The key is: %s, and the message is: %c\n", key, m);
    key_bytes = malloc((strlen(key)/2) * sizeof(char));
    parse_to_bytes(key, key_bytes);

    m_res = get_M0(m, key);

    for (i = 0; i < 13; ++i) {
        v = Int2Char(i+0x03);
        acc += i+3;
        char res[256] = {0};

        printf("Guessing k[%u] ...... \n", i);
        for (j = 0; j < 256; ++j) {
            Int2Hex(j, s);
            
            sprintf(str, "echo -n '%c' | openssl enc -K '0%cFF%c%c%s' -rc4 | xxd", m, v, s[1], s[0], key);
            fp = popen(str,"r"); 
            if (fgets(buf, sizeof buf, fp) != NULL) {
                ptr = strchr(buf, ':');
                parse_output(ptr);
                ptr_res = malloc((strlen(ptr)/2) * sizeof(char));
                parse_to_bytes(ptr, ptr_res);

                index = (((ptr_res[0]^m_res)%256) - j - acc)%256;
                res[index]++;
                fclose(fp);
            }
        }
        uint k, max = 0;
        char aux[2];
        for (k = 1; k < 256; ++k) 
            if (res[k] > res[max]) max = k;
        acc += max;
        Int2Hex(max, aux);
        
        printf("Guessed k[%u] = %c%c (with freq. %u)\n", i, aux[1], aux[0], res[max]);
    }
}
