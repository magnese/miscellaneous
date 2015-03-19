#ifndef MATRIX_HH
#define MATRIX_HH

#include <vector>

template<typename T>
class Matrix
{
public:
    explicit Matrix(const std::size_t size):
        m_(size,std::vector<T>(size,0))
    {}

    inline T& operator()(const std::size_t& i,const std::size_t j)
    {
        return m_[i][j];
    }

    inline const std::size_t size() const
    {
        return m_.size();
    }

private:
    std::vector<std::vector<T>> m_;
};
#endif // MATRIX_HH
