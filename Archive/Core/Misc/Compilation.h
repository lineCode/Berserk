//
// Created by Egor Orachyov on 22.05.2018.
//

#ifndef BERSERKENGINE_COMPILATION_H
#define BERSERKENGINE_COMPILATION_H

/**
 * @brief Debug compilation mode
 *
 * Set DEBUG to 1 and RELEASE to 0 to enable compilation with
 * all the additional features: params check in functions, console
 * logging, debug GUI, profiler, DEV MENU, ...
 *
 * @warning Reduces performance of an application
 */

#ifndef DEBUG
    #define DEBUG 1
#endif

/**
 * @brief Release compilation mode
 *
 * Set RELEASE to 1 and DEBUG to 0 to disable additional performance-heavy
 * functions (see DEBUG) (it seems to be main for-user-application mode)
 *
 * @warning High risk of an application's crashes
 */

#ifndef RELEASE
    #define RELEASE 0
#endif

#endif //BERSERKENGINE_COMPILATION_H
