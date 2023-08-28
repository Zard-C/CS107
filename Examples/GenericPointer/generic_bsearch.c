#include <stdio.h>

/*
 *  Binary Search algorithm using generic pointers and callback function
 *  to find the index of the  last element which is less than or equal to
 *  target element. calculate elem address manually and compare elements
 *  by callback function provided by the caller
 *  args:  void* elemAddr , the start address of the array
 *         int elemnum,  the number of array elements
 *         void* target,  the address of target element
 *         int (*cmpfn)(const void* vp1, const void* vp2), callback funtion provided by the caller for comparing
 * elements return value: the index of the last elements which is less than or euqal to the target element, the caller
 * determines if it is a target element
 */

int binsearch(void *elemAddr, int elemSize, int elemnum, const void *target, int (*cmpfn)(const void *, const void *))
{
    int lo = 0;
    int hi = elemnum;
    int mi = 0;
    void *vp = NULL;
    while (lo < hi)
    {
        mi = (lo + hi) >> 1;
        vp = (char *)elemAddr + mi * elemSize;
        if (cmpfn(target, vp) < 0)
            hi = mi;
        else
            lo = mi + 1;
    }
    return --lo;
}
