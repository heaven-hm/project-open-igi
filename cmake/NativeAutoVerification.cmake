# Explicit acceptance targets. Do not make the candidate depend on these: the
# verifier must compile an unaccepted candidate before it can instrument it.
find_package(Python3 COMPONENTS Interpreter REQUIRED)
set(IGI_AUTOVERIFY_LEVEL "1" CACHE STRING "Retail level for paired native verification")
foreach(native_scope changed all)
    add_custom_target(verify_natives_${native_scope}
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/scripts/native_autoverify.py"
                run --scope ${native_scope} --level "${IGI_AUTOVERIFY_LEVEL}" --retry
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        USES_TERMINAL
        COMMENT "Strict ${native_scope} native static and paired runtime verification")
endforeach()
