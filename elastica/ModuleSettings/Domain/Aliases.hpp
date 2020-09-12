#pragma once

namespace elastica {

  namespace tt {

    //==========================================================================
    //
    //  ALIAS DECLARATIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Alias declaration for nested \c DomainType type definitions.
    // \ingroup aliases
    //
    // The domain_type_t alias declaration provides a convenient shortcut to
    // access the nested \a DomainType type definition of the given type \a T.
    // The following code example shows both ways to access the nested type
    // definition:

       \code
       using Type1 = typename T::DomainType;
       using Type2 = domain_type_t<T>;
       \endcode
    */
    template <typename T>
    using domain_type_t = typename T::DomainType;
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica
