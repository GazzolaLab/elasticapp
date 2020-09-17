#pragma once

namespace elastica {

  namespace tt {

    //==========================================================================
    //
    //  ALIAS DECLARATIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Alias declaration for nested \c AdmissibleSystemTags type
    // definitions.
    // \ingroup type_traits
    //
    // The admissible_system_tags_t alias declaration provides a convenient
    // shortcut to access the nested \a AdmissibleSystemTags type definition
    // of the given type \a T. The following code example shows both ways to
    // access the nested type definition:

       \code
       using Type1 = typename T::AdmissibleSystemTags;
       using Type2 = admissible_system_tags_t<T>;
       \endcode
    */
    template <typename T>
    using admissible_system_tags_t = typename T::AdmissibleSystemTags;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Alias declaration for nested \c PrimitiveType type
    // definitions.
    // \ingroup type_traits
    //
    // The primitive_type_t alias declaration provides a convenient
    // shortcut to access the nested \a PrimitiveType type definition
    // of the given type \a T. The following code example shows both ways to
    // access the nested type definition:

       \code
       using Type1 = typename T::PrimitiveType;
       using Type2 = primitive_type_t<T>;
       \endcode
    */
    template <typename SystemTag>
    using primitive_type_t = typename SystemTag::PrimitiveType;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Alias declaration for nested \c SystemType type definitions.
    // \ingroup type_traits
    //
    // The system_type_t alias declaration provides a convenient
    // shortcut to access the nested \a SystemType type definition
    // of the given type \a T. The following code example shows both ways to
    // access the nested type definition:

       \code
       using Type1 = typename T::SystemType;
       using Type2 = system_type_t<T>;
       \endcode
    */
    template <typename SystemTag>
    using system_type_t = typename SystemTag::SystemType;
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica
