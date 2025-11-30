#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>


typedef struct	json {
	enum {
		MAP, // an object.
		INTEGER,
		STRING
	} type;
	union {
		struct {
			struct pair	*data;
			size_t		size;
		} map;
		int	integer;
		char	*string;
	};
}	json;

typedef struct	pair {
	char	*key;
	json	value;
}	pair;

void	free_json(json j);
int	argo(json *dst, FILE *stream);

int	peek(FILE *stream)
{
	int	c = getc(stream);
	ungetc(c, stream);
	return c;
}

void	unexpected(FILE *stream)
{
	if (peek(stream) != EOF)
		printf("unexpected token '%c'\n", peek(stream));
	else
		printf("unexpected end of input\n");
}

int	accept(FILE *stream, char c)
{
	if (peek(stream) == c)
	{
		(void)getc(stream);
		return 1;
	}
	return 0;
}

int	expect(FILE *stream, char c)
{
	if (accept(stream, c))
		return 1;
	unexpected(stream);
	return 0;
}

void	free_json(json j)
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
			break ;
		case STRING:
			free(j.string);
			break ;
		default:
			break ;
	}
}

void	serialize(json j)
{
	switch (j.type)
	{
		case INTEGER:
			printf("%d", j.integer);
			break ;
		case STRING:
			putchar('"');
			for (int i = 0; j.string[i]; i++)
			{
				if (j.string[i] == '\\' || j.string[i] == '"')
					putchar('\\');
				putchar(j.string[i]);
			}
			putchar('"');
			break ;
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
			break ;
	}
}

int parse_map(json *dst, FILE *stream)
{
	dst->type = MAP;
	dst->map.data = NULL;
	dst->map.size = 0;
	int c = peek(stream);
	if (c == EOF)
	{
		unexpected(stream);
		return (-1);
	}
	else if (c == )

}

int parse_int(json *)
{
    int c == peek(stream);
    if (c != '-' || !isdigit(c))
    {
        unexpected(stream);
        return (-1);
    }

    int n = 0;
    fscanf(stream, "%d", &n)
}

char *get_str(FILE *stream)
{
	char *result = calloc(4096, sizeof(char));
	if (!result)
		return(NULL);
	int i = 0;
	int c = fgetc(stream);

	while (1)
	{
		c = peek(stream);
		if (c == '"')
		{
			fgetc(stream);
			break;
		}
		if (c == EOF)
		{
			unexpected(stream);
			return (NULL);
		}
		if (c == '\\')
		{
			c = getc(stream);
			if (c == EOF)
			{
				unexpected(stream);
				return (NULL);
			}
		}
		result[i++] = c;
	}

	result[i] = '\0';
	return(result);
}

int parser(json *dst, FILE *stream)
{
    int c = peek(stream); // check the value without incrementing.
    if (c == EOF)
    {
        unexpected(stream);
        return (-1);
    }

    if (isdigit(c) || c == '-')
        return(parse_int(ds, stream));
    else if (c == '"') // check the NULL OR STRING.
    {
        dst->type = STRING;
        dst->string = get_str(stream); // implement get_str.
        if (ds->string == NULL)
            return (-1);
        return (1);
    }
    else if (c == '{') // { for the map. wdym map right man ?
        return (parse_map(dst, stream));
    else
    {
        unexpected(stream);
        return (-1);
    }
}

int	argo(json *dst, FILE *stream)
{
    skip_white_spaces(stream);

    if (parser(dst, stream) == -1)
        return (-1);
    
    skip_white_spaces(stream);

}

int	main(int argc, char **argv)
{
	if (argc != 2)
		return 1;
	char *filename = argv[1];
	FILE *stream = fopen(filename, "r");
	json	file;
	if (argo (&file, stream) != 1)
	{
		free_json(file);
		return 1;
	}
	serialize(file);
	printf("\n");
}
