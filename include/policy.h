#ifndef POLICY_H
#define POLICY_H

auto fifo(std::string) -> int;
auto lfu(std::string) -> int;
auto lru_stack(std::string) -> int;
auto lru_clock(std::string) -> int;
auto lru_ref8(std::string) -> int;
auto do_optional(std::string) -> int;

#endif
