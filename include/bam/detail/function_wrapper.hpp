// PIMPL/boost::any style function wrapper by Anthony Williams from C++ Concurrency in Action

#ifndef FUNCTION_WRAPPER_HPP
#define FUNCTION_WRAPPER_HPP

#include <memory>
#include <utility>

namespace bam { namespace detail {

    class function_wrapper
    {
        struct impl_base {
            virtual void call()=0;
            virtual ~impl_base() {}
        };

        std::unique_ptr<impl_base> impl;
        template<typename F>
        struct impl_type: impl_base {
            F f;
            impl_type(F&& f_): f(std::move(f_)) {}
            void call() { f(); }
        };
    public:
        template<typename F>
        function_wrapper(F&& f): impl(new impl_type<F>(std::move(f))) {}
        function_wrapper() = default;
        function_wrapper(function_wrapper&& other): impl(std::move(other.impl)) {}

        void operator()() { impl->call(); }
        function_wrapper& operator=(function_wrapper&& other) {
            impl=std::move(other.impl);
            return *this;
        }

        function_wrapper(const function_wrapper&)=delete;
        function_wrapper(function_wrapper&)=delete;
        function_wrapper& operator=(const function_wrapper&)=delete;
    };


}}

#endif // FUNCTION_WRAPPER_HPP
