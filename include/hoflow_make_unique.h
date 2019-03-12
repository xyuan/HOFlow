/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef HOFLOW_MAKE_UNIQUE_H
#define HOFLOW_MAKE_UNIQUE_H

#include <memory>

// replace with std::make_unique when we move to C++14
template<typename T, typename... Args>std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif /* HOFLOW_MAKE_UNIQUE_H */

