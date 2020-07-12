#ifndef CHIP_MANAGER_H_H_H
#define CHIP_MANAGER_H_H_H

typedef void(*init_done_cb)();
typedef struct
{

    const char * name;
    void  (*vendor_init)(init_done_cb cb);
} chip_mgr_t;

#endif
