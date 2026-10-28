#ifndef TEXTURE_H__
#define TEXTURE_H__

typedef struct Texture {
	char *buff;
	size_t chunks;
	size_t width;

} Texture;

// Function prototypes
//----------------------------------------------------------------------------------
void     Texture_init     (Texture *texture);
void     Texture_mem_free (Texture *texture);
Texture* Texture_load     (Texture *texture, const char *filename);
//----------------------------------------------------------------------------------
// End of function prototypes

#ifdef TEXTURE_IMPLEMENTATION

    // =============================================================================
    // @@@ + Texture_init
    // =============================================================================
    void Texture_init(Texture *texture)
    {
    	texture->buff   = NULL;
    	texture->chunks = 0;
    	texture->width  = 0;
    }

    // =============================================================================
    // @@@ + Texture_mem_free
    // =============================================================================
    void Texture_mem_free(Texture *texture)
    {
    	if (texture->buff)
    		free(texture->buff);

    	texture->buff = NULL;
    }

	// =============================================================================
	// @@@ + Texture_load
	// =============================================================================
	Texture* Texture_load(Texture *texture, const char *filename)
	{
		FILE *file = fopen(filename, "r");

		if (!file) return NULL;

		char str[1024];
		
		for (char *p = NULL;;)
		{
			fgets(str, 1024, file);

			if (feof(file))
				break;

			if (texture->buff == NULL)
			{
				texture->width  = strlen(str) - 1; // -1 is the new line character
				texture->chunks = texture->width / SIZE;
				texture->buff   = malloc(sizeof(char) * texture->width * SIZE);

				p = texture->buff;
			}

			memcpy(p, str, texture->width);
			p += texture->width;

			memcpy(p, str, texture->width);
			p += texture->width;
		}

		return texture;
	}

#endif
#endif
