//
// Created by Egor Orachyov on 06.07.2018.
//

#ifndef BERSERKENGINE_LINKEDLIST_H
#define BERSERKENGINE_LINKEDLIST_H

#include "../Essential/Types.h"
#include "../Essential/Assert.h"
#include "../Essential/UsageDescriptors.h"

#include "Memory/PoolAllocator.h"

namespace Berserk
{

    /**
     * Linked list container based up on PoolAllocator. Store data (not pointers),
     * expand internal buffer if there is not enough memory
     */
    template <typename Element> class DATA_API LinkedList
    {
    private:

        /* List Node */
        typedef struct Node
        {
        public:
            Element data;
            Node* next;
        } Node;

    public:

        LinkedList();
        ~LinkedList();

        /**
         * Init list with start capacity
         * @param capacity Number of free elements in buffer
         */
        void init(uint32 capacity = 16);

        /**
         * Add element to the list
         *
         * @param element Element which should be STORED into list
         */
        void add(const Element& element);

        /**
         * Add element in the front of the list
         *
         * @param element Element which should be STORED into list
         */
        void addFront(const Element& element);

        /**
         * Add element in the back of the list
         *
         * @param element Element which should be STORED into list
         */
        void addBack(const Element& element);

        /**
         * Removes element from the list if it exist
         *
         * @param element Element to be removed
         */
        void remove(Element& element);

        /**
         * Empty list and remove all the elements (will be called standard destructors
         * for each element)
         */
        void empty();

        /**
         * Get first element from the list
         *
         * @return First element
         */
        Element& getFirst() const;

        /**
         * Get current element of iterator
         *
         * @return Current element
         */
        Element& getCurrent() const;

        /**
         * Get last element of the list
         *
         * @return Last element
         */
        Element& getLast() const;

        /**
         * Returns the next element which is stored after param
         *
         * @param element Element to find the next after that
         * @return Element which stays after param element
         */
        Element& getNext(Element& element) const;

        /**
         * Iterator - return null if restart == false or returns
         * current element and moves iterator pointer to the next element
         *
         * @param restart Flag should the iterator begin from start
         * @return
         */
        Element& iterate(bool restart = false);

        /**
         * Get max num of elements which could be stored in the list
         *
         * @return Max num of element
         */
        uint32 getCapacity() const;

        /**
         * Get num of elements currently stored in the list
         *
         * @return Current number of elements
         */
        uint32 getSize() const;

        /**
         * Get total size of one list Node
         *
         * @return Size of Node
         */
        uint32 getSizeOfNode() const;

    private:

        uint32 mSize;
        Node* mHead;
        Node* mTail;
        Node* mIterator;
        PoolAllocator mPool;

    };

    template <typename Element>
    LinkedList<Element>::LinkedList()
    {
        mSize = 0;
        mHead = NULL;
        mTail = NULL;
        mIterator = NULL;
    }

    template <typename Element>
    LinkedList<Element>::~LinkedList()
    {
        empty();
    }

    template <typename Element>
    void LinkedList<Element>::init(uint32 capacity)
    {
        ASSERT(!mHead, "List should have NULL head");
        ASSERT(!mTail, "List should have NULL head");

        mPool.init(sizeof(Element), capacity);
        mSize = 0;
        mHead = NULL;
        mTail = NULL;
        mIterator = NULL;
    }

    template <typename Element>
    void LinkedList<Element>::add(const Element& element)
    {
        if (mHead == NULL)
        {
            mHead = (Node*)mPool.allocBlock();
            mHead->data = element;
            mHead->next = NULL;

            mTail = mHead;
        }
        else
        {
            mTail->next = (Node*)mPool.allocBlock();
            mTail = mTail->next;
            mTail->data = element;
            mTail->next = NULL;
        }

        mSize += 1;
    }

    template <typename Element>
    void LinkedList<Element>::addFront(const Element& element)
    {
        if (mHead == NULL)
        {
            mHead = (Node*)mPool.allocBlock();
            mHead->data = element;
            mHead->next = NULL;

            mTail = mHead;
        }
        else
        {
            Node* newHead = (Node*)mPool.allocBlock();
            newHead->next = mHead;
            newHead->data = element;
            mHead = newHead;
        }

        mSize += 1;
    }

    template <typename Element>
    void LinkedList<Element>::addBack(const Element& element)
    {
        if (mHead == NULL)
        {
            mHead = (Node*)mPool.allocBlock();
            mHead->data = element;
            mHead->next = NULL;

            mTail = mHead;
        }
        else
        {
            mTail->next = (Node*)mPool.allocBlock();
            mTail = mTail->next;
            mTail->data = element;
            mTail->next = NULL;
        }

        mSize += 1;
    }

    template <typename Element>
    void LinkedList<Element>::remove(Element& element)
    {
        Node* prev = NULL;
        Node* tmp = mHead;
        while (tmp)
        {
            if (tmp->data == element)
            {
                if (!tmp->next)
                {
                    mTail = prev;
                }

                tmp->data.~Element();

                if (prev)
                {
                    prev->next = tmp->next;
                    mPool.freeBlock((void*)tmp);
                }
                else
                {
                    mHead = tmp->next;
                    mPool.freeBlock((void*)tmp);
                }

                mSize -= 1;
                mIterator = NULL;

                return;
            }

            prev = tmp;
            tmp = tmp->next;
        }
    }

    template <typename Element>
    void LinkedList<Element>::empty()
    {
        Node* next;
        Node* tmp = mHead;
        while (tmp)
        {
            next = tmp->next;
            tmp->data.~Element();
            mPool.freeBlock((void*)tmp);
            tmp = next;
        }

        mSize = 0;
        mHead = NULL;
        mTail = NULL;
        mIterator = NULL;
    }

    template <typename Element>
    Element& LinkedList<Element>::getFirst() const
    {
        ASSERT(mHead, "Data should be allocated before usage");
        return mHead->data;
    }

    template <typename Element>
    Element& LinkedList<Element>::getLast() const
    {
        ASSERT(mTail, "Data should be allocated before usage");
        return mTail->data;
    }

    template <typename Element>
    Element& LinkedList<Element>::iterate(bool restart)
    {
        Element element;

        if (restart)
        {
            element = NULL;
            mIterator = mHead;
        }
        else
        {
            if (mIterator)
            {
                element = mIterator->data;
                mIterator = mIterator->next;
            }
            else
            {
                element = NULL;
                mIterator = NULL;
            }
        }

        return element;
    }

    template <typename Element>
    Element& LinkedList<Element>::getCurrent() const
    {
        return mIterator->data;
    }

    template <typename Element>
    Element& LinkedList<Element>::getNext(Element& element) const
    {
        Node* tmp = mHead;
        while (tmp)
        {
            if (tmp->data == element)
            {
                return tmp->next->data;
            }

            tmp = tmp->next;
        }

        return NULL;
    }

    template <typename Element>
    uint32 LinkedList<Element>::getCapacity() const
    {
        return mPool.getCapacity();
    }

    template <typename Element>
    uint32 LinkedList<Element>::getSize() const
    {
        return mSize;
    }

    template <typename Element>
    uint32 LinkedList<Element>::getSizeOfNode() const
    {
        return sizeof(Node);
    }

} // namespace Berserk

#endif //BERSERKENGINE_LINKEDLIST_H
