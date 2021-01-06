# Distributed under the MIT License. See LICENSE.txt for details.

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        ArraySize.hpp
        Cpp17.hpp
        Cpp20.hpp
        InvokeResult.hpp
        IsA.hpp
        IsCallable.hpp
        IsDefined.hpp
        IsDetected.hpp
        IsInvokable.hpp
        IsIterable.hpp
        IsMaplike.hpp
        IsStdArray.hpp
        IsStdArrayOfSize.hpp
        IsStreamable.hpp
        Void.hpp
)
