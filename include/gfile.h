/*
 Copyright (c) 2019 Mathieu Laurendeau <mat.lau@laposte.net>
 License: GPLv3
 */

#ifndef GFILE_H_
#define GFILE_H_

#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define LINE_MAX 1024
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * File abstraction layer.
 */

#ifdef WIN32
wchar_t * gfile_utf8_to_utf16le(const char * inbuf);
char * gfile_utf16le_to_utf8(const wchar_t * inbuf);
#endif

#ifdef __linux__
int gfile_makeown(const char *path);
#endif

FILE * gfile_fopen(const char *path, const char *mode);

#ifdef WIN32
typedef _WDIR GFILE_DIR;
typedef struct _wdirent GFILE_DIRENT;
typedef struct _stat GFILE_STAT;
#else
typedef DIR GFILE_DIR;
typedef struct dirent GFILE_DIRENT;
typedef struct stat GFILE_STAT;
#endif

GFILE_DIR * gfile_opendir (const char *dirname);

int gfile_closedir(GFILE_DIR *dirp);

GFILE_DIRENT *gfile_readdir(GFILE_DIR *dirp);

int gfile_remove(const char *path);

char * gfile_tempdir();

char * gfile_name(GFILE_DIRENT *dirent);

int gfile_stat(const char *path, GFILE_STAT *buf);

int gfile_isfile(const char *dir, const char *name);

char * gfile_homedir();

#ifdef __cplusplus
}
#endif

#endif /* GFILE_H_ */
