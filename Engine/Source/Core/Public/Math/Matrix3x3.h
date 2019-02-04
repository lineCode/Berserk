//
// Created by Egor Orachyov on 01.02.2019.
//

#ifndef BERSERK_MATRIX3X3_H
#define BERSERK_MATRIX3X3_H

#include "Public/Misc/Types.h"
#include "Public/Misc/Assert.h"
#include "Public/Misc/UsageDescriptors.h"
#include "Public/Math/MathUtility.h"
#include "Public/Strings/StaticString.h"

namespace Berserk
{

    /**
     * Matrix with size of 3x3 which stores its data in per string format and
     * multiplies vectors via right side (M * v)
     * @tparam T type (considered as float32 or float64)
     */
    template <typename T>
    class CORE_EXPORT Matrix3x3
    {
    public:

        /**
         * Initialize matrix with 1
         */
        Matrix3x3();

        /**
         * Initialize matrix with values
         */
        Matrix3x3(T m11, T m12, T m13,
                  T m21, T m22, T m23,
                  T m31, T m32, T m33);

        /**
         * Get transposed matrix
         *
         * @return
         */
        Matrix3x3 getTranspose();

        /**
         * Get matrix determinant
         *
         * @return
         */
        T getDeterminant();

        /**
         * Return pointer to its internal array of values
         *
         * @return float* pointer to m
         */
        T* get() const;

        /**
         * Assign via matrix M
         *
         * @param M
         * @return
         */
        Matrix3x3& operator = (const Matrix3x3& M);

        /**
         * Per value addition (m[i] + M.m[i])
         *
         * @param M
         * @return
         */
        Matrix3x3 operator + (const Matrix3x3& M) const;

        /**
         * Per value subtraction (m[i] - M.m[i])
         *
         * @param M
         * @return
         */
        Matrix3x3 operator - (const Matrix3x3& M) const;

        /**
         * Multiplication of matrix (this * M)
         *
         * @param M
         * @return
         */
        Matrix3x3 operator * (const Matrix3x3& M) const;

        /**
         * Multiplication via number a (m[i] * a)
         *
         * @param a
         * @return
         */
        Matrix3x3 operator * (T a) const;

        /**
         * Division by number a (m[i] / a)
         *
         * @param a
         * @return
         */
        Matrix3x3 operator / (T a) const;

        /**
         * get vector = this * v
         *
         * @param v
         * @return
         */
        Vector3<T> operator * (const Vector3<T>& v) const;

    private:

        T m[9];

    };

    template <typename T>
    Matrix3x3<T>::Matrix3x3()
    {
        m[0] = 1; m[1] = 0; m[2] = 0;
        m[3] = 0; m[4] = 1; m[5] = 0;
        m[6] = 0; m[7] = 0; m[8] = 1;
    }

    template <typename T>
    Matrix3x3<T>::Matrix3x3(T m11, T m12, T m13,
                            T m21, T m22, T m23,
                            T m31, T m32, T m33)
    {
        m[0] = m11; m[1] = m12; m[2] = m13;
        m[3] = m21; m[4] = m22; m[5] = m23;
        m[6] = m31; m[7] = m32; m[8] = m33;
    }

    template <typename T>
    Matrix3x3<T> Matrix3x3<T>::getTranspose()
    {
        return Matrix3x3(m[0], m[3], m[6],
                         m[1], m[4], m[7],
                         m[2], m[5], m[8]);
    }

    template <typename T>
    T Matrix3x3<T>::getDeterminant()
    {
        return (m[0] * m[4] * m[8] +
                m[3] * m[7] * m[2] +
                m[6] * m[1] * m[5] -
                m[2] * m[4] * m[6] -
                m[5] * m[7] * m[0] -
                m[8] * m[1] * m[3]);
    }

    template <typename T>
    T* Matrix3x3<T>::get() const
    {
        return (T*)m;
    }

    template <typename T>
    Matrix3x3<T>& Matrix3x3<T>::operator = (const Matrix3x3<T>& M)
    {
        m[0] = M.m[0];
        m[1] = M.m[1];
        m[2] = M.m[2];
        m[3] = M.m[3];
        m[4] = M.m[4];
        m[5] = M.m[5];
        m[6] = M.m[6];
        m[7] = M.m[7];
        m[8] = M.m[8];

        return *this;
    }

    template <typename T>
    Matrix3x3<T> Matrix3x3<T>::operator + (const Matrix3x3<T>& M) const
    {
        return Matrix3x3(m[0] + M.m[0], m[1] + M.m[1], m[2] + M.m[2],
                         m[3] + M.m[3], m[4] + M.m[4], m[5] + M.m[5],
                         m[6] + M.m[6], m[7] + M.m[7], m[8] + M.m[8]);
    }

    template <typename T>
    Matrix3x3<T> Matrix3x3<T>::operator - (const Matrix3x3<T>& M) const
    {
        return Matrix3x3(m[0] - M.m[0], m[1] - M.m[1], m[2] - M.m[2],
                         m[3] - M.m[3], m[4] - M.m[4], m[5] - M.m[5],
                         m[6] - M.m[6], m[7] - M.m[7], m[8] - M.m[8]);
    }

    template <typename T>
    Matrix3x3<T> Matrix3x3<T>::operator * (const Matrix3x3<T>& M) const
    {
        return Matrix3x3(

                // 1 string

                m[0] * M.m[0] + m[1] * M.m[3] + m[2] * M.m[6],
                m[0] * M.m[1] + m[1] * M.m[4] + m[2] * M.m[7],
                m[0] * M.m[2] + m[1] * M.m[5] + m[2] * M.m[8],

                // 2 string

                m[3] * M.m[0] + m[4] * M.m[3] + m[5] * M.m[6],
                m[3] * M.m[1] + m[4] * M.m[4] + m[5] * M.m[7],
                m[3] * M.m[2] + m[4] * M.m[5] + m[5] * M.m[8],

                // 3 string

                m[6] * M.m[0] + m[7] * M.m[3] + m[8] * M.m[6],
                m[6] * M.m[1] + m[7] * M.m[4] + m[8] * M.m[7],
                m[6] * M.m[2] + m[7] * M.m[5] + m[8] * M.m[8]

        );
    }

    template <typename T>
    Matrix3x3<T> Matrix3x3<T>::operator * (const T a) const
    {
        return Matrix3x3(m[0] * a, m[1] * a, m[2] * a,
                         m[3] * a, m[4] * a, m[5] * a,
                         m[6] * a, m[7] * a, m[8] * a);
    }

    template <typename T>
    Matrix3x3<T> Matrix3x3<T>::operator / (const T a) const
    {
        return Matrix3x3(m[0] / a, m[1] / a, m[2] / a,
                         m[3] / a, m[4] / a, m[5] / a,
                         m[6] / a, m[7] / a, m[8] / a);
    }

    template <typename T>
    Vector3<T> Matrix3x3<T>::operator * (const Vector3<T>& v) const
    {
        return Vector3<T>(m[0] * v.x + m[1] * v.y + m[2] * v.z,
                          m[3] * v.x + m[4] * v.y + m[5] * v.z,
                          m[6] * v.x + m[7] * v.y + m[8] * v.z);
    }

} // namespace Berserk

#endif //BERSERK_MATRIX3X3_H
