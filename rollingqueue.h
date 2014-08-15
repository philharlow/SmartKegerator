#ifndef ROLLINGQUEUE_H
#define ROLLINGQUEUE_H

#include <vector>

using namespace std;

template <class E>
class RollingQueue
{
public:
    RollingQueue();
    RollingQueue(int max);

    int Count;

    void Add(E add);
    E* Get(int index);
    //E* Get(int index);
    E* PeekNewest();
    E* PeekOldest();

private:
    int maxSize;
    int offset;
    vector<E> items;

};


// Definitions
template<class E>
RollingQueue<E>::RollingQueue()
{
    maxSize = 10;
    offset = 0;
    Count = 0;
}

template<class E>
RollingQueue<E>::RollingQueue(int max)
{
    maxSize = max;
    offset = 0;
    Count = 0;
}

template<class E>
void RollingQueue<E>::Add(E item)
{
    if (items.size() < maxSize)
    {
        items.push_back(item);
        Count = items.size();
    }
    else
    {
        items[offset] = item;
        offset = (offset + 1) % Count;
    }
}

template<class E>
E* RollingQueue<E>::Get(int index)
{
    return &items[(index + offset) % Count];
}

template<class E>
E* RollingQueue<E>::PeekNewest()
{
    if (Count < maxSize)
        return &items[Count-1];
    return &items[(offset + Count - 1) % Count];
}

template<class E>
E* RollingQueue<E>::PeekOldest()
{
    return &items[offset];
}


#endif // ROLLINGQUEUE_H
