#include <iostream>

template <typename T>
class A {
public:
    A() {value = 0;};
    T Fun(T* x) {
        return Sum(x);
    }

private:
    T value;

private:
    T Sum(T* x) {
        T sum = 0;

        sum = *x;

        return sum;
    }

};

int main() {
    A<int> a;

    int b = 10;

    std::cout << a.Fun(&b) << std::endl;

    return 0;
}
