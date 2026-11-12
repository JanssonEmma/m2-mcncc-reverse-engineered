#include <cstdio>

// 081b6140
void WriteVersion()
{
    std::FILE* file = std::fopen("VERSION.txt", "w");
    std::fprintf(file, "game svn revision %s\n", "404");
    std::fprintf(file, "\tplayer count: %d\n", 4);
    std::fprintf(file, "\twith new ai\n");
    std::fclose(file);
}
