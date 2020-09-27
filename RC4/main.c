#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

char Int2Char(const int i) {
    if (i > 9) {
        switch (i) {
            case 10:
                return 'a';
                break;
            case 11:
                return 'b';
                break;
            case 12:
                return 'c';
                break;
            case 13:
                return 'd';
                break;
            case 14:
                return 'e';
                break;
            case 15:
                return 'f';
                break;
        }
    } else return (char) (i + '0');
}

void Int2Hex(const int i, char* s) {
    if (i%16 > 9) {
        switch (i%16) {
            case 10:
                s[0] = 'a';
                break;
            case 11:
                s[0] = 'b';
                break;
            case 12:
                s[0] = 'c';
                break;
            case 13:
                s[0] = 'd';
                break;
            case 14:
                s[0] = 'e';
                break;
            case 15:
                s[0] = 'f';
                break;
        }
    } else s[0] = (char) (i%16 + '0');

    if (i/16 > 9) {
        switch (i/16) {
            case 10:
                s[1] = 'a';
                break;
            case 11:
                s[1] = 'b';
                break;
            case 12:
                s[1] = 'c';
                break;
            case 13:
                s[1] = 'd';
                break;
            case 14:
                s[1] = 'e';
                break;
            case 15:
                s[1] = 'f';
                break;
        }
    } else s[1] = (char) (i/16 + '0');
}

unsigned char Hex2Char (char c) {
    //printf("hola\n");
    if ('0' <= c && c <= '9') return (unsigned char)(c - '0');
    if ('A' <= c && c <= 'F') return (unsigned char)(c - 'A' + 10);
    if ('a' <= c && c <= 'f') return (unsigned char)(c - 'a' + 10);
    return 0xFF;
}

uint Hex2Int (char c) {
    char aux = 0x01;
    uint res = 0;
    for (int i = 0; i < 8; ++i) {
        if ((uint)(aux&c) != 0) res += (uint)pow(2, i);
        aux = aux << 1;
    }
    return res;
}

void parse_to_bytes(unsigned char* input, unsigned char* output) {
    for (int i = 0; i < strlen(input)-1; i += 2) {
        output[i/2] = (Hex2Char(input[i]) << 4) + Hex2Char(input[i+1]);
        //printf("output: %#x\n", (Hex2Char(input[i]) << 4) + Hex2Char(input[i+1]));
    }
}

void parse_output(char* ptr) {
    //printf("1:%s\n", ptr);
    //printf("2:%s\n", &ptr[2]);
    memcpy(ptr, &ptr[2], strlen(ptr)-3);
    //printf("%s\n", ptr);
    int i = 0, j = 0;
    while (1) {
        if (ptr[i] == ' ') {
            if (ptr[i+1] == ' ') {
                ptr[i] = '\0';
                break;
            } else memcpy(&ptr[i], &ptr[i+1], sizeof ptr -1);
        } else i++;
        //printf("%s\n", ptr);
    }
}

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
        //printf("%s", str);
        fp = popen(str,"r"); 
        if (fgets(buf, sizeof buf, fp) != NULL) {
            //printf("%s\n", buf);
            ptr = strchr(buf, ':');
            //printf("%s\n", ptr);
            parse_output(ptr);
            //printf("%s\n", ptr);
            ptr_res = malloc((strlen(ptr)/2) * sizeof(char));
        }
        fclose(fp);
        parse_to_bytes(ptr, ptr_res);
        index = ((i+2)%256 ^ Hex2Int(ptr_res[0])) % 256;
        //printf("%#x %#x %#x %d %d\n", m, key_bytes[0], (i+6)%256, (uint)Hex2Int(key_bytes[0]), (i+6)%256 + Hex2Int(key_bytes[0]) % 256);
        mapm[index]++;
    }
    int res = 0;
    for (i = 1; i < 256; ++i) {
        if (mapm[i] > mapm[res]) res = i;
        //printf("%u ", mapm[i]);
    }
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
            //printf("%s\n", str);
            fp = popen(str,"r"); 
            if (fgets(buf, sizeof buf, fp) != NULL) {
                //printf("%s\n", buf);
                ptr = strchr(buf, ':');
                //printf("%s\n", ptr);
                parse_output(ptr);
                //printf("%s\n", ptr);
                ptr_res = malloc((strlen(ptr)/2) * sizeof(char));
            }
            fclose(fp);
            parse_to_bytes(ptr, ptr_res);
            //printf("%#x %#x\n", ptr_res[0], m_res);
            index = (((Hex2Int(ptr_res[0])^m_res)%256) - j - acc)%256;
            //printf("%#x %#x %#x %d %d\n", m, key_bytes[0], (i+6)%256, (uint)Hex2Int(key_bytes[0]), (i+6)%256 + Hex2Int(key_bytes[0]) % 256);
            res[index]++;
        }
        uint k, max = 0;
        char aux[2];
        for (k = 1; k < 256; ++k) {
            if (res[k] > res[max]) max = k;
            //printf("%u ", res[k]);
        }
        acc += max;
        Int2Hex(max, aux);
        printf("Guessed k[%u] = %c%c (with freq. %u)\n", i, aux[1], aux[0], res[max]);
        //printf("Guessed k[%u] = %c (with freq. %u)\n", res, mapm[res]);
    }
}
