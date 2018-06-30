//
// Created by Egor Orachyov on 30.06.2018.
//

#ifndef BERSERKENGINE_MATRICES_H
#define BERSERKENGINE_MATRICES_H

#include "Matrix2x2.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"

namespace Berserk
{

    /**
     * Initialize matrix with d element on the main diagonal
     *
     * @param d
     * @return
     */
    Matrix4x4 newMatrix(float32 d);

    /**
     * Initialize matrix via vectors as its columns
     *
     * @param c1
     * @param c2
     * @param c3
     * @param c4
     * @return
     */
    Matrix4x4 newMatrix(Vector4 c1, Vector4 c2, Vector4 c3, Vector4 c4);

    /**
     * Scale matrix for x, y, z axises
     *
     * @param sX
     * @param sY
     * @param sZ
     * @return
     */
    Matrix4x4 Scale(float32 sX, float32 sY, float32 sZ);

    /**
     * Translation matrix for t vector
     *
     * @param t
     * @return
     */
    Matrix4x4 Translate(Vector3 t);

    /**
     * Clockwise rotation around the x-axis
     *
     * @param angle
     * @return
     */
    Matrix4x4 RotateX(float32 angle);

    /**
     * Clockwise rotation around the y-axis
     *
     * @param angle
     * @return
     */
    Matrix4x4 RotateY(float32 angle);

    /**
     * Clockwise rotation around the z-axis
     *
     * @param angle
     * @return
     */
    Matrix4x4 RotateZ(float32 angle);

    /**
     * Clockwise rotation around the axis
     *
     * @param angle
     * @return
     */
    Matrix4x4 Rotate(Vector3 axis, float32 angle);

    /**
     * Look at view matrix
     *
     * @param eye Current viewer position
     * @param target Point of viewing
     * @param up Up vector to define orientation
     * @return
     */
    Matrix4x4 LookAt(Vector3 eye, Vector3 target, Vector3 up);

    /**
     * Perspective projection for OpenGL
     *
     * @warning Fovy should be me more than 0
     * @warning Aspect should be more than 0
     *
     * @param fovy Angle between left and right sides in radians
     * @param aspect Width-to-height ratio
     * @param near Near clip plane
     * @param far Far clip plane
     * @return
     */
    Matrix4x4 Perspective(float32 fovy, float32 aspect, float32 near, float32 far);

    Matrix4x4 Orthographic();

} // namespace Berserk

#endif //BERSERKENGINE_MATRICES_H
