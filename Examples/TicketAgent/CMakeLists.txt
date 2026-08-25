cmake_minimum_required(VERSION 3.15)
project(TicketAgent LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
find_package(Threads REQUIRED)

if(NOT DEFINED CS107_C_WARNINGS AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CS107_C_WARNINGS -Wall -Wextra)
endif()

if(DEFINED CS107_EXAMPLE_OUTPUT_DIR)
    set(TICKET_AGENT_OUTPUT_DIR "${CS107_EXAMPLE_OUTPUT_DIR}/ticket-agent")
else()
    set(TICKET_AGENT_OUTPUT_DIR "${CMAKE_BINARY_DIR}")
endif()

function(configure_ticket_agent_target target)
    target_compile_definitions(${target} PRIVATE _GNU_SOURCE)
    target_compile_options(${target} PRIVATE ${CS107_C_WARNINGS})
    target_compile_features(${target} PRIVATE c_std_11)
    target_link_libraries(${target} PRIVATE Threads::Threads)
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${TICKET_AGENT_OUTPUT_DIR}"
    )
endfunction()

add_executable(TicketAgent src/ticketAgent.c)
configure_ticket_agent_target(TicketAgent)

add_executable(TicketAgentSemaphore src/ticketAgentSemaphore.c)
configure_ticket_agent_target(TicketAgentSemaphore)
