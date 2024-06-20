#include "Engine/Headers/3D/vertexArray.h"

#include <GL/glew.h>

const unsigned int VertexArray::ItemMax = 8;

// xyzとuvの５つのfloat型データフォーマット
VertexArray::VertexArray(const float* verts, unsigned numVerts, const unsigned* indices,
                         unsigned numIndices)
{
    this->numVerts   = numVerts;
    this->numIndices = numIndices;

    // 頂点リストオブジェクトを作成して使用可能にする
    {
        glGenVertexArrays(1, &this->vertexArray);
        glBindVertexArray(this->vertexArray);
    }

    // 頂点情報を作成
    {
        glGenBuffers(1, &this->vertexBuffer);
        // 頂点情報を格納するバッファ生成
        // GL_ARRAY_BUFFERで指定
        glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
        // GenBufferで作成したデータ領域に書き込むサイズを計算
        // 頂点データ構造(x,y,z,u,v) => 3次元座標とUV座標

        int vertexsSize = this->numVerts * sizeof(float) * ItemMax;
        glBufferData(GL_ARRAY_BUFFER, vertexsSize, verts, GL_STATIC_DRAW);
    }

    // 頂点並びの添え字を作成
    {
        glGenBuffers(1, &this->indexBuffer);
        // インデックスバッファ生成
        // GL_ELEMENT_ARRAY_BUFFERでインデックスバッファ専用のが作れる
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);

        // インデックス情報に書き込むデータサイズ計算
        int indexSize = this->numIndices * sizeof(unsigned int);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indices, GL_STATIC_DRAW);
    }

    // データレイアウト作成
    {
        // xyzの頂点とuv頂点の５つの頂点
        float strideSize            = sizeof(float) * ItemMax;
        int layoutOffsetPointerSize = 0;

        // xyzの頂点データを関連付けする
        // グループ1
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(strideSize), 0);
        layoutOffsetPointerSize += sizeof(float) * 3;

        // 法線の頂点データを関連付けする
        // グループ2
        glEnableVertexAttribArray(1);
        // xyzの頂点データ分アドレスをオフセットずらしている
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(strideSize),
                              reinterpret_cast<void*>(layoutOffsetPointerSize));
        layoutOffsetPointerSize += sizeof(float) * 3;

        // uvの頂点データを関連付けする
        // グループ2
        glEnableVertexAttribArray(2);
        // xyzの頂点データ分アドレスをオフセットずらしている
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(strideSize),
                              reinterpret_cast<void*>(layoutOffsetPointerSize));
    }
}

VertexArray::~VertexArray()
{
    glDeleteBuffers(1, &this->vertexBuffer);
    glDeleteBuffers(1, &this->indexBuffer);
    glDeleteVertexArrays(1, &this->vertexArray);
}

void VertexArray::SetActive()
{
    glBindVertexArray(this->vertexArray);
}