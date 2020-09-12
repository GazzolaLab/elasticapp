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
    // \ingroup aliases
    //
    // The admissible_environment_tags_t alias declaration provides a convenient
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

  }  // namespace tt

}  // namespace elastica
