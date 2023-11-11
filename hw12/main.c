#define _GNU_SOURCE

#include <pthread.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>
#include <stdlib.h>
#include <getopt.h>

#include "chan/chan.h"

#define MAX_LINE 1000
#define MAX_TOKENS 100

typedef struct {
    char *key;
    long val;
} url_stat;

GHashTable* url_size_ht;
pthread_mutex_t url_mx = PTHREAD_MUTEX_INITIALIZER;
GHashTable* ref_count_ht;
pthread_mutex_t ref_mx = PTHREAD_MUTEX_INITIALIZER;

chan_t* jobs;
chan_t* done;

void calc(char *str);
void update_top(url_stat top[], int len, char *key, long val);
void* worker();

int main(int argc, char **argv) {
    int c;
    // int digit_optind = 0;
    char dir_path[512] = "/mnt/c/Users/tyghr/Downloads/testlog"; // "/mnt/c/Users/tyghr/Downloads/19_threads_homework-12926-1514cc"
    int num_threads = 1;

    while (1) {
        // int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"logdir", 1, 0, 0},
            {"threads", 1, 0, 0},
            {0, 0, 0, 0}
        };

        c = getopt_long (argc, argv, "d:t:",
                 long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 0:
            if (optarg) {
                if (strcmp(long_options[option_index].name,"logdir") == 0) {
                    strncpy(dir_path, optarg, strlen(optarg));
                } else if (strcmp(long_options[option_index].name,"threads") == 0) {
                    num_threads = atoi(optarg);
                    if (num_threads < 1)
                        exit(EXIT_FAILURE);
                }
            }
            break;
        case 'd':
            strncpy(dir_path, optarg, strlen(optarg));
            break;
        case 't':
            num_threads = atoi(optarg);
            if (num_threads < 1)
                exit(EXIT_FAILURE);
            break;
        default:
            printf ("неправильное использование аргументов\n");
            exit(EXIT_FAILURE);
        }
    }

    url_size_ht = g_hash_table_new(g_str_hash, g_str_equal);
    ref_count_ht = g_hash_table_new(g_str_hash, g_str_equal);

    // Initialize channels.
    jobs = chan_init(10);
    done = chan_init(0);

    for (int i = 0; i < num_threads; i++) {
        pthread_t th;
        pthread_create(&th, NULL, worker, NULL);
    }

    DIR *d;
    struct dirent *delem;
    FILE *fp;
    char fullpath_buf[MAX_LINE];
    char line[MAX_LINE];

    if ((d = opendir(dir_path))) {
        while ((delem = readdir(d)) != NULL) {
            if( strcmp( delem->d_name, "." ) == 0 || strcmp( delem->d_name, ".." ) == 0 )
                continue;

            if( delem->d_type == DT_REG ) {
                sprintf(fullpath_buf,"%s/%s",dir_path, delem->d_name);

                if ( (fp=fopen(fullpath_buf, "r")) != NULL ) {
                    printf("processing %s\n", fullpath_buf);

                    char *cptr;
                    while(!feof (fp)) {
                        if ((cptr = fgets(line, MAX_LINE, fp))) {
                            chan_send(jobs, (void*) strdup(line));
                        }
                    }

                    fclose(fp);
                }
            }
        }
        closedir(d);

        chan_close(jobs);
        printf("sent all jobs\n");
    }

    // Wait for all jobs to be received.
    for (int i = 0; i < num_threads; i++)
        chan_recv(done, NULL);

    GHashTableIter iter;
    char *ikey;
    long *ival;

    long all_total_bytes = 0;
    url_stat top_urls_by_total_size[10];
    url_stat top_refs_by_times[10];
    for (int i=0; i<10; i++) {
        top_urls_by_total_size[i].val = 0;
        top_refs_by_times[i].val = 0;
    }

    g_hash_table_iter_init (&iter, ref_count_ht);
    while (g_hash_table_iter_next (&iter, (gpointer) &ikey, (gpointer) &ival)) {
        update_top(top_refs_by_times, 10, ikey, *ival);
    }

    g_hash_table_iter_init (&iter, url_size_ht);
    while (g_hash_table_iter_next (&iter, (gpointer) &ikey, (gpointer) &ival)) {
        all_total_bytes += *ival;
        update_top(top_urls_by_total_size, 10, ikey, *ival);
    }

    printf("All total bytes: %ld\n", all_total_bytes);

    printf("\nTop refs by times:\n");
    for (int i=0; i<10; i++) {
        if (top_refs_by_times[i].val != 0)
            printf("\ttimes: %ld\tref: %s\n", top_refs_by_times[i].val, top_refs_by_times[i].key);
    }

    printf("\nTop urls by size:\n");
    for (int i=0; i<10; i++) {
        if (top_refs_by_times[i].val != 0)
            printf("\ttotal_size: %ld\turl: %s\t\n", top_urls_by_total_size[i].val, top_urls_by_total_size[i].key);
    }

    g_hash_table_unref(url_size_ht);
    g_hash_table_unref(ref_count_ht);

    // Clean up channels.
    chan_dispose(jobs);
    chan_dispose(done);

    pthread_mutex_destroy(&ref_mx);
    pthread_mutex_destroy(&url_mx);
}

void update_top(url_stat top[], int len, char *key, long val) {
    if (len < 1)
        return;

    for (int i=0; i<len; i++) {
        if (top[i].val < val) {
            for (int j=len-1; j>i; j--) {
                if (top[j-1].val == 0)
                    continue;

                top[j].key = top[j-1].key;
                top[j].val = top[j-1].val;
            }

            top[i].val = val;
            top[i].key = strdup(key);

            return;
        }
    }
}

void calc(char *str) {
    char *saveptr;
    char *token = strtok_r(str, " ", &saveptr);
    char *t_url;
    char *t_ref;
    long *url_size;
    long *ref_count;
    int t_bytes;

    for (int i = 0; i < MAX_TOKENS && token != NULL; i++) {
        if (i == 6) {
            t_url = g_strdup(token);
        } else if (i == 9) {
            t_bytes = atoi(token);
            pthread_mutex_lock(&url_mx);
            if ((url_size = (long*) g_hash_table_lookup(url_size_ht, t_url))) {
                *url_size += t_bytes;
            } else {
                url_size = (long*)malloc(sizeof(long));
                *url_size = t_bytes;
                g_hash_table_insert(url_size_ht, t_url, url_size);
            }
            pthread_mutex_unlock(&url_mx);
        } else if (i == 10) {
            t_ref = g_strdup(token);
            pthread_mutex_lock(&ref_mx);
            if ((ref_count = (long*) g_hash_table_lookup(ref_count_ht, t_ref))) {
                (*ref_count)++;
            } else {
                ref_count = (long*)malloc(sizeof(long));
                *ref_count = 1;
                g_hash_table_insert(ref_count_ht, t_ref, ref_count);
            }
            pthread_mutex_unlock(&ref_mx);
        }
        token = strtok_r(NULL, " ", &saveptr);
    }

    free(token);
}

void* worker() {
    // Process jobs until channel is closed.
    void *job;
    while (chan_recv(jobs, &job) == 0) {
        char *wline = (char *)job;
        calc(wline);
        free(wline);
    }

    chan_send(done, "1");
    return NULL;
}
