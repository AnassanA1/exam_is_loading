#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef struct	json {
	enum {
		MAP,
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
	pair *data;
	size_t size;
	json key;

	if (!expect(stream, '{'))
		return (-1);
	data = NULL;
	size = 0;
	while (!accept(stream, '}'))
	{
		data = realloc(data, sizeof(pair) * (size + 1));
		if (!parse_string(&key, stream))
		{
			free(data);
			return (-1);
		}
		if (!expect(stream, ':'))
		{
			free(key.string);
			free(data);
			return (-1);
		}
		if (parser(&data[size].value, stream) == -1)
		{
			free(key.string);
			free(data);
			return (-1);
		}

	}
}

int parse_integer(json *dst, FILE *stream)
{
	int c = peek(stream);

	if (isdigit(c) || c == '-')
	{
		dst->type = INTEGER;
		dst->integer = c;
		return (1);
	}
	unexpected(stream);
	return(-1);
}

int parse_string(json *dst, FILE *stream)
{
	char buf[4096];
	int i;
	char c;

	if (!accept(stream, '"'))
		return (-1);
	i = 0;
	while (1)
	{
		c = getc(stream);
		if (c == EOF)
		{
			unexpected(stream);
			return (-1);
		}
		if (c == '"')
			break;
		if (c == '\\')
		{
			c = getc(stream);
			if (c == EOF)
			{
				unexpected(stream);
				return (-1);
			}
		}
		buf[i++] = c;
	}
	buf[i] = '\0';
	dst->string = strdup(buf);
	dst->type = STRING;
	return (1);
}

int parser(json *dst, FILE *stream)
{
	int c;

	c = peek(stream);
	if (c == '"')
		return (parse_string(dst, stream));
	else if (isdigit(c) || c == '-')
		return (parse_integer(dst, stream));
	else if (c == '{')
		return (parse_map(dst, stream));
	else
	{
		unexpected(stream);
		return (-1);
	}
}

int argo(json *dst, FILE *stream)
{
	return (parser(dst, stream));
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
