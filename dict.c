#include <curl/curl.h>
#include <json-c/json.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BASE_URL "https://api.dictionaryapi.dev/api/v2/entries/en/%s"

/* The MemoryStruct struct is a data structure used to store dynamically allocated
memory for receiving and holding HTTP response data during libcurl operations.
It typically includes a pointer to the allocated memory block and a size variable
to track the current size of the allocated memory. */ 

struct MemoryStruct {
    char *memory;
    size_t size;
};

/* The WriteMemoryCallback() function is a callback function used in conjunction with 
libcurl's easy interface to store received data into memory, typically used for 
handling HTTP responses during data transfer operations. */

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) 
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL)
    {
        /* out of memory */
        fprintf(stderr, "not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

void parse_response(const char *json_response)
{
    struct json_object *root;
    root = json_tokener_parse(json_response);

    if (root == NULL)
    {
        printf("Error parsing JSON\n");
        return;
    }

    /* Check if the root object is an array */
    if (json_object_is_type(root, json_type_array))
    {
        int array_len = json_object_array_length(root);
        printf("Number of definitions: %d\n", array_len);

        /* Iterate over each object in the array */
        for (int i = 0; i < array_len; i++)
        {
            struct json_object *obj = json_object_array_get_idx(root, i);
            struct json_object *meanings_array, *word_obj;
            json_object_object_get_ex(obj, "meanings", &meanings_array);
            json_object_object_get_ex(obj, "word", &word_obj);

            if (word_obj != NULL && json_object_get_type(word_obj) == json_type_string)
            {
                const char *word = json_object_get_string(word_obj);
                printf("Word: %s\n", word);
            }
            else
            {
                printf("Word field not found or invalid\n");
            }

            if (meanings_array != NULL && json_object_get_type(meanings_array) == json_type_array)
            {
                int meanings_len = json_object_array_length(meanings_array);

                /* Iterate over meanings */
                for (int j = 0; j < meanings_len; j++)
                {
                    struct json_object *meaning_obj = json_object_array_get_idx(meanings_array, j);
                    struct json_object *part_of_speech_obj, *definitions_array;
                    json_object_object_get_ex(meaning_obj, "partOfSpeech", &part_of_speech_obj);
                    json_object_object_get_ex(meaning_obj, "definitions", &definitions_array);

                    if (j == 0) { // Only print the information for the top definition
                        if (part_of_speech_obj != NULL && json_object_get_type(part_of_speech_obj) == json_type_string)
                        {
                            const char *part_of_speech = json_object_get_string(part_of_speech_obj);
                            printf("Part of Speech: %s\n", part_of_speech);
                        }
                        else
                        {
                            printf("Part of speech field not found or invalid\n");
                        }

                        if (definitions_array != NULL && json_object_get_type(definitions_array) == json_type_array)
                        {
                            int definitions_len = json_object_array_length(definitions_array);

                            /* Print the first definition */
                            if (definitions_len > 0)
                            {
                                struct json_object *first_definition = json_object_array_get_idx(definitions_array, 0);
                                struct json_object *definition_obj;
                                json_object_object_get_ex(first_definition, "definition", &definition_obj);

                                if (definition_obj != NULL && json_object_get_type(definition_obj) == json_type_string)
                                {
                                    const char *definition = json_object_get_string(definition_obj);
                                    printf("Definition: %s\n", definition);
                                }
                                else
                                {
                                    printf("Definition field not found or invalid\n");
                                }
                            }
                        }
                        else
                        {
                            printf("Definitions field not found or invalid\n");
                        }
                    }
                }
            }
            else
            {
                printf("Meanings field not found or invalid\n");
            }
        }
    }
    else
    {
        printf("Root object is not an array\n");
    }

    json_object_put(root); // Release the JSON object
}

/*
The get_definition() function retrieves dictionary data from an external API 
based on the provided word, handling the HTTP request using libcurl 
and parsing the JSON response to extract definition details using the parse_response() function.
*/

void get_definition(const char *word) {
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    char url[256];
    snprintf(url, sizeof(url), BASE_URL, word);

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        else {
            // Parse JSON response
            parse_response(chunk.memory);
        }

        curl_easy_cleanup(curl);
        free(chunk.memory);
    }
    curl_global_cleanup();
}

/* The main() function gets the location information from the command line arguments.
 User is a word. This information is passed to the get_definition() function. If the command line argument is 
 incorrect, the user is prompted with the correct input format.
 */
int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Usage: %s <word>\n", argv[0]);
        return 1;
    }

    char *word = argv[1];
    get_definition(word);

    return 0;
}
