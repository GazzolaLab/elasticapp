# Checklist for best single-core performance

## Common
- [ ] Compile on Release/MinSizeRel
- [ ] Pass `-DDEBUG_SYMBOLS=OFF` to CMake
- [ ] Choose `SIMD` execution in `Configuration/Kernels.hpp` (or override in client)
- [ ] Apply dtenstransp.patch to `BlazeTensor` (`elastica_quick_setup` already patches this in for you)
- [ ] Install `Sleef` for vectorized trigonometric operations (pass `-DUSE_SLEEF=ON` to CMake)
- [ ] Use `unchecked` for `ELASTICA_DEFAULT_SYSTEM_INDEX_CHECK` in `Configuration/Systems.hpp` (or override in client)

## Problem-dependent
- [ ] Do not resort to `AlwaysNewBlockingPolicy`
- [ ] Use higher block-sizes (`> 256UL`) in `RestrictSizeAcrossBlockPolicy`
