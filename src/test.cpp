#include <iostream>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "raygui.h"
#define RRES_IMPLEMENTATION
#include "rres.h" 
#define RRES_RAYLIB_IMPLEMENTATION
#define RRES_SUPPORT_COMPRESSION_LZ4
#define RRES_SUPPORT_ENCRYPTION_AES
#define RRES_SUPPORT_ENCRYPTION_XCHACHA20
#include "rres-raylib.h"     // Required to map rres data chunks into raylib structs


#include <conio.h>          // Windows only, no stardard library

#define KEY_ESCAPE  27

static unsigned char *LoadDataBuffer(rresResourceChunkData data, unsigned int rawSize);
static void UnloadDataBuffer(unsigned char *buffer);   // Unload data buffer

int main(void)
{

    FILE *rresFile = fopen("Gloomwood.rres", "wb");

    // Define rres file header
    // NOTE: We are loading 4 files that generate 5 resource chunks to save in rres
    
    
    rresFileHeader header = {
        .id{'r','r','e','s'},       // File identifier: rres
        .version = 100,         // File version: 100 for version 1.0
        .chunkCount = 1,        // Number of resource chunks in the file (MAX: 65535)
        .cdOffset = 0,          // Central Directory offset in file (0 if not available)
        .reserved = 0           // <reserved>
    };

    // Write rres file header
    fwrite(&header, sizeof(rresFileHeader), 1, rresFile);
    
    rresResourceChunkInfo chunkInfo = { 0 };    // Chunk info
    rresResourceChunkData chunkData = { 0 };    // Chunk data
    
    unsigned char *buffer = NULL;

    Wave musicAssets = LoadWave("../asset/Gloomwood.wav");
    unsigned int rawSize = musicAssets.frameCount*musicAssets.channels*(musicAssets.sampleSize/8);

    // Define chunk info: WAVE
    chunkInfo.type[0] = 'W';         // Resource chunk type (FourCC)
    chunkInfo.type[1] = 'A';         // Resource chunk type (FourCC)
    chunkInfo.type[2] = 'V';         // Resource chunk type (FourCC)
    chunkInfo.type[3] = 'E';         // Resource chunk type (FourCC)
   
    //Resource chunk identifier (generated from filename CRC32 hash)
    chunkInfo.id = rresComputeCRC32("../asset/Gloomwood.wav", strlen("../asset/Gloomwood.wav"));

    chunkInfo.compType = RRES_COMP_NONE,     // Data compression algorithm
    chunkInfo.cipherType = RRES_CIPHER_NONE, // Data encription algorithm
    chunkInfo.flags = 0,             // Data flags (if required)
    chunkInfo.baseSize = 5*sizeof(unsigned int) + rawSize;   // Data base size (uncompressed/unencrypted)
    chunkInfo.packedSize = chunkInfo.baseSize; // Data chunk size (compressed/encrypted + custom data appended)
    chunkInfo.nextOffset = 0,        // Next resource chunk global offset (if resource has multiple chunks)
    chunkInfo.reserved = 0,          // <reserved>

    // Define chunk data: WAVE
    chunkData.propCount = 4;
    chunkData.props = (unsigned int *)RRES_CALLOC(chunkData.propCount, sizeof(unsigned int));
    chunkData.props[0] = musicAssets.frameCount;      // props[0]:frameCount
    chunkData.props[1] = musicAssets.sampleRate;      // props[1]:sampleRate
    chunkData.props[2] = musicAssets.sampleSize;      // props[2]:sampleSize
    chunkData.props[3] = musicAssets.channels;        // props[3]:channels    
    chunkData.raw = musicAssets.data;

     // Get a continuous data buffer from chunkData
    buffer = LoadDataBuffer(chunkData, rawSize);
    
    // Compute data chunk CRC32 (propCount + props[] + data)
    //chunkInfo.crc32 = rresComputeCRC32(buffer, chunkInfo.packedSize);

    // Write resource chunk into rres file
    fwrite(&chunkInfo, sizeof(rresResourceChunkInfo), 1, rresFile);
    fwrite(buffer, 1, chunkInfo.packedSize, rresFile);
    
    // Free required memory
    memset(&chunkInfo, 0, sizeof(rresResourceChunkInfo));
    RRES_FREE(chunkData.props);
    UnloadDataBuffer(buffer);
    UnloadWave(musicAssets);

    fclose(rresFile);

    Sound sound = { 0 };            // Store RRES_DATA_WAVE loaded data -> LoadSoundFromWave()

    // Load content from rres file
    rresResourceChunk chunk = { 0 }; // Single resource chunk
    rresResourceMulti multi = { 0 }; // Multiple resource chunks

    static unsigned char key = 0;
    InitAudioDevice();
    
    Music music = LoadMusicStream("../asset/kano.mp3");
    PlayMusicStream(music);
    printf("\nPress ESC to stop...\n");
    while (key != KEY_ESCAPE)
        {
            key = 0;
            if (_kbhit()) key = _getch();
            

            UpdateMusicStream(music);
        }
    
    UnloadMusicStream(music);   // Unload music stream data


    InitWindow(400, 200, "template test");
    SetTargetFPS(60);

    bool showMessageBox = false;
    while (!WindowShouldClose())
    
    {

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
    
            if (GuiButton((Rectangle){ 24, 24, 120, 30 }, "#191#Show Message")) showMessageBox = true;

            if (showMessageBox)
            {
                int result = GuiMessageBox((Rectangle){ 85, 70, 250, 100 },
                    "#191#Message Box", "Hi! This is testTemplate!", "just test");

                if (result >= 0) showMessageBox = false;
            }

        EndDrawing();
    }
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

// Load a continuous data buffer from rresResourceChunkData struct
static unsigned char *LoadDataBuffer(rresResourceChunkData data, unsigned int rawSize)
{
    unsigned char *buffer = (unsigned char *)RRES_CALLOC((data.propCount + 1)*sizeof(unsigned int) + rawSize, 1);
    
    memcpy(buffer, &data.propCount, sizeof(unsigned int));
    for (int i = 0; i < data.propCount; i++) memcpy(buffer + (i + 1)*sizeof(unsigned int), &data.props[i], sizeof(unsigned int));
    memcpy(buffer + (data.propCount + 1)*sizeof(unsigned int), data.raw, rawSize);
    
    return buffer;
}

// Unload data buffer
static void UnloadDataBuffer(unsigned char *buffer)
{
    RRES_FREE(buffer);
}
