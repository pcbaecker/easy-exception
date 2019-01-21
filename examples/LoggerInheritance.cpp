#include <ee/Logger.hpp>
#include <iostream>

class BaseClass {
public:
    BaseClass() {
        mLogger.setName(__func__);
    }

    ee::Logger mLogger;
};

class DerivedClass : public BaseClass {
public:
    DerivedClass() {
        mLogger.setName(__func__);
    }
};

int main() {
    DerivedClass a;
    std::cout << a.mLogger.getName() << std::endl;

    return EXIT_SUCCESS;
}