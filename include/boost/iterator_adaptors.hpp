//  (C) Copyright David Abrahams 2000. Permission to copy, use,
//  modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided
//  "as is" without express or implied warranty, and with no claim as
//  to its suitability for any purpose.

// (C) Copyright Jeremy Siek 2000. Permission to copy, use, modify,
// sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#ifndef BOOST_ITERATOR_ADAPTOR_DWA053000_HPP_
# define BOOST_ITERATOR_ADAPTOR_DWA053000_HPP_

// MSVC complains about the wrong stuff unless you disable this. We should add
// this to config.hpp

#include <boost/operators.hpp>
#include <iterator>

namespace boost {

// Just a "type envelope"; works around some MSVC deficiencies.
template <class T>
struct type {};

//=============================================================================
// Default policies for iterator adaptors. You can use this as a base
// class if you want to customize particular policies.
struct default_iterator_policies
{
    template <class Reference, class Iterator>
    static Reference dereference(type<Reference>, const Iterator& x)
        { return *x; }

    template <class Iterator>
    static void increment(Iterator& x)
        { ++x; }

    template <class Iterator>
    static void decrement(Iterator& x)
        { --x; }

    template <class Iterator, class DifferenceType>
    static void advance(Iterator& x, DifferenceType n)
        { x += n; }

    template <class Difference, class Iterator1, class Iterator2>
    static Difference distance(type<Difference>, Iterator1& x, Iterator2& y)
        { return y - x; }

    template <class Iterator1, class Iterator2>
    static bool equal(Iterator1& x, Iterator2& y)
        { return x == y; }

