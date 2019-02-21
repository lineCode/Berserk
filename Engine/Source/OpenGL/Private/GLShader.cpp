//
// Created by Egor Orachyov on 07.02.2019.
//

#include "GLShader.h"
#include "GLInclude.h"

namespace Berserk
{

    void GLShader::initialize(const char *name)
    {
        mProgram = 0;

        for (uint32 i = 0; i < MAX_SHADER_COUNT; i++) mShaders[i] = 0;

        mReferenceCount = 0;
        mResourceName = name;

        new(&mUniformMap) HashMap<CName,uint32>(CName::Hashing);
    }

    void GLShader::addReference()
    {
        mReferenceCount += 1;
    }

    void GLShader::release()
    {
        if (mReferenceCount > 0)
        {
            mReferenceCount -= 1;
        }

        if (mReferenceCount == 0 && mProgram)
        {
            PUSH("GLShader: delete | name: %s | program: %u ", mResourceName.get(), mProgram);

            for (uint32 i = 0; i < MAX_SHADER_COUNT; i++)
            {
                if (mShaders[i]) { glDeleteShader(mShaders[i]); }
                mShaders[i] = 0;
            }

            glDeleteProgram(mProgram);
            mProgram = 0;

            delete (&mUniformMap);
        }
    }

    uint32 GLShader::getReferenceCount()
    {
        return mReferenceCount;
    }

    uint32 GLShader::getMemoryUsage()
    {
        return 0;
    }

    const char* GLShader::getName()
    {
        return mResourceName.get();
    }

    void GLShader::createProgram()
    {
        mProgram = glCreateProgram();
        FAIL(mProgram, "Cannot create GL GPU program");
    }

    void GLShader::attachShader(ShaderType type, const char *source, const char* filename)
    {
        if (!mProgram)
        {
            ERROR("An attempt to load shader to not initialized GPU program [file: %s]", filename);
            return;
        }

        mShaders[type] = glCreateShader(getShaderType(type));
        FAIL(mShaders[type], "Cannot create GL GPU shader [file: %s]", filename);

        auto shader = mShaders[type];

        const char* sources[] = {source};
        glShaderSource(shader, 1, sources, nullptr);

        glCompileShader(shader);

        int32 result = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

        if (!result)
        {
            ERROR("Cannot compile shader [file: %s]", filename);

            int32 logLen;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

            if (logLen > 0)
            {
                char buffer[Buffers::SIZE_1024];
                logLen = Math::min(logLen, Buffers::SIZE_1024);

                int32 written;
                glGetShaderInfoLog(shader, Buffers::SIZE_1024, &written, buffer);

                PUSH("Shader log [file: %s]", filename);
                PUSH("%s", buffer);
            }

            glDeleteShader(shader);
            mShaders[type] = 0;

            return;
        }

        glAttachShader(mProgram, shader);
    }

