#include <span>
#include "Mesh.hpp"


Mesh::Mesh(std::span<const float> vertices, std::span<const unsigned int> indices){
    idx_count = indices.size();

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);  
    glEnableVertexAttribArray(1); 

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float))); 
    
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* indices.size(), indices.data(), GL_STATIC_DRAW);
}

void Mesh::render(){
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, idx_count, GL_UNSIGNED_SHORT, 0);
}
