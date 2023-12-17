#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

static void __attribute__((noreturn)) finish_with_error(PGconn *conn) {
    puts(PQerrorMessage(conn));
    PQfinish(conn);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    if (argc != 4) {
        perror("need args");
        return EXIT_FAILURE;
    }
    char *dbname = argv[1];
    char *tablename = argv[2];
    char *colname = argv[3];

    char dburl[100];
    sprintf(dburl, "host=localhost port=5432 dbname=%s user=postgres connect_timeout=10", dbname);
    PGconn *conn = PQconnectdb(dburl);
    if (PQstatus(conn) != CONNECTION_OK) {
        finish_with_error(conn);
    }

    char query[500];
    sprintf(query, "SELECT %s FROM %s", colname, tablename);
    PGresult* res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        finish_with_error(conn);
    }

    float stat_avg = 0;
    float stat_max = 0;
    float stat_min = 0;
    float stat_total = 0;
    float stat_total_2 = 0;
    float stat_total_disp = 0;

    int nrows = PQntuples(res);
    for (int i = 0; i < nrows; i++) {
        float val = atof(PQgetvalue(res, i, 0));

        stat_total += val;
        stat_total += val * val;

        if (i == 0) {
            float stat_avg = val;
            float stat_max = val;
            float stat_min = val;
            continue;
        }

        stat_avg = (stat_avg + val) / 2 ;
        if ( val > stat_max)
            stat_max = val;
        if ( val < stat_min)
            stat_max = val;
    }

    float stat_avg = 0;
    float stat_max = 0;
    float stat_min = 0;
    float stat_total = 0;

    float stat_disp = stat_total_2 / nrows - stat_avg * stat_avg ;

    printf("stat_avg: %f\n", stat_avg);
    printf("stat_max: %f\n", stat_max);
    printf("stat_min: %f\n", stat_min);
    printf("stat_total: %f\n", stat_total);
    printf("stat_disp: %f\n", stat_disp);

    PQclear(res);
    PQfinish(conn);
    return EXIT_SUCCESS;
}
