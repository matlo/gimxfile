/*
 Copyright (c) 2019 Mathieu Laurendeau <mat.lau@laposte.net>
 License: GPLv3
 */

#include <gfile.h>
#include <gimxlog/include/glog.h>
#include <gimxcommon/include/gerror.h>
#include <string.h>

#ifndef WIN32
#include <pwd.h> //to get the homedir + uid and gid
#include <sys/types.h> //to get the homedir
#include <unistd.h> //to get the homedir + chown
#else
#undef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_VERSION_FROM_WIN32_WINNT(NTDDI_VISTA)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h> //to get the homedir
#include <knownfolders.h>
#include <objbase.h>
#endif

GLOG_INST(GLOG_NAME)

#ifdef WIN32
wchar_t * gfile_utf8_to_utf16le(const char * inbuf)
{
  wchar_t * outbuf = NULL;
  int outsize = MultiByteToWideChar(CP_UTF8, 0, inbuf, -1, NULL, 0);
  if (outsize != 0) {
      outbuf = (wchar_t*) malloc(outsize * sizeof(*outbuf));
      if (outbuf != NULL) {
         int res = MultiByteToWideChar(CP_UTF8, 0, inbuf, -1, outbuf, outsize);
         if (res == 0) {
             free(outbuf);
             outbuf = NULL;
         }
      }
  }

  return outbuf;
}

char * gfile_utf16le_to_utf8(const wchar_t * inbuf)
{
  char * outbuf = NULL;
  int outsize = WideCharToMultiByte(CP_UTF8, 0, inbuf, -1, NULL, 0, NULL, NULL);
  if (outsize != 0) {
      outbuf = (char*) malloc(outsize * sizeof(*outbuf));
      if (outbuf != NULL) {
         int res = WideCharToMultiByte(CP_UTF8, 0, inbuf, -1, outbuf, outsize, NULL, NULL);
         if (res == 0) {
             free(outbuf);
             outbuf = NULL;
         }
      }
  }

  return outbuf;
}
#endif

#ifdef __linux__
int gfile_makeown(const char *path) {
    int ret = chown(path, getpwuid(getuid())->pw_uid, getpwuid(getuid())->pw_gid);
    if (ret < 0) {
        PRINT_ERROR_ERRNO("chown");
    }
    return ret;
}
#endif

#ifndef WIN32

FILE * gfile_fopen(const char *path, const char *mode) {
    return fopen(path, mode);
}

GFILE_DIR * gfile_opendir (const char * path) {
    return opendir(path);
}

int gfile_closedir(GFILE_DIR *dirp) {
    return closedir(dirp);
}

GFILE_DIRENT * gfile_readdir(GFILE_DIR *dirp) {
    return readdir(dirp);
}

int gfile_stat(const char *path, GFILE_STAT *buf) {
    return stat(path, buf);
}

int gfile_remove(const char *path) {
    return remove(path);
}

char * gfile_tempdir() {
    return strdup("/tmp");
}

char * gfile_name(GFILE_DIRENT *dirent) {
    return strdup(dirent->d_name);
}

char * gfile_homedir() {
    return strdup(getpwuid(getuid())->pw_dir);
}

#else

FILE * gfile_fopen(const char *path, const char *mode) {
    wchar_t * wpath = gfile_utf8_to_utf16le(path);
    wchar_t * wmode = gfile_utf8_to_utf16le(mode);
    FILE* file = _wfopen(wpath, wmode);
    free(wmode);
    free(wpath);
    return file;
}

GFILE_DIR * gfile_opendir (const char * path) {
    wchar_t * wpath = gfile_utf8_to_utf16le(path);
    GFILE_DIR * dir = _wopendir(wpath);
    free(wpath);
    return dir;
}

int gfile_closedir(GFILE_DIR *dirp) {
    return _wclosedir(dirp);
}

GFILE_DIRENT * gfile_readdir(GFILE_DIR *dirp) {
    return _wreaddir(dirp);
}

int gfile_stat(const char *path, GFILE_STAT *buf) {
    wchar_t * wpath = gfile_utf8_to_utf16le(path);
    int ret = _wstat(wpath, buf);
    free(wpath);
    return ret;
}

int gfile_remove(const char *path) {
    wchar_t * wpath = gfile_utf8_to_utf16le(path);
    int result = _wremove(wpath);
    free(wpath);
    return result;
}

char * gfile_tempdir() {
    wchar_t temp[MAX_PATH + 1];
    if (!GetTempPathW(MAX_PATH, temp)) {
        return strdup("");
    }
    return gfile_utf16le_to_utf8(temp);
}

char * gfile_name(GFILE_DIRENT *dirent) {
    return gfile_utf16le_to_utf8(dirent->d_name);
}

char * gfile_homedir() {
    wchar_t * path = NULL;
    if(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &path))
    {
      PRINT_ERROR_OTHER("SHGetKnownFolderPath failed");
      return NULL;
    }
    char * homedir = gfile_utf16le_to_utf8(path);
    CoTaskMemFree(path);
    return homedir;
}
#endif

int gfile_isfile(const char *dir, const char *name) {
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", dir, name);
    GFILE_STAT buf;
    if(gfile_stat(path, &buf) == 0) {
        return S_ISREG(buf.st_mode);
    }
    return 0;
}
