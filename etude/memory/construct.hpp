//
//  construct:
//    オブジェクトを構築します。
// 
//    ::new(p) T(...) のスマートポインタ版です。
//    生ポインタを直接扱うよりは少しはマシになります。
//    
//  Copyright (C) 2010-11  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_MEMORY_INCLUDED_CONSTRUCT_HPP_
#define ETUDE_MEMORY_INCLUDED_CONSTRUCT_HPP_

#include <memory>
#include "pseudo_destructor_call.hpp"
#include "default_deallocate.hpp"
#include "../utility/holder.hpp"
#include "../unpacked_tuple.hpp"

#include "apply_in_place.hpp"

namespace etude {

  // void* 版 construct
  template<class T, class... Args, 
    class Result = std::unique_ptr< T, pseudo_destructor_call<T> >
  >
  inline Result construct( void* p, Args&&... args ) {
    return Result( ::new(p) T( std::forward<Args>(args)... ) );
  }
  
  // in_place_factory 版
  template<class T, class InPlace,
    class Result = std::unique_ptr< T, pseudo_destructor_call<T> >,
    class = typename std::enable_if<
      is_in_place_applyable<InPlace, T>::value
    >::type
  >
  inline Result construct( InPlace && x, void* p ) {
    return Result( apply_in_place<T>( std::forward<InPlace>(x), p ) );
  }
  // typed in place factory 版
  template<class TypedInPlace,
    class T = typename typed_in_place_associated<TypedInPlace>::type,
    class Result = std::unique_ptr< T, pseudo_destructor_call<T> >
  >
  inline Result construct( TypedInPlace && x, void* p ) {
    return Result( apply_typed_in_place( std::forward<TypedInPlace>(x), p ) );
  }
  
  // unpack 対応
  template<class T, class Tuple, std::size_t... Indices,
    class Result = std::unique_ptr< T, pseudo_destructor_call<T> >
  >
  inline Result construct( void* p, unpacked_tuple<Tuple, Indices...> t ) {
    (void)t;  // Indices... が空の場合の警告避け
    return Result( ::new(p) T( etude::move<Indices>(t)... ) );
  }
  
  
  // unique_ptr 版 construct
  
  // 破棄してからメモリを解放するファンクタ
  template<class T, class Dealloc>
  class dispose_and_deallocate_
    : private etude::holder<Dealloc>
  {
    typedef etude::holder<Dealloc> dealloc_;
    
   public:
    dispose_and_deallocate_() = default;
    dispose_and_deallocate_( dispose_and_deallocate_ const& ) = default;
    dispose_and_deallocate_( dispose_and_deallocate_ && )     = default;
    
    template<class D/*,
      class = typename std::enable_if<
        std::is_convertible<D, Dealloc>::value
      >::type
    */>
    explicit dispose_and_deallocate_( D && d )
      : dealloc_( std::forward<D>(d) ) {}
    
    Dealloc &      get_deallocator()       { return dealloc_::get(); }
    Dealloc const& get_deallocator() const { return dealloc_::get(); }
    
    void operator()( T* p ) {
      auto& dealloc = get_deallocator();
      
      p->~T();
      dealloc(p);
    }
    
    void operator()( T* p ) const {
      auto& dealloc = get_deallocator();
      
      p->~T();
      dealloc(p);
    }
    
  };
  // 特殊版
  // construct<T>( operator_new<T>(), ... ) の結果を std::unique_ptr<T> に入れられるよう
  template<class T>
  struct dispose_and_deallocate_< T, default_deallocate<T> >
    : std::default_delete<T>
  {
    dispose_and_deallocate_() = default;
    explicit dispose_and_deallocate_( default_deallocate<T> ) {}
    
    default_deallocate<T> get_deallocator() const { return default_deallocate<T>(); }
    
  };
  
  // 通常版
  template<class T, class D, class... Args,
    class Del = dispose_and_deallocate_<T, D>,
    class Result = std::unique_ptr<T, Del>
  >
  inline Result construct( std::unique_ptr<void, D> && p, Args&&... args ) {
    T* const pt = ::new( p.get() ) T( std::forward<Args>(args)... );
    p.release();
    
    return Result( pt,
      Del( std::forward<D>( p.get_deleter() ) )
    );
  }
  
  // in_place_factory 版
  template<class T, class D, class InPlace,
    class Del = dispose_and_deallocate_<T, D>,
    class Result = std::unique_ptr<T, Del>,
    class = typename std::enable_if<
      is_in_place_applyable<InPlace, T>::value
    >::type
  >
  inline Result construct( InPlace && x, std::unique_ptr<void, D> && p ) {
    T* const pt = apply_in_place<T>( std::forward<InPlace>(x), p.get() );
    p.release();
    
    return Result( pt,
      Del( std::forward<D>( p.get_deleter() ) )
    );
  }
  // typed in place factory 版
  template<class TypedInPlace, class D,
    class T = typename typed_in_place_associated<TypedInPlace>::type,
    class Del = dispose_and_deallocate_<T, D>,
    class Result = std::unique_ptr<T, Del>
  >
  inline Result construct( TypedInPlace && x, std::unique_ptr<void, D> && p ) {
    T* const pt = apply_typed_in_place( std::forward<TypedInPlace>(x), p.get() );
    p.release();
    
    return Result( pt,
      Del( std::forward<D>( p.get_deleter() ) )
    );
  }
  
  // unpack 対応
  template<class T, class D, class Tuple, std::size_t... Indices,
    class Result = std::unique_ptr< T, dispose_and_deallocate_<T, D> >
  >
  inline Result construct( 
    std::unique_ptr<void, D> && p, unpacked_tuple<Tuple, Indices...> t
  ){
    (void)t;  // Indices... が空の場合の警告避け
    return etude::construct<T>( std::move(p), etude::move<Indices>(t)... );
  }
  
}

#endif  // #ifndef ETUDE_MEMORY_INCLUDED_CONSTRUCT_HPP_
