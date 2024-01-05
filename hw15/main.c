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
    sprintf(query, "SELECT MIN(%1$s), MAX(%1$s), AVG(%1$s), SUM(%1$s), VAR_POP(%1$s) FROM %2$s", colname, tablename);
    PGresult* res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        finish_with_error(conn);
    }

    int nrows = PQntuples(res);
    if (nrows != 1) {
        finish_with_error(conn);
    }

    float stat_min   = atof(PQgetvalue(res, 0, 0));
    float stat_max   = atof(PQgetvalue(res, 0, 1));
    float stat_avg   = atof(PQgetvalue(res, 0, 2));
    float stat_total = atof(PQgetvalue(res, 0, 3));
    float stat_disp  = atof(PQgetvalue(res, 0, 4));

    printf("stat_avg: %f\n", stat_avg);
    printf("stat_max: %f\n", stat_max);
    printf("stat_min: %f\n", stat_min);
    printf("stat_total: %f\n", stat_total);
    printf("stat_disp: %f\n", stat_disp);

    PQclear(res);
    PQfinish(conn);
    return EXIT_SUCCESS;
}
