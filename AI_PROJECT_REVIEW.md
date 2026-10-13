# AI Project Review

## Project Summary

A handwritten C simulation of preemptive shortest-remaining-time scheduling across one or multiple CPUs, including explicit parallel-process decomposition and an alternative allocation strategy.

Chen Jiang’s COMP30023 Computer Systems coursework, with an original source context dated September 2021. The CPU queues and per-unit-time scheduling model are preserved.

## Original Engineering Strengths

The implementation separates process records, CPU initialization, queue ownership, allocation, and the scheduling loop into small C modules. These boundaries make the simulation mechanics explicit and support focused discussion of systems concepts.

Parallel jobs are represented through parent/child records and completion accounting, while a distinct ownership chain frees allocations independently of the CPU queue links. The design makes shared logical completion and physical execution state directly inspectable.

## Technical Highlights

The original scheduler advances active execution one time unit at a time. Ready work is allocated to CPUs by remaining workload and selected by remaining execution time, with deterministic identifier tie breaks.

Parallel parents retain logical completion state while children carry execution counters; decimal-looking subprocess identifiers are now rendered from exact parent/subprocess integers. Idle periods can advance to the next arrival without changing active execution semantics.

## 2026 Maintenance Improvements

- Validated CLI flags and ordered unique process records; rejected malformed, empty, zero-duration, and overflow inputs.
- Corrected idle-gap termination, safe candidate selection, queued-job accounting, serialized fallback state, and child ownership registration.
- Separated parent/subprocess IDs to preserve exact large IDs and suffixes beyond 9.
- Added deterministic fixtures and an independently implemented scheduling oracle; retained both original strategies.

The changes retain the project's original mechanisms and strengthen their execution
boundaries and verification contract. They do not substitute a new project concept.

## Verification

- Apple Clang 21: normal -O2 build and regression suite passed.
- AddressSanitizer + UndefinedBehaviorSanitizer regression suite passed on macOS.
- GCC 14.3 in a network-disabled Linux container: strict -Werror sanitizer checks passed, including LeakSanitizer defaults.
- Critical make test checks are rerun on the merged default branch before pushing it.
- make test: seven methods passed, including 30 independent SRTF-oracle cases, 100-job cases, both strategies, 1–1,024 processors, idle gaps, simultaneous completion, and repeated execution.
- Strict Clang -O0 -Werror build and regression suite passed.
- Evidence: scheduler_before_one.log, scheduler_before_idle.log, scheduler_after_tests.log, scheduler_O0_tests.log, scheduler_linux_tests.log.

Verification is limited to the commands and cases listed above; passing these checks is not a claim of universal correctness.

## Skills Demonstrated

Systems simulation, preemption, parent/child state, exact identifiers, queue accounting, independent test oracles, and sanitizer verification.

## Engineering Assessment

This project provides a strong systems discussion because execution, allocation, and logical completion are modeled explicitly. The maintenance strengthens the simulation contract and adds an independent correctness check without replacing the handwritten scheduler.

## Maintenance Transparency

The original implementation is preserved in `archive/handwritten-original` at
`65183a9e5ea5de2ce38151157d3b473da0c133d2`. The reviewed maintained version contains a later AI-assisted engineering
maintenance pass focused on verification, robustness, testing, and documentation
while retaining the original project approach.
