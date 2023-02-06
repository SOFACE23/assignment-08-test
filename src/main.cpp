#include <string>
#include <iostream>

int main()
{
    std::string s = "Jane,127.0.0.1:5000";
    std::string delimiter = ",";
    auto split = s.find(delimiter);
    std::string name = s.substr(0, split);
    std::string endpoint = s.substr(split+1);

    std::cout << "name: " << name << ", "<<  "endpoint: " << endpoint << std::endl;
}