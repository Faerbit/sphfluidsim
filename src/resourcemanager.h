#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <map>
#include <memory>
#include <iostream>

template<typename T, class Derived>
class ResourceManager {

public:
    static std::shared_ptr<T> load(std::string filePath) {
        auto search = resources.find(filePath);
        if (search != resources.end()) {
            auto ptr = search->second.lock();
            if (ptr) {
                return ptr;
            }
        }
        auto ptr = ResourceManager::loadResource(filePath);
        resources[filePath] = ptr;
        return ptr;
    }

private:
    static std::map<std::string, std::weak_ptr<T>> resources;
    static std::shared_ptr<T> loadResource(std::string filePath) {
        std::cout << "Loading " << filePath << std::endl;
        return Derived::loadResource(filePath);
    }
};

template<typename T, class Derived>
std::map<std::string, std::weak_ptr<T>>  ResourceManager<T, Derived>::resources;

#endif // RESOURCEMANAGER_H
