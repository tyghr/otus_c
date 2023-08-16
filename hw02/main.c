# include <stdio.h>
# include <string.h>

# define ENC_MAX 3
const char * encoding_names[ENC_MAX] = {"cp1251","koi8r","iso8859"};
char all_encoding_names[100];
const int encodings[ENC_MAX][128]   = {
    {0x0402,0x0403,0x201A,0x0453,0x201E,0x2026,0x2020,0x2021,0x20AC,0x2030,0x0409,0x2039,0x040A,0x040C,0x040B,0x040F,0x0452,0x2018,0x2019,0x201C,0x201D,0x2022,0x2013,0x2014,0x0000,0x2122,0x0459,0x203A,0x045A,0x045C,0x045B,0x045F,0x00A0,0x040E,0x045E,0x0408,0x00A4,0x0490,0x00A6,0x00A7,0x0401,0x00A9,0x0404,0x00AB,0x00AC,0x00AD,0x00AE,0x0407,0x00B0,0x00B1,0x0406,0x0456,0x0491,0x00B5,0x00B6,0x00B7,0x0451,0x2116,0x0454,0x00BB,0x0458,0x0405,0x0455,0x0457,0x0410,0x0411,0x0412,0x0413,0x0414,0x0415,0x0416,0x0417,0x0418,0x0419,0x041A,0x041B,0x041C,0x041D,0x041E,0x041F,0x0420,0x0421,0x0422,0x0423,0x0424,0x0425,0x0426,0x0427,0x0428,0x0429,0x042A,0x042B,0x042C,0x042D,0x042E,0x042F,0x0430,0x0431,0x0432,0x0433,0x0434,0x0435,0x0436,0x0437,0x0438,0x0439,0x043A,0x043B,0x043C,0x043D,0x043E,0x043F,0x0440,0x0441,0x0442,0x0443,0x0444,0x0445,0x0446,0x0447,0x0448,0x0449,0x044A,0x044B,0x044C,0x044D,0x044E,0x044F},
    {0x2500,0x2502,0x250C,0x2510,0x2514,0x2518,0x251C,0x2524,0x252C,0x2534,0x253C,0x2580,0x2584,0x2588,0x258C,0x2590,0x2591,0x2592,0x2593,0x2320,0x25A0,0x2219,0x221A,0x2248,0x2264,0x2265,0x00A0,0x2321,0x00B0,0x00B2,0x00B7,0x00F7,0x2550,0x2551,0x2552,0x0451,0x2553,0x2554,0x2555,0x2556,0x2557,0x2558,0x2559,0x255A,0x255B,0x255C,0x255D,0x255E,0x255F,0x2560,0x2561,0x0401,0x2562,0x2563,0x2564,0x2565,0x2566,0x2567,0x2568,0x2569,0x256A,0x256B,0x256C,0x00A9,0x044E,0x0430,0x0431,0x0446,0x0434,0x0435,0x0444,0x0433,0x0445,0x0438,0x0439,0x043A,0x043B,0x043C,0x043D,0x043E,0x043F,0x044F,0x0440,0x0441,0x0442,0x0443,0x0436,0x0432,0x044C,0x044B,0x0437,0x0448,0x044D,0x0449,0x0447,0x044A,0x042E,0x0410,0x0411,0x0426,0x0414,0x0415,0x0424,0x0413,0x0425,0x0418,0x0419,0x041A,0x041B,0x041C,0x041D,0x041E,0x041F,0x042F,0x0420,0x0421,0x0422,0x0423,0x0416,0x0412,0x042C,0x042B,0x0417,0x0428,0x042D,0x0429,0x0427,0x042A},
    {0x0080,0x0081,0x0082,0x0083,0x0084,0x0085,0x0086,0x0087,0x0088,0x0089,0x008A,0x008B,0x008C,0x008D,0x008E,0x008F,0x0090,0x0091,0x0092,0x0093,0x0094,0x0095,0x0096,0x0097,0x0098,0x0099,0x009A,0x009B,0x009C,0x009D,0x009E,0x009F,0x00A0,0x0401,0x0402,0x0403,0x0404,0x0405,0x0406,0x0407,0x0408,0x0409,0x040A,0x040B,0x040C,0x00AD,0x040E,0x040F,0x0410,0x0411,0x0412,0x0413,0x0414,0x0415,0x0416,0x0417,0x0418,0x0419,0x041A,0x041B,0x041C,0x041D,0x041E,0x041F,0x0420,0x0421,0x0422,0x0423,0x0424,0x0425,0x0426,0x0427,0x0428,0x0429,0x042A,0x042B,0x042C,0x042D,0x042E,0x042F,0x0430,0x0431,0x0432,0x0433,0x0434,0x0435,0x0436,0x0437,0x0438,0x0439,0x043A,0x043B,0x043C,0x043D,0x043E,0x043F,0x0440,0x0441,0x0442,0x0443,0x0444,0x0445,0x0446,0x0447,0x0448,0x0449,0x044A,0x044B,0x044C,0x044D,0x044E,0x044F,0x2116,0x0451,0x0452,0x0453,0x0454,0x0455,0x0456,0x0457,0x0458,0x0459,0x045A,0x045B,0x045C,0x00A7,0x045E,0x045F},
};

