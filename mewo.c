//402105857
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#define api "e8e4c660e284651ad443503a2fa52726"
#define url "http://api.openweathermap.org/data/2.5/forecast?q=%s&appid=%s&units=metric"

struct memoryy {
    char *data;
    size_t size_d;
};
struct memoryy *create_memory() {
    struct memoryy *mem = malloc(sizeof(struct memoryy));
    if (mem) {
        mem->data = malloc(1);  
        mem->data[0] = '\0';     
        mem->size_d = 0;        
    }
    return mem;
}
static int callback(size_t *contents, size_t size, size_t nmemb, void *x) {
    size_t sizer = size * nmemb; 
    struct memoryy *mem = (struct memoryy *)x; 
    size_t new_size = mem->size_d + sizer + 1; 
    char *new_data = realloc(mem->data, new_size);
    if (new_data == NULL) {
        printf("Error in memory allocation\n");
        return 0;
    }
    memcpy(new_data + mem->size_d, contents, sizer);
    new_data[new_size - 1] = 0; 
    mem->data = new_data; 
    mem->size_d = new_size - 1; 
    return sizer; 
}
char *get_weather(const char *city) {
    CURL *curl;
    CURLcode result;
    struct memoryy *y = create_memory(); 
    if (!y) {
        fprintf(stderr, "Failed to allocate memory\n");
        return 0;
    }
    char urll[256];
    snprintf(urll, sizeof(urll), url, city, api);
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, urll);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)y);
        result = curl_easy_perform(curl);
        if (result != CURLE_OK) {
            fprintf(stderr, "error, details: %s\n", curl_easy_strerror(result));
            free(y->data);
            free(y);
            y = NULL;
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return y ? y : NULL; 
}
void print_result(char *jdata) {
    cJSON *json = cJSON_Parse(jdata);
    if (json == NULL) {
        printf("Error parsing JSON\n");
        return;
    }
    cJSON *list = cJSON_GetObjectItem(json, "list");
    if (!list) {
        printf("No information found\n");
        cJSON_Delete(json);
        return;
    }
    for (int i = 0; i < 7; ++i) { 
        cJSON *entry = cJSON_GetArrayItem(list, i);
        cJSON *main = cJSON_GetObjectItem(entry, "main");
        cJSON *temp = cJSON_GetObjectItem(main, "temp");
        cJSON *temp_min = cJSON_GetObjectItem(main, "temp_min"); 
        cJSON *temp_max = cJSON_GetObjectItem(main, "temp_max");
        cJSON *dt_txt = cJSON_GetObjectItem(entry, "dt_txt");
        cJSON *weather = cJSON_GetObjectItem(entry, "weather");
        cJSON *description = cJSON_GetArrayItem(weather, 0);
        printf("date: %s\n", dt_txt->valuestring);
        printf("Average temperature: %d°C\n", temp->valuedouble);
        printf("Min Temperature: %d°C\n", temp_min->valuedouble); 
        printf("Max Temperature: %d°C\n", temp_max->valuedouble);
        printf("Weather: %s\n\n", description->valuestring);
    }
    cJSON_Delete(json);
}

int main() {
    char shahr[50];
    printf("Enter the name of the city you want: ");
    scanf("%40s", shahr);
    char *mewo = get_weather(shahr);
    if (mewo) {
        print_result(mewo);
        free(mewo);
    } else {
        printf("Program execution failed. Please run the program again.\n");
    }

    return 0;
}