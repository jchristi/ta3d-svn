#ifndef MESH_H
#define MESH_H

#include <QGLWidget>
#include <QString>
#include <QVector>
#include <QFile>
#include "misc/vector.h"
#include "types.h"

enum MeshType { MESH_TRIANGLES, MESH_TRIANGLE_STRIP };

#define SURFACE_ADVANCED		0x01		// Tell it is not a 3Do surface
#define	SURFACE_REFLEC			0x02		// Reflection
#define SURFACE_LIGHTED			0x04		// Lighting
#define SURFACE_TEXTURED		0x08		// Texturing
#define SURFACE_GOURAUD			0x10		// Gouraud shading
#define SURFACE_BLENDED			0x20		// Alpha Blending
#define SURFACE_PLAYER_COLOR	0x40		// The color is the owner's color
#define SURFACE_GLSL			0x80		// Use a shader to create a surface effect

#define ROTATION				0x01
#define ROTATION_PERIODIC		0x02
#define ROTATION_COSINE			0x04		// Default calculation is linear
#define TRANSLATION				0x10
#define TRANSLATION_PERIODIC	0x20
#define TRANSLATION_COSINE		0x40

class ANIMATION				// Class used to set default animation to a model, this animation will play if no ANIMATION_DATA is provided (ie for map features)
{
public:
    byte        type;
    Vector3D    angle_0;
    Vector3D    angle_1;
    float       angle_w;
    Vector3D    translate_0;
    Vector3D    translate_1;
    float       translate_w;
};

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void destroy();

    void draw();

    void load(const QString &filename);
    void save(const QString &filename);

    void load3DM(const QString &filename);
    void load3DO(const QString &filename);
    void loadASC(const QString &filename, float size = 10.0f);
    void load3DS(const QString &filename, float scale = 10.0f);

    void computeNormals();

private:
    void load3DMrec(QFile &file);

private:
    QString             name;
    Vec                 pos;
    Mesh                *child;
    Mesh                *next;
    QVector<Vec>        vertex;
    QVector<Vec>        normal;
    QVector<GLuint>     index;
    QVector<GLuint>     tex;
    QVector<GLfloat>    tcoord;
    int                 type;

public:
    static Mesh instance;
};

#endif // MESH_H
