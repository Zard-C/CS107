cmake_minimum_required(VERSION 3.15)
project(TicketAgent)

add_executable(TicketAgent src/TicketAgent.c)
target_link_libraries(TicketAgent pthread)