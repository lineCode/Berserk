//
// Created by Egor Orachyov on 27.06.2018.
//

#include "../Math/Matrix2x2.h"

namespace Berserk
{

    Matrix2x2::Matrix2x2()
    {
        m[0] = 0;
        m[1] = 0;
        m[2] = 0;
        m[3] = 0;
    }

    Matrix2x2::Matrix2x2(float32 m11, float32 m12,
                         float32 m21, float32 m22)
    {
        m[0] = m11; m[1] = m12;
        m[2] = m21; m[3] = m22;
    }

    Matrix2x2 Matrix2x2::getTranspose()
    {
        return Matrix2x2(m[0], m[2], m[1], m[3]);
    }

    float32 Matrix2x2::getDeterminant()
    {
        return (m[0] * m[3] - m[1] * m[2]);
    }

    float32* Matrix2x2::getArray()
    {
        return (float32*)m;
    }

    Matrix2x2 Matrix2x2::operator = (const Matrix2x2& M)
    {
        this->m[0] = M.m[0];
        this->m[1] = M.m[1];
        this->m[2] = M.m[2];
        this->m[3] = M.m[3];

        return *this;
    }

    Matrix2x2 Matrix2x2::operator + (const Matrix2x2& M)
    {
        return Matrix2x2(m[0] + M.m[0], m[1] + M.m[1],
                         m[2] + M.m[2], m[3] + M.m[3]);
    }

    Matrix2x2 Matrix2x2::operator - (const Matrix2x2& M)
    {
        return Matrix2x2(m[0] - M.m[0], m[1] - M.m[1],
                         m[2] - M.m[2], m[3] - M.m[3]);
    }

    Matrix2x2 Matrix2x2::operator * (const Matrix2x2& M)
    {
        return Matrix2x2(

                // 1 string

                m[0] * M.m[0] + m[1] * M.m[2],
                m[0] * M.m[1] + m[1] * M.m[3],

                // 2 string

                m[2] * M.m[0] + m[3] * M.m[2],
                m[2] * M.m[1] + m[3] * M.m[3]

        );
    }

    Matrix2x2 Matrix2x2::operator * (const float32 a)
    {
        return Matrix2x2(m[0] * a, m[1] * a,
                         m[2] * a, m[3] * a);
    }

    Matrix2x2 Matrix2x2::operator / (const float32 a)
    {
        return Matrix2x2(m[0] / a, m[1] / a,
                         m[2] / a, m[3] / a);
    }

    Vector2 Matrix2x2::operator * (const Vector2& v)
    {
        return Vector2(m[0] * v.x + m[1] * v.y, m[2] * v.x + m[3] * v.y);
    }

} // namespace Berserk