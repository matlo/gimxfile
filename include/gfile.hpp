/*
 Copyright (c) 2019 Mathieu Laurendeau <mat.lau@laposte.net>
 License: GPLv3
 */

#ifndef GFILE_HPP_
#define GFILE_HPP_

#include <ext/stdio_filebuf.h>
#include <fcntl.h>
#include <gimxfile/include/gfile.h>

/*
 * File abstraction layer.
 */

#ifdef WIN32

#define OFBUF(path) \
    wchar_t * wpath = gfile_utf8_to_utf16le(path.c_str()); \
    __gnu_cxx::stdio_filebuf<char> fb(_wopen(wpath, _O_BINARY | _O_WRONLY | _O_TRUNC | _O_CREAT, 0666), std::ios::out | std::ios::binary); \
    free(wpath);

#define IFBUF(path) \
    wchar_t * wpath = gfile_utf8_to_utf16le(path.c_str()); \
    __gnu_cxx::stdio_filebuf<char> fb(_wopen(wpath, _O_BINARY | _O_RDONLY), std::ios::in | std::ios::binary); \
    free(wpath);

#else

#define OFBUF(path) \
    __gnu_cxx::stdio_filebuf<char> fb(open(path.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0666), std::ios::out);

#define IFBUF(path) \
    __gnu_cxx::stdio_filebuf<char> fb(open(path.c_str(), O_RDONLY), std::ios::in);

#endif

#define IFSTREAM(path, name) \
    IFBUF(path) \
    std::istream name (&fb);

#define OFSTREAM(path, name) \
    OFBUF(path) \
    std::ostream name (&fb);

#endif /* GFILE_HPP_ */
