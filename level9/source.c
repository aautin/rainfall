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

    return (*class2 + *class1);
}
