#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "vector3.h"
#include "texture.h"
#include "structs.h"

/*! \def NO_TEXTURES
\brief Maxim�ln� po�et textur p�i�azen�ch materi�lu.
*/
#define NO_TEXTURES 7

/*! \def IOR_AIR
\brief Index lomu vzduchu za norm�ln�ho tlaku.
*/
#define IOR_AIR 1.000293f

/*! \def IOR_AIR
\brief Index lomu vody.
*/
#define IOR_WATER 1.33f

/*! \def IOR_GLASS
\brief Index lomu skla.
*/
#define IOR_GLASS 1.5f

/* types of shaders */
enum class Shader : char { NORMAL = 1, LAMBERT = 2, PHONG = 3, GLASS = 4, PBR = 5, MIRROR = 6, TS = 7, CT = 8 };

#pragma pack(push, 1)
struct GLMaterial {
	Color3f diffuse;			//12B 
	GLbyte pad0[4];				//+4 = 16 B
	GLuint64 texDiffuse = 0;	//8 B
	GLbyte pad1[8];				//+8 = 16 B

	Color3f rma;				//12B 
	GLbyte pad2[4];				//+4 = 16 B
	GLuint64 texRMA = 0;		//8 B
	GLbyte pad3[8];				//+8 = 16 B

	Color3f normal;				//12B 
	GLbyte pad4[4];				//+4 = 16 B
	GLuint64 texNormal = 0;		//8 B
	GLbyte pad5[8];				//+8 = 16 B
};
#pragma pack(pop)

/*! \class Material
\brief A simple material.

\author Tom� Fabi�n
\version 0.9
\date 2011-2018
*/
class Material {
public:
	//! Implicitn� konstruktor.
	/*!
	Inicializuje v�echny slo�ky materi�lu na v�choz� matn� �ed� materi�l.
	*/
	Material();

	//! Specializovan� konstruktor.
	/*!
	Inicializuje materi�l podle zadan�ch hodnot parametr�.

	\param name n�zev materi�lu.
	\param ambient barva prost�ed�.
	\param diffuse barva rozptylu.
	\param specular barva odrazu.
	\param emission barva emise.
	\param shininess lesklost.
	\param ior index lomu.
	\param shader shader to be used.
	\param textures pole ukazatel� na textury.
	\param no_textures d�lka pole \a textures. Maxim�ln� \a NO_TEXTURES - 1.
	*/
	Material(std::string& name, const Color3f& ambient, const Color3f& diffuse,
			 const Color3f& specular, const Color3f& emission, const float reflectivity,
			 const float shininess, const float ior, const Shader shader,
			 Texture3u** textures = NULL, const int no_textures = 0);

	//! Destruktor.
	/*!
	Uvoln� v�echny alokovan� zdroje.
	*/
	~Material();

	//void Print();

	//! Nastav� n�zev materi�lu.
	/*!
	\param name n�zev materi�lu.
	*/
	void set_name(const char* name);

	//! Vr�t� n�zev materi�lu.
	/*!
	\return N�zev materi�lu.
	*/
	std::string name() const;

	//! Nastav� texturu.
	/*!
	\param slot ��slo slotu, do kter�ho bude textura p�i�azena. Maxim�ln� \a NO_TEXTURES - 1.
	\param texture ukazatel na texturu.
	*/
	void set_texture(const int slot, Texture3u* texture);

	//! Vr�t� texturu.
	/*!
	\param slot ��slo slotu textury. Maxim�ln� \a NO_TEXTURES - 1.
	\return Ukazatel na zvolenou texturu.
	*/
	Texture3u* texture(const int slot) const;

	Shader shader() const;

	void set_shader(Shader shader);

	Color3f ambient(const Coord2f* tex_coord = nullptr) const;
	Color3f diffuse(const Coord2f* tex_coord = nullptr) const;
	Color3f specular(const Coord2f* tex_coord = nullptr) const;
	Color3f bump(const Coord2f* tex_coord = nullptr) const;
	float roughness(const Coord2f* tex_coord = nullptr) const;

	Color3f emission(const Coord2f* tex_coord = nullptr) const;

	GLMaterial GenerateGLMaterial();
public:
	Color3f ambient_; /*!< RGB barva prost�ed� \f$\left<0, 1\right>^3\f$. */
	Color3f diffuse_; /*!< RGB barva rozptylu \f$\left<0, 1\right>^3\f$. */
	Color3f specular_; /*!< RGB barva odrazu \f$\left<0, 1\right>^3\f$. */

	Color3f emission_; /*!< RGB barva emise \f$\left<0, 1\right>^3\f$. */

	float shininess; /*!< Koeficient lesklosti (\f$\ge 0\f$). ��m je hodnota v�t��, t�m se jev� povrch lesklej��. */
	float roughness_; /*!< Koeficient drsnosti. */
	float metallicness; /*!< Koeficient kovovosti. */
	float reflectivity; /*!< Koeficient odrazivosti. */
	float ior; /*!< Index lomu. */

	static const char kDiffuseMapSlot; /*!< ��slo slotu difuzn� textury. */
	static const char kSpecularMapSlot; /*!< ��slo slotu spekul�rn� textury. */
	static const char kNormalMapSlot; /*!< ��slo slotu norm�lov� textury. */
	static const char kOpacityMapSlot; /*!< ��slo slotu transparentn� textury. */
	static const char kRoughnessMapSlot; /*!< ��slo slotu textury drsnosti. */
	static const char kMetallicnessMapSlot; /*!< ��slo slotu textury kovovosti. */
	static const char kRMAMapSlot;

	int idx = idxCounter++;
private:
	Texture3u* textures_[NO_TEXTURES]; /*!< Pole ukazatel� na textury. */
	/*
	slot 0 - diffuse map + alpha
	slot 1 - specular map + opaque alpha
	slot 2 - normal map
	slot 3 - transparency map
	*/

	std::string name_; /*!< Material name. */

	Shader shader_{ Shader::NORMAL }; /*!< Type of used shader. */

	static inline int idxCounter = 0;
};

#endif