    template <class Iterator1, class Iterator2>
    static bool less(Iterator1& x, Iterator2& y)
        { return x < y; }
};

//=============================================================================// iterator_adaptor - A generalized adaptor around an existing iterator, which is itself an iterator
//
//      Iterator - the iterator type being wrapped.
//
//      Policies - a set of policies determining how the resulting iterator
//      works.
//
//      NonconstIterator - the corresponding non-const iterator type for
//      Iterator, if any. You don't need to supply this if you are not make a
//      const/non-const iterator pair.
//
//      Traits - a class satisfying the same requirements as a specialization of
//      std::iterator_traits for the resulting iterator.
//
template <class Iterator, class Policies, 
#ifndef BOOST_NO_STD_ITERATOR_TRAITS
          class Traits = std::iterator_traits<Iterator>,
#else
          class Traits,
#endif
          class NonconstIterator = Iterator
         >
struct iterator_adaptor
    : std::iterator<typename Traits::iterator_category, typename Traits::value_type, typename Traits::difference_type, typename Traits::pointer, typename Traits::reference>
{
    typedef iterator_adaptor<Iterator, Policies, Traits, NonconstIterator> Self;
public:
    typedef typename Traits::difference_type difference_type;
    typedef typename Traits::value_type value_type;
    typedef typename Traits::pointer pointer;
    typedef typename Traits::reference reference;
    typedef typename Traits::iterator_category iterator_category;

    iterator_adaptor(const Iterator& impl)
        : m_impl(impl) {}

    template <class OtherTraits>
    iterator_adaptor(const iterator_adaptor<NonconstIterator, Policies, OtherTraits, NonconstIterator>& rhs)
        : m_impl(rhs.m_impl) {}

    template <class OtherTraits>
    Self& operator=(const iterator_adaptor<NonconstIterator, Policies, OtherTraits, NonconstIterator>& rhs)
        { m_impl = rhs.m_impl; return *this; }
    
    reference operator*() const {
        return Policies::dereference(type<reference>(), m_impl);
    }

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning( disable : 4284 )
#endif

    pointer operator->() const
        { return &*this; }

#ifdef _MSC_VER
# pragma warning(pop)
#endif

    reference operator[](difference_type n)
        { return *(*this + n); }
    
    Self& operator++() {
        Policies::increment(m_impl);
        return *this;
    }

    Self& operator++(int) { Self tmp(*this); ++*this; return tmp; }
    
    Self& operator--() {
        Policies::decrement(m_impl);
        return *this;
    }
    
    Self& operator--(int) { Self tmp(*this); --*this; return tmp; }

    Self& operator+=(difference_type n) {
        Policies::advance(m_impl, n);
        return *this;
    }
  
    Self& operator-=(difference_type n) {
        Policies::advance(m_impl, -n);
        return *this;
    }

private:
    typedef Policies policies_type;
public: // too many compilers have trouble when this is private.
    Iterator m_impl;
};

template <class Iterator, class Policies, class Traits, class NonconstIterator>
iterator_adaptor<Iterator,Policies,Traits,NonconstIterator>
operator-(iterator_adaptor<Iterator,Policies,Traits,NonconstIterator> p, const typename Traits::difference_type x)
{
    return p -= x;
}

template <class Iterator, class Policies, class Traits, class NonconstIterator>
iterator_adaptor<Iterator,Policies,Traits,NonconstIterator>
operator+(iterator_adaptor<Iterator,Policies,Traits,NonconstIterator> p, const typename Traits::difference_type x)
{
    return p += x;
}

template <class Iterator, class Policies, class Traits, class NonconstIterator>
iterator_adaptor<Iterator,Policies,Traits,NonconstIterator>
operator+(const typename Traits::difference_type x, iterator_adaptor<Iterator,Policies,Traits,NonconstIterator> p)
{
    return p += x;
}

template <class Iterator1, class Iterator2, class Policies, class Traits1, class Traits2, class NonconstIterator>
typename Traits1::difference_type operator-(
    const iterator_adaptor<Iterator1,Policies,Traits1,NonconstIterator>& x,
    const iterator_adaptor<Iterator2,Policies,Traits2,NonconstIterator>& y )
{
    typedef typename Traits1::difference_type difference_type;
    return Policies::distance(type<difference_type>(), y.m_impl, x.m_impl);
}

template <class Iterator1, class Iterator2, class Policies, class Traits1, class Traits2, class NonconstIterator>
inline bool 
operator==(const iterator_adaptor<Iterator1,Policies,Traits1,NonconstIterator>& x, const iterator_adaptor<Iterator2,Policies,Traits2,NonconstIterator>& y) {
    return Policies::equal(x.m_impl, y.m_impl);
}

template <class Iterator1, class Iterator2, class Policies, class Traits1, class Traits2, class NonconstIterator>
inline bool 
operator<(const iterator_adaptor<Iterator1,Policies,Traits1,NonconstIterator>& x, const iterator_adaptor<Iterator2,Policies,Traits2,NonconstIterator>& y) {
    return Policies::less(x.m_impl, y.m_impl);
}

template <class Iterator1, class Iterator2, class Policies, class Traits1, class Traits2, class NonconstIterator>
inline bool 
operator>(const iterator_adaptor<Iterator1,Policies,Traits1,NonconstIterator>& x,
          const iterator_adaptor<Iterator2,Policies,Traits2,NonconstIterator>& y) { 
    return Policies::less(y.m_impl, x.m_impl);
}

template <class Iterator1, class Iterator2, class Policies, class Traits1, class Traits2, class NonconstIterator>
inline bool 
operator>=(const iterator_adaptor<Iterator1,Policies,Traits1,NonconstIterator>& x, const iterator_adaptor<Iterator2,Policies,Traits2,NonconstIterator>& y) {
    return !Policies::less(x.m_impl, y.m_impl);
}

template <class Iterator1, class Iterator2, class Policies, class Traits1, class Traits2, class NonconstIterator>
inline bool 
operator<=(const iterator_adaptor<Iterator1,Policies,Traits1,NonconstIterator>& x,
           const iterator_adaptor<Iterator2,Policies,Traits2,NonconstIterator>& y) {
    return !Policies::less(y.m_impl, x.m_impl);
}

template <class Iterator1, class Iterator2, class Policies, class Traits1, class Traits2, class NonconstIterator>
inline bool 
operator!=(const iterator_adaptor<Iterator1,Policies,Traits1,NonconstIterator>& x, 
           const iterator_adaptor<Iterator2,Policies,Traits2,NonconstIterator>& y) {
    return !Policies::equal(x.m_impl, y.m_impl);
}

//=============================================================================
// iterator_adaptors - A type generator that simplifies creating
//   mutable/const pairs of iterator adaptors.

template <class Iterator, class ConstIterator, 
#ifdef BOOST_NO_STD_ITERATOR_TRAITS
          class Traits, 
          class ConstTraits, 
#else
          class Traits = std::iterator_traits<Iterator>, 
          class ConstTraits = std::iterator_traits<ConstIterator>, 
#endif
          class Policies = default_iterator_policies>
class iterator_adaptors
{
public:
    typedef iterator_adaptor<Iterator, Policies, Iterator, Traits> iterator;
    typedef iterator_adaptor<ConstIterator, Policies, Iterator, ConstTraits> 
      const_iterator;
};


} // namespace boost

#endif