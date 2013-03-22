#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <memory>

namespace bam {

    // make_unique by Herb Sutter http://herbsutter.com/gotw/_102/
    //! standard make_unique funciton
    template<typename T, typename ...Args>
    std::unique_ptr<T> make_unique( Args&& ...args )
    {
        return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
    }
}

#endif // UTILITY_HPP
