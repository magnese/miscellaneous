#include <iostream>
#include <vector>

int main()
{
    // donut parameters
    const double R(10.0);
    const double r(4.0);

    // crate donut
    const int dim(2*R+1);
    std::vector<std::vector<char>> mat(dim,std::vector<char>(dim,' '));
    for(int i=0;i!=dim;++i)
        for(int j=0;j!=dim;++j)
        {
            const double x(static_cast<double>(i-R));
            const double y(static_cast<double>(j-R));
            if((x*x+y*y)>r*r && (x*x+y*y)<R*R)
                mat[i][j]='D';
        }

    // output donut
    for(const auto& row: mat)
    {
        for(const auto& elem:row)
            std::cout<<elem<<" ";
        std::cout<<std::endl;
    }
    return 0;
}
