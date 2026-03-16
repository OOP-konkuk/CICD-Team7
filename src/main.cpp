#include <iostream>

int main() {
    // cppcheck: 초기화 안 된 변수
    int x;
    std::cout << x << std::endl;

    // cppcheck: 메모리 누수
    int* ptr = new int[10];

    // clang-tidy: 사용 안 하는 변수
    int unused = 42;

    // cppcheck: 배열 범위 초과
    int arr[5];
    arr[10] = 1;

    return 0;
}