void fill_encodings_str() {
    all_encoding_names[0]='\0';
    for (int i=0; i<ENC_MAX;) {
        strcat(all_encoding_names, encoding_names[i++]);
        if (i!=ENC_MAX)
            strcat(all_encoding_names, "|");
    }
    return;
}

int exit_error_wrong_args() {
    printf("Allowed options: \n\
 -c <%s> \n\
 -i <input_filename> \n\
 -o [output_filename] // default: stdout \n\
\n", all_encoding_names);
    return -1;
}

int validate_input_enc(char * encode_from) {
    for (int i=0; i<ENC_MAX; i++)
        if (strcmp(encode_from,encoding_names[i])==0)
            return i;
    return -1;
}

int write_utf8_char(unsigned int c, FILE * fout) {
    if (c <= 0x7f) {
        // 0xxxxxxx
        unsigned int c1 = c&((1<<7)-1);
        fwrite(&c1, sizeof(char), 1, fout);
    } else if (c <= 0x7ff) {
        // 110xxxxx 10xxxxxx
        unsigned int c1 = ((c>>6)&((1<<5)-1))|(3<<6);
        unsigned int c2 = (c&((1<<6)-1))|(1<<7);
        fwrite(&c1, sizeof(char), 1, fout);
        fwrite(&c2, sizeof(char), 1, fout);
    } else if (c <= 0xffff) {
        // 1110xxxx 10xxxxxx 10xxxxxx
        unsigned int c1 = ((c>>12)&((1<<4)-1))|(7<<5);
        unsigned int c2 = ((c>>6)&((1<<6)-1))|(1<<7);
        unsigned int c3 = (c&((1<<6)-1))|(1<<7);
        fwrite(&c1, sizeof(char), 1, fout);
        fwrite(&c2, sizeof(char), 1, fout);
        fwrite(&c3, sizeof(char), 1, fout);
    } else if (c <= 0x10ffff) {
        // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        unsigned int c1 = ((c>>18)&((1<<3)-1))|(15<<4);
        unsigned int c2 = ((c>>12)&((1<<6)-1))|(1<<7);
        unsigned int c3 = ((c>>6)&((1<<6)-1))|(1<<7);
        unsigned int c4 = (c&((1<<6)-1))|(1<<7);
        fwrite(&c1, sizeof(char), 1, fout);
        fwrite(&c2, sizeof(char), 1, fout);
        fwrite(&c3, sizeof(char), 1, fout);
        fwrite(&c4, sizeof(char), 1, fout);
    } else {
        return -1;
    }

    return 0;
}

int convert_file(FILE * fin, FILE * fout, int enc_id) {
    unsigned char c;
    
    // unsigned char utf8_signature[] = {0xef, 0xbb, 0xbf};
    // fwrite(&utf8_signature, sizeof(char), 3, fout);

    while(fread(&c,sizeof(char),1,fin)==1 && !feof(fin) && !ferror(fin)){
        if (c<128) {
            write_utf8_char(c,fout);
        } else {
            write_utf8_char(encodings[enc_id][c-128],fout);
        }
    }

    return 0;
}

int main(int argc, char **argv) {
    char * in_filename = "";
    char * out_filename = "";
    char * encode_from = "";
    FILE *fin;
    FILE *fout;
    int enc_id;
    fill_encodings_str();

    for (int i=1; i < argc; i++) {
        if ( argv[i][0] != '-' ) {
            printf("Unknown option %s\n\n", argv[i]);
            return exit_error_wrong_args();
        }

        if (++i == argc) {
            printf("Not enough args after %s\n\n", argv[i-1]);
            return exit_error_wrong_args();
        }

        switch (argv[i-1][1]) {
        case 'c':
            encode_from = argv[i];
            break;
        case 'i':
            in_filename = argv[i];
            break;
        case 'o':
            out_filename = argv[i];
            break;
        default:
            printf("Unknown option -%c\n", argv[i][1]);
            return exit_error_wrong_args();
        }
    }

    if (strlen(in_filename) == 0 || strlen(encode_from) == 0) {
        printf("Not enough args\n\n");
        return exit_error_wrong_args();
    }

    if ((enc_id = validate_input_enc(encode_from)) == -1) {
        printf("Wrong encoding %s\n", encode_from);
        return exit_error_wrong_args();
    }

    fin=fopen(in_filename,"rb");
    if ( fin == NULL ) {
        printf("Error opening input file\n\n");
        return -1;
    }

    if (strlen(out_filename) == 0) {
        out_filename = "stdout";
        fout = stdout;
    } else {
        fout=fopen(out_filename,"wb");
        if ( fout == NULL ) {
            printf("Error opening output file\n\n");
            return -1;
        }
    }

    printf("********************\n");
    printf(" reading from %s\n", in_filename);
    printf(" writing to %s\n", out_filename);
    printf(" encoding from %s (enc_id=%d)\n", encode_from, enc_id);
    printf("********************\n");

    convert_file(fin, fout, enc_id);

    fclose(fin);
    fclose(fout);

    return 0;
}
