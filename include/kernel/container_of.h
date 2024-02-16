#ifndef KERNEL_CONTAINER_OF_H
#define KERNEL_CONTAINER_OF_H

#ifndef container_of
#define container_of(ptr, type, member) ({                              \
                const typeof(((type*)0)->member)*__mptr = (ptr);        \
                (type*)((char*)__mptr - offsetof(type, member)); })
#endif

#endif
