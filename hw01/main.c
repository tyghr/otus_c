# include <stdio.h>

#define SIGNATURE_SIZE 4
#define FILENAME_SIZE 20

const int test_sig[] = {0x00, 0x31 , 0x32, 0x33};
const int zip_file_header[] = {0x50, 0x4B , 0x03, 0x04};
const int zip_cd_end[] = {0x50, 0x4B , 0x05, 0x06};
const int zip_filename_offset = 0x01a;

int check_signature(const int * sig, int size, int * buf, int pos) {
    while (--size > -1) {
        if (buf[(pos+size+1)%SIGNATURE_SIZE] != sig[size]) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char **argv) { 
    FILE *fp;
    int buf[SIGNATURE_SIZE]={0};
    long int cur=-1;

    for (int i=1; i < argc; i++) {
        fp=fopen(argv[i],"rb");
        if ( fp == NULL ) {
            return 1;
        }

        while (fread(&buf[(++cur)%SIGNATURE_SIZE],sizeof(char),1,fp)==1 && buf[cur%SIGNATURE_SIZE]!=EOF) {
            if (check_signature(zip_file_header, SIGNATURE_SIZE, buf, cur)) {
                    long int offset=ftell(fp);
                    fseek(fp,zip_filename_offset,SEEK_CUR);
                    char fname[FILENAME_SIZE] = {0};
                    int j,c;
                    for (j=0, c=0; (c = fgetc(fp)) != EOF && j < FILENAME_SIZE; j++)
                        fname[j] = (char) c;
                    fname[j]='\0';
                    printf("%s\tdetected signature zip_file_header at %08lx\twith filename %s\n",argv[i],offset-SIGNATURE_SIZE+1,fname);
            } else if (check_signature(zip_cd_end, SIGNATURE_SIZE, buf, cur)) {
                    long int offset=ftell(fp);
                    printf("%s\tdetected signature zip_cd_end at\t%08lx\n",argv[i],offset-SIGNATURE_SIZE+1);
            }
        }
        
        fclose(fp);
    }

    return 0;
}
