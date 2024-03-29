#include "pch.h"
#include "texture.h"

FIBITMAP* BitmapFromFile(const char* file_name, int& width, int& height) {
	// image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	// pointer to the image, once loaded
	FIBITMAP* dib = nullptr;
	// pointer to the image data
	//BYTE * bits = nullptr;

	// check the file signature and deduce its format
	fif = FreeImage_GetFileType(file_name, 0);
	// if still unknown, try to guess the file format from the file extension
	if (fif == FIF_UNKNOWN) {
		fif = FreeImage_GetFIFFromFilename(file_name);
	}
	// if known
	if (fif != FIF_UNKNOWN) {
		// check that the plugin has reading capabilities and load the file
		if (FreeImage_FIFSupportsReading(fif)) {
			dib = FreeImage_Load(fif, file_name);
		}
		// if the image loaded
		if (dib) {
			// get the image width and height
			width = int(FreeImage_GetWidth(dib));
			height = int(FreeImage_GetHeight(dib));

			// if each of these is ok
			if ((width == 0) || (height == 0)) {
				FreeImage_Unload(dib);
				dib = nullptr;
			}
		}
	}

	return dib;
}

FIBITMAP* Custom_FreeImage_ConvertToRGBF(FIBITMAP* dib) {
	FIBITMAP* src = NULL;
	FIBITMAP* dst = NULL;

	if (!FreeImage_HasPixels(dib)) return NULL;

	const FREE_IMAGE_TYPE src_type = FreeImage_GetImageType(dib);

	// check for allowed conversions 
	switch (src_type) {
		case FIT_BITMAP:
		{
			// allow conversion from 24- and 32-bit
			const FREE_IMAGE_COLOR_TYPE color_type = FreeImage_GetColorType(dib);
			if ((color_type != FIC_RGB) && (color_type != FIC_RGBALPHA)) {
				src = FreeImage_ConvertTo24Bits(dib);
				if (!src) return NULL;
			}
			else {
				src = dib;
			}
			break;
		}
		case FIT_UINT16:
			// allow conversion from 16-bit
			src = dib;
			break;
		case FIT_RGB16:
			// allow conversion from 48-bit RGB
			src = dib;
			break;
		case FIT_RGBA16:
			// allow conversion from 64-bit RGBA (ignore the alpha channel)
			src = dib;
			break;
		case FIT_FLOAT:
			// allow conversion from 32-bit float
			src = dib;
			break;
		case FIT_RGBAF:
			// allow conversion from 128-bit RGBAF
			src = dib;
			break;
		case FIT_RGBF:
			// RGBF type : clone the src
			return FreeImage_Clone(dib);
			break;
		default:
			return NULL;
	}

	// allocate dst image

	const unsigned width = FreeImage_GetWidth(src);
	const unsigned height = FreeImage_GetHeight(src);

	dst = FreeImage_AllocateT(FIT_RGBF, width, height);
	if (!dst) {
		if (src != dib) {
			FreeImage_Unload(src);
		}
		return NULL;
	}

	// copy metadata from src to dst
	FreeImage_CloneMetadata(dst, src);

	// convert from src type to RGBF

	const unsigned src_pitch = FreeImage_GetPitch(src);
	const unsigned dst_pitch = FreeImage_GetPitch(dst);

	switch (src_type) {
		case FIT_BITMAP:
		{
			// calculate the number of bytes per pixel (3 for 24-bit or 4 for 32-bit)
			const unsigned bytespp = FreeImage_GetLine(src) / FreeImage_GetWidth(src);

			const BYTE* src_bits = (BYTE*)FreeImage_GetBits(src);
			BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst);

			for (unsigned y = 0; y < height; y++) {
				const BYTE* src_pixel = (BYTE*)src_bits;
				FIRGBF* dst_pixel = (FIRGBF*)dst_bits;
				for (unsigned x = 0; x < width; x++) {
					// convert and scale to the range [0..1]
					dst_pixel->red = (float)(src_pixel[FI_RGBA_RED]) / 255.0F;
					dst_pixel->green = (float)(src_pixel[FI_RGBA_GREEN]) / 255.0F;
					dst_pixel->blue = (float)(src_pixel[FI_RGBA_BLUE]) / 255.0F;

					src_pixel += bytespp;
					dst_pixel++;
				}
				src_bits += src_pitch;
				dst_bits += dst_pitch;
			}
		}
		break;

		case FIT_UINT16:
		{
			const BYTE* src_bits = (BYTE*)FreeImage_GetBits(src);
			BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst);

			for (unsigned y = 0; y < height; y++) {
				const WORD* src_pixel = (WORD*)src_bits;
				FIRGBF* dst_pixel = (FIRGBF*)dst_bits;

				for (unsigned x = 0; x < width; x++) {
					// convert and scale to the range [0..1]
					const float dst_value = (float)src_pixel[x] / 65535.0F;
					dst_pixel[x].red = dst_value;
					dst_pixel[x].green = dst_value;
					dst_pixel[x].blue = dst_value;
				}
				src_bits += src_pitch;
				dst_bits += dst_pitch;
			}
		}
		break;

		case FIT_RGB16:
		{
			const BYTE* src_bits = (BYTE*)FreeImage_GetBits(src);
			BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst);

			for (unsigned y = 0; y < height; y++) {
				const FIRGB16* src_pixel = (FIRGB16*)src_bits;
				FIRGBF* dst_pixel = (FIRGBF*)dst_bits;

				for (unsigned x = 0; x < width; x++) {
					// convert and scale to the range [0..1]
					dst_pixel[x].red = (float)(src_pixel[x].red) / 65535.0F;
					dst_pixel[x].green = (float)(src_pixel[x].green) / 65535.0F;
					dst_pixel[x].blue = (float)(src_pixel[x].blue) / 65535.0F;
				}
				src_bits += src_pitch;
				dst_bits += dst_pitch;
			}
		}
		break;

		case FIT_RGBA16:
		{
			const BYTE* src_bits = (BYTE*)FreeImage_GetBits(src);
			BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst);

			for (unsigned y = 0; y < height; y++) {
				const FIRGBA16* src_pixel = (FIRGBA16*)src_bits;
				FIRGBF* dst_pixel = (FIRGBF*)dst_bits;

				for (unsigned x = 0; x < width; x++) {
					// convert and scale to the range [0..1]
					dst_pixel[x].red = (float)(src_pixel[x].red) / 65535.0F;
					dst_pixel[x].green = (float)(src_pixel[x].green) / 65535.0F;
					dst_pixel[x].blue = (float)(src_pixel[x].blue) / 65535.0F;
				}
				src_bits += src_pitch;
				dst_bits += dst_pitch;
			}
		}
		break;

		case FIT_FLOAT:
		{
			const BYTE* src_bits = (BYTE*)FreeImage_GetBits(src);
			BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst);

			for (unsigned y = 0; y < height; y++) {
				const float* src_pixel = (float*)src_bits;
				FIRGBF* dst_pixel = (FIRGBF*)dst_bits;

				for (unsigned x = 0; x < width; x++) {
					// convert by copying greyscale channel to each R, G, B channels
					// NOT assume float values are in [0..1] !!!
					const float value = src_pixel[x];
					dst_pixel[x].red = value;
					dst_pixel[x].green = value;
					dst_pixel[x].blue = value;
				}
				src_bits += src_pitch;
				dst_bits += dst_pitch;
			}
		}
		break;

		case FIT_RGBAF:
		{
			const BYTE* src_bits = (BYTE*)FreeImage_GetBits(src);
			BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst);

			for (unsigned y = 0; y < height; y++) {
				const FIRGBAF* src_pixel = (FIRGBAF*)src_bits;
				FIRGBF* dst_pixel = (FIRGBF*)dst_bits;

				for (unsigned x = 0; x < width; x++) {
					// convert and skip alpha channel
					dst_pixel[x].red = src_pixel[x].red;
					dst_pixel[x].green = src_pixel[x].green;
					dst_pixel[x].blue = src_pixel[x].blue;
				}
				src_bits += src_pitch;
				dst_bits += dst_pitch;
			}
		}
		break;
	}

	if (src != dib) {
		FreeImage_Unload(src);
	}

	return dst;
}

