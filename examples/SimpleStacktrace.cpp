#include <iostream>
#include <ee/Stacktrace.hpp>

std::string goo(int a) {
    auto stacktrace = ee::Stacktrace::create(a);
    if (stacktrace.has_value()) {
        return stacktrace->get()->asString();
    } else {
        return "";
    }
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