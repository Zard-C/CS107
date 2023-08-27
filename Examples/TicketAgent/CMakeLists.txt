cmake_minimum_required(VERSION 3.15)
project(TicketAgent)

set(CMAKE_C_STANDARD 11)
add_executable(TicketAgent src/TicketAgent.c)
target_link_libraries(TicketAgent pthread)

add_executable(TicketAgentSemaphore src/TicketAgentSemaphore.c)
target_link_libraries(TicketAgentSemaphore pthread)