FIBITMAP* Custom_FreeImage_ConvertToRGBAF(FIBITMAP* dib) {
	FIBITMAP* src = NULL;
	FIBITMAP* dst = NULL;

	if (!FreeImage_HasPixels(dib)) return NULL;

	const FREE_IMAGE_TYPE src_type = FreeImage_GetImageType(dib);

	// check for allowed conversions 
	switch (src_type) {
		case FIT_BITMAP:
		{
			// allow conversion from 32-bit
			const FREE_IMAGE_COLOR_TYPE color_type = FreeImage_GetColorType(dib);
			if (color_type != FIC_RGBALPHA) {
				src = FreeImage_ConvertTo32Bits(dib);
				if (!src) return NULL;
			}
			else {
				src = dib;
			}
			break;
		}
		case FIT_UINT16:
			// allow conversion from 16-bit
			src = dib;
			break;
		case FIT_RGB16:
			// allow conversion from 48-bit RGB
			src = dib;
			break;
		case FIT_RGBA16:
			// allow conversion from 64-bit RGBA
			src = dib;
			break;
		case FIT_FLOAT:
			// allow conversion from 32-bit float
			src = dib;
			break;
		case FIT_RGBF:
			// allow conversion from 96-bit RGBF
			src = dib;
			break;
		case FIT_RGBAF:
			// RGBAF type : clone the src
			return FreeImage_Clone(dib);
			break;
		default:
			return NULL;
	}

	// allocate dst image

	const unsigned width = FreeImage_GetWidth(src);
	const unsigned height = FreeImage_GetHeight(src);

	dst = FreeImage_AllocateT(FIT_RGBAF, width, height);
	if (!dst) {
		if (src != dib) {
			FreeImage_Unload(src);
		}
		return NULL;
	}

	// copy metadata from src to dst
	FreeImage_CloneMetadata(dst, src);

	// convert from src type to RGBAF

	const unsigned src_pitch = FreeImage_GetPitch(src);
	const unsigned dst_pitch = FreeImage_GetPitch(dst);

	switch (src_type) {
		case FIT_BITMAP:
		{
			// calculate the number of bytes per pixel (4 for 32-bit)
			const unsigned bytespp = FreeImage_GetLine(src) / FreeImage_GetWidth(src);

			const BYTE* src_bits = (BYTE*)FreeImage_GetBits(src);
			BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst);

			for (unsigned y = 0; y < height; y++) {
				const BYTE* src_pixel = (BYTE*)src_bits;
				FIRGBAF* dst_pixel = (FIRGBAF*)dst_bits;
				for (unsigned x = 0; x < width; x++) {
					// convert and scale to the range [0..1]
					dst_pixel->red = (float)(src_pixel[FI_RGBA_RED]) / 255.0F;
					dst_pixel->green = (float)(src_pixel[FI_RGBA_GREEN]) / 255.0F;
					dst_pixel->blue = (float)(src_pixel[FI_RGBA_BLUE]) / 255.0F;
					dst_pixel->alpha = (float)(src_pixel[FI_RGBA_ALPHA]) / 255.0F;

					src_pixel += bytespp;
					dst_pixel++;
				}
				src_bits += src_pitch;
				dst_bits += dst_pitch;
			}
		}
		break;

		case FIT_UINT16:
		{
			const BYTE* src_bits = (BYTE*)FreeImage_GetBits(src);
			BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst);

			for (unsigned y = 0; y < height; y++) {
				const WORD* src_pixel = (WORD*)src_bits;
				FIRGBAF* dst_pixel = (FIRGBAF*)dst_bits;

				for (unsigned x = 0; x < width; x++) {
					// convert and scale to the range [0..1]
					const float dst_value = (float)src_pixel[x] / 65535.0F;
					dst_pixel[x].red = dst_value;
					dst_pixel[x].green = dst_value;
					dst_pixel[x].blue = dst_value;
					dst_pixel[x].alpha = 1.0F;
				}
				src_bits += src_pitch;
				dst_bits += dst_pitch;
			}
		}
		break;

		case FIT_RGB16:
		{
			const BYTE* src_bits = (BYTE*)FreeImage_GetBits(src);
			BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst);

			for (unsigned y = 0; y < height; y++) {
				const FIRGB16* src_pixel = (FIRGB16*)src_bits;
				FIRGBAF* dst_pixel = (FIRGBAF*)dst_bits;

				for (unsigned x = 0; x < width; x++) {
					// convert and scale to the range [0..1]
					dst_pixel[x].red = (float)(src_pixel[x].red) / 65535.0F;
					dst_pixel[x].green = (float)(src_pixel[x].green) / 65535.0F;
					dst_pixel[x].blue = (float)(src_pixel[x].blue) / 65535.0F;
					dst_pixel[x].alpha = 1.0F;
				}
				src_bits += src_pitch;
				dst_bits += dst_pitch;
			}
		}
		break;

		case FIT_RGBA16:
		{
			const BYTE* src_bits = (BYTE*)FreeImage_GetBits(src);
			BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst);

			for (unsigned y = 0; y < height; y++) {
				const FIRGBA16* src_pixel = (FIRGBA16*)src_bits;
				FIRGBAF* dst_pixel = (FIRGBAF*)dst_bits;

				for (unsigned x = 0; x < width; x++) {
					// convert and scale to the range [0..1]
					dst_pixel[x].red = (float)(src_pixel[x].red) / 65535.0F;
					dst_pixel[x].green = (float)(src_pixel[x].green) / 65535.0F;
					dst_pixel[x].blue = (float)(src_pixel[x].blue) / 65535.0F;
					dst_pixel[x].alpha = (float)(src_pixel[x].alpha) / 65535.0F;
				}
				src_bits += src_pitch;
				dst_bits += dst_pitch;
			}
		}
		break;

		case FIT_FLOAT:
		{
			const BYTE* src_bits = (BYTE*)FreeImage_GetBits(src);
			BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst);

			for (unsigned y = 0; y < height; y++) {
				const float* src_pixel = (float*)src_bits;
				FIRGBAF* dst_pixel = (FIRGBAF*)dst_bits;

				for (unsigned x = 0; x < width; x++) {
					// convert by copying greyscale channel to each R, G, B channels
					// NOT assume float values are in [0..1] !!!
					const float value = src_pixel[x];
					dst_pixel[x].red = value;
					dst_pixel[x].green = value;
					dst_pixel[x].blue = value;
					dst_pixel[x].alpha = 1.0F;
				}
				src_bits += src_pitch;
				dst_bits += dst_pitch;
			}
		}
		break;

		case FIT_RGBF:
		{
			const BYTE* src_bits = (BYTE*)FreeImage_GetBits(src);
			BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst);

			for (unsigned y = 0; y < height; y++) {
				const FIRGBF* src_pixel = (FIRGBF*)src_bits;
				FIRGBAF* dst_pixel = (FIRGBAF*)dst_bits;

				for (unsigned x = 0; x < width; x++) {
					// convert pixels directly, while adding a "dummy" alpha of 1.0
					dst_pixel[x].red = src_pixel[x].red;
					dst_pixel[x].green = src_pixel[x].green;
					dst_pixel[x].blue = src_pixel[x].blue;
					dst_pixel[x].alpha = 1.0F;
				}
				src_bits += src_pitch;
				dst_bits += dst_pitch;
			}
		}
		break;
	}

	if (src != dib) {
		FreeImage_Unload(src);
	}

	return dst;
}

