# include <cstring>
# include <iostream>
int main()
{
    char buffer[90000];
    memset(&buffer, 0, 90000);

    for(int i = 0; i < sizeof(buffer); ++i)
       buffer[i] = i;
    for(int i = 0; sizeof(buffer); ++i)
        std::cout << i << ": " << buffer[i] << std::endl;

    return (1);

}