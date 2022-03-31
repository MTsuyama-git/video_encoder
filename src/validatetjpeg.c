#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <ctime>

extern "C" {
#include <SDL.h>
#include <jpeglib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>

}

#define TRAW_FMT_ID 0xDEAD
#define TJPG_FMT_ID 0xC0DE

int step_traw_frame(FILE *f, int size, void* data, int width, int height);
int step_jpeg_frame(uint8_t*, SDL_Renderer*, SDL_Texture*, SDL_Rect*, FILE*, int, int);

int main(int argc, char** argv) {
    if(argc < 3) {
        fprintf(stderr, "Usage %s cmd input\n", argv[0]);
        return EXIT_FAILURE;
    }
    SDL_Window *window = NULL;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Rect sdlRect;
    int pixformat;
    static bool initialized;

    int ret;
    size_t i;
    char* cmd = argv[1];
    FILE* f;
    float mspf, fps; // ms per frame, fps
    uint16_t fmt, ver, width, height; // format version width height
    uint32_t nof, frame_size; // number of rames, size of frame
    uint8_t* bmp_buffer;
    uint8_t* buffer_array[1];

    f = fopen(argv[2], "rb");
    if(f == NULL) {
        /* std::cerr << "Could not open " << argv[1] << std::endl; */
        return EXIT_FAILURE;
    }

    // read format
    fread(&fmt, sizeof(uint16_t), 1, f);
    // read version
    fread(&ver, sizeof(uint16_t), 1, f);
    // read width
    fread(&width, sizeof(uint16_t), 1, f);
    // read height
    fread(&height, sizeof(uint16_t), 1, f);
    // read # of frame
    fread(&nof, sizeof(uint32_t), 1, f);
    // read ms per frame
    fread(&mspf, sizeof(float), 1, f);

    fprintf(stdout, "# of frame :%d\n", nof);
    fprintf(stdout, "mili second per frame :%.2f\n", mspf);

    float framerate = 1000 / mspf;
    fprintf(stdout, "frame rate: %.3f fps\n", framerate);

    if(fmt == TRAW_FMT_ID) {
        fprintf(stdout, "traw format\r\n");
        frame_size = 480 * 320 * 4;
        bmp_buffer = NULL;
        pixformat=SDL_PIXELFORMAT_BGRA32;
    }
    else if(fmt == TJPG_FMT_ID){
        pixformat=SDL_PIXELFORMAT_RGB24;
        fprintf(stdout, "tjpg format\r\n");
        bmp_buffer = (uint8_t*)malloc(480 * 320 * 3);
    }

    fprintf(stdout, "VERSION    :0x%04X\n", ver);
    fprintf(stdout, "WIDTH      :%d\n", width);
    fprintf(stdout, "HEIGHT     :%d\n", height);

    if(strcmp(argv[1], "validate") == 0) {
        goto end;
    }
    window = SDL_CreateWindow("Preview", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    texture = SDL_CreateTexture(renderer, pixformat, 0, width, height);
    sdlRect.w = width;
    sdlRect.h = height;
    ret = 0;
    i = 0;
    while(ret == 0) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                break;
            }
            else if(e.type == SDL_KEYDOWN) {
                if(e.key.keysym.sym == SDLK_q) {
                    break;
                }
            }
        }
        if( i < nof) {
            if(fmt == TRAW_FMT_ID) {
                /* ret = step_traw_frame(f, width * height * 4); */
            }
            else {
                ret = step_jpeg_frame(bmp_buffer, renderer, texture, &sdlRect, f, width, height);
            }
            ++i;
        }
    }
    if(ret != 0) {
        fprintf(stderr, "Error occured at frame #%lu\n", i);
    }

    usleep(1000);
    SDL_DestroyWindow(window);
    if(bmp_buffer != NULL) {
        free(bmp_buffer);
        bmp_buffer = NULL;
    }
    SDL_Quit();
end:
    fclose(f);

    return EXIT_SUCCESS;
}



int step_traw_frame(FILE *f, int size, void* data, int width, int height) {
    size_t cur, end;
    cur = ftell(f);
    fseek(f, 0, SEEK_END);
    end = ftell(f);
    fseek(f, cur, SEEK_SET);
    if(end - cur < size) {
        return -1;
    }
    fseek(f, (cur + size), SEEK_SET);
    /* SDL_UpdateTexture(texture, &sdlRect, data, width*4); //BGRA32 */
    return 0;
}

int step_jpeg_frame(uint8_t* bmp_buffer, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect* sdlRect, FILE* f, int width, int height) {
    uint8_t* buffer;
    uint32_t data_size;
    size_t cur, end;
    FILE* g;
    uint8_t* buffer_array[1];
    static struct jpeg_decompress_struct cinfo;
    static struct jpeg_error_mgr jerr;
    cur = ftell(f);
    fseek(f, 0, SEEK_END);
    end = ftell(f);
    fseek(f, cur, SEEK_SET);
    fread(&data_size, sizeof(uint32_t), 1, f);
    buffer = (uint8_t*)malloc(sizeof(uint8_t)*data_size);
    fread(buffer, sizeof(uint8_t), data_size, f);
    g = fmemopen(buffer, sizeof(uint8_t)*data_size, "rb");
    jpeg_create_decompress(&cinfo);
    cinfo.err = jpeg_std_error(&jerr);
    
    jpeg_stdio_src(&cinfo, g);
    jpeg_read_header(&cinfo, TRUE);
    int __width = cinfo.image_width;
    int __height = cinfo.image_height;
    int __ch = cinfo.num_components;
    printf("%dx%d\n", __width, __height);
    jpeg_start_decompress(&cinfo);
    while (cinfo.output_scanline < cinfo.output_height) {
        buffer_array[0] = bmp_buffer + (cinfo.output_scanline) * __width * __ch;
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
    }
    memset(sdlRect, 0, sizeof(SDL_Rect));
    sdlRect->w = __width;
    sdlRect->h = __height;
    SDL_UpdateTexture(texture, sdlRect, bmp_buffer, width*3); //RGB24
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, sdlRect);
    SDL_RenderPresent(renderer);
    
    fclose(g);
    free(buffer);
    return 0;
}