void CreateBindlessTexture(GLuint& texture, GLuint64& handle, const int width, const int height, const GLvoid* data, GLenum dtype) {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);		//bind empty texture object to the target

	//set the texture wrapping/filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//copy data from the host buffer
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, dtype, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);			//unbind the newly created texture from the target
	handle = glGetTextureHandleARB(texture);	//produces a handle representing the texture in a shader function
	glMakeTextureHandleResidentARB(handle);
}

BindlessTexture LoadLODTextures(const std::initializer_list<const char*>& file_names) {
	BindlessTexture res = {};

	GLuint tex_prefiltered_env_map_ = 0;

	const GLint max_level = GLint(file_names.size()) - 1; // assume we have a list of images representing different levels of a map
	glGenTextures(1, &tex_prefiltered_env_map_);
	glBindTexture(GL_TEXTURE_2D, tex_prefiltered_env_map_);
	if (glIsTexture(tex_prefiltered_env_map_)) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, max_level);
		int width, height;
		GLint level = 0;
		for (const char* filepath : file_names) {
			Texture3f prefiltered_env_map = Texture3f(filepath);
			// for HDR images use GL_RGB32F or GL_RGB16F as internal format !!!
			glTexImage2D(GL_TEXTURE_2D, level, GL_RGB32F, prefiltered_env_map.width(), prefiltered_env_map.height(), 0, GL_RGB, GL_FLOAT, prefiltered_env_map.data());
			width = prefiltered_env_map.width() / 2;
			height = prefiltered_env_map.height() / 2;
			level++;
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	res.id = tex_prefiltered_env_map_;
	res.handle = glGetTextureHandleARB(tex_prefiltered_env_map_);
	glMakeTextureHandleResidentARB(res.handle);

	return res;
}
