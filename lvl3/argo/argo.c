#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef struct json {
    enum {
        MAP,
        INTEGER,
        STRING
    } type;
    union {
        struct {
            struct pair *data;
            size_t size;
        } map;
        int integer;
        char *string;
    };
} json;

typedef struct pair {
    char *key;
    json value;
} pair;

void free_json(json j);
int argo(json *dst, FILE *stream);

int peek(FILE *stream)
{
    int c = getc(stream);
    ungetc(c, stream);
    return c;
}

void unexpected(FILE *stream)
{
    if (peek(stream) != EOF)
        printf("Unexpected token '%c'\n", peek(stream));
    else
        printf("Unexpected end of input\n");
}

int accept(FILE *stream, char c)
{
    if (peek(stream) == c)
    {
        (void)getc(stream);
        return 1;
    }
    return 0;
}

int expect(FILE *stream, char c)
{
    if (accept(stream, c))
        return 1;
    unexpected(stream);
    return 0;
}

// ✅ ADDED: Skip whitespace function
void skip_whitespace(FILE *stream)
{
    int c;
    while ((c = peek(stream)) == ' ' || c == '\t' || c == '\n' || c == '\r')
        getc(stream);
}

void free_json(json j)
{
    switch (j.type)
    {
        case MAP:
            for (size_t i = 0; i < j.map.size; i++)
            {
                free(j.map.data[i].key);
                free_json(j.map.data[i].value);
            }
            free(j.map.data);
            break;
        case STRING:
            free(j.string);
            break;
        default:
            break;
    }
}

void serialize(json j)
{
    switch (j.type)
    {
        case INTEGER:
            printf("%d", j.integer);
            break;
        case STRING:
            putchar('"');
            for (int i = 0; j.string[i]; i++)
            {
                if (j.string[i] == '\\' || j.string[i] == '"')
                    putchar('\\');
                putchar(j.string[i]);
            }
            putchar('"');
            break;
        case MAP:
            putchar('{');
            for (size_t i = 0; i < j.map.size; i++)
            {
                if (i != 0)
                    putchar(',');
                serialize((json){.type = STRING, .string = j.map.data[i].key});
                putchar(':');
                serialize(j.map.data[i].value);
            }
            putchar('}');
            break;
    }
}

int parse_int(json *dst, FILE *stream)
{
    int c = peek(stream);
    
    // Check if it's a valid start of integer
    if (!isdigit(c) && c != '-')
    {
        unexpected(stream);
        return -1;
    }
    
    int n = 0;
    fscanf(stream, "%d", &n);
    dst->type = INTEGER;
    dst->integer = n;
    return 1;
}

char *get_str(FILE *stream)
{
    size_t capacity = 64;
    size_t length = 0;
    char *res = malloc(capacity);
    
    if (!res)
        return NULL;
    
    // Consume opening quote
    if (!expect(stream, '"'))
    {
        free(res);
        return NULL;
    }
    
    while (1)
    {
        int c = peek(stream);
        
        if (c == '"')
        {
            getc(stream);  // Consume closing quote
            break;
        }
        
        if (c == EOF)
        {
            free(res);
            unexpected(stream);
            return NULL;
        }
        
        c = getc(stream);
        
        // Handle escape sequences
        if (c == '\\')
        {
            c = getc(stream);
            if (c == EOF)
            {
                free(res);
                unexpected(stream);
                return NULL;
            }
        }
        
        // Resize if needed
        if (length + 1 >= capacity)
        {
            capacity *= 2;
            char *new_res = realloc(res, capacity);
            if (!new_res)
            {
                free(res);
                return NULL;
            }
            res = new_res;
        }
        
        res[length++] = c;
    }
    
    res[length] = '\0';
    return res;
}


int parse_map(json *dst, FILE *stream)
{
    dst->type = MAP;
    dst->map.size = 0;
    dst->map.data = NULL;
    
    if (!expect(stream, '{'))
        return -1;
    
    skip_whitespace(stream);  // Skip after '{'
    
    // Handle empty map
    if (accept(stream, '}'))
        return 1;
    
    while (1)
    {
        skip_whitespace(stream);  //  Skip before key
        
        int c = peek(stream);
        if (c != '"')
        {
            unexpected(stream);
            return -1;
        }
        
        // Grow array
        pair *new_data = realloc(dst->map.data, (dst->map.size + 1) * sizeof(pair));
        if (!new_data)
            return -1;
        
        dst->map.data = new_data;
        pair *current = &dst->map.data[dst->map.size];
        
        // Parse key
        current->key = get_str(stream);
        if (current->key == NULL)
            return -1;
        
        dst->map.size++;
        
        skip_whitespace(stream);  // Skip before ':'
        
        if (!expect(stream, ':'))
            return -1;
        
        skip_whitespace(stream); 
        
        // Parse value (recursive)
        if (argo(&current->value, stream) == -1)
            return -1;
        
        skip_whitespace(stream); 
        
        c = peek(stream);
        
        if (c == '}')
        {
            getc(stream);  // Consume '}'
            break;
        }
        
        if (c == ',')
        {
            getc(stream);  // Consume ','
            continue;
        }
        
        unexpected(stream);
        return -1;
    }
    
    return 1;
}

int parser(json *dst, FILE *stream)
{
    int c = peek(stream);
    
    if (c == EOF)
    {
        unexpected(stream);
        return -1;
    }
    
    if (isdigit(c) || c == '-')
        return parse_int(dst, stream);
    else if (c == '"')
    {
        dst->type = STRING;
        dst->string = get_str(stream);
        if (dst->string == NULL)
            return -1;
        return 1;
    }
    else if (c == '{')
        return parse_map(dst, stream);
    else
    {
        unexpected(stream);
        return -1;
    }
}

int argo(json *dst, FILE *stream)
{
    skip_whitespace(stream);
    
    if (parser(dst, stream) == -1)
        return -1;
    
    skip_whitespace(stream);

    return 1;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 1;
    
    char *filename = argv[1];
    FILE *stream = fopen(filename, "r");
    
    if (!stream)
        return 1;
    
    json file;
    
    if (argo(&file, stream) != 1)
    {
        free_json(file);
        fclose(stream);
        return 1;
    }
    
    serialize(file);
    printf("\n");
    
    free_json(file);
    fclose(stream);
    return 0;
}