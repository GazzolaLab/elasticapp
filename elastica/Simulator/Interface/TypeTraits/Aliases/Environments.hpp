#pragma once

namespace elastica {

  namespace tt {

    //==========================================================================
    //
    //  ALIAS DECLARATIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Alias declaration for nested \c AdmissibleEnvironmentTags type
    // definitions.
    // \ingroup type_traits
    //
    // The admissible_environment_tags_t alias declaration provides a convenient
    // shortcut to access the nested \a AdmissibleEnvironmentTags type
    definition
    // of the given type \a T. The following code example shows both ways to
    // access the nested type definition:

       \code
       using Type1 = typename T::AdmissibleEnvironmentTags;
       using Type2 = admissible_environment_tags_t<T>;
       \endcode
    */
    template <typename T>
    using admissible_environment_tags_t = typename T::AdmissibleEnvironmentTags;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Alias declaration for nested \c AdmissibleImposedPrimitives type
    // definitions.
    // \ingroup type_traits
    //
    // The admissible_imposed_primitives_t alias declaration provides a
    // convenient shortcut to access the nested \a AdmissibleImposedPrimitives
    // type definition of the given type \a T. The following code example shows
    // both ways to access the nested type definition:

       \code
       using Type1 = typename T::AdmissibleImposedPrimitives;
       using Type2 = admissible_imposed_primitives_t<T>;
       \endcode
    */
    template <typename T>
    using admissible_imposed_primitives_t =
        typename T::AdmissibleImposedPrimitives;
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica
