#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>

struct MemoryStruct {
    char * memory;
    size_t size;
};

static size_t WriteMemoryCallback(void * contents, size_t size, size_t nmemb, void * userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct * mem = (struct MemoryStruct*)userp;

    char * ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = '\0';

    return realsize;
}

struct weather {
    struct json_object *j_temp_C;
    struct json_object *j_FeelsLikeC;
    struct json_object *j_humidity;
    struct json_object *j_pressure;
    struct json_object *j_uvIndex;
    struct json_object *j_visibility;
    struct json_object *j_weatherDesc;
    struct json_object *j_winddir16Point;
    struct json_object *j_windspeedKmph;
};

int main(int argc, char **argv) {
    if (argc != 2) {
        perror("need cityname");
        return EXIT_FAILURE;
    }
    char *city = argv[1];
    char url[100];
    sprintf(url, "http://wttr.in/%s?format=j1", city);

    CURL * curl_handle;
    CURLcode res;

    struct MemoryStruct chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_ALL);

    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    // TODO curl_easy_perform не обрабатывается, попробуйте запустить программу с отключенной сетью, получите segfault - не хорошо
    res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return EXIT_FAILURE;
    }
    curl_easy_cleanup(curl_handle);

    // json decode
    struct weather *current_weather = (struct weather *) malloc(sizeof(struct weather));
    struct json_object *j_current_condition;

    json_object_object_get_ex(
        json_tokener_parse(chunk.memory),
        "current_condition",
        &j_current_condition
    );
    if (json_object_array_length(j_current_condition) < 1) {
        return EXIT_FAILURE;
    }

    // TODO Имена json полей можно было бы вынести отдельными дефайнами
    // TODO json_object_object_get_ex возвращает bool, результат удалось ли распарсить поле, у вас это нигде не проверяется
    json_object_object_get_ex(json_object_array_get_idx(j_current_condition, 0), "temp_C", &current_weather->j_temp_C);
    json_object_object_get_ex(json_object_array_get_idx(j_current_condition, 0), "FeelsLikeC", &current_weather->j_FeelsLikeC);
    json_object_object_get_ex(json_object_array_get_idx(j_current_condition, 0), "humidity", &current_weather->j_humidity);
    json_object_object_get_ex(json_object_array_get_idx(j_current_condition, 0), "pressure", &current_weather->j_pressure);
    json_object_object_get_ex(json_object_array_get_idx(j_current_condition, 0), "uvIndex", &current_weather->j_uvIndex);
    json_object_object_get_ex(json_object_array_get_idx(j_current_condition, 0), "visibility", &current_weather->j_visibility);
    json_object_object_get_ex(json_object_array_get_idx(j_current_condition, 0), "winddir16Point", &current_weather->j_winddir16Point);
    json_object_object_get_ex(json_object_array_get_idx(j_current_condition, 0), "windspeedKmph", &current_weather->j_windspeedKmph);
    struct json_object *j_w_desc;
    json_object_object_get_ex(json_object_array_get_idx(j_current_condition, 0), "weatherDesc", &j_w_desc);
    if (json_object_array_length(j_w_desc) > 0) {
        json_object_object_get_ex(json_object_array_get_idx(j_w_desc, 0), "value", &current_weather->j_weatherDesc);
    }

    printf("weather in %s:\n", city);
    printf("\tdesc: %s\n", json_object_get_string(current_weather->j_weatherDesc));
    printf("\ttemp: %sC\tfeels like: %sC\n", json_object_get_string(current_weather->j_temp_C), json_object_get_string(current_weather->j_FeelsLikeC));
    printf("\twind: %skmh %s\n", json_object_get_string(current_weather->j_windspeedKmph), json_object_get_string(current_weather->j_winddir16Point));
    printf("\tvisibility: %s\n", json_object_get_string(current_weather->j_visibility));
    printf("\tuvIndex: %s\n", json_object_get_string(current_weather->j_uvIndex));
    printf("\tpressure: %s\n", json_object_get_string(current_weather->j_pressure));
    printf("\thumidity: %s\n", json_object_get_string(current_weather->j_humidity));

    // Delete the json objects
    json_object_put(current_weather->j_weatherDesc);
	json_object_put(j_w_desc);
    json_object_put(current_weather->j_windspeedKmph);
    json_object_put(current_weather->j_winddir16Point);
    json_object_put(current_weather->j_visibility);
    json_object_put(current_weather->j_uvIndex);
    json_object_put(current_weather->j_pressure);
    json_object_put(current_weather->j_humidity);
    json_object_put(current_weather->j_FeelsLikeC);
    json_object_put(current_weather->j_temp_C);
	json_object_put(j_current_condition);

    free(current_weather);
    free(chunk.memory);

    curl_global_cleanup();

    return 0;
}