    void GLShader::link()
    {
        if (!mProgram)
        {
            ERROR("An attempt to link not initialized GPU program");
            return;
        }

        glLinkProgram(mProgram);

        int32 status;
        glGetProgramiv(mProgram, GL_LINK_STATUS, &status);
        if (!status)
        {
            ERROR("Cannot link GL GPU program");

            int32 logLen;
            glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &logLen);

            if (logLen > 0)
            {
                char log[Buffers::SIZE_1024];
                logLen = Math::min(logLen, Buffers::SIZE_1024);

                GLsizei written;
                glGetProgramInfoLog(mProgram, logLen, &written, log);

                PUSH("Shader program log");
                PUSH("%s", log);
            }

            return;
        }
    }

    void GLShader::validate()
    {
        if (!mProgram)
        {
            return;
        }

        int32 status;
        glValidateProgram(mProgram);
        glGetProgramiv(mProgram, GL_VALIDATE_STATUS, &status);

        if (GL_FALSE == status)
        {

            int32 length = 0;
            char log[Buffers::SIZE_1024];

            glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &length);

            if (length > 0)
            {
                length = Math::min(length, Buffers::SIZE_1024);
                int32 written = 0;
                glGetProgramInfoLog(mProgram, length, &written, log);

                PUSH("Shader program validation");
                PUSH("%s", log);
            }
        }
    }

    void GLShader::use()
    {
        glUseProgram(mProgram);
    }

    void GLShader::addUniformVariable(const char *name)
    {
        int32 location = glGetUniformLocation(mProgram, name);

        if (location == NOT_FOUND)
        {
            WARNING("Cannot find uniform location %s", name);
            return;
        }

        mUniformMap.add(CName(name), (uint32)location);
    }

    void GLShader::bindAttributeLocation(uint32 location, const char *name)
    {
        glBindAttribLocation(mProgram, location, name);
    }

    void GLShader::bindFragmentDataLocation(uint32 location, const char *name)
    {
        glBindFragDataLocation(mProgram, location, name);
    }
    void GLShader::setUniform(const char *name, int32 i)
    {
        auto location = mUniformMap[CName(name)];
        FAIL(location, "Attempt to access unresolved uniform variable [name: %s]", name);
        glUniform1i(*location, i);
    }

    void GLShader::setUniform(const char *name, uint32 i)
    {
        auto location = mUniformMap[CName(name)];
        FAIL(location, "Attempt to access unresolved uniform variable [name: %s]", name);
        glUniform1ui(*location, i);
    }

    void GLShader::setUniform(const char *name, float32 f)
    {
        auto location = mUniformMap[CName(name)];
        FAIL(location, "Attempt to access unresolved uniform variable [name: %s]", name);
        glUniform1f(*location, f);
    }

    void GLShader::setUniform(const char *name, const Vec2f &v)
    {
        auto location = mUniformMap[CName(name)];
        FAIL(location, "Attempt to access unresolved uniform variable [name: %s]", name);
        glUniform2f(*location, v.x, v.y);
    }

    void GLShader::setUniform(const char *name, const Vec3f &v)
    {
        auto location = mUniformMap[CName(name)];
        FAIL(location, "Attempt to access unresolved uniform variable [name: %s]", name);
        glUniform3f(*location, v.x, v.y, v.z);
    }

    void GLShader::setUniform(const char *name, const Vec4f &v)
    {
        auto location = mUniformMap[CName(name)];
        FAIL(location, "Attempt to access unresolved uniform variable [name: %s]", name);
        glUniform4f(*location, v.x, v.y, v.z, v.w);
    }

    void GLShader::setUniform(const char *name, const Mat2x2f &m)
    {
        auto location = mUniformMap[CName(name)];
        FAIL(location, "Attempt to access unresolved uniform variable [name: %s]", name);
        glUniformMatrix2fv(*location, 1, GL_TRUE, m.get());
    }

    void GLShader::setUniform(const char *name, const Mat3x3f &m)
    {
        auto location = mUniformMap[CName(name)];
        FAIL(location, "Attempt to access unresolved uniform variable [name: %s]", name);
        glUniformMatrix3fv(*location, 1, GL_TRUE, m.get());
    }

    void GLShader::setUniform(const char *name, const Mat4x4f &m)
    {
        auto location = mUniformMap[CName(name)];
        FAIL(location, "Attempt to access unresolved uniform variable [name: %s]", name);
        glUniformMatrix4fv(*location, 1, GL_TRUE, m.get());
    }

    void GLShader::setSubroutines(ShaderType type, uint32 count, uint32 *indices)
    {
        glUniformSubroutinesuiv(getShaderType(type), count, indices);
    }

    int32 GLShader::getUniformLocation(const char *name)
    {
        int32 location = glGetUniformLocation(mProgram, name);
        if (location == NOT_FOUND) WARNING("Cannot find uniform location %s", name);
        return location;
    }

    int32 GLShader::getUniformBlockIndex(const char *name)
    {
        int32 location = glGetUniformBlockIndex(mProgram, name);
        if (location == NOT_FOUND) WARNING("Cannot find uniform block index %s", name);
        return location;
    }

    int32 GLShader::getAttributeLocation(const char *name)
    {
        int32 location = glGetAttribLocation(mProgram, name);
        if (location == NOT_FOUND) WARNING("Cannot find attribute location %s", name);
        return location;
    }

    int32 GLShader::getSubroutineLocation(ShaderType type, const char *name)
    {
        int32 location = glGetSubroutineUniformLocation(mProgram, getShaderType(type), name);
        if (location == NOT_FOUND) WARNING("Cannot find subroutine location %s", name);
        return location;
    }

    uint32 GLShader::getSubroutineIndex(ShaderType type, const char *name)
    {
        uint32 location = glGetSubroutineIndex(mProgram, getShaderType(type), name);
        if (location == GL_INVALID_INDEX) WARNING("Cannot find subroutine index %s", name);
        return location;
    }

    uint32 GLShader::getShaderType(ShaderType type)
    {
        switch (type)
        {
            case VERTEX:
                return GL_VERTEX_SHADER;

            case GEOMETRY:
                return GL_GEOMETRY_SHADER;

            case TESSELLATION_CONTROL:
                return GL_TESS_CONTROL_SHADER;

            case TESSELLATION_EVALUATION:
                return GL_TESS_EVALUATION_SHADER;

            case FRAGMENT:
                return GL_FRAGMENT_SHADER;

            case COMPUTE:
                return GL_COMPUTE_SHADER;

            default:
                WARNING("Invalid argument value");
        }

        return 0;
    }

} // namespace Berserk