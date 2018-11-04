#include <iostream>
#include <Stacktrace.hpp>

std::string goo(int a) {
    ee::Stacktrace<16> stacktrace;
    return stacktrace.asString();
}

std::string bar(int a) {
    return goo(a);
}

std::string foo(int a) {
    return bar(a);
}

int main() {
    std::cout << foo(16) << std::endl;
    return EXIT_SUCCESS;
}