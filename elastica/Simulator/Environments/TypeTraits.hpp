#pragma once

namespace elastica {

  namespace environments {

    namespace tt {

      template <typename SimConfig, typename Tag>
      struct GetEnvironmentType {
        //        using type = decltype(typename Tag::template
        //        type<SimConfig>::instance());
        using type = typename Tag::template type<SimConfig>;
      };

    }  // namespace tt

  }  // namespace environments

  // TODO : ValidEnvironment (has resolve member)

}  // namespace elastica