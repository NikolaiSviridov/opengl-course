#ifndef MODEL_H
#define MODEL_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../includes/tiny_obj_loader.h"
#include "../includes/stb_image.h"

#include <util/mesh.h>
#include <util/shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;


class Model 
{
public:
    // model data 
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    string directory;
    glm::vec3 position;

    Model() {}

    Model(const std::string& filename, const std::string& basepath, float factor)
    {
        tinyobj::attrib_t attrib;
        std::vector <tinyobj::shape_t> shapes;
        std::vector <tinyobj::material_t> materials;
        std::string err;
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str(), basepath.c_str())) {
            fprintf(stderr, "tinyobj::LoadObj(%s) error: %s\n", filename.c_str(), err.c_str());
            exit(1);
        }

        if (!err.empty()) {
            fprintf(stderr, "tinyobj::LoadObj(%s) warning: %s\n", filename.c_str(), err.c_str());
        }

        for (int k = 0; k < shapes.size(); k++) {
            meshes.push_back(processMesh(attrib, shapes[k].mesh, materials, basepath, factor));
        }
    }

    // draws the model, and thus all its meshes
    void draw(shader_t &shader)
    {
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }
    
private:

    Mesh processMesh(tinyobj::attrib_t& attrib,
                     tinyobj::mesh_t& meshToAdd,
                     std::vector<tinyobj::material_t>& materials,
                     const std::string& basepath,
                     float factor)
    {
        Mesh newMesh;
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        int indicesOffset = 0;

        for (int f = 0; f < meshToAdd.num_face_vertices.size(); f++) {
            int faceVerticesNumber = meshToAdd.num_face_vertices[f];

            for (int v = 0; v < faceVerticesNumber; v++) {
                tinyobj::index_t i = meshToAdd.indices[indicesOffset + v];
                vertices.push_back(attrib.vertices[3 * i.vertex_index] / factor);
                vertices.push_back(attrib.vertices[3 * i.vertex_index + 1] / factor);
                vertices.push_back(attrib.vertices[3 * i.vertex_index + 2] / factor);
                vertices.push_back(attrib.normals[3 * i.normal_index]);
                vertices.push_back(attrib.normals[3 * i.normal_index + 1]);
                vertices.push_back(attrib.normals[3 * i.normal_index + 2]);
                vertices.push_back(attrib.texcoords[2 * i.texcoord_index]);
                vertices.push_back(attrib.texcoords[2 * i.texcoord_index + 1]);
            }

            for (int v = 0; v < faceVerticesNumber; v++) {
                indices.push_back(indices.size());
            }

            indicesOffset += faceVerticesNumber;
        }

        for (int i : meshToAdd.material_ids) {
            load_texture(materials[i].diffuse_texname, "texture_diffuse", textures, basepath);
            load_texture(materials[i].specular_texname, "texture_specular", textures, basepath);
            load_texture(materials[i].normal_texname, "texture_normal", textures, basepath);
            load_texture(materials[i].ambient_texname, "texture_ambient", textures, basepath);

            newMesh.ambient = glm::vec3(materials[i].ambient[0], materials[i].ambient[1], materials[i].ambient[2]);
            newMesh.specular = glm::vec3(materials[i].specular[0], materials[i].specular[1], materials[i].specular[2]);
            newMesh.diffuse = glm::vec3(materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2]);

            if (textures.size() == 1)
                break;
        }

        unsigned int VBO, EBO, VAO;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) 0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 3));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 6));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        newMesh.vertices = vertices;
        newMesh.indices = indices;
        newMesh.textures = textures;
        newMesh.IndexCount = indices.size();
        newMesh.VAO = VAO;

        return newMesh;
    }

    bool FileExists(const std::string& abs_filename) {
        bool ret;
        FILE* fp = fopen(abs_filename.c_str(), "rb");
        if (fp) {
            ret = true;
            fclose(fp);
        } else {
            ret = false;
        }

        return ret;
    }

    void load_texture(
                     std::string& texture_filename,
                     const std::string& texture_type,
                     std::vector<Texture>& textures,
                     const std::string& basepath) {
        if (texture_filename.length() == 0)
            return;

        unsigned int texture_id;
        int w, h;
        int comp;

        if (!FileExists(texture_filename)) {
            texture_filename = basepath + texture_filename;
            if (!FileExists(texture_filename)) {
                std::cerr << "Unable to find file: " << texture_filename << std::endl;
                exit(1);
            }
        }

        for (auto loaded_texture : textures_loaded) {
            if (loaded_texture.path == texture_filename) {
                textures.push_back(loaded_texture);
                return;
            }
        }

        std::cout << "Loading texture: " << texture_filename << std::endl;

        unsigned char *image = stbi_load(texture_filename.c_str(), &w, &h, &comp, STBI_default);
        if (!image) {
            std::cerr << "Unable to load texture: " << texture_filename << std::endl;
            exit(1);
        }

        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (comp == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        } else if (comp == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        } else {
            assert(0);
        }

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(image);

        Texture texture;
        texture.id = texture_id;
        texture.type = texture_type;
        texture.path = texture_filename;
        textures.push_back(texture);
        textures_loaded.push_back(texture);
    }

    static Texture loadMaterialTextures(const std::string& path) {
        Texture texture;

        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            if (nrChannels == 3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            } else if (nrChannels == 4) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            }
        } else {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
            exit(1);
        }
        stbi_image_free(data);

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindTexture(GL_TEXTURE_2D, 0);

        texture.path = path;
        texture.id = textureID;
        return texture;
    }
};

#endif
