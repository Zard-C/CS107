cmake_minimum_required(VERSION 3.15)
project(CS107Examples LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CS107_EXAMPLE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/examples")

find_package(Threads REQUIRED)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CS107_C_WARNINGS -Wall -Wextra)
endif()

function(add_cs107_c_executable target source output_subdir)
    add_executable(${target} ${source})
    target_compile_definitions(${target} PRIVATE _GNU_SOURCE)
    target_compile_options(${target} PRIVATE ${CS107_C_WARNINGS})
    target_compile_features(${target} PRIVATE c_std_11)

    set(output_dir "${CS107_EXAMPLE_OUTPUT_DIR}")
    if(output_subdir)
        set(output_dir "${CS107_EXAMPLE_OUTPUT_DIR}/${output_subdir}")
    endif()

    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${output_dir}"
    )
endfunction()

function(configure_cs107_c_target target)
    target_compile_definitions(${target} PRIVATE _GNU_SOURCE)
    target_compile_options(${target} PRIVATE ${CS107_C_WARNINGS})
    target_compile_features(${target} PRIVATE c_std_11)
endfunction()

add_cs107_c_executable(channeling Examples/Channeling/channeling.c "")
add_cs107_c_executable(dinning Examples/DinningPhilosohper/dinning.c "")
target_link_libraries(dinning PRIVATE Threads::Threads)

add_cs107_c_executable(ice_cream_store_simulation Examples/IceCreamStore/ice_cream_store_simulation.c "")
target_link_libraries(ice_cream_store_simulation PRIVATE Threads::Threads)

add_cs107_c_executable(generic_lsearch Examples/GenericPointer/generic_lsearch.c "generic-pointer")
add_cs107_c_executable(generic_swap Examples/GenericPointer/generic_swap.c "generic-pointer")
add_cs107_c_executable(lsearch_int Examples/GenericPointer/lsearch_int.c "generic-pointer")
add_cs107_c_executable(swapint Examples/GenericPointer/swapint.c "generic-pointer")

add_library(generic_bsearch OBJECT Examples/GenericPointer/generic_bsearch.c)
configure_cs107_c_target(generic_bsearch)

add_subdirectory(Examples/TicketAgent)

add_custom_target(examples
    DEPENDS
        channeling
        dinning
        ice_cream_store_simulation
        generic_lsearch
        generic_swap
        lsearch_int
        swapint
        generic_bsearch
        TicketAgent
        TicketAgentSemaphore
)
