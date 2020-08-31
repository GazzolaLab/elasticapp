#pragma once

namespace elastica {

  /// Models the block concept
  /// \tparam Plugin  The computational plugin type
  template <class Plugin>
  class Block;

  //  template <template <typename, typename...> class TT, typename SENDER,
  //            typename... ARGS>
  //  class CRTPBase<TT<SENDER, ARGS...>> {
  //    // ...
  //  };

  //  template <template <typename...> class Plugin>
  //  class Block;

  // Specialization for a Cosserat Rod plugin
//  template <template <typename /*Traits*/, typename /* Block */,
//                      template <typename /*Traits*/, typename /* Block */>
//                      class... /* Skills*/>
//            class Plugin>
//  class Block;

}  // namespace elastica