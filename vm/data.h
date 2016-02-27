#ifndef __DATA_H__
#define __DATA_H__

#ifdef WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif
#define DATA_PATH "data" PATH_SEP

#define SHADER(name) DATA_PATH "shaders" PATH_SEP name
#define TEXTURE(name) DATA_PATH "textures" PATH_SEP name
#define FONT(name) DATA_PATH "fonts" PATH_SEP name

#endif
