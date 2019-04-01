//
// Created by Egor Orachyov on 28.03.2019.
//

#ifndef BERSERK_IOBJECT_H
#define BERSERK_IOBJECT_H

#include <Misc/Crc32.h>
#include <Misc/Types.h>
#include <Strings/String.h>
#include <Memory/MemorySizer.h>

namespace Berserk::EntitySystem
{

    /**
     * The base class for all engine game-play foundation system objects
     * and components. Root class for all objects in the Entity Component System module.
     * Its interface should not be changed.
     */
    class IObject
    {
    public:

        /** Default fields setup */
        IObject();

        /** Primary destructor */
        ~IObject() = default;

    public:

        /** Called after object creation */
        virtual void initialize()   { /** should be override by child */ }

        /** Called before object disposing */
        virtual void destroy()      { /** should be override by child */ }

        /** Called to serialize object */
        virtual void serialize()    { /** should be override by child */ }

    public:

        /** @return True, if system updates this object at all (ECS) */
        bool isActive()             { return mIsActive; }

        /** @return True, if object updated, but its progress is paused (game-play) */
        bool isPaused()             { return mIsPaused; }

        /** @return True, if can edit configuration of this object (ECS) */
        bool isEditable()           { return mIsEditable; }

        /** @return True, if can attach this object to others (ECS) */
        bool isAttachable()         { return mIsAttachable; }

        /** @return True, if can use toggle mechanics (game-play) */
        bool isToggleable()         { return mIsToggleable; }

        /** @return True, if can get damage (game-play) */
        bool isDamageable()         { return mIsDamageable; }

        /** @return True, if object should be setup after multi-threaded execution (ECS) */
        bool isDirty()              { return mIsDirty; }

        /** @return True, if can move this object on the scene (ECS) */
        bool isMovable()            { return mIsMovable; }

        /** @return True, if can provess this object in multi-threaded mode (ECS) */
        bool isMultiThreaded()      { return mIsMultiThreaded; }

        /** @return True, if object has transformation component (ECS) */
        bool hasTransformation()    { return mHasTransformation; }

        /** @return True, if object has rendering component (ECS) */
        bool hasRenderComponet()    { return mHasRenderComponent; }

        /** @return True, if object has audio component (ECS) */
        bool hasAudioComponet()     { return mHasAudioComponent; }

        /** @return True, if object has physics component (ECS) */
        bool hasPhysicsComponet()   { return mHasPhysicsComponent; }

        /** @return True, if object has AI component (ECS) */
        bool hasAIComponet()        { return mHasAIComponent; }

        /** Allows to rename object */
        virtual void rename(const char* name);

        /** Allows to profile memory usage by this object */
        virtual void getMemoryUsage(MemorySizer *sizer);

        /** @return This hashed object by default crc32 algorithm */
        virtual uint32 hash();

        /** @return One-line string info about object */
        virtual const char* toString() { return ""; }

        /** @return Poiner to char* object name */
        virtual const char* getName()  { return mObjectName.get(); }

    protected:

        uint8 mIsActive             : 1;
        uint8 mIsPaused             : 1;
        uint8 mIsEditable           : 1;
        uint8 mIsAttachable         : 1;
        uint8 mIsToggleable         : 1;
        uint8 mIsDamageable         : 1;
        uint8 mIsDirty              : 1;
        uint8 mIsMovable            : 1;
        uint8 mIsMultiThreaded      : 1;
        uint8 mHasTransformation    : 1;
        uint8 mHasRenderComponent   : 1;
        uint8 mHasAudioComponent    : 1;
        uint8 mHasPhysicsComponent  : 1;
        uint8 mHasAIComponent       : 1;

        CString mObjectName;

    };

} // namespace Berserk::EntitySystem

#endif //BERSERK_IOBJECT_H