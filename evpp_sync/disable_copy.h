#ifndef __DISABLE_COPY_H_
#define __DISABLE_COPY_H_

class disable_copy {
public:
    disable_copy(const disable_copy&) = delete;
    disable_copy& operator = (const disable_copy&) = delete;
protected:
    disable_copy() {}
    virtual ~disable_copy() {};
};

#endif
