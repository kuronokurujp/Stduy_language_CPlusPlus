#ifndef __VERTEX_ARRAY_H__
#define __VERTEX_ARRAY_H__

/// 頂点配列
class VertexArray
{
public:
    static const unsigned int ItemMax;

    VertexArray(const float* verts, unsigned numVerts, const unsigned* indices,
                unsigned numIndices);
    ~VertexArray();

    void SetActive();

    const unsigned GetNumIndices() const { return this->numIndices; }
    const unsigned GetNumVerts() const { return this->numVerts; }

private:
    unsigned numVerts;
    unsigned numIndices;
    unsigned vertexBuffer;
    unsigned indexBuffer;
    unsigned vertexArray;
};

#endif  // __VERTEX_ARRAY_H__
