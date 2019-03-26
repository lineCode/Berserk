//
// Created by Egor Orachyov on 17.03.2019.
//

#ifndef BERSERK_GLPROFILE_H
#define BERSERK_GLPROFILE_H

#ifndef OPENGL_PROFILE
    #define OPENGL_PROFILE 1
#endif

#if OPENGL_PROFILE
    #define PROFILE_GL_PLATFORM 1
#endif

#if OPENGL_PROFILE
    #define PROFILE_GL_MANAGERS 1
#endif

#if PROFILE_GL_MANAGERS
    #define PROFILE_GL_TEXTURE_MANAGER 0
#endif

#if PROFILE_GL_MANAGERS
    #define PROFILE_GL_SHADER_MANAGER 1
#endif

#if PROFILE_GL_MANAGERS
    #define PROFILE_GL_BUFFER_MANAGER 1
#endif

#endif //BERSERK_GLPROFILE_H
