#ifndef AUDIO_H
#define AUDIO_H

extern "C" {
    int __decodeFile(const char *, const int, double **, int *);
}

namespace Audio
{
    inline int decodeFile(const char *filename, int sample_rate, double **data)
    {
        int size;
        if (__decodeFile(filename, sample_rate, data, &size) < 0)
            return -1;
        return size;
    }
}

#endif // AUDIO_H
