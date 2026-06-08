#include <cstring>
#include <stdlib.h>

class N
{
public:
    N(int n) : n(n) {}

    virtual int operator+(const N &rhs)
	{
        return this->n + rhs.n;
    }

    virtual int operator-(const N &rhs)
	{
        return this->n - rhs.n;
    }

    // vptr isnt really accessible in C++
    // it's just here to visualize the vtable accessibility in the main function
    void* vptr[2] = { (void*) &N::operator+, (void*) &N::operator- };

    void setAnnotation(char *s)
	{
        memcpy(this->annotation, s, strlen(s));
    }

    char annotation[100];
    int n;
};

int main(int argc, char *argv[])
{
    if (argc < 2) exit(1);

    N *class1 = new N(5);
    N *class2 = new N(6);

    class1->setAnnotation(argv[1]);

    N* ptr1 = class1;
    N* ptr2 = class2;
    void* vptr1 = ptr1->vptr[0];

    return vptr1(*ptr1, *ptr2);
}
