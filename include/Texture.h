#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
namespace VCTII
{
	struct Texture2D {
		GLuint textureID;
		int texWidth;
		int texHeight;
		int colorChannels;
	};

	struct Texture3D {
		GLuint textureID;
		int tex3DSize;
	};
}
#endif // TEXTURE_H