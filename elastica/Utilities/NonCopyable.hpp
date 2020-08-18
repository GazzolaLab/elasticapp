#pragma once

namespace elastica {

  //*************************************************************************************************
  /*!\brief Base class for non-copyable class instances.
   * \ingroup util
   *
   * The NonCopyable class is intended to work as a base class for non-copyable
   classes. Both the
   * copy constructor and the copy assignment operator are declared private and
   left undefined in
   * order to prohibit copy operations of the derived classes.\n
   *
   * \b Note: It is not necessary to publicly derive from this class. It is
   sufficient to derive
   * privately to prevent copy operations on the derived class.

     \code
     class A : private NonCopyable
     { ... };
     \endcode

     https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Non-copyable_Mixin
   */
  class NonCopyable {
   public:
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
    //    NonCopyable(NonCopyable&&) = delete;
    //    NonCopyable& operator=(const NonCopyable&&) = delete;

   protected:
    inline NonCopyable() = default;
    virtual inline ~NonCopyable() = default;
    NonCopyable(NonCopyable&&) = default;
    NonCopyable& operator=(NonCopyable&&) = default;
  };

}  // namespace elastica
